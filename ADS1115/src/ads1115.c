/**
 * @file ads1115.c
 * @brief ADS1115 16-bit ADC Driver Implementation
 *
 * CERT-C compliant implementation of ADS1115 driver with comprehensive
 * error handling and input validation.
 */

#include "ads1115.h"

 /**
  * @brief Voltage scaling factors for each PGA setting
  *
  * These values represent volts per LSB for 16-bit signed conversion
  * CERT-C DCL00-C: Const correctness for lookup table
  */
static const float PGA_VOLTAGE_SCALES[] = {
    0.0001875f,     /* ±6.144V -> 6.144V / 32768 */
    0.000125f,      /* ±4.096V -> 4.096V / 32768 */
    0.0000625f,     /* ±2.048V -> 2.048V / 32768 */
    0.00003125f,    /* ±1.024V -> 1.024V / 32768 */
    0.000015625f,   /* ±0.512V -> 0.512V / 32768 */
    0.0000078125f   /* ±0.256V -> 0.256V / 32768 */
};

/* Tag for verbosity control in main files */
static const char* ADS_TAG = "ADS1115";

/**
 * @brief Get voltage scale factor from PGA setting
 *
 * CERT-C compliant lookup function with bounds checking
 *
 * @param pga PGA enumeration value
 * @return Voltage scale factor, or default scale on invalid input
 */
static float get_voltage_scale(ads1115_pga_t pga)
{
    /* CERT-C INT31-C: Validate shift operations */
    uint32_t pga_index = (uint32_t)pga >> 9U; /* Extract bits 11:9 */

    /* CERT-C ARR38-C: Array bounds checking */
    if (pga_index >= (sizeof(PGA_VOLTAGE_SCALES) / sizeof(PGA_VOLTAGE_SCALES[0]))) {
        ESP_LOGW(ADS_TAG, "Invalid PGA setting 0x%x, using default", (unsigned int)pga);
        return PGA_VOLTAGE_SCALES[2]; /* Default to ±2.048V */
    }

    return PGA_VOLTAGE_SCALES[pga_index];
}

/**
 * @brief Validate input parameters for NULL pointers
 *
 * CERT-C EXP34-C: Null pointer validation
 *
 * @param device_handle Device handle pointer
 * @param config Configuration pointer
 * @param data Data pointer (can be NULL if not used)
 * @return true if valid, false otherwise
 */
static bool validate_parameters(const i2c_master_dev_handle_t* device_handle,
    const void* config,
    const void* data)
{
    if ((device_handle == NULL) || (*device_handle == NULL)) {
        ESP_LOGE(ADS_TAG, "Device handle is NULL");
        return false;
    }

    if (config == NULL) {
        ESP_LOGE(ADS_TAG, "Configuration pointer is NULL");
        return false;
    }

    /* Data pointer validation only if provided */
    if ((data != NULL) && (*(void**)data == NULL)) {
        ESP_LOGE(ADS_TAG, "Data output pointer is NULL");
        return false;
    }

    return true;
}

/**
 * @brief Convert raw ADC value to appropriate output format
 *
 * @param raw_value 16-bit signed ADC value
 * @param config Configuration containing format and scale
 * @return Converted value (raw counts or voltage)
 */
static float convert_adc_value(int16_t raw_value, const ads1115_config_t* config)
{
    if (config->data_format == ADS1115_DATA_VOLTAGE) {
        return (float)raw_value * config->voltage_scale;
    }
    else {
        return (float)raw_value; /* Raw ADC counts */
    }
}

esp_err_t ads1115_init_default_config(ads1115_config_t* config)
{
    /* CERT-C EXP34-C: Validate pointer */
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Initialize with safe defaults */
    memset(config, 0, sizeof(ads1115_config_t));

    config->input_mux = ADS1115_MUX_AIN0_GND;
    config->pga = ADS1115_PGA_2_048V;
    config->mode = ADS1115_MODE_SINGLE;
    config->data_rate = ADS1115_DR_128_SPS;
    config->comp_mode = ADS1115_COMP_MODE_TRADITIONAL;
    config->comp_polarity = ADS1115_COMP_POL_LOW;
    config->comp_latch = ADS1115_COMP_LAT_DISABLED;
    config->comp_queue = ADS1115_COMP_QUEUE_DISABLE;
    config->data_format = ADS1115_DATA_VOLTAGE;
    config->voltage_scale = get_voltage_scale(config->pga);

    return ESP_OK;
}

