//lcd.c
// From Olimex Sample http://www.olimex.com
//

#include <string.h>
#include "lcd.h"
#include "delay.h"
#include "lpc213x.h"
#include "config.h"

// D4-D7 is on P0.4 - P0.7
//#define PIN_LCD_D4	4	// P0.4
//#define PIN_LCD_D5	5	// P0.5
//#define PIN_LCD_D6	6	// P0.6
//#define PIN_LCD_D7	7	// P0.7

// Control pins
//#define PIN_LCD_RS	10	// P0.10
//#define PIN_LCD_RW	12	// P0.12
//#define PIN_LCD_E 	13	// P0.13

unsigned long data;
//void Delay (unsigned long a) { while (--a!=0); }

void E_Pulse( void )
{
	IOSET0 = PIO0_LCD_E_BIT; //set E to high
	delay_us_(10);
	IOCLR0 = PIO0_LCD_E_BIT; //set E to low
}

void LCDInit( void )
{	
	// This is done in the low init routine!!!
	
	// first set D4, D5, D6, D7, RS, RW, E to output ports
	/*IODIR0 = PIO0_LCD_DB4_BIT |
					PIO0_LCD_DB5_BIT |
					PIO0_LCD_DB6_BIT |
					PIO0_LCD_DB7_BIT |
					PIO0_LCD_RS_BIT |
					PIO0_LCD_RW_BIT |
					PIO0_LCD_E_BIT;
					
	delay_ms(100);				*/

	// LCD initialization
	// step by step from LCD Display data sheed
	IOCLR0 = PIO0_LCD_RS_BIT;				// set RS port to 0
	IOCLR0 = PIO0_LCD_RW_BIT;				// set R/W port to 0
	IOCLR0 = PIO0_LCD_E_BIT;				// set E port to 0

	delay_ms( 110 );       					// delay ~110ms

	IOSET0 = PIO0_LCD_DB4_BIT;			// set D4 port to 1	0x30
	IOSET0 = PIO0_LCD_DB5_BIT;			// set D5 port to 1	
	IOCLR0 = PIO0_LCD_DB6_BIT;			// set D6 port to 0
	IOCLR0 = PIO0_LCD_DB7_BIT;			// set D7 port to 0

	E_Pulse();          						// high->low to E port (pulse)
	delay_ms( 10 );       					// delay ~10ms

	IOSET0 = PIO0_LCD_DB4_BIT;			// set D4 port to 1	0x30
	IOSET0 = PIO0_LCD_DB5_BIT;			// set D5 port to 1
	IOCLR0 = PIO0_LCD_DB6_BIT;			// set D6 port to 0
	IOCLR0 = PIO0_LCD_DB7_BIT;			// set D7 port to 0

	E_Pulse();            					// high->low to E port (pulse)
	delay_ms(10);         					// delay ~10ms

	IOSET0 = PIO0_LCD_DB4_BIT;			// set D4 port to 1	0x30
	IOSET0 = PIO0_LCD_DB5_BIT;			// set D4 port to 1
	IOCLR0 = PIO0_LCD_DB6_BIT;			// set D5 port to 0
	IOCLR0 = PIO0_LCD_DB7_BIT;			// set D6 port to 0
	
	E_Pulse();           						// high->low to E port (pulse)
	delay_ms(10);       						// delay ~10ms

	// Set interface to be 4-bit long
	IOCLR0 = PIO0_LCD_DB4_BIT;			// set D4 port to 0	0x20
	IOSET0 = PIO0_LCD_DB5_BIT;			// set D4 port to 1
	IOCLR0 = PIO0_LCD_DB6_BIT;			// set D5 port to 0
	IOCLR0 = PIO0_LCD_DB7_BIT;			// set D6 port to 0

	E_Pulse();           						// high->low to E port (pulse)
	delay_ms(10);       						// delay ~10ms
	
}

void LCDSendCommand( unsigned long a )
{				
	IOCLR0 = PIO0_LCD_RW_BIT;	// set RW port to 0

	delay_ms(2);                // delay for LCD char ~2ms

	data = 0xffffff0f | a;                  	// get high 4 bits
	IOCLR0 |= 0x000000f0;                    	// clear D4-D7
	IOSET0 = (IOSET0 | 0x000000f0) & data;  	// set D4-D7

	IOCLR0 = PIO0_LCD_RS_BIT;									// set RS port to 0 -> display set to comand mode                 	
	E_Pulse();                              	// pulse to set d4-d7 bits

	data = (a << 4) & 0x000000f0;   	  			// get low 4 bits
	IOCLR0 |= 0x000000f0;                    	// clear D4-D7
	IOSET0 = (IOSET0 & 0xffffff0f) | data;		// set D4-D7 (only PORTC4-PORTC7)

	IOCLR0 = PIO0_LCD_RS_BIT;									// set RS port to 0 -> display set to comand mode
	E_Pulse();                              	// pulse to set d4-d7 bits

}

void LCDSendChar(unsigned long a)
{
	IOCLR0 = PIO0_LCD_RW_BIT;									// set RW port to 0

	delay_ms(2);                            	// delay for LCD char ~2ms

	data = 0xffffff0f | a;                  	// get high 4 bits
	IOCLR0 |= 0x000000f0;                    	// clear D4-D7
	IOSET0 = (IOSET0 | 0x000000f0) & data;  	// set D4-D7

	IOSET0 = PIO0_LCD_RS_BIT;									// set RS port to 1 -> display set to data mode                	
	E_Pulse();                              	// pulse to set d4-d7 bits

	data = (a << 4) & 0x000000f0;   	  			// get low 4 bits
	IOCLR0 |= 0x000000f0;                    	// clear D4-D7
	IOSET0 = (IOSET0 & 0xffffff0f) | data;		// set D4-D7 (only PORTC4-PORTC7)

	IOSET0 = PIO0_LCD_RS_BIT;									// set RS port to 1 -> display set to data mode
	E_Pulse();                              	// pulse to set d4-d7 bits
}

void LCDSendTxt(char* a)
{
	for ( int i=0; i<strlen(a); i++ ) {
		LCDSendChar( a[i] );
	}
}

