/*
  Copyright (C) 2001 Jesper Hansen <jesperh@telia.com>.

  Rewritten by:	Nikolai Vorontsov <nickviz@mail.be>

  Rewritten by  Romuald Bialy (aka MIS) <romek_b@o2.pl>

  This file is part of the yampp system.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <io.h>
#include <timer.h>
#include <sig-avr.h>
#include <progmem.h>

#include "remote.h"
#include "delay.h"
#include "uart.h"					// enable for debug

#ifdef ENABLE_IR

#ifdef SETUP_REMOTE_CODES
#include "lcd.h"

void lcd_hexB(u08 din)
{
u08 d = (din >> 4) & 0x0f;
	lcd_putchar((d < 10) ? d+'0' : d+'A'-10);
	d = din & 0x0f;
	lcd_putchar((d < 10) ? d+'0' : d+'A'-10);
}

#endif


bool rec80_active(void)
{
	register u08 i = 50;

	while (i--)
		if (bit_is_clear(IR_PORT-2, IR_BIT))
			return true;
	return false;
}

static inline void timer0_src(u08 src)
{
	outb (src, TCCR0);
}

#ifndef SETUP_REMOTE_CODES
//---------------------------------------------------------------------------------------

#if (REM_STD == 1)
/*
	The REC-80 format used by Panasonic is a space coded 48 bit code consisting 
	of a 32 bit group id, followed by a 16 bit commmand word.
	Leading this is a header consisting of a 10T signal and a 4T pause.
	All bits start with a pulse of length T. The length of the pause following
	indicates the bit value. A T pause for a 0-bit and a 3T pause for a 1-bit.
*/

u16 get_rec80(void)
{
	u08 i, tmp = 0;
	u08 time;
	u08 T2,T4;
	union u16convert code;

	loop_until_bit_is_set(IR_PORT-2, IR_BIT);	// skip leading signal

	timer0_src(CK256);				//update every 32us
	timer0_start();

	while (bit_is_set(IR_PORT-2, IR_BIT))
	{
		T2 = inp(TCNT0);
		if (T2 >= 100)		// max wait time
			return 0;
	}
	
	// measure time T

	timer0_start();
	loop_until_bit_is_set(IR_PORT-2, IR_BIT);
	
	T2 = inp(TCNT0);		// T is normally around 0E-10 hex = 15 -> 480 uS
	T2 = T2 * 2;
	// max time is 4T
	T4 = T2 * 2;		

	for (i = 0; i < 48; i++)
	{
		timer0_start();
		while(1)
		{
			time = inp(TCNT0);
			if (time > T4)
				return 0;
			
			// measure time on the lo flank
			if (bit_is_clear(IR_PORT-2, IR_BIT))
			{
				tmp <<= 1;
				if (time >= T2)
					tmp++;
				break;
			}
		}

		// save command data as we go
		if( i == 39)
			code.bytes.low = tmp;

		if( i == 47)
			code.bytes.high = tmp;
		
		// syncronize - wait for next hi flank
		loop_until_bit_is_set(IR_PORT - 2, IR_BIT);
	}
#ifdef SHOW_REMOTE_CODES
	UART_Puts_p(PSTR("IR-CODE: "));
	UART_Printfu16(code.value); 
	EOL();		 		 // enable for debuging
#endif	
	return code.value;
}
#endif // (REM_STD == 1)

//---------------------------------------------------------------------------------------

#if (REM_STD == 2)
/*
	The NEC format is a space coded 32 bit code consisting 
	of a 16 bit group id, followed by a 16 bit commmand word.
	Leading this is a header consisting of a 16T signal and a 8T pause.
	All bits start with a pulse of length T. The length of the pause following
	indicates the bit value. A T pause for a 0-bit and a 3T pause for a 1-bit.

	Modified REC80 format by Will Jenkins, wdj@cus.org.uk
*/

