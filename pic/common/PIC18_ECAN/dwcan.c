/******************************************************************************
*                                                                             *
*  File:                           DWCAN.C                                    *
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
*                           |--- RXF0 --- RXB0 (Receive Buffer 0)             *
*                           |--- RXF1 -|                                      *
*                           |                                                 *
*                           |--- RXF2 --- RXB1 (Receive Buffer 1)             *
*                           |--- RXF3 -|                                      *
*  RXM0 (Receive Mask 0) ---|                                                 *
*                           |--- RXF4 --- RXB2 (Receive Buffer 2)             *
*                           |--- RXF5 -|                                      *
*                           |                                                 *
*                           |--- RXF6 --- RXB3 (Receive Buffer 3)             *
*                           |--- RXF7 -|                                      *
*                                                                             *
*                                                                             *
*                           |--- RXF8 --- RXB4 (Receive Buffer 4)             *
*                           |--- RXF9 -|                                      *
*                           |                                                 *
*                           |--- RXF10 -- RXB5 (Receive Buffer 5)             *
*                           |--- RXF11 -|                                     *
*  RXM1 (Receive Mask 0) ---|                                                 *
*                           |--- RXF12 -- RXB6 (Receive Buffer 6)             *
*                           |--- RXF13 -|                                     *
*                           |                                                 *
*                           |--- RXF14 -- RXB7 (Receive Buffer 7)             *
*                           |--- RXF15 -|                                     *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*    1.0   - T.R.S. - Initial release of PIC18 single port ECAN driver...     *
*                                                                             *
******************************************************************************/

#include "define.h"      /* Standard type declarations...          */
#include <p18cxxx.h>     /* PIC18F4680 register definition file... */
#include "stderror.h"    /* Standard error types...                */
#include <string.h>      /* For the C memset() routine...          */
#include "system.h"

/* Multiple inclusion protection for API functions and data... */
#define  _DWCAN_H_
#include "dwcan.h"
#undef   _DWCAN_H_

/* Local Macro definitions... */
#define DWCAN_WAKEIE_ENABLE     PIE3bits.WAKIE  = 1
#define DWCAN_WAKEIE_DISABLE    PIE3bits.WAKIE  = 0
#define DWCAN_ERRIE_ENABLE      PIE3bits.ERRIE  = 1
#define DWCAN_ERRIE_DISABLE     PIE3bits.ERRIE  = 0
#define DWCAN_RXBIE_ENABLE      PIE3bits.RXBnIE = 1
#define DWCAN_RXBIE_DISABLE     PIE3bits.RXBnIE = 0
#define DWCAN_TXB0IE_ENABLE     PIE3bits.TXBnIE = 1
#define DWCAN_TXB0IE_DISABLE    PIE3bits.TXBnIE = 0
#define DWCAN_TXB1IE_ENABLE     TXBIEbits.TXB1IE = 1
#define DWCAN_TXB1IE_DISABLE    TXBIEbits.TXB1IE = 0
#define DWCAN_TXB2IE_ENABLE     TXBIEbits.TXB2IE = 1
#define DWCAN_TXB2IE_DISABLE    TXBIEbits.TXB2IE = 0

/* EICODE bit helper macro definitions (Mode 1 "Enhanced Legacy" Only)... */
#define EWIN_EICODE_BIT_MASK    0x1F
#define READ_EICODE_BITS        (CANSTAT & EWIN_EICODE_BIT_MASK)
#define EICODE_NO_INTERRUPT     0x00
#define EICODE_ERROR            0x02
#define EICODE_TXB2             0x04
#define EICODE_TXB1             0x06
#define EICODE_TXB0             0x08
#define EICODE_WAKEUP           0x0E
#define EICODE_RXB0             0x10
#define EICODE_RXB1             0x11
#define EICODE_RXTXB0           0x12
#define EICODE_RXTXB1           0x13
#define EICODE_RXTXB2           0x14
#define EICODE_RXTXB3           0x15
#define EICODE_RXTXB4           0x16
#define EICODE_RXTXB5           0x17

/* EWIN bit helper macro definitions (Mode 1 "Enhanced Legacy" Only)... */
#define WRITE_EWIN_BITS(Y)      ECANCON = ((ECANCON & (~EWIN_EICODE_BIT_MASK)) | ((Y) & EWIN_EICODE_BIT_MASK))
#define EWIN_TXB0               0x03
#define EWIN_TXB1               0x04
#define EWIN_TXB2               0x05
#define EWIN_RXB0               0x10
#define EWIN_RXB1               0x11
#define EWIN_RXTXB0             0x12
#define EWIN_RXTXB1             0x13
#define EWIN_RXTXB2             0x14
#define EWIN_RXTXB3             0x15
#define EWIN_RXTXB4             0x16
#define EWIN_RXTXB5             0x17

/* CAN operating mode helper macro definitions (Mode 1 "Enhanced Legacy" Only)... */
#define OP_MODE_BIT_MASK        0xE0
#define GET_OP_MODE             (CANSTAT & OP_MODE_BIT_MASK)
#define OP_MODE_NORMAL          0x00
#define OP_MODE_SLEEP_DIS       0x20
#define OP_MODE_LOOPBACK        0x40
#define OP_MODE_LISTEN_ONLY     0x60
#define OP_MODE_CONFIGURE       0x80
#define REQ_OP_MODE(Z)          CANCON = ((CANCON & (~OP_MODE_BIT_MASK)) | ((Z) & OP_MODE_BIT_MASK) )

/* Local function declarations... */
static void         send_can_msg( void );
static void         send_can_msg_polled( CAN_MSG* pTxMsg );
static void         receive_can_msg( void );
static void         flush_tx_queue( void );
static void         flush_rx_queue( void );
static CAN_BUF_STAT tx_queue_status( void );
static CAN_BUF_STAT rx_queue_status( void );
static STDERROR     set_filter( CAN_FILTER eFilter, ULONG ulFilterVal );
static STDERROR     set_mask( CAN_MASK eMask, ULONG ulMaskVal );

/* Local variable declarations... */
static volatile CAN_STATE eTxCanState;
static volatile CAN_STATE eRxCanState;
#pragma udata CANTXBUF
/* Always use "pTxQueue" to access this multi-bank array... */
static volatile CAN_MSG pCanTxQueue[DWCAN_TX_QUEUE_LENGTH];
#pragma udata CANRXBUF
/* Always use "pRxQueue" to access this multi-bank array... */
static volatile CAN_MSG pCanRxQueue[DWCAN_RX_QUEUE_LENGTH];
#pragma udata
static volatile CAN_MSG* pTxQueue;
static volatile CAN_MSG* pRxQueue;
static volatile UBYTE ucTxGetPtr;
static volatile UBYTE ucTxPutPtr;
static volatile UBYTE ucRxGetPtr;
static volatile UBYTE ucRxPutPtr;
static volatile UBYTE ucQueuedTxActive;
far void (*ovrflw_callback)(void);  /* Callback function pointer for RX overflow condition... */
far void (*bus_off_callback)(void); /* Callback function pointer for CAN bus off condition... */
far void (*wake_up_callback)(void); /* Callback function pointer for CAN wakeup condition... */

