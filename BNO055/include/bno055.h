#ifndef _BNO055_H_
#define _BNO055_H_

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
#include "helpers_bno055.h"

typedef enum bno055_reg_t
{
    // Page 0 Registers
    CHIP_ID = 0x00,       // Chip ID
    ACC_ID = 0x01,        // Accelerometer ID
    MAG_ID = 0x02,        // Magnetometer ID
    GYR_ID = 0x03,        // Gyroscope ID
    SW_REV_ID_LSB = 0x04, // Software Revision ID (LSB)
    SW_REV_ID_MSB = 0x05, // Software Revision ID (MSB)
    BL_REV_ID = 0x06,     // Bootloader Revision ID

    PAGE_ID = 0x07, // Page ID

    ACC_DATA_X_LSB = 0x08, // Accelerometer Data X LSB
    ACC_DATA_X_MSB = 0x09, // Accelerometer Data X MSB
    ACC_DATA_Y_LSB = 0x0A, // Accelerometer Data Y LSB
    ACC_DATA_Y_MSB = 0x0B, // Accelerometer Data Y MSB
    ACC_DATA_Z_LSB = 0x0C, // Accelerometer Data Z LSB
    ACC_DATA_Z_MSB = 0x0D, // Accelerometer Data Z MSB

    MAG_DATA_X_LSB = 0x0E, // Magnetometer Data X LSB
    MAG_DATA_X_MSB = 0x0F, // Magnetometer Data X MSB
    MAG_DATA_Y_LSB = 0x10, // Magnetometer Data Y LSB
    MAG_DATA_Y_MSB = 0x11, // Magnetometer Data Y MSB
    MAG_DATA_Z_LSB = 0x12, // Magnetometer Data Z LSB
    MAG_DATA_Z_MSB = 0x13, // Magnetometer Data Z MSB

    GYR_DATA_X_LSB = 0x14, // Gyroscope Data X LSB
    GYR_DATA_X_MSB = 0x15, // Gyroscope Data X MSB
    GYR_DATA_Y_LSB = 0x16, // Gyroscope Data Y LSB
    GYR_DATA_Y_MSB = 0x17, // Gyroscope Data Y MSB
    GYR_DATA_Z_LSB = 0x18, // Gyroscope Data Z LSB
    GYR_DATA_Z_MSB = 0x19, // Gyroscope Data Z MSB

    EUL_DATA_X_LSB = 0x1A, // Euler Angle X LSB
    EUL_DATA_X_MSB = 0x1B, // Euler Angle X MSB
    EUL_DATA_Y_LSB = 0x1C, // Euler Angle Y LSB
    EUL_DATA_Y_MSB = 0x1D, // Euler Angle Y MSB
    EUL_DATA_Z_LSB = 0x1E, // Euler Angle Z LSB
    EUL_DATA_Z_MSB = 0x1F, // Euler Angle Z MSB

    QUA_DATA_W_LSB = 0x20, // Quaternion W LSB
    QUA_DATA_W_MSB = 0x21, // Quaternion W MSB
    QUA_DATA_X_LSB = 0x22, // Quaternion X LSB
    QUA_DATA_X_MSB = 0x23, // Quaternion X MSB
    QUA_DATA_Y_LSB = 0x24, // Quaternion Y LSB
    QUA_DATA_Y_MSB = 0x25, // Quaternion Y MSB
    QUA_DATA_Z_LSB = 0x26, // Quaternion Z LSB
    QUA_DATA_Z_MSB = 0x27, // Quaternion Z MSB

    LIA_DATA_X_LSB = 0x28, // Linear Acceleration X LSB
    LIA_DATA_X_MSB = 0x29, // Linear Acceleration X MSB
    LIA_DATA_Y_LSB = 0x2A, // Linear Acceleration Y LSB
    LIA_DATA_Y_MSB = 0x2B, // Linear Acceleration Y MSB
    LIA_DATA_Z_LSB = 0x2C, // Linear Acceleration Z LSB
    LIA_DATA_Z_MSB = 0x2D, // Linear Acceleration Z MSB

    GRV_DATA_X_LSB = 0x2E, // Gravity Vector X LSB
    GRV_DATA_X_MSB = 0x2F, // Gravity Vector X MSB
    GRV_DATA_Y_LSB = 0x30, // Gravity Vector Y LSB
    GRV_DATA_Y_MSB = 0x31, // Gravity Vector Y MSB
    GRV_DATA_Z_LSB = 0x32, // Gravity Vector Z LSB
    GRV_DATA_Z_MSB = 0x33, // Gravity Vector Z MSB

    TEMP = 0x34,           // Temperature
    CALIB_STAT = 0x35,     // Calibration Status
    ST_RESULT = 0x36,      // Self-Test Result
    INT_STA = 0x37,        // Interrupt Status
    SYS_CLK_STATUS = 0x38, // System Clock Status
    SYS_STATUS = 0x39,     // System Status
    SYS_ERR = 0x3A,        // System Error

    UNIT_SEL = 0x3B,    // Unit Selection
    OPR_MODE = 0x3D,    // Operation Mode
    PWR_MODE = 0x3E,    // Power Mode
    SYS_TRIGGER = 0x3F, // System Trigger
    TEMP_SOURCE = 0x40, // Temperature Source

    AXIS_MAP_CONFIG = 0x41, // Axis Mapping Configuration
    AXIS_MAP_SIGN = 0x42,   // Axis Sign Configuration

    ACC_OFFSET_X_LSB = 0x55, // Accelerometer Offset X LSB
    ACC_OFFSET_X_MSB = 0x56, // Accelerometer Offset X MSB
    ACC_OFFSET_Y_LSB = 0x57, // Accelerometer Offset Y LSB
    ACC_OFFSET_Y_MSB = 0x58, // Accelerometer Offset Y MSB
    ACC_OFFSET_Z_LSB = 0x59, // Accelerometer Offset Z LSB
    ACC_OFFSET_Z_MSB = 0x5A, // Accelerometer Offset Z MSB

    MAG_OFFSET_X_LSB = 0x5B, // Magnetometer Offset X LSB
    MAG_OFFSET_X_MSB = 0x5C, // Magnetometer Offset X MSB
    MAG_OFFSET_Y_LSB = 0x5D, // Magnetometer Offset Y LSB
    MAG_OFFSET_Y_MSB = 0x5E, // Magnetometer Offset Y MSB
    MAG_OFFSET_Z_LSB = 0x5F, // Magnetometer Offset Z LSB
    MAG_OFFSET_Z_MSB = 0x60, // Magnetometer Offset Z MSB

    GYR_OFFSET_X_LSB = 0x61, // Gyroscope Offset X LSB
    GYR_OFFSET_X_MSB = 0x62, // Gyroscope Offset X MSB
    GYR_OFFSET_Y_LSB = 0x63, // Gyroscope Offset Y LSB
    GYR_OFFSET_Y_MSB = 0x64, // Gyroscope Offset Y MSB
    GYR_OFFSET_Z_LSB = 0x65, // Gyroscope Offset Z LSB
    GYR_OFFSET_Z_MSB = 0x66, // Gyroscope Offset Z MSB

    ACC_RADIUS_LSB = 0x67, // Accelerometer Radius LSB
    ACC_RADIUS_MSB = 0x68, // Accelerometer Radius MSB
    MAG_RADIUS_LSB = 0x69, // Magnetometer Radius LSB
    MAG_RADIUS_MSB = 0x6A, // Magnetometer Radius MSB

    // Page 1 Registers
    ACC_CONFIG = 0x08,       // Accelerometer Configuration
    MAG_CONFIG = 0x09,       // Magnetometer Configuration
    GYR_CONFIG_0 = 0x0A,     // Gyroscope Configuration 0
    GYR_CONFIG_1 = 0x0B,     // Gyroscope Configuration 1
    ACC_SLEEP_CONFIG = 0x0C, // Accelerometer Sleep Configuration
    GYR_SLEEP_CONFIG = 0x0D, // Gyroscope Sleep Configuration
    INT_MSK = 0x0F,          // Interrupt Mask
    INT_EN = 0x10,           // Interrupt Enable
    ACC_AM_THRES = 0x11,     // Accelerometer Any-Motion Threshold
    ACC_INT_SETTINGS = 0x12, // Accelerometer Interrupt Settings
    ACC_HG_DURATION = 0x13,  // Accelerometer High-G Duration
    ACC_HG_THRES = 0x14,     // Accelerometer High-G Threshold
    ACC_NM_THRES = 0x15,     // Accelerometer No-Motion Threshold
    ACC_NM_SET = 0x16,       // Accelerometer No-Motion Settings
    GYR_INT_SETTING = 0x17,  // Gyroscope Interrupt Settings
    GYR_HR_X_SET = 0x18,     // Gyroscope High Rate X-Axis Settings
    GYR_DUR_X = 0x19,        // Gyroscope Duration X
    GYR_HR_Y_SET = 0x1A,     // Gyroscope High Rate Y-Axis Settings
    GYR_DUR_Y = 0x1B,        // Gyroscope Duration Y
    GYR_HR_Z_SET = 0x1C,     // Gyroscope High Rate Z-Axis Settings
    GYR_DUR_Z = 0x1D,        // Gyroscope Duration Z
    GYR_AM_THRES = 0x1E,     // Gyroscope Any-Motion Threshold
    GYR_AM_SET = 0x1F,       // Gyroscope Any-Motion Settings
} bno055_reg_t;

