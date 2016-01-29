//********************************************************************
//                                                                    
// File name: VSCP_app.c                                               
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   General VSCP CAN routines             
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file from             
//                                                                   
//  	VSCP (Very Simple Control Protocol)                                            
//  	http://www.vscp.org                                                            
//                                                                                   
//  	akhe@eurosource.se                                                             
//                                                                                   
//   Copyright (C) 1995-2006 Ake Hedman, eurosource, <akhe@eurosource.se>            
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
//
//
//           code for any proprietary components.                    
//******************************************************************************
#include <Registers.h>
#include "Application.h"
#include <VSCP_drv.h>
#include <VSCP_Functions.h>
#include <VSCP_Class.h>
#include <VSCP_Type.h>
#include "VSCP_drv.h"
#include "TypeDefs.h"
#include "EEprom.h"
#include <define_vscp.h>

extern VSCP_msg_t SendMsg;

////////////////////////////////////////////////////////////////////////////////
// read_app_register
//
void read_app_register( uint08_t reg )
{
	SendMsg.priority = VSCP_PRIORITY_MEDIUM;
	SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	SendMsg.length =  2;
	SendMsg.data[0] = reg;
	SendMsg.data[1] = 0x00; // default read

	switch ( reg ) {
		// Zone
		case 0x21:
			SendMsg.data[1] = readEEPROM( REG_APP_ZONE );
			break;
		// SubZone
		case 0x22:
			SendMsg.data[1] = readEEPROM( REG_DOOR_SUBZONE );
			break;		

		default:
			SendMsg.data[1] = 0;	
			break;	
	}
	// Send data
	VSCP_enqMsgTx( &SendMsg, FALSE);	
}
 
///////////////////////////////////////////////////////////////////////////////
// write_app_register
//
void write_app_register( uint08_t reg, uint08_t val )
{
	SendMsg.priority = VSCP_PRIORITY_MEDIUM;
	SendMsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	SendMsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;
	SendMsg.length = 2;
	SendMsg.data[0] = reg;
	SendMsg.data[1] = ~val; // error return

	switch ( reg ) {
		// Zone
		case 0x23:
			writeEEPROM( REG_APP_ZONE, val);
			SendMsg.data[1] = readEEPROM( REG_APP_ZONE);
			break;
		// SubZone
		case 0x24:
			writeEEPROM( REG_DOOR_SUBZONE, val);
			SendMsg.data[1] = readEEPROM( REG_DOOR_SUBZONE);
			break;	

		default:
			SendMsg.data[1] = ~val; // error return	
			break;
	}
	// Send data
	VSCP_enqMsgTx( &SendMsg, FALSE);	
} 



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//
uint08_t vscp_getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//
uint08_t vscp_getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//
uint08_t vscp_getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
// Get GUID from EEPROM
//
uint08_t vscp_getGUID( uint08_t idx )
{
	uint08_t aux;
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_REG_GUID + idx );		
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
// Get device URL from EEPROM
//
uint08_t vscp_getMDF_URL( uint08_t idx )
{
////	return vscp_deviceURL[idx];   ///////////////ver tema EEPROM, que esta el lugar reservado
}

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//
uint08_t vscp_getUserID( uint08_t idx )
{
	uint08_t aux;
 	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//
void vscp_setUserID( uint08_t idx, uint08_t data )
{
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		writeEEPROM( idx + VSCP_EEPROM_REG_USERID, data );
		ReleaseResource(RESOURCE_EE_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//
uint08_t vscp_getManufacturerId( uint08_t idx )
{
	uint08_t aux;
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
// setVSCPManufacturerId
// 
// Set Manufacturer id and subid to EEPROM
//
void vscp_setManufacturerId( uint08_t idx, uint08_t data )
{
	if (GetResource(RESOURCE_EE_ID) == E_OK)
		{
		writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );	
		ReleaseResource(RESOURCE_EE_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//
uint08_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_PIC1;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//
uint08_t vscp_getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
}
	
///////////////////////////////////////////////////////////////////////////////
//  getNickname
//
uint08_t vscp_getNickname( void )
{
	uint08_t aux;
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_NICKNAME );
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
//
void vscp_setNickname( uint08_t nickname )
{
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
		ReleaseResource(RESOURCE_EE_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  getSegmentCRC
//
uint08_t vscp_getSegmentCRC( void )
{
	uint08_t aux;
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_SEGMENT_CRC );
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  setSegmentCRC
//
void vscp_setSegmentCRC( uint08_t crc )
{
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, crc );
		ReleaseResource(RESOURCE_EE_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//
void vscp_setControlByte( uint08_t ctrl )
{
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		writeEEPROM( VSCP_EEPROM_CONTROL, ctrl );
		ReleaseResource(RESOURCE_EE_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//
uint08_t vscp_getControlByte( void )
{
	uint08_t aux;
	if (GetResource(RESOURCE_EE_ID) == E_OK)
	{
		aux = readEEPROM( VSCP_EEPROM_CONTROL );
		ReleaseResource(RESOURCE_EE_ID);
		return aux;
	}
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_init_app
//	Extern Function for Init application

void vscp_init_app(void)
{	
	DoorOpen_init_app();
} 

//*******************************************************************************                                                      
// vscp_setDefaultEeprom( void)
//
//*******************************************************************************                                                      
void vscp_setDefaultEeprom( void)
{

}

