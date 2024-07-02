/*
 * I2CHandler.c
 *
 *  Created on: 30 Jun 2024
 *      Author: Olawale
 */

#include <II2CHandler.h>
#include <driverlib.h>


void i2c_init(void) {
    EUSCI_B_I2C_initMasterParam i2cParams = {
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK, /**< SMCLK Clock Source*/
        12000000,                      /**< SMCLK = 12MHz*/
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS, /**< Desired I2C Clock of 400khz*/
        1,                             /**< No byte counter threshold*/
        EUSCI_B_I2C_NO_AUTO_STOP       /**< No Autostop*/
    };
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &i2cParams);

    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);
}

void i2c_write_byte(uint8_t reg, uint8_t data) {
    while (EUSCI_B_I2C_isBusBusy(EUSCI_B0_BASE));

    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg);
    EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, data);
    EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);

    while (EUSCI_B_I2C_isBusBusy(EUSCI_B0_BASE));
}

uint8_t i2c_read_byte(uint8_t reg) {
    uint8_t data = 0;

    while (EUSCI_B_I2C_isBusBusy(EUSCI_B0_BASE));

    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, reg);

    while (EUSCI_B_I2C_isBusBusy(EUSCI_B0_BASE));

    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    data = EUSCI_B_I2C_masterReceiveSingleByte(EUSCI_B0_BASE);

    return data;
}
