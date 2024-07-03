/** @file ESR--I2C-GY291.c
 *  @brief Project main-file: Requesting multiple data from ADXL345 acceleration sensor
 *  @author Sven Steddin
 *  @date 2019-06-18
 *
 *  This project has been derived from TI sample code projects
 *  MSP430ware v:3.80.07.00 (2019-06-15):
 *  - eusci_b_i2c_ex3_masterTxMultiple
 *  - eusci_b_i2c_ex1_masterRxMultiple
 *
 *
 */
/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//! This example shows how to configure the I2C module as a master for
//! multiple byte transmission and reception in order to configure control
//! registers and to read sensor data of a ADXL345 acceleration sensor on a
//! GY291 sensor board.
//!
//! The example has been modified to run an a MSP430FR2355 Launchpad.
//! The example uses Ti Driverlib functions, which is supplemented by a
//! function to make use of the transmission byte counter of the MSP430FR2355
//!
//! Purpose:
//! - Modify control registers of ADXL345 in order to switch it from standby to
//!   measurement mode (see datasheet)
//! - Repeatedly read samples of sensory data in x-, y- and z-direction
//!
//! Intended use:
//! 1. Connect ADXL sensor board to MSP430FR2355 according to the diagram below
//! 2. Read sensor data from array sensorData in debug mode
//!
//! Description:
//! The USCI_B0 unit is used to read and write data to an I2C slave in
//! interrupt mode. The byte counter function of the USCI_B0 is used to
//! generate a stop condition. The byte counter is loaded prior to the
//! start of the transmission and decremented automatically on each byte
//! which is sent or received. Write and read operations are implemented
//! by interrupt service routines.
//!
//! Pullup-resistors are already installed on the sensor board GY291, thus no
//! external pullup resistors are required.
//! The Launchpad drives the GY291 power supply
//! I2C-Address of GY291: 0x53
//! ACLK = n/a, MCLK = SMCLK = BRCLK =  DCO = ~1MHz
//!
//!                                /|\  /|\
//!               MSP430FR2355     4,7k 4,7k  GY291 - ADXL345
//!               Launchpad         |    |    sensorboard
//!             -----------------   |    |   ------------
//!            |     P1.2/UCB0SDA|<-|----+->|SDA         |
//!            |                 |  |       |            |
//!            |                 |  |       |            |
//!            |     P1.3/UCB0SCL|<-+------>|SCL         |
//!            |                 |          |            |
//!            |                 |          |            |
//!            |             Vcc |--------->|Vcc (3,3V)  |
//!            |             GND |--------->|GND         |
//!
//! Warning: Example program only covers the happy path of coding;
//!          no error cases are regarded
//!
//! written by S. Steddin (2019-06-18)
//!
//
//*****************************************************************************
#include "driverlib.h"
#include "Board.h"


//*****************************************************************************
//
//Set the address for slave module. This is a 7-bit address sent in the
//following format:
//[A6:A5:A4:A3:A2:A1:A0:RS]
//
//A zero in the "RS" position of the first byte means that the master
//transmits (sends) data to the selected slave, and a one in this position
//means that the master receives data from the slave.
//
//*****************************************************************************
#define SLAVE_ADDRESS 0x53

//*****************************************************************************
//
//Target frequency for SMCLK in kHz
//
//*****************************************************************************
#define CS_SMCLK_DESIRED_FREQUENCY_IN_KHZ   1000

//*****************************************************************************
//
//SMCLK/FLLRef Ratio
//
//*****************************************************************************
#define CS_SMCLK_FLLREF_RATIO   30


//*****************************************************************************
//
//Init of the MSP430 peripherals required
//
//*****************************************************************************
void InitMSP430Peripherals(void);

 //*****************************************************************************
 //
 //Loading of the USCI_B byte counter
 //
 //*****************************************************************************
void EUSCI_B_I2C_masterSetByteCnt(uint16_t baseAddress, uint16_t cnt);

//*****************************************************************************
//
//ADXL345 measurement data (read in 2's complement, little endian format
//
//*****************************************************************************
typedef struct {
    int16_t xAxis;
    int16_t yAxis;
    int16_t zAxis;
} sensorData_t;
sensorData_t gi_sensorData;     // as sensor data is read byte by byte, typecast to
                                // into uint8_t is required on read operation
uint8_t * gp_rxData;            // pointer to sensory data used in I2C_RX isr


uint8_t txArr[32];              // data array for sending single or multiple control
                                // register data to the ADXL sensor
uint8_t * gp_txData;            // pointer to control register data used in I2C_TX isr


uint16_t rxcounter = 0;         // used for debugging



