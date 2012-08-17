///////////////////////////////////////////////////////////////////////////////
//
// File: hulkevents.c
//
// HULK event handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: hulkevents.c,v $
// $Revision: 1.1 $
//



#include "types.h"
#include "lpc213x.h"
//#include <stdint.h> C99 types int32_t, uint32_t, etc.
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "lawicel232can.h"
#include "eeprom.h"
#include <vscp_class.h>
#include <vscp_type.h>
#include "vscp.h"
#include "hulk.h"
#include "hulkevents.h"


///////////////////////////////////////////////////////////////////////////////
// sendAlarmEvent
//

int sendAlarmEvent( unsigned char nAlarm )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_ALARM;
	vscp_omsg.type = VSCP_TYPE_ALARM_ALARM;
	vscp_omsg.data[ 0 ] = nAlarm;
	vscp_omsg.data[ 1 ] = vscp_alarmstatus;

	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sendErrorEvent
//

int sendErrorEvent( unsigned char nError )
{
	vscp_omsg.priority = VSCP_PRIORITY_HIGH;
	vscp_omsg.flag = VSCP_VALID_MSG + 1;
	vscp_omsg.class = VSCP_CLASS2_LEVEL1_INFORMATION;
	vscp_omsg.type = VSCP_TYPE_INFORMATION_ERROR;
	vscp_omsg.data[ 0 ] = nError;

	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendTempEvent
//

int sendTempEvent( uint8_t idx, uint8_t temp )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_MEASUREMENT;
	vscp_omsg.type = VSCP_TYPE_MEASUREMENT_TEMPERATURE;
	
	vscp_omsg.data[ 0 ] = 0x60 + idx;
	vscp_omsg.data[ 1 ] = temp;
	
	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendStateEvent
//

int sendStateEvent( uint8_t subzone, uint8_t from_state, uint8_t to_state )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 5;
	vscp_omsg.class = VSCP_CLASS1_INFORMATION;
	vscp_omsg.type = VSCP_TYPE_INFORMATION_STATE;
	
	vscp_omsg.data[ 0 ] = 0;	// Not used
	vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
	vscp_omsg.data[ 2 ] = subzone;
	vscp_omsg.data[ 3 ] = from_state;
	vscp_omsg.data[ 4 ] = to_state;
	
	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendHeartBeat
//

int sendHeartBeat( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS1_INFORMATION;
	vscp_omsg.type = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
	
	vscp_omsg.data[ 0 ] = 0;	// Not used
	vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
	vscp_omsg.data[ 2 ] = 0;
	
	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sendRelayState
//

int sendRelayState( uint8_t subzone, BOOL bState )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS1_INFORMATION;
	if ( bState ) {
		vscp_omsg.type = VSCP_TYPE_INFORMATION_ON;
	}
	else {
		vscp_omsg.type = VSCP_TYPE_INFORMATION_OFF;
	}
	
	vscp_omsg.data[ 0 ] = 0;	// Not used
	vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
	vscp_omsg.data[ 2 ] = subzone;
	
	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendActiveState
//

int sendActiveState( uint8_t subzone, BOOL bState )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS1_INFORMATION;
	if ( bState ) {
		vscp_omsg.type = VSCP_TYPE_INFORMATION_ACTIVE;
	}
	else {
		vscp_omsg.type = VSCP_TYPE_INFORMATION_INACTIVE;
	}
	
	vscp_omsg.data[ 0 ] = 0;	// Not used
	vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
	vscp_omsg.data[ 2 ] = subzone;
	
	// Send data
	if ( !vscp_sendMsg() ) {
		return FALSE;
	}

	return TRUE;
}

