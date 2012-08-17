///////////////////////////////////////////////////////////////////////////////
//
// File: debug.c
//
// debug functionality
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: debug.c,v $
// $Revision: 1.4 $
//

#include "uart.h"
#include "debug.h"
#include "armVIC.h"

#ifdef ENABLE_DEBUG

///////////////////////////////////////////////////////////////////////////////
// debugWrite
//

void debugWrite( char *p )
{
	unsigned mycpsr;
	const char *pp = p;
	
	while ( 0 != *pp ) {
	
		pp = uart0Puts( pp );
		
		mycpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( mycpsr );
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// debugWritecrlf
//

void debugWritecrlf( char *p )
{
	debugWrite( p );
	uart0Puts( "\r\n" );
}

#endif
