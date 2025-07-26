# ADS1115 Library Migration Guide

**From Version 1.0 to Version 1.1**

This guide helps you migrate from the original ADS1115 library to the improved, CERT-C compliant version 1.1. The new version provides enhanced security, better error handling, and improved performance while maintaining core functionality.

## Overview of Changes

### Major Improvements

- ✅ **CERT-C Compliance**: Enhanced security and reliability
- ✅ **Comprehensive Error Handling**: Better validation and error reporting
- ✅ **Performance Optimizations**: Reduced code duplication and memory usage
- ✅ **Enhanced Documentation**: Complete Doxygen documentation
- ✅ **Improved API Design**: More intuitive function names and parameters

### Breaking Changes

- 🔄 **Function Names**: All functions now use `ads1115_` prefix
- 🔄 **Enum Values**: Updated with proper `ADS1115_` prefixes
- 🔄 **Configuration Structure**: Some field names have changed
- 🔄 **Parameter Validation**: Stricter input validation (may catch previously ignored errors)

---

## Migration Steps

### Step 1: Update Include Statement

**No changes required** - the header file name remains the same.

```c
#include "ads1115.h"  // ✅ Same as before
```

### Step 2: Update Function Names

All public functions now use the `ads1115_` prefix for better namespace management.

| **Old Function**            | **New Function**             |
| --------------------------- | ---------------------------- |
| `configure_ads1115()`       | `ads1115_configure()`        |
| `read_continuous_ads1115()` | `ads1115_read_continuous()`  |
| `read_single_ads1115()`     | `ads1115_read_single_shot()` |

**Migration Example:**

```c
// ❌ Old Version 1.0
ESP_ERROR_CHECK(configure_ads1115(&ads1115_handle, &ads1115_config));
ESP_ERROR_CHECK(read_single_ads1115(&ads1115_handle, &ads1115_config, &data));
ESP_ERROR_CHECK(read_continuous_ads1115(&ads1115_handle, &ads1115_config, &data));

// ✅ New Version 1.1
ESP_ERROR_CHECK(ads1115_configure(&ads1115_handle, &ads1115_config));
ESP_ERROR_CHECK(ads1115_read_single_shot(&ads1115_handle, &ads1115_config, &data));
ESP_ERROR_CHECK(ads1115_read_continuous(&ads1115_handle, &ads1115_config, &data));
```

### Step 3: Update Enum Values

All enum values now have proper `ADS1115_` prefixes to prevent naming conflicts.

#### Input Multiplexer Configuration

| **Old Value** | **New Value**           |
| ------------- | ----------------------- |
| `AIN0_AIN1`   | `ADS1115_MUX_AIN0_AIN1` |
| `AIN0_AIN3`   | `ADS1115_MUX_AIN0_AIN3` |
| `AIN1_AIN3`   | `ADS1115_MUX_AIN1_AIN3` |
| `AIN2_AIN3`   | `ADS1115_MUX_AIN2_AIN3` |
| `AIN0_GND`    | `ADS1115_MUX_AIN0_GND`  |
| `AIN1_GND`    | `ADS1115_MUX_AIN1_GND`  |
| `AIN2_GND`    | `ADS1115_MUX_AIN2_GND`  |
| `AIN3_GND`    | `ADS1115_MUX_AIN3_GND`  |

#### Programmable Gain Amplifier

| **Old Value** | **New Value**        |
| ------------- | -------------------- |
| `FSR_6_144V`  | `ADS1115_PGA_6_144V` |
| `FSR_4_096V`  | `ADS1115_PGA_4_096V` |
| `FSR_2_048V`  | `ADS1115_PGA_2_048V` |
| `FSR_1_024V`  | `ADS1115_PGA_1_024V` |
| `FSR_0_512V`  | `ADS1115_PGA_0_512V` |
| `FSR_0_256V`  | `ADS1115_PGA_0_256V` |

#### Operating Mode

| **Old Value**                | **New Value**             |
| ---------------------------- | ------------------------- |
| `CONTINUOUS_CONVERSION_MODE` | `ADS1115_MODE_CONTINUOUS` |
| `SINGLE_SHOT_MODE`           | `ADS1115_MODE_SINGLE`     |

#### Data Rate

| **Old Value** | **New Value**        |
| ------------- | -------------------- |
| `SPS_8`       | `ADS1115_DR_8_SPS`   |
| `SPS_16`      | `ADS1115_DR_16_SPS`  |
| `SPS_32`      | `ADS1115_DR_32_SPS`  |
| `SPS_64`      | `ADS1115_DR_64_SPS`  |
| `SPS_128`     | `ADS1115_DR_128_SPS` |
| `SPS_250`     | `ADS1115_DR_250_SPS` |
| `SPS_475`     | `ADS1115_DR_475_SPS` |
| `SPS_860`     | `ADS1115_DR_860_SPS` |

