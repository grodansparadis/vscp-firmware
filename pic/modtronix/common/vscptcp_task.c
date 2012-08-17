// File: vscptcp_task.c
// 
// 

#include <projdefs.h>
#include <vscp_projdefs.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/*
 * These headers must be included for required defs.
 */

#include "net/stacktsk.h"
#include "net/tick.h"
#include "net/mac.h"

#if defined(STACK_USE_DHCP)
#include "net/dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "net/http.h"
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "net/ftp.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "net/xeeprom.h"
#endif


// For debug only.
#include "net/ARPTsk.h"
#include "net/arp.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/icmp.h"
#include "net/delay.h"
#include "debug.h"

#include "tcphelpers.h"
#include <vscp_2.h>
#include "vscptcp_task.h"

// Extern from vscp_task.c
extern vscpMsg inMsg;
extern vscpMsg outMsg;

// Externs from vscp_2.c
extern uint8_t vscp_function_flags;



#if defined(VSCP_USE_TCP )

static uint8_t vscp_state = VSCP_STATE_NONE;

// VSCP socket
NODE_INFO vscp_nodeinfo;

// Socket for ntp server connection
static TCP_SOCKET vscpsocket = 0;

// Server to use
static uint8_t nServer = 0;

// Connection Count before failure
static uint8_t cntBeforeFail;

// Tick for last activity
static TICK32 lastActivity = 0;

// Retry counter for transmissions
static uint8_t cntEventTxTries;

// Counter for read timeouts
static uint8_t cntReadTimeout;

static ROM char vscp_tcpcmd_user[] = "USER ";
static ROM char vscp_tcpcmd_pass[] = "PASS ";
static ROM char vscp_tcpcmd_fast[] = "BIN1\r\n";
static ROM char vscp_tcpcmd_quit[] = "QUIT\r\n";
static ROM char crlf[] ="\r\n";

///////////////////////////////////////////////////////////////////////////////
// vscp_sendTCPEvent
//

