#include "max30102.h"

esp_err_t max30102_display_chip_info(i2c_master_dev_handle_t *max30102_handle)
{
    uint8_t part_id, rev_id, write_buffer = PART_ID;
    esp_err_t ret = i2c_master_transmit_receive(*max30102_handle, &write_buffer, sizeof(write_buffer), &part_id, sizeof(part_id), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from device with address 0x57, register address: 0x%x", write_buffer);
        return ret;
    }

    write_buffer = REVISION_ID;
    ret = i2c_master_transmit_receive(*max30102_handle, &write_buffer, sizeof(write_buffer), &rev_id, sizeof(rev_id), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from device with address 0x57, register address: 0x%x", write_buffer);
        return ret;
    }

    ESP_LOGI("MAX30102", "Part ID: 0x%x; Revision ID: 0x%x", part_id, rev_id);
    return ESP_OK;
}

esp_err_t max30102_mode_set(i2c_master_dev_handle_t *max30102_handle, max30102_mode_t mode)
{
    uint8_t write_buffer[2] = {MODE_CONFIG, mode};
    esp_err_t ret = i2c_master_transmit(*max30102_handle, write_buffer, sizeof(write_buffer), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to device with address 0x57, register address: 0x%x", write_buffer[0]);
        return ret;
    }

    ESP_LOGI("MAX30102", "Sensor mode set to %d", mode);
    return ESP_OK;
}

esp_err_t max30102_get_die_temp(i2c_master_dev_handle_t *max30102_handle, float *temp)
{
    uint8_t read_buffer[2], write_buffer[2] = {DIE_TEMPERATURE_CONFIG, 0x01};
    esp_err_t ret = i2c_master_transmit(*max30102_handle, write_buffer, sizeof(write_buffer), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to device with address 0x57, register address: 0x%x", write_buffer[0]);
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    write_buffer[0] = DIE_TEMPERATURE_INT;
    ret = i2c_master_transmit_receive(*max30102_handle, &write_buffer[0], sizeof(write_buffer[0]), &read_buffer[0], sizeof(read_buffer[0]), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from device with address 0x57, register address: 0x%x", write_buffer[0]);
        return ret;
    }

    write_buffer[0] = DIE_TEMPERATURE_FRAC;
    ret = i2c_master_transmit_receive(*max30102_handle, &write_buffer[0], sizeof(write_buffer[0]), &read_buffer[1], sizeof(read_buffer[1]), 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from device with address 0x57, register address: 0x%x", write_buffer[0]);
        return ret;
    }

    *temp = (int)read_buffer[0] + (read_buffer[1] * 0.0625f);
    return ESP_OK;
}

esp_err_t max30102_configure(i2c_master_bus_handle_t *i2c_bus, i2c_master_dev_handle_t *max30102_handle, max30102_config_t sensor_conf)
{
    esp_err_t ret = i2c_master_probe(*i2c_bus, 0x57, 100);
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "MAX30102 not found.");
        return ret;
    }
    ESP_LOGD("I2C", "Found MAX30102. Initializing configuration.");

    uint8_t write_buffer_2[2] = {INTERRUPT_ENABLE_1, sensor_conf.interrupt};
    if (i2c_master_transmit(*max30102_handle, write_buffer_2, sizeof(write_buffer_2), 100) != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to device with address 0x57, register address: 0x%x", write_buffer_2[0]);
        return ret;
    }

    ESP_LOGI("MAX30102", "Interrupts Set Status - A_FULL: %d, PPG_RDY: %d, ALC_OVF: %d, DIE_TEMP_RDY: %d",
             sensor_conf.interrupt >> 7, ((sensor_conf.interrupt >> 6) & 0x01),
             ((sensor_conf.interrupt >> 5) & 0x01), ((sensor_conf.interrupt >> 1) & 0x01));
    return ESP_OK;
}