/**
 * @brief           Serial port configuration
 * @file            sercfg.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 * @section description Description
 **********************************
 * This module contains functions used for the serial configuration of the application.
 * This is the menu that is displayed on startup via the USART and allows the device to
 * be configured.
 *
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
 * 2005-06-07, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef SERCFG_H
#define SERCFG_H

#include "net\ip.h"

/**
 * Delays for the given time, and checks for any input on the serial port. If anything is received
 * within this time, it enters the serial configuration menu. If not, it returns
 *
 * @param delay     Delay in seconds
 */
void scfInit(BYTE delay);

/**
 * Writes out the IP value to the serial port
 *
 * @param IPVal     IP value to write out to the port
 */
void scfDisplayIPValue(IP_ADDR *IPVal);

#endif
