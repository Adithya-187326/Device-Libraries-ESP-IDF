idf_component_register(
    SRCS 
        "src/bno055.c"
        "src/helpers_bno055.c"
    INCLUDE_DIRS
        "."
        "include"
    REQUIRES
        log
        esp_driver_i2c
        esp_driver_gpio
        esp_common
        freertos
)
