/*
 * IAccelerationHandler.h
 *
 *  Created on: 17.06.2024
 *      Author: ricow
 *
 *  This header file is an interface for the use of the acceleration sensor.
 */

#ifndef SRC_INTERFACE_IACCELERATIONHANDLER_H_
#define SRC_INTERFACE_IACCELERATIONHANDLER_H_

#include "IModule.h"


/**
 * preInitAccelerationHandler():
 *
 * The preInit method is for setting up all Port/Pin register values and initializing all basic setups.
 */
void preInitAccelerationHandler();


/**
 * initAccelerationHandler():
 *
 * The init method is for setting up basic working requirements and startups.
 */
void initAccelerationHandler();


/**
 * setTimeUntilWarning(int time):
 *
 * This method is used to define the maximum Time in Seconds that the AccelerationSensor has not to be moved to activate warning mode.
 *
 *  param:
 *      time: the time to set as maximum - in Seconds
 */
void setTimeUntilWarning(int time);


#endif /* SRC_INTERFACE_IACCELERATIONHANDLER_H_ */
