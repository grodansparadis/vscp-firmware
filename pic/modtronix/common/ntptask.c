// File: ntptask.c
// 
// 

#include <projdefs.h>

#include <stdlib.h>
#include <string.h>


#if defined(STACK_USE_NTP_SERVER)


/*
 * These headers must be included for required defs.
 */

#include <net/stacktsk.h>
#include <net/tick.h>
#include <net/mac.h>

#if defined(STACK_USE_DHCP)
#include <net/dhcp.h>
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include <net/http.h>
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include <ftp.h>
#endif

#if defined(MPFS_USE_EEPROM)
#include <xeeprom.h>
#endif


// For debug only.
#include <net/ARPTsk.h>
#include <net/arp.h>
#include <net/tcp.h>
#include <net/icmp.h>
#include <net/delay.h>
#include <debug.h>

#include "ntptask.h"



static short ntp_state = NTP_STATE_NONE;

// NTP socket
NODE_INFO ntp_nodeinfo;

// Time
extern unsigned char hour;
extern unsigned char minute;
extern unsigned char second;

static unsigned long timetick;		// Holder for pre. second counter

// Socket for ntp server connection
static TCP_SOCKET ntpsocket = 0;

// Server to use
static unsigned char nServer = 0;

// Count before failure
static unsigned char cntBeforeFail;

// Tick for last activity
static unsigned long lastActivity = 0;

static ROM char DebugMsgTimeout[] = "\rTimeout"; 
static ROM char DebugMsgCloseSocket[] = "\rClose Socket";
static ROM char DebugMsgArpInit[] = "\rArpInit"; 
static ROM char DebugMsgArpInit2[] = "\rArpInit2";
static ROM char DebugMsgArpTxReady[] = "\rArpTxReady"; 
static ROM char DebugMsgArpResolved[] = "\rArpResolved"; 
static ROM char DebugMsgTcpConnect[] = "\rTCPConnect"; 
static ROM char DebugMsgTcpConnectSuccess[] = "\rTCPConnect Success"; 
static ROM char DebugMsgTcpReceive[] = "\rTCP-Receive";

////////////////////////////////////////////////////////////////////////////////////////////
// ntp_task
//