typedef enum bno055_operation_mode_t
{
    CONFIG_MODE,
    ACC_ONLY_MODE,
    MAG_ONLY_MODE,
    GYRO_ONLY_MODE,
    ACC_MAG_MODE,
    ACC_GYRO_MODE,
    MAG_GYRO_MODE,
    AMG_MODE,
    IMU_MODE,
    COMPASS_MODE,
    M4G_MODE,
    NDOF_FMC_OFF_MODE,
    NDOF_MODE
} bno055_operation_mode_t;

typedef enum bno055_units_t
{
    ACC_M_S2 = 0x00,    // Acceleration in m/s^2
    ACC_MG = 0x01,      // Acceleration in mg
    GY_DPS = 0x00,      // Gyroscope in degrees per second
    GY_RPS = 0x02,      // Gyroscope in radians per second
    EUL_DEG = 0x00,     // Euler angles in degrees
    EUL_RAD = 0x04,     // Euler angles in radians
    TEMP_C = 0x00,      // Temperature in degrees Celsius
    TEMP_F = 0x10,      // Temperature in degrees Fahrenheit
    ORI_ANDRIOD = 0x00, // Android orientation mode
    ORI_WINDOWS = 0x80  // Windows orientation mode
} bno055_units_t;

typedef enum bno055_sensor_t
{
    ACCELEROMETER = 8,
    MAGNETOMETER = 14,
    GYROSCOPE = 20,
    EULER_ANGLE = 26,
    QUATERNION = 32,
    LINEAR_ACCELERATION = 40,
    GRAVITY = 46,
    TEMPERATURE = 52
} bno055_sensor_t;

