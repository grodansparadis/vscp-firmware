#include "Define_VSCP.h"
#include <Registers.h>
#include <eeprom.h>
#include <VSCP_drv.h>
#include <VSCP_Functions.h>
#include <VSCP_Class.h>
#include <VSCP_Type.h>
#include <Application.h>

extern VSCP_msg_t  RxMsg;


void doDecisionMatrix( void)
{
	uint08_t i;
	uint08_t dmflags;
	uint16_t class_filter;
	uint16_t class_mask;
	uint08_t type_filter;
	uint08_t type_mask;
	for ( i=0; i<DMATRIX_NBR; i++ ) 
	{

		// Get DM flags for this row
		dmflags = readEEPROM( REG_DMATRIX_START + DMATRIX_POS_FLAGS + ( DMATRIX_LEN * i));


		// Is the DM row enabled?
		if ( dmflags & DMATRIX_FLAG_ENABLED ) 
		{

			// Should the originating id be checked and if so is it the same?
			if ( dmflags & DMATRIX_FLAG_CHECK_OADDR){
				if ( RxMsg.nickname != readEEPROM( REG_DMATRIX_START + DMATRIX_POS_OADDR + ( DMATRIX_LEN * i))) {
				continue;					
				}
			}	
		
			// Check if zone should match and if so if it match
			if ( dmflags & DMATRIX_FLAG_CHECK_ZONE) {
				if ( 0xff != RxMsg.data[1] ) {
					if ( RxMsg.data[1] != readEEPROM( REG_APP_ZONE)) {
						continue;
					}
				}	
			}				


			class_filter = ( dmflags & DMATRIX_FLAG_CLASS_FILTER )*256 + 
									   readEEPROM( REG_DMATRIX_START + DMATRIX_POS_CLASSFILTER + ( DMATRIX_LEN * i));

			class_mask 	 = ( dmflags & DMATRIX_FLAG_CLASS_MASK )*256 + 
										 readEEPROM( REG_DMATRIX_START + DMATRIX_POS_CLASSMASK + ( DMATRIX_LEN * i));

			type_filter  = readEEPROM( REG_DMATRIX_START + DMATRIX_POS_TYPEFILTER + ( DMATRIX_LEN * i));
			type_mask 	 = readEEPROM( REG_DMATRIX_START + DMATRIX_POS_TYPEMASK 	+ ( DMATRIX_LEN * i));
				
			if ( !(( class_filter ^ RxMsg.vscp_class) & class_mask) && !(( type_filter ^ RxMsg.vscp_type) & type_mask)) 
			{
				switch( readEEPROM( REG_DMATRIX_START + DMATRIX_POS_ACTION + ( DMATRIX_LEN * i))) 
				{
					case ACTION_DOOR_OPEN: // Set event to Task Loop
						if ( dmflags & DMATRIX_FLAG_CHECK_SUBZONE) {
							if ( 0xff != RxMsg.data[2] ) {
								if ( RxMsg.data[2] != readEEPROM( REG_DOOR_SUBZONE)) {
									continue;
								}
							}	
						}				
								// All filters are pass, is time to send event to Loop_Task()
						SetEvent( VSCP_LOOP_ID, DOOR_EVENT); 
						break;
						
					case ACTION_ALERT_ON: // Set event to Task Loop
						if ( dmflags & DMATRIX_FLAG_CHECK_SUBZONE) {
							if ( 0xff != RxMsg.data[2] ) {
								if ( RxMsg.data[2] != readEEPROM( REG_ALERT_SUBZONE)) {
									continue;
								}
							}	
						}				
								// All filters are pass, is time to send event to Loop_Task()
						SetEvent( VSCP_LOOP_ID, ALERT_EVENT_ON); 
						break;

					case ACTION_ALERT_OFF: // Set event to Task Loop
						if ( dmflags & DMATRIX_FLAG_CHECK_SUBZONE) {
							if ( 0xff != RxMsg.data[2] ) {
								if ( RxMsg.data[2] != readEEPROM( REG_ALERT_SUBZONE)) {
									continue;
								}
							}	
						}				
								// All filters are pass, is time to send event to Loop_Task()
						SetEvent( VSCP_LOOP_ID, ALERT_EVENT_OFF); 
						break;
				} // case	
					// Check if Subzone should match and if so if it match


			} // Filter/mask
		} // Row enabled
	} // for each row	
}