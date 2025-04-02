# _ADS1115_

This is the component library for ADS1115.

# IMPLEMENTATION

You can add this to component folder to your project's working directory work without any further modifications. Below is a short explanation of the folder structure:

```
├── CMakeLists.txt
├── components
|   ├── ADS1115
|   |   ├── CMakeLists.txt
|   |   ├── include
|   |   ├── src
|   |   ├── README.md                This is the file you are currently reading
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md
```

# SAMPLE INITIATION CODE FOR CONTINUOUS CONVERSION

This code demonstrates how to initialize necessary structs to work with the ADS1115 component. The verbosity can be adjusted to get more low level reports

```C
#include <stdio.h>
#include "ads1115.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("ADS", ESP_LOG_VERBOSE);

    i2c_master_bus_config_t i2c_master_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_NUM_0,
        .intr_priority = 0,
        .scl_io_num = GPIO_NUM_7,
        .sda_io_num = GPIO_NUM_6,
        .trans_queue_depth = 0,
    };
    i2c_master_bus_handle_t i2c_master_bus = NULL;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_conf, &i2c_master_bus));

    i2c_device_config_t ads1115_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x48,
        .flags.disable_ack_check = 1,
        .scl_speed_hz = 400000,
        .scl_wait_us = 0,
    };
    i2c_master_dev_handle_t ads1115;

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus, &ads1115_conf, &ads1115));

    // Setup ADS1115
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

    // Starting reading in loop
    float adc_data = 0;
    while (1)
    {
        ESP_ERROR_CHECK(read_continuous_ads1115(&ads1115, &adc_config, &adc_data));
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}
```

# SAMPLE INITIATION CODE FOR SINGLE SHOT CONVERSION

This code demonstrates how to initialize necessary structs to work with the ADS1115 component. The verbosity can be adjusted to get more low level reports

```C
#include <stdio.h>
#include "ads1115.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("ADS", ESP_LOG_VERBOSE);

    i2c_master_bus_config_t i2c_master_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_NUM_0,
        .intr_priority = 0,
        .scl_io_num = GPIO_NUM_7,
        .sda_io_num = GPIO_NUM_6,
        .trans_queue_depth = 0,
    };
    i2c_master_bus_handle_t i2c_master_bus = NULL;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_conf, &i2c_master_bus));

    i2c_device_config_t ads1115_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x48,
        .flags.disable_ack_check = 1,
        .scl_speed_hz = 400000,
        .scl_wait_us = 0,
    };
    i2c_master_dev_handle_t ads1115;

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus, &ads1115_conf, &ads1115));

    // Setup ADS1115
    ads1115_config_t adc_config = {
        .comp_latch = COMP_LATCH_DISABLED,
        .comp_mode = TRADITIONAL_COMPARATOR,
        .comp_polarity = ACTIVE_LOW,
        .comp_queue = COMP_DISABLE,
        .conv_mode = SINGLE_SHOT_MODE,
        .data_rate = SPS_128,
        .gain = FSR_0_256V,
        .input_mux = AIN0_AIN1,
        .data_type = DATA_ADC_RAW
    };
    ESP_ERROR_CHECK(configure_ads1115(&ads1115, &adc_config));

    // Starting reading as needed
    float adc_data = 0;
    adc_config.input_mux = AIN2_GND;
    ESP_ERROR_CHECK(read_single_ads1115(&ads1115, &adc_config, &adc_data));

    vTaskDelay(pdMS_TO_TICKS(100));

    adc_config.gain = FSR_1_024V;
    adc_config.input_mux =AIN0_AIN1;
    ESP_ERROR_CHECK(read_single_ads1115(&ads1115, &adc_config, &adc_data));
}
```

# FUTURE REVISION

Comparator feature based functions are yet to be engineered