u16 get_rec80(void)
{
	u08 i, tmp = 0;
	u08 time;
	u08 T2,T4;
	union u16convert code;

	loop_until_bit_is_set  (IR_PORT-2, IR_BIT);		// skip leading signal

	timer0_src(CK256);	//update every 32us
	timer0_start();

	while (bit_is_set(IR_PORT-2, IR_BIT))
	{
		T2 = inp(TCNT0);
		if (T2 >= 140)	// max wait time was 100
			return 0;
	}
	
	// measure time T
	timer0_start();
	loop_until_bit_is_set(IR_PORT-2, IR_BIT);
	
	T2 = inp(TCNT0);		// T is normally around 0E-10 hex = 15 -> 480 uS
	T2 = T2 * 2;
	// max time is 4T
	T4 = T2 * 2;		

	for (i = 0; i < 32; i++)
	{
		timer0_start();
		while(1)
		{
			time = inp(TCNT0);
		
			if (time > T4)
				return 0;
			
			// measure time on the lo flank
			if (bit_is_clear(IR_PORT-2, IR_BIT))
			{
				tmp <<= 1;
				if (time >= T2)
					tmp += 1;
				break;
			}
		}

		// save command data as we go
		if( i == 15)
			code.bytes.high = tmp;

		 if( i == 31)
			code.bytes.low = tmp;
		
		// syncronize - wait for next hi flank
		loop_until_bit_is_set(IR_PORT - 2, IR_BIT);
	 }

#ifdef SHOW_REMOTE_CODES
	UART_Puts_p(PSTR("IR-CODE: "));
	UART_Printfu16(code.value);
	EOL();		 		 // enable for debuging
#endif
	return code.value;
 }
#endif // (REM_STD == 2)

//---------------------------------------------------------------------------------------

#if (REM_STD == 3) || (REM_STD == 4)
/*
	The SONY format is a pulse coded 12 or 15 bit code consisting 
	of a 6 bit device code, followed by a 6 or 9 bit commmand word.
	Leading this is a header consisting of a 4T signal pulse.
	All bits are composed of a pulse of either 1T or 2T in length followed by a
	1T pause. A 2T pulse indicates a 1 bit, and a 1T pulse is a 0 bit.
*/ 

u16 get_rec80(void)
{
	u08  d, T2;
	u16 tmp;
	union u16convert code;

	code.value = 0;
	timer0_src(CK256);	//update every 32us
	timer0_start();
	loop_until_bit_is_set(IR_PORT-2, IR_BIT);	// leading signal
	if (inp(TCNT0) <= 60) return 0;			// is it the leading Signal ?			

	// now looking for the Data-bits
#if (REM_STD == 3)
	for(d=0; d < 14; d++)
#else
	for(d=0; d < 11; d++)
#endif
	{
		tmp = 1;
		while (bit_is_set(IR_PORT-2, IR_BIT))
		{
			T2 = inp(TCNT0);
			if (T2 >= 140)			// max wait time
				return 0;
		}
		timer0_start();
		while (bit_is_clear(IR_PORT-2, IR_BIT))
		{
			T2 = inp(TCNT0);
			if (T2 >= 140)			// max wait time
				return 0;
		}
		if (inp(TCNT0) >= 35)			// pulse longer than 1 ms? then logic "1"
			code.value += (tmp << d);	// the device code 

	}
#ifdef SHOW_REMOTE_CODES
	UART_Puts_p(PSTR("IR-CODE: "));
	UART_Printfu16(code.value);
	EOL();		 				// enable for debuging
#endif

	return code.value;
}
#endif // (REM_STD == 3) || (REM_STD == 4)

//---------------------------------------------------------------------------------------

#if (REM_STD == 5)
/*
	The RC-5 format used by Philips is bi-phase coded 13 bit code consisting 
	of a 7 bit device code, followed by 6 bit commmand word. Highest two bits of
	device code is startbit and troggle bit. Troggle bit must be masked out 
	because	its change after any keypress.
*/ 