int8_t vscp_sendTCPEvent( PVSCPMSG pmsg )
{
	uint8_t i;
	
	for ( i=0; i<16; i++ ) {
		UDPPut( vscp_getGUID( i ) );
	}
		
	vscp_function_flags |= VSCP_FUNCTION_TXOBJ_IN_USE;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getTCPEvent
//

int8_t vscp_getTCPEvent( PVSCPMSG pmsg )
{
	if ( vscp_function_flags & VSCP_FUNCTION_RXOBJ_IN_USE ) return TRUE;
	return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////
// vscp_tcp_task
//

void vscp_tcp_task( void )
{		
	uint8_t i,c;
	uint8_t errorcode;
	
	// Skip if TCP is not enabled
	if ( !( vscp_function_flags & VSCP_FUNCTION_USE_TCP ) )  {
		return;
	}
	
	// check if state machine is stuck somewhere and reset the it after a while if needed :
    if ( ( vscp_state != VSCP_STATE_NONE ) && 
            ( vscp_state != VSCP_STATE_REST ) &&
	    	( TickGetDiff( TickGet(), lastActivity ) > 
    	        ( VSCP_TIMEOUT * TICK_SECOND ) ) ) {

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByte( 'X' );
#endif		
        
        // exit fast mode in case its needed
        //FAST_WriteCommandEvent( vscpsocket, VSCP_FAST_COMMAND_CLOSE );
        
        // Quit TCP mode
		//writeRomString2Socket( vscpsocket, vscp_tcpcmd_quit, TRUE );
						
        TCPDisconnect( vscpsocket );
       		
		// Check if we should try again or if its time
		// to pack it in
        cntBeforeFail++;
        if ( cntBeforeFail > VSCP_MAX_RETRIES ) {
	    	
	    	cntBeforeFail = 0;
	    	vscp_state = VSCP_STATE_NONE;		// Give up...
	    	
	    }
 
        vscp_state = VSCP_STATE_ARP;
    }
		
	switch( vscp_state ) {
	
		// ================================================================
		//                          Start a connect
		// ================================================================	
		case VSCP_STATE_NONE:
		
			cntReadTimeout = 0;             // No read retries yet
			
			vscp_state = VSCP_STATE_ARP;    // Must get MAC addres for server
			cntBeforeFail = 0;              // Init. failure counter
			break;
		
		// ================================================================
		//                      Initiate ARP request
		// ================================================================	
		case VSCP_STATE_ARP:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte( '1' );
#endif			
			vscp_nodeinfo.IPAddr.v[ 0 ] = appcfgGetc( APPCFG_VSCP_SERVER_IP0 );	
			vscp_nodeinfo.IPAddr.v[ 1 ] = appcfgGetc( APPCFG_VSCP_SERVER_IP1 );
			vscp_nodeinfo.IPAddr.v[ 2 ] = appcfgGetc( APPCFG_VSCP_SERVER_IP2 );
			vscp_nodeinfo.IPAddr.v[ 3 ] = appcfgGetc( APPCFG_VSCP_SERVER_IP3 );
		
			vscp_nodeinfo.MACAddr.v[ 0 ] = 0;
			vscp_nodeinfo.MACAddr.v[ 1 ] = 0;
			vscp_nodeinfo.MACAddr.v[ 2 ] = 0;
			vscp_nodeinfo.MACAddr.v[ 3 ] = 0;
			vscp_nodeinfo.MACAddr.v[ 4 ] = 0;
			vscp_nodeinfo.MACAddr.v[ 5 ] = 0;
			
			if ( ARPIsTxReady() ) {	
				ARPResolve( &vscp_nodeinfo.IPAddr ) ;	// resolve IP adress
				vscp_state = VSCP_STATE_ARP_RESOLVE;
				lastActivity = TickGet();
			}
			break;	
		
		// ================================================================
		// ================================================================
		case VSCP_STATE_ARP2:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte( '2' );	
#endif			
			if ( ARPIsTxReady() ) {	
				ARPResolve( &vscp_nodeinfo.IPAddr ) ;	// resolve IP adress
				vscp_state = VSCP_STATE_ARP_RESOLVE;
				lastActivity = TickGet();
			}
			break;	
		
		// ================================================================
		//                    Waiting for ARP resolv
		// ================================================================	
		case VSCP_STATE_ARP_RESOLVE:	
		
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
			debugPutByte( '3' );
#endif			
		
			if ( ARPIsResolved( &vscp_nodeinfo.IPAddr, &vscp_nodeinfo.MACAddr ) ) {
				vscp_state = VSCP_STATE_CONNECT;
				lastActivity = TickGet();	
			}
			break;
			
		case VSCP_STATE_CONNECT:	

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
			debugPutByte( '4' );
#endif			
				
			// Try to connect				
			vscpsocket = TCPConnect( &vscp_nodeinfo, VSCP_LEVEL2_TCP_PORT );
			if ( INVALID_SOCKET != vscpsocket ) {
				vscp_state = VSCP_STATE_CONNECT_WAIT;	// yes
				lastActivity = TickGet();
			}
			break;
		
		// ================================================================
		//                          Waiting for connection
		// ================================================================
		case VSCP_STATE_CONNECT_WAIT:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)
			debugPutByte( '5' );
#endif			
			
			if ( TCPIsConnected( vscpsocket ) ) {
				vscp_state = VSCP_STATE_USERNAME;
				lastActivity = TickGet();
				cntReadTimeout = 0; // Used for multiline respons	
			}
			break;									
		
		// ================================================================
		//                 Verify connect and send username
		// ================================================================	
		case VSCP_STATE_USERNAME:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
			debugPutByte( '6' );
#endif			
			
			// Fetch data if we are connected
			if ( TCPIsGetReady( vscpsocket ) ) {
									
				// Check for positive response
				if ( checkResponse( vscpsocket ) ) {
						
					// yes we got a positive response
						
					// Free buffer
					TCPDiscard( vscpsocket );
						
					// Send username
					writeRomString2Socket( vscpsocket, vscp_tcpcmd_user, FALSE );	
					
					i = 0;
					while ( ( c = appcfgGetc( APPCFG_USERNAME0 + i ) ) ) {
						TCPPut( vscpsocket, c );
						i++;
					}
					
					writeRomString2Socket( vscpsocket, crlf, TRUE );
						
					vscp_state = VSCP_STATE_USERNAME_RESPONSE;
						
				}
				else {
    				
    				if ( 5 > cntReadTimeout ) {	    // Max rows
					    // Failed
						
					    // Free buffer
					    TCPDiscard( vscpsocket );
									
					    // We are done
					    TCPDisconnect( vscpsocket );
					    vscp_state = VSCP_STATE_REST;
			        } 
			        else {
    			        lastActivity = TickGet();
    				    cntReadTimeout++;
    			    }    
				}	
			}
			break;	
			
		// ================================================================
		// ================================================================	
		case VSCP_STATE_USERNAME_RESPONSE:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
			debugPutByte( '7' );
#endif			
			
			// Fetch data if we are connected
			if ( TCPIsGetReady( vscpsocket ) ) {
						
				// Check for positive response
				if ( checkResponse( vscpsocket ) ) {
						
					// yes we got a positive response
						
					// Free buffer
					TCPDiscard( vscpsocket );
						
					// Send password
					writeRomString2Socket( vscpsocket, vscp_tcpcmd_pass, FALSE );	
					
					i = 0;
					while ( ( c = appcfgGetc( APPCFG_PASSWORD0 + i ) ) ) {
						TCPPut( vscpsocket, c );
						i++;
					}
					
					writeRomString2Socket( vscpsocket, crlf, TRUE );
						
					vscp_state = VSCP_STATE_PASSWORD_RESPONSE;
						
				}
				else {	
					// Failed
						
					// Free buffer
					TCPDiscard( vscpsocket );
											
					// We are done
					TCPDisconnect( vscpsocket );
					vscp_state = VSCP_STATE_REST;
				}
			}
			break;
			
		// ================================================================
		// ================================================================
		case VSCP_STATE_PASSWORD_RESPONSE:
	
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte( '8' );
#endif				
			
			// Fetch data if we are connected
			if ( TCPIsGetReady( vscpsocket ) ) {
 	
				// Check for positive response
				if ( checkResponse( vscpsocket ) ) {
						
					// yes we got a positive response
												
					// Send FAST command
					writeRomString2Socket( vscpsocket, vscp_tcpcmd_fast, TRUE );	
							
					vscp_state = VSCP_STATE_FAST_RESPONSE;
						
				}
				else {	
					// Failed
								
					// We are done
					TCPDisconnect( vscpsocket );
					vscp_state = VSCP_STATE_REST;

				}
				
				// Free buffer
				TCPDiscard( vscpsocket );
		}
		break;
			
		// ================================================================
		// ================================================================
		case VSCP_STATE_FAST_RESPONSE:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte( '9' );
#endif				
							
			// Fetch data if we are connected
			if ( TCPIsGetReady( vscpsocket ) ) {
	
				// Check for positive response
				if ( checkResponse( vscpsocket ) ) {
						
					// yes we got a positive response and
					// are in binary mode																
					vscp_state = VSCP_STATE_WORK_IDLE;
					cntEventTxTries = 0;
										
				}
				else {	
					// Failed	
										
					// We are done
					TCPDisconnect( vscpsocket );
					vscp_state = VSCP_STATE_REST;

				}
				
				// Free buffer
				TCPDiscard( vscpsocket );
		}
		break;
			
		// ================================================================
		// The WORK_IDE state is where we are when no events need to be sent or
		// there is nothing to receive.
		// ================================================================
		
		case VSCP_STATE_WORK_IDLE:

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)				
			debugPutByte( 'I' );
#endif				
				
			// Check to see if there is an event to send
			if ( vscp_function_flags & VSCP_FUNCTION_TXOBJ_IN_USE ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)					
				debugPutByte( 'S' );
#endif					
				// Yes we should send an event
				if ( TCPIsPutReady( vscpsocket ) ) {
					lastActivity = TickGet();
					FAST_WriteEvent( vscpsocket, &outMsg );
					vscp_state = VSCP_STATE_WORK_TX_WAIT_RESPONSE;
				}
				else if ( TickGetDiff( TickGet(), lastActivity ) > 
    				            ( VSCP_RXTX_TIMEOUT * TICK_SECOND ) ) {
					vscp_state = VSCP_STATE_NONE;
					TCPDisconnect( vscpsocket );
				}
			}
			else if ( !( vscp_function_flags & VSCP_FUNCTION_RXOBJ_IN_USE ) ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)					
				debugPutByte( 'C' );
#endif					
				if ( TCPIsPutReady( vscpsocket ) ) {
					lastActivity = TickGet();
					FAST_WriteCommandEvent( vscpsocket, VSCP_FAST_COMMAND_READ );	
					vscp_state = VSCP_STATE_WORK_RX_WAIT_RESPONSE;				
				}
				else if ( TickGetDiff( TickGet(), lastActivity ) > ( VSCP_RXTX_TIMEOUT * TICK_SECOND ) ) {
					vscp_state = VSCP_STATE_NONE;
					TCPDisconnect( vscpsocket );
				}
			}
			else {
				vscp_state = VSCP_STATE_REST;
			}
			break;
			
			// ================================================================	
			// An outgoing FAST frame has been sent to the server. Wait for
			// response. We let the standard timeout handler take us out if no response.	
			// ================================================================
			case VSCP_STATE_WORK_TX_WAIT_RESPONSE:
				
				// Fetch data if we are connected
				if ( TCPIsGetReady( vscpsocket ) ) {
					
					lastActivity = TickGet(); // Register activity
					
					FAST_CheckResponse( vscpsocket, &errorcode );
					
					if ( VSCP_FAST_ERROR_OK == errorcode ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)						
						debugPutByte( 'T' );
#endif						
						// Frame sent OK
						vscp_function_flags &= ~VSCP_FUNCTION_TXOBJ_IN_USE;
						vscp_state = VSCP_STATE_WORK_IDLE; 
						cntEventTxTries = 0;	
					}
					else {
						// Problem sending this frame.
						vscp_state = VSCP_STATE_WORK_IDLE; // try agin
						cntEventTxTries++;
					}
					
					// Free buffer
					TCPDiscard( vscpsocket );
				}
				else if ( TickGetDiff( TickGet(), lastActivity ) > ( VSCP_RXTX_TIMEOUT * TICK_SECOND ) ) {
					cntEventTxTries++;
					// We let the connection timeout if we have not
					// tried to send this event three times yet.
					if ( cntEventTxTries < 3 ) {
						vscp_state = VSCP_STATE_WORK_IDLE; // try agin
					}
					
				}
				break;	
			
			
			// ================================================================
			// A request for data has been sent we wait for either a packet
			// or a negative error frame.
			// ================================================================
			case VSCP_STATE_WORK_RX_WAIT_RESPONSE:
						
				// Fetch data if we are connected
				if ( TCPIsGetReady( vscpsocket ) ) {
					
					uint8_t type;
					lastActivity = TickGet(); // Note activity
					
					if ( FAST_ReadEvent( vscpsocket, &inMsg, &type  ) ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)						
						debugPutByte( '+' );
#endif					
						// Reset Read timout counter
						cntReadTimeout = 0;
							
						// Set flag to indicate that we have received an event
						vscp_function_flags |= VSCP_FUNCTION_RXOBJ_IN_USE;
						vscp_state = VSCP_STATE_WORK_IDLE;
					}
					else {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)						
						debugPutByte( '-' );
