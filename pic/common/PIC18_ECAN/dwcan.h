/******************************************************************************
*                                                                             *
*  File:                           DWCAN.H                                    *
*                                                                             *
*  Description: This file contains the API for an ECAN driver that supports   *
*               the enhanced legacy mode of the ECAN peripheral. All 6        *
*               configurable CAN message objects are configured as receive    *
*               objects giving this driver 3 dedicated transmit and 8 recieve *
*               message buffers. All default CAN baud rate, filter, and mask  *
*               settings can be controlled at compile time from this header   *
*               file. This driver uses the EWIN bits in the ECANCON register  *
*               to access all transmit and receive buffers through RXB0 in    *
*               access RAM eliminating the need for RAM banking reducing code *
*               size and increasing code performance. Interrupt priorities    *
*               used for RX, TX, error, and wake-up events are also           *
*               configurable. Receive messages are automatically queued into  *
*               a software receive queue in the CAN receive interrupt. There  *
*               are two transmission methods: polled through TXB1/TXB2 or     *
*               software queued using TXB0 and the transmit interrupts.       *
*               Transmit priority is organized such that TXB0 has lower HW    *
*               priority than TXB1/TXB2. TXB1/TXB2 allow the application to   *
*               side step the software TX queue if necessary. It is also very *
*               important to note that the ECANCON register must be saved in  *
*               the interrupt context for both low and high priorty interrupts*
*               in order to maintain the EWIN bit settings even though        *
*               interrupts may be occurring!!!                                *
*                                                                             *
*               Currently this driver supports PIC18F4680/4585/2680/2585 only *
*               but will support other ECAN devices in the future wih minimal *
*               changes...                                                    *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Software License Agreement                                                 *
*                                                                             *
*  The software supplied herewith by Microchip Technology Incorporated        *
*  (the “Company”) for its PICmicro® Microcontroller is intended and          *
*  supplied to you, the Company’s customer, for use solely and                *
*  exclusively on Microchip PICmicro Microcontroller products. The            *
*  software is owned by the Company and/or its supplier, and is               *
*  protected under applicable copyright laws. All rights are reserved.        *
*   Any use in violation of the foregoing restrictions may subject the        *
*  user to criminal sanctions under applicable laws, as well as to            *
*  civil liability for the breach of the terms and conditions of this         *
*  license.                                                                   *
*                                                                             *
*  THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,          *
*  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED          *
*  TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                *
*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,          *
*  IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                 *
*  CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Created By:  Thomas R. Shelburne 11/18/04                                  *
*                                                                             *
*  Documentation:                                                             *
*                    ------------------------------------------------         *
*                    Receive Filter Configuration / Buffer Assignment         *
*                    ------------------------------------------------         *
*                                                                             *
*                           |--- RXF0 --- RXB0 (Receive Buffer 0, RXB0)       *
*                           |--- RXF1 -|                                      *
*                           |                                                 *
*                           |--- RXF2 --- RXB1 (Receive Buffer 1, RXB1)       *
*                           |--- RXF3 -|                                      *
*  RXM0 (Receive Mask 0) ---|                                                 *
*                           |--- RXF4 --- RXB2 (Receive Buffer 2, B0)         *
*                           |--- RXF5 -|                                      *
*                           |                                                 *
*                           |--- RXF6 --- RXB3 (Receive Buffer 3, B1)         *
*                           |--- RXF7 -|                                      *
*                                                                             *
*                                                                             *
*                           |--- RXF8 --- RXB4 (Receive Buffer 4, B2)         *
*                           |--- RXF9 -|                                      *
*                           |                                                 *
*                           |--- RXF10 -- RXB5 (Receive Buffer 5, B3)         *
*                           |--- RXF11 -|                                     *
*  RXM1 (Receive Mask 1) ---|                                                 *
*                           |--- RXF12 -- RXB6 (Receive Buffer 6, B4)         *
*                           |--- RXF13 -|                                     *
*                           |                                                 *
*                           |--- RXF14 -- RXB7 (Receive Buffer 7, B5)         *
*                           |--- RXF15 -|                                     *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*    1.0   - T.R.S. - Initial release of PIC18 ECAN driver...                 *
*                                                                             *
******************************************************************************/

#include "define.h"
#include "stderror.h"

/* Only declare the API functions as external for calling modules... */
#ifdef _DWCAN_H_
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Global macro definitions... */
#define DWCAN_TX_QUEUE_LENGTH     39 /* No more than 39 unless linker script and multi-bank buffer location changed... */
#define DWCAN_RX_QUEUE_LENGTH     39 /* No more than 39 unless linker script and multi-bank buffer location changed... */

/* Configure default CAN subsystem interrupt priority utilization... */
#define DWCAN_PRIORITY_LOW        0 /* 0 - Do not change... */
#define DWCAN_PRIORITY_HIGH       1 /* 1 - Do not change... */
#define DWCAN_TX_PRIORITY         DWCAN_PRIORITY_LOW
#define DWCAN_RX_PRIORITY         DWCAN_PRIORITY_LOW
#define DWCAN_ERROR_PRIORITY      DWCAN_PRIORITY_LOW
#define DWCAN_WAKEUP_PRIORITY     DWCAN_PRIORITY_LOW

