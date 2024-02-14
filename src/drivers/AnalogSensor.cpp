//
// Created by vaige on 5.2.2024.
//

#include "AnalogSensor.h"
#include "EEPROM.h"


constexpr int addressForCalibration{0};
constexpr int addressForMagicNumber{128};
constexpr int magicNumber{6942066};


Driver::AnalogSensor::AnalogSensor(uint8_t pin, TwoPointCalibration defaultCalibration, int baseAddress)
:
    mVoltageReader(pin, 10),
    mCalibration(std::move(defaultCalibration)),
    mBaseAddress(baseAddress)
{
    if (isCalibrated())
    {
        loadCalibration();
    }
}

float Driver::AnalogSensor::read() const
{
    return mPH;
}

void Driver::AnalogSensor::update(Duration dt)
{
    const float x = (mVoltageReader.voltage() - mCalibration.first.first);
    const float dx = mCalibration.second.first - mCalibration.first.first;
    const float dy = mCalibration.second.second - mCalibration.first.second;
    const float k = dy / dx;
    const float c = mCalibration.first.second;
    mPH = k * x + c;
}

bool Driver::AnalogSensor::calibrate(float value, int numSamples)
{
    const float voltage = [this, numSamples]() {
        float total{0.0f};
        for (int i = 0; i < numSamples; ++i)
        {
            total += mVoltageReader.voltage();
        }
        return total / static_cast<float>(numSamples);
    }();

    if (std::abs(voltage - mCalibration.first.first) < std::abs(voltage - mCalibration.second.first))
    {
        mCalibration.first.first = voltage;
        mCalibration.first.second = value;
    }
    else
    {
        mCalibration.second.first = voltage;
        mCalibration.second.second = value;
    }

    validateCalibration();
    storeCalibration();

    return true;
}


void Driver::AnalogSensor::validateCalibration()
{
    if (mCalibration.first > mCalibration.second)
    {
        std::swap(mCalibration.first, mCalibration.second);
    }
    else if (mCalibration.first.first == mCalibration.second.first)
    {
        mCalibration.second.first += 0.1f;
    }
}

void Driver::AnalogSensor::storeCalibration()
{
    if (!isCalibrated()) {
        EEPROM.writeInt(mBaseAddress + addressForMagicNumber, magicNumber);
        EEPROM.commit();
    }
    EEPROM.put(mBaseAddress + addressForCalibration, mCalibration);
    EEPROM.commit();
}

void Driver::AnalogSensor::loadCalibration()
{
    EEPROM.get(mBaseAddress + addressForCalibration, mCalibration);
}

bool Driver::AnalogSensor::isCalibrated() const
{
    return EEPROM.readInt(mBaseAddress + addressForMagicNumber) == magicNumber;
}


