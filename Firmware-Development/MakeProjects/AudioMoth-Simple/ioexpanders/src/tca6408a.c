// TCA6408A I2C device class (implementation)
// Follows the same API / coding style as the TCA6424A driver you provided.

#include <stdint.h>
#include <stdbool.h>
#include "tca6408a.h"
#include "em_gpio.h"
#include "io_expander_i2c.h" /* contains I2Cdev_* wrappers in your project */

static uint8_t devAddr_tca6408a;
static uint8_t buffer_tca6408a[1];

/* Default constructor, uses default I2C address. */
void TCA6408A_Construct_Default(void)
{
    devAddr_tca6408a = TCA6408A_DEFAULT_ADDRESS;
}

/* Specific address constructor. */
void TCA6408A_Construct(uint8_t address)
{
    devAddr_tca6408a = address;
}

/* Power on and prepare for general usage.
 * The TCA6408A defaults to all pins configured as inputs after power-on.
 * This function mirrors the style of the TCA6424A driver and enables
 * the IO expander enable pin (if present on the board).
 */
void TCA6408A_initialize(void)
{
    TCA6408A_Construct_Default();

    I2Cdev_Init();

    
    TCA6408A_setPinDirection(TCA6408A_P1, TCA6408A_OUTPUT); // P0.1 as output (GREEN LED)
    TCA6408A_setPinDirection(TCA6408A_P2, TCA6408A_OUTPUT); // P0.2 as output (RED LED)
    TCA6408A_writePin(TCA6408A_P1, TCA6408A_LOW); // turn off GREEN LED
    TCA6408A_writePin(TCA6408A_P2, TCA6408A_LOW); // turn off GREEN LED

    TCA6408A_setPinDirection(TCA6408A_P0, TCA6408A_INPUT); // P0.0 as input (mode switch)
}

/* Verify I2C connection */
bool TCA6408A_testConnection()
{
    /* Try to read the input register (1 byte). If read returns 1 -> OK */
    return I2Cdev_readBytes(devAddr_tca6408a, TCA6408A_RA_INPUT, 1, buffer_tca6408a) == 1;
}

/* INPUT register access */

/* Read single input pin (0..7) */
bool TCA6408A_readPin(uint8_t pin)
{
    I2Cdev_readBit(devAddr_tca6408a, TCA6408A_RA_INPUT, pin, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Read full 8-bit port */
uint8_t TCA6408A_readPort(void)
{
    I2Cdev_readByte(devAddr_tca6408a, TCA6408A_RA_INPUT, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Read into provided byte pointer */
void TCA6408A_readAll(uint8_t *port)
{
    I2Cdev_readBytes(devAddr_tca6408a, TCA6408A_RA_INPUT, 1, port);
}

/* OUTPUT register access */

/* Get stored output level of a single pin (flip-flop) */
bool TCA6408A_getPinOutputLevel(uint8_t pin)
{
    I2Cdev_readBit(devAddr_tca6408a, TCA6408A_RA_OUTPUT, pin, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Get output register (all 8 bits) */
uint8_t TCA6408A_getPortOutputLevel(void)
{
    I2Cdev_readByte(devAddr_tca6408a, TCA6408A_RA_OUTPUT, buffer_tca6408a);
    return buffer_tca6408a[0];
}

void TCA6408A_getAllOutputLevel(uint8_t *port)
{
    I2Cdev_readBytes(devAddr_tca6408a, TCA6408A_RA_OUTPUT, 1, port);
}

/* Set single output pin value (0/1) */
void TCA6408A_writePin(uint8_t pin, bool value)
{
    I2Cdev_writeBit(devAddr_tca6408a, TCA6408A_RA_OUTPUT, pin, value);
}

/* Write entire 8-bit output register */
void TCA6408A_writePort(uint8_t value)
{
    I2Cdev_writeByte(devAddr_tca6408a, TCA6408A_RA_OUTPUT, value);
}

void TCA6408A_writeAll(uint8_t value)
{
    buffer_tca6408a[0] = value;
    I2Cdev_writeBytes(devAddr_tca6408a, TCA6408A_RA_OUTPUT, 1, buffer_tca6408a);
}

/* POLARITY register access */

/* Get single pin polarity (0 normal, 1 inverted) */
bool TCA6408A_getPinPolarity(uint8_t pin)
{
    I2Cdev_readBit(devAddr_tca6408a, TCA6408A_RA_POLARITY, pin, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Get polarity register (all 8 bits) */
uint8_t TCA6408A_getPortPolarity(void)
{
    I2Cdev_readByte(devAddr_tca6408a, TCA6408A_RA_POLARITY, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Set single pin polarity */
void TCA6408A_setPinPolarity(uint8_t pin, bool polarity)
{
    I2Cdev_writeBit(devAddr_tca6408a, TCA6408A_RA_POLARITY, pin, polarity);
}

/* Set entire polarity register */
void TCA6408A_setPortPolarity(uint8_t polarity)
{
    I2Cdev_writeByte(devAddr_tca6408a, TCA6408A_RA_POLARITY, polarity);
}

void TCA6408A_setAllPolarity(uint8_t polarity)
{
    buffer_tca6408a[0] = polarity;
    I2Cdev_writeBytes(devAddr_tca6408a, TCA6408A_RA_POLARITY, 1, buffer_tca6408a);
}

/* CONFIG register access (direction) */

/* Single pin direction: 1=input, 0=output */
bool TCA6408A_getPinDirection(uint8_t pin)
{
    I2Cdev_readBit(devAddr_tca6408a, TCA6408A_RA_CONFIG, pin, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Get 8-bit direction/config register */
uint8_t TCA6408A_getPortDirection(void)
{
    I2Cdev_readByte(devAddr_tca6408a, TCA6408A_RA_CONFIG, buffer_tca6408a);
    return buffer_tca6408a[0];
}

/* Set direction for a single pin */
void TCA6408A_setPinDirection(uint8_t pin, bool direction)
{
    I2Cdev_writeBit(devAddr_tca6408a, TCA6408A_RA_CONFIG, pin, direction);
}

/* Set direction for whole port */
void TCA6408A_setPortDirection(uint8_t direction)
{
    I2Cdev_writeByte(devAddr_tca6408a, TCA6408A_RA_CONFIG, direction);
}

void TCA6408A_setAllDirection(uint8_t direction)
{
    buffer_tca6408a[0] = direction;
    I2Cdev_writeBytes(devAddr_tca6408a, TCA6408A_RA_CONFIG, 1, buffer_tca6408a);
}
