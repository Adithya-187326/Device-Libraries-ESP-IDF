#include "bno055.h"

esp_err_t set_page(i2c_master_dev_handle_t *slave_handle, uint8_t page)
{
    uint8_t register_address, register_content = 0x00;

    // Reading page id register
    register_address = PAGE_ID;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    // Check if already in same page
    if (register_content == page)
    {
        ESP_LOGV("BNO_CONFIG", "Already in page: '%d'", page);
        return ESP_OK;
    }

    // Setting page id
    uint8_t write_buffer[2] = {register_address, page};
    ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t set_external_crystal_use(i2c_master_dev_handle_t *slave_handle, bool state)
{
    uint8_t register_address, register_content = 0x00;

    // Set page to 0
    set_page(slave_handle, 0);

    // Reading operation mode
    register_address = OPR_MODE;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if ((register_content & 0x0f) != CONFIG_MODE)
    {
        ESP_LOGE("BNO_CONFIG", "Cannot set external crystal usage in non configuration modes. Current mode: %d", register_content & 0x0f);
        return ESP_ERR_INVALID_STATE;
    }

    register_address = SYS_TRIGGER;
    ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    register_content = (register_content & ~0x80) | (state ? 0x80 : 0x00);
    uint8_t write_buffer[2] = {register_address, register_content};
    ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(650));
    return ESP_OK;
}

esp_err_t set_operation_mode(i2c_master_dev_handle_t *slave_handle, bno055_operation_mode_t operation_mode)
{
    uint8_t register_address, register_content = 0x00;

    // Reading operation mode
    register_address = OPR_MODE;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    // Check if already in same mode
    if ((register_content & 0x0f) == operation_mode)
        return ESP_OK;

    // Setting operation mode
    register_content = (register_content & ~0x0f) | operation_mode;
    uint8_t write_buffer[2] = {register_address, register_content};
    ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
    return ESP_OK;
}

