/*
 * SHTC3 Temperature and humidity sensor I2C driver header
 * Author: Carson Elliott
 * Date: May 20 2024
 */

#ifndef SHTC3_H
#define SHTC3_H

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_i2c.h"
#include <stdint.h>

// Defines
#define SHTC3_I2C_ADDR (0x70 << 1) // 8-bit I2C address
#define SHTC3_ID 0xEFC8

// Measurement Commands
#define SHTC3_REG_TF_NMCSE 0x7CA2
#define SHTC3_REG_RHF_NMCSE 0x5C24
#define SHTC3_REG_TF_NMCSD 0x7866
#define SHTC3_REG_RHF_NMCSD 0x58E0
#define SHTC3_REG_TF_LPCSE 0x6458
#define SHTC3_REG_RHF_LPCSE 0x44DE
#define SHTC3_REG_TF_LPCSD 0x609C
#define SHTC3_REG_RHF_LPCSD 0x401A

#define SHTC3_READID 0xEFC8
#define SHTC3_SOFTRESET 0x805D
#define SHTC3_SLEEP 0xB098
#define SHTC3_WAKEUP 0x3517

typedef struct {
    I2C_HandleTypeDef* i2cHandle;
    float temp;
    float rh;
} SHTC3;

HAL_StatusTypeDef SHTC3_Read_Temperature_Humidity(SHTC3* dev);
HAL_StatusTypeDef SHTC3_Init(SHTC3* dev, I2C_HandleTypeDef* i2cHandle);
HAL_StatusTypeDef SHTC3_Read_Temperature(SHTC3* dev);
HAL_StatusTypeDef SHTC3_Read_Humidity(SHTC3* dev);
HAL_StatusTypeDef SHTC3_ReadRegister(SHTC3* dev, uint8_t reg, uint8_t* data);
HAL_StatusTypeDef SHTC3_ReadRegisters(SHTC3* dev, uint8_t reg, uint8_t* data, uint8_t len);
HAL_StatusTypeDef SHTC3_WriteRegister(SHTC3* dev, uint8_t reg, uint8_t* data);

uint8_t CalculateCRC(uint8_t *data, uint8_t length);

#endif // SHTC3_H
