# _MAX30102_

This is the component library for MAX6675 communicating over SPI.

# IMPLEMENTATION

You can add this to component folder to your project's working directory work without any further modifications. Below is a short explanation of the folder structure:

```
├── CMakeLists.txt
├── components
|   ├── MAX30102
|   |   ├── CMakeLists.txt
|   |   ├── include
|   |   ├── src
|   |   ├── README.md                This is the file you are currently reading
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md
```

# ADDING THE COMPONENT

Add one line of code in the CMakeLists.txt in the project folder (not the main folder), to add an extra component directory. It is really crucial that the order of the statements is not missed as it can lead to CMake errors.

```C
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components/MAX6675")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project-name)
```

# SAMPLE CODE

This code demonstrates how to initialize all things necessary to work with the MAX6675 component. The verbosity can be adjusted to get error logs, info logs, or no logs.

```c
#include <stdio.h>
#include "max6675.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("MAX6675", ESP_LOG_INFO);

    spi_bus_config_t temp_sense_bus = {
        .miso_io_num = GPIO_NUM_6,
        .sclk_io_num = GPIO_NUM_9,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
    };

    spi_device_interface_config_t temp_config = {
        .address_bits = 0,
        .clock_speed_hz = 100000,
        .command_bits = 0,
        .dummy_bits = 0,
        .flags = 0,
        .mode = 0,
        .queue_size = 4,
        .spics_io_num = GPIO_NUM_7};

    spi_device_handle_t max6675;

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &temp_sense_bus, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &temp_config, &max6675));

    gpio_config_t cs_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_NUM_7),
        .pull_down_en = 0,
        .pull_up_en = 1};

    ESP_ERROR_CHECK(gpio_config(&cs_conf));

    float temperature;

    while (1)
    {
        get_max6675_data(max6675, GPIO_NUM_7, &temperature);
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}
```
