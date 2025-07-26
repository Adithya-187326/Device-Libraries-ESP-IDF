#ifndef _AS5600_H_
#define _AS5600_H_

#pragma once

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driver/gpio.h"

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif

#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

typedef enum as5600_config_reg_t
{
    ZMCO = 0x00,
    ZPOS_0 = 0x01,
    ZPOS_1 = 0x02,
    MPOS_0 = 0x03,
    MPOS_1 = 0x04,
    MANG_0 = 0x05,
    MANG_1 = 0x06,
    CONF_0 = 0x07,
    CONF_1 = 0x08
} as5600_config_reg_t;

typedef enum as5600_output_reg_t
{
    RAW_ANGLE_0 = 0x0c,
    RAW_ANGLE_1 = 0x0d,
    ANGLE_0 = 0x0e,
    ANGLE_1 = 0x0f
} as5600_output_reg_t;

typedef enum as5600_status_reg_t
{
    STATUS = 0x0b,
    ADC = 0x1a,
    MAGNITUDE_0 = 0x1b,
    MAGNITUDE_1 = 0x1c
} as5600_status_reg_t;

typedef enum as5600_command_t
{
    BURN = 0xff,
    BURN_SETTING = 0x40,
    BURN_ANGLE = 0x80
} as5600_command_t;

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t as5600_initialize(i2c_bus_t* i2c_bus);

    esp_err_t as5600_set_start_position(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_set_stop_position(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_set_max_angle(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_configure(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_read_raw_angle(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_read_scaled_angle(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_read_magnet_status(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_read_gain(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_read_magnitude(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_burn_angle(i2c_bus_t* i2c_bus /* Add in the argument for it */);

    esp_err_t as5600_burn_setting(i2c_bus_t* i2c_bus /* Add in the argument for it */);

#ifdef __cplusplus
}
#endif

#endif