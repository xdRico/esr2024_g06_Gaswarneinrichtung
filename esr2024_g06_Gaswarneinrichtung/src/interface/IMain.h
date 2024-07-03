/** @file Main.h
 *  @brief This header file is an interface for use in all modules in the project.
 *  @author Gruppe
 *  @date 2024-06-17
 */


#ifndef SRC_INTERFACE_IMAIN_H_
#define SRC_INTERFACE_IMAIN_H_

#include <stdbool.h>
#include <msp430.h>
#include <driverlib.h>
#include <Board.h>


/**
 * preInit():
 *
 * The preInit method is for setting up all Port/Pin register values and initializing all basic setups.
 */
void preInit();


/**
 * init():
 *
 * The init method is for setting up basic working requirements and startups.
 */
void init();


/**
 * postInit():
 *
 * The postInit method is for setting up Interrupts.
 */
void postInit();

/**
 * loop():
 *
 * The loop method gets called repeatedly while the device is on-stated
 */
void loop();


#endif /* SRC_INTERFACE_IMAIN_H_ */
