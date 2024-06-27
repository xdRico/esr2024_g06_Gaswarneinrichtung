/*
 * AccelerationHandler.c
 *
 *  Created on: 17.06.2024
 *      Author:
 *
 *  This Class implements the IAccelerationHandler.h header file.
 *  It uses Pin 9 (P1.3) and 10 (P1.2) for the I2C connection,
 *  Pin 31 (P3.2) for activating/deactivating the sensor
 *  and Pin 29 (P3.5) for the digital value input.
 */

#include <IAccelerationHandler.h>


/**
 * Implementation of IAccelerationHandler
 */
void preInitAccelerationHandler(){

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);
}


/**
 * Implementation of IAccelerationHandler
 */
void initAccelerationHandler(){
    _no_operation();
}


/**
 * Implementation of IAccelerationHandler
 */
void setAccelerationSensorActive(bool active){
    if(active)
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN2);
    // reInit??
    else
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);


}
