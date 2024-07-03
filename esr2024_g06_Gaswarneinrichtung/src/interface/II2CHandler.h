/** @file II2CHandler.h
 *  @brief  This Headerfile is an Interace for die I2C communikation
 *  @author Gruppe 6
 *  @date 2024-06-17
 */


#include <stdint.h>
#ifndef SRC_INTERFACE_II2CHANDLER_H_
#define SRC_INTERFACE_II2CHANDLER_H_


#define SLAVE_ADDRESS 0x53 /**< ADXL345 I2C address*/

/**
 *
 */
void i2c_init(void);

/**
 *
 */
void i2c_write_byte(uint8_t reg, uint8_t data);

/**
 *
 */
uint8_t i2c_read_byte(uint8_t reg);

#endif /**< SRC_INTERFACE_II2CHANDLER_H_ */
