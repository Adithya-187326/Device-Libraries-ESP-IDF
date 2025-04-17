#ifndef _MAX_6675_H_
#define _MAX_6675_H_

#pragma once

#include "driver/gpio.h"
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @note
     * The red wire from thermocouple corresponds to the hot junction and has to go into +
     *
     * The blue wire from thermocouple corresponds to the cold junction and has to go into -
     */
    esp_err_t get_max6675_data(spi_device_handle_t max6675, gpio_num_t cs, float *temp_output);

#ifdef __cplusplus
}
#endif

#endif
