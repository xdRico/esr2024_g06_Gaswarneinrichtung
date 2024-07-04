/** @file OutputHandler.c
 *  @brief This Class implements the IOutputHandler.h header file.
 *         It uses the two onboard LEDs of the MSP430FR2355
 *         as well as a Piezo speaker on Pin 19 (P2.0).
 *  @author Gruppe 6
 *  @date 2024-06-17
 */

#include <IOutputHandler.h>


// Timer Cnt für 440Hz @ 1,048 MHz Takt cnt = 1,048*10^6/440 = 2381
// Kalibrierung mit Osci ergab jedoch, dass beim Zählerwert 2400
// exakt 440Hz ausgegeben werden; dies gilt aber nur für das Exemplar
// des Launchpads, auf dem die Kalibrierung durchgeführt wurde. Bei
// anderen Exemplaren kann ein anderer Wert erforderlich sein.
// Mit dem hier experimentell ermittelten Zählerwert lässt sich
// errechnen, welche Frequenz der DCO auf diesem Board tatsächlich liefert:
// fDCO = 440 Hz * 2400 = 1,056 MHz statt der erwarteten 1,048 MHz
// Dies erklärt sich dadurch, dass die FLL-Referenz des DCOs (REFO)
// nicht exakt die erwarteten 32768 Hz zur Verfügung stellt.

// Halbton tiefer: Faktor 2^(1/12) * 2^16 = 69433
// Halbton höher: Faktor 1/ 2^(1/12) * 2^16 = 61858

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



/**
 * Implementation of IOutputHandler.h
 */
void preInitOutputHandler(){
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
    //GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
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

    //Für das Clock-System werden die Default-Einstellungen übernommen.
        //D.h., das SMClk = MClk = 1,048 MHz, FLL aktiviert mit REFO als Taktgeber
        //Es müssen also keine Einstellungen am CS vorgenommen werden.

        //Entriegeln der GPIOs, damit AClk bereits starten kann
        PMM_unlockLPM5();

        //Grundperiode von Timer0_B3 im Up-Mode festlegen: CCR0 einstellen
        //Beachte: Wenn Zähler im Up-Mode betrieben wird, so kann CCR0 nicht für die
        //Ausgabe eines Digitalsignals verwendet werden (also auf ein Outputpin gelegt
        //werden, sondern definiert die Grundperiode.
        //Das Ausgabesignal wird stattdessen mit den Output-Units von CCR1 oder CCR2 erzeugt,
        //deren Zählerwert beim SET-RESET Mode das PWM-Tastverhältnis festlegt.

        //Einstellungen für die Grundperiode über CCR0:
        Timer_B_initUpModeParam param = {0};
        param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
        param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
        param.timerPeriod = d1;
        param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
        param.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
        param.timerClear = TIMER_B_DO_CLEAR;
        param.startTimer = true;
        Timer_B_initUpMode(TIMER_B0_BASE, &param);

        //Einstellungen für das 50%-PWM Lautsprechersignal am Ausgang, der CCR1
        //zugeordnet ist:
        Timer_B_initCompareModeParam param1 = {0};
        param1.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_1;
        param1.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE;
        param1.compareOutputMode = TIMER_B_OUTPUTMODE_RESET_SET;
        param1.compareValue = c1>>1;
        Timer_B_initCompareMode(TIMER_B0_BASE, &param1);



        //Initialize WDT module in timer interval mode,
        //with ACLK as source at an interval of 500 ms.
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

    togglePiezo(alert);
}


/**
 * Implementation of IOutputHandler
 */
void togglePiezo(bool alert) {

    if(!alert){
        GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6 + GPIO_PIN7);
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);

        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6 + GPIO_PIN7);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

        return;
    }

    // Configure Pins for XIN and XOUT
    //Set P2.6 and P2.7 as Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P2,
            GPIO_PIN6 + GPIO_PIN7,
            GPIO_SECONDARY_MODULE_FUNCTION
    );


    //Ausgangspin von Timer0_B3, CCR1 (TB0.1 - Pin 1.6)
    //statt als GPIO-Pin zu verwenden, mit Timer0_B3 CCR1 Output verbinden:
    //secondary function für das Pin aktivieren
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN6,
            GPIO_SECONDARY_MODULE_FUNCTION
    );

}


//Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt
void WDT_A_ISR (void)
{
    if (idx == 0) {
        idx++;
    } else {
        idx=0;
    }
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, cTonleiter[idx]);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_1, cTonleiter[idx]>>1);
}