esp_err_t ads1115_configure(i2c_master_dev_handle_t* device_handle,
    ads1115_config_t* config)
{
    /* CERT-C EXP34-C: Parameter validation */
    if (!validate_parameters(device_handle, config, NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;
    uint8_t reg_addr = ADS1115_REG_CONFIG;
    uint8_t read_buffer[ADS1115_REGISTER_SIZE_BYTES] = { 0 };

    /* Read current configuration for verification */
    ret = i2c_master_transmit_receive(*device_handle,
        &reg_addr, sizeof(reg_addr),
        read_buffer, sizeof(read_buffer),
        pdMS_TO_TICKS(ADS1115_I2C_TIMEOUT_MS));

    /* CERT-C ERR33-C: Check return values */
    if (ret != ESP_OK) {
        ESP_LOGE(ADS_TAG, "Failed to read config register: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGV(ADS_TAG, "Current config: 0x%04X",
        (unsigned int)((read_buffer[0] << 8U) | read_buffer[1]));

    /* Build new configuration word - CERT-C EXP45-C: Proper parentheses */
    uint16_t config_word = (uint16_t)(ADS1115_OS_START_SINGLE |
        config->input_mux |
        config->pga |
        config->mode);

    uint16_t config_word_low = (uint16_t)(config->data_rate |
        config->comp_mode |
        config->comp_polarity |
        config->comp_latch |
        config->comp_queue);

    /* Prepare write buffer */
    uint8_t write_buffer[3] = {
        ADS1115_REG_CONFIG,
        (uint8_t)(config_word >> 8U),
        (uint8_t)(config_word_low & 0xFFU)
    };

    /* Write configuration */
    ret = i2c_master_transmit(*device_handle,
        write_buffer, sizeof(write_buffer),
        pdMS_TO_TICKS(ADS1115_I2C_TIMEOUT_MS));

    if (ret != ESP_OK) {
        ESP_LOGE(ADS_TAG, "Failed to write config register: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Update voltage scale factor */
    config->voltage_scale = get_voltage_scale(config->pga);

    ESP_LOGD(ADS_TAG, "ADS1115 configured successfully. Scale factor: %e V/LSB",
        (double)config->voltage_scale);

    return ESP_OK;
}

esp_err_t ads1115_read_continuous(i2c_master_dev_handle_t* device_handle,
    const ads1115_config_t* config,
    float* data)
{
    /* CERT-C EXP34-C: Parameter validation */
    if (!validate_parameters(device_handle, config, &data)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;
    uint8_t reg_addr = ADS1115_REG_CONVERSION;
    uint8_t read_buffer[ADS1115_REGISTER_SIZE_BYTES] = { 0 };

    /* Read conversion register */
    ret = i2c_master_transmit_receive(*device_handle,
        &reg_addr, sizeof(reg_addr),
        read_buffer, sizeof(read_buffer),
        pdMS_TO_TICKS(ADS1115_I2C_TIMEOUT_MS));

    /* CERT-C ERR33-C: Check return values */
    if (ret != ESP_OK) {
        ESP_LOGE(ADS_TAG, "Failed to read conversion register: %s", esp_err_to_name(ret));
        return ret;
    }

    /* CERT-C INT31-C: Safe bit operations with proper casting */
    int16_t raw_value = (int16_t)((uint16_t)(read_buffer[0] << 8U) |
        (uint16_t)read_buffer[1]);

    /* Convert to requested format */
    *data = convert_adc_value(raw_value, config);

    ESP_LOGV(ADS_TAG, "Continuous read - Raw: %d, Output: %f %s",
        raw_value, (double)*data,
        (config->data_format == ADS1115_DATA_VOLTAGE) ? "V" : "counts");

    return ESP_OK;
}

esp_err_t ads1115_read_single_shot(i2c_master_dev_handle_t* device_handle,
    const ads1115_config_t* config,
    float* data)
{
    /* CERT-C EXP34-C: Parameter validation */
    if (!validate_parameters(device_handle, config, &data)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;

    /* Build configuration for single-shot conversion */
    uint16_t config_word = (uint16_t)(ADS1115_OS_START_SINGLE |
        config->input_mux |
        config->pga |
        ADS1115_MODE_SINGLE);

    uint16_t config_word_low = (uint16_t)(config->data_rate |
        config->comp_mode |
        config->comp_polarity |
        config->comp_latch |
        config->comp_queue);

    /* Write single-shot configuration */
    uint8_t write_buffer[3] = {
        ADS1115_REG_CONFIG,
        (uint8_t)(config_word >> 8U),
        (uint8_t)(config_word_low & 0xFFU)
    };

    ret = i2c_master_transmit(*device_handle,
        write_buffer, sizeof(write_buffer),
        pdMS_TO_TICKS(ADS1115_I2C_TIMEOUT_MS));

    if (ret != ESP_OK) {
        ESP_LOGE(ADS_TAG, "Failed to start single-shot conversion: %s",
            esp_err_to_name(ret));
        return ret;
    }

    /* Wait for conversion completion */
    vTaskDelay(pdMS_TO_TICKS(ADS1115_CONVERSION_DELAY_MS));

    /* Read conversion result */
    uint8_t reg_addr = ADS1115_REG_CONVERSION;
    uint8_t read_buffer[ADS1115_REGISTER_SIZE_BYTES] = { 0 };

    ret = i2c_master_transmit_receive(*device_handle,
        &reg_addr, sizeof(reg_addr),
        read_buffer, sizeof(read_buffer),
        pdMS_TO_TICKS(ADS1115_I2C_TIMEOUT_MS));

    if (ret != ESP_OK) {
        ESP_LOGE(ADS_TAG, "Failed to read conversion result: %s", esp_err_to_name(ret));
        return ret;
    }

    /* CERT-C INT31-C: Safe bit operations */
    int16_t raw_value = (int16_t)((uint16_t)(read_buffer[0] << 8U) |
        (uint16_t)read_buffer[1]);

    /* Convert to requested format */
    *data = convert_adc_value(raw_value, config);

    ESP_LOGV(ADS_TAG, "Single-shot read - Raw: %d, Output: %f %s",
        raw_value, (double)*data,
        (config->data_format == ADS1115_DATA_VOLTAGE) ? "V" : "counts");

    return ESP_OK;
}