///////////////////////////////////////////////////////////////////////////////
//
// File: lawicel232can.c
//
// Lawicel CAN232 (http://www.can232.com) handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: lawicel232can.c,v $
// $Revision: 1.6 $
//

#include <string.h>
#include <stdlib.h>
#include "lawicel232can.h"
#include "uart.h"
#include "itoa.h"
#include "armVIC.h"

///////////////////////////////////////////////////////////////////////////////
// SendFrame
//

boolean SendCANFrame( uint32_t id, uint8_t size, uint8_t *pData )
{
	unsigned _cpsr;
	int i;
	char wrkbuf[ 10 ];
	char canmsg[ 50 ];
	const char *p;
	
	if ( size > 8 ) return false;
	
	_itoa( id, wrkbuf, 16 );
	strcpy( canmsg, "T" );
	
	// Add zeros in front of id so length always is 8
	for ( i=0; i < ( 8 - strlen( wrkbuf ) ); i++ ) {
		strcat( canmsg, "0" );
	}
	
		
	// Copy in id
	strcat( canmsg, wrkbuf );
	
	// data size
	_itoa( size, wrkbuf, 16 );
	strcat( canmsg, wrkbuf );
	
	for ( i=0; i<size; i++ ) {
		
		_itoa( pData[ i ], wrkbuf, 16 );
		
		if ( 1 == strlen( wrkbuf ) ) {
			strcat( canmsg, "0" );
		}
		
		strcat( canmsg, wrkbuf );
	}
	
	// add CR at end
	strcat( canmsg, "\r" );
	
	// Send the frame
	//uart1Puts( canmsg );
	
	p = canmsg;
	
	while ( 0 != *p ) {
		
		p = uart1Puts( p );
		
		_cpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( _cpsr );
		
	}
	
	//uart0Puts( canmsg );
	//uart0Puts( "\r\n" );
	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// getFrame
//

boolean getCANFrame( uint32_t *pid, uint8_t *psize, uint8_t *pData )
{
	int i;
	int c;
	
	char * pEnd;
	static char canmsg[ 50 ];
	static int cnt;
	static boolean bMsg = false;
	
	// Must be valid pointers
	if ( NULL == pid ) return false;
	if ( NULL == psize ) return false;
	if ( NULL == pData ) return false;
	
	// If no character in the queue => No message
	while ( -1 != ( c = uart1Getch() ) ) {
	
		if ( bMsg ) {
		
			if ( 0x0d == c ) {
			
				// A full frame received
				//		Format iiiiiiiidxxxxxxxxxxxxxxxx
				//			iiiiiiii - id
				//			d - Number of databytes
				//			xxx....databytes
				
				
				// Get datacount first
				*psize = ( canmsg[ 8 ] - 0x30 );
				canmsg[ 8 ] = 0;
				
				*pid = strtoul( canmsg, &pEnd, 16 );
				
				strcpy( canmsg, canmsg + 9 );
				for ( i=*psize-1; i>=0; i-- ) {
					pData[ i ] = (unsigned char)strtoul( canmsg + 2 * i, &pEnd, 16 );
					canmsg[ 2 * i ] = 0;
				}
				
				SendCANFrame( *pid, *psize, pData );
				cnt = 0;
				bMsg = false;
				return true;
				
			}
			else {
				canmsg[ cnt ] = (char)c;
				cnt++;
				canmsg[ cnt ] = 0;
				if ( cnt > sizeof( canmsg ) ) {
					// Something is wrong - start again (no frame is this long)
					cnt = 0;
					bMsg = false;
				}
			}
		}
		else {
			if ( 'T' == c ) {
				bMsg = true;	// We have a message
				cnt = 0;
			}
		}
		
	}
	
	return false;
	
}
