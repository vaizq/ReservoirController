#include "dfrobot_ph_sensor_v2.hpp"
#include "esp_err.h"
#include <cstring>


Driver::PhSensor::PhSensor(int pin)
{
    // Find channel
    adc_unit_t unit;
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pin, &unit, &mChannel));

    // ADC init configuration
    adc_oneshot_unit_init_cfg_t initCfg;
    std::memset(&initCfg, 0, sizeof initCfg);
    initCfg.unit_id = unit;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&initCfg, &mHandle));

    // ADC channel configuration 
    adc_oneshot_chan_cfg_t channelCfg;
    std::memset(&channelCfg, 0, sizeof channelCfg);
    channelCfg.bitwidth = ADC_BITWIDTH_DEFAULT;
    channelCfg.atten = ADC_ATTEN_DB_11;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(mHandle, mChannel, &channelCfg));

    // ADC calibration configuration
    adc_cali_line_fitting_config_t caliCfg;
    std::memset(&caliCfg, 0, sizeof caliCfg);
    caliCfg.bitwidth = ADC_BITWIDTH_DEFAULT;
    caliCfg.atten = ADC_ATTEN_DB_11;

    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&caliCfg, &mCaliHandle));
}

Driver::PhSensor::~PhSensor()
{
    adc_cali_delete_scheme_line_fitting(mCaliHandle);
    adc_oneshot_del_unit(mHandle);
}

float Driver::PhSensor::readPH()
{
    const float voltage = readVoltage();
    const float k = (7.0f - 4.0f) / (mVoltageAtPh7 - mVoltageAtPh4);
    const float b = 7.0f - k * mVoltageAtPh7;
    return k * voltage + b; // y = kx + b
}

bool Driver::PhSensor::calibrate()
{
    const float voltage = readVoltage();
    if (std::abs(voltage - mVoltageAtPh7) < std::abs(voltage - mVoltageAtPh4))
    {
        mVoltageAtPh7 = voltage;
    }
    else
    {
        mVoltageAtPh4 = voltage;
    }

    return true;
}

float Driver::PhSensor::readVoltage()
{
    int raw;
    int voltage;
    adc_oneshot_read(mHandle, mChannel, &raw);
    adc_cali_raw_to_voltage(mCaliHandle, raw, &voltage);
    return static_cast<float>(voltage);
}