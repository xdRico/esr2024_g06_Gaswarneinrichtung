/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
/*
 * Projekttitel:
 * Ausgabe einer Tonleiter
 *
 * Zweckbestimmung:
 * Demonstration, wie �ber die Rechenvorschrift zur Erzeugung von Tonintervallen
 * �ber den PWM-Ausgang eines Timers die T�ne der C-Dur Tonleiter ausgegeben werden k�nnen.
 *
 * Bestimmungsgem��er Gebrauch:
 * 1. Piezo Lautsprecher mit Port 1.6 verbinden
 * 2. Launchpad anschlie�en
 * 3. Programm �bertragen
 *
 * Funktionale Anforderungen:
 * 1. Wiedergabe der c-Dur Tonleiter
 * 2. Dauer eines Tones genau 1 Sek.
 * 3. �bergang der T�ne ohne Pause
 *
 * Nicht funktionale Anfordeurngen:
 * 1. Es gen�gt den Piezo unidirektional anzuschlie�en
 * 2. Das Tonsignal soll als 50% PWM von Timer0_B3 erzeugt werden
 * 3. Die Dauer der T�ne sollen �ber den Watchdog Timer generiert werden
 * 4. Das System soll im LPM0 betrieben werden
 * 5. Die Taktung des WDT soll �ber den externen Uhrenquarz erfolgen
 * 6. SMClk wir mit Default-Einstellungen aus REFO-Clk abgeleitet: ca. 1,048 MHz
 * 7. Berechnung der f�r die Ausgabe der T�ne erforderlichen Z�hlerst�nde
 *    soll mittel Ganzzahlarithmetik erfolgen
 *
 * Verdrahtung:
 *
 *             -----------------
 *         /|\|              XIN|-
 *          | |                 |  32kHz
 *          --|RST          XOUT|-.
 *
 *            |                 |
 *            |       P1.6/TB0.1|--> CCR1 - 50% PWM ---> Piezo
 *            |                 |                          |
 *                                                         |
 *                                                        GND
 * Iterationsplan:
 * Ver. 0.1: Dauer des Tones wird �ber Z�hlerschleife generiert (busy waiting)
 * Ver. 0.2: Dauer des Tones wird �ber watchdog Timer generiert (low Power mode)
 *
 * Sonstige Hinweise:
 * - Berechnung der Frequenzen
 *      Die Berechnung der Tonfrequenzen erfolgt ausgehend vom Kammerton a' (440Hz)
 *      entsprechend der Vorgabe f�r eine temperierte Stimmung. D.h. die Frequenz
 *      des n�chst h�heren Tones ergibt sich �ber die Multiplikation mit einem festen
 *      Faktor k. Da die Tonleiter in 12 Halbt�ne unterteilt ist, muss die Multiplikation
 *      12 mal erfolgen, um die eine Oktave zu durchlaufen (innerhalb einer Oktave
 *      erh�lt man beim 12. Ton die doppelte Frequenz des 1. Tones der Tonleiter).
 *      D.h. k*k*k*k*k*k*k*k*k*k*k*k = k^12 = 2 --> k = 2^(1/12) = 1,059463
 *
 *      Da die Frequenz f�r die Tonausgabe �ber den SMClk erzeugt wird, macht es
 *      keinen Sinn tiefer als LPM0 zu gehen, da der SMClk auch w�hrend der Sleep-
 *      Phasen noch aktiv bleiben muss. Das Timer Modul �berstimmt daher die
 *      ggf. per Software eingestellten tieferen Sleep Phasen.
 *
 * - Validierung:
 *      Die Korrektheit des Codes wird �berpr�ft, indem die Tonfrequenz des c'
 *      �ber ein Osci dargestellt und vermessen wird.
 *      Sollfrequenz: 261 Hz
 *      Istfrequenz: 261 Hz
 *      Der zugeh�rige Screenshot ist im Verzeichnis Docs hinterlegt
 *
 * Links:
 * - https://de.wikipedia.org/wiki/Frequenzen_der_gleichstufigen_Stimmung
 * - https://www.scinexx.de/dossierartikel/vom-tiefen-bis-zum-hohen-c
 *
 *
 *
 * To do:
 * 1) Das Programm hat noch 2 Schw�chen: Beim Reset wird ein Chirp h�rbar, d.h.
 * beim ersten Ton wird nicht die eigentlich erw�nschte Frequenz ausgegeben,
 * sondern eine langsam ansteigende Frequenz. Ursache unklar. Im Osci-Bild ist
 * erkennbar, dass dieser Frequenzanstieg von einer tieferen Frequenz aus startet
 * und dann auf die gew�nschte Frequenz zul�uft; m�glicherweise ein Einschwingvorgang
 * beim Piezo oder beim DCO (sollte nicht zu erwarten sein)
 * 2) Im Debugger sind gelegentlich h�here Frequenzen h�rbar, als wenn anschlie�end das
 * Programm stand-alone l�uft. Ursache unklar. Die Frequenzen, die au�erhalb des Debuggers
 * durchlaufen werden sind korrekt (Osci)
 *
 * Vorlagen:
 * 1. TI sample project timer_b_ex3_pwm
 * 2. TI sample project wdt_a_ex2_intervalACLK
 *
 * Author:
 * S. Steddin
 * 2021-06-15 - Ver. 0.2
 */
