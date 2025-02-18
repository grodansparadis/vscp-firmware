//******************************************************************************
//                                                                    
// File name: Task2_VSCP.c                                             
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   v1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Main VSCP TASK            
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file. Ideas are taken from 
//																		PICos18 Tutorial 
//                                    VSCP Ake Hedman, eurosource             
//                                                                    
//            PICos18 is free software; you can redistribute it       
//            and/or modify it under the terms of the GNU General     
//            Public License as published by the Free Software        
//            Foundation; either version 2, or (at your option)       
//            any later version.                                      
//                                                                    
//            PICos18 is distributed in the hope that it will be      
//            useful, but WITHOUT ANY WARRANTY; without even the      
//            implied warranty of MERCHANTABILITY or FITNESS FOR A    
//            PARTICULAR PURPOSE.  See the GNU General Public         
//            License for more details.                               
//                                                                    
//            You should have received a copy of the GNU General      
//            Public License along with gpsim; see the file           
//            COPYING.txt. If not, write to the Free Software         
//            Foundation, 59 Temple Place - Suite 330,                
//            Boston, MA 02111-1307, USA.                             
//                                                                    
//          > A special exception to the GPL can be applied should    
//            you wish to distribute a combined work that includes    
//            PICos18, without being obliged to provide the source    
//            code for any proprietary components. 
//
//  	VSCP (Very Simple Control Protocol)                                            
//  	http://www.vscp.org                                                            
//                                                                                   
//  	akhe@eurosource.se                                                             
//                                                                                   
//   Copyright (C) 1995-2025 Ake Hedman, eurosource, <akhe@eurosource.se>            
//                                                                                   
//  This software is provided 'as-is', without any express or implied                
//  warranty.  In no event will the authors be held liable for any damages           
//  arising from the use of this software.                                           
//                                                                                   
//  Permission is granted to anyone to use this software for any purpose,            
//  including commercial applications, and to alter it and redistribute it           
//  freely, subject to the following restrictions:                                   
//                                                                                   
//  1. The origin of this software must not be misrepresented; you must not          
//     claim that you wrote the original software. If you use this software          
//     in a product, an acknowledgment in the product documentation would be         
//     appreciated but is not required.                                              
//  2. Altered source versions must be plainly marked as such, and must not be       
//     misrepresented as being the original software.                                
//  3. This notice may not be removed or altered from any source distribution.      
//                                                                                    
//   	This file is part of VSCP - Very Simple Control Protocol 	                      
//   	http://www.vscp.org                                                             
//                                                                                    
//******************************************************************************

#include "Define_VSCP.h"
#include <Registers.h>
#include <eeprom.h>
#include <VSCP_drv.h>
#include <VSCP_Functions.h>
#include <VSCP_Class.h>
#include <VSCP_Type.h>
#include <Application.h>

/**********************************************************************
 * Variables shared with the rest of application.
 **********************************************************************/
extern uint08_t RXQueueCount;
extern VSCP_msg_t  RxMsg;
extern VSCP_msg_t  SendMsg;
EventMaskType Prot_event;

// The GUID reset is used when the VSCP_TYPE_PROTOCOL_RESET_DEVICE
// is received. Bit 4,5,6,7 is set for each received frame with 
// GUID data. Bit 4 is for index = 0, bit 5 is for index = 1 etc.
// This means that a bit is set if a frame with correct GUID is
// received.  
uint08_t vscp_guid_reset;

/**********************************************************************
 *
 * Protocol implementation 
 *
 **********************************************************************/
