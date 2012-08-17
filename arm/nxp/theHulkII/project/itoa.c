///////////////////////////////////////////////////////////////////////////////
//
// File: itoa.c
//
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: itoa.c,v $
// $Revision: 1.3 $
//

#include "types.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "itoa.h"
  
char * __itoa(int val, char *result, int base, int nbrNum )
{
	int j=0;
	int i=0;
	int k=0;
	int l=0;
	int temp;
	char *p, *p2;
	char res[32];
	char res2[nbrNum+1];
	char cList[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	
	if ( result == NULL || base > 16 || base < 2 ) return NULL;
	
	if ( 0 == val ) {
		strcpy( result, "0" );
		return result;
	}
	
	j = val;
	i = 31;
	
	while ( i >= 0 ) {
		l = pow( base, i );
		temp = (int)j/l;
		res[ k++ ] = cList[ temp ];
		j -= temp * l;
		i--;
	}
	
	i = 0;
	j = 31-(nbrNum-1);
	for ( k=j; k<32; k++ ) {
		res2[i++] = res[k];
	}
    
	res2[ nbrNum ]='\0';
	
	// Take away leading zeros
	p = p2 = res2;
	while( *p ) {
		if ( *p != '0' ) {
			p2 = p;
			break;
		}

		p++;
	}
	
	memcpy( result ,p2, strlen( p2 ) + 1 );
	return result;
 }


#define __MAX_INT_CHARS 7

char* itoa(int16_t i)
{
	static char a[__MAX_INT_CHARS];
	
	char *b = a + sizeof(a) - 1;
	
	uint8_t   sign = (i < 0);

	if (sign) i = -i;
	
	*b = 0;
	
	do {
		*--b = '0' + (i % 10);
		i /= 10;
	} while (i);
	
	if (sign) *--b = '-';
	
	return b;
}

