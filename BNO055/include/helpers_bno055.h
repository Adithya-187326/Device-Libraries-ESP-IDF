#ifndef _HELPERS_BNO055_H_
#define _HELPERS_BNO055_H_

#pragma once

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"

typedef struct vector_t
{
    float x;
    float y;
    float z;
} vector_t;

typedef struct quaternion_t
{
    float x;
    float y;
    float z;
    float w;
} quaternion_t;

typedef struct offset_t
{
    vector_t accel;
    vector_t gyro;
    vector_t mag;
} offset_t;

typedef struct scale_t
{
    float accel;
    float gyro;
    float euler;
    float mag;
    float temp;
    float quat;
} scale_t;

typedef struct sensor_config_t
{
    offset_t offsets;
    float accel_radius;
    float gyro_radius;
    scale_t sensor_scale;
    gpio_num_t reset_io;
} sensor_config_t;

#ifdef __cplusplus
extern "C"
{
#endif

    int signum(double value);

    double absolute(double value);

    int quotient(double dividend, double divisor);

#ifdef __cplusplus
}
#endif

#endif