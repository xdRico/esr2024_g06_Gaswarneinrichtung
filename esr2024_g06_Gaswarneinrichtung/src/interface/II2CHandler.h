/*
 * II2CHandler.h
 *
 *  Created on: 30 Jun 2024
 *      Author: Olawale
 */

#include <stdint.h>
#ifndef SRC_INTERFACE_II2CHANDLER_H_
#define SRC_INTERFACE_II2CHANDLER_H_


#define SLAVE_ADDRESS 0x53 // ADXL345 I2C address

void i2c_init(void);
void i2c_write_byte(uint8_t reg, uint8_t data);
uint8_t i2c_read_byte(uint8_t reg);

#endif /* SRC_INTERFACE_II2CHANDLER_H_ */
