#include "max6675.h"

esp_err_t get_max6675_data(spi_device_handle_t max6675, gpio_num_t cs, float *temp_output)
{
    spi_transaction_t trans_desc = {
        .flags = SPI_TRANS_USE_RXDATA,
        .rxlength = 16,
        .length = 16};

    ESP_ERROR_CHECK(gpio_set_level(cs, 0));
    esp_err_t ret = spi_device_polling_transmit(max6675, &trans_desc);
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPI", "Error in reading data from MAX6675: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(5));
    ESP_ERROR_CHECK(gpio_set_level(cs, 1));

    uint16_t miso_value = (((uint16_t)trans_desc.rx_data[0]) << 8) | ((uint16_t)trans_desc.rx_data[1]);
    if (miso_value & 0x4)
    {
        ESP_LOGE("MAX6675", "Thermocouple not connected.");
        *temp_output = 1000000;
        return ESP_ERR_NOT_SUPPORTED;
    }

    *temp_output = ((float)(miso_value >> 3)) * 0.25;
    ESP_LOGI("MAX6675", "Temperature at thermocouple end: %.2f°C", *temp_output);
    return ESP_OK;
}