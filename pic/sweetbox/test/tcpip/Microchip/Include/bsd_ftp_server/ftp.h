/********************************************************************
 * FileName:		FTP.h		
 * Dependencies:
 * Processor:				
 * Hardware:		
 * Assembler:		
 * Linker:		
 * Company:		Microchip Technology, Inc.
 *
 * Software License Agreement:
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *********************************************************************
 * File Description:
 *
 * FTP Server
 *
 * Change History:
 * Name				Date            Changes
 * Sean Justice		03/10/06		Initial Version
 ********************************************************************/
#ifndef _FTP_HEADER_FILE
#define _FTP_HEADER_FILE

#include "generic.h"
#include "bsd_tcp_ip\tcpip.h"
#include "fat16\fat.h"

/************************************************************************
 * public defines
 ************************************************************************/
#define FTP_TX_BUF_SIZE					64
#define FTP_USER_NAME_LEN				(10)

/************************************************************************
 * public enums
 ************************************************************************/
typedef enum _SM_FTP
{
    SM_FTP_NOT_CONNECTED,
    SM_FTP_CONNECTED,
    SM_FTP_USER_NAME,
    SM_FTP_USER_PASS,
    SM_FTP_RESPOND
} SM_FTP;
/****************************************************************
 * public functions
 ****************************************************************/
/*********************************************************************
 * Function:		PUBLIC BOOL FTPInit(void)       
 *
 * PreCondition:	The TCP/IP is initialized.  
 *
 * Input:           None.
 *
 * Output:          true if socket was successfully creasted. else false
 *
 * Side Effects:    None
 *
 * Overview:        This function will create a socket and bind it to
 *					the FTP port.  It will listen to incoming connection
 *					requests.
 ********************************************************************/
PUBLIC BOOL FTPInit(void);
/*********************************************************************
 * Function:        PUBLIC BOOL FTPServer(void)
 *
 * PreCondition:    FTPInit has been called
 *
 * Input:           None.
 *
 * Output:          true if FTP connections are being serviced, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function handles incoming FTP connections and 
 *					services all incoming commands
 ********************************************************************/
PUBLIC BOOL FTPServer(void);
/*********************************************************************
 * Function:		PUBLIC BOOL FTPGetUser(BYTE *user)      
 *
 * PreCondition:    None.
 *
 * Input:           user	- pointer to the user buffer
 *
 * Output:          true if there is a FTP connection, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function fills the user pointer with 
 *					FTP user information.
 ********************************************************************/
PUBLIC BOOL FTPGetUser(BYTE *user);
/*********************************************************************
 * Function:            PUBLIC FTP_COMMAND FTPGetCammand(void)      
 *
 * PreCondition:        None.   
 *
 * Input:               None.
 *
 * Output:              Current FTP command
 *
 * Side Effects:        None
 *
 * Overview:            This funciton will return the current 
 *                      FTP command
 *
 ********************************************************************/
PUBLIC SM_FTP FTPGetStage(void);

#endif
