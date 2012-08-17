/*********************************************************************
 *
 *               HTTP definitions on Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        Http.h
 * Dependencies:    None
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
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
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01 Original (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
********************************************************************/

#ifndef HTTP_H
#define HTTP_H

#include <projdefs.h>
#include "tcp.h"    //Requered for TCP_SOCKET definition

#define HTTP_PORT               (80)

#define HTTP_START_OF_VAR       (0x0000)
#define HTTP_END_OF_VAR         (0xFFFF)

/*
 * Maximum length of a requested resource via HTTP = 8 + 3.
 */
#define HTTP_MAX_RESOURCE_NAME_LEN (12)

//A HTTP variable consists out of a group and a value part. The group is the first character
//following the % character, and the value the second.
typedef struct _HTTP_VAR
{
    BYTE group;     //The group of this HTTP Variable
    BYTE val;       //The value of this HTTP Variable
} HTTP_VAR;


/*********************************************************************
 * Function:        void HTTPInit(void)
 *
 * PreCondition:    TCP must already be initialized.
 *
 * Input:           None
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        Set all HTTP connections to Listening state.
 *                  Initialize FSM for each connection.
 *
 * Note:            This function is called only one during lifetime
 *                  of the application.
 ********************************************************************/
void HTTPInit(void);


/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened HTTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:        Browse through each connections and let it
 *                  handle its connection.
 *                  If a connection is not finished, do not process
 *                  next connections.  This must be done, all
 *                  connections use some static variables that are
 *                  common.
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
void HTTPServer(void);

/*********************************************************************
 * Function:        BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen)
 *
 * PreCondition:    HTTPGetRqstRes() must have already be called.
 *
 * Input:           s   -   TCP Socket from which to read the next name-value parameter (if any)
 *                  param -  Will hold name and value strings when returning
 *                  paramLen - On entry into routine holds lengh of param buffer.
 *                             Will hold Index of value string on return, or 0 if there were none.
 *
 * Output:          1 if there are parameters to follow, else 0
 *
 * Side Effects:    None
 *
 * Overview:        Writes the name and value string to given param buffer. Both strings are
 *                  NULL terminated. On return name ane value string can be accessed as follows:
 *                  - Pointer to Name parameter = &param[0]
 *                  - Pointer to Value parameter = &param[paramLen]
 *
 * Note:            
 ********************************************************************/
BYTE HTTPGetParam(TCP_SOCKET s, BYTE* param, BYTE* paramLen);

#if defined(THIS_IS_HTTP_SERVER)
    /*
     * Main application must implement these callback functions
     * to complete Http.c implementation.
     */

/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened HTTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:        Browse through each connections and let it
 *                  handle its connection.
 *                  If a connection is not finished, do not process
 *                  next connections.  This must be done, all
 *                  connections use some static variables that are
 *                  common.
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
extern WORD HTTPGetVar(HTTP_VAR httpVar, WORD ref, BYTE* val);


/*********************************************************************
 * Function:        void HTTPExecGetCmd(TCP_SOCKET s, BYTE* rqstRes)
 *
 * PreCondition:    None
 *
 * Input:           s           - Socket that is currently receiving this HTTP command
 *                  rqstRes     - Name of the Requested resource - GET command's action
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a "callback" from HTTPServer task. Whenever a remote node
 *                  performs interactive GET task on page that was served, HTTPServer calls this
 *                  functions. Use HTTPGetParam() to get all name-value parameters.
 *
 * Note:            none
 ********************************************************************/
extern void HTTPExecGetCmd(TCP_SOCKET s, BYTE* rqstRes);
#endif


#endif
