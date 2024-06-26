#include <msp430.h> 
#include <IModule.h>
#include "IOutputHandler.h"
#include "IAccelerationHandler.h"
#include "IGasHandler.h"

bool interruptP2 = false;
bool interruptP3 = false;
bool interruptP4 = false;
bool interruptRTC = false;
bool onState = false;
bool warningState = false;
bool alarmState = false;

int clk = 0;
const int intClockTime = 5; //in s/10
const int timeRestingTillAlarm = 15 * 10; //in s/10
int timeWhenOnBtnPressed = 0;
int timeWhenLastMoved = 0;

// in s/10
int clkLEDAlert = 0;
const int timeToBlinkLEDWarning = 10;
const int timeToWaitLEDWarning = 30;
const int timeToBlinkLEDAlarm = 10;
const int timeToWaitLEDAlarm = 10;

int clkLEDOn = 0;
const int timeToBlinkLEDOn = 5;
const int timeToWaitLEDOn = 50;

void interruptP2Handler();
void interruptP3Handler();
void interruptP4Handler();
void gasMeasurementHandler();


/**
 * main.c
 *
 * The main method gets called at start of the device
 * interrupts:
 *      P2: Acknowledge-Button
 *      P3: Movement/Tap from Acc-Sensor
 *      P4: On/Off-Button
 *
 *      RTC: Timing and LPM
 */
int main(void){

    preInit();

    init();


    while(onState){
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

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_S1, GPIO_PIN_S1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_S2, GPIO_PIN_S2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);

    GPIO_setOutputLowOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setOutputLowOnPin(GPIO_PORT_LED2, GPIO_PIN_LED2);

    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);


    //preInit Modules

    preInitOutputHandler();
    preInitAccelerationHandler();
    preInitGasHandler();
}


/**
 * Implementation of IModule.h
 */
void init(){
    //init Device

    //init Modules

    initOutputHandler();
    initAccelerationHandler();
    initGasHandler();
}


/**
 * Implementation of IModule.h
 */
void postInit(){
    //postInit Device
    GPIO_selectInterruptEdge(GPIO_PORT_S1, GPIO_PIN_S1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(GPIO_PORT_S2, GPIO_PIN_S2, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);
    GPIO_enableInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);

    PMM_unlockLPM5();

    GPIO_clearInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);
    GPIO_clearInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);
    GPIO_clearInterrupt(GPIO_PORT_P3, GPIO_PIN5);



    /*
     * Initialize RTC
     * RTC count reload compare value at 256.
     * 1024/32768 * 256 = 8 seconds (1024/32768 * 16 = 0.5 seconds?)
     * Source = 32kHz crystal, divided by 1024
     */
    RTC_init(RTC_BASE, 16-1, RTC_CLOCKPREDIVIDER_1024);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_XT1CLK);

    /*
     * Initialize and enable ICC
     */
    //Disable global interrupt during setup
    __bic_SR_register(GIE);

    //NOTE: ICC_LEVEL_0 is the highest priority, ICC_LEVEL3 is the lowest priority
    //Set P2 interrupt medium priority
    ICC_setInterruptLevel(ICC_ILSR_P2, ICC_LEVEL_2);
    //Set P3, P4 interrupt higher priority
    ICC_setInterruptLevel(ICC_ILSR_P3, ICC_LEVEL_3);
    ICC_setInterruptLevel(ICC_ILSR_P4, ICC_LEVEL_3);
    //Set RTC interrupt lower priority
    ICC_setInterruptLevel(ICC_ILSR_RTC_COUNTER, ICC_LEVEL_1);

    //Enable ICC module
    ICC_enable();


    //postInit Modules

}


/**
 * loop():
 *
 * The loop method gets called repeatedly, until the device gets turned off
 *  To time:
 *  On:
 *      Gasmessung
 *      LED On
 *      on Warning:
 *          LED Alarm
 *          Speaker Alarm
 *          Accelerator Sensor
 */