#endif						
						// Probably no message available
						
						// If we have TX traffic we handle that
						if ( vscp_function_flags & VSCP_FUNCTION_TXOBJ_IN_USE ) {
							vscp_state = VSCP_STATE_WORK_IDLE;
						}
						else {
							// We rest for a while
							vscp_state = VSCP_STATE_REST;
						}
												
						TCPDiscard( vscpsocket );  // Free buffer
					}	
				}
				else if ( TickGetDiff( TickGet(), lastActivity ) > ( VSCP_RXTX_TIMEOUT * TICK_SECOND ) ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)						
						debugPutByte( '*' );
#endif					
					vscp_state = VSCP_STATE_WORK_IDLE; // try agin
					
					cntReadTimeout++;
					if ( cntReadTimeout > 128 ) {
						// Start again
						vscp_state = VSCP_STATE_NONE;
						TCPDisconnect( vscpsocket );
					}
					
				}
				break;
				
			// ================================================================
			// ================================================================	
			case VSCP_STATE_REST:
				if ( TickGetDiff( TickGet(), lastActivity ) > 20 ) {
					vscp_state = VSCP_STATE_WORK_IDLE;
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)					
					debugPutByte( 'R' );
#endif					
				}
				break;	
	}	
}


////////////////////////////////////////////////////////////////////////////////////////////
// Check for '+OK' response
//

