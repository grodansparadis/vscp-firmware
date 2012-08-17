/*
**  File:       WiCAN.H
**
**  Purpose:    WiCAN LIB for SJA1000
**
**  Chip:		AT90S8515, ATmega161
**
**  Version:    1.0.6, 16:th of August 2002
**
**  Author:     Lars Wictorsson
**              LAWICEL / SWEDEN
**              http://www.lawicel.com   lars@lawicel.com
**
**  Copyright:  The copyright to the computer program(s) herein is the
**              property of LAWICEL HB, Sweden. The program(s) may be used
**              and/or copied only with the written permission of LAWICEL HB
**              in accordance with the terms and conditions stipulated in
**              the agreement/contract under which the program(s) have been
**              supplied.
**
**  Compiler:   This program is tested with ICCAVR version 6.25C.
**
**  Remarks:    The Base CAN adress is set to 0x4000 in the LIB, however
**              you may overide this by removing the remark below and set
**              your base address accordingly. Works only with WiCAN source!
**
**  History:    2000-03-16  1.0.0   Created (LWI)
**              2000-04-04  1.0.1   Optimized FIFO (LWI)
**              2000-11-07  1.0.2   Changed INT0 from edge to low level detect (LWI)
**              2001-05-06  1.0.3   Added support for 29bit ID's (LWI)
**              2001-05-14  1.0.4   Added support for Acceptance Mask/Code & RTR frames (LWI)
**              2002-01-23  1.0.5   Re-Compiled with latest ICCAVR (LWI)
**              2002-08-16  1.0.6   Optimized the code for speed/size + Mega161 support (LWI)
*/

/*
**  Set here what type of MCU you are using (NOTE! Works only with Source version)
*/
#define CAN_BASE    0x4000

#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef		__WiCAN__H
#define		__WiCAN__H

/*
**  Set here what type of MCU you are using (NOTE! Works only with Source version)
**  For DEMO and OBJ version these settings has no effect!
*/
#define  WiCAN_8515
//#define  WiCAN_M8515      // Not supported Yet
//#define  WiCAN_M161
//#define  WiCAN_M162      // Not supported Yet
//#define  WiCAN_M128      // Not supported Yet


/*
**  Standard definitions for variable types
*/
#define  _U08     unsigned char
#define  _S08     char
#define  _U16     unsigned int
#define  _S16     int
#define  _U32     unsigned long
#define  _S32     long

/*
**  Standard CAN bit rates according to CiA draft standard 102 Vesion 2.0
**
**  Intended for SJA1000 operating at 16MHz.
**  See notes in Draft standard 102 for bus length, sample points etc.
**
**      Speed                       Bus length  Sample Point    Comments            
*/
#define WiCAN_SPEED_10K     0   //  5000m       87%
#define WiCAN_SPEED_20K     1   //  2500m       87%             Has to be supported by all modules (CANopen)
#define WiCAN_SPEED_50K     2   //  1000m       87%
#define WiCAN_SPEED_100K    3   //  600m        87%             Not recommended for new applications (CANopen)
#define WiCAN_SPEED_125K    4   //  500m        87%
#define WiCAN_SPEED_250K    5   //  250m        87%
#define WiCAN_SPEED_500K    6   //  100m        87%
#define WiCAN_SPEED_800K    7   //  50m         80%
#define WiCAN_SPEED_1M      8   //  25m         75%
#define WiCAN_SPEED_MANUAL  255 //                              Manual setting with Btr0 & Btr1

//
//  Error codes
//
#define WiCAN_OK                0
#define WiCAN_ERR               1
#define WiCAN_INIT_SPEED_ERR    1
#define WiCAN_NOT_INITIATED     1
#define WiCAN_NOT_STARTED       1
#define WiCAN_TXBUF_FULL        2
#define WiCAN_RXBUF_EMPTY       2

//
//  Flag bits
//
#define WiCAN_LOST_FRAME        0x01
#define WiCAN_BUS_OFF           0x02

//
//  WiCAN version number
//
#define WiCAN_VER100            0x01
#define WiCAN_VER101            0x02
#define WiCAN_VER102            0x03
#define WiCAN_VER103            0x04
#define WiCAN_VER104            0x05
#define WiCAN_VER105            0x06
#define WiCAN_VER106            0x07

//
//  Frame Flag bits
//
#define WiCAN_29                0x80
#define WiCAN_RTR               0x40

//
//  CAN Message structure
//
typedef struct {
    _U32 id;
    _U08 len;
    _U08 flags;
    _U08 byte[8];
  } WiCAN_Object;

//
//  Implementations (API)
//
_U08 WiCAN_Init(_U08, _U08, _U08, _U08);
_U08 WiCAN_Start(void);
_U08 WiCAN_Stop(void);
_U08 WiCAN_SendFrame(WiCAN_Object *);
_U08 WiCAN_GetFrame(WiCAN_Object *);
void WiCAN_EmptyOutQueue(void);
void WiCAN_EmptyInQueue(void);
_U08 WiCAN_GetRxQueueSize(void);
_U08 WiCAN_GetTxQueueSize(void);
_U08 WiCAN_GetStatus(void);
_U08 WiCAN_GetVersion(void);
_U08 WiCAN_SetAccCode(_U08, _U08, _U08, _U08);
_U08 WiCAN_SetAccMask(_U08, _U08, _U08, _U08);

#endif