void loop(){

    //Enter LPM3 and global interrupt
    __bis_SR_register(LPM3_bits + GIE);
    clk += intClockTime;
    clkLEDAlert += intClockTime;
    clkLEDOn += intClockTime;

    // On-Btn
    interruptP4Handler();
    // Gas-Msrmnt
    gasMeasurementHandler();
    // Acc-Mvmnt
    interruptP3Handler();
    // Ack-Btn
    interruptP2Handler();

    if(alarmState){
        if(clkLEDAlert >= timeToBlinkLEDAlarm){
            setAlertLED(false);
            if(clkLEDAlert >= timeToBlinkLEDAlarm + timeToWaitLEDAlarm){
                clkLEDAlert = 0;
                setAlertLED(true);
            }
        }
    } else if(warningState){
        if(clkLEDAlert >= timeToBlinkLEDWarning){
            setAlertLED(false);
            if(clkLEDAlert >= timeToBlinkLEDWarning + timeToWaitLEDWarning){
                clkLEDAlert = 0;
                setAlertLED(true);
            }
        }
    } else
        setAlertLED(false);

    if(onState){
        if(clkLEDOn >= timeToBlinkLEDOn){
            setOnLED(false);
            if(clkLEDOn >= timeToBlinkLEDOn + timeToWaitLEDOn){
                clkLEDOn = 0;
                setOnLED(true);
            }
        }
    }
}


// Ack-Btn
void interruptP2Handler(){
    if(!interruptP2)
        return;
    if(alarmState){
        alarmState = false;
        timeWhenLastMoved = clk;
    }
    interruptP2 = false;
}


// Acc-Mvmnt
void interruptP3Handler(){
    if(!warningState) return;
    if(interruptP3){
        timeWhenLastMoved = clk;
        interruptP3 = false;
        return;
    }
    if(timeWhenLastMoved == 0){
        timeWhenLastMoved = clk;
        return;
    }
    if(clk > timeWhenLastMoved + timeRestingTillAlarm)
        alarmState = true;
}


// On-Btn
void interruptP4Handler(){
    if(!interruptP4){
        timeWhenOnBtnPressed = 0;
        GPIO_enableInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);
        return;
    }

    if(timeWhenOnBtnPressed == 0){
        timeWhenOnBtnPressed = clk;
        return;
    }
    if(clk < timeWhenOnBtnPressed + 25) return; //2.5s


    onState = !onState;
    GPIO_disableInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);
    interruptP4 = false;

}


// Gas-Msrmnt
void gasMeasurementHandler(){
    measure();
    if(isValueCritical()){
        if(!warningState)
            timeWhenLastMoved = clk;
        warningState = true;
    }else{
        warningState = false;
        clkLEDAlert = 0;
    }
}


// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    //Clear S2 IFG
    GPIO_clearInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);
    if(!onState) return;
    interruptP2 = true;
    //__bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3


//    //Set LED2 to indicate port interrupt
//    GPIO_setOutputHighOnPin(
//            GPIO_PORT_LED2,
//            GPIO_PIN_LED2);
//    // Keep LED2 output high for 0.5 second
//    __delay_cycles(500000);
//    //Clear LED2 output
//    GPIO_setOutputLowOnPin(
//            GPIO_PORT_LED2,
//            GPIO_PIN_LED2);
}


// Port 3 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT3_VECTOR
__interrupt void Port_3(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT3_VECTOR))) Port_3 (void)
#else
#error Compiler not supported!
#endif
{
    //Clear P3.5 IFG
    GPIO_clearInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    if(!onState) return;
    interruptP3 = true;
}


// Port 4 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) Port_4 (void)
#else
#error Compiler not supported!
#endif
{

    //Clear S1 IFG
    GPIO_clearInterrupt(GPIO_PORT_S1,GPIO_PIN_S1);
    interruptP4 = true;
    __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3

}


// RTC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(RTCIV,RTCIV_RTCIF))
    {
        case  RTCIV_NONE:
            break;          // No interrupt
        case  RTCIV_RTCIF:                  // RTC Overflow
            //Enable global interrupt
            __bis_SR_register(GIE);
            interruptRTC = true;
            __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3

//            //Set LED1 to indicate RTC interrupt
//            GPIO_setOutputHighOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
//            //Keep LED1 output high for 4 seconds
//            __delay_cycles(4000000);
//            //Clear LED1 output
//            GPIO_setOutputLowOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);

            break;
        default:
            break;
    }
}

