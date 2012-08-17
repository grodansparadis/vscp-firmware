/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File:vscp_projdefs.h
 *
 *  SweetBox Module
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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

// Undef if GUID is in EEPROM
#define GUID_IN_EEPROM

// undef if Manufacturer id is in EEPROM
#define MANUFACTURER_INFO_IN_EEPROM

// Undef this if it should be OK to write to protected
// locations under special situations
//#define ENABLE_WRITE_2PROTECTED_LOCATIONS

// Number of decisision matrix rows
#define VSCP_DECISION_MATRIX_ROWS      32

// Size for decision matrix action argument
#define VSCP_ACTION_PARAM_SIZE          4

// Define one of VSCP_USE_TCP or VSCP_USE_UDP
#define VSCP_USE_TCP
//#define VSCP_USE_USP

// undefine if fallback from TCP to UDP should be
// used.  
//#define VSCP_TCP_FALLBACK

// Undefine this if this is a memory limited device that can't hold
// a full Level II event buffer of 512-25 bytes
#define VSCP_LEVEL2_LIMITED_DEVICE

// This is the size ues for the limited ize buffer. The default value
// of 32 is usually a good value 
#define VSCP_LEVEL2_LIMITED_BUFFER_SIZE    32
