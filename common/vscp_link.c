// FILE: vscp_link.c

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2019 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "vscp.h"
#include "vscp_link.h"

int vscp_link_parser( const char *cmd ) 
{
    const char *p = cmd;
    const char *pcmd;

    // Check pointer
    if ( NULL == cmd ) return VSCP_ERROR_INVALID_POINTER;

    // Remove whitespace from command
    while( *p && ( *p == ' ' ) ) {
        p++;
    }

    pcmd = p;

    if ( NULL != ( p = strstr( pcmd, "noop" ) ) ) {
        p += 4;
        vscp_link_doCmdNoop( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "help" ) ) ) {
        p += 4;
        vscp_link_doCmdHelp( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "quit" ) ) ) {
        p += 4;
        vscp_link_doCmdQuit( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "user" ) ) ) {
        p +=4;
        vscp_link_doCmdUser( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "pass" ) ) ) {
        p +=4;
        vscp_link_doCmdPassword( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "challenge" ) ) ) {
        p +=9;
        vscp_link_doCmdChallenge( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "send" ) ) ) {
        p +=4;
        vscp_link_doCmdSend( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "retr" ) ) ) {
        p +=4;
        vscp_link_doCmdRetrive( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "rcvloop" ) ) ) {
        p +=7;
        vscp_link_doCmdRcvLoop( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "quitloop" ) ) ) {
        p +=8;
        vscp_link_doCmdQuitLoop( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "cdta" ) ) ) {
        p +=4;
        vscp_link_doCmdCmdCheckData( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "chkdata" ) ) ) {
        p +=7;
        vscp_link_doCmdCheckData( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "clra" ) ) ) {
        p +=4;
        vscp_link_doCmdClearAll( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "clrall" ) ) ) {
        p +=6;
        vscp_link_doCmdClearAll( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "stat" ) ) ) {
        p +=4;
        vscp_link_doCmdStatistics( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "info" ) ) ) {
        p +=4;
        vscp_link_doCmdInfo( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "chid" ) ) ) {
        p +=4;
        vscp_link_doCmdGetChannelId( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "getchid" ) ) ) {
        p +=7;
        vscp_link_doCmdGetChannelId( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "sgid" ) ) ) {
        p +=4;
        vscp_link_doCmdSetChannelId( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "setguid" ) ) ) {
        p +=7;
        vscp_link_doCmdSetChannelId( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "ggid" ) ) ) {
        p +=4;
        vscp_link_doCmdGetGUID( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "getguid" ) ) ) {
        p +=7;
        vscp_link_doCmdGetGUID( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "vers" ) ) ) {
        p +=4;
        vscp_link_doCmdGetVersion( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "version" ) ) ) {
        p +=7;
        vscp_link_doCmdGetVersion( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "sflt" ) ) ) {
        p +=4;
        vscp_link_doCmdSetFilter( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "setfilter" ) ) ) {
        p +=9;
        vscp_link_doCmdSetFilter( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "smsk" ) ) ) {
        p +=4;
        vscp_link_doCmdSetMask( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "setmask" ) ) ) {
        p +=7;
        vscp_link_doCmdSetMask( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "wcyd" ) ) ) {
        p +=4;
        vscp_link_doCmdWhatCanYouDo( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "whatcanyoudo" ) ) ) {
        p +=12;
        vscp_link_doCmdWhatCanYouDo( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "+" ) ) ) {
        p +=1;
        vscp_link_doCmdCommandAgain( p );
    }
    else if ( NULL != ( p = strstr( pcmd, "dm" ) ) ) {
        p +=2;
        vscp_link_doCmdDM( p );
    }
#ifdef VSCP_LINK_ENABLE_CMD_VAR
    else if ( NULL != ( p = strstr( pcmd, "var" ) ) ) {
        p +=3;
        vscp_link_doCmdVariable( p );
    }
#endif   
#ifdef VSCP_LINK_ENABLE_CMD_INTERFACE
    else if ( NULL != ( p = strstr( pcmd, "interface" ) ) ) {
        p +=9;
        vscp_link_doCmdInterface( p );
    }
#endif  
    else {
        vscp_link_reply("-OK - Unknown command\r\n");
    }

    return VSCP_ERROR_SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////
// doCmdDM
//
// enable
// disable
// list
// add
// delete
// reset
// clrtrig
// clrerr
// save
// load
//

