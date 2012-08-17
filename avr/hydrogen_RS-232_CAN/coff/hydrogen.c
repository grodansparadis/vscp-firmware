// hydrogen.c
//
// Copyright (C) 2002 Ake Hedman, Eurosource, akhe@users.sourceforge.net
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
// parts from code written by Ake Hedman, CC Systems AB
// parts derived from code written by Arnaud Westenberg 
// arnaud@wanadoo.nl and his can drive and
// Version 0.6.1 T.Motylewski@bfad.de 13.03.2001

#include <stdlib.h>
#include <interrupt.h>
#include <pgmspace.h>
#include <string.h>
#include "can.h"
#include "hydrogen.h"
#include <iom161.h>
#include <io.h>
#include <sig-avr.h>
#include <interrupt.h>
#include <wdt.h>
#include "uart.h" 
#include "sja1000.h"


// Globals
char gbuffer[ SIZE_INPUT_BUFFER ]; // Input buffer
u8 gmode = OFF_LINE;               // Major state
u8 bVerbose = TRUE;                // ATV1
u8 bQuite = FALSE;                 // ATQ0

u32 ustime = 0; // Timestamp value in micro seconds            

int main( void )
{  
  int c; 
  u8 pos = 0;                   // Position in buffer
  u8 atstate = WAITING_FOR_A;   // AT-state
 
  initHardware();
  uart0Init( B115200 );
  //uart0Init(B2x115200);       // Double speed
  sja1000_init();
 
  sei();
  uart0BlockingPuts( "\r\n" IDSTR "\r\n" );

  for ( ;; ) {

    // Give the dog a bone
    wdt_reset();   

    if ( -1 != ( c = uart0Getch() ) ) {

      if ( OFF_LINE == gmode ) {

	// echo character
	uart0BlockingPutch( c ); 

	if ( WAITING_FOR_A == atstate ) {
	  c &= ~0x20; // -> upper case
	  if ( 'A' == c ) {
	    atstate =  WAITING_FOR_T;
	  }
	}
	else if ( WAITING_FOR_T == atstate ) {
	  c &= ~0x20; // -> upper case
	  if ( 'T' == c ) {
	    atstate =  WAITING_FOR_CR;
	    pos = 0;
	  }
	  else if ( ( '/' & ~0x20 ) == c ) {    // "a/"
	    // Redo last command
	    doCommand();
	    atstate =  WAITING_FOR_A;
	  }
	}
	else if ( WAITING_FOR_CR == atstate ) {
	  // CR
	  if ( 0x0d == c ) {
	    gbuffer[ pos ] = 0x00;
	    doCommand();
	    atstate =  WAITING_FOR_A;
	  }
	  // Backspace
	  else if ( 0x08 == c ) {
	    if ( pos ) pos -= 1;
	  }
	  // Space
	  else if ( 0x20 == c  ) {
	    ; // Ignore
	  }
	  // Tab
	  else if ( 0x09 == c  ) {
	    ; // Ignore
	  }
	  else {
	    gbuffer[ pos++ ] = c;
	    if ( pos >= SIZE_INPUT_BUFFER ) {
	      // Buffer FULL
	      doResponse( FALSE, 0  );
	      atstate =  WAITING_FOR_A;
	    }
	  }
	}
	else {
	  // Invalid state
	  doResponse( FALSE, 0  );
	  atstate =  WAITING_FOR_A;
	}
      }
      else { // ON_LINE
	// Packet are just transfered over the
	// line
      }
    } // Character	
  } // Main loop
  return 0;
} 


///////////////////////////////////////////////////////////////////////////////
// initHardware
//

