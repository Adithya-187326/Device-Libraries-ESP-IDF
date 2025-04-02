# _BNO055_

This is the component library for BNO055 communicating over I2C.

# WARNING

BNO055 is a device that does clock stretching. The ESP-IDF I2C Master HAL and LL source file has a fix for clock stretching (adding capability to change time period of stretch) in IDF Version v5.3.2 (post Sep '24). Update or upgrade accordingly for proper functioning.

# IMPLEMENTATION

You can add this to component folder to your project's working directory work without any further modifications. Below is a short explanation of the folder structure:

```
├── CMakeLists.txt
├── components
|   ├── BNO055
|   |   ├── CMakeLists.txt
|   |   ├── include
|   |   ├── src
|   |   ├── README.md                This is the file you are currently reading
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md
```

# SAMPLE CODE

This code demonstrates how to initialize necessary structs to work with the BNO055 component. The verbosity can be adjusted to get raw sensor/config outputs

```c
#include <stdio.h>
#include "bno055.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("BNO_CONFIG", ESP_LOG_INFO);
    esp_log_level_set("BNO_SENSOR", ESP_LOG_VERBOSE);

    imu_t imu_9_dof = {0};

    i2c_master_bus_config_t i2c_master_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_NUM_0,
        .intr_priority = 0,
        .scl_io_num = GPIO_NUM_3,
        .sda_io_num = GPIO_NUM_2,
        .trans_queue_depth = 0,
    };
    i2c_master_bus_handle_t i2c_master_bus = NULL;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_master_conf, &i2c_master_bus));

    i2c_device_config_t bno055_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x28,
        .flags.disable_ack_check = 0,
        .scl_speed_hz = 400000,
        .scl_wait_us = 0xffff,
    };
    i2c_master_dev_handle_t bno055;

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus, &bno055_conf, &bno055));

    ESP_ERROR_CHECK(bno055_initialize(&bno055, &imu_9_dof));
    ESP_ERROR_CHECK(bno055_configure(&bno055, &imu_9_dof, NDOF_MODE, (ACC_MG | GY_RPS | EUL_RAD)));
    while (1)
    {
        bno055_get_readings(&bno055, &imu_9_dof, GRAVITY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

# FUTURE REVISION

Axis remaps, setting offsets and interrupt functions are yet to be engineered.
