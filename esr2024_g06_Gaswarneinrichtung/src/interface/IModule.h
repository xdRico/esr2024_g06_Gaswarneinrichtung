/** @file Module.h
 *  @brief This header file is an interface for use in all modules in the project.
 *  @author ricow
 *  @date 2024-06-17
 */


#ifndef SRC_INTERFACE_IMODULE_H_
#define SRC_INTERFACE_IMODULE_H_

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


/**
 * mainRoutine():
 *
 * The mainRoutine method is for the main routine of the program, that gets called in the main loop
 */
void mainRoutine();


#endif /* SRC_INTERFACE_IMODULE_H_ */
