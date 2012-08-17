/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: smartec.c
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

#include "vscp_compiler.h" 	// This file should be in your project folder
#include "vscp_projdefs.h"	// This file should be in your project folder
#include "smartec.h"


///////////////////////////////////////////////////////////////////////////////
// Celsius2Fahrenheit
//

long Celsius2Fahrenheit( long tc )
{
	return ( ( 9 * tc + 16000 ) / 5 );
}

///////////////////////////////////////////////////////////////////////////////
// Fahrenheit2Celsius
//

long Fahrenheit2Celsius( long tf )
{
	return ( (5 * tf - 16000 ) / 9);
}


///////////////////////////////////////////////////////////////////////////////
// Celsius2Kelvin
//

long Celsius2Kelvin( long tc )
{
	return ( tc + 27316 );
}

///////////////////////////////////////////////////////////////////////////////
// Kelvin2Celsius
//

long Kelvin2Celsius( long tf )
{
	return ( tf - 27316 );
}
 
