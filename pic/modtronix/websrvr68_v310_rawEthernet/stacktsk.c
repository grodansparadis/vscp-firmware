/**
 * @brief           Stack Task Module for Modtronix TCP/IP Stack
 * @file            stacktsk.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 **********************************************************************
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
 **********************************************************************
 * File History
 *
 * 2004-10-02, David Hosken (DH):
 *    - Created from Microchip TCP/IP stack v2.20.04.01
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 *********************************************************************/

#define THIS_IS_STACK_TASK

#include "projdefs.h"
#include "checkcfg.h"
#include "stacktsk.h"
#include "debug.h"
#include "mac.h"


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE5, msgCode)


/////////////////////////////////////////////////
//Modtronix TCP/IP Stack Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
ROM char STACK_VER_STR[] = "V2.51";

typedef union _STACK_FLAGS
{
    struct
    {
        //Indicates if DHCP is bound yet. This flag is set on reset. It is cleared when DHCP is bound.
        //It is reset again if DHCP is enabled, and the link is lost
        unsigned char bInConfigMode : 1;
        
        //Indicate if DHCP has already been reset
        unsigned char bDHCPReset : 1;
    } bits;
    BYTE Val;
} STACK_FLAGS;

STACK_FLAGS stackFlags;

/*
 * Stack FSM states.
 */
typedef enum _SM_STACK
{
    SM_STACK_IDLE,
    SM_STACK_MAC,
    SM_STACK_VSCP
} SM_STACK;
static SM_STACK smStack;

MAC_ADDR	remoteNode;

/**
 * This function must be called before any of the
 * stack or its component routines be used.
 *
 * Side Affect: Stack and its componentns are initialized
 */
void StackInit(void)
{
    smStack = SM_STACK_IDLE;
    
    MACInit();

    //ARPInit();

}

/**
 * This FSM checks for new incoming packets, and routes it to appropriate
 * stack components. It also performs timed operations.
 *
 * This function must be called periodically called
 * to make sure that timely response.
 *
 * @preCondition    StackInit() is already called.
 *
 * side affect:     Stack FSM is executed.
 */
 
 
 
void StackTask(void)
{
    /*
    static WORD dataCount;
        
    union
    {
        BYTE MACFrameType;
        BYTE IPFrameType;
    } type;

    BOOL lbContinue;

    do
    {
        lbContinue = FALSE;

        switch ( smStack ) {
        
        	case SM_STACK_IDLE:
        	case SM_STACK_MAC:

            	// Check if the MAC RX Buffer has any data, and if it does, read the header.
            	// Get the next header from the NIC. The node who sent it's address will be copied to 
            	// 'remoteNode.MACAddr'.
            	// Header was NOT read if MACGetHeader returned FALSE
            	if ( !MACGetHeader( &remoteNode, &type.MACFrameType ) ) {
                	break;      //case SM_STACK_IDLE:  AND   case SM_STACK_MAC:
           	 	}
           	 	
           	 	if ( ( type.MACFrameType == MAC_VSCP ) ) {
            	lbContinue = TRUE;
            	if ( ( type.MACFrameType == MAC_VSCP ) && 
	            	!( ( MY_MAC_BYTE1 == remoteNode.v[ 0 ] ) &&
		             ( MY_MAC_BYTE1 == remoteNode.v[ 1 ] ) &&
			         ( MY_MAC_BYTE1 == remoteNode.v[ 2 ] ) &&
				     ( MY_MAC_BYTE1 == remoteNode.v[ 3 ] ) &&
					 ( MY_MAC_BYTE1 == remoteNode.v[ 4 ] ) &&
					 ( MY_MAC_BYTE1 == remoteNode.v[ 5 ] ) )	  ) {
	        		smStack = SM_STACK_VSCP;
	        	} 
            	else {         	
                	MACDiscardRx(); //Discard the contents of the current RX buffer              	
            	}
            	}
            	break;      //case SM_STACK_IDLE:  AND   case SM_STACK_MAC:
            
        	case SM_STACK_VSCP:
        	{
        		BYTE buf[ 21 ];
        		BYTE b;
        		//BYTE type;
        		//MAC_ADDR remote;
        		//MACGetHeader( &remote, &type );
        		//b = remote.v[ 0 ];
        		//b = remote.v[ 1 ];
        		//b = remote.v[ 2 ];
        		//b = remote.v[ 3 ];
        		//b = remote.v[ 4 ];
        		//b = remote.v[ 5 ];
        		
        		MACGetArray( buf, sizeof( buf ) );
        		b = buf[ 0 ];
        		b = buf[ 0 ];
        		b = buf[ 5 ];
        		b = buf[ 6 ];
        		b = buf[ 7 ];
        		b = buf[ 8 ];
        		smStack = SM_STACK_IDLE;
        		MACDiscardRx();
        	}	
        		break;     

        }       // switch(smStack)

        FAST_USER_PROCESS();
        
    } while( lbContinue );


    //debugPutGenRomStr(2, (ROM char*)"MACTask");     //@mxd:2:%s
*/
    //Perform routine MAC tasks
    MACTask();
}

