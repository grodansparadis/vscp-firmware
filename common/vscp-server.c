///////////////////////////////////////////////////////////////////////////////
// File: vscp_server.c
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2022 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
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

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <inttypes.h>
#include "vscp-compiler.h"
#include "vscp-projdefs.h"
#include "vscp-server.h"

///////////////////////////////////////////////////////////////////////////////
// vscpServerParser
//

int vscpServerParser( char *pCommand )
{
    int rv = 0;
    uint16_t i;
    
    // Convert string to uppercase
    i = 0;
    while ( pCommand[ i ] ) {
        pCommand[ i ] = toupper( pCommand[ i ] );
        i++;
    }  
      
    // ********************************************************************
    //                                 QUIT
    // *********************************************************************
    if ( NULL != strstr( pCommand, "QUIT" ) ) {
        vscp_server_sendReply( MSG_GOODBY );
        vscp_server_command_quit();
    }
    //*********************************************************************
    //                            No Operation
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "NOOP" ) ) {
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                             Send event
    //
    // "send head,class,type,obid,timestamp,GUID,data1,data2,data3...."
    //*********************************************************************     
    else if ( NULL != strstr( pCommand, "SEND" ) ) {
                
        // parse the send argument
        vscpEvent event;
        if ( VSCP_SRV_ERROR_SUCCESS == vscp_server_parse_send( pCommand + 4, 
                                                                    &event ) ) {
            // Handle the command                                    
            vscp_server_command_send( &event );    
        }    
        else {                            
            vscp_server_sendReply( MSG_PARAMETER_ERROR );
        }
    }      
    //*********************************************************************
    //                            Read event
    // "RETR n" the argument can be omitted meaning "1"
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "RETR" ) ) {
        
        char *p = pCommand + 4;
        vscpEvent event;
#ifdef VSCP_LEVEL2_LIMITED_DEVICE
        char buf[ 31 + 64 +  3 * ( 512 - 25 + 1 ) ];
#else        
        char buf[ 31 + 64 +  3 * ( LIMITED_DEVICE_DATASIZE + 1 ) ];
#endif        

        // Get read count
        uint16_t count = readStringValue( p );
                
        // Default to read one event
        if ( 0 == count ) count = 1;
        
        char wrkbuf[ 32 ];
        while ( count-- ) {
            
            // Get event
            if ( !vscp_server_command_retr( &event ) ) break; 
            
            // Empty reply string
            buf[ 0 ] = 0x00;
        
            // Event available. We should output a dataline.
            // Format is
            // head,class,type,obid,timestamp,GUID,data0,data1,data2,...........<cr><lf>
            
            // head
            itoa( event.head, wrkbuf );
            strcat( buf, wrkbuf ); 
            strcat( buf, "," );
             
            // class 
            itoa( event.vscp_class, wrkbuf );
            strcat( buf, wrkbuf ); 
            strcat( buf, "," ); 
            
            // type
            itoa( event.vscp_type, wrkbuf );
            strcat( buf, wrkbuf ); 
            strcat( buf, "," );
            
            // obid  
            strcat( buf, "0," );
            
            // timestamp
            itoa( vscp_server_getTicks(), wrkbuf );
            strcat( buf, wrkbuf ); 
            strcat( buf, "," );
            
            // GUID
            vscp_server_GUID2String( event.GUID, wrkbuf );
            strcat( buf, wrkbuf ); 
            strcat( buf, "," );
            
            // Data
            vscp_server_Data2String( event.data, event.sizeData, buf + strlen( buf ) );
            strcat( buf, "\r\n" );
            vscp_server_sendReply( buf );
        }
            
        vscp_server_sendReply( MSG_OK );    
    }
    //*********************************************************************
    //                       Check if data is available
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "CDTA" ) ) {
        uint32_t cnt = vscp_server_command_cdta();
        char buf[ 32 ];
        atoi( cnt, buf );
        strcat( buf, "\r\n" );
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                          Clear input queue
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "CLRA" ) ) {
        vscp_server_command_clra();
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                        Get Statistics
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "STAT" ) ) {
        vscp_statistics stat;
        char buf[ 80 ];
        char wrkbuf[ 10 ];
        vscp_server_command_stat( &stat );
        
        // Null string
        *buf = 0x00;
        
        // Bus off's
        itoa( stat.cntBusOff, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Bus warnings
        itoa( stat.cntBusWarnings, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Overruns
        itoa( stat.cntOverruns, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Received data
        itoa( stat.cntReceiveData, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Received frames
        itoa( stat.cntReceiveFrames, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Transmitted data
        itoa( stat.cntTransmitData, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Transmitted frames
        itoa( stat.cntTransmitFrames, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "\r\n" );
        
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                          Get Status
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "INFO" ) ) {
        vscp_interface_info info;
        char buf[ 80 ];
        char wrkbuf[ 10 ];
        
        // Null string
        *buf = 0x00;

        vscp_server_command_info( &info );
        
        // Channel status flags/code
        itoa( info.channel_status, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Last error code
        itoa( info.lasterrorcode, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        // Last error sub code
        itoa( info.lasterrorsubcode, wrkbuf );
        strcat( buf, wrkbuf );
        strcat( buf, "," );
        
        strcat( buf, info.lasterrorstr );
        strcat( buf, "\r\n" );
        
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                           Get Channel ID
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "CHID" ) ) {
        char buf[ 10 ];
        
        // Null string
        *buf = 0x00;
        
        uint32_t chid = vscp_server_command_chid();
        itoa( chid, buf );
        strcat( buf, "\r\n" );
        
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                          Set Channel GUID
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "SGID" ) ) {
        char *p = pCommand + 4;
        uint8_t GUID[ 16 ];
        
        vscp_server_ParseGUID( p, GUID );
        vscp_server_command_sgid( GUID );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                          Get Channel GUID
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "GGID" ) ) {
        uint8_t GUID[ 16 ];
        char buf[ 32 ];
        
        // Fetch the interface GUID
        vscp_server_command_ggid( GUID );
        vscp_server_GUID2String( GUID, buf );
        strcat( buf, "\r\n" );
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                             Get Version
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "VERS" ) ) {
        
        uint8_t major;
        uint8_t minor;
        uint8_t sub;
        char wrk[ 5 ];
        char buf[ 32 ];
        
        // Get version
        vscp_server_command_ver( &major, &minor, &sub );
        
        buf[ 0 ] = 0x00;    // Empty string
        
        // Major version
        itoa( major, wrk );
        strcat( buf, wrk );
        strcat( buf, "," );
        
        // Minor version
        itoa( minor, wrk );
        strcat( buf, wrk );
        strcat( buf, "," );        
        
        // Sub minor version
        itoa( sub, wrk );
        strcat( buf, wrk );
        strcat( buf, "\r\n" );        
        
        vscp_server_sendReply( buf );
        vscp_server_sendReply( MSG_OK );
    }
	//*********************************************************************
	//                             Set Filter
	//*********************************************************************    
    else if ( NULL != strstr( pCommand, "SFLT" ) ) {
        
        uint8_t filter_priority = 0;
        uint16_t filter_class = 0;
        uint8_t filter_type = 0;
        char *p;
        uint8_t filter_GUID[ 16 ];
        
        // Nill GUID
        memset( filter_GUID, 0x00, 16 );
        
        p = strtok( pCommand + 4, "," );
        if ( NULL != p ) filter_priority = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) filter_class = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) filter_type = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) vscp_server_ParseGUID( p, filter_GUID );
        
        // Set the filter
        vscp_server_command_sflt( filter_priority,
                                      filter_class,
                                      filter_type,
                                      filter_GUID );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                             Set Mask
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "SMSK" ) ) {
        
        uint8_t mask_priority = 0;
        uint16_t mask_class = 0;
        uint8_t mask_type = 0;
        char *p;
        uint8_t mask_GUID[ 16 ];
        
        // Nill GUID
        memset( mask_GUID, 0x00, 16 );
        
        p = strtok( pCommand + 4, "," );
        if ( NULL != p ) mask_priority = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) mask_class = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) mask_type = readStringValue( p );
        
        p = strtok( NULL, "," );
        if ( NULL != p ) vscp_server_ParseGUID( p, mask_GUID );
        
        // Set the filter
        vscp_server_command_smsk( mask_priority,
                                      mask_class,
                                      mask_type,
                                      mask_GUID );
        vscp_server_sendReply( MSG_OK );
    }
    //*********************************************************************
    //                               Username
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "USER" ) ) {
        vscp_server_command_user( pCommand + 4 );
        vscp_server_sendReply( MSG_USENAME_OK );    // YES!!! Always OK    
    }
    //*********************************************************************
    //                             Password
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "PASS" ) ) {
        if ( vscp_server_command_password( pCommand + 4 ) ) {
            vscp_server_sendReply( MSG_PASSWORD_ERROR );   
        }   
        else {
            vscp_server_sendReply( MSG_PASSWORD_OK );
        }     
    }
    //*********************************************************************
    //                           Enter Binary Mode
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "FAST" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
    //                        + (repeat last command)
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "+" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
	//*********************************************************************
    //                             Rcvloop
    //*********************************************************************    
    else if ( NULL != strstr( pCommand, "RCVLOOP" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
    //                              Help
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "HELP" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
    //                              Restart
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "RESTART" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
    //                         Client/interface
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "INTERFACE" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
    //                               Test
    //*********************************************************************
    else if ( NULL != strstr( pCommand, "TEST" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
	//                               Shutdown
	//*********************************************************************
    else if ( NULL != strstr( pCommand, "SHUTDOWN" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
	//*********************************************************************
    //                               canmode
	//*********************************************************************    
    else if ( NULL != strstr( pCommand, "CANMODE" ) ) {
        vscp_server_sendReply( MSG_NOT_IMPLEMENTED );
    }
    //*********************************************************************
	//                               Que?
	//*********************************************************************
    else {
        // Unknown command
        vscp_server_sendReply( MSG_UNKNOWN_COMMAND );
    }    
    
    return rv;
}

// case-independent ASCII character equality comparison
#define     CIEQ(c1, c2)     ( ((c1)&~040) == ((c2)&~040) )


////////////////////////////////////////////////////////////////////////////////////
// a2ul
// 
 
const char * a2ul( const char *src, uint8_t srclen, uint8_t base, uint32_t *resultp )
{
	const char *stop;
	static char hex[] = "0123456789abcdef";
	static char uchex[] = "0123456789ABCDEF";
	int d;
	char c;
	char *p;
	unsigned long r;
	unsigned long rlimit;
	int dlimit;

	if ( 0 == srclen ) {
	    srclen = strlen(src);
	}
		
	if  ( 0 == srclen ) {
	    return "empty string";
	}	

	if ( base == 0 || base == 13 ) {
	
		if ( srclen > 2 && *src == '0' && CIEQ(*(src+1), 'x') ) {
			return a2ul( src + 2, srclen - 2, 16, resultp );
		}	
	
		if ( srclen > 1 && *src == '0' && base != 13 ) {
			return a2ul( src + 1, srclen - 1, 8, resultp );
		}	
	
		return a2ul( src, srclen, 10, resultp );
	}
	
	if ( base != 8 && base != 10 && base != 16 ) {
		return "unsupported number base";
	}	

	r = 0;
	stop = src + srclen;
	if (base == 16) {
		while (src < stop) {
			c = *src++;
			p = strchr(hex, c);
			if ( p != NULL ) {
				d = p - hex;
			}	
			else {
				p = strchr(uchex, c);
				if ( p == NULL ) {
					return "non-hex-digit in hex number";
				}	
				d = p - uchex;
			}
			r = (r << 4) | d;
		}
		
		// defer length check to catch invalid digits first 
		if ( srclen > sizeof(unsigned long ) * 2 ) {
			return "hex number too long";
		}	
	} 
	else {
		rlimit = ULONG_MAX / base;
		dlimit = (int)(ULONG_MAX - rlimit*base);
		
        while (src < stop) {
			
			c = *src++;
			d = c - '0';
			
    		if ( d < 0 || d >= base ) {
				return "non-digit in number";
			}	
			
			if ( r > rlimit || ( r == rlimit && d > dlimit ) ) {
				return "unsigned-long overflow";
			}
				
			r = r*base + d;
			
		}
	}

    *resultp = r;
	return NULL;
	
}

////////////////////////////////////////////////////////////////////////////////////
// dec2hex
//

void dec2hex( uint8_t dec, char * pBuf, uint16_t len )
{
    uint32_t i;

	for (i = 0; i < len; i++) {
		uint32_t value = dec & 0x0f;
		if ( value < 10 ) {
			pBuf[ len - i - 1 ] = value + '0';
		}	
		else {
			pBuf[ len - i - 1 ] = value - 10 + 'A';
		}	
		
		dec >>= 4;
		
	}
}    

/*
uintmax_t hex2dec(const unsigned char *data, unsigned int len)
{
	unsigned int i;
	uintmax_t value = 0;

	for (i = 0; i < len; i++) {
		value = value*0x10;
		if (data[i] >= '0' && data[i] <= '9')
			value += data[i]-'0';
		else if (data[i] >= 'A' && data[i] <= 'F')
			value += data[i]-'A' + 10;
		else
			return 0;
	}
	return value;
}
*/


////////////////////////////////////////////////////////////////////////////////////
// hex2dec
//

uint32_t hex2dec( const char *pHex )
{
    int i;
    uint32_t val = 0;
    uint8_t len = strlen( pHex );
    
    for ( i = 0; i < len; i++ ) {
        if ( 0x20 == pHex[ i ] ) break;
        if ( pHex[ i ] <= 57 ) {
            val += ( pHex[ i ] - 48 ) * ( 1 << ( 4 * ( len- 1 - i ) ) );
        }    
        else {
            val += ( pHex[ i ] - 55 ) * ( 1 << ( 4 * ( len - 1 - i ) ) );
        }
    }        
    return val;
}


////////////////////////////////////////////////////////////////////////////////////
// readStringValue
//

uint32_t readStringValue ( char *pString )
{
    uint32_t ul;
    char *p;
    
    // Convert string to uppercase
    uint16_t i = 0;
    while ( pString[ i ] ) {
        pString[ i ] = tolower( pString[ i ] );
        i++;
    }
    
    if ( NULL != ( p = strstr( pString, "0x" ) ) ) {
        p += 2; // Point at the data
        a2ul( pString, 0, 16, &ul );
        return ul;
    }
    else
    {
        a2ul( pString, 0, 10, &ul );
        return ul;
    } 

  return ul;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_ParseGUID
//

void vscp_server_ParseGUID( char *pArg, uint8_t *pGUID )
{
    uint8_t i;
    char *p;
    
    memset( pGUID, 0x00, 16 );
    if ( NULL == pArg ) return;
    
    p = strtok( pArg, ":" );
    for ( i=0; i<16; i++ ) {
        if ( NULL == p ) break;
        pGUID[ 15-i ] = readStringValue( p ); 
        p = strtok( NULL, ":" );
    }    
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_GUID2String
//

void vscp_server_GUID2String( uint8_t *pGUID, char *pbuf )
{
    int i;
    char wrkbuf[ 2 ];
    
    pbuf[ 0 ] = 0x00;    // Empty string
    for ( i=0; i<16; i++ ) {
        dec2hex( pGUID[ 15-i ], wrkbuf, 2 );
        strcat( pbuf, wrkbuf );
        if ( i != 15 ) strcat( pbuf, ":" );
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_ParseData
//

uint16_t vscp_server_ParseData( char *pArg, uint8_t *pData, uint16_t *pcntData )
{
    char *p;
    if ( NULL == pArg ) return VSCP_SRV_ERROR_ARGUMENT;
    
    *pcntData = 0;
    p = strtok( pArg, ":" );
#ifdef VSCP_LEVEL2_LIMITED_DEVICE
    uint16_t datasize = VSCP_LEVEL2_LIMITED_BUFFER_SIZE;
#else    
    uint16_t datasize = 512-25;
#endif    
    for ( *pcntData=0; *pcntData < datasize; *pcntData++ ) {
        if ( NULL == p ) break;
        pData[ *pcntData ] = readStringValue( p ); 
        p = strtok( NULL, ":" );
    }   
    
    return *pcntData;    
}    

///////////////////////////////////////////////////////////////////////////////
// vscp_server_Data2String
//

void vscp_server_Data2String( uint8_t *pData, uint16_t size, char *pbuf )
{
    int i;
    char wrkbuf[ 2 ];
    
    pbuf[ 0 ] = 0x00;    // Empty string
    for ( i=0; i<size; i++ ) {
        dec2hex( pData[ 15-i ], wrkbuf, 2 );
        strcat( pbuf, wrkbuf );
        if ( i != 15 ) strcat( pbuf, ":" );
    }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_server_parse_send
//

int vscp_server_parse_send( char *pArg, vscpEvent *pEvent ) 
{
    // Get VSCP head
    char *p = strtok( pArg, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    pEvent->head = readStringValue( p );
    
    // Get VSCP class
    p = strtok( NULL, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    pEvent->vscp_class = readStringValue( p );  
    
    // Get VSCP type
    p = strtok( NULL, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    pEvent->vscp_type = readStringValue( p ); 
    
    // Get VSCP obid
    p = strtok( NULL, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    uint32_t pvscp_obid = readStringValue( p );
    
    // Get VSCP timestamp
    p = strtok( NULL, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    uint32_t pvscp_timestamp = readStringValue( p );
    
    // Get VSCP GUID
    p = strtok( NULL, "," );
    if ( NULL == p ) return VSCP_SRV_ERROR_ARGUMENT;
    vscp_server_ParseGUID( p, pEvent->GUID );
    
    // Get VSCP data if any
    p = strtok( NULL, "," );
    if ( NULL != p ) {
        vscp_server_ParseData( p, pEvent->data, &pEvent->sizeData );
    }    
    
    return VSCP_SRV_ERROR_SUCCESS;
}


