#ifndef _TCA6424A_I2C_H_
#define _TCA6424A_I2C_H_

#include <stdbool.h>
#include <stdint.h>

#define TCA6424A_I2C_DEVICE I2C0

void I2Cdev_Init();
int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
int8_t I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
int8_t I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);

#endif // _TCA6424A_I2C_H_