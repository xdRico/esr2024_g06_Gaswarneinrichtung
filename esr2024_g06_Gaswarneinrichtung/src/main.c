#include <msp430.h> 
#include <IModule.h>
#include "IOutputHandler.h"
#include "IAccelerationHandler.h"
#include "IGasHandler.h"


/**
 * main.c
 *
 * The main method gets called at start of the device
 */
int main(void){

    preInit();

    init();


    while(getOnState() == true){
        loop();
    }
    return 0;
}


/**
 * Implementation of IModule.h
 */
void preInit(){
    //preInit device

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer



    //preInit Modules

    preInitOutputHandler();
    //preInitAccelerationHandler();
    //preInitGasHandler();
}


/**
 * Implementation of IModule.h
 */
void init(){
    //init Device



    //init Modules

    initOutputHandler();
    //initAccelerationHandler();
    //initGasHandler();
}


/**
 * loop():
 *
 * The loop method gets called repeatedly, until the device gets turned off
 */
void loop(){
    __no_operation();
}
