///////////////////////////////////////////////////////////////////////////////
// File: display.c
//
// Display functionality
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: display.c,v $
// $Revision: 1.3 $
//

#include "hulk.h"
#include "display.h"
#include "string.h"

///////////////////////////////////////////////////////////////////////////////
// clrDisplayBuf
//

void clrDisplayBuf( void )
{
	memset( gdisplaybuf1, 0, sizeof( gdisplaybuf1 ) );
	memset( gdisplaybuf2, 0, sizeof( gdisplaybuf2 ) );
}

///////////////////////////////////////////////////////////////////////////////
// padBuf
//

char * padBuf( char *p )
{
	while ( strlen( p ) < 16 )  {
		strcat( p, " " );
	}
	
	return p;
}