#### Comparator Configuration

| **Old Value**            | **New Value**                   |
| ------------------------ | ------------------------------- |
| `TRADITIONAL_COMPARATOR` | `ADS1115_COMP_MODE_TRADITIONAL` |
| `WINDOW_COMPARATOR`      | `ADS1115_COMP_MODE_WINDOW`      |
| `ACTIVE_LOW`             | `ADS1115_COMP_POL_LOW`          |
| `ACTIVE_HIGH`            | `ADS1115_COMP_POL_HIGH`         |
| `COMP_LATCH_DISABLED`    | `ADS1115_COMP_LAT_DISABLED`     |
| `COMP_LATCH_ENABLED`     | `ADS1115_COMP_LAT_ENABLED`      |
| `COMP_QUEUE_1`           | `ADS1115_COMP_QUEUE_1`          |
| `COMP_QUEUE_2`           | `ADS1115_COMP_QUEUE_2`          |
| `COMP_QUEUE_4`           | `ADS1115_COMP_QUEUE_4`          |
| `COMP_DISABLE`           | `ADS1115_COMP_QUEUE_DISABLE`    |

#### Data Output Format

| **Old Value**  | **New Value**          |
| -------------- | ---------------------- |
| `DATA_ADC_RAW` | `ADS1115_DATA_RAW`     |
| `DATA_VOLTS`   | `ADS1115_DATA_VOLTAGE` |

### Step 4: Update Configuration Structure

The configuration structure has some field name changes:

| **Old Field Name**  | **New Field Name** |
| ------------------- | ------------------ |
| `gain`              | `pga`              |
| `conv_mode`         | `mode`             |
| `data_type`         | `data_format`      |
| `conversion_factor` | `voltage_scale`    |

**Migration Example:**

```c
// ❌ Old Version 1.0
ads1115_config_t adc_config = {
    .input_mux = AIN0_AIN1,
    .gain = FSR_0_256V,
    .conv_mode = CONTINUOUS_CONVERSION_MODE,
    .data_rate = SPS_128,
    .comp_mode = TRADITIONAL_COMPARATOR,
    .comp_polarity = ACTIVE_LOW,
    .comp_latch = COMP_LATCH_DISABLED,
    .comp_queue = COMP_DISABLE,
    .data_type = DATA_ADC_RAW
};

// ✅ New Version 1.1
ads1115_config_t adc_config = {
    .input_mux = ADS1115_MUX_AIN0_AIN1,
    .pga = ADS1115_PGA_0_256V,
    .mode = ADS1115_MODE_CONTINUOUS,
    .data_rate = ADS1115_DR_128_SPS,
    .comp_mode = ADS1115_COMP_MODE_TRADITIONAL,
    .comp_polarity = ADS1115_COMP_POL_LOW,
    .comp_latch = ADS1115_COMP_LAT_DISABLED,
    .comp_queue = ADS1115_COMP_QUEUE_DISABLE,
    .data_format = ADS1115_DATA_RAW
};
```

### Step 5: Use New Initialization Function (Recommended)

The new version provides a safe initialization function:

```c
// ✅ New recommended approach
ads1115_config_t config;
ESP_ERROR_CHECK(ads1115_init_default_config(&config));

// Customize as needed
config.input_mux = ADS1115_MUX_AIN0_GND;
config.pga = ADS1115_PGA_4_096V;
config.data_format = ADS1115_DATA_VOLTAGE;
```

---

## Complete Migration Example

### Before (Version 1.0)

```c
#include "ads1115.h"

void app_main(void)
{
    // I2C setup code here...

    ads1115_config_t adc_config = {
        .comp_latch = COMP_LATCH_DISABLED,
        .comp_mode = TRADITIONAL_COMPARATOR,
        .comp_polarity = ACTIVE_LOW,
        .comp_queue = COMP_DISABLE,
        .conv_mode = CONTINUOUS_CONVERSION_MODE,
        .data_rate = SPS_128,
        .gain = FSR_0_256V,
        .input_mux = AIN0_AIN1,
        .data_type = DATA_ADC_RAW
    };

    ESP_ERROR_CHECK(configure_ads1115(&ads1115, &adc_config));

    float adc_data = 0;
    while (1) {
        ESP_ERROR_CHECK(read_continuous_ads1115(&ads1115, &adc_config, &adc_data));
        printf("ADC Reading: %.2f\n", adc_data);
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}
```

