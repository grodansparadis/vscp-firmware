/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	All mandatory functions for the vscp protocol class
*******************************************************************************/

#include "digiprobe.inc"

void vscp_protocol_class(void)
{
	switch ( vscp_node_state ) {
		case VSCP_STATE_STARTUP:			// Cold/warm reset
			// Get nickname from EEPROM
			
			if ( VSCP_ADDRESS_FREE == vscp_nickname )
			{
				// new on segment need a nickname
				vscp_node_state = VSCP_STATE_INIT; 	
			}
			else
			{
				// been here before - go on
				vscp_node_state = VSCP_STATE_ACTIVE;
				vscp_goActiveState();
			}
			break;

		case VSCP_STATE_INIT:			// Assigning nickname
			vscp_handleProbeState();
			break;

		case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
			vscp_goActiveState();					
			break;

		case VSCP_STATE_ACTIVE:			// The normal state
				
			if ( vscp_imsg.flags & VSCP_VALID_MSG ) {	// incoming message?
					
			vscp_handleProtocolEvent();
			}
			break;

		case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
			vscp_error();
			break;

		default:					// Should not be here...
			vscp_node_state = VSCP_STATE_STARTUP;
			break;

		} // switch 
}


///////////////////////////////////////////////////////////////////////////////
//   						VSCP Functions
//							--------------
//
// All methods below must be implemented to handle VSCP functionality
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//
//