void ntp_task( void )
{		
	// check if state machine is stuck somewhere and reset the it after a while if needed :
    if ( ( ntp_state != NTP_STATE_NONE ) && 
	    	( TickGetDiff( TickGet(), lastActivity ) > ( NTP_TIMEOUT * TICK_SECOND ) ) ) {
		
		//if ( TCPIsConnected( ntpsocket ) ) {
           TCPDisconnect( ntpsocket );
        //}
        		
		// Check if we should try again or if its time
		// to pack it in
        cntBeforeFail++;
        if ( cntBeforeFail > NTP_MAX_RETRIES ) {
	    	
	    	cntBeforeFail = 0;
	    	ntp_state = NTP_STATE_NONE;		// Give up...
	    	
	    	// We try one of the other servers next time
	    	nServer++;	
			if ( nServer > NTP_N_OF_SERVERS - 1  ) {
				nServer = 0;
			}
			debugPutByte( '0' + nServer );
	    }
 
        ntp_state = NTP_STATE_ARP;
    }
		
	switch( ntp_state ) {
	
		case NTP_STATE_NONE:
		
			// Check timer and see if we should fetch
			// correct time from the timeserver.
			
			// We update the internal time from a server
			// every full hour.
			if ( 0 == ( minute + second ) ) {
				ntp_state = NTP_STATE_ARP;	// Must get MAC addres for server
				cntBeforeFail = 0;			// Init. failure counter
			}
			break;
			
			
			
		case NTP_STATE_ARP:
			
			if ( 0 == nServer ) {
				ntp_nodeinfo.IPAddr.v[ 0 ] = NTP_SERVER1_IP_v0;	
				ntp_nodeinfo.IPAddr.v[ 1 ] = NTP_SERVER1_IP_v1;
				ntp_nodeinfo.IPAddr.v[ 2 ] = NTP_SERVER1_IP_v2;
				ntp_nodeinfo.IPAddr.v[ 3 ] = NTP_SERVER1_IP_v3;
			}
			else if ( 1 == nServer ) {
				ntp_nodeinfo.IPAddr.v[ 0 ] = NTP_SERVER2_IP_v0;	
				ntp_nodeinfo.IPAddr.v[ 1 ] = NTP_SERVER2_IP_v1;
				ntp_nodeinfo.IPAddr.v[ 2 ] = NTP_SERVER2_IP_v2;
				ntp_nodeinfo.IPAddr.v[ 3 ] = NTP_SERVER2_IP_v3;
			}
			else if ( 2 == nServer ) {
				ntp_nodeinfo.IPAddr.v[ 0 ] = NTP_SERVER3_IP_v0;	
				ntp_nodeinfo.IPAddr.v[ 1 ] = NTP_SERVER3_IP_v1;
				ntp_nodeinfo.IPAddr.v[ 2 ] = NTP_SERVER3_IP_v2;
				ntp_nodeinfo.IPAddr.v[ 3 ] = NTP_SERVER3_IP_v3;
			}
		
			ntp_nodeinfo.MACAddr.v[ 0 ] = 0;
			ntp_nodeinfo.MACAddr.v[ 1 ] = 0;
			ntp_nodeinfo.MACAddr.v[ 2 ] = 0;
			ntp_nodeinfo.MACAddr.v[ 3 ] = 0;
			ntp_nodeinfo.MACAddr.v[ 4 ] = 0;
			ntp_nodeinfo.MACAddr.v[ 5 ] = 0;
			
			if ( ARPIsTxReady() ) {	
				ARPResolve( &ntp_nodeinfo.IPAddr ) ;	// resolve IP adress
				ntp_state = NTP_STATE_ARP_RESOLVE;
				lastActivity = TickGet();
			}
			break;	
		
		case NTP_STATE_ARP2:
				
			if ( ARPIsTxReady() ) {	
				debugPutByte('X');
				ARPResolve( &ntp_nodeinfo.IPAddr ) ;	// resolve IP adress
				ntp_state = NTP_STATE_ARP_RESOLVE;
				lastActivity = TickGet();
			}
			break;	
			
		case NTP_STATE_ARP_RESOLVE:	
		
			if ( ARPIsResolved( &ntp_nodeinfo.IPAddr, &ntp_nodeinfo.MACAddr ) ) {
				ntp_state = NTP_STATE_CONNECT;
				lastActivity = TickGet();	
			}
			break;
			
		case NTP_STATE_CONNECT:	
				
			// Try to connect				
			ntpsocket = TCPConnect( &ntp_nodeinfo, NTP_PORT );
			if ( INVALID_SOCKET != ntpsocket ) {
				ntp_state = NTP_STATE_CONNECT_WAIT;	// yes
				lastActivity = TickGet();
			}
			break;
		
		// Waiting for connection
		case NTP_STATE_CONNECT_WAIT:

			if ( TCPIsConnected( ntpsocket ) ) {
				ntp_state = NTP_STATE_RECEIVE_PHASE;
				lastActivity = TickGet();	
			}
			break;
							
							
			
		case NTP_STATE_RECEIVE_PHASE:
		
			// Fetch data if we are connected
			if ( TCPIsGetReady( ntpsocket ) ) {
				{
					char i;
					time_t time;
					struct tm tt;
					unsigned char c;
					char *p = (char *)&time + 3;
					
					for ( i=0; i<4; i++ ) {
						TCPGet( ntpsocket, &c );	
						*p-- = c;
					}
					
					// Free buffer
					TCPDiscard( ntpsocket );
					
					// Get time and update clock
					gmtime( &time, &tt ); 
					second = tt.tm_sec;
					minute = tt.tm_min;
					hour = tt.tm_hour;
					
					{
						char c, val;
				
						// Hour
						c = hour % 10;
						val = hour / 10;
						debugPutByte( '0' + ( val % 10 ) );
						debugPutByte( '0' + c );
				
						debugPutByte( ':' );
				
						// minute
						c = minute % 10;
						val = minute / 10;
						debugPutByte( '0' + ( val % 10 ) );
						debugPutByte( '0' + c );
				
						debugPutByte( ':' );
				
						// second
						c = second % 10;
						val = second / 10;
						debugPutByte( '0' + ( val % 10 ) );
						debugPutByte( '0' + c );
				
						debugPutByte( 0x0d );
						debugPutByte( 0x0a );
					}
					
					// We are done
					TCPDisconnect( ntpsocket );
					ntp_state = NTP_STATE_NONE;
					
				}
			}
			break;	
		
	}
	
	
	
	// Do timekeeping
	if ( TickGetDiff( TickGet(), timetick ) >= TICK_SECOND ) {
		
		timetick = TickGet();
		
		second++;
		debugPutByte( '.' );
		if ( second > 60 ) {
			
			minute++;
			second = 0;
			
			{
				char c, val;
				
				// Hour
				c = hour % 10;
				val = hour / 10;
				
				// minute
				c = minute % 10;
				val = minute / 10;
				
				// second
				c = second % 10;
				val = second / 10;

			}
		}
		
		if ( minute > 60 ) {
			hour++;
			minute = 0;
		}
		
		if ( hour >= 24  ) {
			hour = 0;
		}
		
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////
//							    Time conversion routines
////////////////////////////////////////////////////////////////////////////////////////////

#define	YEAR0					1900
#define	EPOCH_YR				1970
#define	SECS_DAY				(24L * 60L * 60L)
#define	LEAPYEAR(year)	        (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	        (LEAPYEAR(year) ? 366 : 365)
#define	FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define	FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

#define TIME_MAX                2147483647L

const int _ytab[2][12] = 
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

////////////////////////////////////////////////////////////////////////////////////////////
// gmtime
//

struct tm * gmtime( const time_t *timer, struct tm *tmbuf )
{
 	time_t time = *timer;
  	unsigned long dayclock, dayno;
  	int year = EPOCH_YR;

  	dayclock = (unsigned long) time % SECS_DAY;
  	dayno = (unsigned long) time / SECS_DAY;

  	tmbuf->tm_sec = dayclock % 60;
  	tmbuf->tm_min = (dayclock % 3600) / 60;
  	tmbuf->tm_hour = dayclock / 3600;
  	tmbuf->tm_wday = (dayno + 4) % 7; 	// Day 0 was a thursday
  	while ( dayno >= ( unsigned long ) YEARSIZE( year ) ) {
    	dayno -= YEARSIZE(year);
    	year++;
  	}
  	tmbuf->tm_year = year - YEAR0;
  	tmbuf->tm_yday = dayno;
  	tmbuf->tm_mon = 0;
  	while ( dayno >= (unsigned long) _ytab[ LEAPYEAR( year ) ][ tmbuf->tm_mon ] ) {
    	dayno -= _ytab[ LEAPYEAR(year) ][ tmbuf->tm_mon ];
    	tmbuf->tm_mon++;
  	}
  	tmbuf->tm_mday = dayno + 1;
  	tmbuf->tm_isdst = 0;

  	return tmbuf;
}

#endif // ntp task