TASK(VSCP_ProtEngine)
{
	ActivateTask( VSCP_LOOP_ID);			// Go to Active State
	VSCP_RCV_Register();							// Driver send an event when CAN Msg arrives     

	while ( 1)												// Loop Forever	
	{ 
																								//  	if ( !VSCP_CountMsgRx()) 
																								//  	{
	 	WaitEvent(VSCP_QUEUE_RX | WD_EVENT);			// No more MSG arrived. Wait for event
  	GetEvent( VSCP_PROT_ID, &Prot_event); 
																								//		}
		
		if( Prot_event & WD_EVENT)
    {
      ClearEvent(WD_EVENT);
      SetEvent( VSCP_LOOP_ID, WD_EVENT);
		}

		if( Prot_event & VSCP_QUEUE_RX)
    {
      ClearEvent( VSCP_QUEUE_RX);		
		
			VSCP_deqMsgRx(&RxMsg);					// Yes, incoming message
					
			if ( VSCP_CLASS1_PROTOCOL == RxMsg.vscp_class ) {
				switch( RxMsg.vscp_type ) {
	
					case VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT:
						vscp_handleHeartbeat();
						break;
	
					case VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE:
						vscp_newNodeOnline();
						break;
					
					case VSCP_TYPE_PROTOCOL_SET_NICKNAME:
						vscp_handleSetNickname();
						break;
					
					case VSCP_TYPE_PROTOCOL_DROP_NICKNAME:
						vscp_handleDropNickname();
						break;
	
					case VSCP_TYPE_PROTOCOL_READ_REGISTER:
						if (( 2 == RxMsg.length) && 
								( vscp_nickname == RxMsg.data[ 0 ] ) ) {
	
							if ( RxMsg.data[ 1 ] < 0x80 )  {
								read_app_register( RxMsg.data[ 1 ] );		// Read application specific register
							}
							else {
								vscp_readStdReg();					// Read VSCP register
							}
						}
						break;
	
					case VSCP_TYPE_PROTOCOL_WRITE_REGISTER:
						if (( 3 == RxMsg.length) && 
								( vscp_nickname == RxMsg.data[ 0 ] ) ) {
							if ( RxMsg.data[ 1 ] < 0x80 ) {
														// Write application specific register
								write_app_register( RxMsg.data[ 1 ], RxMsg.data[ 2 ] );
							}
							else {
								vscp_writeStdReg();					// Write VSCP register
							}
						}
						break;
						
					case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
						if (( vscp_nickname == RxMsg.data[ 0 ] ) &&
								( 1 == RxMsg.data[ 1 ] ) &&
								( vscp_getGUID( 0) == RxMsg.data[ 2 ] ) &&
								( vscp_getGUID( 3) == RxMsg.data[ 3 ] ) &&
								( vscp_getGUID( 5) == RxMsg.data[ 4 ] ) &&
								( vscp_getGUID( 7) == RxMsg.data[ 5 ] ) &&
								( ( vscp_page_select >> 8 ) == RxMsg.data[ 6 ] ) &&
								( ( vscp_page_select & 0xff ) == RxMsg.data[ 7 ] ) ) {											
														// OK, We should enter boot loader mode
														// 	First, activate bootloader mode
									vscp_goBootloaderMode();	 											
						}
						break;
						
			case VSCP_TYPE_PROTOCOL_RESET_DEVICE:
				switch ( RxMsg.data[ 0 ] >> 4 ) {
									
					case 0:
						if (( vscp_getGUID( 0) == RxMsg.data[ 1 ] ) &&
							  ( vscp_getGUID( 1) == RxMsg.data[ 2 ] ) &&
							  ( vscp_getGUID( 2) == RxMsg.data[ 3 ] ) &&
							  ( vscp_getGUID( 3) == RxMsg.data[ 4 ] ) ) {
							   	vscp_guid_reset |= 0x10;
						}	
						break;
										
					case 1:
						if (( vscp_getGUID( 4) == RxMsg.data[ 1 ] ) &&
							  ( vscp_getGUID( 5) == RxMsg.data[ 2 ] ) &&
							  ( vscp_getGUID( 6) == RxMsg.data[ 3 ] ) &&
							  ( vscp_getGUID( 7) == RxMsg.data[ 4 ] ) ) {
							     vscp_guid_reset |= 0x20;
						}
						break;
										
					case 2:
						if (( vscp_getGUID( 8 ) == RxMsg.data[ 1 ] ) &&
							  ( vscp_getGUID( 9 ) == RxMsg.data[ 2 ] ) &&
						    ( vscp_getGUID( 10) == RxMsg.data[ 3 ] ) &&
							  ( vscp_getGUID( 11) == RxMsg.data[ 4 ] ) ) {
									vscp_guid_reset |= 0x40;
						}
						break;
										
					case 3:
						if (( vscp_getGUID( 12) == RxMsg.data[ 1 ] ) &&
							  ( vscp_getGUID( 13) == RxMsg.data[ 2 ] ) &&
							  ( vscp_getGUID( 14) == RxMsg.data[ 3 ] ) &&
							  ( vscp_getGUID( 15) == RxMsg.data[ 4 ] ) ) {
									vscp_guid_reset |= 0x80;
						}
						break;
										
					default:
						vscp_guid_reset = 0;	
						break;				
				}
									
				if ( 0xf0 == (vscp_guid_reset & 0xf0) ) {
					// Do a reset
					vscp_init();
				}	
				break;
								
//			case VSCP_TYPE_PROTOCOL_PAGE_READ:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//									
//					uint8_t i;
//					uint8_t pos = 0;
//					uint8_t bSent = TRUE;
//										 
//					for ( i = RxMsg.data[ 1 ]; 
//							i < ( RxMsg.data[ 1 ] + RxMsg.data[ 2 ] + 1 ); 
//							i++ ) {
//											   	
//						SendMsg.length = ( pos % 7 ) + 1;
//						SendMsg.data[ ( pos % 7 ) + 1 ] = 
//													vscp_readAppReg( i );
//						bSent = FALSE;	   	
//											  
//						if ( pos && ( 0 == ( pos % 7 ) ) ) {	   
//							
//							SendMsg.priority = VSCP_PRIORITY_NORMAL;	
//							SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//							SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
//							SendMsg.data[ 0 ] = pos/7;	// index
//											
//							SendMsg.length = 7; // Count = 7			
//						
//							// send the event
//							vscp_sendEvent();
//											
//							bSent = TRUE;
//						}
//										
//						pos++;
//															
//					}
//									
//					// Send any pending event
//					if ( bSent ) {
//						SendMsg.priority = VSCP_PRIORITY_NORMAL;	
//						SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//						SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
//						SendMsg.data[ 0 ] = pos/7;	// index
//																	
//						// send the event
//						vscp_sendEvent();	
//					}	
//										
//				}	
//				break;	
//								
//			case VSCP_TYPE_PROTOCOL_PAGE_WRITE:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//					uint8_t i;
//					uint8_t pos = RxMsg.data[ 1 ];
//									
//					for ( i = 0; (i < ( RxMsg.flags & 0x0f ) - 2); i++ ) {
//						// Write VSCP register
//						SendMsg.data[ i + 1 ] =
//										vscp_writeStdReg( RxMsg.data[ 1 ] + i,
//										RxMsg.data[ 2 + i ] );			
//					}
//									
//					SendMsg.priority = VSCP_PRIORITY_NORMAL;	
//					SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//					SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_PAGE_RESPONSE;
//					SendMsg.data[ 0 ] = 0;	// index
//					SendMsg.length = 
//												( RxMsg.flags & 0x0f ) - 2;
//																	
//					// send the event
//					vscp_sendEvent();
//																				
//				}
//				break;	
//								
//			case VSCP_TYPE_PROTOCOL_INCREMENT_REGISTER:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//									
//					SendMsg.priority = VSCP_PRIORITY_NORMAL;
//					SendMsg.length = 2;
//					SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//					SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
//										
//					SendMsg.data[ 0 ] = RxMsg.data[ 1 ];
//					SendMsg.data[ 1 ] = vscp_writeAppReg( 
//													RxMsg.data[ 1 ],
//													vscp_readAppReg( RxMsg.data[ 1 ] ) + 1 );
//									
//					// send the event
//					vscp_sendEvent();										
//				}
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_DECREMENT_REGISTER:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//									
//					SendMsg.priority = VSCP_PRIORITY_NORMAL;
//					SendMsg.length = 2;
//					SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//					SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
//										
//					SendMsg.data[ 0 ] = RxMsg.data[ 1 ];
//					SendMsg.data[ 1 ] = vscp_writeAppReg( 
//														RxMsg.data[ 1 ],
//														vscp_readAppReg( RxMsg.data[ 1 ] ) - 1 );
//									
//					// send the message
//					vscp_sendEvent();	
//				}
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_WHO_IS_THERE:
//				if ( ( vscp_nickname == RxMsg.data[ 0 ] ) ||
//					 ( 0xff == RxMsg.data[ 0 ] ) ) {
//											
//					uint8_t i,j;
//														
//					// Send data
//					for ( j = 0; j < 7; j++ ) {
//									
//						SendMsg.priority = VSCP_PRIORITY_NORMAL;
//						SendMsg.length = 8;
//						SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//						SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE;
//						SendMsg.data[ 0 ] = j;	
//										
//						for ( i = 1; i < 8; i++ ) {
//							SendMsg.data[ i ] = vscp_readStdReg( 0xd0 + 7*j + i );
//						}
//										
//						// Correct read error for last run
//						if ( 6 == j ) {
//							SendMsg.data[ 7 ] = 0;
//						}	
//										
//						// send the event
//						vscp_sendEvent();
//										
//					}		
//										
//				}
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO:
//								
//				SendMsg.priority = VSCP_PRIORITY_NORMAL;
//				SendMsg.length = 7;
//				SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//				SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;
//										
//				vscp_getMatrixInfo();
//								
//				// send the event
//				vscp_sendEvent();
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF:
//				vscp_getEmbeddedMdfInfo();
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_READ:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//					uint8_t i;
//					uint16_t page_save;
//					uint8_t page_msb = RxMsg.data[ 1 ];
//					uint8_t page_lsb = RxMsg.data[ 2 ];
//									
//					// Save the current page
//					page_save = vscp_page_select;
//									
//					// Check for valid count
//					if ( RxMsg.data[ 3 ] > 6 ) RxMsg.data[ 3 ] = 6;
//									
//					SendMsg.priority = VSCP_PRIORITY_NORMAL;
//					SendMsg.length = 3 + RxMsg.data[ 3 ];
//					SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//					SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
//									
//					for ( i=RxMsg.data[ 2 ];
//							i < ( RxMsg.data[ 2 ] + RxMsg.data[ 3 ] );
//							i++ ) {
//						SendMsg.data[ 3 + ( i - RxMsg.data[ 2 ] ) ] = 
//											vscp_readRegister( i ); 						
//					}
//									
//					// Restore the saved page
//					vscp_page_select = page_save;
//									
//					// send the event
//					vscp_sendEvent();
//										
//				}	
//				break;
//								
//			case VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE:
//				if ( vscp_nickname == RxMsg.data[ 0 ] ) {
//					uint8_t i;
//					uint16_t page_save;
//					uint8_t page_msb = RxMsg.data[ 1 ];
//					uint8_t page_lsb = RxMsg.data[ 2 ];
//					
//					// Save the current page
//					page_save = vscp_page_select;
//					
//					SendMsg.priority = VSCP_PRIORITY_NORMAL;
//					SendMsg.length = 3 + RxMsg.data[ 3 ];
//					SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
//					SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE;
//					
//					for ( i=RxMsg.data[ 2 ];
//							i < ( RxMsg.data[ 2 ] + RxMsg.data[ 3 ] );
//							i++ ) {
//						SendMsg.data[ 3 + 
//									( i - RxMsg.data[ 2 ] ) ] = 
//										vscp_writeRegister( i, 
//															RxMsg.data[ 4 + 
//															( i - RxMsg.data[ 2 ] ) ] ); 						
//					}
//
//					// Restore the saved page
//					vscp_page_select = page_save;
//					
//					// send the event
//					vscp_sendEvent();
//										
//				}
//				break;
	
					default:
						break;					// Do work load
				}							
			} 										// endif VSCP_CLASS1_PROTOCOL
			else 
			{
			/////NO Protocol MESSAGE
						// Decision Matrix
				doDecisionMatrix();	
			}
		}	//endif event
	} 											//	Endwhile ( 1) 
}


