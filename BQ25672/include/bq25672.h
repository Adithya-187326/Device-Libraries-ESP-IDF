#ifndef _BQ25672_H_
#define _BQ25672_H_

#pragma once

#include <math.h>
#include "driver/gpio.h"
#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

typedef enum
{
    MINIMAL_SYSTEM_VOLTAGE_REG,
    CHARGE_VOLTAGE_LIMIT_REG,
    CAHRGE_CURRENT_LIMIT_REG,
    INPUT_VOLTAGE_LIMIT_REG = 0x5,
    INPUT_CURRENT_LIMIT_REG,
    PRECHARGE_CONTROL_REG = 0x8,
    TERMINATION_CONTROL_REG,
    RECHARGE_CONTROL_REG,
    VOTG_REGULATION_REG,
    IOTG_REGULATION_REG = 0xd,
    TIMER_CONTROL_REG,
    CHARGER_CONTROL_0_REG,
    CHARGER_CONTROL_1_REG,
    CHARGER_CONTROL_2_REG,
    CHARGER_CONTROL_3_REG,
    CHARGER_CONTROL_4_REG,
    CHARGER_CONTROL_5_REG,
    MPPT_CONTROL_REG,
    TEMPERATURE_CONTROL_REG,
    NTC_CONTROL_0_REG,
    NTC_CONTROL_1_REG,
    ICO_CURRENT_LIMIT_REG,
    CHARGER_STATUS_0_REG = 0x1b,
    CHARGER_STATUS_1_REG,
    CHARGER_STATUS_2_REG,
    CHARGER_STATUS_3_REG,
    CHARGER_STATUS_4_REG,
    FAULT_STATUS_0_REG,
    FAULT_STATUS_1_REG,
    CHARGER_FLAG_0_REG,
    CHARGER_FLAG_1_REG,
    CHARGER_FLAG_2_REG,
    CHARGER_FLAG_3_REG,
    FAULT_FLAG_0_REG,
    FAULT_FLAG_1_REG,
    CHARGER_MASK_0_REG,
    CHARGER_MASK_1_REG,
    CHARGER_MASK_2_REG,
    CHARGER_MASK_3_REG,
    FAULT_MASK_0_REG,
    FAULT_MASK_1_REG,
    ADC_CONTROL_REG,
    ADC_FUNTION_DISABLE_0_REG,
    ADC_FUNTION_DISABLE_1_REG,
    IBUS_ADC_REG = 0x31,
    IBAT_ADC_REG = 0x33,
    VBUS_ADC_REG = 0x35,
    VAC1_ADC_REG = 0x37,
    VAC2_ADC_REG = 0x39,
    VBAT_ADC_REG = 0x3b,
    VSYS_ADC_REG = 0x3d,
    TS_ADC_REG = 0x3f,
    TDIE_ADC_REG = 0x41,
    DP_ADC_REG = 0x43,
    DM_ADC_REG = 0x45,
    DPDM_DRIVER_REG = 0x47,
    PART_INFORMATION_REG
} bq25672_register_t;

typedef enum
{
    BQ25672_SFET_10S_DELAY = 0x00,
    BQ25672_WAKE_UP_1S = 0x00,
    BQ25672_SFET_NO_DELAY = 0x01,
    BQ25672_WAKE_UP_15MS = 0x01 << 3
} bq25672_action_time_t;

typedef enum
{
    BQ25672_IDLE_MODE,
    BQ25672_SHUTDOWN_MODE = 0x01 << 1,
    BQ25672_SHIP_MODE = 0x02 << 1,
    BQ25672_SYSTEM_POWER_RESET = 0x03 << 1
} bq25672_power_mode_t;

typedef enum
{
    IBUS_ADC = 0x31,
    IBAT_ADC = 0x33,
    VBUS_ADC = 0x35,
    VAC1_ADC = 0x37,
    VAC2_ADC = 0x39,
    VBAT_ADC = 0x3b,
    VSYS_ADC = 0x3d,
    TS_ADC = 0x3f,
    TDIE_ADC = 0x41,
    DP_ADC = 0x43,
    DM_ADC = 0x45
} bq25672_adc_t;

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t bq25672_init(i2c_master_dev_handle_t *slave_handle);

    esp_err_t bq25672_config_sleep_wakeup(i2c_master_dev_handle_t *slave_handle, bq25672_action_time_t wakeup_delay, bq25672_action_time_t sleep_delay);

    esp_err_t bq25672_enter_mode(i2c_master_dev_handle_t *slave_handle, bq25672_power_mode_t mode);

    esp_err_t bq25672_read_adc(i2c_master_dev_handle_t *slave_handle, bq25672_adc_t adc, int16_t *adc_value);

#ifdef __cplusplus
}
#endif

#endif