int vscp_link_doCmdDM( const char *cmd )
{
    const char *p = cmd;
    const char *pcmd;

    // Check pointer
    if ( NULL == cmd ) return VSCP_ERROR_INVALID_POINTER;

    // Remove whitespace from command
    while( *p && ( *p == ' ' ) ) {
        p++;
    }

    pcmd = p;

    if ( NULL != ( p = strstr( pcmd, "enable" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "disable" ) ) ) {
        p +=7;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "list" ) ) ) {
        p +=4;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "add" ) ) ) {
        p +=3;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "delete" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "reset" ) ) ) {
        p +=5;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "clrtrig" ) ) ) {
        p +=7;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "clrerr" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "save" ) ) ) {
        p +=6;
        
    }
    else if ( NULL != ( p = strstr( pcmd, "load" ) ) ) {
        p +=6;
        
    }
    else {
        vscp_link_reply("-OK - DM - unknown sub command\r\n");
    }

    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// doCmdVar
//
// list
// write
// read
// readvalue
// writevalue
// readnote
// writenote
// reset
// readreset
// remove
// readremove
// length
// save
// load
//

///////////////////////////////////////////////////////////////////////////////
// doCmdInterface
//
// list
// close
//



int vscp_link_reply( const char *msg )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdNoop( const char *cmd )
{
    vscp_link_callback_write("+OK\r\n");
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdHelp( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdQuit( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdUser( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdPassword( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdChallenge( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdSend( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdRetrive( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdRcvLoop( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdQuitLoop( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdCmdCheckData( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdCheckData( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdClearAll( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdStatistics( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdInfo( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdGetChannelId( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdSetChannelId( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}


int vscp_link_doCmdGetGUID( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdGetVersion( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdSetFilter( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdSetMask( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdWhatCanYouDo( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}

int vscp_link_doCmdCommandAgain( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}


#ifdef VSCP_LINK_ENABLE_CMD_VAR
int vscp_link_doCmdVariable( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}
#endif


#ifdef VSCP_LINK_ENABLE_CMD_INTERFACE
int vscp_link_doCmdInterface( const char *cmd )
{
    return VSCP_ERROR_SUCCESS;
}
#endif      

///////////////////////////////////////////////////////////////////////////////
// newEvent
//

vscpEvent *newEvent( void )
{
    vscpEvent *pev = NULL;

    pev = (vscpEvent *)VSCP_LINK_MALLOC( sizeof(vscpEvent) );
    if ( NULL != pev ) pev->pdata = NULL;

    return pev;   
}

///////////////////////////////////////////////////////////////////////////////
// deleteEvent
//

int deleteEvent( vscpEvent **pev )
{
    if ( NULL == *pev ) return VSCP_ERROR_INVALID_POINTER;

    if ( NULL != (*pev)->pdata ) {
        VSCP_LINK_FREE( (*pev)->pdata );
    }

    VSCP_LINK_FREE(*pev);
    *pev = NULL;
    
    return VSCP_ERROR_SUCCESS;   
}

///////////////////////////////////////////////////////////////////////////////
// parseGuid
//

int parseGuid( const char *strguid, char **endptr, uint8_t *guid )
{
    int i;
    char *p = (char *)strguid;

    // Check pointers
    if ( NULL == strguid ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == guid ) return VSCP_ERROR_INVALID_POINTER;

    memset( guid, 0, 16 );

    // remove initial white space
    while ( ' ' == *p ) p++;

    // Empty GUID or GUID  set to '-' means all nulls
    if ( !*p || ( '-' == *p ) )  {
        if ( NULL != endptr ) *endptr = p;
        return VSCP_ERROR_SUCCESS;
    }
    
    for ( i=0; i<16; i++ ) {
        guid[i] = (uint8_t)strtol( p, &p, 16 );
        if ( !*p ) break; // at end?
        if ( i != 15 ) {
            if ( ':' != *p )   return VSCP_ERROR_ERROR;
            p++;    // Move beyond ':'
            if ( p > (strguid + strlen(strguid) ) ) {
                if ( NULL != endptr ) *endptr = p;
                return VSCP_ERROR_ERROR;
            }
        }
    }

    if ( NULL != endptr ) *endptr = p;
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// writeGuidToString
//

int writeGuidToString( const uint8_t *guid, char *strguid )
{
    // Check pointers
    if ( NULL == strguid ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == guid ) return VSCP_ERROR_INVALID_POINTER;

    sprintf( strguid, 
                "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
                "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                guid[0], guid[1], guid[2], guid[3],
                guid[4], guid[5], guid[6], guid[7],
                guid[8], guid[9], guid[10], guid[11],
                guid[12], guid[13], guid[14], guid[15] );
    
    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseFilter
//
// filter-priority, filter-class, filter-type, filter-GUID
//

int parseFilter( const char *strfilter, vscpEventFilter *evfilter )
{
    char *p = (char *)strfilter;

    // Check pointers
    if ( NULL == strfilter ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == evfilter ) return VSCP_ERROR_INVALID_POINTER;

    // Priority
    evfilter->filter_priority = (uint8_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strfilter + strlen(strfilter) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // Class filter
    evfilter->filter_class = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;    
    if ( p > (strfilter + strlen(strfilter) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // Type filter
    evfilter->filter_type = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strfilter + strlen(strfilter) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // GUID filter
    return parseGuid( p, NULL, evfilter->filter_GUID );
}

///////////////////////////////////////////////////////////////////////////////
// parseMask
//
// mask-priority, mask-class, mask-type, mask-GUID
//

int parseMask( const char *strmask, vscpEventFilter *evfilter )
{
    char *p = (char *)strmask;

    // Check pointers
    if ( NULL == strmask ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == evfilter ) return VSCP_ERROR_INVALID_POINTER;

    // Priority
    evfilter->mask_priority = (uint8_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strmask + strlen(strmask) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // Class mask
    evfilter->mask_class = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strmask + strlen(strmask) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // Type mask
    evfilter->mask_type = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strmask + strlen(strmask) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // GUID mask
    return parseGuid( p, NULL, evfilter->mask_GUID );
}


///////////////////////////////////////////////////////////////////////////////
// parseEvent
//
// head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
//

int parseEvent( const char *strevent, vscpEvent *ev )
{
    uint8_t buf[512];
    char *p = (char *)strevent;

    // Check pointers
    if ( NULL == strevent ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == ev ) return VSCP_ERROR_INVALID_POINTER;

    // Set all defaults
    memset( ev, 0, sizeof(ev) );

    // head
    ev->head = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    if ( p > (strevent + strlen(strevent) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // VSCP class
    ev->vscp_class = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    if ( p > (strevent + strlen(strevent) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // VSCP type
    ev->vscp_type = (uint16_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++;
    if ( p > (strevent + strlen(strevent) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // obid
    ev->obid = (uint32_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    if ( p > (strevent + strlen(strevent) ) ) {
        return VSCP_ERROR_ERROR;
    }

    // datetime YYYY-MM-DDTHH:MM:SS

    // Date may not be given (left blank) and in 
    // this case it should be set to all nulls for
    // the interface to set it instead.

    if (',' == *p) {
        p++; // point beyond comma
    }
    else {
        // year
        ev->year = (uint16_t)strtol( p, &p, 0 );
        if ( '-' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond dash
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }

        // month
        ev->month = (uint16_t)strtol( p, &p, 0 );
        if ( '-' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond dash
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }

        // day
        ev->day = (uint16_t)strtol( p, &p, 0 );
        if ( 'T' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond dash
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }

        // hour
        ev->hour = (uint16_t)strtol( p, &p, 0 );
        if ( ':' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond colon
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }

        // minute
        ev->minute = (uint16_t)strtol( p, &p, 0 );
        if ( ':' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond colon
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }

        // second
        ev->second = (uint16_t)strtol( p, &p, 0 );
        if ( ',' != *p ) return VSCP_ERROR_ERROR;
        p++; // point beyond comma
        if ( p > (strevent + strlen(strevent) ) ) {
            return VSCP_ERROR_ERROR;
        }
    }

    // timestamp
    ev->timestamp = (uint32_t)strtol( p, &p, 0 );
    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    if ( p > (strevent + strlen(strevent) ) ) {
        return VSCP_ERROR_ERROR;
    }
 
    // Get GUID
    if ( VSCP_ERROR_SUCCESS != parseGuid( p, &p, ev->GUID ) ) {
        return VSCP_ERROR_ERROR;
    }

    if ( ',' != *p ) return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    ev->sizeData = 0;
    
    // Get data (if any )
    while ( ( p <= ( strevent + strlen( strevent ) ) ) &&
            ( ev->sizeData < 512 ) ) {      
        buf[ev->sizeData] = (uint8_t)strtol( p, &p, 0 );
        ev->sizeData++;
        p++; // point beyond comma
    }

    // Copy in data (if any)
    if ( ev->sizeData ) {
        ev->pdata = VSCP_LINK_MALLOC(ev->sizeData);
        if ( NULL == ev->pdata ) return VSCP_ERROR_ERROR;
        memcpy( ev->pdata, buf, ev->sizeData );
    }

    return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseEventEx
//

int parseEventEx( const char *streventex, vscpEventEx *evex )
{
    char *p = (char *)streventex;

    // Check pointers
    if ( NULL == streventex ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == evex ) return VSCP_ERROR_INVALID_POINTER;

    memset( evex, 0, sizeof( vscpEventEx ) );

    vscpEvent *ev = newEvent();
    if ( NULL == ev ) return VSCP_ERROR_ERROR;

    if ( VSCP_ERROR_SUCCESS != parseEvent( streventex, ev ) ) {
        deleteEvent( &ev );
        return VSCP_ERROR_ERROR;
    }

    // Copy in data
    evex->head = ev->head;
    evex->obid = ev->obid;
    evex->year = ev->year;
    evex->month = ev->month;
    evex->day = ev->day;
    evex->hour = ev->hour;
    evex->minute = ev->minute;
    evex->second = ev->second;
    evex->timestamp = ev->timestamp;
    evex->vscp_class = ev->vscp_class;
    evex->vscp_type = ev->vscp_type;
    memcpy( evex->GUID, ev->GUID, 16 );
    evex->sizeData = ev->sizeData;
    if ( evex->sizeData ) {
        memcpy( evex->data, ev->pdata, evex->sizeData );
    }

    deleteEvent( &ev );

    return VSCP_ERROR_SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////
// eventToString
//
// head       4  - 0xff or 255
// class      6  - 0xFFFF or 65535
// type       6  - 0xFFFF or 65535
// obid       10 - 0xFFFFFFFF
// time       20 - YYYY-MM-DDTHH:MM:SSZ
// timestamp  10 - 0xFFFFFFFF
// GUID       47 - 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
// ---------------------------------------------------------------
// Total:     103 + 7 (commas) + data = 110 + data
//

int eventToString( const vscpEvent *ev, char *strevent, size_t len )
{
    char buf[48];   // Can hold full GUID

    // Check pointers
    if ( NULL == ev ) return VSCP_ERROR_INVALID_POINTER;
    if ( NULL == strevent ) return VSCP_ERROR_INVALID_POINTER;

    // Len must be able to hold content 
    if ( len < 110 ) return VSCP_ERROR_BUFFER_TO_SMALL;

    if ( ev->year || ev->month || ev->day || ev->hour || ev->minute || ev->second ) {
        sprintf( strevent, 
                    "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,", 
                    (unsigned)ev->head, (unsigned)ev->vscp_class, (unsigned)ev->vscp_type, 
                    (unsigned long)ev->obid, 
                    (int)ev->year, (int)ev->month, (int)ev->day, 
                    (int)ev->hour, (int)ev->minute, (int)ev->second,
                    (unsigned long)ev->timestamp );
    }
    else {
        sprintf( strevent, 
                    "%d,%u,%u,%lu,,%lu,", 
                    (unsigned)ev->head, (unsigned)ev->vscp_class, (unsigned)ev->vscp_type, 
                    (unsigned long)ev->obid, (unsigned long)ev->timestamp );
    }

    // GUID
    writeGuidToString( ev->GUID, buf );                
    strcat( strevent, buf );

    // Data
    if ( ( NULL != ev->pdata ) && ev->sizeData ) {

        if ( len < ( strlen(strevent) + 1 + 1 ) ) {
            return VSCP_ERROR_BUFFER_TO_SMALL;
        }
        strcat( strevent, "," );

        for ( int i=0; i < ev->sizeData; i++ ) {

            if ( i < (ev->sizeData-1) ) {
                sprintf( buf, "%d,", (int)ev->pdata[i] );
            }
            else {
                sprintf( buf, "%d", (int)ev->pdata[i] );
            }

            // Check that data fits
            if ( len < ( strlen(strevent) + 1 + strlen( buf ) ) ) {
                return VSCP_ERROR_BUFFER_TO_SMALL;
            }

            // Copy in the data
            strcat( strevent, buf );

        }

    }

    return VSCP_ERROR_SUCCESS;
}

