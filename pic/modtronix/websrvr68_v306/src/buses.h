 /**
 * @brief           Serial Buses
 * @file            buses.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_httpsrvr
 *
 *
 * @section description Description
 **********************************
 * This module contains code for implementing different serial buses.
 *
 * @section buses_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //--------------------  Buses Configuration --------------------
 //*********************************************************************
 //Default "UDP Command Port"
 //#define DEFAULT_CMD_UDPPORT (54123)

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
 * 2006-12-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
//  Serial Buses - documentation module
 /**
 * @defgroup mod_buses Serial Buses
 *
 * <b>Serial Buses have not been implemented yet! Will be implemented in a future version! </b>
 *
 * The @appname implements many different serial buses that can be linked together via the web
 * based interface. For example, USART 1 (RS232 port) can be linked to a UDP port to implement a
 * USART to UDP interface converters.
 * The following buses are currently implemented:
 * - Two software SPI buses, SWSPI1 and SWSPI2
 * - One master I2C buses.
 * - Two USARTS, USART1 and USART2. They use the actual USART1 and 2 hardware peripherals of
 *   the CPU
 * - Two UDP ports
 * - One TCP port
 *
 * By default, there are 8 buffers that can be used for the buses. Each bus can be
 * configured to have a receive and/or transmit buffer. When configured to have a receive buffer,
 * all received data will be stored to it. When configured to have a transmit buffer,
 * all data contained in this buffer will be transmitted.
 * The following buffers are defined:
 * - Buffer 0: 128 bytes
 * - Buffer 1: 128 bytes
 * - Buffer 2: 64 bytes
 * - Buffer 3: 64 bytes
 * - Buffer 4: 32 bytes
 * - Buffer 5: 32 bytes
 * - Buffer 6: 32 bytes
 * - Buffer 7: 32 bytes
 *********************************************************************/

#ifndef BUSES_H
#define BUSES_H


/**
 * Initializes "UDP Command Port" and "UDP Command Responce Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void busInit(void);

/**
 * Must be called every couple of ms
 *
 */
void busProcess(void);

#endif
