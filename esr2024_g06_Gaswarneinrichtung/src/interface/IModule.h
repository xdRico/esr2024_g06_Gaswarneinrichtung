/*
 * Module.h
 *
 *  Created on: 17.06.2024
 *      Author: ricow
 *
 *  This header file is an interface for use in all modules in the project.
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
void preinit();


/**
 * init():
 *
 * The init method is for setting up basic working requirements and startups.
 */
void init();


#endif /* SRC_INTERFACE_IMODULE_H_ */
