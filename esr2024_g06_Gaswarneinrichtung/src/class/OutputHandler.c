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


bool isOn = false;
bool warningState = false;
bool alarmState = false;


/**
 * Implementation of IOutputHandler.h
 */
void preinit(){
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

}


/**
 * Implementation of IOutputHandler.h
 */
void init(){
    __no_operation();                   // For debug
}


/**
 * Implementation of IOutputHandler.h
 */
void setOnState(bool on){
    isOn = on;
}


/**
 * Implementation of IOutputHandler.h
 */
void setWarningState(bool warning){
    warningState = warning;
}


/**
 * Implementation of IOutputHandler.h
 */
void setAlarmState(bool alarm){
    alarmState = alarm;
}


/**
 * Implementation of IOutputHandler.h
 */
void mainRoutine(){
    int toImplement; //to Implement
}