void initHardware( void )
{
  unsigned char ddrd;
  // Enable external memory access 
  // wait-states
  // INT0 low flank
  outp( ( ( 1 << SRE ) | ( 1 << SRW10 ) | ( 1 << ISC10  ) ), MCUCR  );	
  
  //PORTD bit 5 is output
  ddrd = inp( DDRD );
  outp( ddrd | 0x20, DDRD );
  outp( inp( PORTD ) | 0x20, PORTD );

  // TIMER0 
  // This timer is used as a uS counter for use by the timestamp
  // value 
  // T = 1/f (f=7.372000 MHz)
  // Prescaper set to 8
  // Period time is = 1/f/prescaler = prescaler/f
  // This gives 1.085 uS as period ( 8% error)
  // on each overflow.
 
  outp( 0x02, TIMSK ); // Enable TOMER0 overflow IRQ
  outp( 0x02, TIFR );  // Reset overflow flag
  outp( 0x02, TCCR0 ); // Prescaler = 8
 
  // Initiate Watchdog - 60 ms
  outp( 0x0a, WDTCR );
}

///////////////////////////////////////////////////////////////////////////////
// TIMER0 overflow
//

SIGNAL( SIG_OVERFLOW0 )
{
  ustime += 256;
}

///////////////////////////////////////////////////////////////////////////////
// udelay
//
// Delay for a number of microseconds

void udelay( u32 t )
{
  
}


  /*
  register u8 led = inp( PORTB );

  if ( led & 1 )
    led &= ~0x0f;
  else
    led |=  0x0f;
	
  outp( led, PORTB );     // turn LEDs on 
  */

///////////////////////////////////////////////////////////////////////////////
// CRLF
//

void crlf( void )
{
  uart0BlockingPutch( 0x0d );
  uart0BlockingPutch( 0x0a );
}


///////////////////////////////////////////////////////////////////////////////
// doCommand
//
// Handle a 'AT' command
//

void doCommand( void )
{
  int rv = 0;

  switch( ( *gbuffer & ~0x20 ) ) {
    
    // AT<CR>
  case 0x00:
    // OK
    break;

    // Autobaud detection
  case 'A': 
    rv = cmd_autobaud();
    break;
    
    // Set Baudrate
  case 'B': 
    rv = cmd_baudrate();
    break;
    
  case 'C': //
    {
      char buf[20];
      int rr;

      *buf = 0;
      rr = getNumericValue( ( gbuffer + 1 ) );
      itoa( rr, buf, 10 );
      uart0BlockingPutch( ' ' );
      uart0BlockingPuts( buf );
    }
    break;
    
  case 'D': // Dial - Go online
    rv = cmd_online();
    break;

  case 'E': // Turn on/off echo
    rv = cmd_echo();
    break;
    
  case 'F': // Set Filter/Mask
    rv = cmd_filter();
    break;
    
  case 'G': // Send Burst n - number of messages. sz - delay between frames
    rv = cmd_sendBurst();
    break;
    
  case 'H': // Hook - Go online
    rv = cmd_online();
    break;
    
  case 'I': // Information
    rv = cmd_info();
    break;
    
  case 'J':
    rv = -1; // Not implemented
    break;
    
  case 'K': 
    {
      canmsg_t msg;
      msg.id = 0x334;
      msg.length = 2;
      msg.data[ 0 ] = 1;
      msg.data[ 1 ] = 2;
      msg.flags |=  MSG_EXT;

      //if ( !sja1000_sendMsg( &msg ) ) {
      //doResponse( FALSE, 0  );
      //}
      if ( !sja1000_writeMsgToBuf( &msg ) ) {
	rv = -1; // Not implemented;
      }

      rv = 0;
    }
    break;
    
  case 'L':  // Go online in Listen only mode
    rv = cmd_listen();
    break;
    
  case 'M':
    rv = -1; // Not implemented
    break;
    
  case 'N':
    rv = -1; // Not implemented
    break;
    
  case 'O':
    rv = -1; // Not implemented
    break;
    
  case 'P':
    rv = -1; // Not implemented
    break;
    
  case 'Q': // Quite 
    rv = cmd_quite();
    break;
    
  case 'R': // Read message
    rv = cmd_readmsg();
    break;
    
  case 'S': // Set/read registers
    rv = cmd_register();
    break;
    
  case 'T':
    rv = -1; // Not implemented
    break;
    
  case 'U':
    rv = -1; // Not implemented
    break;
    
  case 'V': // Verbal
    rv = cmd_verbal();
    break;
    
  case 'W': // Write message
    rv = cmd_writemsg();
    break;
    
  case 'X':
    rv = -1; // Not implemented
    break;
    
  case 'Y':
    rv = -1; // Not implemented
    break;
    
  case 'Z': // Reset device
    rv = cmd_reset();
    break;
    
  case '?': // Get statistics
    rv = -1; // Not implemented
    break;

  default:  // Uknown command
    rv = -1; // Not implemented
    break;
  }

  // Print out response
  if ( 0 == rv ) {
    doResponse( TRUE, rv  );
  }
  else {
    doResponse( FALSE, rv  );
  }
}

