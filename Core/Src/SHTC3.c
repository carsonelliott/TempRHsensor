/*
 * SHTC3 Temperature and humidity sensor I2C driver
 * Author: Carson Elliott
 * Date: May 20 2024
 */

#include "SHTC3.h"
#include "main.h"

HAL_StatusTypeDef SHTC3_Init(SHTC3* dev, I2C_HandleTypeDef* i2cHandle) {
    dev->i2cHandle = i2cHandle; // Assign the device i2cHandle to the i2cHandle passed as parameter
    HAL_StatusTypeDef ret;		// Declare status variable

    // Split the wakeup command 0x3517 in to two separate 8 bit values 0x35 and 0x17
    uint8_t wakeup_cmd[2] = { SHTC3_WAKEUP >> 8, SHTC3_WAKEUP & 0xFF };

    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, wakeup_cmd, 2, HAL_MAX_DELAY); // Transmit command to sensor
    if (ret != HAL_OK) { // Check return value
        return ret;
    }

    uint8_t read_id_cmd[2] = { SHTC3_READID >> 8, SHTC3_READID & 0xFF };
    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, read_id_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    uint8_t id[3]; // 2 bytes for ID, 1 byte for CRC
    ret = HAL_I2C_Master_Receive(dev->i2cHandle, SHTC3_I2C_ADDR, id, 3, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    // Add CRC check here if necessary
    return HAL_OK;
}

HAL_StatusTypeDef SHTC3_Read_Temperature_Humidity(SHTC3* dev) {
    HAL_StatusTypeDef ret;
    uint8_t wakeup_cmd[2] = { SHTC3_WAKEUP >> 8, SHTC3_WAKEUP & 0xFF };
    uint8_t temp_hum_cmd[2] = { SHTC3_REG_TF_NMCSE >> 8, SHTC3_REG_TF_NMCSE & 0xFF };
    uint8_t data[6];

    // Wake up the sensor
    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, wakeup_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    // Send the temperature and humidity measurement command
    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, temp_hum_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    // Wait for the measurement to complete
    HAL_Delay(15);

    // Receive the data from the sensor
    ret = HAL_I2C_Master_Receive(dev->i2cHandle, SHTC3_I2C_ADDR, data, 6, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    // Extract the raw temperature and humidity values and their CRCs
    uint16_t raw_temperature = (data[0] << 8) | data[1]; // Combine the two 8-bit bytes received from the sensor into a single 16-bit integer
    uint8_t crc_temperature = data[2];
    uint16_t raw_humidity = (data[3] << 8) | data[4];
    uint8_t crc_humidity = data[5];

    // Verify the CRC of the temperature data
    if (CalculateCRC(&data[0], 2) != crc_temperature) {
        return HAL_ERROR;
    }

    // Verify the CRC of the humidity data
    if (CalculateCRC(&data[3], 2) != crc_humidity) {
        return HAL_ERROR;
    }

    // Convert the raw temperature to degrees Celsius
    dev->temp = -45 + 175 * ((float)raw_temperature / 65535.0);

    // Convert the raw humidity to percentage
    dev->rh = 100 * ((float)raw_humidity / 65535.0);

    return HAL_OK;
}




HAL_StatusTypeDef SHTC3_Read_Temperature(SHTC3* dev) {
    HAL_StatusTypeDef ret;
    uint8_t wakeup_cmd[2] = { SHTC3_WAKEUP >> 8, SHTC3_WAKEUP & 0xFF };
    uint8_t temp_cmd[2] = { SHTC3_REG_TF_NMCSE >> 8, SHTC3_REG_TF_NMCSE & 0xFF };
    uint8_t data[6];

    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, wakeup_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, temp_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    HAL_Delay(15);

    ret = HAL_I2C_Master_Receive(dev->i2cHandle, SHTC3_I2C_ADDR, data, 6, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    uint16_t raw_temperature = (data[0] << 8) | data[1];
    uint8_t crc_temperature = data[2];

    if (CalculateCRC(data, 2) != crc_temperature) {
        return HAL_ERROR;
    }

    dev->temp = -45 + 175 * ((float)raw_temperature / 65535.0);
    return HAL_OK;
}

HAL_StatusTypeDef SHTC3_Read_Humidity(SHTC3* dev) {
    HAL_StatusTypeDef ret;
    uint8_t wakeup_cmd[2] = { SHTC3_WAKEUP >> 8, SHTC3_WAKEUP & 0xFF };
    uint8_t hum_cmd[2] = { SHTC3_REG_RHF_NMCSE >> 8, SHTC3_REG_RHF_NMCSE & 0xFF };
    uint8_t data[6];

    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, wakeup_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    ret = HAL_I2C_Master_Transmit(dev->i2cHandle, SHTC3_I2C_ADDR, hum_cmd, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    HAL_Delay(15);

    ret = HAL_I2C_Master_Receive(dev->i2cHandle, SHTC3_I2C_ADDR, data, 6, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        return ret;
    }

    uint16_t raw_humidity = (data[3] << 8) | data[4];
    uint8_t crc_humidity = data[5];

    if (CalculateCRC(&data[3], 2) != crc_humidity) {
        return HAL_ERROR;
    }

    dev->rh = 100 * ((float)raw_humidity / 65535.0);
    return HAL_OK;
}

HAL_StatusTypeDef SHTC3_ReadRegister(SHTC3* dev, uint8_t reg, uint8_t* data) {
    return HAL_I2C_Mem_Read(dev->i2cHandle, SHTC3_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SHTC3_ReadRegisters(SHTC3* dev, uint8_t reg, uint8_t* data, uint8_t len) {
    return HAL_I2C_Mem_Read(dev->i2cHandle, SHTC3_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SHTC3_WriteRegister(SHTC3* dev, uint8_t reg, uint8_t* data) {
    return HAL_I2C_Mem_Write(dev->i2cHandle, SHTC3_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}


uint8_t CalculateCRC(uint8_t *data, uint8_t length) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
