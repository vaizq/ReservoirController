#include "ec_sensor.hpp"
#include "esp_err.h"
#include <cstring>


Driver::ECSensor::ECSensor(int pin)
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

float Driver::ECSensor::readEC()
{
    return readVoltage() / 3300.0f * 2.0f;
}

float Driver::ECSensor::readVoltage()
{
    int raw;
    int voltage;
    adc_oneshot_read(mHandle, mChannel, &raw);
    adc_cali_raw_to_voltage(mCaliHandle, raw, &voltage);
    return static_cast<float>(voltage);
}
