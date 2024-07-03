/** @file IAccelerationHandler.h
 *  @brief This header file is an interface for the use of the acceleration sensor.
 *  @author Gruppe 6
 *  @date 2024-06-17
 */

#ifndef SRC_INTERFACE_IACCELERATIONHANDLER_H_
#define SRC_INTERFACE_IACCELERATIONHANDLER_H_

#include <IMain.h>


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
 * postInitAccelerationHandler():
 *
 * The postInit method is for setting up Interrupts.
 */
void postInitAccelerationHandler();


/**
 * setAccelerationSensorActive(bool active):
 *
 * This method activates or deactivates the accelerationSensor
 *  args:
 *      active: the bool that activates / deactivates the sensor
 */
void setAccelerationSensorActive(bool active);


#endif /* SRC_INTERFACE_IACCELERATIONHANDLER_H_ */
