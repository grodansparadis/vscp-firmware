/*********************************************************************
 *
 *                  ICMP Module Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ICMP.h
 * Dependencies:    StackTsk.h
 *                  IP.h
 *                  MAC.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/

#ifndef ICMP_H
#define ICMP_H

#include "stacktsk.h"
#include "ip.h"
#include "mac.h"

typedef enum _ICMP_CODE
{
    ICMP_ECHO_REPLY = 0,
    ICMP_ECHO_REQUEST = 8
} ICMP_CODE, ICMP_REQUESTS;


/*********************************************************************
 * Function:        BOOL ICMPIsTxReady()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if transmit buffer is ready
 *                  FALSE if transmit buffer is not ready
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
#define ICMPIsTxReady()     MACIsTxReady()


/*********************************************************************
 * Function:        void ICMPPut(NODE_INFO *remote,
 *                               ICMP_CODE code,
 *                               BYTE *data,
 *                               BYTE len,
 *                               WORD id,
 *                               WORD seq)
 *
 * PreCondition:    ICMPIsTxReady() == TRUE
 *
 * Input:           remote      - Remote node info
 *                  code        - ICMP_ECHO_REPLY or ICMP_ECHO_REQUEST
 *                  data        - Data bytes
 *                  len         - Number of bytes to send
 *                  id          - ICMP identifier
 *                  seq         - ICMP sequence number
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Note:            A ICMP packet is created and put on MAC.
 *
 ********************************************************************/
void ICMPPut(NODE_INFO *remote,
             ICMP_CODE code,
             BYTE *data,
             BYTE len,
             WORD id,
             WORD seq);


/*********************************************************************
 * Function:        BOOL ICMPGet(ICMP_CODE *code,
 *                              BYTE *data,
 *                              BYTE *len,
 *                              WORD *id,
 *                              WORD *seq)
 *
 * PreCondition:    MAC buffer contains ICMP type packet.
 *
 * Input:           code    - Buffer to hold ICMP code value
 *                  data    - Buffer to hold ICMP data
 *                  len     - Buffer to hold ICMP data length
 *                  id      - Buffer to hold ICMP id
 *                  seq     - Buffer to hold ICMP seq
 *
 * Output:          TRUE if valid ICMP packet was received
 *                  FALSE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL ICMPGet(ICMP_CODE *code,
             BYTE *data,
             BYTE *len,
             WORD *id,
             WORD *seq);



#endif