int8_t sendVSCPFrame( uint16_t vscpclass, 
                        uint8_t vscptype, 
                        uint8_t nodeid,
                        uint8_t priority,
                        uint8_t size, 
                        uint8_t *pData )
{
	CANMsg msg;
    
    msg.id = ( (uint32_t)priority << 26 ) |
						( (uint32_t)vscpclass << 16 ) |
						( (uint32_t)vscptype << 8) |
						nodeid;		// nodeaddress (our address)
	
	
	msg.flags = CAN_IDFLAG_EXTENDED;
	msg.len = size;
    if ( size ) {
        memcpy( msg.byte, pData, size );
    }
	       
	if ( ERROR_OK != can_SendFrame( &msg ) ) {
		return FALSE;
	}
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//
//

int8_t getVSCPFrame( uint16_t *pvscpclass, 
                        uint8_t *pvscptype, 
                        uint8_t *pNodeId,
                        uint8_t *pPriority, 
                        uint8_t *pSize, 
                        uint8_t *pData )
{
	CANMsg msg;
	
	// Check for received frame
    if ( ERROR_OK != can_readFrame( &msg ) ) {
		return FALSE;
	}
	
	*pNodeId = msg.id & 0x0ff;
	*pvscptype = ( msg.id >> 8 ) & 0xff;
	*pvscpclass = ( msg.id >> 16 ) & 0x1ff;
    *pPriority = (uint16_t)( 0x07 & ( msg.id >> 26 ) );
    *pSize = msg.len;
    if ( msg.len ) {
        memcpy( pData, msg.byte, msg.len );
    }
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// readEEPROM
int readEEPROM( uint8_t addr )
{
	// want to use the avrlib-eeprom functions!
	uint8_t test = eeprom_read_byte( &addr );
	return test;
}

//*/
///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
int writeEEPROM( uint8_t addr, uint8_t data )
{
	eeprom_update_byte(&addr, data);
	return TRUE;
}
//*/

///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//
void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = eventClass;
	vscp_omsg.type = eventTypeId;

	vscp_omsg.data[ 0 ] = idx;	// Register
	vscp_omsg.data[ 1 ] = eeprom_read_byte(&STD_REG_ZONE);
	vscp_omsg.data[ 2 ] = eeprom_read_byte(&STD_REG_SUBZONE);

	vscp_sendEvent();	// Send data
}

///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// vscp_getRegisterPagesUsed
//
// Return number of register pages this application uses.
//

uint8_t vscp_getRegisterPagesUsed( void )
{
    return 1;   // Application uses one register page
}


///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint8_t reg )
{
	uint8_t rv = 0;
	if (reg < PAGE0_USER_REGISTERS)
		rv = eeprom_read_byte(&P0_USER_REGS[reg]);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
// writes data to application register (0x00-0x7f) on register page 0

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
	uint8_t rv = 0xff;
	// write the byte
	if (reg < PAGE0_USER_REGISTERS)
		{
		eeprom_update_byte(&P0_USER_REGS[reg],val);
		// read back and compare, return bitwise NOT if correctly written
		rv = eeprom_read_byte(&P0_USER_REGS[reg]);
		if ( rv != val) rv = ~val;
		}
	return rv;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_readNicknamePermanent
//
// Fetch nickname from permanent storage
// return read nickname.
//

uint8_t vscp_readNicknamePermanent( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_NICKNAME);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// 	vscp_writeNicknamePermanent
// 
//	Write nickname to permanent storage
//

void vscp_writeNicknamePermanent( uint8_t nickname )
{
	eeprom_update_byte(&STD_REG_NICKNAME,nickname);
}


///////////////////////////////////////////////////////////////////////////////
//	vscp_getZone
//
//	Get Zone for device
//	Just return zero if not used. 
//

uint8_t vscp_getZone( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_ZONE);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
//	vscp_getSubzone
//
//	Get Subzone for device
// 	Just return zero if not used. 
//

uint8_t vscp_getSubzone( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_SUBZONE);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
//	vscp_goBootloaderMode
//
//	Go bootloader mode
//	This routine force the system into bootloader mode according
//	to the selected protocol. 
//

void vscp_goBootloaderMode( void )
{
	// TODO
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getMajorVersion
//
// Get Major version number for this hardware module
//

uint8_t vscp_getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMinorVersion
//
// Get Minor version number for this hardware module
//

uint8_t vscp_getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubMinorVersion
//
// Get Subminor version number for this hardware module
//

uint8_t vscp_getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getGUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
	#ifdef GUID_IN_FLASH
	uint8_t rv = pgm_read_byte(&STD_REG_GUID[idx]);
	#else
	uint8_t rv = eeprom_read_byte(&STD_REG_GUID[idx]);
	#endif
	return rv;
}


///////////////////////////////////////////////////////////////////////////////
// vscp_setGUID
//
// Write GUID if it is in EEPROM
//

void vscp_setGUID( uint8_t idx, uint8_t data )
{
#ifdef GUID_IN_EEPROM
	eeprom_update_byte(&STD_REG_GUID[idx], data);	
#endif	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMDF_URL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
	#ifdef MDF_URL_IN_FLASH
	uint8_t rv = pgm_read_byte(&STD_REG_MDF_URL[idx]);
	#else
	uint8_t rv = eeprom_read_byte(&STD_REG_MDF_URL[idx]);
	#endif
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getUserID
// Get UserID id and subid from EEPROM

uint8_t vscp_getUserID( uint8_t idx )
{
	uint8_t rv = eeprom_read_byte(&STD_REG_USERID[idx]);
	return rv;	
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
	eeprom_update_byte(&STD_REG_USERID[idx],data);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
	#ifdef MANUFACTID_IN_FLASH
	uint8_t rv = pgm_read_byte(&STD_REG_MANUFACTID[idx]);
	#else
	uint8_t rv = eeprom_read_byte(&STD_REG_MANUFACTID[idx]);
	#endif
	return rv;	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_setManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
	#ifndef MANUFACTID_IN_FLASH
	eeprom_update_byte(&STD_REG_MANUFACTID[idx],data);	
	#endif
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getBootLoaderAlgorithm
//
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_VSCP;	
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getBufferSize
//
// Get the buffer size
//

uint8_t vscp_getBufferSize( void ) 
{
  return 8;	// Standard CAN frame
}

	
///////////////////////////////////////////////////////////////////////////////
//  vscp_getNickname
//

uint8_t vscp_getNickname( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_NICKNAME);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setNickname
//

void vscp_setNickname( uint8_t nickname )
{
	eeprom_update_byte(&STD_REG_NICKNAME,nickname);
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_SEGMENT_CRC);
	return rv;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
	eeprom_update_byte(&STD_REG_SEGMENT_CRC, crc);
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
	eeprom_update_byte(&STD_REG_CONTROL, ctrl);
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_getControlByte
//

uint8_t vscp_getControlByte( void )
{
	int8_t rv = eeprom_read_byte(&STD_REG_CONTROL);
	return rv;
}



///////////////////////////////////////////////////////////////////////////////
//                       Implemention of Decision Matrix
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void vscp_getMatrixInfo( char *pData )
{
    pData[ 0 ] = DECISION_MATRIX_ELEMENTS;
    pData[ 1 ] = STD_REG_DECISION_MATRIX;
    pData[ 2 ] = 0;
    pData[ 3 ] = 0;
    pData[ 4 ] = 0;
    pData[ 5 ] = 0;
    pData[ 6 ] = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
// The routine expects vscp_imsg to contain a vaild incoming event
#if DECISION_MATRIX_ELEMENTS > 0
static void doDM( void )
{
}
#endif


///////////////////////////////////////////////////////////////////////////////
// vscp_getEmbeddedMdfInfo
//
//	Get embedded MDF info
//	If available this routine sends an embedded MDF file
//	in several events. See specification CLASS1.PROTOCOL
//	Type=35/36
//
#ifdef EMBEDDED_MDF
void vscp_getEmbeddedMdfInfo( void )
{
	// TODO
}
#endif

/*	-------------------------------------------------------------------
	Initialize complete EEPROM, has no parameters
	-----------------------------------------------------------------*/
void init_app_eeprom( void )
{
	// rewrite standard vscp registers in eeprom
	eeprom_update_byte(&STD_REG_NICKNAME	, VSCP_ADDRESS_FREE );
	eeprom_update_byte(&STD_REG_SEGMENT_CRC	, 0 				);
	eeprom_update_byte(&STD_REG_CONTROL		, 0b10000000 		);
	eeprom_update_byte(&STD_REG_ZONE		, 0 				);
	eeprom_update_byte(&STD_REG_SUBZONE		, 0 				);

	//write the default user id (5 bytes)
	eeprom_update_dword( (uint32_t*)&STD_REG_USERID[0], 0x01121973	);
	eeprom_update_byte(				&STD_REG_USERID[4], 0xdd  		);

	// rewrite user registers in eeprom, this time we have only
	// one page of registers
	//write the default current offset to eeprom = 0.0
	eeprom_update_float((float*)&P0_USER_REGS[P0_REG_IP_OFF], 0.0);
	//write the default current scaler to eeprom = 1.0
	eeprom_update_float((float*)&P0_USER_REGS[P0_REG_IP_CAL], 1.0);
}
