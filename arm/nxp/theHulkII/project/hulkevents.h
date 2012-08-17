///////////////////////////////////////////////////////////////////////////////
//
// File: hulkevents.h
//
// HULK event handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: hulkevents.h,v $
// $Revision: 1.1 $
//


int sendAlarmEvent( unsigned char nAlarm );
int sendErrorEvent( unsigned char nError );
int sendTempEvent( uint8_t idx, uint8_t temp );
int sendStateEvent( uint8_t subzone, uint8_t from_state, uint8_t to_state );
int sendHeartBeat( void );
int sendRelayState( uint8_t subzone, BOOL bState );
int sendActiveState( uint8_t subzone, BOOL bState );
