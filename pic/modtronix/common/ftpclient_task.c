// File: loadtask.c
// 
// 

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * These headers must be included for required defs.
 */

#include "stacktsk.h"
#include "tick.h"
#include "mac.h"

#if defined(STACK_USE_DHCP)
#include "dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "http.h"
#endif

#include "mpfs.h"

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "ftp.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif


// For debug only.
#include "ARPTsk.h"
#include "arp.h"
#include "tcp.h"
#include "icmp.h"
#include "delay.h"
#include "debug.h"
#include "appcfg.h"

#include "ftpclient_task.h"

#define DEBUG

#ifdef DEBUG
#define DBG_OUT(a)	serPutByte(a)
#else
#define DBG_OUT(a)	
#endif

// Flag that is true if work should be done and stays that while working
extern BOOL bftpLoadWork;	

static short ftp_state = LFTP_STATE_NONE;

// Tick for last activity
static unsigned long lastActivity = 0;

// LFTP socket
static NODE_INFO ftp_nodeinfo;

// Socket for lftp server connection
static TCP_SOCKET ftpsocket;

// Socket for lftp server data connection
static TCP_SOCKET datasocket;

// for the data channel
static TCP_PORT portdata;

// Count before failure
static unsigned char cntBeforeFail;


ROM char user[] ="USER anonymous\r\n";
ROM char pass[] ="PASS akhe@eurosource.se\r\n";
ROM char retr[] ="RETR /pub/vscp/ttt.txt\r\n";

ROM char quit[] ="QUIT\r\n";
ROM char pasv[] ="PASV\r\n";
ROM char crlf[] ="\r\n";

////////////////////////////////////////////////////////////////////////////////////////////
// lftp_task
//