esp_err_t set_units(i2c_master_dev_handle_t *slave_handle, imu_t *imu, uint8_t units_selected)
{
    uint8_t register_address, register_content = 0x00;

    ESP_LOGV("BNO_CONFIG", "Setting units to: '%d'", units_selected);
    // Reading operation mode
    register_address = OPR_MODE;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    // Check if not in config mode
    if ((register_content & 0x0f) != CONFIG_MODE)
        return ESP_ERR_INVALID_STATE;

    ESP_LOGV("BNO_CONFIG", " BNO is in config mode, will set page 0");
    // Set page to 0
    ret = set_page(slave_handle, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE("BNO_CONFIG", "Failed to set page 0. Error: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGV("BNO_CONFIG", " BNO is page 0, will check units");
    // Check if already in same units
    register_address = UNIT_SEL;
    ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    if ((register_content & 0x9f) == units_selected)
    {
        ESP_LOGV("BNO_CONFIG", "Already in units: '%d'", units_selected);
    }
    else
    {
        ESP_LOGV("BNO_CONFIG", " BNO has different units, will set units");
        // Setting units
        register_content = (register_content & ~0x9f) | units_selected;
        uint8_t write_buffer[2] = {register_address, register_content};
        ret = i2c_master_transmit(*slave_handle, write_buffer, sizeof(write_buffer), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to write to register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
    }

    switch (units_selected & 0x01)
    {
    case ACC_MG:
        imu->bno055_config.sensor_scale.accel = 1.0f;
        break;

    case ACC_M_S2:
        imu->bno055_config.sensor_scale.accel = 100.0f;
        break;
    }
    switch (units_selected & 0x02)
    {
    case GY_RPS:
        imu->bno055_config.sensor_scale.gyro = 900.0f;
        break;

    case GY_DPS:
        imu->bno055_config.sensor_scale.gyro = 16.0f;
        break;
    }
    switch (units_selected & 0x04)
    {
    case EUL_RAD:
        imu->bno055_config.sensor_scale.euler = 900.0f;
        break;

    case EUL_DEG:
        imu->bno055_config.sensor_scale.euler = 16.0f;
        break;
    }
    switch (units_selected & 0x10)
    {
    case TEMP_F:
        imu->bno055_config.sensor_scale.temp = 0.5f;
        break;

    case TEMP_C:
        imu->bno055_config.sensor_scale.temp = 1.0f;
        break;
    }
    imu->bno055_config.sensor_scale.mag = 16.0f; // magnetometer is always in uT
    imu->bno055_config.sensor_scale.quat = 16384.0f;

    return ESP_OK;
}

esp_err_t bno055_initialize(i2c_master_dev_handle_t *slave_handle, imu_t *imu)
{
    uint8_t register_address, register_content = 0x00;

    // Reading chip ID register
    register_address = CHIP_ID;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    switch (register_content)
    {
    case 0xA0:
        ESP_LOGD("BNO_CONFIG", "Verified BNO055 chip ID '0x%x'", register_content);
        break;

    default:
        ESP_LOGE("BNO_CONFIG", "Invalid chip ID: '0x%x'", register_content);
        return ESP_ERR_INVALID_MAC;
    }

    // Configuring reset pin and setting high
    ret = gpio_set_direction(imu->bno055_config.reset_io, GPIO_MODE_OUTPUT);
    if (ret != ESP_OK)
        return ret;

    ret = gpio_set_pull_mode(imu->bno055_config.reset_io, GPIO_PULLUP_ENABLE);
    if (ret != ESP_OK)
        return ret;

    ret = gpio_set_level(imu->bno055_config.reset_io, 1);
    if (ret != ESP_OK)
        return ret;

    // Set page to 0
    ret = set_page(slave_handle, 0);
    if (ret != ESP_OK)
        return ret;

    // Setting operation mode register to config mode - 0b0000
    ret = set_operation_mode(slave_handle, CONFIG_MODE);

    vTaskDelay(pdMS_TO_TICKS(50));
    return ret;
}

esp_err_t bno055_configure(i2c_master_dev_handle_t *slave_handle, imu_t *imu, bno055_operation_mode_t operation_mode, uint8_t units_selected)
{
    esp_err_t ret;

    ESP_LOGV("BNO_CONFIG", "Setting external crystal.");
    // Setting the external crystal to be used
    ret = set_external_crystal_use(slave_handle, 1);
    if (ret != ESP_OK)
        return ret;

    ESP_LOGV("BNO_CONFIG", "Setting operation mode to config.");
    // Set operation mode config
    ret = set_operation_mode(slave_handle, CONFIG_MODE);
    if (ret != ESP_OK)
        return ret;

    ESP_LOGV("BNO_CONFIG", "Setting units.");
    // Setting units
    ret = set_units(slave_handle, imu, units_selected);
    if (ret != ESP_OK)
        return ret;

    // Set it to required operation mode - NDOF parameter (for PW)
    ret = set_operation_mode(slave_handle, operation_mode);
    if (ret != ESP_OK)
        return ret;

    ESP_LOGI("BNO_CONFIG", "Configured BNO055 for NDOF operation.");
    return ret;
}

esp_err_t bno055_calibration_status(i2c_master_dev_handle_t *slave_handle)
{
    // Reading calibration status register
    uint8_t register_address = CALIB_STAT, register_content = 0x00;
    esp_err_t ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content, sizeof(register_content), pdMS_TO_TICKS(1000));
    if (ret != ESP_OK)
    {
        ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGV("BNO_SENSOR", "Calibration status - Acc: %d, Gyro: %d, Mag: %d, Sys: %d", (register_content & 0x0c) / 4, (register_content & 0x30) / 16, (register_content & 0x03), (register_content & 0xc0) / 64);
    return ESP_OK;
}

esp_err_t bno055_get_readings(i2c_master_dev_handle_t *slave_handle, imu_t *imu, bno055_sensor_t sensor)
{
    esp_err_t ret;
    uint8_t register_address = sensor, register_content[8] = {0}, reg_count;
    switch (sensor)
    {
    case QUATERNION:
        reg_count = 8;
        break;

    case TEMPERATURE:
        reg_count = 1;
        break;

    default:
        reg_count = 6;
        break;
    }

    for (size_t i = 0; i < reg_count; i++)
    {
        ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content[i], sizeof(register_content[i]), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
        register_address++;
    }

    switch (sensor)
    {
    case ACCELEROMETER:
        imu->raw_acceleration.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.accel;
        imu->raw_acceleration.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.accel;
        imu->raw_acceleration.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.accel;
        ESP_LOGV("BNO_SENSOR", "Acceleration vector - X: %.3f, Y: %.3f, Z: %.3f", imu->raw_acceleration.x, imu->raw_acceleration.y, imu->raw_acceleration.z);
        break;

    case MAGNETOMETER:
        imu->magnetometer.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.mag;
        imu->magnetometer.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.mag;
        imu->magnetometer.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.mag;
        ESP_LOGV("BNO_SENSOR", "Magnetometer vector - X: %.3f, Y: %.3f, Z: %.3f", imu->magnetometer.x, imu->magnetometer.y, imu->magnetometer.z);
        break;

    case GYROSCOPE:
        imu->gyroscope.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.gyro;
        imu->gyroscope.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.gyro;
        imu->gyroscope.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.gyro;
        ESP_LOGV("BNO_SENSOR", "Gyroscope vector - X: %.3f, Y: %.3f, Z: %.3f", imu->gyroscope.x, imu->gyroscope.y, imu->gyroscope.z);
        break;

    case EULER_ANGLE:
        imu->euler_angles.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.euler;
        imu->euler_angles.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.euler;
        imu->euler_angles.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.euler;
        ESP_LOGV("BNO_SENSOR", "Euler vector - Yaw: %.3f, Pitch: %.3f, Roll: %.3f", imu->euler_angles.x, imu->euler_angles.y, imu->euler_angles.z);
        break;

    case QUATERNION:
        imu->quaternion.w = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.quat;
        imu->quaternion.x = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.quat;
        imu->quaternion.y = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.quat;
        imu->quaternion.z = (int16_t)((register_content[7] << 8) | register_content[6]) / imu->bno055_config.sensor_scale.quat;
        ESP_LOGV("BNO_SENSOR", "Quaternion vector - W: %.3f, X: %.3f, Y: %.3f, Z: %.3f", imu->quaternion.w, imu->quaternion.x, imu->quaternion.y, imu->quaternion.z);
        break;

    case LINEAR_ACCELERATION:
        imu->linear_acceleration.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.accel;
        imu->linear_acceleration.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.accel;
        imu->linear_acceleration.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.accel;
        ESP_LOGV("BNO_SENSOR", "Linear acceleration vector - X: %.3f, Y: %.3f, Z: %.3f", imu->linear_acceleration.x, imu->linear_acceleration.y, imu->linear_acceleration.z);
        break;

    case GRAVITY:
        imu->gravity.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.accel;
        imu->gravity.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.accel;
        imu->gravity.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.accel;
        ESP_LOGV("BNO_SENSOR", "Gravity vector - X: %.3f, Y: %.3f, Z: %.3f", imu->gravity.x, imu->gravity.y, imu->gravity.z);
        break;

    case TEMPERATURE:
        imu->temperature = (int8_t)register_content[0] / imu->bno055_config.sensor_scale.temp;
        ESP_LOGV("BNO_SENSOR", "Temperature - %.3f", imu->temperature);
        break;

    default:
        ESP_LOGE("BNO_SENSOR", "Invalid sensor type");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t bno055_get_offsets(i2c_master_dev_handle_t *slave_handle, imu_t *imu)
{
    esp_err_t ret;
    uint8_t register_address = ACC_OFFSET_X_LSB, register_content[18];
    for (size_t i = 0; i < 6; i++)
    {
        ret = i2c_master_transmit_receive(*slave_handle, &register_address, sizeof(register_address), &register_content[i], sizeof(register_content[i]), pdMS_TO_TICKS(1000));
        if (ret != ESP_OK)
        {
            ESP_LOGE("I2C", "Failed to read from register '0x%x'. Error: %s", register_address, esp_err_to_name(ret));
            return ret;
        }
        register_address++;
    }

    imu->bno055_config.offsets.accel.x = (int16_t)((register_content[1] << 8) | register_content[0]) / imu->bno055_config.sensor_scale.accel;
    imu->bno055_config.offsets.accel.y = (int16_t)((register_content[3] << 8) | register_content[2]) / imu->bno055_config.sensor_scale.accel;
    imu->bno055_config.offsets.accel.z = (int16_t)((register_content[5] << 8) | register_content[4]) / imu->bno055_config.sensor_scale.accel;
    ESP_LOGV("BNO_SENSOR", "Accel offset vector - X: %.3f, Y: %.3f, Z: %.3f", imu->bno055_config.offsets.accel.x, imu->bno055_config.offsets.accel.y, imu->bno055_config.offsets.accel.z);

    imu->bno055_config.offsets.mag.x = (int16_t)((register_content[7] << 8) | register_content[6]) / imu->bno055_config.sensor_scale.mag;
    imu->bno055_config.offsets.mag.y = (int16_t)((register_content[9] << 8) | register_content[8]) / imu->bno055_config.sensor_scale.mag;
    imu->bno055_config.offsets.mag.z = (int16_t)((register_content[11] << 8) | register_content[10]) / imu->bno055_config.sensor_scale.mag;
    ESP_LOGV("BNO_SENSOR", "Mag offset vector - X: %.3f, Y: %.3f, Z: %.3f", imu->bno055_config.offsets.mag.x, imu->bno055_config.offsets.mag.y, imu->bno055_config.offsets.mag.z);

    imu->bno055_config.offsets.gyro.x = (int16_t)((register_content[13] << 8) | register_content[12]) / imu->bno055_config.sensor_scale.gyro;
    imu->bno055_config.offsets.gyro.y = (int16_t)((register_content[15] << 8) | register_content[14]) / imu->bno055_config.sensor_scale.gyro;
    imu->bno055_config.offsets.gyro.z = (int16_t)((register_content[17] << 8) | register_content[16]) / imu->bno055_config.sensor_scale.gyro;
    ESP_LOGV("BNO_SENSOR", "Gyro offset vector - X: %.3f, Y: %.3f, Z: %.3f", imu->bno055_config.offsets.gyro.x, imu->bno055_config.offsets.gyro.y, imu->bno055_config.offsets.gyro.z);

    return ESP_OK;
}

esp_err_t bno055_set_offsets(i2c_master_dev_handle_t *slave_handle, imu_t *imu)
{
    return ESP_OK;
}

esp_err_t bno055_reset(i2c_master_dev_handle_t *slave_handle, imu_t *imu)
{
    esp_err_t ret = gpio_set_level(imu->bno055_config.reset_io, 0);
    if (ret != ESP_OK)
        return ret;
    vTaskDelay(pdMS_TO_TICKS(100));
    ret = gpio_set_level(imu->bno055_config.reset_io, 1);
    return ESP_OK;
}

esp_err_t bno055_set_axis(i2c_master_dev_handle_t *slave_handle, uint8_t axis_remap, uint8_t axis_sign)
{
    return ESP_OK;
}

esp_err_t bno055_power_mode_set(i2c_master_dev_handle_t *slave_handle, bno055_power_mode_t power_mode)
{
    return ESP_OK;
}