///////////////////////////////////////////////////////////////////////////////
// doResponse
//
// Send response on AT command
//

void doResponse( int bResult, u8 code  )
{
  if ( bResult ) {
    // OK 
    uart0BlockingPuts( "\r\n" RESPONSE_OK "\r\n" );
  }
  else {
    // ERROR
    uart0BlockingPuts( "\r\n" RESPONSE_ERROR "\r\n" );
  }
}

/////////////////////////////////////////////////////////////////////
//                 C O M M A N D  -- H A N D L E R S
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// cmd_online
//
// 'd baud' - goes online with baud parameter
// 'a' - goes on line with current parameters
// 'o' - Go online again from +++
// 'h' - goes online
//
// returns o if OK or error code if not
//

int cmd_online( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_reset
//
// 'z'   - reset settings to config profile 0.
// 'z&f' - reset settings to factory defaults.
// 'z&w' - Save current settings in non-volatile memory.
//
// returns 0 if OK or error code if not
//

int cmd_reset( void )
{
  return 1;
}

////////////////////////////////////////////////////////////////////
// cmd_quiet
//
// 'q0' - quite off (enable messages) 
// 'q1' - quite on (disable messages) 
//
// returns 0 if OK or error code if not
//

int cmd_quite( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_verbal
//
// 'v0' - verbal off (numeric messages) 
// 'v1' - verbal on (word messages) 
//
// returns 0 if OK or error code if not
//

int cmd_verbal( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_echo
//
// 'e0' - echo off (characters not echoed) 
// 'e1' - echo on (characterrs are echoed) 
//
// returns 0 if OK or error code if not
//

int cmd_echo( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_info
//
// 'i'  - Device info string.
// 'i0' - Device info string.
// 'i1' - Hardware version
// 'i2' - softare version
//
// returns 0 if OK or error code if not
//

int cmd_info( void )
{
  // 'I0' or 'I'
  if ( ( *(gbuffer+1) == '0' ) || ( *(gbuffer+1) == 0x00 )  ) {
    uart0BlockingPuts( "\r\n" IDSTR );
    return 0;
  }
  // 'I1'
  else if ( *(gbuffer+1) == '1' ) {
    uart0BlockingPuts( "\r\n" HARDWARE_VERSION );
    return 0;
  }
  // 'I2'
  else if ( *(gbuffer+1) == '2' ) {
    uart0BlockingPuts( "\r\n" SOFTWARE_VERSION );
    return 0;
  }
  else {
    // Don't understand
    return -1;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
// cmd_baudrate
//
// 'b'  - Default baudrate (250 kbps).
// 'b0' - Baudrate set to 1 Mbps.
// 'b1' - Baudrate set to 500 Mbps.
// 'b2' - Baudrate set to 250 Mbps. (default)
// 'b3' - Baudrate set to 125 Mbps.
// 'b4' - Baudrate set to 100 Mbps.
// 'b5' - Baudrate set to 50 Mbps.
// 'b6' - Baudrate set to 20 Mbps.
// 'b7' - Baudrate set to 10 Mbps.
//
// returns 0 if OK or error code if not
//

int cmd_baudrate( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_filter
//
// 'f'  - Disable filtering.
// 'f0' - Disable filtering.
// 'f1' - Enable filtering.
// 'f2 id b' - set filter accept/reject id depending on b.
//
// returns 0 if OK or error code if not
//

int cmd_filter( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_listen
//
// 'l'  - Go on-line in listen only mode.
//
// returns 0 if OK or error code if not
//

int cmd_listen( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_autobaud
//
// 'a'  - Go on-line in autobaud mode.
//
// returns 0 if OK or error code if not
//

int cmd_autobaud( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_readmsg
//
// 'r'   - Read a message from the CAN bus (if available).
// 'r-'  - read all in the buffer.
// 'rn'  - read 'n' messages from buffer (max buffer size).
// 'r+n' - Read 'n' messages from buffer with wait (abort with char).
//
// returns 0 if OK or error code if not
//

int cmd_readmsg( void )
{
  canmsg_t msg;
  int i;
  int cnt = 1;
  int bBlocking = FALSE;

  wdt_reset();   

  // 'R-' 
  if ( *(gbuffer+1) == '-' ) {       // read all
    cnt = getRXMsgCnt();
  }
  
  // 'R+n'
  else if ( *(gbuffer+1) == '+' ) {
    if ( *(gbuffer+2) == 0x00 ) {
      cnt = 1;
    }
    else {
      cnt = atoi( (gbuffer+2) );
    }
    bBlocking = TRUE;
  }
  
  // 'R' read one message
  else if ( *(gbuffer+1) == 0  ) {   // no arg
    cnt = 1;
  }
  
  // 'Rn'
  else {
    cnt = atoi( (gbuffer+1) );
    // 'R0' wait forever for one message
    if ( !cnt ) {
      cnt = 1;
      bBlocking = TRUE;
    }
  }

  if ( 0 == cnt ) cnt = 1;

  if ( bBlocking ) {
    for( ; ; ) {
      wdt_reset();
      if ( -1 != uart0Getch() ) break; // aborted by char...
      if ( sja1000_readMsgFromBuf( &msg ) ) {
	print_CAN_Msg( &msg );
	cnt--;
	if ( 0 == cnt ) break;  // done
      }
    }
  }
  else {
    cnt %= CAN_RX_BUFFER_SIZE; // Max buffer size
    for ( i = 0; i< cnt; i++ ) {
      if ( sja1000_readMsgFromBuf( &msg ) ) {
	print_CAN_Msg( &msg );
      }
      else {
	return -1;
      }
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////
// cmd_writemsg
//
// 'w'  - Write a message out on the CAN bus.
// 'w' - write last message again
// 'we' - write extended frame
// 'ws  - write standard frame
// 'wr  - remote frame
// 'wxid,d0,d2,d3 where x is format and id is id and dn is data
// 0x... before a number interpret it as a hex number otherwise decimal 
// note: length is redundant info and is not given. 
//
// returns 0 if OK or error code if not
//

int cmd_writemsg( void )
{
  int rv = 0;
  int i;
  static canmsg_t canMsg;
  char *p = (gbuffer+1);

  // 'ws' 
  if ( (*p & ~0x20) == 'S' ) {       // Standard
    p++;
    canMsg.flags = 0;
  }
  // 'we'
  else if ( (*p & ~0x20) == 'E' ) {  // Extended Frame
    p++;
    canMsg.flags = MSG_EXT;
  }
  // 'wr'
  else if ( (*p & ~0x20) == 'R' ) {  // Remote Frame
    p++;
    canMsg.flags = MSG_RTR;
  }

  // 'w' 'ws' 'we' 'wr'?
  if ( 0 == *p ) {   
    // Send 'last message'
    if ( !sja1000_writeMsgToBuf( &canMsg ) ) {
      rv = -1; // error;
    }
  }
  else {

    uart0BlockingPutch( '<' );
    // NULL length is default
    canMsg.length = 0;

    // if there is a number at the current buffer pos
    // we have an id otherwise there is an error
    if ( ( *p >= '0' ) && ( *p <= '9' ) ) {
      uart0BlockingPutch( '-' );
      if ( 0xffff == ( canMsg.id = getNumericValue( p ) ) ) {
	// Error
	uart0BlockingPutch( '>' );
	return -1;
      }
      
    }
    else {
      // Syntax error
      uart0BlockingPutch( ':' );
      rv = -1;
    }
    
    uart0BlockingPutch( '?' );
    while ( *p && ( *p != ',') && ( *p != ' ') ) {
      p++;
    } 
    
    if ( *p ) {
    
      uart0BlockingPutch( '=' );
      p++; // move past ',' or space
      
      // We have data
      for ( i=0; i<8; i++ ) {
	if ( ( *p >= '0' ) && ( *p <= '9' ) ) {
	  canMsg.data[ i ] = getNumericValue( p );
	  
	  while ( *p && ( *p != ',') && ( *p != ' ') ) {
	    p++;
	  }
	  if ( 0 == *p ) break;
	  p++;
	}
	else {
	  // Syntax error
	  break;;
	}
      }
    }
      
    // Send  message
    if ( !sja1000_writeMsgToBuf( &canMsg ) ) {
      rv = -1; // error;
    }
  }

  return rv;
}

////////////////////////////////////////////////////////////////////
// cmd_register
//
// 's7=10'  - Set register value.
// 's7?'    - Read register value.
//
// S0 - 
// S1 -
// S2 -  0-255 Escape character, normally '+', 128-255 disables escape.
// S3 -  0-127 End of line character (0x0d/CR)
// S4 -  0-127 Line Feed character ( 0x0a/LF )
// S5 -  0-127 BS character ( 0x08/BS)
// s6 -  0-255 Baudrate
// s7 -  0-100 Sample point
// s9 -  0-255 btr0 
// s10 - 0-255 btr1
// s11 - 0-2 sjw
// s12 - 
// s13 -
// s14 -
// s15 - Messages in CAN RX buffer
// s16 - Messages in CAN TX buffer
// s17 -
// s18 -
// s19 -
// s20 - 0x00000000-0xffffffff  Current 32-bit time stamp value
// s21 - 0x00000000-0xffffffff  Received messages counter
// s22 - 0x00000000-0xffffffff  Transmitted messages counter
// s23 - 0x00000000-0xffffffff  Received data counter
// s24 - 0x00000000-0xffffffff  Transmitted data counter
// s25 - 0x00000000-0xffffffff  Received messages overun counter
// s26 - 0x00000000-0xffffffff  Transmitt overrun counter
// s27 - 0x00000000-0xffffffff  Bus warning counter counter
// s28 - 0x00000000-0xffffffff  Bus off counter
//
// returns 0 if OK or error code if not
//

int cmd_register( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// cmd_sendburst
//
// 'Gn'   - Write a ( or n )standard id message out on the CAN bus.
// 'Gsn'  - Write a ( or n )standard id message out on the CAN bus.
// 'Gen'  - Write a ( or n ) extended id  message out on the CAN bus.
//
// returns 0 if OK or error code if not
//

int cmd_sendBurst( void )
{
  return -1;
}

////////////////////////////////////////////////////////////////////
// hexout
//

void nibbleout (unsigned char b )
{
  b &= 0x0f;
  if ( b < 10 ) {
    uart0BlockingPutch( ( 0x30 + b ) );
  }
  else {
    uart0BlockingPutch( ( 0x41 + ( b - 10 ) ) );
  }
}

////////////////////////////////////////////////////////////////////
// hexout
//

void hexout( unsigned char b )
{
  unsigned char temp = b;

  b >>= 4;
  nibbleout( b );
  
  b = ( temp & 0x0f );
  nibbleout( b );
}


////////////////////////////////////////////////////////////////////
// print_CAN_Msg
//

void print_CAN_Msg( canmsg_t *pmsg )
{
  int i;

  crlf();

  // flags for packet   
  hexout( ( ( pmsg->flags >> 24 ) & 0xff ) );
  hexout( ( ( pmsg->flags >> 16 ) & 0xff ) );
  hexout( ( ( pmsg->flags >> 8  ) & 0xff ) );
  hexout( ( pmsg->flags & 0xff ) );
  uart0BlockingPutch( 0x20 );

  // id for frame
  hexout( ( ( pmsg->id >> 24 ) & 0xff ) );
  hexout( ( ( pmsg->id >> 16 ) & 0xff ) );
  hexout( ( ( pmsg->id >> 8  ) & 0xff ) );
  hexout( ( pmsg->id & 0xff ) );
  uart0BlockingPutch( 0x20 );

  // timestamp for frame
  hexout( ( ( pmsg->timestamp >> 24 ) & 0xff ) );  
  hexout( ( ( pmsg->timestamp >> 16 ) & 0xff ) );
  hexout( ( ( pmsg->timestamp >> 8  ) & 0xff ) );
  hexout( (   pmsg->timestamp & 0xff ) );
  uart0BlockingPutch( 0x20 );

  // length for data part of frame
  hexout( pmsg->length );
  uart0BlockingPutch( 0x20 );

  // CAN frame data
  for ( i = 0; i<pmsg->length; i++ ) {
    hexout( pmsg->data[ i ] );
  }
}

////////////////////////////////////////////////////////////////////
// getNumericValue
//

u32 getNumericValue( char *p )
{
  crlf();
  // must be a number '0x or a nummeric digit
  if ( ( *p >= '0' ) && ( *p <= '9' ) ) {
    
    // it is a nummeric digit
    if ( ( *(p+1) == 0 ) || *(p+1) == ',' || *(p+1) == 0x20  ) {
    
      // Decimal number
      return atoi( p );
    }
    else if ( (*(p+1) & ~0x20 ) == 'X'  ) {
      
      uart0BlockingPutch( 'A' );
      // Hex number
      p += 2; // point beyond '0x'
      
      // Make sure it is't just '0x'
      if ( *p ) {
	int digit[ 4 ];
	int i = 0;

	// First digit
	if ( -1 == ( digit[ i ] = getHexValue( *p++ ) ) ) {
	  uart0BlockingPutch( 'B' );
	  return 0xffff;
	}

	i++; // digit 2 
	if ( -1 == ( digit[ i ] = getHexValue( *p++ ) ) ) {
	  // One digit 0x9
	  uart0BlockingPutch( 'C' );
	  return digit[ 0 ];
	}

	i++; // digit 3
	if ( -1 == ( digit[ i ] = getHexValue( *p++ ) ) ) {
	  // Two digit 0x99
	  uart0BlockingPutch( 'D' );
	  return digit[ 0 ] * 16 + digit[ 1 ];
	}
	
	i++; // digit 4
	if ( -1 == ( digit[ i ] = getHexValue( *p++ ) ) ) {
	  // Three digit 0x999
	  uart0BlockingPutch( 'E' );
	  return digit[ 0 ] * 256 + digit[ 1 ] * 16 + digit[ 2 ];
	}

	// Four digit 0x9999
	uart0BlockingPutch( 'F' );
	return ( digit[ 0 ] * 4096 + 
		 digit[ 1 ] * 256 + 
		 digit[ 2 ] * 16 + digit[ 3 ] );
      }
      else {
	// it is '0x'
	uart0BlockingPutch( 'x' );
	return 0xffff;
      }
    }
    else {
      return atoi( p );
    }
  }
  else {
    uart0BlockingPutch( 'y' );
    return 0xffff;
  }

  uart0BlockingPutch( 'z' );
  return 0xffff;
}

////////////////////////////////////////////////////////////////////
// getHexValue
//

int getHexValue( char c )
{
  if ( ( c >= '0' ) && ( c <= '9' ) ) {
    return (c - '0' );
  }
  else if ( ( (c & ~0x20) >= 'A' ) && ( (c & ~0x20) <= 'F' ) ) {
    return ( ( (c & ~0x20) - 'A' ) + 10 );
  }
  
  return -1; // error
}