// TODO Modify the power modes properly
typedef enum bno055_power_mode_t
{
    NORMAL_MODE = 0x00,
    LOW_POWER_MODE = 0x01,
    SUSPEND_MODE = 0x02
} bno055_power_mode_t;

typedef struct imu_t
{
    vector_t raw_acceleration;
    vector_t gyroscope;
    vector_t magnetometer;
    vector_t euler_angles;
    quaternion_t quaternion;
    vector_t linear_acceleration;
    vector_t gravity;
    float temperature;
    sensor_config_t bno055_config;
} imu_t;

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t bno055_initialize(i2c_master_dev_handle_t *slave_handle, imu_t *imu);

    esp_err_t bno055_configure(i2c_master_dev_handle_t *slave_handle, imu_t *imu, bno055_operation_mode_t operation_mode, uint8_t units_selected);

    esp_err_t bno055_reset(i2c_master_dev_handle_t *slave_handle, imu_t *imu);

    esp_err_t bno055_calibration_status(i2c_master_dev_handle_t *slave_handle);

    esp_err_t bno055_get_readings(i2c_master_dev_handle_t *slave_handle, imu_t *imu, bno055_sensor_t sensor);

    esp_err_t bno055_get_offsets(i2c_master_dev_handle_t *slave_handle, imu_t *imu);

    // TODO Below functions to be engineered
    esp_err_t bno055_set_axis(i2c_master_dev_handle_t *slave_handle, uint8_t axis_remap, uint8_t axis_sign);

    esp_err_t bno055_power_mode_set(i2c_master_dev_handle_t *slave_handle, bno055_power_mode_t power_mode);

    esp_err_t bno055_set_offsets(i2c_master_dev_handle_t *slave_handle, imu_t *imu);

    // TODO Add in functions that use the BNO055's interrupts

#ifdef __cplusplus
}
#endif

#endif