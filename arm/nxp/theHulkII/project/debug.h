///////////////////////////////////////////////////////////////////////////////
//
// File: debug.h
//
// debug functionality
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: debug.h,v $
// $Revision: 1.1 $
//

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUGWRITE( p )  		debugWrite( p )
#define DEBUGWRITECRLF( p )  	debugWritecrlf( p )
#else
#define DEBUGWRITE( p )  		
#define DEBUGWRITECRLF( p )  		
#endif

void debugWrite( char *p );
void debugWritecrlf( char *p );
