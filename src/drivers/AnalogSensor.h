//
// Created by vaige on 5.2.2024.
//

#ifndef RESERVOIRCONTROLLER_ANALOGSENSOR_H
#define RESERVOIRCONTROLLER_ANALOGSENSOR_H

#include <utility>
#include <stdint.h>
#include "read_voltage.hpp"
#include "../core/common.hpp"

namespace Driver
{
    // Analog sensor with linear output using two point calibration
    class AnalogSensor
    {
    public:
        using TwoPointCalibration = std::pair<std::pair<float, float>, std::pair<float, float>>;

        explicit AnalogSensor(uint8_t pin, TwoPointCalibration defaultCalibration = {{0.0f, 0.0f},
                                                                            {1.0f, 0.0f}}, int baseAddress = 0);
        [[nodiscard]] float read() const;

        bool calibrate(float value, int numSamples = 1);

        void update(Duration dt);

    private:
        [[nodiscard]] bool isCalibrated() const;

        void validateCalibration();

        void storeCalibration();

        void loadCalibration();

        VoltageReader mVoltageReader;
        TwoPointCalibration mCalibration{};
        float mPH{7.0f};
        int mBaseAddress{0};
    };
}

#endif //RESERVOIRCONTROLLER_ANALOGSENSOR_H
