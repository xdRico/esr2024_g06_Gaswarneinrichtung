/*
 * OutputHandler.c
 *
 *  Created on: 17 Jun 2024
 *      Author: ricow
 *
 *  This Class implements the IOutputHandler.h header file.
 *  It uses the two onboard LEDs of the MSP430FR2355 as well as a Piezo speaker on Pin 19 (P2.0).
 *
 * */
#include <IOutputHandler.h>


/**
 * Implementation of IOutputHandler.h
 */
void preInitOutputHandler(){
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
}


/**
 * Implementation of IOutputHandler.h
 */
void initOutputHandler(){
    __no_operation();                   // For debug
}


/**
 * Sets the State of the On-LED
 *
 *  args:
 *      on: the state to set the On-LED
 */
void setOnLED(bool on){
    if(on)
        GPIO_setOutputHighOnPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    else
        GPIO_setOutputLowOnPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
}


/**
 * Sets the State of the warning-LED
 *
 *  args:
 *      on: the state to set the warning-LED
 */
void setAlertLED(bool alert){
    if(alert)
        GPIO_setOutputHighOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    else
        GPIO_setOutputLowOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
}


/**
 * Implementation of IOutputHandler.h
 */
void mainRoutineOutputHandler(){

    //TODO Implement
}
