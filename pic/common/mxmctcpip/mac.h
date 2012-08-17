/*********************************************************************
 *
 *                  MAC Module Defs for Microchip Stack
 *
 *********************************************************************
 * FileName:        MAC.h
 * Dependencies:    stacktsk.h
 * Processor:       PIC18C
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01     Original        (Rev 1.0)
 * Nilesh Rajbharti     11/27/01    Added SLIP
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 ********************************************************************/

/*******************************************************
----------------- Mac Configuration --------------------
********************************************************
//The following should be defined in the projdefs.h file OR on the command line

//When STACK_USE_FAST_NIC is defined, a bit longer, but faster code is used.
#define STACK_USE_FAST_NIC

//STACK_DISABLES_INTS can be defined if interrupts are to be disabled during the MAC access routines
//#define STACK_DISABLES_INTS

//Keep a count of CNTR1 - CNTR3 registers
//#define MAC_CNTR1_3
/********************************************************/

#ifndef MAC_H
#define MAC_H

#include <projdefs.h>

#define MAC_IP      (0)
#define MAC_ARP     (0x6)
#define MAC_UNKNOWN (0x0ff)

#define INVALID_BUFFER  (0xff)

typedef struct _MAC_ADDR
{
    BYTE v[6];
} MAC_ADDR;

void    MACTask(void);
void    MACInit(void);
#if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
void	MACInitEepromRegs(void);
#endif
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
void    MACShowDebug(void);

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
