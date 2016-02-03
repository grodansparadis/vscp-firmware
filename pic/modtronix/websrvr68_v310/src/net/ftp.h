/**
 * @brief           FTP Server Module for Modtronix TCP/IP Stack
 * @file            ftp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, tcp.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_user_ftp
 *
 * @section description Description
 **********************************
 * This module implements a basic FTP client.
 * <br>For a detailed description, see the @ref mod_tcpip_user_ftp section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP User Protocols].
 *
 *
 * @section ftp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- FTP  Configuration --------------------
 //*********************************************************************
 //General configuration data
 #define FTP_COMMAND_PORT                (21ul)
 #define FTP_DATA_PORT                   (20ul)
 #define FTP_TIMEOUT                     (TICK)((TICK)180 * TICK_SECOND)
 #define MAX_FTP_ARGS                    (7ul)
 #define MAX_FTP_CMD_STRING_LEN          (31ul)

 //Configure FTP mudule to have PUT
 #define FTP_PUT_ENABLED
 @endcode
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
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/23/01  Original        (Rev 1.0)
 ********************************************************************/

/////////////////////////////////////////////////
// FTP - documentation module
/**
@defgroup mod_tcpip_user_ftp FTP
@ingroup mod_tcpip_user

@section mod_tcpip_user_ftp_conf Configuration
For details on configuring this module, see @ref ftp_conf section of ftp.h file.
*/


#ifndef FTP_H
#define FTP_H

/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file

#if !defined(FTP_COMMAND_PORT)
#define FTP_COMMAND_PORT                (21ul)
#endif

#if !defined(FTP_DATA_PORT)
#define FTP_DATA_PORT                   (20ul)
#endif

#if !defined(FTP_TIMEOUT)
#define FTP_TIMEOUT                     ((TICK16)180ul * (TICK16)TICKS_PER_SECOND)
#endif

#if !defined(MAX_FTP_ARGS)
#define MAX_FTP_ARGS                    (7ul)
#endif

#if !defined(MAX_FTP_CMD_STRING_LEN)
#define MAX_FTP_CMD_STRING_LEN          (31ul)
#endif

#define FTP_USER_NAME_LEN               (9ul)     //8 characters + NULL

#ifdef THIS_IS_FTP
/**
 * Compare given login and password with internal values and return TRUE or FALSE.
 * This is a callback from Telnet Server to user application.  User application must
 * implement this function in his/her source file return correct response based
 * on internal login and password information.
 *
 * @param login     Telnet User login name
 * @param password  Telnet User password
 * @return          TRUE if login and password verfies
 *                  FALSE if login and password does not match
 */
extern BOOL FTPVerify(char *login, char *password);
#endif


/**
 * Initializes internal variables of Telnet
 *
 * @preCondition    TCP module is already initialized
 */
void FTPInit(void);


/**
 * This function acts as a task (similar to one in RTOS). This function
 * performs its task in co-operative manner.  Main application must call
 * this function repeatdly to ensure all open or new connections are served on time.
 *
 * @preCondition    FTPInit() must already be called.
 * @return          Opened Telnet connections are served.
 */
BOOL FTPServer(void);


#endif
