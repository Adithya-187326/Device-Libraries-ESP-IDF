#ifndef _MAX_30102_H_
#define _MAX_30102_H_

#pragma once

#include "driver/gpio.h"
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "helpers_max30102.h"

typedef enum
{
    /* Status */

    INTERRUPT_STATUS_1,
    INTERRUPT_STATUS_2,
    INTERRUPT_ENABLE_1,
    INTERRUPT_ENABLE_2,
    FIFO_WRITE_POINTER,
    OVERFLOW_COUNTER,
    FIFO_READ_POINTER,
    FIFO_DATA_REGISTER,

    /* Configuration */

    FIFO_CONFIG,
    MODE_CONFIG,
    SPO2_CONFIG,
    LED_PULSE_AMPLITUDE_1 = 0x0c,
    LED_PULSE_AMPLITUDE_2,
    MULTI_LED_CONTROL_REG_1 = 0x11,
    MULTI_LED_CONTROL_REG_2,

    /* Die Temperature */

    DIE_TEMPERATURE_INT = 0x1f,
    DIE_TEMPERATURE_FRAC,
    DIE_TEMPERATURE_CONFIG,

    /* Revision and Part ID */

    REVISION_ID = 0xfe,
    PART_ID = 0xff
} max30102_reg_t;

typedef enum
{
    NO_INT,
    POWER_READY_INT,
    DIE_TEMP_READY_INT,
    AMBIENT_LIGHT_CANCELLATION_OVF_INT = 32,
    PPG_READY_INT = 64,
    ALMOST_FULL_INT = 128
} max30102_int_t;

typedef enum
{
    HEART_RATE_MODE = 2,
    SPO2_MODE,
    MULTI_LED_MODE = 7,
    MAx30102_RESET = 64,
    MAX30102_SHUTDOWN = 128
} max30102_mode_t;

typedef struct
{
    max30102_int_t interrupt;
    max30102_mode_t mode;
} max30102_config_t;

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t max30102_configure(i2c_master_bus_handle_t *i2c_bus, i2c_master_dev_handle_t *max30102_handle, max30102_config_t sensor_conf);

    esp_err_t max30102_display_chip_info(i2c_master_dev_handle_t *max30102_handle);

    esp_err_t max30102_mode_set(i2c_master_dev_handle_t *max30102_handle, max30102_mode_t mode);

    esp_err_t max30102_get_die_temp(i2c_master_dev_handle_t *max30102_handle, float *temp);

#ifdef __cplusplus
}
#endif

#endif