/**************************************************************************
 *
 * FILE NAME:			lm75.h
 * FILE DESCRIPTION:	LM75 9 bit temperature sensor driver header
 * 
 * FILE CREATION DATE:	03-01-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 02a,3jan04  erd 	adapted from pca9555 to lm75
 * 01a,26may03 erd	written
 *
 ***************************************************************************/

#ifndef __LM75_H_
#define __LM75_H_

#include "system.h"

// ==========================================================================
// External interface

// lm75_init()
//
// Initialize the temperature sensor module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

extern BOOL lm75_init();

// lm75_getTemp(uint_16 *temp)
//
// Gets the current date and time
//
// Arguments:
//	temp: 9 bit temperature value
//
// Return value:
//	If TRUE, successfully received date and time from IC. If FALSE, 
//  device did not respond correctly and datetime does not hold proper
//  data

extern BOOL lm75_getTemp(uint_16 *temp);

// lm75_test()
//
// Read three consecutive reads of the temperature and compare results
//
// Return value:
//   TRUE if test is a success, FALSE if failed
//

extern BOOL lm75_test();


#endif /* __LM75_H_ */
