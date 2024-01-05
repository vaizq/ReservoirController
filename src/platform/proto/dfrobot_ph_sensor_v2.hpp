#ifndef PH_SENSOR_HPP
#define PH_SENSOR_HPP

#include <stdint.h>
#include <functional>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"


namespace Proto
{

class PhSensor
{
public:
    PhSensor(int pin);
    ~PhSensor();
    PhSensor(const PhSensor&) = delete;
    PhSensor& operator=(const PhSensor&) = delete;
    float readPH();
    bool calibrate();
private:
    float readVoltage();

    adc_channel_t mChannel;
    adc_oneshot_unit_handle_t mHandle;
    adc_cali_handle_t mCaliHandle;
    float mVoltageAtPh7 = 1500.0f;
    float mVoltageAtPh4 = 2032.44f;
};

}

#endif