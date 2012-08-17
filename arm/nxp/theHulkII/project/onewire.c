/************************************************************************/
/*                                                                      */
/*      Access Dallas 1-Wire Device with Philips LPC2000 ARM7DTMI	    */
/*                                                                      */
/* based on code for ATMEL AVR from  Peter Dannegger <danni@specs.de>   */
/* modified by Martin Thomas <eversmith@heizung-thomas.de> 9/2004       */
/* adapted to ARM7/LPC2000 by M. Thomas 11/2004                         */
/************************************************************************/

#include "lpc213x.h"
#include "types.h"
#include "config.h"
#include "armVIC.h"
#include "sysTime.h"
#include "debug.h"
#include "delay.h"
#include "uart.h"
#include "onewire.h"

#define OW_GET_IN()   ( IOPIN0 & PIO0_ONE_WIRE_BIT )
#define OW_OUT_LOW()  ( IOCLR0 = PIO0_ONE_WIRE_BIT )
#define OW_OUT_HIGH() ( IOSET0 = PIO0_ONE_WIRE_BIT )
#define OW_DIR_IN()   ( IODIR0 &= ~(PIO0_ONE_WIRE_BIT) )
#define OW_DIR_OUT()  ( IODIR0 |= PIO0_ONE_WIRE_BIT )

///////////////////////////////////////////////////////////////////////////////
// ow_input_pin_state
//

BOOL ow_input_pin_state()
{
	return ( (BOOL)OW_GET_IN() );
}

///////////////////////////////////////////////////////////////////////////////
// ow_parasite_enable
//

void ow_parasite_enable(void)
{
	
}

///////////////////////////////////////////////////////////////////////////////
// ow_delay_debug
//

void ow_delay_debug(void)
{
	OW_OUT_LOW(); // disable internal pull-up (maybe on from parasite)
	OW_DIR_OUT(); // pull OW-Pin low for 480us
	delay_us( 100 );
	OW_OUT_HIGH();
	delay_us( 500 );
	OW_OUT_LOW();
	delay_us( 100 );
	OW_DIR_IN(); // pull OW-Pin low for 480us
}

///////////////////////////////////////////////////////////////////////////////
// ow_reset
//
// Return 0 if reset went OK
//

BOOL ow_reset(void)
{
	uint8_t err = 0;
	unsigned mycpsr;
	
	OW_OUT_LOW(); 					// disable internal pull-up (maybe on from parasite)
	OW_DIR_OUT(); 					// pull OW-Pin low for 480us
	
	delay_us( 480 );
	
	mycpsr = disableIRQ();
	
	// set Pin as input - wait for clients to pull low
	OW_DIR_IN(); 					// input
	
	delay_us( 66 );
	err = OW_GET_IN();				// Check for presence
									
	restoreIRQ( mycpsr );
	
	if ( err) return (BOOL)err;	// no presence detect
									// nobody pulled to low, still high
	
	// after a delay the clients should release the line
	// and input-pin gets back to high due to pull-up-resistor
	delay_us( 480 - 66 );
	if ( 0 == OW_GET_IN() ) {		// short circuit
		err = 1;
	}
		
	return (BOOL)err;
}

///////////////////////////////////////////////////////////////////////////////
// ow_bit_io
//
// Timing issue when using runtime-bus-selection (!OW_ONE_BUS):
//   The master should sample at the end of the 15-slot after initiating
//   the read-time-slot. The variable bus-settings need more
//   cycles than the constant ones so the delays had to be shortened 
//   to achive a 15uS overall delay 
//   Setting/clearing a bit in I/O Register needs 1 cyle in OW_ONE_BUS
//   but around 14 cyles in configureable bus (us-Delay is 4 cyles per uS) 
//

uint8_t ow_bit_io( uint8_t b )
{
	unsigned mycpsr;
	
	mycpsr = disableIRQ();
	
	OW_DIR_OUT(); 					// drive bus low
	
	delay_us(1); 					// Recovery-Time wuffwuff was 1
	if ( b ) OW_DIR_IN(); 			// if bit is 1 set bus high (by ext. pull-up)
		
	// wuffwuff delay was 15uS-1 see comment above
	delay_us( 15 - 1 - OW_CONF_DELAYOFFSET );
		
	if ( 0 == OW_GET_IN() ) b = 0;  // sample at end of read-timeslot
	
	delay_us(100-15);
	OW_DIR_IN();
	
	restoreIRQ(mycpsr);
	
	return b;
}

///////////////////////////////////////////////////////////////////////////////
// ow_byte_wr
//

uint8_t ow_byte_wr( uint8_t b )
{
	uint8_t i = 8, j;
	
	do {
		j = ow_bit_io( b & 1 );
		b >>= 1;
		if( j ) b |= 0x80;
	} while( --i );
	
	return b;
}

///////////////////////////////////////////////////////////////////////////////
// ow_byte_rd
//

uint8_t ow_byte_rd( void )
{
	// read by sending 0xff (a dontcare?)
	return ow_byte_wr( 0xFF ); 
}

///////////////////////////////////////////////////////////////////////////////
// ow_rom_search
//

uint8_t ow_rom_search( uint8_t diff, uint8_t *id )
{
	uint8_t i, j, next_diff;
	uint8_t b;
	
	if ( ow_reset() ) return OW_PRESENCE_ERR;	// error, no device found

	ow_byte_wr( OW_SEARCH_ROM );				// ROM search command
	next_diff = OW_LAST_DEVICE;					// unchanged on last device
	
	i = OW_ROMCODE_SIZE * 8;					// 8 bytes
	
	do {
		j = 8;									// 8 bits
		do {
			b = ow_bit_io( 1 );					// read bit
			if ( ow_bit_io( 1 ) ) {			// read complement bit
				if ( b )	{					// 11
					return OW_DATA_ERR;		// data error
				}
			}
			else {
				if ( !b ) {				// 00 = 2 devices
					if ( diff > i || ((*id & 1) && diff != i) ) {
						b = 1;				// now 1
						next_diff = i;		// next pass 0
					}
				}
			}
			ow_bit_io( b );     		// write bit
			*id >>= 1;
			if( b ) *id |= 0x80;		// store bit
			
			i--;
			
		} while( --j );
		
		id++;							// next byte
	
	} while( i );
	
	return next_diff;					// to continue search
}

///////////////////////////////////////////////////////////////////////////////
// ow_command
//

void ow_command( uint8_t command, uint8_t *id )
{
	uint8_t i;

	if ( ow_reset() ) {
		uart0Puts( "OW: Reset failed!\r\n" );
	}

	if ( id ) {
	
		ow_byte_wr( OW_MATCH_ROM );		// to a single device
		i = OW_ROMCODE_SIZE;
		
		do {
			ow_byte_wr( *id );
			id++;
		} while( --i );
	} 
	else {
		ow_byte_wr( OW_SKIP_ROM );		// to all devices
	}
	
	ow_byte_wr( command );
}
