/*********************************************************************
 *
 *                  MAC Module Defs for Microchip Stack
 *
 *********************************************************************
 * FileName:        MAC.h
 * Dependencies:    StackTsk.h
 * Processor:       PIC18C
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01     Original        (Rev 1.0)
 * Nilesh Rajbharti     11/27/01    Added SLIP
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 ********************************************************************/

#ifndef MAC_H
#define MAC_H

#include "stacktsk.h"

#define MAC_IP      (0)
#define MAC_ARP     (0x6)
#define MAC_UNKNOWN (0x0ff)

#define INVALID_BUFFER  (0xff)

void    MACInit(void);
BOOL    MACIsTxReady(void);

BOOL    MACGetHeader(MAC_ADDR *remote, BYTE* type);
BYTE    MACGet(void);
WORD    MACGetArray(BYTE *val, WORD len);
void    MACDiscardRx(void);

void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen);
void    MACPut(BYTE val);
void    MACPutArray(BYTE *val, WORD len);
void    MACFlush(void);
void    MACDiscardTx(BUFFER buffer);

void    MACSetRxBuffer(WORD offset);
void    MACSetTxBuffer(BUFFER buffer, WORD offset);
void    MACReserveTxBuffer(BUFFER buffer);

WORD    MACGetOffset(void);

WORD    MACGetFreeRxSize(void);


#define MACGetRxBuffer()        (NICCurrentRdPtr)
#define MACGetTxBuffer()        (NICCurrentTxBuffer)

#if     !defined(THIS_IS_MAC_LAYER)
#if     !defined(STACK_USE_SLIP)
extern  BYTE NICCurrentTxBuffer;
extern  BYTE NICCurrentRdPtr;
#else
#define NICCurrentTxBuffer      (0)
#define NICCurrentRdPtr         (0)
#endif
#endif

BOOL MACIsLinked(void);


#endif
