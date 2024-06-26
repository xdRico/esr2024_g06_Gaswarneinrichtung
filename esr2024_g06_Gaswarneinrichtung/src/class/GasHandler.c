/*
 * GasHandler.c
 *
 *  Created on: 17.06.2024
 *      Author:
 *
 *  This Class implements the IGasHandler.h header file.
 *  It uses Pin 32 (P3.3) for activating the Gas Sensor,
 *  Pin 30 (P3.1) for the digital input value and
 *  Pin 25 (P5.1) for the analog input value.
 */

#include <IGasHandler.h>


/**
 * Implementation of IGasHandler.h
 */
void preInitGasHandler(){

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN1);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3);

    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
}


/**
 * Implementation of IGasHandler.h
 */
void initGasHandler(){
    setBurnerActive(true);
}


/**
 * Implementation of IGasHandler.h
 */
void measure(){
    //TODO
}

/**
 * Implementation of IGasHandler.h
 */
bool isValueCritical(){
    //TODO
    return true;
}

/**
 * Implementation of IGasHandler.h
 */
void setBurnerActive(bool active){
    if(active)
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN3);
    else
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
}