void gy291_main (void)
{
    // Stop WDT
    WDT_A_hold(WDT_A_BASE);     // stop watchdog
    InitMSP430Peripherals();    // setup clock unit and I2C peripheral

    // Switch ADXL345 from standby mode to measurement mode:
    txArr[0] = 0x2D;                // address of POWER_CNT control register
    txArr[1] = 0x08;                // data for POWER_CNT register
    gp_txData = (uint8_t*) txArr;   // set data pointer to top of control data array

    // Request 2 data bytes to be transmitted (address + data)
    EUSCI_B_I2C_masterSetByteCnt(EUSCI_B0_BASE, 2); // thereby reset USCI_B I2C interface

    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
            EUSCI_B_I2C_NAK_INTERRUPT +
            EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
            );
    // Enable master Transmit interrupt
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
            EUSCI_B_I2C_NAK_INTERRUPT +
            EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
            );

    // Start transmission of control register data by issuing of start condition and
    // register address of ADXL345 sensor
    EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);

    // Wait until TX interrupt occurs, indicating that first byte can be transferred
    // to the TX transmission register; program control returns as soon as any bytes
    // are transferred according to the initial byte count setting
    LPM0;

    // End of initialization

    while (1) {
        // Since program control returns, as soon as the last byte is transferred to the
        // TX transmission register, it is required to wait until the last byte has really
        // been transmitted. This is done by polling the stop condition:
        // (warning: this is not fail safe; on error, the program will be stuck in an endless loop!)
        while (EUSCI_B_I2C_SENDING_STOP ==
                EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));


        txArr[0] = 0x32;                // set ADXL345 address pointer to X-Axis Data 0 register: 0x32
        gp_txData = (uint8_t*) txArr;   // set data pointer to top of control data array
        // Request 1 data byte to be transmitted (address)
        EUSCI_B_I2C_masterSetByteCnt(EUSCI_B0_BASE, 1);

        EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_enable(EUSCI_B0_BASE);
        EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                EUSCI_B_I2C_NAK_INTERRUPT +
                EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                );
        // Enable master Transmit interrupt
        EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                EUSCI_B_I2C_NAK_INTERRUPT +
                EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT
                );
        // Start transmission of control register data by issuing of start condition and
        // register address of ADXL345 sensor
        EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);

        // Wait until address value is transmitted
        LPM0;

        // Make sure, that transmission has been completed
        while (EUSCI_B_I2C_SENDING_STOP ==
                EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));


        rxcounter = 0;
        gp_rxData = (uint8_t*) &gi_sensorData;  // set data pointer to the top of the sensor data array

        // Request 6 data bytes to be read (sensor data)
        EUSCI_B_I2C_masterSetByteCnt(EUSCI_B0_BASE, 6);

        EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_enable(EUSCI_B0_BASE);

        EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
            EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT
            );

        // Enable master Receive interrupt
        EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
            EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
            EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
            EUSCI_B_I2C_NAK_INTERRUPT
            );
        // Start reading of sensor data by issuing of start condition and
        // register address of ADXL345 sensor
        EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);

        // Wait until data of x-, y- and z-axis has been transferred
        LPM0;

        // Make sure, that transmission has been completed
        while (EUSCI_B_I2C_SENDING_STOP ==
                EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));

        _no_operation();    // set breakpoint here, in order to check sensor data
    }
}


void InitMSP430Peripherals(void) {
    //Set Ratio and Desired MCLK Frequency  and initialize DCO
    CS_initFLLSettle(
        CS_SMCLK_DESIRED_FREQUENCY_IN_KHZ,
        CS_SMCLK_FLLREF_RATIO
        );

    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(
            CS_SMCLK,
            CS_DCOCLKDIV_SELECT,
            CS_CLOCK_DIVIDER_1
            );

    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(
            CS_MCLK,
            CS_DCOCLKDIV_SELECT,
            CS_CLOCK_DIVIDER_1
            );

    // Configure Pins for I2C

    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_UCB0SCL,
        GPIO_PIN_UCB0SCL,
        GPIO_FUNCTION_UCB0SCL
    );

    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_UCB0SDA,
        GPIO_PIN_UCB0SDA,
        GPIO_FUNCTION_UCB0SDA
    );


    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
        SLAVE_ADDRESS
        );

    __bis_SR_register(GIE);
}

void EUSCI_B_I2C_masterSetByteCnt(uint16_t baseAddress, uint16_t cnt) {
    //Disable the USCI module and clears the other bits of control register
    HWREG16(baseAddress + OFS_UCBxCTLW0) |= UCSWRST;
    //Byte Count Threshold
    HWREG16(baseAddress + OFS_UCBxTBCNT) = cnt;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    // one interrupt vector for any I2C events:
    // on interrupt, interrupt source has to be determined
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
    {
        case USCI_NONE:             // No interrupts break;
            break;
        case USCI_I2C_UCALIFG:      // Arbitration lost
            break;
        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
            EUSCI_B_I2C_masterReceiveStart(EUSCI_B0_BASE);
            break;
        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
            break;
        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
            break;
        case USCI_I2C_UCRXIFG3:     // RXIFG3
            break;
        case USCI_I2C_UCTXIFG3:     // TXIFG3
            break;
        case USCI_I2C_UCRXIFG2:     // RXIFG2
            break;
        case USCI_I2C_UCTXIFG2:     // TXIFG2
            break;
        case USCI_I2C_UCRXIFG1:     // RXIFG1
            break;
        case USCI_I2C_UCTXIFG1:     // TXIFG1
            break;
        case USCI_I2C_UCRXIFG0:     // RXIFG0
            // Get RX data
            *gp_rxData++ = EUSCI_B_I2C_masterReceiveSingle(EUSCI_B0_BASE);
            rxcounter++;
            break; // Vector 24: RXIFG0 break;
        case USCI_I2C_UCTXIFG0:     // TXIFG0
            //Set TX data
            EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, *gp_txData);
            gp_txData++;
            break;
        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
            // Exit LPM0
            // Since the interrupt occurs shortly before the end of the transmission,
            // main program has to check the end of the transmission, e.g. by polling
            // the stop bit in USCI_Bx_CTLW0 control register
            __bic_SR_register_on_exit(CPUOFF);
            break;
        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
            break;
        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
            break;
        default:
            break;
    }
}