/* Configure default CAN wake up line filtering state... */
#define DWCAN_WAKFIL_DISABLE      0 /* 0 - Do not change... */
#define DWCAN_WAKFIL_ENABLE       1 /* 1 - Do not change... */
#define DWCAN_WAKFIL_DEFAULT      DWCAN_WAKFIL_ENABLE

/* Default CAN RX acceptance mask values... */
#define DWCAN_MASK_0_DEF          0x00000000 /* Associated with Filters 0-7, wide open... */
#define DWCAN_MASK_1_DEF          0x00000000 /* Associated with Filters 8-15, wide open... */

/* Default CAN RX acceptance filter values... */
#define DWCAN_FILT_0_DEF          0x00000000 /* Associated with RXB0, wide open... */
#define DWCAN_FILT_1_DEF          0x00000000 /* Associated with RXB0, wide open... */
#define DWCAN_FILT_2_DEF          0x00000000 /* Associated with RXB1, wide open... */
#define DWCAN_FILT_3_DEF          0x00000000 /* Associated with RXB1, wide open... */
#define DWCAN_FILT_4_DEF          0x00000000 /* Associated with RXB2, wide open... */
#define DWCAN_FILT_5_DEF          0x00000000 /* Associated with RXB2, wide open... */
#define DWCAN_FILT_6_DEF          0x00000000 /* Associated with RXB3, wide open... */
#define DWCAN_FILT_7_DEF          0x00000000 /* Associated with RXB3, wide open... */
#define DWCAN_FILT_8_DEF          0x00000000 /* Associated with RXB4, wide open... */
#define DWCAN_FILT_9_DEF          0x00000000 /* Associated with RXB4, wide open... */
#define DWCAN_FILT_10_DEF         0x00000000 /* Associated with RXB5, wide open... */
#define DWCAN_FILT_11_DEF         0x00000000 /* Associated with RXB5, wide open... */
#define DWCAN_FILT_12_DEF         0x00000000 /* Associated with RXB6, wide open... */
#define DWCAN_FILT_13_DEF         0x00000000 /* Associated with RXB6, wide open... */
#define DWCAN_FILT_14_DEF         0x00000000 /* Associated with RXB7, wide open... */
#define DWCAN_FILT_15_DEF         0x00000000 /* Associated with RXB7, wide open... */

/* Default CAN baud rate declarations... */
#ifdef _25MHZ_ /* 500kBaud bit rate selection for 25MHz clock... */

  /***************************************************************************************/
  /* Initialize the CAN bit timing register...                                           */
  /*                                                                                     */
  /* |------------ CAN Bit Time (25Tq *  80ns, gives 500kBaud) ----------------------->| */
  /* |------------ Sample Point(s) at 68.000% ---------------->|                         */
  /* |-------------|-------------------|-----------------------|-----------------------| */
  /* | Synch = 1Tq | Prop. Delay = 8Tq | Time Segment 1 = 8Tq  | Time Segment 2 = 8Tq  | */
  /* |-------------|-------------------|-----------------------|-----------------------| */
  /***************************************************************************************/
#define DWCAN_BRGCON1             0x00 /* Tq = (2*(BRGCON1+1))/Fosc = (2*(0x01+1))/GBL_OSC_CLOCK = 80ns --> 25Tq per bit at 500kBaud */
#define DWCAN_SJW_1               0x00 /* SJW = 2Tq */
#define DWCAN_SJW_0               0x01 /* SJW = 2Tq */
#define DWCAN_BRGCON2             0x3F /* Prop = 8Tq, PSEG1 = 8Tq */
#define DWCAN_BRGCON3             0x07 /* PSEG2 = 8Tq */

#else /* 500kBaud bit rate selection for 20MHz clock... */

  /***************************************************************************************/
  /* Initialize the CAN bit timing register...                                           */
  /*                                                                                     */
  /* |------------ CAN Bit Time (20Tq * 100ns, gives 500kBaud) ----------------------->| */
  /* |------------ Sample Point(s) at 70.000% ---------------->|                         */
  /* |-------------|-------------------|-----------------------|-----------------------| */
  /* | Synch = 1Tq | Prop. Delay = 7Tq | Time Segment 1 = 6Tq  | Time Segment 2 = 6Tq  | */
  /* |-------------|-------------------|-----------------------|-----------------------| */
  /***************************************************************************************/
#define DWCAN_BRGCON1             0x00 /* Tq = (2*(BRGCON1+1))/Fosc = (2*(0x00+1))/GBL_OSC_CLOCK = 100ns --> 20Tq per bit at 500kBaud */
#define DWCAN_SJW_1               0x00 /* SJW = 2Tq */
#define DWCAN_SJW_0               0x01 /* SJW = 2Tq */
#define DWCAN_BRGCON2             0x2E /* Prop = 7Tq, PSEG1 = 6Tq */
#define DWCAN_BRGCON3             0x05 /* PSEG2 = 8Tq */

#endif

