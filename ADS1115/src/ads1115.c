#include "ads1115.h"

esp_err_t configure_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config)
{
    esp_err_t ret;
    uint8_t read_buffer[2] = {0}, register_addr = CONFIGURATION_REG;
    ret = i2c_master_transmit_receive(*ads1115_handle, &register_addr, sizeof(register_addr), read_buffer, sizeof(read_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
        return ret;

    ESP_LOGV("ADS", "Config register read successful. Data: %x", (read_buffer[0] << 8) | read_buffer[1]);

    uint8_t write_buffer[3] = {
        CONFIGURATION_REG,
        (START_CONVERSION | ads1115_config->input_mux | ads1115_config->gain | ads1115_config->conv_mode),
        (ads1115_config->data_rate | ads1115_config->comp_mode | ads1115_config->comp_polarity | ads1115_config->comp_latch | ads1115_config->comp_queue)};

    ret = i2c_master_transmit(*ads1115_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", CONFIGURATION_REG, esp_err_to_name(ret));
        return ret;
    }

    switch (ads1115_config->gain)
    {
    case FSR_0_256V:
        ads1115_config->conversion_factor = 0.0000078125f;
        break;

    case FSR_0_512V:
        ads1115_config->conversion_factor = 0.000015625f;
        break;

    case FSR_1_024V:
        ads1115_config->conversion_factor = 0.00003125f;
        break;

    case FSR_2_048V:
        ads1115_config->conversion_factor = 0.0000625f;
        break;

    case FSR_4_096V:
        ads1115_config->conversion_factor = 0.000125f;
        break;

    case FSR_6_144V:
        ads1115_config->conversion_factor = 0.0001875f;
        break;

    default:
        ads1115_config->conversion_factor = 0.0000625f;
        break;
    }

    ESP_LOGD("ADS", "ADS1115 configured successfully! Conversion initialized");
    return ESP_OK;
}

esp_err_t read_continuous_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config, float_t *data)
{
    esp_err_t ret;
    uint8_t read_buffer[2] = {0}, write_buffer = CONVERSION_REG;

    ret = i2c_master_transmit_receive(*ads1115_handle, &write_buffer, sizeof(write_buffer), read_buffer, sizeof(read_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
        return ret;

    switch (ads1115_config->data_type)
    {
    case DATA_ADC_RAW:
        *data = read_buffer[0] << 8 | read_buffer[1];
        ESP_LOGV("ADS", "ADS1115 Continuous ADC Raw Data: %f", (*data));
        return ESP_OK;

    case DATA_VOLTS:
        *data = (read_buffer[0] << 8 | read_buffer[1]) * ads1115_config->conversion_factor;
        ESP_LOGV("ADS", "ADS1115 One-shot ADC Voltage: %f V", *data);
        return ESP_OK;

    default:
        return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t read_single_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config, float_t *data)
{
    esp_err_t ret;
    uint8_t write_buffer[3] = {
        CONFIGURATION_REG,
        (START_CONVERSION | ads1115_config->input_mux | ads1115_config->gain | SINGLE_SHOT_MODE),
        (ads1115_config->data_rate | ads1115_config->comp_mode | ads1115_config->comp_polarity | ads1115_config->comp_latch | ads1115_config->comp_queue)};

    ret = i2c_master_transmit(*ads1115_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", CONFIGURATION_REG, esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    switch (ads1115_config->gain)
    {
    case FSR_0_256V:
        ads1115_config->conversion_factor = 0.0000078125f;
        break;

    case FSR_0_512V:
        ads1115_config->conversion_factor = 0.000015625f;
        break;

    case FSR_1_024V:
        ads1115_config->conversion_factor = 0.00003125f;
        break;

    case FSR_2_048V:
        ads1115_config->conversion_factor = 0.0000625f;
        break;

    case FSR_4_096V:
        ads1115_config->conversion_factor = 0.000125f;
        break;

    case FSR_6_144V:
        ads1115_config->conversion_factor = 0.0001875f;
        break;

    default:
        ads1115_config->conversion_factor = 0.0000625f;
        break;
    }

    uint8_t read_buffer[2] = {0}, register_addr = CONVERSION_REG;
    ret = i2c_master_transmit_receive(*ads1115_handle, &register_addr, sizeof(register_addr), read_buffer, sizeof(read_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
        return ret;

    switch (ads1115_config->data_type)
    {
    case DATA_ADC_RAW:
        *data = read_buffer[0] << 8 | read_buffer[1];
        ESP_LOGV("ADS", "ADS1115 One-shot ADC Raw Data: %f", (*data));
        return ESP_OK;

    case DATA_VOLTS:
        *data = (read_buffer[0] << 8 | read_buffer[1]) * ads1115_config->conversion_factor;
        ESP_LOGV("ADS", "ADS1115 One-shot ADC Voltage: %f V", *data);
        return ESP_OK;

    default:
        return ESP_ERR_INVALID_ARG;
    }
}