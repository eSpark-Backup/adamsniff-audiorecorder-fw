#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_device.h"
#include "io_expander_i2c.h"

I2C_TypeDef *i2c = IO_EXPANDER_DEVICE;

void I2Cdev_Init()
{
    // Enable clock for I2C and GPIO
    CMU_ClockEnable(cmuClock_I2C0, true); // Replace cmuClock_I2C0 if using a different I2C instance
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Configure PC0 (SDA) and PC1 (SCL) as open-drain outputs with pull-ups
    GPIO_PinModeSet(gpioPortC, 0, gpioModeWiredAndPullUp, 1); // PC0 -> SDA
    GPIO_PinModeSet(gpioPortC, 1, gpioModeWiredAndPullUp, 1); // PC1 -> SCL

    // Clear any previous routes and set the routing for SDA and SCL
    i2c->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN;
    i2c->ROUTE = (i2c->ROUTE & (~_I2C_ROUTE_LOCATION_MASK)) | I2C_ROUTE_LOCATION_LOC4;

    // Initialize the I2C settings
    I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

    // Initialize the I2C peripheral
    I2C_Init(i2c, &i2cInit);

    // Enable the I2C bus
    I2C_Enable(i2c, true);
}

int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef result;

    // Configure transfer sequence
    seq.addr = devAddr << 1;
    seq.flags = I2C_FLAG_WRITE_READ;

    // Set up buffers
    uint8_t regBuffer[1] = { regAddr };
    seq.buf[0].data = regBuffer;
    seq.buf[0].len = 1;

    seq.buf[1].data = data;
    seq.buf[1].len = length;

    // Perform I2C transfer
    result = I2C_TransferInit(i2c, &seq);
    while (result == i2cTransferInProgress)
    {
        result = I2C_Transfer(i2c);
    }

    // Return number of bytes read if successful, 0 otherwise
    return (result == i2cTransferDone) ? length : 0;
}

int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data)
{
    return I2Cdev_readBytes(devAddr, regAddr, 1, data);
}

int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = I2Cdev_readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

int8_t I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    I2C_TransferSeq_TypeDef seq;
    I2C_TransferReturn_TypeDef result;

    // Configure transfer sequence
    seq.addr = devAddr << 1;
    seq.flags = I2C_FLAG_WRITE;

    // Create a buffer for register address and data
    uint8_t buffer[length + 1];
    buffer[0] = regAddr;
    for (uint8_t i = 0; i < length; i++)
    {
        buffer[i + 1] = data[i];
    }

    seq.buf[0].data = buffer;
    seq.buf[0].len = length + 1;

    // Perform I2C transfer
    result = I2C_TransferInit(i2c, &seq);
    while (result == i2cTransferInProgress)
    {
        result = I2C_Transfer(i2c);
    }
    
    // Return number of written read if successful, 0 otherwise
    return (result == i2cTransferDone) ? length : 0;
}

int8_t I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    return I2Cdev_writeBytes(devAddr, regAddr, 1, &data);
}

bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    I2Cdev_readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2Cdev_writeByte(devAddr, regAddr, b);
}