#include <stdint.h>
#include "driverlib.h"


// Timer Cnt f�r 440Hz @ 1,048 MHz Takt cnt = 1,048*10^6/440 = 2381
// Kalibrierung mit Osci ergab jedoch, dass beim Z�hlerwert 2400
// exakt 440Hz ausgegeben werden; dies gilt aber nur f�r das Exemplar
// des Launchpads, auf dem die Kalibrierung durchgef�hrt wurde. Bei
// anderen Exemplaren kann ein anderer Wert erforderlich sein.
// Mit dem hier experimentell ermittelten Z�hlerwert l�sst sich
// errechnen, welche Frequenz der DCO auf diesem Board tats�chlich liefert:
// fDCO = 440 Hz * 2400 = 1,056 MHz statt der erwarteten 1,048 MHz
// Dies erkl�rt sich dadurch, dass die FLL-Referenz des DCOs (REFO)
// nicht exakt die erwarteten 32768 Hz zur Verf�gung stellt.

// Halbton tiefer: Faktor 2^(1/12) * 2^16 = 69433
// Halbton h�her: Faktor 1/ 2^(1/12) * 2^16 = 61858

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


const unsigned int cTonleiter[] = {c1, d1, e1, f1, g1, a1, h1, c2};
unsigned int idx = 0;

void initSystem();

void main(void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    initSystem();

    _BIS_SR(LPM0_bits + GIE);                 // CPU off

}


void initSystem() {
    //F�r das Clock-System werden die Default-Einstellungen �bernommen.
    //D.h., das SMClk = MClk = 1,048 MHz, FLL aktiviert mit REFO als Taktgeber
    //Es m�ssen also keine Einstellungen am CS vorgenommen werden.

    // Configure Pins for XIN and XOUT
    //Set P2.6 and P2.7 as Module Function Input.
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN6 + GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );


    //Ausgangspin von Timer0_B3, CCR1 (TB0.1 - Pin 1.6)
    //statt als GPIO-Pin zu verwenden, mit Timer0_B3 CCR1 Output verbinden:
    //secondary function f�r das Pin aktivieren
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        GPIO_PORT_P1,
        GPIO_PIN6,
        GPIO_SECONDARY_MODULE_FUNCTION
        );

    //Entriegeln der GPIOs, damit AClk bereits starten kann
    PMM_unlockLPM5();

    //Grundperiode von Timer0_B3 im Up-Mode festlegen: CCR0 einstellen
    //Beachte: Wenn Z�hler im Up-Mode betrieben wird, so kann CCR0 nicht f�r die
    //Ausgabe eines Digitalsignals verwendet werden (also auf ein Outputpin gelegt
    //werden, sondern definiert die Grundperiode.
    //Das Ausgabesignal wird stattdessen mit den Output-Units von CCR1 oder CCR2 erzeugt,
    //deren Z�hlerwert beim SET-RESET Mode das PWM-Tastverh�ltnis festlegt.

    //Einstellungen f�r die Grundperiode �ber CCR0:
    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = c1;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);

    //Einstellungen f�r das 50%-PWM Lautsprechersignal am Ausgang, der CCR1
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




//Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt
void WDT_A_ISR (void)
{
    if (idx < 7) {
        idx++;
    } else {
        idx=0;
    }
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, cTonleiter[idx]);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_1, cTonleiter[idx]>>1);
}


