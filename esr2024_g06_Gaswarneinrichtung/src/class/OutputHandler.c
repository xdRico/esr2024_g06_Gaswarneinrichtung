/** @file OutputHandler.c
 *  @brief This Class implements the IOutputHandler.h header file.
 *         It uses the two onboard LEDs of the MSP430FR2355
 *         as well as a Piezo speaker on Pin 19 (P2.0).
 *  @author Gruppe 6
 *  @date 2024-06-17
 */

#include <IOutputHandler.h>


/**
 * Implementation of IOutputHandler.h
 */
void preInitOutputHandler(){
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
}




/**
 * Implementation of IOutputHandler.h
 */
void initOutputHandler(){
    __no_operation();                   // For debug
}

//********************************************************************************************************/
/*! @fn             void setOnLED(bool on)
 *  @brief          Sets the State of the On-LED

 *  @param [on]     the state to set the On-LED
 *
 ************************************************************************************************************/

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

void togglePiezo(unsigned int frequency) {
    unsigned int delayTime = 500000 / frequency; // Calculate half period for given frequency in microseconds
    P3OUT ^= PIEZO_PIN;   // Toggle Piezo pin
    delayMicroseconds(delayTime); // Half period delay
}


/**
 * Activates the Piezo for beeping
 *
 *  args:
 *
 */
void activateAlertPiezo(bool accelerationAlert){
    if (accelerationAlert){


    }

}