/* Multiple inclusion protection... */
#ifndef DWCAN_TYPES
#define DWCAN_TYPES
/* CAN status return values... */
typedef enum
{
  CAN_STATE_ERROR_ACTIVE  = 0x00,
  CAN_STATE_ERROR_PASSIVE = 0x01,
  CAN_STATE_ERROR_BUS_OFF = 0x02
} CAN_STATE;

/* CAN TX/RX buffer status return values... */
typedef enum
{
  CAN_BUF_EMPTY,
  CAN_BUF_PARTIAL,
  CAN_BUF_FULL
} CAN_BUF_STAT;

/* Available CAN acceptance filters enumerated... */
typedef enum
{
  FILT_0  = 0,
  FILT_1  = 1,
  FILT_2  = 2,
  FILT_3  = 3,
  FILT_4  = 4,
  FILT_5  = 5,
  FILT_6  = 6,
  FILT_7  = 7,
  FILT_8  = 8,
  FILT_9  = 9,
  FILT_10 = 10,
  FILT_11 = 11,
  FILT_12 = 12,
  FILT_13 = 13,
  FILT_14 = 14,
  FILT_15 = 15
} CAN_FILTER;

/* Available CAN acceptance mask enumeration... */
typedef enum
{
  MASK_0  = 0,
  MASK_1  = 1
} CAN_MASK;

/* Non-queued high priority TX message buffer enumeration... */
typedef enum
{
  MSG_BUF_1 = 0,
  MSG_BUF_2 = 1
} MSG_NON_Q;

/* CAN message typedef... */
typedef volatile struct
{
  ULONG    ulCanMsgId;
  UBYTE    ucData[8];
  BITFIELD ucDataLen:4; /* TX/RX -> Data length...0<=len<=8         */
  BITFIELD bExtended:1; /* STD_FRM=Standard ID, EXT_FRM=Extended ID */
  BITFIELD bRemote:1;   /* STD_FRM=Normal, REMOTE_FRM=Remote        */
} CAN_MSG;
#define STD_FRM    0 /* Do not change... */
#define EXT_FRM    1 /* Do not change... */
#define REMOTE_FRM 1 /* Do not change... */
#endif

/* Function declarations for DWCAN subsystem API... */
GLOBAL STDERROR     dwcan_init( void );                                        /* CAN initialization routine...         */
GLOBAL STDERROR     dwcan_sleep( void );                                       /* Prepare CAN for sleep with wakeup...  */
GLOBAL STDERROR     dwcan_halt( void );                                        /* Completely halt CAN driver...         */
GLOBAL CAN_STATE    dwcan_tx_error_status( void );                             /* Check CAN TX communications status... */
GLOBAL CAN_STATE    dwcan_rx_error_status( void );                             /* Check CAN RX communications status... */
GLOBAL CAN_BUF_STAT dwcan_tx_queue_status( void );                             /* Check CAN RX queue status...          */
GLOBAL CAN_BUF_STAT dwcan_rx_queue_status( void );                             /* Check CAN RX queue status...          */
GLOBAL STDERROR     dwcan_q_tx_message( CAN_MSG* pMsg );                       /* Queue a SW CAN TX message for TXB0... */
GLOBAL STDERROR     dwcan_polled_tx_message( MSG_NON_Q eBuf, CAN_MSG* pMsg );  /* Send a polled CAN TX msg in TXB1/TXB2 */
GLOBAL STDERROR     dwcan_get_rx_message( CAN_MSG* pMsg );                     /* Check for CAN RX messages in queue... */
GLOBAL void         dwcan_flush_tx_queue( void );                              /* Flush all queued CAN TX messages...   */
GLOBAL void         dwcan_flush_rx_queue( void );                              /* Flush all queued CAN RX messages...   */
GLOBAL STDERROR     dwcan_set_filter( CAN_FILTER eFilter, ULONG ulFilterVal ); /* WARNING: Disables CAN briefly...      */
GLOBAL STDERROR     dwcan_set_mask( CAN_MASK eMask, ULONG ulMaskVal );         /* WARNING: Disables CAN briefly...      */
GLOBAL void         dwcan_set_ovrflw_callback( far void(*ovrflw_cb)(void) );   /* Set callback for RX overflow cond...  */
GLOBAL void         dwcan_set_bus_off_callback( far void(*bus_off_cb)(void) ); /* Set callback for bus off cond...      */
GLOBAL void         dwcan_set_wake_up_callback( far void(*wake_up_cb)(void) ); /* Set callback for bus off cond...      */
GLOBAL void         dwcan_tx_int( void );                                      /* The CAN TX interrupt handler...       */
GLOBAL void         dwcan_rx_int( void );                                      /* The CAN RX interrupt handler...       */
GLOBAL void         dwcan_error_int( void );                                   /* The CAN errror interrupt handler...   */
GLOBAL void         dwcan_wake_int( void );                                    /* The CAN wake-up interrupt handler...  */
GLOBAL void         dwcan_begin_critical_region( void );                       /* Disable all ECAN interrupt sources... */
GLOBAL void         dwcan_end_critical_region( void );                         /* Enable all ECAN interrupt sources...  */

#undef GLOBAL
