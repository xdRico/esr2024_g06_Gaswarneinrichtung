/** @file GasHandler.c
 *  @brief  This Class implements the IGasHandler.h header file.
 *          It uses Pin 32 (P3.7) for activating the Gas Sensor and
 *          Pin 30 (P3.1) for the digital input value or
 *          Pin 25 (P5.1) for the analog input value.
 *  @author ricow
 *  @date 2024-06-17
 */

#include <IGasHandler.h>


/**
 * Implementation of IGasHandler.h
 */
void preInitGasHandler(){

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN1);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);

    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
}


/**
 * Implementation of IGasHandler.h
 */
void initGasHandler(){
    setBurnerActive(true);
}


/**
 *
 */
void postInitGasHandler(){
    GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_clearInterrupt(GPIO_PORT_P3, GPIO_PIN7);

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
    return false;
}

/**
 * Implementation of IGasHandler.h
 */
void setBurnerActive(bool active){
    if(active)
        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
    else
        GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
}