uint8_t checkResponse( TCP_SOCKET sock )
{

    char buf[ 64 ];
    char *p = buf;
    
    while ( TCPGet( sock, (uint8_t *)p ) ) {
        p++;
        if ( ( p - buf ) > sizeof( buf ) ) break;
    }    
    
    if ( NULL != strstr( buf, "+OK" ) ) {
        return TRUE;
    }    
    
    return FALSE;

}



////////////////////////////////////////////////////////////////////////////////////////////
// FAST_WriteEvent
//

void FAST_WriteEvent( TCP_SOCKET sock, vscpMsg *pMsg )
{
	uint8_t i;
	
	// Write start of frame	
	TCPPut( sock, 0x55 );
	TCPPut( sock, 0xaa );
	
	// Frame Type
	TCPPut( sock, VSCP_FAST_FRAMETYPE_VSCP );
	
	// Write length
	TCPPut( sock, ( ( pMsg->sizeData + 21 ) >> 8 & 0xff ) ); 	// MSB
	TCPPut( sock, ( pMsg->sizeData + 21 ) & 0xff ); 			// LSB
		
	// Head
	TCPPut( sock,  pMsg->head );
	
	// Write class
	TCPPut( sock, ( pMsg->vscp_class >> 8 ) & 0xff  ); 			// MSB
	TCPPut( sock, pMsg->vscp_class & 0xff ); 					// LSB
	
	// Write type
	TCPPut( sock, ( pMsg->vscp_type >> 8 ) & 0xff  ); 			// MSB
	TCPPut( sock, pMsg->vscp_type & 0xff ); 					// LSB
	
	// Send GUID
	TCPPutArray( sock, pMsg->GUID, 16 );
	
	// Send Data
	TCPPutArray( sock, pMsg->data, pMsg->sizeData );
	
	// Send it
	TCPFlush( sock );
}


