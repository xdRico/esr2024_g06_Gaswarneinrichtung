/*
 * IGasHandler.h
 *
 *  Created on: 17.06.2024
 *      Author: ricow
 *
 *  This header file is an interface for the use of the gas measuring sensor.
 */

#ifndef SRC_INTERFACE_IGASHANDLER_H_
#define SRC_INTERFACE_IGASHANDLER_H_

#include "IModule.h"


/**
 * preInitGasHandler():
 *
 * The preInit method is for setting up all Port/Pin register values and initializing all basic setups.
 */
void preInitGasHandler();


/**
 * initGasHandler():
 *
 * The init method is for setting up basic working requirements and startups.
 */
void initGasHandler();


/**
 * measure():
 *
 * This method is used to get the current value of the gas sensor.
 *
 */
void measure();


/**
 * isValueCritical():
 *
 * This method is used to define the maximum Time in Seconds that the AccelerationSensor has not to be moved to activate warning mode.
 *
 *  return:
 *      bool: if the current value is critical and a warning has to be given
 */
bool isValueCritical();


/**
 * setBurnerActive(bool active):
 *
 * This method is used to define, if the burner of the gas sensor is activated.
 *
 *  param:
 *      active: the bool that defines, if the burner is active
 */
void setBurnerActive(bool active);


/**
 * setCriticalValue(int criticalValue):
 *
 * This method is used to definethe critical Value for the gas sensor, at which a waning is given.
 *
 *  param:
 *      criticalValue: the int that defines the critical Value, where a warning should be given
 */
void setCriticalValue(int criticalValue);


#endif /* SRC_INTERFACE_IGASHANDLER_H_ */
