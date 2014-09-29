/***
 * 
 * CAN message processing
 *
 ***/
#include <inttypes.h>
#include <compiler.h>
#include <vscp_2.h>

#include "vscp_2.h" 
#include "can18f.h"
#include "can_msg.h" 

vscpMsg can_msg2send;
BOOL	bCanMsgValid = FALSE;
BOOL	bCanErrMsgValid = FALSE;
BYTE	g_can_lokal_error = 0;

void process_can_message()
{
	BYTE state_incoming;
	BYTE substate_incoming;
	
	// Incoming CAN data
	unsigned long ican_id;
	//BYTE ican_data[ 8 ]; 
	BYTE ican_dlc; 
	BYTE ican_flags;
	uint8_t msgflags;
	
	if ( bCanErrMsgValid )
	{
		send_can_error_message( g_can_lokal_error );
	}

	if ( bCanMsgValid )
	{
		if (  vscp_sendEvent( &can_msg2send ) )
			bCanMsgValid = FALSE;
	}
		
	// Incoming CAN message
	if ( vscp18f_readMsg( &ican_id, can_msg2send.data, &ican_dlc, &ican_flags ) ) 
	{	
		can_msg2send.sizeData = ican_dlc;	
		msgflags = ican_flags;		
		if ( ican_flags & CAN_RX_XTD_FRAME ) {
			msgflags |= 0x80;	// This is an extended frame 
		}
			
		if ( ican_flags & CAN_RX_RTR_FRAME ) {
			msgflags |= 0x40; // This is a RTR frame
		}
		can_msg2send.head = 0; //msgflags;

		can_msg2send.vscp_class = ican_id & 0xffff;
		can_msg2send.vscp_type = ( ican_id >> 16 ) & 0xffff;

		if (  vscp_sendEvent( &can_msg2send ) )
			bCanMsgValid = FALSE;
	}

/*
		switch ( vscp_node_state ) {

			case VSCP_STATE_STARTUP:			// Cold/warm reset

				// Get nickname from EEPROM
				if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
					// new on segment need a nickname
					vscp_node_state = VSCP_STATE_INIT; 	
				}
				else {
					// been here before - go on
					vscp_node_state = VSCP_STATE_ACTIVE;
					vscp_active();
				}
				break;

			case VSCP_STATE_INIT:			// Assigning nickname
				vscp_probe();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_rcv_preactive();					
				break;

			case VSCP_STATE_ACTIVE:			// The normal state
				
				if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?
					
					// Yes, incoming message
					if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) {
						
						switch( vscp_imsg.vscp_type ) {

							case VSCP_TYPE_SEGCTRL_HEARTBEAT:
								vscp_rcv_heartbeat();
								break;

							case VSCP_TYPE_NEW_NODE_ONLINE:
								vscp_rcv_new_node_online();
								break;
							
							case VSCP_TYPE_SET_NICKNAME:
								vscp_rcv_set_nickname();
								break;
							
							case VSCP_TYPE_DROP_NICKNAME:
								vscp_rcv_drop_nickname();
								break;

							case VSCP_TYPE_READ_REGISTER:
								break;

							case VSCP_TYPE_WRITE_REGISTER:
								break;
								
							case VSCP_TYPE_ENTER_BOOT_LOADER:
								if ( ( vscp_nickname == vscp_imsg.data[ 0 ] ) &&
										( 1 == vscp_imsg.data[ 1 ] ) &&
										( readEEPROM( EEPROM_REG_GUIID0 + 0 ) == vscp_imsg.data[ 2 ] ) &&
										( readEEPROM( EEPROM_REG_GUIID0 + 3 ) == vscp_imsg.data[ 3 ] ) &&
										( readEEPROM( EEPROM_REG_GUIID0 + 5 ) == vscp_imsg.data[ 4 ] ) &&
										( readEEPROM( EEPROM_REG_GUIID0 + 7 ) == vscp_imsg.data[ 5 ] ) &&
										( ( vscp_page_select >> 8 ) == vscp_imsg.data[ 6 ] ) &&
										( ( vscp_page_select & 0xff ) == vscp_imsg.data[ 7 ] ) ) {											
									// OK, We should enter boot loader mode
									// 	First, activate bootloader mode
									writeEEPROM( EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
									//_asm goto _startup reset _endasm
									_asm reset _endasm
								}
								break;
								
							case VSCP_TYPE_GET_MATRIX_INFO:
								//sendDMatrixInfo();
								break;	

							default:
								break;
						}							
					}
					
					// Decision Matrix
					//doDM();	
					
				}
				break;

			case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = STATE_STARTUP;
				break;

		} 
*/
}

void send_can_error_message( unsigned char i_can_error )
{
	vscpMsg can_err_msg;
	can_err_msg.sizeData = 1;
	can_err_msg.data[0] = i_can_error;	
	can_err_msg.head = 0;

	can_err_msg.vscp_class = 255;
	can_err_msg.vscp_type = 255;

	if (  vscp_sendEvent( &can_err_msg ) )
		bCanErrMsgValid = FALSE;
	else
	{
		bCanErrMsgValid = TRUE;
		g_can_lokal_error = i_can_error;
	}
}