/******************************************************************************
*                      GLOBAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: STDERROR dwcan_init( void )                                      *
*  Description: This function will initialize the ECAN interface for the      *
*               PIC18F-X585/X680 in "Enhanced Legacy" mode configured for 8   *
*               receive and 3 transmit buffers. This function is designed to  *
*               work properly from a fresh power up or from a run-time        *
*               re-initialization...                                          *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_init( void )
{
  STDERROR erReturnCode;

  /* Disable any current CAN activity and enter configuration mode... */
  erReturnCode = dwcan_halt();

  /* Configure ECAN I/O pins properly... */
  TRISBbits.TRISB3  = 1; /* Make sure that CAN RX pin is an input... */
  CIOCONbits.ENDRHI = 1; /* CAN TX pin drive Vdd for recessive state (vs. Tri-State when recessive) */
  CIOCONbits.CANCAP = 0; /* Do not use CCP1 for timer capture with CAN module... */

  /* Initialize CAN port variables... */
  eTxCanState = CAN_STATE_ERROR_ACTIVE;
  eRxCanState = CAN_STATE_ERROR_ACTIVE;
  flush_tx_queue();
  flush_rx_queue();
  ucQueuedTxActive = FALSE;
  ovrflw_callback  = NULL; /* Empty function callback pointers by default... */
  bus_off_callback = NULL; /* Empty function callback pointers by default... */
  wake_up_callback = NULL; /* Empty function callback pointers by default... */

  /* Initialize CAN port queues and access pointers... */
  pTxQueue = &(pCanTxQueue[0]); /* Only access CAN_MSG TX queues through this pointer... */
  pRxQueue = &(pCanRxQueue[0]); /* Only access CAN_MSG RX queues through this pointer... */
  memset( ((void*)pTxQueue), 0, (DWCAN_TX_QUEUE_LENGTH * sizeof(CAN_MSG)) );
  memset( ((void*)pRxQueue), 0, (DWCAN_RX_QUEUE_LENGTH * sizeof(CAN_MSG)) );

  /* Select operating mode for ECAN module as mode 1 "Enhanced Legacy" mode... */
  ECANCONbits.MDSEL1 = 0;
  ECANCONbits.MDSEL0 = 1;
  ECANCONbits.FIFOWM = 0; /* Not used in mode 1 "Enhanced Legacy" mode... */
  WRITE_EWIN_BITS( EWIN_RXB0 ); /* Map RXB0 to itself in access RAM before continuing... */

  /* Setup for requested ECAN baud rate (based on values defined in header file)... */
  BRGCON1 = DWCAN_BRGCON1;
  BRGCON1bits.SJW1 = DWCAN_SJW_1;
  BRGCON1bits.SJW0 = DWCAN_SJW_0;
  BRGCON2 = DWCAN_BRGCON2;
  BRGCON2bits.SEG2PHTS = 1; /* Freely programmable PSEG2 */
  BRGCON3 = DWCAN_BRGCON3;
  BRGCON3bits.WAKFIL = 0; /* Disable the wakeup line filtering feature... */
  BRGCON3bits.WAKDIS = 1; /* Disable the CAN wakeup feature... */

  /* Setup CAN message acceptance filter/mask registers to receive all standard/extended ID CAN messages... */
  set_mask( MASK_0, DWCAN_MASK_0_DEF );
  set_mask( MASK_1, DWCAN_MASK_1_DEF );
  MSEL0 = 0x00; /* Filters 00-03 associated with acceptance mask 0 */
  MSEL1 = 0x00; /* Filters 04-07 associated with acceptance mask 0 */
  MSEL2 = 0x55; /* Filters 08-11 associated with acceptance mask 1 */
  MSEL3 = 0x55; /* Filters 12-15 associated with acceptance mask 1 */
  SDFLC = 0x00; /* Do NOT filter on any CAN message data bytes!!! (this is typically a DeviceNet feature) */
  set_filter( FILT_0,  DWCAN_FILT_0_DEF  );
  set_filter( FILT_1,  DWCAN_FILT_1_DEF  );
  set_filter( FILT_2,  DWCAN_FILT_2_DEF  );
  set_filter( FILT_3,  DWCAN_FILT_3_DEF  );
  set_filter( FILT_4,  DWCAN_FILT_4_DEF  );
  set_filter( FILT_5,  DWCAN_FILT_5_DEF  );
  set_filter( FILT_6,  DWCAN_FILT_6_DEF  );
  set_filter( FILT_7,  DWCAN_FILT_7_DEF  );
  set_filter( FILT_8,  DWCAN_FILT_8_DEF  );
  set_filter( FILT_9,  DWCAN_FILT_9_DEF  );
  set_filter( FILT_10, DWCAN_FILT_10_DEF );
  set_filter( FILT_11, DWCAN_FILT_11_DEF );
  set_filter( FILT_12, DWCAN_FILT_12_DEF );
  set_filter( FILT_13, DWCAN_FILT_13_DEF );
  set_filter( FILT_14, DWCAN_FILT_14_DEF );
  set_filter( FILT_15, DWCAN_FILT_15_DEF );

  /* Configure TXB0 message buffer used for queued CAN message transmission... */
  TXB0CONbits.TXPRI1 = 0; /* Priority 1 (low), used for queued message transmission...  */
  TXB0CONbits.TXPRI0 = 1;
  TXBIEbits.TXB0IE   = 1; /* Enable interrupts on TXB0... */
  TXB1CONbits.TXPRI1 = 1; /* Priority 2 (med.), may be used for non-queued CAN message support in future... */
  TXB1CONbits.TXPRI0 = 0;
  TXBIEbits.TXB1IE   = 0; /* Disable interrupts on TXB1... */
  TXB2CONbits.TXPRI1 = 1; /* Priority 3 (high), may be used for non-queued CAN message support in future... */
  TXB2CONbits.TXPRI0 = 1;
  TXBIEbits.TXB2IE   = 0; /* Disable interrupts on TXB2... */

  /* Configure RX message buffers and link to masks/filters... */
  BIE0     = 0xFF; /* Enable all CAN receive buffers to generate interrupts... */
  BSEL0    = 0x00; /* Make all configurable TX/RX buffers into RX buffers for a total of 8 RX buffers... */
  RXB0CON  = 0x00; /* Clear RXB0 RXFUL and RXM1 bits...  */
  RXFCON0  = 0xFF;
  RXFBCON0 = 0x00; /* Mask 0 and receive filters 0-1 assigned to RXB0... */
  RXB1CON  = 0x00; /* Clear RXB1 RXFUL and RXM1 bits...  */
  RXFCON1  = 0xFF;
  RXFBCON1 = 0x11; /* Mask 0 and receive filters 2-3 assigned to RXB1... */
  B0CON    = 0x00; /* Clear B0 RXFUL and RXM1 bits...  */
  RXFBCON2 = 0x22;  /* Mask 0 and receive filters 4-5 assigned to B0... */
  B1CON    = 0x00; /* Clear B1 RXFUL and RXM1 bits...  */
  RXFBCON3 = 0x33;  /* Mask 0 and receive filters 6-7 assigned to B1... */
  B2CON    = 0x00; /* Clear B2 RXFUL and RXM1 bits...  */
  RXFBCON4 = 0x44;  /* Mask 1 and Receive filters 8-9 assignedto B2... */
  B3CON    = 0x00; /* Clear B3 RXFUL and RXM1 bits...  */
  RXFBCON5 = 0x55;  /* Mask 1 and receive filters 10-11 assigned to B3... */
  B4CON    = 0x00; /* Clear B4 RXFUL and RXM1 bits...  */
  RXFBCON6 = 0x66;  /* Mask 1 and receive filters 12-13 assigned to B4... */
  B5CON    = 0x00; /* Clear B5 RXFUL and RXM1 bits...  */
  RXFBCON7 = 0x77;  /* Mask 1 and receive filters 14-15 assigned to B5... */

  /* Setup interrupt priorities for various CAN interrupt sources... */
  IPR3bits.TXBnIP = DWCAN_TX_PRIORITY;
  IPR3bits.RXBnIP = DWCAN_RX_PRIORITY;
  IPR3bits.ERRIP  = DWCAN_ERROR_PRIORITY;
  IPR3bits.WAKIP  = DWCAN_WAKEUP_PRIORITY;

  /* Clear any pending interrupt flags and setup error interrupts... */
  PIR3 = 0x00; /* Clear any pending CAN interrupt flags... */

  /* Exit ECAN configuration mode, go to "normal" run mode... */
  REQ_OP_MODE( OP_MODE_NORMAL );

  /* Enable the CAN port receive interrupts only; transmit interrupts only get enabled as needed... */
  dwcan_end_critical_region();

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR dwcan_sleep( void )                                     *
*  Description: This function prepares the ECAN module for sleep mode with    *
*               wakeup interrupts on CAN activity enabled. Note that if the   *
*               GIEH and GIEL bits are enabled and an interrupt occurs that   *
*               the interrupt vector will be taken. If GIEH and GIEL are      *
*               disabled while you are sleeping (after executing a sleep      *
*               instruction) the wake-up interrupt flag will wakeup the       *
*               processor and code execution will begin immediately after the *
*               sleep instruction (no interrupt vector taken)! Remember that  *
*               this should not be called until the CAN bus is assumed to be  *
*               idle just before putting the processor to sleep...            *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_sleep( void )
{
  STDERROR erReturnCode;

  /* Halt the ECAN module and enter configuration mode... */
  erReturnCode = dwcan_halt();

  /* Prepare the ECAN module for sleep with wake up on CAN activity enabled... */
  BRGCON3bits.WAKFIL = DWCAN_WAKFIL_DEFAULT;
  BRGCON3bits.WAKDIS = 0; /* Enable the CAN wakeup feature... */

  /* Clear ECAN wakeup interrupt indication and enable the interrupt... */
  PIR3bits.WAKIF = 0;
  DWCAN_WAKEIE_ENABLE;

  /* Enter "disable mode" leaving only the wake-up on bus CAN activity interrupt enabled... */
  REQ_OP_MODE(OP_MODE_SLEEP_DIS);
  while ( GET_OP_MODE != OP_MODE_SLEEP_DIS );

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR dwcan_halt( void )                                      *
*  Description: This function completely disables the CAN peripheral and      *
*               enters ECAN configuration mode...                             *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_halt( void )
{
  STDERROR erReturnCode = ERR(DWCAN,SUCCESS);

  /* Disable the CAN port interrupts... */
  dwcan_begin_critical_region();

  /* Abort any pending transmissions, enter ECAN configuration mode... */
  CANCONbits.ABAT = 1; /* Set global transmission abort flag... */
  REQ_OP_MODE( OP_MODE_CONFIGURE );
  while ( GET_OP_MODE != OP_MODE_CONFIGURE );
  CANCONbits.ABAT = 0; /* Clear global transmission abort flag... */

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: CAN_STATE dwcan_tx_error_status( void )                          *
*  Description: This function returns the current TX state of the requested   *
*               CAN port indicating whether it is currently in an error state *
*               or is functioning properly.                                   *
*  Returns: CAN_STATE                                                         *
*                                                                             *
******************************************************************************/
CAN_STATE dwcan_tx_error_status( void )
{
  CAN_STATE eCanState;

  /* Begin ERROR interrupt critical region... */
  DWCAN_ERRIE_DISABLE;

  /* Get the CAN transmit state for this port... */
  eCanState = eTxCanState;

  /* End ERROR interrupt critical region... */
  DWCAN_ERRIE_ENABLE;

  return eCanState;
}

/******************************************************************************
*                                                                             *
*  Function: CAN_STATE dwcan_rx_error_status( void )                          *
*  Description: This function returns the current RX state of the requested   *
*               CAN port indicating whether it is currently in an error state *
*               or is functioning properly.                                   *
*  Returns: CAN_STATE                                                         *
*                                                                             *
******************************************************************************/
CAN_STATE dwcan_rx_error_status( void )
{
  CAN_STATE eCanState;

  /* Begin ERROR interrupt critical region... */
  DWCAN_ERRIE_DISABLE;

  /* Get the CAN receive state for this port... */
  eCanState = eRxCanState;

  /* End ERROR interrupt critical region... */
  DWCAN_ERRIE_ENABLE;

  return eCanState;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_tx_wueue_status( void )                                    *
*  Description: This function returns the status of the software based CAN TX *
*               buffer for the requested port whether it is empty, full, or   *
*               partially full.                                               *
*  Returns: CAN_BUF_STAT                                                      *
*                                                                             *
******************************************************************************/
CAN_BUF_STAT dwcan_tx_queue_status( void )
{
  CAN_BUF_STAT enCanStatus;

  /* Begin TX critical region for requested port... */
  DWCAN_TXB0IE_DISABLE;

  /* Retrieve the TX queue status... */
  enCanStatus = tx_queue_status();

  /* Only re-enable the transmit interrupts if we are actively transmitting... */
  if ( ucQueuedTxActive == TRUE )
  {
    /* End TX critical region for requested port... */
    DWCAN_TXB0IE_ENABLE;
  }

  return enCanStatus;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_rx_queue_status( void )                                    *
*  Description: This function returns the status of the software based CAN RX *
*               buffer for the requested port whether itis empty, full, or    *
*               partially full.                                               *
*  Returns: CAN_BUF_STAT                                                      *
*                                                                             *
******************************************************************************/
CAN_BUF_STAT dwcan_rx_queue_status( void )
{
  CAN_BUF_STAT enCanStatus;

  /* Begin RX critical region for requested port... */
  DWCAN_RXBIE_DISABLE;

  /* Retrieve the RX queue status for requested port... */
  enCanStatus = rx_queue_status();

  /* End RX critical region for requested port... */
  DWCAN_RXBIE_ENABLE;

  return enCanStatus;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_q_tx_message( CAN_MSG* pMsg )                              *
*  Description: This function will attempt to queue a CAN TX message in the   *
*               software TX queue of the requested port. If the requested     *
*               ports queue is full or the CAN port is non-functional then an *
*               error code will be returned.                                  *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_q_tx_message( CAN_MSG* pMsg )
{
  STDERROR erReturnCode = ERR(DWCAN,SUCCESS);

  /* Begin TX critical region for requested port... */
  DWCAN_TXB0IE_DISABLE;

  /* See if we can queue the CAN message... */
  switch( tx_queue_status() )
  {
    case CAN_BUF_PARTIAL:
    case CAN_BUF_EMPTY:
      /* Queue the new message in the CAN transmit queue... */
      pTxQueue[ucTxPutPtr] = *pMsg;

      /* Advance the "Put" pointer... */
      ucTxPutPtr += 1;
      if ( (ucTxPutPtr) > (DWCAN_TX_QUEUE_LENGTH-1) ) ucTxPutPtr = 0;

      /* See if transmission is currently inactive... */
      if ( ucQueuedTxActive == FALSE )
      {
        /* Manually send the first message to kick off */
        /* CAN transmits since CAN TX has been idle... */
        send_can_msg();
        
        /* Clear the transmit interrupt indication... */
        PIR3bits.TXBnIF = 0;
      }
      break;

    case CAN_BUF_FULL:
      erReturnCode = ERR(DWCAN,QUEUE_FULL);
      break;

    default:
      erReturnCode = ERR(DWCAN,FAILURE);
      break;
  }

  /* Only re-enable the transmit interrupts if we are actively transmitting... */
  if ( ucQueuedTxActive == TRUE )
  {
    /* End TX critical region for requested port... */
    DWCAN_TXB0IE_ENABLE;
  }

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR dwcan_polled_tx_message( MSG_NON_Q eBuf, CAN_MSG* pMsg )*
*  Description: This function will attempt to transmit a CAN message in the   *
*               requested CAN buffer. This allows for an alternative to queued*
*               CAN transmission through TXB0 for messages that have to have  *
*               very low latency. There are three dedicated transmit buffers  *
*               in the PIC18 ECAN module. They each have an assignable        *
*               transmit priority. The function dwcan_init() automatically    *
*               sets up TXB0 as the lowest priority followed by TXB1(med) and *
*               TXB2(high). These buffers are used in a polled mode. This     *
*               function will successfully setup a transmit operation as long *
*               as the previous transfer has completed.                       *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_polled_tx_message( MSG_NON_Q eBuf, CAN_MSG* pMsg )
{
  STDERROR erReturnCode = ERR(DWCAN,SUCCESS);
  UBYTE ucContinue = FALSE;

  /* See what buffer is being requested and see if it is available... */
  switch ( eBuf )
  {
    case MSG_BUF_1:
      /* See if TXB1 is idle and can now be utilized... */
      if ( TXB1CONbits.TXREQ == 0 )
      {
        /* Point at TXB1 using the EWIN bits in preparation for transmit... */
        WRITE_EWIN_BITS(EWIN_TXB1);
        ucContinue = TRUE;
      }
      else
      {
        erReturnCode = ERR(DWCAN,BUFFER_FULL);
      }
      break;

    case MSG_BUF_2:
      /* See if TXB2 is idle and can now be utilized... */
      if ( TXB2CONbits.TXREQ == 0 )
      {
        /* Point at TXB2 using the EWIN bits in preparation for transmit... */
        WRITE_EWIN_BITS(EWIN_TXB2);
        ucContinue = TRUE;
      }
      else
      {
        erReturnCode = ERR(DWCAN,BUFFER_FULL);
      }
      break;

    default:
      erReturnCode = ERR(DWCAN,INVALID_PARAM);
      break;
  }

  /* If the buffer was free, send the message... */
  if ( ucContinue )
  {
    /* Transmit the message in TXB selected by using current EWIN bit settings... */
    send_can_msg_polled( pMsg );
  }

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_get_rx_message( CAN_MSG* pMsg )                            *
*  Description: This function will return a new CAN receive message if there  *
*               is one in the CAN RX queue for the requested port. If there   *
*               is not a message available then an error will be returned.    *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_get_rx_message( CAN_MSG* pMsg )
{
  STDERROR erReturnCode;

  /* Begin RX critical region for requested port... */
  DWCAN_RXBIE_DISABLE;

  /* See if we have an RX CAN message to give the caller... */
  switch( rx_queue_status() )
  {
    case CAN_BUF_FULL:
    case CAN_BUF_PARTIAL:
      /* Copy RX CAN message to callers memory... */
      *pMsg = pRxQueue[ucRxGetPtr];

      /* Release the RX CAN message... */
      ucRxGetPtr += 1;
      if ( (ucRxGetPtr) > (DWCAN_RX_QUEUE_LENGTH-1) ) ucRxGetPtr = 0;

      /* Indicate success... */
      erReturnCode = ERR(DWCAN,SUCCESS);
      break;

    case CAN_BUF_EMPTY:
      erReturnCode = ERR(DWCAN,QUEUE_EMPTY);
      break;

    default:
      erReturnCode = ERR(DWCAN,FAILURE);
      break;
  }

  /* End RX critical region for requested port... */
  DWCAN_RXBIE_ENABLE;

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_tx_int( void )                                        *
*  Description: This function is the single function called from all TX       *
*               interrupt routines that processes all transmit interrupts for *
*               all CAN ports and must be re-entrant.                         *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_tx_int( void )
{
  /* See if we have any more CAN messages to transmit... */
  if ( tx_queue_status() != CAN_BUF_EMPTY )
  {
    /* Send the next message... */
    send_can_msg();
  }
  else /* No more CAN messages to transmit... */
  {
    /* Disable CAN transmit interrupts, indicate that TX is idle... */
    DWCAN_TXB0IE_DISABLE;
    ucQueuedTxActive = FALSE;
  }
  
  /* Clear the transmit interrupt indication... */
  PIR3bits.TXBnIF = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_rx_int( void )                                        *
*  Description: This function is the single function called from all RX       *
*               interrupt routines that processes all receive interrupts for  *
*               all CAN ports and must be re-entrant.                         *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_rx_int( void )
{
  /* Check RXB0 for a new message... */
  WRITE_EWIN_BITS(EWIN_RXB0); /* Must map RXB0 to access RAM using EWIN bits!!! */
  if ( RXB0CONbits.RXFUL )
  {
    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }
      
      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB1 for a new message... */
  if ( RXB1CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXB1);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB2 for a new message... */
  if ( B0CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB0);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB3 for a new message... */
  if ( B1CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB1);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB4 for a new message... */
  if ( B2CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB2);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB5 for a new message... */
  if ( B3CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB3);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB6 for a new message... */
  if ( B4CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB4);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Check RXB7 for a new message... */
  if ( B5CONbits.RXFUL )
  {
    /* Set EWIN bits to map correct RX buffer into universal access RAM location... */
    WRITE_EWIN_BITS(EWIN_RXTXB5);

    /* Check the CAN software RX queue for free space... */
    if ( rx_queue_status() != CAN_BUF_FULL )
    {
      /* Read the new message from the CAN hardware into RX buffer... */
      receive_can_msg();
    }
    else
    {
      /* See if an overflow callback function address has been configured... */
      if ( ovrflw_callback != NULL )
      {
        /* Signal overflow to application layer through callback function... */
        ovrflw_callback();
      }

      /* Clear the receive message pending flag... */
      RXB0CONbits.RXFUL = 0;
    }
  }

  /* Clear the receive interrupt indication... */
  PIR3bits.RXBnIF = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_error_int( void )                                     *
*  Description: This function is the error interrupt handler that monitors    *
*               CAN change of state interrupts and receiver overflow          *
*               conditions...                                                 *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_error_int( void )
{
  /* See if we had a CAN RX overflow interrupt... */
  if ( COMSTATbits.RXBnOVFL )
  {
    /* See if an overflow callback function address has been configured... */
    if ( ovrflw_callback != NULL )
    {
      /* Signal overflow to application layer through callback function... */
      ovrflw_callback();
    }

    /* Clear ECAN RX buffer overflow interrupt indication... */
    COMSTATbits.RXBnOVFL = 0;
  }

  /* See if ECAN RX state change has been issued... */
  if ( COMSTATbits.RXBP )
  {
    /* Indicate that we are now in the RX error passive state... */
    eRxCanState = CAN_STATE_ERROR_PASSIVE;
  }
  else if ( COMSTATbits.RXWARN )
  {
    /* Indicate that we are now in the RX error active state... */
    eRxCanState = CAN_STATE_ERROR_ACTIVE;
  }
  else
  {
    /* Indicate that we are now in the RX error active state... */
    eRxCanState = CAN_STATE_ERROR_ACTIVE;
  }
  
  /* See if ECAN TX state change has been issued... */
  if ( COMSTATbits.TXBO )
  {
    /* Indicate that we are now in the bus off state... */
    eTxCanState = CAN_STATE_ERROR_BUS_OFF;

    /* See if a bus off callback function address has been configured... */
    if ( bus_off_callback != NULL )
    {
      /* Signal bus off transition to application layer through callback function... */
      bus_off_callback();
    }
  }
  else if ( COMSTATbits.TXBP )
  {
    /* Indicate that we are now in the TX error passive state... */
    eTxCanState = CAN_STATE_ERROR_PASSIVE;
  }
  else if ( COMSTATbits.TXWARN )
  {
    /* Indicate that we are now in the TX error active state... */
    eTxCanState = CAN_STATE_ERROR_ACTIVE;
  }
  else
  {
    /* Indicate that we are now in the TX error active state... */
    eTxCanState = CAN_STATE_ERROR_ACTIVE;
  }

  /* Clear ECAN error interrupt indication... */
  PIR3bits.ERRIF = 0;
  PIR3bits.IRXIF = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_wake_int( void )                                      *
*  Description: This function is the wakeup interrupt handler ...             *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_wake_int( void )
{
  /* See if a wake up callback function address has been configured... */
  if ( wake_up_callback != NULL )
  {
    /* Signal CAN wakeup to application layer through callback function... */
    wake_up_callback();
  }

  /* Clear ECAN wakeup interrupt indication... */
  PIR3bits.WAKIF = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_flush_tx_queue( void )                                *
*  Description: This function flushes all pending objects out of the ports    *
*               CAN transmit queue. It does not flush transmit objects that   *
*               are already in the CAN hardware transmit objects which are    *
*               pending.                                                      *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_flush_tx_queue( void )
{
  /* Begin TX critical region for requested port... */
  DWCAN_TXB0IE_DISABLE;

  /* Flush the requested port's transmit queue... */
  flush_tx_queue();

  /* Only re-enable the transmit interrupts if we are actively transmitting... */
  if ( ucQueuedTxActive == TRUE )
  {
    /* End TX critical region for requested port... */
    DWCAN_TXB0IE_ENABLE;
  }
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_flush_rx_queue( void )                                *
*  Description: This function flushes all pending RX objects from the CAN     *
*               receive queue...                                              *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void dwcan_flush_rx_queue( void )
{
  /* Begin RX critical region for requested port... */
  DWCAN_RXBIE_DISABLE;

  /* Flush the receive queue... */
  flush_rx_queue();

  /* End RX critical region for requested port... */
  DWCAN_RXBIE_ENABLE;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_set_filter( CAN_FILTER eFilter, ULONG ulFilterVal )        *
*  Description: This function is the public API call used to update filter    *
*               values when CAN is already running. Since filter registers    *
*               can only be modified while the ECAN module is in configuration*
*               mode we must perform the switch to configuration mode and     *
*               back to normal mode here, setting the filter registers while  *
*               in configuration mode. It is important to realize that CAN    *
*               messages could be missed during filter register changes!!!    *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_set_filter( CAN_FILTER eFilter, ULONG ulFilterVal )
{
  STDERROR erReturnCode;

  /* Enter ECAN configuration mode... */
  REQ_OP_MODE( OP_MODE_CONFIGURE );
  while ( GET_OP_MODE != OP_MODE_CONFIGURE );

  /* Make call to actually change acceptance filter registers... */
  erReturnCode = set_filter( eFilter, ulFilterVal );

  /* Exit ECAN configuration mode... */
  REQ_OP_MODE( OP_MODE_NORMAL );

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: dwcan_set_mask( CAN_MASK eMask, ULONG ulMaskVal )                *
*  Description: This function is the public API call used to update mask      *
*               values when CAN is already running. Since mask registers      *
*               can only be modified while the ECAN module is in configuration*
*               mode we must perform the switch to configuration mode and     *
*               back to normal mode here, setting the mask registers while    *
*               in configuration mode. It is important to realize that CAN    *
*               messages could be missed during mask register changes!!!      *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR dwcan_set_mask( CAN_MASK eMask, ULONG ulMaskVal )
{
  STDERROR erReturnCode;

  /* Enter configuration mode... */
  REQ_OP_MODE( OP_MODE_CONFIGURE );
  while ( GET_OP_MODE != OP_MODE_CONFIGURE );

  /* Make call to actually change acceptance mask registers... */
  erReturnCode = set_mask( eMask, ulMaskVal );

  /* Exit configuration mode... */
  REQ_OP_MODE( OP_MODE_NORMAL );

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_set_ovrflw_callback( far void(*ovrflw_cb)(void) )     *
*  Description: This function stores a new value for the callback function    *
*               pointer that is used to allow application developers to       *
*               capture and handle CAN RX message overflow conditions and     *
*               handle them appropriately based on their application needs... *
*                                                                             *
*               The function pointer passed in must be declared as:           *
*               far void <func_name>( void );                                 *
*                                                                             *
*  Returns: void                                                              *
*                                                                             *
******************************************************************************/
void dwcan_set_ovrflw_callback( far void(*ovrflw_cb)(void) )
{
  /* Enter ERROR interrupt critical region... */
  DWCAN_ERRIE_DISABLE;

  /* Store the new CAN RX overflow callback function pointer... */
  ovrflw_callback = ovrflw_cb;

  /* Exit ERROR interrupt critical region... */
  DWCAN_ERRIE_ENABLE;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_set_bus_off_callback( far void(*bus_off_cb)(void) )   *
*  Description: This function stores a new value for the callback function    *
*               pointer that is used to allow application developers to       *
*               capture and handle CAN bus off conditions and handle them     *
*               appropriately based on their application needs...             *
*                                                                             *
*               The function pointer passed in must be declared as:           *
*               far void <func_name>( void );                                 *
*                                                                             *
*  Returns: void                                                              *
*                                                                             *
******************************************************************************/
void dwcan_set_bus_off_callback( far void(*bus_off_cb)(void) )
{
  /* Enter ERROR interrupt critical region... */
  DWCAN_ERRIE_DISABLE;

  /* Store the new CAN bus off condition callback function pointer... */
  bus_off_callback = bus_off_cb;

  /* Exit ERROR interrupt critical region... */
  DWCAN_ERRIE_ENABLE;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_set_wake_up_callback( far void(*wake_up_cb)(void) )   *
*  Description: This function stores a new value for the callback function    *
*               pointer that is used to allow application developers to       *
*               capture and handle CAN wake up conditions and handle them     *
*               appropriately based on their application needs. This function *
*               pointer must be set before any call to dwcan_sleep() since we *
*               are not protected by a critical region...                     *
*                                                                             *
*               The function pointer passed in must be declared as:           *
*               far void <func_name>( void );                                 *
*                                                                             *
*  Returns: void                                                              *
*                                                                             *
******************************************************************************/
void dwcan_set_wake_up_callback( far void(*wake_up_cb)(void) )
{
  /* Store the new CAN wake up condition callback function pointer... */
  wake_up_callback = wake_up_cb;
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_begin_critical_region( void )                         *
*  Description: This function disables all ECAN interrupt sources...          *
*                                                                             *
*  Returns: void                                                              *
*                                                                             *
******************************************************************************/
void dwcan_begin_critical_region( void )
{
  /* Disable all ECAN interrupt sources... */
  DWCAN_RXBIE_DISABLE;
  DWCAN_TXB0IE_DISABLE;
  DWCAN_ERRIE_DISABLE;
  DWCAN_WAKEIE_DISABLE; /* For good measure, could have been enabled by dwcan_sleep()... */
  DWCAN_TXB1IE_DISABLE; /* Disable for good measure, always polled... */
  DWCAN_TXB2IE_DISABLE; /* Disable for good measure, always polled... */
}

/******************************************************************************
*                                                                             *
*  Function: void dwcan_end_critical_region( void )                           *
*  Description: This function enables all ECAN interrupt sources taking into  *
*               account transmitter status...                                 *
*                                                                             *
*  Returns: void                                                              *
*                                                                             *
******************************************************************************/
void dwcan_end_critical_region( void )
{
  /* Enable normal mode ECAN interrupt sources (not including wake-up interrupt) */
  DWCAN_ERRIE_ENABLE;
  DWCAN_RXBIE_ENABLE;

  /* Only re-enable the transmit interrupts if we are actively transmitting... */
  if ( ucQueuedTxActive == TRUE )
  {
    /* End TX critical region for requested port... */
    DWCAN_TXB0IE_ENABLE;
  }
}

/*****************************************************************************
*                       LOCAL FUNCTION DEFINITIONS                           *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: void send_can_msg( void )                                        *
*  Description: This function takes a TX message from the CAN TX queue for    *
*               the specified port and transmits it by loading it into the    *
*               CAN TX object and setting the appropriate flags. Remember that*
*               ECANCON must be saved in the low and high priority interrupt  *
*               context because of the method used to acces the TXB0 using the*
*               EWIN bits...                                                  *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void send_can_msg( void )
{
  /* Map TXB0 into access RAM; faster access and smaller shared TX code using EWIN bits... */
  WRITE_EWIN_BITS(EWIN_TXB0);

  /* Push the CAN TX message into TXB0 and request transmission... */
  send_can_msg_polled( &(pTxQueue[ucTxGetPtr]) );

  /* Indicate that queued transmits are active... */
  ucQueuedTxActive = TRUE;

  /* Release the loaded CAN transmit message from the software queue... */
  ucTxGetPtr++;
  if ( ucTxGetPtr >= DWCAN_TX_QUEUE_LENGTH ) ucTxGetPtr = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void send_can_msg_polled( CAN_MSG* pTxMsg )                      *
*  Description: This function takes a TX message passed and loads it into the *
*               transmit buffer pointed at by the EWIN bits. The EWIN bits map*
*               either TXB0, TXB1 or TXB2 into the same near access RAM space *
*               as RXB0!!!                                                    *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
#define _TXRTR           0x40
#define EID_16_17_MASK   0x03
#define EID_18_20_MASK   0xE0
static void send_can_msg_polled( CAN_MSG* pTxMsg )
{
  UBYTE ucTemp;

  /* Disable the transmit mailbox to get write access... */
  RXB0CONbits.TXREQ = 0;

  /* Clear interrupt pending flag... */
  RXB0CONbits.TXBIF = 0;
  
  /* Load the standard/extended message ID and remote request flags properly... */
  if ( pTxMsg->bExtended == EXT_FRM )
  {
    /* Load the ID registers properly for an extended identifier... */
    ucTemp = (((UBYTE)(pTxMsg->ulCanMsgId >> 16)) & EID_16_17_MASK);
    ucTemp |=(((UBYTE)(pTxMsg->ulCanMsgId >> 13)) & EID_18_20_MASK);
    RXB0SIDL = ucTemp;
    RXB0SIDLbits.EXIDE = 1;
    RXB0SIDH = ((UBYTE)(pTxMsg->ulCanMsgId >> 21));
    RXB0EIDL = ((UBYTE)pTxMsg->ulCanMsgId);
    RXB0EIDH = ((UBYTE)(pTxMsg->ulCanMsgId >> 8));
  }
  else /* Standard frame... */
  {
    /* Load the ID registers properly for a standard identifier... */
    RXB0SIDL = (((UBYTE)pTxMsg->ulCanMsgId) << 5);
    RXB0SIDH = ((UBYTE)(pTxMsg->ulCanMsgId >> 3));
    RXB0EIDL = 0x00;
    RXB0EIDH = 0x00;
  }
  
  /* Load the data length and set for remote request if necessary... */
  if ( pTxMsg->bRemote == REMOTE_FRM )
  {
    RXB0DLC = (_TXRTR + ((UBYTE)pTxMsg->ucDataLen)); /* Set for remote request; if it is... */
  }
  else
  {
    RXB0DLC = (UBYTE)(pTxMsg->ucDataLen); /* Set for remote request; if it is... */
  }
  
  /* Load the CAN message data registers... */
  RXB0D0 = pTxMsg->ucData[0];
  RXB0D1 = pTxMsg->ucData[1];
  RXB0D2 = pTxMsg->ucData[2];
  RXB0D3 = pTxMsg->ucData[3];
  RXB0D4 = pTxMsg->ucData[4];
  RXB0D5 = pTxMsg->ucData[5];
  RXB0D6 = pTxMsg->ucData[6];
  RXB0D7 = pTxMsg->ucData[7];
  
  /* Enable the transmit mailbox... */
  RXB0CONbits.TXREQ = 1;
}

/******************************************************************************
*                                                                             *
*  Function: void receive_can_msg( void )                                     *
*  Description: This function reads the ECAN hardware RX object mapped into   *
*               RXB0 in access RAM by the current EWIN bit settings in ECANCON*
*               and puts the message into the RX queue...                     *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void receive_can_msg( void )
{
  CAN_MSG*  pRxMsg = &(pRxQueue[ucRxPutPtr]);
  ULONG     ulTemp;

  /* See if this is an 11 or 29 bit identifier message... */
  if ( RXB0SIDLbits.EXIDE )
  {
    /* This is an extended frame... */
    pRxMsg->bExtended = EXT_FRM;

    /* Build properly formatted extended message ID... */
    ulTemp  = RXB0EIDL;
    ulTemp |= (((ULONG)RXB0EIDH) << 8);
    ulTemp |= (((ULONG)(RXB0SIDL & EID_16_17_MASK)) << 16);
    ulTemp |= (((ULONG)(RXB0SIDL & EID_18_20_MASK)) << 13);
    ulTemp |= (((ULONG)RXB0SIDH) << 21);
    pRxMsg->ulCanMsgId = ulTemp;
  }
  else
  {
    /* This is a standard frame... */
    pRxMsg->bExtended  = STD_FRM; /* Standard frame... */

    /* Build properly formatted standard message ID... */
    ulTemp  = (ULONG)(RXB0SIDL >> 5);
    ulTemp |= (((ULONG)RXB0SIDH) << 3);
    pRxMsg->ulCanMsgId = ulTemp;
  }

  /* Is this a remote request??? */
  if ( RXB0DLCbits.RXRTR )
  {
    pRxMsg->bRemote = REMOTE_FRM; /* Remote frame...   */
  }
  else
  {
    pRxMsg->bRemote = STD_FRM; /* Standard frame... */
  }

  /* Read and store the message data length... */
  pRxMsg->ucDataLen = (UBYTE)(RXB0DLC & 0x0F);

  /* Read and store the CAN message data bytes (read all bytes, all the time)... */
  pRxMsg->ucData[0] = RXB0D0;
  pRxMsg->ucData[1] = RXB0D1;
  pRxMsg->ucData[2] = RXB0D2;
  pRxMsg->ucData[3] = RXB0D3;
  pRxMsg->ucData[4] = RXB0D4;
  pRxMsg->ucData[5] = RXB0D5;
  pRxMsg->ucData[6] = RXB0D6;
  pRxMsg->ucData[7] = RXB0D7;

  /* Clear the receive message pending flag, freeing the hardware object for next RX... */
  RXB0CONbits.RXFUL = 0;

  /* Increment the CAN RX queue pointer after message is properly received... */
  ucRxPutPtr += 1;
  if ( (ucRxPutPtr) > (DWCAN_RX_QUEUE_LENGTH-1) ) ucRxPutPtr = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void flush_tx_queue( void )                                      *
*  Description: This function flushes the transmit queue and should be called *
*               from within a TX critical region...                           *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void flush_tx_queue( void )
{
  /* Flush the ports transmit queue... */
  ucTxGetPtr = 0;
  ucTxPutPtr = 0;
}

/******************************************************************************
*                                                                             *
*  Function: void flush_rx_queue( void )                                      *
*  Description: This function flushes the transmit queue and should be called *
*               from within an RX critical region...                          *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void flush_rx_queue( void )
{
  /* Flush the ports receive queue... */
  ucRxGetPtr = 0;
  ucRxPutPtr = 0;
}

/******************************************************************************
*                                                                             *
*  Function: CAN_BUF_STAT tx_queue_status( void )                             *
*  Description: This function returns the TX queue status for the requested   *
*               port. This function must be called from within a TX critical  *
*               region.                                                       *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
CAN_BUF_STAT tx_queue_status( void )
{
  CAN_BUF_STAT enCanStatus;

  /* Figure out the CAN transmit queue status... */
  if ( ucTxPutPtr == ucTxGetPtr )
  {
    /* The buffer is empty... */
    enCanStatus = CAN_BUF_EMPTY;
  }
  else if ( ucTxGetPtr < ucTxPutPtr )
  {
    if ( (ucTxGetPtr == 0) && (ucTxPutPtr == (DWCAN_TX_QUEUE_LENGTH-1)) )
    {
      /* The buffer is full... */
      enCanStatus = CAN_BUF_FULL;
    }
    else
    {
      /* The buffer is partially full... */
      enCanStatus = CAN_BUF_PARTIAL;
    }
  }
  else /* ( ucTxGetCan > ucTxPutCan ) */
  {
    if( (ucTxPutPtr+1) == ucTxGetPtr )
    {
      /* The buffer is full... */
      enCanStatus = CAN_BUF_FULL;
    }
    else
    {
      /* The buffer is partially full... */
      enCanStatus = CAN_BUF_PARTIAL;
    }
  }

  return enCanStatus;
}

/******************************************************************************
*                                                                             *
*  Function: CAN_BUF_STAT rx_queue_status( void )                             *
*  Description: This function returns the RX queue status for the requested   *
*               port. This function must be called from within a RX critical  *
*               region.                                                       *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
CAN_BUF_STAT rx_queue_status( void )
{
  CAN_BUF_STAT enCanStatus;

  /* Figure out the CAN transmit queue status... */
  if ( ucRxPutPtr == ucRxGetPtr )
  {
    /* The buffer is empty... */
    enCanStatus = CAN_BUF_EMPTY;
  }
  else if ( ucRxGetPtr < ucRxPutPtr )
  {
    if ( (ucRxGetPtr == 0) && (ucRxPutPtr == (DWCAN_RX_QUEUE_LENGTH-1)) )
    {
      /* The buffer is full... */
      enCanStatus = CAN_BUF_FULL;
    }
    else
    {
      /* The buffer is partially full... */
      enCanStatus = CAN_BUF_PARTIAL;
    }
  }
  else /* ( ucRxGetCan > ucRxPutCan ) */
  {
    if( (ucRxPutPtr+1) == ucRxGetPtr )
    {
      /* The buffer is full... */
      enCanStatus = CAN_BUF_FULL;
    }
    else
    {
      /* The buffer is partially full... */
      enCanStatus = CAN_BUF_PARTIAL;
    }
  }

  return enCanStatus;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR set_filter( CAN_FILTER eFilter, ULONG ulFilterVal )     *
*  Description: This function sets the requested filter value. The assumption *
*               here is that the ECAN module is already in configuration mode.*
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR set_filter( CAN_FILTER eFilter, ULONG ulFilterVal )
{
  STDERROR erReturnCode = ERR(DWCAN,SUCCESS);
  UBYTE ucEIDH = (UBYTE)(ulFilterVal >> 24);
  UBYTE ucEIDL = (UBYTE)(ulFilterVal >> 16);
  UBYTE ucSIDH = (UBYTE)(ulFilterVal >> 8) ;
  UBYTE ucSIDL = (UBYTE)ulFilterVal;

  /* See which filter should be set... */
  switch( eFilter )
  {
    case FILT_0:
      RXF0EIDH = ucEIDH;
      RXF0EIDL = ucEIDL;
      RXF0SIDH = ucSIDH;
      RXF0SIDL = ucSIDL;
      break;

    case FILT_1:
      RXF1EIDH = ucEIDH;
      RXF1EIDL = ucEIDL;
      RXF1SIDH = ucSIDH;
      RXF1SIDL = ucSIDL;
      break;

    case FILT_2:
      RXF2EIDH = ucEIDH;
      RXF2EIDL = ucEIDL;
      RXF2SIDH = ucSIDH;
      RXF2SIDL = ucSIDL;
      break;

    case FILT_3:
      RXF3EIDH = ucEIDH;
      RXF3EIDL = ucEIDL;
      RXF3SIDH = ucSIDH;
      RXF3SIDL = ucSIDL;
      break;

    case FILT_4:
      RXF4EIDH = ucEIDH;
      RXF4EIDL = ucEIDL;
      RXF4SIDH = ucSIDH;
      RXF4SIDL = ucSIDL;
      break;

    case FILT_5:
      RXF5EIDH = ucEIDH;
      RXF5EIDL = ucEIDL;
      RXF5SIDH = ucSIDH;
      RXF5SIDL = ucSIDL;
      break;

    case FILT_6:
      RXF6EIDH = ucEIDH;
      RXF6EIDL = ucEIDL;
      RXF6SIDH = ucSIDH;
      RXF6SIDL = ucSIDL;
      break;

    case FILT_7:
      RXF7EIDH = ucEIDH;
      RXF7EIDL = ucEIDL;
      RXF7SIDH = ucSIDH;
      RXF7SIDL = ucSIDL;
      break;

    case FILT_8:
      RXF8EIDH = ucEIDH;
      RXF8EIDL = ucEIDL;
      RXF8SIDH = ucSIDH;
      RXF8SIDL = ucSIDL;
      break;

    case FILT_9:
      RXF9EIDH = ucEIDH;
      RXF9EIDL = ucEIDL;
      RXF9SIDH = ucSIDH;
      RXF9SIDL = ucSIDL;
      break;

    case FILT_10:
      RXF10EIDH = ucEIDH;
      RXF10EIDL = ucEIDL;
      RXF10SIDH = ucSIDH;
      RXF10SIDL = ucSIDL;
      break;

    case FILT_11:
      RXF11EIDH  = ucEIDH;
      RXF11EIDL  = ucEIDL;
      RXF11SIDH  = ucSIDH;
      RXF11SIDL  = ucSIDL;
      break;

    case FILT_12:
      RXF12EIDH  = ucEIDH;
      RXF12EIDL  = ucEIDL;
      RXF12SIDH  = ucSIDH;
      RXF12SIDL  = ucSIDL;
      break;

    case FILT_13:
      RXF13EIDH  = ucEIDH;
      RXF13EIDL  = ucEIDL;
      RXF13SIDH  = ucSIDH;
      RXF13SIDL  = ucSIDL;
      break;

    case FILT_14:
      RXF14EIDH  = ucEIDH;
      RXF14EIDL  = ucEIDL;
      RXF14SIDH  = ucSIDH;
      RXF14SIDL  = ucSIDL;
      break;

    case FILT_15:
      RXF15EIDH  = ucEIDH;
      RXF15EIDL  = ucEIDL;
      RXF15SIDH  = ucSIDH;
      RXF15SIDL  = ucSIDL;
      break;

    default:
      erReturnCode = ERR(DWCAN,INVALID_PARAM);
      break;
  }

  return erReturnCode;
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR set_mask( CAN_MASK eMask, ULONG ulMaskVal )             *
*  Description: This function sets the requested mask value. The assumption   *
*               here is that the ECAN module is already in configuration mode.*
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR set_mask( CAN_MASK eMask, ULONG ulMaskVal )
{
  STDERROR erReturnCode = ERR(DWCAN,SUCCESS);
  UBYTE ucEIDH = (UBYTE)(ulMaskVal >> 24);
  UBYTE ucEIDL = (UBYTE)(ulMaskVal >> 16);
  UBYTE ucSIDH = (UBYTE)(ulMaskVal >> 8) ;
  UBYTE ucSIDL = (UBYTE)ulMaskVal;

  /* See which mask should be set... */
  switch ( eMask )
  {
    case MASK_0:
      RXM0EIDH = ucEIDH;
      RXM0EIDL = ucEIDL;
      RXM0SIDH = ucSIDH;
      RXM0SIDL = ucSIDL;
      break;

    case MASK_1:
      RXM1EIDH = ucEIDH;
      RXM1EIDL = ucEIDL;
      RXM1SIDH = ucSIDH;
      RXM1SIDL = ucSIDL;
      break;

    default:
      erReturnCode = ERR(DWCAN,INVALID_PARAM);
      break;
  }

  return erReturnCode;
}