////////////////////////////////////////////////////////////////////////////////////////////
// FAST_WriteCommandEvent
//

void FAST_WriteCommandEvent( TCP_SOCKET sock, uint8_t command )
{
	uint8_t i;
	
	// Write start of frame	
	TCPPut( sock, 0x55 );
	TCPPut( sock, 0xaa );
	
	// Write frame tyoe
	TCPPut( sock, VSCP_FAST_FRAMETYPE_COMMAND );
	
	// Write length
	TCPPut( sock, 0 ); 	// MSB
	TCPPut( sock, 1 ); 	// LSB
			
	// command
	TCPPut( sock, command );
	
	// Send it
	TCPFlush( sock );
 
}


////////////////////////////////////////////////////////////////////////////////////////////
// FAST_ReadEvent
//

uint8_t FAST_ReadEvent( TCP_SOCKET sock, vscpMsg *pMsg, uint8_t *ptype  )
{
	uint8_t i;
	uint8_t c[6];	// Databuffer

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( 0x0d );
	debugPutByte( 0x0a );
	debugPutByte( 'F' );
#endif	

	// * * * Read start of frame
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
			return FALSE;
		}
	}
	
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByteHex( c[0] );
	debugPutByte( ' ' );
	debugPutByteHex( c[1] );
	debugPutByte( ' ' );
#endif
 
	
	if ( !(( 0x55 == c[0] ) && ( 0xaa == c[1] ) ) ) {
		TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByte('>');		
#endif		
		return FALSE;				
	}	

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)
	debugPutByte( '#' );
