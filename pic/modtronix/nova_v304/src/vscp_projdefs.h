/**
 * @brief           Project configuration
 * @file            vscp_projdefs.h
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_conf_projdefs
 *
 * @section description Description
 **********************************
 *
 * For details on configuring the project, see @ref mod_conf_vscp_projdefs "Project Configuration" module!
 *
 *********************************************************************/

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Copyright (C) 1995-2007 Ake Hedman, eurosource
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

#ifndef VSCP_PROJDEFS_H
#define VSCP_PROJDEFS_H

/*
IMPORTANT!
==========

Start with projdefs.h  In this file the IP address, mask, default gateway and Ethernet MAC address 
should be set.
*/

// Undefine if VSCP UDP functionality should be used
#define VSCP_USE_UDP

// Undefine  if VSCP TCP functionality should be used
#define VSCP_USE_TCP

// Undefine if the firmware should try to discover a TCP server
// and use that server if possible. Both of TCP and UDP must be enabled
// for this to work.
//	
// If VSCP_TCP_FALLBACK is defined UDP will be used if server discovery fails.
//
#define VSCP_DISCOVER_SERVER

// Undefine to fallback to UDP if no TCP server is found.
// Only have meaning if VSCP_DISCOVER_SERVER also is defined. UDP will be used
// if server discovery fails. If not defined discovery will continue until a server is found.
//#define VSCP_TCP_FALLBACK

// Undefine to use hardcoded ip for TCP server. No autodiscover is used if this
// is defined
//#define VSCP_SERVERIP_HARDCODED_IPADDR

// Undef to use Level I decision matrix instead of Level II 
#define VSCP_USE_LEVEL1_DM

// Uncomment the following to reduce memory usage
// Only 32 bytes will used for data instead of 512-25
//#define VSCP_LEVEL2_LIMITED_DEVICE	

// A Decision Matrix row for VSCP Level II is 14 bytes plus action parameters.
// The define below set the size for the action parameters.
#define VSCP_ACTION_PARAM_SIZE              18 

// Uncoment to send a hearbeat every minute
#define VSCP_SEND_MINUTE_HEARTBEAT

// Implement VSCP BOOTLOADER
//#define VSCP_ENABLE_BOOTLOADER

// Uncoment to use NTP server
//#define STACK_USE_NTP_SERVER

// Manufacturer ID in EEPROM
//#define VSCP_USE_EEPROM_FOR_MANUFACTURERE_ID

#endif
