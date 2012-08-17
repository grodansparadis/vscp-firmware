/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: vscp_server_implemention.h
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

//
// This file implements the callbacks from vscp_server.c/vscp_server.h
//

#include <p32xxxx.h>
#include <plib.h> 

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include <inttypes.h>                 // Include path set to .../firmware/common/

#include <vscp_server.h>

#include "timebase.h"

///////////////////////////////////////////////////////////////////////////////
// vscp_server_sendReply
//

void vscp_server_sendReply( char *pReply )
{
    // TODO
}


///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_quit
//

void vscp_server_command_quit( void )
{
    // TODO
}


///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_send
//

void vscp_server_command_send( vscpEvent *pEvent )
 {
    // TODO    
 }  
 
 
///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_retr
//

 int vscp_server_command_retr( vscpEvent *pevent )
 {
     // TODO
     return 0;
 }  
 
 
///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_cdta
//

uint32_t vscp_server_command_cdta( void )
{
    // TODO
    return 0; 
}
 
///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_cdta
//

void vscp_server_command_clra()
{
    // TODO
}
 
///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_stat
//
 
void vscp_server_command_stat( vscp_statistics *pStatistics )
{
    pStatistics->cntBusOff = 0;
    pStatistics->cntBusWarnings = 0;
    pStatistics->cntOverruns = 0;
    pStatistics->cntReceiveData = 0;
    pStatistics->cntReceiveFrames = 0;
    pStatistics->cntTransmitData = 0;
    pStatistics->cntTransmitFrames = 0;
}
 
///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_info
//
 
void vscp_server_command_info( vscp_interface_info *pInfo )
{
    pInfo->channel_status = 0;
    pInfo->lasterrorcode = 0;
    pInfo->lasterrorsubcode = 0;
    *pInfo->lasterrorstr = 0x00;
} 

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_chid
//

uint32_t vscp_server_command_chid( void )
{
    // TODO
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_sgid
//

void vscp_server_command_sgid( uint8_t *pGUID )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_ggid
//

void vscp_server_command_ggid( uint8_t *pGUID )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_ver

//

void vscp_server_command_ver( uint8_t *major, uint8_t *minor, uint8_t *sub )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_sflt
//

void vscp_server_command_sflt( uint8_t filter_priority,
                                    uint16_t filter_class,
                                    uint8_t filter_type,
                                    uint8_t *filter_GUID )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_smsk
//

void vscp_server_command_smsk( uint8_t mask_priority,
                                    uint16_t mask_class,
                                    uint8_t mask_type,
                                    uint8_t *mask_GUID )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_user
//

void vscp_server_command_user( char *pUser )
{

}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_command_password
//

int vscp_server_command_password( char *pPassword )
{

    return 0;   // Invalid password
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_getTicks
//

uint32_t vscp_server_getTicks( void )
{
    timeBaseGet32();
}