### After (Version 1.1)

```c
#include "ads1115.h"

void app_main(void)
{
    // I2C setup code here...

    // Initialize with safe defaults
    ads1115_config_t adc_config;
    ESP_ERROR_CHECK(ads1115_init_default_config(&adc_config));

    // Customize configuration
    adc_config.comp_latch = ADS1115_COMP_LAT_DISABLED;
    adc_config.comp_mode = ADS1115_COMP_MODE_TRADITIONAL;
    adc_config.comp_polarity = ADS1115_COMP_POL_LOW;
    adc_config.comp_queue = ADS1115_COMP_QUEUE_DISABLE;
    adc_config.mode = ADS1115_MODE_CONTINUOUS;
    adc_config.data_rate = ADS1115_DR_128_SPS;
    adc_config.pga = ADS1115_PGA_0_256V;
    adc_config.input_mux = ADS1115_MUX_AIN0_AIN1;
    adc_config.data_format = ADS1115_DATA_RAW;

    ESP_ERROR_CHECK(ads1115_configure(&ads1115, &adc_config));

    float adc_data = 0;
    while (1) {
        esp_err_t ret = ads1115_read_continuous(&ads1115, &adc_config, &adc_data);
        if (ret == ESP_OK) {
            ESP_LOGI("ADC", "ADC Reading: %.2f", adc_data);
        } else {
            ESP_LOGE("ADC", "Read failed: %s", esp_err_to_name(ret));
        }
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}
```

---

## New Features Available

### 1. Safe Configuration Initialization

```c
// Initialize with validated defaults
ads1115_config_t config;
ESP_ERROR_CHECK(ads1115_init_default_config(&config));
```

### 2. Enhanced Error Handling

```c
esp_err_t ret = ads1115_read_single_shot(&handle, &config, &data);
if (ret != ESP_OK) {
    ESP_LOGE("APP", "ADC read failed: %s", esp_err_to_name(ret));
    // Handle error appropriately
}
```

### 3. Improved Parameter Validation

The new version validates all input parameters and provides clear error messages for invalid configurations.

---

## Testing Your Migration

### 1. Compilation Test

```bash
idf.py build
```

### 2. Basic Functionality Test

```c
void test_basic_functionality(void) {
    ads1115_config_t config;

    // Test configuration initialization
    esp_err_t ret = ads1115_init_default_config(&config);
    assert(ret == ESP_OK);

    // Test parameter validation
    ret = ads1115_configure(NULL, &config);
    assert(ret == ESP_ERR_INVALID_ARG);  // Should fail

    ESP_LOGI("TEST", "Basic migration tests passed");
}
```

### 3. Hardware Validation

Test with your actual hardware setup to ensure readings are consistent with the previous version.

---

## Common Migration Issues

### Issue 1: Compilation Errors

**Problem**: Undefined symbols after migration
**Solution**: Ensure all enum values are updated with new prefixes

### Issue 2: Runtime Errors

**Problem**: ESP_ERR_INVALID_ARG errors that didn't occur before
**Solution**: The new version has stricter parameter validation - check for NULL pointers

### Issue 3: Different Behavior

**Problem**: Readings seem different from v1.0
**Solution**: Verify PGA settings and data format configuration match your previous setup

---

## Support and Troubleshooting

### If You Encounter Issues:

1. **Check the changelog** for any behavior changes
2. **Review compiler warnings** - they often indicate migration issues
3. **Test with default configuration** first, then customize
4. **Enable verbose logging** to debug issues:
   ```c
   esp_log_level_set("ADS1115", ESP_LOG_VERBOSE);
   ```

### Getting Help

- 📧 **Email**: Create an issue on the project repository
- 📚 **Documentation**: Check the complete API reference in the header file
- 🐛 **Bug Reports**: Include your configuration and error messages

---

**Migration Checklist:**

- [ ] Updated function names with `ads1115_` prefix
- [ ] Updated all enum values with `ADS1115_` prefixes
- [ ] Changed structure field names (`gain` → `pga`, etc.)
- [ ] Added error handling for stricter parameter validation
- [ ] Tested compilation and basic functionality
- [ ] Validated with actual hardware
- [ ] Updated any documentation or comments
- [ ] Committed changes with clear migration notes

**🎉 Congratulations!** Your ADS1115 library is now upgraded to v1.1 with enhanced security, better error handling, and improved maintainability.
