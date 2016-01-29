/**
 * @brief           Stack Task Module for Modtronix TCP/IP Stack
 * @file            stacktsk.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    compiler.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *
 * @section description Description
 **********************************
 * This module contains the ARP Server code.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2001-08-10, Nilesh Rajbharti:
 *    - Original
 * 2002-02-09, Nilesh Rajbharti:
 *    - Cleanup
 * 2002-05-022, Nilesh Rajbharti:
 *    - Rev 2.0 (See version.log for detail)
 * 2003-08-07, Nilesh Rajbharti:
 *    - Rev 2.21 - TFTP Client addition
 * 2004-12-07, David Hosken (DH):
 *    - Moved all project configuration data to projdefs.h
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2006-06-18, David Hosken (DH):
 *    - stackFlags is no longer defined as external
 *********************************************************************/

#ifndef STACK_TSK_H
#define STACK_TSK_H

#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP)
    #define StackIsInConfigMode()   (stackFlags.bits.bInConfigMode)
#else
    #define StackIsInConfigMode()   (FALSE)
#endif


/////////////////////////////////////////////////
//Global variables defined in main application
#ifndef THIS_IS_STACK_TASK
    //String must have format Vn.mm or Vnn.mm.
    // - n = major part, and can be 1 or 2 digets long
    // - mm is minor part, and must be 2 digets long!
    extern ROM char STACK_VER_STR[];
#endif
#define STACK_VER_MAJOR 2ul     /* Number from 1 to 99 */
#define STACK_VER_MINOR 51ul    /* Number from 1 to 99 */


/**
 * This function must be called before any of the
 * stack or its component routines be used.
 *
 * Side Affect: Stack and its componentns are initialized
 */
void StackInit(void);


/**
 * This FSM checks for new incoming packets, and routes it to appropriate
 * stack components. It also performs timed operations.
 *
 * This function must be called periodically called
 * to make sure that timely response.
 *
 * @preCondition    StackInit() is already called.
 *
 * side affect:     Stack FSM is executed.
 */
void StackTask(void);


#endif
