#ifndef _SSD_1306_H_
#define _SSD_1306_H_

#pragma once

#include "driver/gpio.h"
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t ssd1306_init(i2c_port_t i2c_num, uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif
