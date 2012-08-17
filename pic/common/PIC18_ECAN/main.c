/******************************************************************************
*                                                                             *
*  File:                         MAIN.C                                       *
*                                                                             *
*  Description: This file contains an example application that shows how to   *
*               use the dual wire ECAN driver developed by T. Shelburne...    *
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
*  Created By:  Thomas R. Shelburne 05/18/04                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*   1.0    - T.R.S. - Initial release of example embedded application...      *
*                                                                             *
******************************************************************************/

#include "define.h"
#include "dwcan.h"    /* Dual wire CAN driver include file... */
#include <p18cxxx.h>  /* Automatic target header include file... */
#include <stdlib.h>
#include "system.h"   /* System initialization support... */
#include "timer.h"    /* Interrupt driven 1msec timer support... */

/* Local Macro definitions...     */
#ifndef _NODE_1_
#define CAN_TX_MSG_ID_EXT     0x0A5FA5FA
#define CAN_TX_MSG_ID_STD     0x500
#define CAN_RX_MSG_ID_EXT     0x0A5FA5FB
#define CAN_RX_MSG_ID_STD     0x501
#define APP_TASK_MS           3
#else
#define CAN_TX_MSG_ID_EXT     0x0A5FA5FB
#define CAN_TX_MSG_ID_STD     0x501
#define CAN_RX_MSG_ID_EXT     0x0A5FA5FA
#define CAN_RX_MSG_ID_STD     0x500
#define APP_TASK_MS           3
#endif
#define BUS_OFF_RECOVER_MS    4

/* Local function declarations... */
far void ecan_rx_overflow_handler( void );
far void ecan_bus_off_handler( void );
far void ecan_wake_up_handler( void );

/* Local variable declarations... */
static UBYTE ucBusOffConditionActive;

