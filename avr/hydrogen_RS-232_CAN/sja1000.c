//
// sja1000.c
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

#include <avr/io.h>
#include "uart.h"
#include "hydrogen.h"
#include "can.h"
#include "sja1000.h"
#include <compat/deprecated.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

extern u32 ustime; 

// CAN received buffer
static canmsg_t  can_rx_buffer[ CAN_RX_BUFFER_SIZE ];
static uint16_t can_rx_insert_idx, can_rx_extract_idx;

// CAN transmitt buffer
static canmsg_t  can_tx_buffer[ CAN_TX_BUFFER_SIZE ];
static uint16_t can_tx_insert_idx, can_tx_extract_idx;

// CAN statistics
static canstatistics_t canstat;

//////////////////////////////////////////////////////////////////////
// sja1000_enable_IRQ
//

void sja1000_enableIRQ( void )
{
#define  ENABLE_CAN_IRQ   
  REG( SJAIER ) = ( ( 1 << IER_BEIE ) | 
		    ( 1 << IER_EPIE ) | 
		    ( 1 << IER_DOIE ) | 
		    ( 1 << IER_EIE ) | 
		    ( 1 << IER_TIE ) | 
		    ( 1 << IER_RIE ) );
  
  // Enable INT0
  outp( inp( GIMSK ) | ( 1 << INT0 ) , GIMSK );
}

//////////////////////////////////////////////////////////////////////
// sja1000_disableIRQ
//

void sja1000_disableIRQ( void )
{
  REG( SJAIER ) = 0;
  
  // Disable INT0
  outp( inp( GIMSK ) & ~(1 << INT0) , GIMSK );
}

//////////////////////////////////////////////////////////////////////
// sja1000_enable_Config
//

