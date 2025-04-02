#ifndef _ADS_1115_H_
#define _ADS_1115_H_

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
    CONVERSION_REG,

    /**
     * Bit 15 - Operational Status (1b)
     * Writing:
     * 0b - No effect
     * 1b - start signle conversion (when power-down)
     * Reading:
     * 0b - Device is performing a conversion
     * 1b - When not
     *
     * Bit 14:12 - Input Multiplexer configuration (000b)
     * 000b - P = AIN0 & N = AIN1
     * 001b - P = AIN0 & N = AIN3
     * 010b - P = AIN1 & N = AIN3
     * 011b - P = AIN2 & N = AIN3
     * 100b - P = AIN0 & N = GND
     * 101b - P = AIN1 & N = GND
     * 110b - P = AIN2 & N = GND
     * 111b - P = AIN3 & N = GND
     *
     * Bit 11:9 - Programmable Gain Amplifier configuration (010b)
     * 000b - FSR = 6.144V
     * 001b - FSR = 4.096V
     * 010b - FSR = 2.048V (default)
     * 011b - FSR = 1.024V
     * 100b - FSR = 0.512V
     * 101b - FSR = 0.256V
     * 110b - FSR = 0.256V
     * 111b - FSR = 0.256V
     *
     * Bit 8 - Device Operating Mode (1b)
     * 0b - Continuous Conversion Mode
     * 1b - Single-shot mode or power-down mode
     *
     * Bit 7:5 - Data Rate (100b)
     * 000b - 8 SPS
     * 001b - 16 SPS
     * 010b - 32 SPS
     * 011b - 64 SPS
     * 100b - 128 SPS
     * 101b - 250 SPS
     * 110b - 475 SPS
     * 111b - 860 SPS
     *
     * Bit 4 - Comparator Mode (0b)
     * 0b - Traditional Comparator
     * 1b - Window Comparator
     *
     * Bit 3 - Comparator Polarity (0b)
     * 0b - Active low
     * 1b - Active high
     *
     * Bit 2 - Comparator Latch (0b)
     * 0b - Comparator latch disabled
     * 1b - Comparator latch enabled
     *
     * Bit 1:0 - Comparator Queue (11b)
     * 00b - Assert after one conversion
     * 01b - Assert after two conversions
     * 10b - Assert after four conversions
     * 11b - Disable Comparator and set ALERT/RDY pin to high-impedance
     */
    CONFIGURATION_REG,

    LOW_THRESH_REG,

    HIGH_THRESH_REG
} ads1115_register_t;

typedef enum
{
    PERFORMING_CONVERSION = 0,
    NO_CONVERSION_CURRENTLY = 1 << 7,
    START_CONVERSION = 1 << 7
} ads1115_status_t;

typedef enum
{
    AIN0_AIN1,
    AIN0_AIN3 = 1 << 4,
    AIN1_AIN3 = 2 << 4,
    AIN2_AIN3 = 3 << 4,
    AIN0_GND = 4 << 4,
    AIN1_GND = 5 << 4,
    AIN2_GND = 6 << 4,
    AIN3_GND = 7 << 4
} ads1115_input_mux_t;

typedef enum
{
    FSR_6_144V = 0,
    FSR_4_096V = 1 << 1,
    FSR_2_048V = 2 << 1,
    FSR_1_024V = 3 << 1,
    FSR_0_512V = 4 << 1,
    FSR_0_256V = 5 << 1
} ads1115_gain_t;

typedef enum
{
    CONTINUOUS_CONVERSION_MODE,
    SINGLE_SHOT_MODE
} ads1115_conv_mode_t;

typedef enum
{
    SPS_8 = 0,
    SPS_16 = 1 << 5,
    SPS_32 = 2 << 5,
    SPS_64 = 3 << 5,
    SPS_128 = 4 << 5,
    SPS_250 = 5 << 5,
    SPS_475 = 6 << 5,
    SPS_860 = 7 << 5
} ads1115_data_rate_t;

typedef enum
{
    TRADITIONAL_COMPARATOR = 0,
    WINDOW_COMPARATOR = 1 << 4
} ads1115_comp_mode_t;

typedef enum
{
    ACTIVE_LOW = 0,
    ACTIVE_HIGH = 1 << 3
} ads1115_comp_polarity_t;

typedef enum
{
    COMP_LATCH_DISABLED = 0,
    COMP_LATCH_ENABLED = 1 << 2
} ads1115_comp_latch_t;

typedef enum
{
    COMP_QUEUE_1 = 0,
    COMP_QUEUE_2 = 1,
    COMP_QUEUE_4 = 2,
    COMP_DISABLE = 3
} ads1115_comp_queue_t;

typedef enum
{
    DATA_ADC_RAW,
    DATA_VOLTS
} ads1115_data_out;

typedef struct
{
    ads1115_input_mux_t input_mux;
    ads1115_gain_t gain;
    ads1115_conv_mode_t conv_mode;
    ads1115_data_rate_t data_rate;
    ads1115_comp_mode_t comp_mode;
    ads1115_comp_polarity_t comp_polarity;
    ads1115_comp_latch_t comp_latch;
    ads1115_comp_queue_t comp_queue;
    ads1115_data_out data_type;
    float conversion_factor;
} ads1115_config_t;

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t configure_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config);

    esp_err_t read_continuous_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config, float_t *data);

    esp_err_t read_single_ads1115(i2c_master_dev_handle_t *ads1115_handle, ads1115_config_t *ads1115_config, float_t *data);

#ifdef __cplusplus
}
#endif

#endif