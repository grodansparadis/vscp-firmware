///////////////////////////////////////////////////////////////////////////////
//
// File: itoa.h
//
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: itoa.h,v $
// $Revision: 1.3 $
//

#define _itoa(a,b,c) __itoa(a,b,c,32)

char* __itoa( int num, char *result, int base, int nbrNum );
char* itoa( int16_t i );
