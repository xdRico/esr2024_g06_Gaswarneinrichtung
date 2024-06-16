/*
 * IOutputHandler.h
 *
 *  Created on: 16 Jun 2024
 *      Author: Olawale
 */

#ifndef SRC_INTERFACE_IOUTPUTHANDLER_H_
#define SRC_INTERFACE_IOUTPUTHANDLER_H_
#include <stdbool.h>

void preinit();

void init();

void setOnState(bool isOn);

void setWarningState(bool warningState);

void setAlarmState(bool alarmState);


#endif /* SRC_INTERFACE_IOUTPUTHANDLER_H_ */
