#ifndef ADS1115_H_
#define ADS1115_H_

/**
 * @file ads1115.h
 * @brief ADS1115 16-bit ADC Driver for ESP-IDF
 *
 * This library provides a comprehensive interface for the Texas Instruments
 * ADS1115 16-bit analog-to-digital converter with I2C interface.
 *
 * Features:
 * - Single-shot and continuous conversion modes
 * - Programmable gain amplifier (PGA) support
 * - Multiple input multiplexer configurations
 * - Raw ADC and voltage output options
 * - CERT-C compliant implementation
 *
 * @author Adithya Venkata Narayanan
 * @version 1.1.0
 * @date 2024
 */

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

 /* CERT-C Compliant Constants */
#define ADS1115_I2C_TIMEOUT_MS          (1000U)    /**< I2C operation timeout */
#define ADS1115_CONVERSION_DELAY_MS     (10U)      /**< Single-shot conversion delay */
#define ADS1115_MAX_ADC_VALUE           (32767)    /**< Maximum positive ADC value */
#define ADS1115_MIN_ADC_VALUE           (-32767)   /**< Maximum negative ADC value */
#define ADS1115_REGISTER_SIZE_BYTES     (2U)       /**< Register size in bytes */

/**
 * @brief ADS1115 Register addresses
 */
typedef enum {
    ADS1115_REG_CONVERSION = 0x00U,     /**< Conversion register (read-only) */
    ADS1115_REG_CONFIG = 0x01U,         /**< Configuration register */
    ADS1115_REG_LOW_THRESH = 0x02U,     /**< Low threshold register */
    ADS1115_REG_HIGH_THRESH = 0x03U     /**< High threshold register */
} ads1115_register_t;

/**
 * @brief Operational Status bits for Configuration Register
 */
typedef enum {
    ADS1115_OS_BUSY = 0x0000U,          /**< Device is performing conversion */
    ADS1115_OS_NOT_BUSY = 0x8000U,      /**< Device is not performing conversion */
    ADS1115_OS_START_SINGLE = 0x8000U   /**< Start single conversion */
} ads1115_op_status_t;

/**
 * @brief Input Multiplexer Configuration
 *
 * Selects the input pins for differential or single-ended measurements
 */
typedef enum {
    ADS1115_MUX_AIN0_AIN1 = 0x0000U,    /**< Differential: AIN0(+) and AIN1(-) */
    ADS1115_MUX_AIN0_AIN3 = 0x1000U,    /**< Differential: AIN0(+) and AIN3(-) */
    ADS1115_MUX_AIN1_AIN3 = 0x2000U,    /**< Differential: AIN1(+) and AIN3(-) */
    ADS1115_MUX_AIN2_AIN3 = 0x3000U,    /**< Differential: AIN2(+) and AIN3(-) */
    ADS1115_MUX_AIN0_GND = 0x4000U,     /**< Single-ended: AIN0 */
    ADS1115_MUX_AIN1_GND = 0x5000U,     /**< Single-ended: AIN1 */
    ADS1115_MUX_AIN2_GND = 0x6000U,     /**< Single-ended: AIN2 */
    ADS1115_MUX_AIN3_GND = 0x7000U      /**< Single-ended: AIN3 */
} ads1115_input_mux_t;

/**
 * @brief Programmable Gain Amplifier (PGA) Configuration
 *
 * Sets the full-scale range of the ADC input
 */
typedef enum {
    ADS1115_PGA_6_144V = 0x0000U,       /**< ±6.144V (default) */
    ADS1115_PGA_4_096V = 0x0200U,       /**< ±4.096V */
    ADS1115_PGA_2_048V = 0x0400U,       /**< ±2.048V */
    ADS1115_PGA_1_024V = 0x0600U,       /**< ±1.024V */
    ADS1115_PGA_0_512V = 0x0800U,       /**< ±0.512V */
    ADS1115_PGA_0_256V = 0x0A00U        /**< ±0.256V */
} ads1115_pga_t;

/**
 * @brief Device Operating Mode
 */
typedef enum {
    ADS1115_MODE_CONTINUOUS = 0x0000U,   /**< Continuous conversion mode */
    ADS1115_MODE_SINGLE = 0x0100U        /**< Single-shot conversion mode */
} ads1115_mode_t;

/**
 * @brief Data Rate Configuration
 *
 * Sets the samples per second (SPS) rate
 */
typedef enum {
    ADS1115_DR_8_SPS = 0x0000U,         /**< 8 samples per second */
    ADS1115_DR_16_SPS = 0x0020U,        /**< 16 samples per second */
    ADS1115_DR_32_SPS = 0x0040U,        /**< 32 samples per second */
    ADS1115_DR_64_SPS = 0x0060U,        /**< 64 samples per second */
    ADS1115_DR_128_SPS = 0x0080U,       /**< 128 samples per second (default) */
    ADS1115_DR_250_SPS = 0x00A0U,       /**< 250 samples per second */
    ADS1115_DR_475_SPS = 0x00C0U,       /**< 475 samples per second */
    ADS1115_DR_860_SPS = 0x00E0U        /**< 860 samples per second */
} ads1115_data_rate_t;

/**
 * @brief Comparator Mode
 */
