/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	hardware specific settings and functions
*******************************************************************************/

#include "digiprobe.inc"
/*
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include "digiprobe_system.h"
*/

//
void init_avr_ports(void)
{
	/* enable MAX3051 transceiver
	PD7 - CAN_SHDN (must be output, low)
	PD6 - CAN_RX (must be input)
	PD5 - CAN_TX (must be output)
	PD4 - was CAN_RS (must be output, low)
	PD1 - SDA, unused so far
	PD0 - SCL, unused so far
	*/
	 DDRD=0b10110000;
	PORTD=0b00000000;

	/* configure port B, carries SPI ADC MCP3551
	PB0 - SPI device select, enable MCP3551 on LOW
	PB1 - SPI Clock out, used as debug LED output
	PB2 - SPI Data out
	PB3 - SPI Data input
	*/
	 DDRB=0b00000011;
	PORTB=0b00001011;

	/* configure port E
	PE0 - Serial Programming Data Input, used as Debug LED
	*/
	 DDRE=0b00000011;
	PORTE=0b00000000;

}

// this selects the proper SPI device, e.g. it enables or disables
// the chip select of the different spi slaves
void spi_chip_select (uint8_t spidevice)
{
	switch (spidevice) {
	
	case SPI_DEVICE_MCP3550:
		PORTB &= ~_BV(PB0);
		break;

	default:
		PORTB |= _BV(PB0);

	}
}

//---------------------------------------------------------------------
//Einfaches Lesen und speichern von Daten in/aus einem Byte-Array
void array_write(uint8_t* array, uint16_t pos,  uint16_t bytes, void* data)
	{
	uint8_t* d = (uint8_t*) data;
	for(uint16_t i=0; i<bytes; i++) array[i+pos] = d[i];
	}

void array_read (uint8_t* array, uint16_t pos,  uint16_t bytes, void* data)
	{
	uint8_t* d = (uint8_t*) data;
	for(uint16_t i=0; i<bytes; i++) d[i] = array[i+pos];
	}
//------------------------------------
// Benutzt wird das so:
// int32_t wert=-200000;
// array_write(vscp_omsg.data, 1, 4, (void*)&wert);
//------------------------------------

///////////////////////////////////////////////////////////////////////////////
//  initTimer

void initTimer()
{	
	// Enable timer0 compare interrupt
	TIMSK0 = ( 1 << OCIE0A );

    // Sets the compare value
	// OCR0A = fclk/(2nf) - 1 = fclk/512000 - 1
	#ifndef FOSC
		#error You must define FOSC in yor makefile
	#elif FOSC == 8000	
		OCR0A = 32;
	#endif	

	// Set Clear on Timer Compare (CTC) mode, CLK/256 prescaler
	TCCR0A = ( 1 << WGM01 ) | ( 0 << WGM00 ) | ( 4 << CS00 );
}