int sja1000_enableConfig( void )
{
  // Disable CAN interrupts
  sja1000_disableIRQ();

  // Set Reset Mode
  while( !( REG( SJAMOD ) & ( 1 << MOD_RM ) ) ) {
    REG( SJAMOD ) = ( 1 << MOD_RM );
  }
 
  // We are in reset mode
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// sja1000_disableConfig
//

int sja1000_disableConfig( void )
{
  // Set Reset Mode
  while( ( REG( SJAMOD ) & ( 1 << MOD_RM ) ) ) {
    REG( SJAMOD ) = 0;
  }

  // Enable CAN interrupts
  sja1000_enableIRQ();

  // We are out of reset mode
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// sja1000_init
//

int sja1000_init( void )
{
  // Buffer initialization
  can_rx_insert_idx = can_rx_extract_idx = 0;
  can_tx_insert_idx = can_tx_extract_idx = 0;

  // Reset statistics
  canstat.cntRxPkt = 0;      // Counter for received packets.
  canstat.cntRxData = 0;     // Counter for received data bytes.
  canstat.cntTxPkt = 0;      // Counter for sent packets.
  canstat.cntTxData = 0;     // Counter for sent data bytes.
  canstat.cntWakeup = 0;     // Counter for chip wakeup's
  canstat.cntWarnings = 0;   // Counter for chip warning states.
  canstat.cntBusOff = 0;     // Counter for chip bus off conditions.
  canstat.cntRxFifoOvr = 0;  // Counter for Rx FIFO overruns.
  canstat.cntTxFifoOvr = 0;  // Counter for transmitter overruns (if NONBLOCK).
  canstat.cntStuffErr = 0;   // Counter for stuff errors.
  canstat.cntFormErr = 0;    // Counter for form erros.
  canstat.cntAckErr = 0;     // Counter for ACK errors.
  canstat.cntBit0Err = 0;    // Counter for Bit 0 errors.
  canstat.cntBit1Err = 0;    // Counter for Bit 1 errors.
  canstat.cntCrcErr = 0;     // Counter for CRC errors.

  // Enable configuration
  if ( !sja1000_enableConfig() ) return FALSE;

  // Set Mode, Clock out, comparator
  // 16 MHz / 2 = 8 MHz 
  REG( SJACDR ) = ( 1 << CDR_PELICAN );
 
  // Set driver output configuration
  // - Normal Mode 
  REG( SJAOCR ) = 0xde; 
  
  // Null mask
  if ( !sja1000_setExtendedMask( 0x00000000, 0xffffffff ) ) return FALSE;

  // Set baudrate
  if ( !sja1000_setBaudrate( INIT_VALUE_BAUDRATE, 
								INIT_VALUE_CRYSTAL, 
								 0,       // sjw
								 75,      // sample point
								 0 ) ) {  // flags
    return FALSE;
  }

  // Enable CAN interrupts
  sja1000_enableIRQ();

  // Release receive buffer
  // Clear data overrun
  REG( SJACMR ) = ( 1 << CMR_CDO ) | ( 1 << CMR_RRB );

  // Disable CAN configuration
  if ( !sja1000_disableConfig() ) return FALSE;

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// sja1000_setExtendedMask
//
// Must be in RESET mode
//

int sja1000_setExtendedMask( u32 code, u32 mask )
{
  int i;

  // LSB to +3, MSB to +0
  for ( i = SJA_PeliCAN_AC_LEN; --i>= 0; ) {
    REG( SJAACR0 + i ) = ( code & 0xff ); 
    REG( SJAAMR0 + i ) = ( mask & 0xff );
    code >>= 8;
    mask >>= 8;
  }
  
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// sja1000_setBaudrate
//
// Must be in RESET mode
//

int sja1000_setBaudrate( u32 rate,
			 u32 clock,
			 u32 sjw,
			 u32 sampl_pt,
			 u32 flags )
{
  int best_error = 0xfff;  // Big error
  int error;
  int best_tseg = 0;
  int best_brp = 0;
  int best_rate = 0;
  int brp = 0;
  int tseg = 0;
  int tseg1 = 0;
  int tseg2 = 0;

  clock /= 2;

  // tseg even = round down
  // tseg odd = round up
  for ( tseg = ( ( 0 + 0 + 2 ) * 2 ); 
	tseg <= ( ( MAX_TSEG2 + MAX_TSEG1 + 2 ) * 2 + 1 );
	tseg++ ) {
    
    brp = clock / ( ( 1 + tseg/2 ) * rate ) + ( tseg % 2 );
    
    if ( brp == 0 || brp > 64 ) continue;
    
    error = rate - clock/( brp * ( 1 + tseg / 2 ) );
    
    if ( error < 0 ) error = -error;
    
    if ( error <= best_error ) {
      best_error = error;
      best_tseg = tseg/2;
      best_brp = brp-1;
      best_rate = clock/( brp * ( 1 + tseg/2 ) );
    }
  }

  if ( best_error && ( rate/best_error < 10 ) ) {
    // This baudrate is not possible with this clock
    return FALSE;
  }

  tseg2 = best_tseg - ( sampl_pt * ( best_tseg + 1 ) ) / 100;

  if ( tseg2 < 0 ) tseg2 = 0;

  if ( tseg2 > MAX_TSEG2 ) tseg2 = MAX_TSEG2;

  tseg1 = best_tseg - tseg2 - 2;

  if ( tseg1 > MAX_TSEG1 ) {
    tseg1 = MAX_TSEG1;
    tseg2 = best_tseg - tseg1 - 2;
  }

  // Set the baudrate
  REG( SJABTR0 ) = ( ( sjw << 6 ) | best_brp ); 
  REG( SJABTR1 ) =
    ( ( ( ( flags & BTR1_SAM ) != 0 ) << 7 ) | ( tseg2 << 4 ) | tseg1 ); 
  
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// sja1000_readMsg
//

void sja1000_readMsg( void )
{
  int i, len;
  uint16_t reg_datastart;   // Regstart for data part of message
  canmsg_t *pmsg = NULL;
  uint16_t temp;
  
  do { 
    // Put the message in the receive fifo if there is
    // a message and there is room for the message
    temp = ( can_rx_insert_idx + 1 ) % CAN_RX_BUFFER_SIZE;
    
    if ( temp != can_rx_extract_idx ) {

      // add message to queue
      pmsg = (canmsg_t *)( (unsigned char *)can_rx_buffer + 
			   can_rx_insert_idx * 
			   sizeof( canmsg_t ) );

      // timestamp
      pmsg->timestamp = ustime + inp( TCNT0 ); 

      if ( REG( SJAFRM ) & ( 1 << FRM_FF ) ) {
	// Extended
	pmsg->id =
	  ( (u32)( REG( SJAID0 ) ) << 21 ) +
	  ( (u32)( REG( SJAID1 ) ) << 13 ) +
	  ( (u32)( REG( SJAID2 ) ) << 5 ) +
	  ( (u32)( REG( SJAID3 ) ) >> 3 );
	reg_datastart = SJADATE;
      } 
      else {
	// Standard
	pmsg->id =
	  ( (u32)( REG( SJAID0 ) ) << 3 ) +
	  ( (u32)( REG( SJAID1 ) ) >> 5 );
	reg_datastart = SJADATS;
      }
      
      // Set flags for Remote Frame, Extended
      pmsg->flags =
	( ( REG( SJAFRM ) & ( 1 << FRM_RTR ) ) ? ( 1 << MSG_RTR ) : 0) |
	( ( REG( SJAFRM )& ( 1 << FRM_FF ) ) ? ( 1 << MSG_EXT ) : 0);
      
      // Set message LEN
      len = ( REG( SJAFRM ) & FRM_DLC_M );
      
      // Get data
      for ( i=0; i < len; i++ ) {
	pmsg->data[ i ] = REG( reg_datastart + i );
      }

      pmsg->length = len;

      // Update buffer pointer
      can_rx_insert_idx = temp;

      // Print out the message
      //print_CAN_Msg( pmsg );

      // Write statistics
      canstat.cntRxPkt++;       // Another packet received
      canstat.cntRxData += len; // +len bytes received     
    }
    else {
      // The fifo is full - Just update statistics
      canstat.cntRxFifoOvr++;
    }

    REG( SJACMR ) =  ( 1 << CMR_RRB ); // Release Receive Buffer

  } while ( REG( SJASR ) & ( 1 << SR_RBS ) ); // More messages
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_prereadConfig

int sja1000_prereadConfig( void )
{
  // If no messages return
  if ( !( REG( SJASR ) & ( 1 << SR_RBS ) ) ) {
    return TRUE;
  }
  
  sja1000_disableIRQ();
  sja1000_readMsg();
  sja1000_enableIRQ();
 
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sja1000_readMsgFromBuf

int sja1000_readMsgFromBuf( canmsg_t *pmsg )
{
  // Must be a valid pointer
  if ( NULL == pmsg ) return FALSE;

  if ( can_rx_insert_idx == can_rx_extract_idx ) {    
    return FALSE; // Empty, nothing to read
  }

  // Get message
  memcpy( (unsigned char *)pmsg, 
	  (unsigned char *)( (unsigned char *)can_rx_buffer + 
			     can_rx_extract_idx * 
			     sizeof( canmsg_t ) ), 
	  sizeof( canmsg_t ) );

  can_rx_extract_idx++;
  can_rx_extract_idx %= CAN_RX_BUFFER_SIZE;

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_sendMsg

int sja1000_sendMsg( canmsg_t *pmsg )
{
  // Wait until Transmit Buffer Status is released 
  while ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) );
  
  if ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ) {

    // Transmit timed out
    REG( SJACMR ) = CMR_AT; // Abort transmission

    // Wait until Transmit Buffer Status is released 
    while ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ); 

    if ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ) {
      // Could not cancel, big problems
      return FALSE;
    }
  }
  
  // Write message to chip
  sja1000_writeMsg( pmsg );

  // Send the message
  REG( SJACMR ) = ( 1 << CMR_TR );

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_writeMsg
//

void sja1000_writeMsg( canmsg_t *pmsg )
{
  int i = 0;
  u32 id = 0;

  // Must have a message
  if ( NULL == pmsg ) return;

  pmsg->length &= FRM_DLC_M;
  REG( SJAFRM )= 
    ( ( pmsg->flags & ( 1 << MSG_EXT ) ) ? ( 1 << FRM_FF ) : 0 ) |
    ( ( pmsg->flags & ( 1 << MSG_RTR ) ) 
      ? ( 1 << FRM_RTR ) : 0 ) | pmsg->length;
  
  // Extended message
  if ( pmsg->flags & ( 1 << MSG_EXT ) ) {
   
    id = ( pmsg->id << 3 );
    REG( SJAID0 ) = ( id & 0xff );
    REG( SJAID1 ) = ( ( id >> 8 )  & 0xff );
    REG( SJAID2 ) = ( ( id >> 16 ) & 0xff );
    REG( SJAID3 ) = ( ( id >> 24 ) & 0xff );
    /*
      // Little Endian
      id = ( pmsg->id << 3 );
      REG( SJAID3 ) = ( id & 0xff );
      id >>= 8;
      REG( SJAID2 ) = ( id & 0xff );
      id >>= 8;
      REG( SJAID1 ) = ( id & 0xff );
      id >>= 8;
      REG( SJAID0 ) = id;
    */
    
    for( i=0; i < pmsg->length; i++) {
      REG( SJADATE + i ) = pmsg->data[ i ];
    }
  } 
  else {
    id = ( pmsg->id >> 5 );
    REG( SJAID0 ) = ( id & 0xff );
    REG( SJAID1 ) = ( ( id >> 8 )  & 0xff );
    /*
      // Little Endian
    id = ( pmsg->id >> 5 );
    REG( SJAID0 ) = ( id & 0xff );
    id >>= 8;
    REG( SJAID1 ) = ( id & 0xff );
    */

    for ( i=0; i < pmsg->length; i++ ) {
      REG( SJADATS + i ) = pmsg->data[ i ];
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// sja1000_sendMsgFromBuf
//

int sja1000_sendMsgFromBuf( void )
{
  canmsg_t *pmsg;

  if ( can_tx_insert_idx == can_tx_extract_idx ) {    
    return FALSE; // Empty, nothing to send
  }

  // Wait until Transmit Buffer Status is released 
  while ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) );
  
  if ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ) {
    
    // Transmit timed out
    REG( SJACMR ) = CMR_AT; // Abort transmission
    
    // Wait until Transmit Buffer Status is released 
    while ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ); 
    
    if ( !( REG( SJASR ) & ( 1 << SR_TBS ) ) ) {
      // Could not cancel, big problems
      return FALSE;
    }
  }
  
  // Get message from buffer
  pmsg = (canmsg_t *)( (unsigned char *)can_tx_buffer + 
		       can_tx_extract_idx * 
		       sizeof( canmsg_t ) );
  
  can_tx_extract_idx++;
  can_tx_extract_idx %= CAN_TX_BUFFER_SIZE;

  // Write message to the chip
  sja1000_writeMsg( pmsg );

  // Send the message
  REG( SJACMR ) = ( 1 << CMR_TR );

  // Statistics
  canstat.cntTxPkt++;                 // Counter for sent packets.
  canstat.cntTxData += pmsg->length;  // Counter for sent data.

  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// sja1000_writeMsgToBuf
//

int sja1000_writeMsgToBuf( canmsg_t *pmsg )
{
  uint16_t temp;

  // Must have a message
  if ( NULL == pmsg ) return FALSE;

  canmsg_t *pmsgbuf;

  // Put the message in the receive fifo if there is
  // a message and there is room for the message
  temp = ( can_tx_insert_idx + 1 ) % CAN_TX_BUFFER_SIZE;
    
  if ( temp == can_tx_extract_idx ) {
    // The fifo is full - Just update statistics
    canstat.cntTxFifoOvr++;
    return FALSE;
  }
  
  // add message to queue
  pmsgbuf = (canmsg_t *)( (unsigned char *)can_tx_buffer + 
			  can_tx_insert_idx * 
			  sizeof( canmsg_t ) );
  memcpy( (unsigned char *)pmsgbuf, 
	  (unsigned char *)pmsg, 
	  sizeof( canmsg_t ) );
  
  // Update buffer pointer
  can_tx_insert_idx = temp;
  
  // Enable TX interrupt if not enabled.
  if ( !( REG( SJAIER ) & ( 1 << IER_TIE ) ) ) {
    REG( SJAIER ) |= ( 1 << IER_TIE );
    sja1000_sendMsgFromBuf();
  }

  sja1000_sendMsgFromBuf();

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_check_tx_stat
//

int sja1000_check_tx_stat( void )
{
  if ( ( SJASR ) & ( 1 << SR_TCS ) ) {
    return FALSE;
  }
  else {
    return  TRUE;
  }
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_setbtregs
//
// Must be in RESET mode
//

int sja1000_setbtregs( u16 btr0, u16 btr1 )
{
  REG( SJABTR0 ) = btr0;
  REG( SJABTR1 ) = btr1;
  
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_remoteRequest

int sja1000_remoteRequest( void )
{
  // sja1000_remote_request not implemented!!!
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_standardMask

int sja1000_standardMask( u16 code, u16 mask )
{
  // sja1000_standard_mask not implemented!!!
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// getRXMsgCnt
//

u16 getRXMsgCnt( void )
{
  return abs( can_rx_insert_idx - can_rx_extract_idx );
}

///////////////////////////////////////////////////////////////////////////////
// getTXMsgCnt
//

u16 getTXMsgCnt( void )
{
  return abs( can_tx_insert_idx - can_tx_extract_idx );
}


///////////////////////////////////////////////////////////////////////////////
// sja1000 irq-handler
//
// signal handler for external interrupt int0 
//

SIGNAL( SIG_INTERRUPT0 )     
{
  u8 irq_register;

  irq_register = REG( SJAIR );

  // *** Data to Read ***
  if ( ( irq_register & ( 1 << IR_RI ) ) ) {
    // Read Data
    sja1000_readMsg();
  } 
  
  // *** Write Data ***
  if ( ( irq_register & ( 1 << IR_TI ) ) ) {
    // Transmitt Data
    if ( !sja1000_sendMsgFromBuf() ) {
      ;
    }
  }
  
  // *** Problems ***
  if ( ( irq_register & ( ( 1 << IR_EI ) | 
			  ( 1 << IR_BEI ) | 
			  ( 1 << IR_EPI ) | 
			  ( 1 << IR_DOI )  )) )  { 
    // Some error happened    
  }

  if ( inp( PORTD ) & 0x020 ) {
    outp( inp( PORTD ) & ~0x20, PORTD );
  }
  else {
    outp( inp( PORTD ) | 0x20, PORTD );
  }
}


