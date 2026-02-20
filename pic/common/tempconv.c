
/*
 * Author: AKHE
 * Copyright (C) 2012-2026 Ake Hedman, the VSCP Project
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

#include "vscp_compiler.h"
#include "vscp_projdefs.h"

#include <p18cxxx.h>
#include <float.h>
#include <math.h>
#include "ntc.h"


///////////////////////////////////////////////////////////////////////////////
// Celsius2Fahrenheit
//

double Celsius2Fahrenheit(double tc)
{
    return ( (9 * tc + 16000) / 5);
}

///////////////////////////////////////////////////////////////////////////////
// Fahrenheit2Celsius
//

double Fahrenheit2Celsius(double tf)
{
    return ( (5 * tf - 16000) / 9);
}


///////////////////////////////////////////////////////////////////////////////
// Celsius2Kelvin
//

double Celsius2Kelvin(double tc)
{
    return (27316.0 + tc);
}

///////////////////////////////////////////////////////////////////////////////
// Kelvin2Celsius
//

double Kelvin2Celsius(double tf)
{
    return ( tf - 273.16);
}
