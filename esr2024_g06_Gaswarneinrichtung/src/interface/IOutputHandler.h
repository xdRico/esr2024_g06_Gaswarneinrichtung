/** @file IOutputHandler.h
 *  @brief This header file is an interface for outputting Data.
 *  @author Olawale
 *  @date 2024-06-16
 */

#ifndef SRC_INTERFACE_IOUTPUTHANDLER_H_
#define SRC_INTERFACE_IOUTPUTHANDLER_H_

#include <IMain.h>


/**
 * preInitOutputHandler():
 *
 * The preInit method is for setting up all Port/Pin register values and initializing all basic setups.
 */
void preInitOutputHandler();


/**
 * initOutputHandler():
 *
 * The init method is for setting up basic working requirements and startups.
 */
void initOutputHandler();


/**
 * postInitOutputHandler():
 *
 * The postInit method is for setting up interrupts.
 */
void postInitOutputHandler();


/**
 * setAlertLED(bool alert):
 *
 * This method is used to de-/activate the alert LED, if the device is switched on
 *
 *  param:
 *      alert: the bool that defines, if the alert LED is on
 */
void setAlertLED(bool alert);


/**
 * setOnLED(bool on):
 *
 * This method is used to de-/activate the On-state LED
 *
 *  param:
 *      on: the bool that defines, if the On-LED is on
 */
void setOnLED(bool on);

/**
 * void setPiezoActive(bool alert):
 *
 * This method is used to Toggle the Piezo
 *
 *  param:
 *      alert: the bool which defines if the piezo is on
 */
void setPiezoActive(bool alert);

#endif /* SRC_INTERFACE_IOUTPUTHANDLER_H_ */
