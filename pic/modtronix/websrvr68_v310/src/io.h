/**
 * @brief           General IO code
 * @file            io.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_io
 *
 *
 * @section description Description
 **********************************
 * This module contains code I/O related code.
 *
 *
 * @subsection io_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- IO Configuration --------------------
 //*********************************************************************
 //Define ...
 //#define IO_....

 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2009-04-22, David Hosken (DH):
 *    - Initial code
 *********************************************************************/

/**
 * @defgroup mod_sys_io IO Code
 * @ingroup mod_sys
 *********************************************************************/


#ifndef _IO_H_
#define _IO_H_


/////////////////////////////////////////////////
//Defines


//This is used by other application modules
#ifndef THIS_IS_IO_MODULE

//Port latch used by Expansion Board code. Corresponding bit in evtLatchExInUpdt indicates which bits contain new values
extern WORD evtLatchExIn;

//Indicates which port latches have been updated. When updated by io.c, this bit is set. When read by
//expansion board code, bit is cleared.
extern WORD evtLatchExInUpdt;

#endif



/////////////////////////////////////////////////
//Function prototypes


/**
 * Initialize the IO module
 */
void ioInit(void);


/**
 * Service this module
 */
void ioTask(void);



#endif    //_IO_H_