u16 get_rec80(void)
{
	u08 i, time, T2, tmp = 0;
	union u16convert code;

	timer0_src(CK256);				//update every 32us
	loop_until_bit_is_set(IR_PORT-2, IR_BIT);	// skip leading signal
	for(i=0; i<13; i++)
	{
		if(bit_is_clear(IR_PORT-2, IR_BIT) )
			T2 = 0;
		else
			T2 = 1;

		timer0_start();
		while(1)
		{
			time=inp(TCNT0);
			if(time > 0x21)
				return 0;

			if(bit_is_clear(IR_PORT-2, IR_BIT) && (T2==1) )
			{
				tmp <<= 1;
				tmp++;
				break;
			}
			else if(bit_is_set(IR_PORT-2, IR_BIT) && (T2==0) )
			{
				tmp <<= 1;
				break;
			}
		}

		//save address data
		if(i == 6)
		{
			code.bytes.high = (tmp & 0x5f);		// save address and cut troggle bit
			tmp=0;
		}

		//delay
		timer0_start();
		while(1)
		{
			time=inp(TCNT0);
			if(time > 0x21)
				break;
		}
	}
	code.bytes.low = tmp;

#ifdef SHOW_REMOTE_CODES
	UART_Puts_p(PSTR("IR-CODE: "));
	UART_Printfu16(code.value);
	EOL();		 					// enable for debuging
#endif
	return code.value;
}

#endif // (REM_STD == 5)

//---------------------------------------------------------------------------------------

#else // #ifndef SETUP_REMOTE_CODES

//---------------------------------------------------------------------------------------

volatile u08 std;

u16 get_rec80(void)
{
		u08 i, T2, T4, time, tmp = 0;
		union u16convert code;

		code.value = 0;
		timer0_src(CK256);				//update every 32us
		timer0_start();
		loop_until_bit_is_set(IR_PORT-2, IR_BIT);	// skip leading signal


	if(std < 2)
	{	
		timer0_start();

		while (bit_is_set(IR_PORT-2, IR_BIT))
		{
			T2 = inp(TCNT0);
			if (T2 >= 140)				// max wait time
				return 0;
		}
	
		// measure time T

		timer0_start();
		loop_until_bit_is_set(IR_PORT-2, IR_BIT);
	
		T2 = inp(TCNT0);				// T is normally around 0E-10 hex = 15 -> 480 uS
		T2 = T2 * 2;
		// max time is 4T
		T4 = T2 * 2;
		
		for (i = 0; i < ((std == 0) ? 48 : 32); i++)
		{
			timer0_start();
			while(1)
			{
				time = inp(TCNT0);
				if (time > T4)
					return 0;
			
				// measure time on the lo flank
				if (bit_is_clear(IR_PORT-2, IR_BIT))
				{
					tmp <<= 1;
					if (time >= T2)
						tmp++;
					break;
				}
			}


			// save command data as we go
			if (std == 0)
			{
				if( i == 39)
					code.bytes.low = tmp;
				if( i == 47)
					code.bytes.high = tmp;
			}
			else
			{
				if( i == 15)
					code.bytes.high = tmp;
				if( i == 31)
					code.bytes.low = tmp;
			}

			// syncronize - wait for next hi flank
			loop_until_bit_is_set(IR_PORT - 2, IR_BIT);
		}
		return (code.value);
	}
	else if(std < 4)					// Format used by SONY remote controllers
	{
		if (inp(TCNT0) <= 60) 
			return 0;				// is it the leading Signal ?			

		// now looking for the Data-bits
		for(i=0; i < ((std == 3) ? 11 : 14); i++)
		{
			tmp = 0x01;
			while (bit_is_set(IR_PORT-2, IR_BIT))
			{
				T2 = inp(TCNT0);
				if (T2 >= 140)			// max wait time
					return 0;
			}
			timer0_start();
			while (bit_is_clear(IR_PORT-2, IR_BIT))
			{
				T2 = inp(TCNT0);
				if (T2 >= 140)			// max wait time
					return 0;
			}
			if (inp(TCNT0) >= 25)			// pulse longer than 1 ms? then logic "1"
				code.value += ((u16)tmp << i);	// the device code 
		}
		return (code.value);
	}

	else	// std=4 -> RC-5;

	{

		for(i=0; i<13; i++)
		{
			if(bit_is_clear(IR_PORT-2, IR_BIT) )
				T2 = 0;
			else
				T2 = 1;

			timer0_start();
			while(1)
			{
				time=inp(TCNT0);
				if(time > 0x21)
					return 0;

				if(bit_is_clear(IR_PORT-2, IR_BIT) && (T2==1) )
				{
					tmp <<= 1;
					tmp++;
					break;
				}
				else if(bit_is_set(IR_PORT-2, IR_BIT) && (T2==0) )
				{
					tmp <<= 1;
					break;
				}
			}

			//save address data
			if(i == 6)
			{
				code.bytes.high = (tmp & 0x5f);		// save address and cut troggle bit
				tmp=0;
			}

			//delay
			timer0_start();
			while(1)
			{
				time=inp(TCNT0);
				if(time > 0x21)
					break;
			}
		}
		code.bytes.low = tmp;
		return(code.value);
	}
}

