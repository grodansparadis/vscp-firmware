/**************************************************************************
 *
 * FILE NAME:			ds1340.h
 * FILE DESCRIPTION:	DS1340 (RTC) driver interface
 *
 * FILE CREATION DATE:	23-11-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,23nov03 erd	written
 *
 ***************************************************************************/

#ifndef __DS1340_H_
#define __DS1340_H_

// ==================================================================
// types

// structure which holds the date and time
typedef struct
{
    uint_8      seconds;    // seconds
    uint_8      minutes;    // minutes
    uint_8      hours;      // hours
    uint_8      day;        // day (0 = saturday)
    uint_8      date;       // date
    uint_8      month;      // month
    uint_8      year;       // year

} DS1340_DATETIME;

// ==================================================================
// interfaces

// ds1340_init()
//
// Initialize the real-time clock module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

extern BOOL ds1340_init();

// ds1340_test()
//
// Test the device
//
// Return value:
//   TRUE if tested successfully, FALSE if failed
//

extern BOOL ds1340_test();


// ds1340_getTime(DS1340_DATETIME *datetime)
//
// Gets the current date and time
//
// Arguments:
//	datetime: pointer to structure to be filled with the current date and time
//
// Return value:
//	If TRUE, successfully received date and time from IC. If FALSE, 
//  device did not respond correctly and datetime does not hold proper
//  data

extern BOOL ds1340_getTime(DS1340_DATETIME *datetime);

// ds1340_setTime(DS1340_DATETIME *datetime)
//
// Sets the current date and time
//
// Arguments:
//	datetime: pointer to structure holding current date and time
//
// Return value:
//	If TRUE, successfully set date and time to IC. If FALSE, 
//  device did not respond correctly and holds undefined data

extern BOOL ds1340_setTime(const DS1340_DATETIME *datetime);

#endif /* __DS1340_H_ */
