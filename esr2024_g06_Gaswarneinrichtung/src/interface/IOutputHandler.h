/*
 * IOutputHandler.h
 *
 *  Created on: 16 Jun 2024
 *      Author: Olawale
 *
 *  This header file is an interface for outputting Data.
 */

#ifndef SRC_INTERFACE_IOUTPUTHANDLER_H_
#define SRC_INTERFACE_IOUTPUTHANDLER_H_

#include "IModule.h"


/**
 * setOnState(bool isOn):
 *
 * This method is used to set the state, if the device is switched on
 *
 *  param:
 *      isOn: the bool that defines, if the device is switched on
 */
void setOnState(bool isOn);


/**
 * setWarningState(bool warningState):
 *
 * This method is used to set the state, if the device is warning for gas
 *
 *  param:
 *      warningState: the bool that defines, if the device is in warning-mode
 */
void setWarningState(bool warningState);


/**
 * setAlertState(bool alertState):
 *
 * This method is used to set the state, if the device is alarming for non-movement
 *
 *  param:
 *      alertState: the bool that defines, if the device is in alert-mode
 */
void setAlertState(bool alertState);


#endif /* SRC_INTERFACE_IOUTPUTHANDLER_H_ */