//---------------------------------------------------------------------------------------

static u08 ir0[]  __attribute__ ((progmem)) = "REC80 ";
static u08 ir1[]  __attribute__ ((progmem)) = "NEC80 ";
static u08 ir2[]  __attribute__ ((progmem)) = "SONY15";
static u08 ir3[]  __attribute__ ((progmem)) = "SONY12";
static u08 ir4[]  __attribute__ ((progmem)) = "RC-5  ";

u08 *standard_name[] = { (u08*)&ir0, (u08*)&ir1, (u08*)&ir2, (u08*)&ir3, (u08*)&ir4 };

//---------------------------------------------------------------------------------------

void remote_setup(void)
{
	u08 i=5;
	
	std = 0;
	lcd_clrscr();
	PRINT_p(PSTR("\r\n\nRemote setup:\r\n"));
	PRINT_p(PSTR("Now trying recognize remote standard,\r\nPlease press several times any key in your remote controller.\r\n\n"));
	lcd_puts_p(PSTR("Stand: 1 ?"));
	PRINT_p(PSTR("Checking standard 1"));
	while(1)					// Recognize IR standard
	{
		WDR;
		if (rec80_active())
		{
			lcd_gotoxy(7,0);
			if (get_rec80())		// If != 0  - found standard
			{
					PRINT_p(PSTR("\r\nYour remote work in standard "));
					UART_SendByte(std+'1');
					PRINT_p(PSTR("  =  "));
					PRINT_p(standard_name[std]);
					lcd_putchar(std+'1');
					lcd_putchar(' ');
					lcd_puts_p(standard_name[std]);
					delay10(60000);
					break;
			}
			else
			{
				if(--i == 0)
				{
					std = (std+1) % 5;
					i=5;
					PRINT_p(PSTR("\rChecking standard "));
					UART_SendByte(std+'1');
					lcd_putchar(std+'1');
				}
//				while(rec80_active());
			}
		}
	}
	
	PRINT_p(PSTR("\r\n\nNow you may setup remote key codes.\r\n"));
	lcd_gotoxy(0,1);
	lcd_puts_p(PSTR("IR-Code: "));
	union u16convert code;
	u16 OldCode = 0;
	while(1)
	{
		WDR;
		if (rec80_active())
		{
			if ((code.value = get_rec80()))
			{
				if(code.value != OldCode)
				{
					lcd_gotoxy(9,1);
					lcd_hexB(code.bytes.high);
					lcd_hexB(code.bytes.low);
					PRINT_p(PSTR("IR-Code: "));
					UART_Printfu16(code.value);
					EOL();
					OldCode = code.value;
				}
			}
		}
	}
}


#endif // #ifndef SETUP_REMOTE_CODES

#endif // ENABLE_IR