typedef enum {
    ADS1115_COMP_MODE_TRADITIONAL = 0x0000U, /**< Traditional comparator */
    ADS1115_COMP_MODE_WINDOW = 0x0010U       /**< Window comparator */
} ads1115_comp_mode_t;

/**
 * @brief Comparator Polarity
 */
typedef enum {
    ADS1115_COMP_POL_LOW = 0x0000U,     /**< Active low (default) */
    ADS1115_COMP_POL_HIGH = 0x0008U     /**< Active high */
} ads1115_comp_polarity_t;

/**
 * @brief Comparator Latch
 */
typedef enum {
    ADS1115_COMP_LAT_DISABLED = 0x0000U, /**< Non-latching (default) */
    ADS1115_COMP_LAT_ENABLED = 0x0004U   /**< Latching */
} ads1115_comp_latch_t;

/**
 * @brief Comparator Queue
 */
typedef enum {
    ADS1115_COMP_QUEUE_1 = 0x0000U,     /**< Assert after 1 conversion */
    ADS1115_COMP_QUEUE_2 = 0x0001U,     /**< Assert after 2 conversions */
    ADS1115_COMP_QUEUE_4 = 0x0002U,     /**< Assert after 4 conversions */
    ADS1115_COMP_QUEUE_DISABLE = 0x0003U /**< Disable comparator (default) */
} ads1115_comp_queue_t;

/**
 * @brief Output data format selection
 */
typedef enum {
    ADS1115_DATA_RAW = 0U,              /**< Raw ADC counts */
    ADS1115_DATA_VOLTAGE = 1U           /**< Voltage in volts */
} ads1115_data_format_t;

/**
 * @brief ADS1115 Configuration Structure
 *
 * Contains all configuration parameters for the ADS1115 device
 */
typedef struct {
    ads1115_input_mux_t input_mux;          /**< Input multiplexer configuration */
    ads1115_pga_t pga;                      /**< Programmable gain amplifier setting */
    ads1115_mode_t mode;                    /**< Operating mode */
    ads1115_data_rate_t data_rate;          /**< Data rate (samples per second) */
    ads1115_comp_mode_t comp_mode;          /**< Comparator mode */
    ads1115_comp_polarity_t comp_polarity;  /**< Comparator polarity */
    ads1115_comp_latch_t comp_latch;        /**< Comparator latch setting */
    ads1115_comp_queue_t comp_queue;        /**< Comparator queue setting */
    ads1115_data_format_t data_format;      /**< Output data format */
    float voltage_scale;                    /**< Voltage scaling factor (calculated) */
} ads1115_config_t;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Configure the ADS1115 device
     *
     * Writes the configuration to the ADS1115 configuration register and
     * calculates the appropriate voltage scaling factor based on PGA setting.
     *
     * @param[in] device_handle Pointer to I2C device handle (must not be NULL)
     * @param[in,out] config Pointer to configuration structure (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if parameters are NULL
     * @return ESP_ERR_TIMEOUT if I2C communication times out
     * @return Other ESP error codes for I2C communication failures
     *
     * @note The voltage_scale field in config will be updated based on PGA setting
     * @note This function performs input validation per CERT-C guidelines
     */
    esp_err_t ads1115_configure(i2c_master_dev_handle_t* device_handle,
        ads1115_config_t* config);

    /**
     * @brief Read ADC value in continuous conversion mode
     *
     * Reads the current conversion result from the ADS1115 conversion register.
     * The device must be previously configured for continuous mode.
     *
     * @param[in] device_handle Pointer to I2C device handle (must not be NULL)
     * @param[in] config Pointer to configuration structure (must not be NULL)
     * @param[out] data Pointer to store the result (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if parameters are NULL
     * @return ESP_ERR_TIMEOUT if I2C communication times out
     * @return Other ESP error codes for I2C communication failures
     *
     * @note Output format depends on config->data_format setting
     * @note No conversion delay is applied in this function
     */
    esp_err_t ads1115_read_continuous(i2c_master_dev_handle_t* device_handle,
        const ads1115_config_t* config,
        float* data);

    /**
     * @brief Perform single-shot ADC conversion
     *
     * Initiates a single conversion, waits for completion, and reads the result.
     * This function temporarily overrides the mode setting to single-shot.
     *
     * @param[in] device_handle Pointer to I2C device handle (must not be NULL)
     * @param[in] config Pointer to configuration structure (must not be NULL)
     * @param[out] data Pointer to store the result (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if parameters are NULL
     * @return ESP_ERR_TIMEOUT if I2C communication times out
     * @return Other ESP error codes for I2C communication failures
     *
     * @note This function includes appropriate conversion delay
     * @note Output format depends on config->data_format setting
     */
    esp_err_t ads1115_read_single_shot(i2c_master_dev_handle_t* device_handle,
        const ads1115_config_t* config,
        float* data);

    /**
     * @brief Initialize ADS1115 configuration with safe defaults
     *
     * Sets up a configuration structure with commonly used default values
     * that comply with CERT-C safety requirements.
     *
     * @param[out] config Pointer to configuration structure to initialize
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if config is NULL
     *
     * @note Default settings: Single-ended AIN0, ±2.048V range, single-shot mode, 128 SPS
     */
    esp_err_t ads1115_init_default_config(ads1115_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* ADS1115_H_ */