void lftp_task( void )
{	
	WORD ttt;
	BYTE c;
	BOOL bPreLine;
	BOOL bPostLine;
	
	// Nothing to do if we don't have a link
	if ( !MACIsLinked() ) return;
	
	// check if state machine is stuck somewhere and reset the it after a while if needed :
    if ( ( ftp_state != LFTP_STATE_NONE ) && 
	    	( TickGetDiff( TickGet(), lastActivity) > ( LFTP_TIMEOUT * TICK_SECOND ) ) ) {
		
		// Close ftp client socker if open
		//if ( TCPIsConnected( ftpsocket ) ) {
        	writeRomString2Socket( quit );
        	TCPDisconnect( ftpsocket );
        	ftpsocket = UNKNOWN_SOCKET;
        //}
        
        // Close data socket if open
        TCPDisconnect( datasocket );
        datasocket = UNKNOWN_SOCKET;
		
		// Check if we should try again or if its time
		// to pack it in
        cntBeforeFail++;
        if ( cntBeforeFail > LFTP_MAX_RETRIES ) {
	    	
	    	cntBeforeFail = 0;
	    	ftp_state = LFTP_STATE_NONE;		// Give up...
	    	bftpLoadWork = FALSE;	// Work is done - failed
	    	
	    }
 
        ftp_state = LFTP_STATE_NONE;
    }
		
		
		
	switch( ftp_state ) {
	
		// **
		// Start to work if its time to do so
		
		case LFTP_STATE_NONE:
		
			// Check timer and see if we should fetch
			// data from the server.
			lastActivity = TickGet();
			
			if ( bftpLoadWork ) {
				ftp_state = LFTP_STATE_ARP;	// Must get MAC address for server
				cntBeforeFail = 0;			// Init. failure counter
				DBG_OUT('A');
			}
			break;
			
			
		//**
		// Resolve the MAC address of the ftp server
			
		case LFTP_STATE_ARP:
			
			ftp_nodeinfo.IPAddr.v[ 0 ] = LFTP_SERVER_IP_v0;
			ftp_nodeinfo.IPAddr.v[ 1 ] = LFTP_SERVER_IP_v1;
			ftp_nodeinfo.IPAddr.v[ 2 ] = LFTP_SERVER_IP_v2;
			ftp_nodeinfo.IPAddr.v[ 3 ] = LFTP_SERVER_IP_v3;
			
			if ( ARPIsTxReady() ) {	
				DBG_OUT('B');
				ARPResolve( &ftp_nodeinfo.IPAddr );	// resolve IP adress
				ftp_state = LFTP_STATE_ARP_RESOLVE;
				lastActivity = TickGet();
			}
			break;	
		
			
		// **
		// Check if the ftp MAC address is resolved
			
		case LFTP_STATE_ARP_RESOLVE:	
			if ( ARPIsResolved( &ftp_nodeinfo.IPAddr, &ftp_nodeinfo.MACAddr ) ) {
				DBG_OUT('D');
				ftp_state = LFTP_STATE_CONNECT;
				lastActivity = TickGet();	
			}
			break;			
			
			
			
		// **
		// Connect to ftp server
			
		case LFTP_STATE_CONNECT:				
			
			// Try to connect				
			ftpsocket = TCPConnect( &ftp_nodeinfo, LFTP_PORT );
			if ( INVALID_SOCKET != ftpsocket ) {
				DBG_OUT('E');
				ftp_state = LFTP_STATE_CONNECT_WAIT;
				lastActivity = TickGet();
			}
			break;
		
			
		// **
		// Waiting for ftp connection
		
		case LFTP_STATE_CONNECT_WAIT:
		
			if ( TCPIsConnected( ftpsocket ) ) {
				DBG_OUT('F');
				ftp_state = LFTP_STATE_USER;
				lastActivity = TickGet();	
			}
			break;
				
							
							
		// Here we wait for server connection and send
		// USER command if OK	
		case LFTP_STATE_USER:

			// Fetch data if we are connected
			if ( TCPIsGetReady( ftpsocket ) ) {
					
				// get first digit
				while( TCPGet( ftpsocket, &c ) ) {
					if ( isdigit( c ) ) break;
				}
					
				// If connected with positive response "2xx - xxxxxxxx..."
				// we send username. If not we just timeout
				if ( '2' == c ) {
					DBG_OUT('G');
					writeRomString2Socket( user );
					ftp_state = LFTP_STATE_PASS;
					lastActivity = TickGet();
				}
					
				TCPDiscard( ftpsocket );
				 	
			}
			break;	
			
			
			
			// **
			// Here we wait for response from USER command
			// and send PASS command if OK
			
			case LFTP_STATE_PASS:
				
			// Fetch data if we are connected
			if ( TCPIsGetReady( ftpsocket ) ) {
				
				DBG_OUT('$');
					
				// get first digit
				while( TCPGet( ftpsocket, &c ) ) {
					DBG_OUT(c);
					if ( isdigit( c ) ) break;
				}
					
				// If connected with positive response "3xx - xxxxxxxx..."
				// we send username. If not we just timeout
				if ( ('3' == c ) || ('2' == c ) ) {
					DBG_OUT('H');
					writeRomString2Socket( pass );
					ftp_state = LFTP_STATE_PASV;	
					lastActivity = TickGet();
				}
					
				TCPDiscard( ftpsocket );
						
			}
			break;
			
			
			
			// **
			// Here we wait for response of PASS command
			// and send PASV command if positive and also
			// creates the data socket
			
			case LFTP_STATE_PASV:
		 
			// Fetch data if we are connected
			if ( TCPIsGetReady( ftpsocket ) ) {
				
				DBG_OUT('!');					
				
				// get first digit
				while( TCPGet( ftpsocket, &c ) ) {
					DBG_OUT(c);
					if ( isdigit( c ) ) break;
				}
					
				// If connected with positive response "2xx - xxxxxxxx..."
				// we send username. If not we just timeout
				if ( '2' == c ) {
					DBG_OUT('I');	
						
					writeRomString2Socket( pasv );
					ftp_state = LFTP_STATE_RETR;
					lastActivity = TickGet();
							
				}
					
				TCPDiscard( ftpsocket );	
				 	
			}
			break;
			
			
			// **
			// Here we wait for the result of PASV command
			// and parse its data
			// if OK we send RETR and go on to the next state
			
			case LFTP_STATE_RETR:
	 				
				// Fetch data if we are connected
				if ( TCPIsGetReady( ftpsocket ) ) {
					
					TCPGet( ftpsocket, &c );
					
					if ( '2' == c ) {
						
						DBG_OUT('J');
						
						// Get pasv parameters
						getPasvParams();
						
						// retrive file
						writeRomString2Socket( retr );
						
						ttt = portdata;
						while ( ttt ) {
							DBG_OUT('0' + (ttt % 10) );
							ttt = ttt / 10;
						} 
						
						ftp_state = LFTP_STATE_DATA_CONNECT;		
					}
					
					TCPDiscard( ftpsocket );					
				}
			break;
		
		
			// **
			// Connect to the data socket
		
			case LFTP_STATE_DATA_CONNECT:
			
				// Try to connect				
				datasocket = TCPConnect( &ftp_nodeinfo, portdata );
				if ( INVALID_SOCKET != datasocket ) {
					DBG_OUT('K');
					ftp_state = LFTP_STATE_WAIT_DATA_CONNECT; 
					lastActivity = TickGet();
				}
			break;
			
			
			// **
			// Wait for the data connection to establish
			
			case LFTP_STATE_WAIT_DATA_CONNECT:
			
				if ( TCPIsConnected( datasocket ) ) {
					DBG_OUT('L');
					//writeRomString2Socket( lftpDataSocket, crlf );
					ftp_state = LFTP_STATE_FETCH_DATA;
					lastActivity = TickGet();
				}
				
				// Check for reply on ftp socket FIX!!!!
				if ( TCPIsGetReady( ftpsocket ) ) {	
					DBG_OUT('?');
					while( TCPGet( ftpsocket, &c ) ) {
						DBG_OUT( c );
					}
					TCPDiscard( ftpsocket );
				}
			break;
		
			
			
			// **
			// Fetch the data and send it out on the
			// serial i/f
			
			case LFTP_STATE_FETCH_DATA:
 
				// Fetch data if we are connected
				if ( TCPIsGetReady( datasocket ) ) {	
					DBG_OUT('M');
					
					// Framework start
					serPutByte( 0x00  );
					serPutByte( 0xff  );
					serPutByte( 0xff  );
					serPutByte( 0x01  );
					serPutByte( 0x01  );
					serPutByte( 0x01  );
					
					bPreLine = FALSE;
					bPostLine = FALSE;
					
					// get data
					while( TCPGet( datasocket, &c ) ) {
						
						if ( 0x0d == c ) {
							// We skip CR
						}
						else if ( 0x0a == c ) {
							// Send end line stuff
							serPutByte( 0xff );
							bPreLine = FALSE;
							bPostLine = TRUE;
						}
						else {
							bPostLine = FALSE;	// no end line codes sent
							if ( !bPreLine ) {
								// Send preline stuff
								bPreLine = TRUE;
								serPutByte( 0x01 );
								serPutByte( 0x03 );
								serPutByte( 0xef );
								serPutByte( 0xb0 );
							}
							serPutByte( c );
						}
					}
					
					// If we end with a row without LF we must send
					// Line end stuff
					if ( !bPostLine ) {
						serPutByte( 0xff );
					}
					
					// Framework end
					serPutByte( 0xff  );
					serPutByte( 0x00  );
					
					ftp_state = LFTP_STATE_END;
					TCPDiscard( datasocket );	
				}
				
				
				// Check for data on ftp socket
				if ( TCPIsGetReady( ftpsocket ) ) {	
					
					while( TCPGet( ftpsocket, &c ) ) {
						DBG_OUT( c );
					}
					TCPDiscard( ftpsocket );
				}
				
			break;
				
				
				
			// **
			// We are done for this time
				
			case LFTP_STATE_END:
				DBG_OUT('*');
				TCPDisconnect( ftpsocket );
				TCPDisconnect( datasocket );
				bftpLoadWork = FALSE;	// Work is done
				ftp_state = LFTP_STATE_NONE;
			break;
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////
// writeRomString2Socket
//

void writeRomString2Socket( ROM char* p ) 
{
	BYTE c;

   	while( ( c = *p++ ) ) {
		TCPPut( ftpsocket, c );
		DBG_OUT( c );
   	}
   	
   	TCPFlush( ftpsocket );
}

////////////////////////////////////////////////////////////////////////////////////////////
// writeSocket
//

void writeString2Socket( char* p ) 
{
   BYTE c;

   while( ( c = *p++ ) ) {
      TCPPut( ftpsocket, c );
   }
   
   TCPFlush( ftpsocket );
}


////////////////////////////////////////////////////////////////////////////////////////////
// writeDecimal
//

void writeDecimal( BYTE b ) 
{
	char i;
	BYTE pos, val, c[3];
	pos = 0;
	val = b;
	
	while( val != 0 ) {
		c[ pos++ ] = val % 10;
		val = val / 10;	
	}
	
	for (i=pos-1; i>=0; i-- ) {
		TCPPut( ftpsocket, '0' + c[ i ] );	
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////
// getPasvParams
//

void getPasvParams( void )
{
	BYTE val;			// value
	BYTE vsave;
	char buf[4];		// item buffer ( one decimal number)
	BYTE pos = 0; 		// pos in item buffer
	BYTE nCommas = 0;	// Counter for commas
	BYTE c;

/*	
	// First read past result code
	// that is find first space	
	while( TCPGet( lftpsocket, &c ) ) {
		if ( ' ' == c ) {
			break;
		}
	}
	
	// Look for first digit
	while( TCPGet( lftpsocket, &c ) ) {
		if ( isdigit( c ) ) {
			break;
		}
	}		
	
	buf[ pos++ ] = c;
*/
	// Look for third comma
	while( TCPGet( ftpsocket, &c ) ) {
		DBG_OUT( c );
		if ( ',' == c ) {
			nCommas++;
			if ( nCommas >= 4 ) break;
		}
	}		
	
	DBG_OUT( '[' );
	while( TCPGet( ftpsocket, &c ) ) {
		DBG_OUT( c );
		if ( isdigit( c ) ) {
			buf[ pos++ ] = c;
		}
		else if ( 0x2c == c  ) {
			
			buf[ pos ] = 0;
			vsave = atoi( buf );	// save until last 	
			pos = 0;
	
		}
		else {
			// end char
			buf[ pos ] = 0;
			val = atoi( buf );
			portdata = ( ( ( unsigned int)vsave ) * 256 ) + val;
			DBG_OUT( ']' );
			break;
		}
	}
}

