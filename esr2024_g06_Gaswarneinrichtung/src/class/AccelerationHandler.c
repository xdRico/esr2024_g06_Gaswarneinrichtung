/** @file AccelerationHandler.c
 *  @brief  This Class implements the IAccelerationHandler.h header file.
 *          It uses Pin 9 (P1.3) and 10 (P1.2) for the I2C connection,
 *          Pin 31 (P3.2) for activating/deactivating the sensor
 *          and Pin 29 (P3.5) for the digital value input.
 *  @author
 *  @date 2024-06-17
 */

#include <IAccelerationHandler.h>
#include "II2CHandler.h"


/**
 * Implementation of IAccelerationHandler
 */
void preInitAccelerationHandler(){

    /**< Configure P?? as output*/

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);
}


/**
 * Implementation of IAccelerationHandler
 */
void initAccelerationHandler(){
    i2c_write_byte(0x2D, 0x00);  /**< Power control: reset all bits*/
    i2c_write_byte(0x2D, 0x08);  /**< Power control: measure mode*/
    i2c_write_byte(0x2E, 0x40);  /**< Enable single tap interrupt*/
    i2c_write_byte(0x1D, 0x30);  /**< Tap threshold: 0x30 -> 3 g*/
    i2c_write_byte(0x21, 0x10);  /**< Tap duration: 0x10 (example)*/
    i2c_write_byte(0x22, 0x10);  /**< Tap latency: 0x10 (example)*/
    i2c_write_byte(0x23, 0x30);  /**< Tap window: 0x30 (example)*/
    i2c_write_byte(0x2A, 0x40);  /**< Single tap interrupt map to INT1*/
}


/**
 * Implementation of IAccelerationHandler
 */
void postInitAccelerationHandler(){
    GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);

    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);

    GPIO_clearInterrupt(GPIO_PORT_P3, GPIO_PIN5);
}


/**
 * Implementation of IAccelerationHandler
 */
void setAccelerationSensorActive(bool active){
    if(active){
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN2);
        preInitAccelerationHandler();
        initAccelerationHandler();
        postInitAccelerationHandler();
    /* reInit??*/
    }else
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);
}
