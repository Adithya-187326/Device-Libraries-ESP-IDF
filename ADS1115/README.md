# ADS1115 16-Bit ADC Driver for ESP-IDF

[![ESP-IDF Version](https://img.shields.io/badge/ESP--IDF-v5.0%2B-blue)](https://github.com/espressif/esp-idf)
[![CERT-C Compliant](https://img.shields.io/badge/CERT--C-Compliant-green)](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A robust, CERT-C compliant ESP-IDF component library for the Texas Instruments ADS1115 16-bit analog-to-digital converter with I2C interface. This library provides comprehensive functionality for both single-shot and continuous conversion modes with extensive error handling and input validation.

## Features

- ✅ **CERT-C Compliant**: Implements secure coding standards with comprehensive input validation
- 🚀 **High Performance**: Optimized I2C communication with configurable timeouts
- 📊 **Flexible Configuration**: Support for all ADS1115 features including:
  - 4 single-ended or 2 differential input channels
  - Programmable gain amplifier (±0.256V to ±6.144V)
  - Configurable data rates (8 to 860 SPS)
  - Built-in comparator functionality
- 🛡️ **Robust Error Handling**: Comprehensive error detection and recovery
- 📚 **Well Documented**: Complete Doxygen documentation with usage examples
- 🔄 **Multiple Output Formats**: Raw ADC counts or calibrated voltage values

## Hardware Support

| Feature            | ADS1115                            |
| ------------------ | ---------------------------------- |
| **Resolution**     | 16-bit                             |
| **Channels**       | 4 single-ended / 2 differential    |
| **Sample Rate**    | 8-860 SPS                          |
| **Input Range**    | ±0.256V to ±6.144V (PGA dependent) |
| **Interface**      | I2C (up to 400kHz)                 |
| **Supply Voltage** | 2.0V to 5.5V                       |

## Installation

### Method 1: ESP Component Registry (Recommended)

```bash
idf.py add-dependency "your-namespace/ads1115"
```

### Method 2: Manual Installation

1. **Add to your project's component directory:**

```
your_project/
├── CMakeLists.txt
├── components/
│   └── ADS1115/                 # This component
│       ├── CMakeLists.txt
│       ├── include/
│       │   └── ads1115.h
│       ├── src/
│       │   └── ads1115.c
│       └── README.md
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── README.md
```

2. **Update your project's CMakeLists.txt:**

```cmake
cmake_minimum_required(VERSION 3.16)

# Add component directory BEFORE include statement
set(EXTRA_COMPONENT_DIRS "components/ADS1115")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(your_project_name)
```

3. **Add dependency in your main component:**

```cmake
# main/CMakeLists.txt
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES ads1115  # Add this line
)
```

## Quick Start

### Basic Usage

```c
#include "ads1115.h"
#include "driver/i2c_master.h"

void app_main(void)
{
    // Configure logging (optional)
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("ADS1115", ESP_LOG_INFO);

    // Initialize I2C master bus
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = GPIO_NUM_22,        // Adjust for your board
        .sda_io_num = GPIO_NUM_21,        // Adjust for your board
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // Configure ADS1115 device
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x48,           // Default ADS1115 address
        .scl_speed_hz = 100000,           // 100kHz for reliable operation
    };

    i2c_master_dev_handle_t ads1115_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &ads1115_handle));

    // Initialize ADS1115 configuration with safe defaults
    ads1115_config_t config;
    ESP_ERROR_CHECK(ads1115_init_default_config(&config));

    // Customize configuration for your application
    config.input_mux = ADS1115_MUX_AIN0_GND;    // Single-ended channel 0
    config.pga = ADS1115_PGA_4_096V;            // ±4.096V range
    config.data_rate = ADS1115_DR_128_SPS;      // 128 samples per second
    config.data_format = ADS1115_DATA_VOLTAGE;  // Output in volts

    // Apply configuration to device
    ESP_ERROR_CHECK(ads1115_configure(&ads1115_handle, &config));

    // Read single-shot conversion
    float voltage;
    esp_err_t ret = ads1115_read_single_shot(&ads1115_handle, &config, &voltage);
    if (ret == ESP_OK) {
        ESP_LOGI("APP", "Measured voltage: %.4f V", voltage);
    } else {
        ESP_LOGE("APP", "Failed to read ADC: %s", esp_err_to_name(ret));
    }
}
```

## Advanced Usage Examples

### Continuous Conversion Mode

```c
void continuous_reading_example(i2c_master_dev_handle_t *handle)
{
    ads1115_config_t config;
    ads1115_init_default_config(&config);

    // Configure for continuous mode
    config.mode = ADS1115_MODE_CONTINUOUS;
    config.input_mux = ADS1115_MUX_AIN0_GND;
    config.pga = ADS1115_PGA_2_048V;
    config.data_rate = ADS1115_DR_250_SPS;
    config.data_format = ADS1115_DATA_VOLTAGE;

    ESP_ERROR_CHECK(ads1115_configure(handle, &config));

    float voltage;
    while (1) {
        esp_err_t ret = ads1115_read_continuous(handle, &config, &voltage);
        if (ret == ESP_OK) {
            ESP_LOGI("ADC", "Continuous reading: %.6f V", voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms between readings
    }
}
```

### Multi-Channel Scanning

```c
void multi_channel_example(i2c_master_dev_handle_t *handle)
{
    ads1115_config_t config;
    ads1115_init_default_config(&config);

    // Configure base settings
    config.pga = ADS1115_PGA_4_096V;
    config.data_rate = ADS1115_DR_128_SPS;
    config.data_format = ADS1115_DATA_VOLTAGE;

    // Array of channels to scan
    ads1115_input_mux_t channels[] = {
        ADS1115_MUX_AIN0_GND,
        ADS1115_MUX_AIN1_GND,
        ADS1115_MUX_AIN2_GND,
        ADS1115_MUX_AIN3_GND
    };

    const char* channel_names[] = {"AIN0", "AIN1", "AIN2", "AIN3"};
    const size_t num_channels = sizeof(channels) / sizeof(channels[0]);

    while (1) {
        ESP_LOGI("ADC", "--- Multi-Channel Reading ---");

        for (size_t i = 0; i < num_channels; i++) {
            config.input_mux = channels[i];

            float voltage;
            esp_err_t ret = ads1115_read_single_shot(handle, &config, &voltage);
            if (ret == ESP_OK) {
                ESP_LOGI("ADC", "%s: %.4f V", channel_names[i], voltage);
            } else {
                ESP_LOGE("ADC", "%s: Error - %s", channel_names[i], esp_err_to_name(ret));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second between scans
    }
}
```

### Differential Measurement

```c
void differential_measurement_example(i2c_master_dev_handle_t *handle)
{
    ads1115_config_t config;
    ads1115_init_default_config(&config);

    // Configure for differential measurement
    config.input_mux = ADS1115_MUX_AIN0_AIN1;   // AIN0(+) - AIN1(-)
    config.pga = ADS1115_PGA_1_024V;            // ±1.024V for better resolution
    config.data_rate = ADS1115_DR_475_SPS;      // Higher sample rate
    config.data_format = ADS1115_DATA_VOLTAGE;

    ESP_ERROR_CHECK(ads1115_configure(handle, &config));

    float differential_voltage;
    esp_err_t ret = ads1115_read_single_shot(handle, &config, &differential_voltage);
    if (ret == ESP_OK) {
        ESP_LOGI("ADC", "Differential voltage (AIN0-AIN1): %.6f V", differential_voltage);
    }
}
```

### Raw ADC Values with Custom Processing

```c
void raw_adc_example(i2c_master_dev_handle_t *handle)
{
    ads1115_config_t config;
    ads1115_init_default_config(&config);

    // Configure for raw ADC output
    config.input_mux = ADS1115_MUX_AIN0_GND;
    config.pga = ADS1115_PGA_2_048V;
    config.data_format = ADS1115_DATA_RAW;      // Raw ADC counts

    ESP_ERROR_CHECK(ads1115_configure(handle, &config));

    float raw_counts;
    esp_err_t ret = ads1115_read_single_shot(handle, &config, &raw_counts);
    if (ret == ESP_OK) {
        // Convert to voltage manually if needed
        float voltage = raw_counts * config.voltage_scale;

        ESP_LOGI("ADC", "Raw ADC: %.0f counts", raw_counts);
        ESP_LOGI("ADC", "Calculated voltage: %.6f V", voltage);

        // Custom processing example: percentage of full scale
        float percentage = (raw_counts / 32767.0f) * 100.0f;
        ESP_LOGI("ADC", "Percentage of full scale: %.2f%%", percentage);
    }
}
```

## API Reference

### Core Functions

| Function                        | Description                                 |
| ------------------------------- | ------------------------------------------- |
| `ads1115_init_default_config()` | Initialize configuration with safe defaults |
| `ads1115_configure()`           | Apply configuration to ADS1115 device       |
| `ads1115_read_single_shot()`    | Perform single ADC conversion               |
| `ads1115_read_continuous()`     | Read from continuous conversion mode        |

### Configuration Options

#### Input Multiplexer (`ads1115_input_mux_t`)

- `ADS1115_MUX_AIN0_AIN1` - Differential: AIN0(+), AIN1(-)
- `ADS1115_MUX_AIN0_AIN3` - Differential: AIN0(+), AIN3(-)
- `ADS1115_MUX_AIN1_AIN3` - Differential: AIN1(+), AIN3(-)
- `ADS1115_MUX_AIN2_AIN3` - Differential: AIN2(+), AIN3(-)
- `ADS1115_MUX_AIN0_GND` - Single-ended: AIN0
- `ADS1115_MUX_AIN1_GND` - Single-ended: AIN1
- `ADS1115_MUX_AIN2_GND` - Single-ended: AIN2
- `ADS1115_MUX_AIN3_GND` - Single-ended: AIN3

#### Programmable Gain Amplifier (`ads1115_pga_t`)

| Setting              | Full Scale Range | Resolution    |
| -------------------- | ---------------- | ------------- |
| `ADS1115_PGA_6_144V` | ±6.144V          | 187.5 µV/LSB  |
| `ADS1115_PGA_4_096V` | ±4.096V          | 125.0 µV/LSB  |
| `ADS1115_PGA_2_048V` | ±2.048V          | 62.5 µV/LSB   |
| `ADS1115_PGA_1_024V` | ±1.024V          | 31.25 µV/LSB  |
| `ADS1115_PGA_0_512V` | ±0.512V          | 15.625 µV/LSB |
| `ADS1115_PGA_0_256V` | ±0.256V          | 7.8125 µV/LSB |

#### Data Rate (`ads1115_data_rate_t`)

- `ADS1115_DR_8_SPS` through `ADS1115_DR_860_SPS`

## Error Handling

The library implements comprehensive error handling following CERT-C guidelines:

```c
esp_err_t ret = ads1115_read_single_shot(&handle, &config, &data);
switch (ret) {
    case ESP_OK:
        ESP_LOGI("APP", "Success: %.4f V", data);
        break;
    case ESP_ERR_INVALID_ARG:
        ESP_LOGE("APP", "Error: Invalid parameters");
        break;
    case ESP_ERR_TIMEOUT:
        ESP_LOGE("APP", "Error: I2C communication timeout");
        break;
    default:
        ESP_LOGE("APP", "Error: %s", esp_err_to_name(ret));
        break;
}
```

## Hardware Connections

### Default Wiring (ESP32)

| ADS1115 Pin | ESP32 Pin | Description            |
| ----------- | --------- | ---------------------- |
| VDD         | 3.3V      | Power supply           |
| GND         | GND       | Ground                 |
| SCL         | GPIO 22   | I2C Clock              |
| SDA         | GPIO 21   | I2C Data               |
| ADDR        | GND       | I2C Address (0x48)     |
| ALRT/RDY    | NC        | Alert/Ready (optional) |

### I2C Addresses

| ADDR Pin Connection | I2C Address    |
| ------------------- | -------------- |
| GND                 | 0x48 (default) |
| VDD                 | 0x49           |
| SDA                 | 0x4A           |
| SCL                 | 0x4B           |

## Troubleshooting

### Common Issues

**Problem**: I2C communication fails

```
Solution: Check wiring, ensure pull-up resistors are enabled, verify I2C address
```

**Problem**: Readings are noisy or unstable

```
Solution: Add decoupling capacitors, use lower data rates, check ground connections
```

**Problem**: Values out of expected range

```
Solution: Verify PGA settings match your input voltage range, check reference voltage
```

### Debug Mode

Enable verbose logging for detailed operation information:

```c
esp_log_level_set("ADS1115", ESP_LOG_VERBOSE);
```

## Performance Characteristics

| Metric                | Value                                  |
| --------------------- | -------------------------------------- |
| **I2C Speed**         | Up to 400kHz                           |
| **Conversion Time**   | 125µs to 125ms (rate dependent)        |
| **Power Consumption** | 150µA (continuous), 0.5µA (power-down) |
| **Memory Usage**      | ~2KB flash, ~100B RAM                  |

## Migration from v1.0

If upgrading from the original version, see the [Migration Guide](MIGRATION.md) for detailed changes and update instructions.

### Key Changes

- Function names now use `ads1115_` prefix
- Enum values updated with proper prefixes
- New parameter validation and error handling
- Configuration structure field name changes

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow CERT-C coding standards
- Add comprehensive tests for new features
- Update documentation for API changes
- Ensure backward compatibility when possible

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE.md) file for details.

## Acknowledgments

- Texas Instruments for the ADS1115 datasheet and specifications
- Espressif Systems for the ESP-IDF framework
- Contributors and users providing feedback and improvements

## Support

- 📖 **Documentation**: Complete API reference in header files
- 🐛 **Issues**: Report bugs on GitHub Issues
- 💬 **Discussions**: Join ESP32 community forums
- 📧 **Contact**: [your-email@example.com]

---

**Version**: 1.1.0  
**ESP-IDF Compatibility**: v5.0+  
**Last Updated**: 2025