#endif	
	
	
	
	// * * * Read Frame type
	
	
	
	if ( !TCPGet( sock, ptype ) ) {
		TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByte('>');
#endif		
		return FALSE;
	}
	
	
	
	// * * * Read frame length
	
	
	
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte('>');
#endif			
			return FALSE;
		}
	}
	
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( '%' );
	debugPutByteHex( c[0] );
	debugPutByte( ' ' );
	debugPutByteHex( c[1] );
	debugPutByte( ' ' );
#endif	
	
	pMsg->sizeData = ((uint16_t)c[0] << 8) + c[1];	
	
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( '?' );
	debugPutByteHex( *ptype );
#endif	
		
	// Must be VSCP frame
	if ( VSCP_FAST_FRAMETYPE_VSCP != *ptype ) {
		TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByte('>');
#endif		
		return FALSE;
	}
	
	pMsg->sizeData = pMsg->sizeData - 21; // Data size - GUID - class - type - head
	
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( '*' );
#endif	
	
	
	
	// * * * Read head
	
	
	
	if ( !TCPGet( sock, &pMsg->head ) ) {
		TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByte('>');
#endif		
		return FALSE;
	}
	
	
	
	// * * * Read VSCP class
	
	
	
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte('>');
#endif			
			return FALSE;
		}
	}
	pMsg->vscp_class = ((uint16_t)c[0] << 8) + c[1];




	// * * * Read VSCP type
	
	
	
	
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte('>');
#endif			
			return FALSE;
		}
	}
	pMsg->vscp_type = ((uint16_t)c[0] << 8) + c[1];

#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( 'L' );
#endif	
	
	
	
	// * * * Read VSCP GUID
	
	
	for ( i=0; i<16; i++ ) {
		if ( !TCPGet( sock, &pMsg->GUID[i] ) ) {
			TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)			
			debugPutByte('>');
#endif			
			return FALSE;
		}
	}
	
	
	
	
	// * * * Read VSCP data
	
	
	
	
	for ( i=0; i<pMsg->sizeData; i++ ) {
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)
		debugPutByte( '.' );
#endif
		if ( !TCPGet( sock, &pMsg->data[ i ] ) ) {
			TCPDiscard( vscpsocket );
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)
			debugPutByte('>');
#endif
			return FALSE;
		}
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)		
		debugPutByteHex( pMsg->data[ i ]  );
#endif		
	}		
		
	// Frame received
	TCPDiscard( vscpsocket );

	return TRUE;
}





////////////////////////////////////////////////////////////////////////////////////////////
// FAST_CheckResponse
//

uint8_t FAST_CheckResponse( TCP_SOCKET sock, uint8_t *perrorcode )
{
	uint8_t i;
	uint8_t c[6];	// Databuffer
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByte( '=' );
#endif	
	
	// * * * Read start of frame
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
			return FALSE;
		}
	}
#if (DEBUG_VSCP_TCP >= LOG_DEBUG)	
	debugPutByteHex( c[0] );
	debugPutByte( ' ' );
	debugPutByteHex( c[1] );
	debugPutByte( ' ' );
#endif 
	
	if ( !(( 0x55 == c[0] ) && ( 0xaa == c[1] )  ) ) {
		TCPDiscard( vscpsocket );
		return FALSE;				
	}	
	
		// * * * Read Frame type
	if ( !TCPGet( sock, &c[0] ) ) {
		TCPDiscard( vscpsocket );
		return FALSE;
	}
	
	// Must be error frame
	if ( VSCP_FAST_FRAMETYPE_ERROR != c[0] ) {
		TCPDiscard( vscpsocket );
		return FALSE;
	}

	// * * * Read frame length
	for ( i=0; i<2; i++ ) {
		if ( !TCPGet( sock, &c[i] ) ) {
			TCPDiscard( vscpsocket );
			return FALSE;
		}
	}
	
	// Length should be one bytes
	if ( !( 0 == c[0] && 1 == c[1] ) ) {
		TCPDiscard( vscpsocket );
		return FALSE;
	}
	
	// * * * Read error code
	if ( !TCPGet( sock, perrorcode ) ) {
		TCPDiscard( vscpsocket );
		return FALSE;
	}
		
	// Frame received
	TCPDiscard( vscpsocket );
	return TRUE;	
}


#endif
