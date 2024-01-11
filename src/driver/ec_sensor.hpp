#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

namespace Driver 
{

class ECSensor
{
public:
    ECSensor(int pin);
    float readEC();
private:
    float readVoltage();

    adc_channel_t mChannel;
    adc_oneshot_unit_handle_t mHandle;
    adc_cali_handle_t mCaliHandle;

};

}