/******************************************************************************
*                      GLOBAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: void main(void)                                                  *
*  Description: This function is shows how to use and call various API types. *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void main( void )
{
  static _TIMER_  sAppTask;
  static STDERROR erErrorStatus;
  static CAN_MSG  sTxMsg;
  static CAN_MSG  sRxMsg;
  static UBYTE    ucCounter = 0x00;

  /* Perform system initialization... */
  system_init();

  /* Perform system hardware tests... */
  erErrorStatus = system_test();
  if ( GET_ERROR(erErrorStatus) == SUCCESS )
  {
    /* Initialize the free running 1msec timer... */
    erErrorStatus = timer_init( GBL_INSTRUCT_CLOCK );
    if ( GET_ERROR(erErrorStatus) == SUCCESS )
    {
      /* Perform ECAN initialization... */
      ucBusOffConditionActive = FALSE;
      erErrorStatus = dwcan_init();
      if ( GET_ERROR(erErrorStatus) == SUCCESS )
      {
        /* Setup ECAN callback functions... */
        dwcan_set_ovrflw_callback( ecan_rx_overflow_handler );
        dwcan_set_bus_off_callback( ecan_bus_off_handler );
        dwcan_set_wake_up_callback( ecan_wake_up_handler );

        /* Start the application task timer initially... */
        timer_start( &sAppTask, 1 );

        /* Execute main application forever... */
        while( 1 )
        {
          /* Don't run ECAN tasks if bus off was encountered... */
          if ( ucBusOffConditionActive == FALSE )
          {
            /* See if it is time to transmit an ECAN message... */
            if ( timer_check( &sAppTask ) == TIMER_EXPIRED )
            {
              /* Restart the template task timer... */
              timer_start( &sAppTask, APP_TASK_MS );
            
              /* See if counter value should change... */
              if ( READ_INCR_SWITCH == 0x00 ) ucCounter++;
              if ( READ_DECR_SWITCH == 0x00 ) ucCounter--;
            
              /* Setup and transmit a CAN message... */
              sTxMsg.bExtended  = EXT_FRM;
              sTxMsg.bRemote    = STD_FRM;
              sTxMsg.ulCanMsgId = CAN_TX_MSG_ID_EXT;
              sTxMsg.ucDataLen  = 8;
              sTxMsg.ucData[0]  = 0xAA;
              sTxMsg.ucData[1]  = 0x55;
              sTxMsg.ucData[2]  = ucCounter;
              sTxMsg.ucData[3]  = ~ucCounter;
              sTxMsg.ucData[4]  = 0x00;
              sTxMsg.ucData[5]  = 0x01;
              sTxMsg.ucData[6]  = 0x55;
              sTxMsg.ucData[7]  = 0xAA;
              if ( ucBusOffConditionActive == FALSE )
              {
                erErrorStatus = dwcan_q_tx_message( &sTxMsg );
              }
              sTxMsg.bExtended  = STD_FRM;
              sTxMsg.ulCanMsgId = CAN_TX_MSG_ID_STD;
              sTxMsg.ucData[4]  = 0x02;
              sTxMsg.ucData[5]  = 0x03;
              if ( ucBusOffConditionActive == FALSE )
              {
                erErrorStatus = dwcan_q_tx_message( &sTxMsg );
              }
              sTxMsg.ulCanMsgId += 0x100;
              sTxMsg.ucData[4]  = 0x04;
              sTxMsg.ucData[5]  = 0x05;
              if ( ucBusOffConditionActive == FALSE )
              {
                erErrorStatus = dwcan_q_tx_message( &sTxMsg );
              }
            }
            
            /* Check for any ECAN RX messages of interest... */
            erErrorStatus = dwcan_get_rx_message( &sRxMsg );
            if ( GET_ERROR(erErrorStatus) == SUCCESS )
            {
              /* Process messages based on CAN ID... */
              switch ( sRxMsg.ulCanMsgId )
              {
                case CAN_RX_MSG_ID_STD:
                  if ( sRxMsg.ucDataLen == 8 )
                  {
                    /* Update value displayed on PORTD LED's... */
#ifndef _PROFILE_
                    PORTD = sRxMsg.ucData[2];
#endif      
                  }
                  break;
                  
                case CAN_RX_MSG_ID_EXT:
                  if ( sRxMsg.ucDataLen == 8 )
                  {
                    /* Update value displayed on PORTD LED's... */
#ifndef _PROFILE_
                    PORTD = sRxMsg.ucData[2];
#endif      
                  }
                  break;
            
                default:
                  /* Message was not of any interest... */
                  break;
              }
            }
          }
          else /* Bus-off condition was detected and CAN was halted... */
          {
            /* Clear the bus-off indication... */
            ucBusOffConditionActive = FALSE;
            
            /* Put a hard coded delay here of 128 * 11 recessive bits here (>2.8msec at 500kBaud)... */
            timer_start( &sAppTask, BUS_OFF_RECOVER_MS );
            while ( timer_check( &sAppTask ) != TIMER_EXPIRED );
          
            /* Re-initialize the ECAN interfaces and recover... */
            dwcan_init();
          
            /* Setup ECAN callback functions... */
            dwcan_set_ovrflw_callback( ecan_rx_overflow_handler );
            dwcan_set_bus_off_callback( ecan_bus_off_handler );
            dwcan_set_wake_up_callback( ecan_wake_up_handler );
          }
        }
      }
    }
  }
}

/******************************************************************************
*                       LOCAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: far void ecan_rx_overflow_handler( void )                        *
*  Description: This function is an example of how to declare the callback    *
*               function used to handle CAN RX overflow conditions in a users *
*               application. This call back function is called from the ECAN  *
*               error interrupt routine!!!                                    *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
far void ecan_rx_overflow_handler( void )
{
  /* Recover from CAN RX overflow condition if necessary... */
  /* KLU - Put code here... */
}

/******************************************************************************
*                                                                             *
*  Function: far void ecan_bus_off_handler( void )                            *
*  Description: This function is an example of how to declare the callback    *
*               function used to handle CAN bus off conditions in a users     *
*               application. This call back function is called from the ECAN  *
*               error interrupt routine!!!                                    *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
far void ecan_bus_off_handler( void )
{
  /* Indicate to main application that bus off occurred... */
  ucBusOffConditionActive = TRUE;

  /* Disable the ECAN module... */
  dwcan_halt();
}

/******************************************************************************
*                                                                             *
*  Function: far void ecan_wake_up_handler( void )                            *
*  Description: This function is an example of how to declare the callback    *
*               function used to handle CAN bus off conditions in a users     *
*               application. This call back function is called from the ECAN  *
*               wakeup interrupt routine!!!                                   *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
far void ecan_wake_up_handler( void )
{
  /* Recover from CAN wake up condition if necessary... */
  /* KLU - Put code here... */
}
