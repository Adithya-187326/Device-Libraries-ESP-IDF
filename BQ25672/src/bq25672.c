#include "bq25672.h"

esp_err_t bq25672_init(i2c_master_dev_handle_t *slave_handle)
{
    uint8_t register_address = PART_INFORMATION_REG, register_content = 0x00;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if (((register_content & 0x7) == 0x1) && (((register_content >> 3) & 0x7) == 0x4))
    {
        ESP_LOGI("BQ25672", "Verified BQ25672 chip ID and device revision!");
        return ESP_OK;
    }
    else
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_bus_rm_device(*slave_handle));
        ESP_LOGE("BQ25672", "Invalid BQ25672 chip ID and device revision!");
        return ESP_ERR_INVALID_MAC;
    }
}

esp_err_t bq25672_config_sleep_wakeup(i2c_master_dev_handle_t *slave_handle, bq25672_action_time_t wakeup_delay, bq25672_action_time_t sleep_delay)
{
    uint8_t register_address = CHARGER_CONTROL_3_REG, register_content = 0x00;
    // Set the wake up time to 15ms
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if ((register_content & ~0xf7) != wakeup_delay)
    {
        register_content = (register_content & 0xf7) | wakeup_delay;
        uint8_t write_buffer[2] = {register_address, register_content};
        ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
    }
    ESP_LOGI("BQ25672", "Successfully set the wake up delay to %s", (wakeup_delay != 0) ? "15ms." : "1s.");

    register_address = CHARGER_CONTROL_2_REG;
    ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if ((register_content & 0x01) != sleep_delay)
    {
        register_content = (register_content & ~0x01) | sleep_delay;
        uint8_t write_buffer[2] = {register_address, register_content};
        ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
    }
    ESP_LOGI("BQ25672", "Successfully set the ship FET action delay to %s", (sleep_delay != 0) ? "0s." : "10s.");

    return ESP_OK;
}

esp_err_t bq25672_enter_mode(i2c_master_dev_handle_t *slave_handle, bq25672_power_mode_t mode)
{
    uint8_t register_address = CHARGER_CONTROL_2_REG, register_content = 0x00;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if ((register_content & 0x06) != mode)
    {
        register_content = (register_content & ~0x06) | mode;
        uint8_t write_buffer[2] = {register_address, register_content};
        ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
    }
    ESP_LOGI("I2C", "Written 0x%x to register 0x%x", register_content & 0x06, register_address);

    return ESP_OK;
}

esp_err_t bq25672_read_adc(i2c_master_dev_handle_t *slave_handle, bq25672_adc_t adc, int16_t *adc_value)
{
    return ESP_OK;
}