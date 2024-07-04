/** @file OutputHandler.c
 *  @brief This Class implements the IOutputHandler.h header file.
 *         It uses the two onboard LEDs of the MSP430FR2355
 *         as well as a Piezo speaker on Pin 3 (P1.6).
 *         The Piezo uses Timers B0 CCR0 and B0 CCR1 as well as WatchdogTimer A.
 *  @author Gruppe 6
 *  @date 2024-06-17
 */

#include <IOutputHandler.h>


#define HALBTON_TIEFER(wert) (unsigned int)((wert*69432L)>>16)
#define HALBTON_HOEHER(wert) (unsigned int)((wert*61858L)>>16)

#define a1 2400

#define h1 HALBTON_HOEHER(HALBTON_HOEHER(a1))
#define c2 HALBTON_HOEHER(h1)

#define g1 HALBTON_TIEFER(HALBTON_TIEFER(a1))
#define f1 HALBTON_TIEFER(HALBTON_TIEFER(g1))
#define e1 HALBTON_TIEFER(f1)
#define d1 HALBTON_TIEFER(HALBTON_TIEFER(e1))
#define c1 HALBTON_TIEFER(HALBTON_TIEFER(d1))

const unsigned int cTonleiter[] = {d1, f1};
unsigned int idx = 0;

Timer_B_initUpModeParam param = {0};


/**
 * Implementation of IOutputHandler.h
 */
void preInitOutputHandler(){
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P2,
            GPIO_PIN6 + GPIO_PIN7,
            GPIO_SECONDARY_MODULE_FUNCTION
    );


    //Ausgangspin von Timer0_B3, CCR1 (TB0.1 - Pin 1.6)
    //statt als GPIO-Pin zu verwenden, mit Timer0_B3 CCR1 Output verbinden:
    //secondary function fr das Pin aktivieren
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN6,
            GPIO_SECONDARY_MODULE_FUNCTION
    );



}


/**
 * Implementation of IOutputHandler.h
 */
void initOutputHandler(){
    __no_operation();                   // For debug
}


/**
 * Implementation of IOutputHandler.h
 */
void postInitOutputHandler(){
    //Grundperiode von Timer0_B3 im Up-Mode festlegen: CCR0 einstellen
        //Beachte: Wenn Zhler im Up-Mode betrieben wird, so kann CCR0 nicht fr die
        //Ausgabe eines Digitalsignals verwendet werden (also auf ein Outputpin gelegt
        //werden, sondern definiert die Grundperiode.
        //Das Ausgabesignal wird stattdessen mit den Output-Units von CCR1 oder CCR2 erzeugt,
        //deren Zhlerwert beim SET-RESET Mode das PWM-Tastverhltnis festlegt.

        //Einstellungen fr die Grundperiode ber CCR0:
        param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
        param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
        param.timerPeriod = c1;
        param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
        param.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
        param.timerClear = TIMER_B_DO_CLEAR;
        param.startTimer = false;
        Timer_B_initUpMode(TIMER_B0_BASE, &param);

        //Einstellungen fr das 50%-PWM Lautsprechersignal am Ausgang, der CCR1
        //zugeordnet ist:
        Timer_B_initCompareModeParam param1 = {0};
        param1.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_1;
        param1.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
        param1.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
        param1.compareValue = c1>>1;
        Timer_B_initCompareMode(TIMER_B0_BASE, &param1);



        //Initialize WDT module in timer interval mode,
        //with ACLK as source at an interval of 1000 ms.
        WDT_A_initIntervalTimer(WDT_A_BASE,
            WDT_A_CLOCKSOURCE_ACLK,
            WDT_A_CLOCKDIVIDER_32K);

        WDT_A_start(WDT_A_BASE);

        //Enable Watchdog Interupt
        SFR_clearInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);
        SFR_enableInterrupt(SFR_WATCHDOG_INTERVAL_TIMER_INTERRUPT);

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

/**
 * Implementation of IOutputHandler.h
 */
void setPiezoActive(bool alert){
    if(alert){
        param.startTimer = true;
        Timer_B_initUpMode(TIMER_B0_BASE, &param);
        return;
    }
    param.startTimer = false;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);
}

/**
 * WDT_A_ISR()
 *
 * Interrupt Servcie Routine for the WatchdogTimer for changing signal sound frequency
 */
#pragma vector=WDT_VECTOR
__interrupt
void WDT_A_ISR (void)
{
    if (idx == 0)
        idx++;
    else
        idx=0;

    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, cTonleiter[idx]);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_1, cTonleiter[idx]>>1);
}




