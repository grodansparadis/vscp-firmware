
#include <io.h>
#include "hal_lcd.h"
#include "timer.h"

static uint8 lcdX, lcdY;

const uint8 pfeilAbChar[8] = { 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00 };
const uint8 pfeilAufChar[8] = { 0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00 };
const uint8 menuPfeilChar[8] = { 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08, 0x00 };


/*************************************************************/
/*                      LOCAL FUNCTIONS                      */
/*************************************************************/


#if LCD_IO_MODE
static void halLcdWrite(uint8 data,uint8 rs) 
{
    if (rs) sbi(LCD_RS_PORT, LCD_RS_PIN);
    else    cbi(LCD_RS_PORT, LCD_RS_PIN);
    /* all data pins high (inactive) */
    outp(LCD_PORT_MASK, LCD_DATA_PORT);
    /* configure data pins as output */
    outp(LCD_PORT_MASK, DDR(LCD_DATA_PORT));
    /* write mode */
    cbi(LCD_RW_PORT, LCD_RW_PIN);
    /* write command/data */
    outp(data, LCD_DATA_PORT);
    /* toggle enable line */
    halLcdE_high();
    halLcdE_low();
}
#else							 
#define halLcdWrite(d,rs) if (rs) *(volatile uint8*)(LCD_IO_DATA) = d; else *(volatile uint8*)(LCD_IO_FUNCTION) = d;
/*#define halLcdwrite(d,rs) outp( (d), (rs) ? LCD_IO_DATA : LCD_IO_FUNCTION )*/
/* rs==0 -> write instruction to LCD_IO_FUNCTION */
/* rs==1 -> write data to LCD_IO_DATA */
#endif


#if LCD_IO_MODE
static uint8 halLcdRead(uint8 rs) 
{
    register uint8 data;

    if (rs) sbi(LCD_RS_PORT, LCD_RS_PIN);
    else    cbi(LCD_RS_PORT, LCD_RS_PIN);
    /* configure data pins as input */
    outp(~LCD_PORT_MASK, DDR(LCD_DATA_PORT));
    /* read mode */
    sbi(LCD_RW_PORT, LCD_RW_PIN);
    /* toggle enable line */
    halLcdE_high();
    /* read command/data */
    data = inp(PIN(LCD_DATA_PORT));
    halLcdE_low();
    return data;
}
#else
#define halLcdRead(rs) (rs) ? *(volatile uint8*)(LCD_IO_DATA) : *(volatile uint8*)(LCD_IO_FUNCTION)
/*#define halLcdread(rs) inp( (rs) ? LCD_IO_DATA : LCD_IO_FUNCTION ) */
/* rs==0 -> read instruction from LCD_IO_FUNCTION */
/* rs==1 -> read data from LCD_IO_DATA */
#endif


static void halLcdWaitbusy(void)
/* loops while lcd is busy */
{
    while (halLcdRead(0) & (1 << LCD_BUSY)) {}
}


static void halLcdNewline(void)
/* goto start of next line */
{
    lcdX = 0;
	lcdY++;

    if (lcdY >= LCD_LINES)
        lcdY = 0;
}


static void halLcdGoto(void)
/* goto position (lcdX,lcdY) */
{
	uint8 i = 0;
	
	if ( (lcdY & 0x01) != 0 )
		i = 0x40;

	i += (lcdY >> 1) * LCD_LINE_LENGTH;
	
    halLcdCommand((1<<LCD_DDRAM)+i+lcdX);
}

static void halLcdGotocgram( uint8 pos )
{
    halLcdCommand((1<<LCD_CGRAM)+pos);
}


static void halLcdPutData( const uint8 data)
// send data to LCD
{
    halLcdWaitbusy();

	halLcdWrite(data, 1);     
}

/*************************************************************/
/*                     PUBLIC FUNCTIONS                      */
/*************************************************************/


void halLcdCommand(uint8 cmd)
/* send commando <cmd> to LCD */
{
    halLcdWaitbusy();
    halLcdWrite(cmd, 0);
}


void halLcdGotoXY(uint8 x, uint8 y)
/* goto position (x,y) */
{
    lcdX = x; lcdY = y;
    halLcdGoto();
}

void halLcdSetCursor( lcdCursorMode c )
{
    halLcdCommand( (1<<LCD_ON) | (1<<LCD_ON_DISPLAY) | (uint8)c );
}


void halLcdClrscr(void)
/* clear lcd */
{
    lcdX = lcdY = 0;
    halLcdCommand(1<<LCD_CLR);
    /*delay(1640);*/
}


void halLcdHome(void)
/* set cursor to home position */
{
    lcdX = lcdY = 0;
    halLcdCommand(1<<LCD_HOME);
    /*delay(1640);*/
}


void halLcdPutChar(uint8 data)
/* print character to current cursor position */
{
    halLcdWaitbusy();
    if (data=='\n') 
	{
        halLcdNewline();
        halLcdGoto();
    }
    else 
	{
        halLcdWrite(data, 1);

        if (++lcdX >= LCD_LINE_LENGTH) 
		{
            halLcdNewline();
            halLcdGoto();
        }
  }
}

void halLCDSetupUserCharacters()
{
	// Menu position indicator
	halLcdDefineChar( 0, menuPfeilChar );
	// Pfeil nach unten
	halLcdDefineChar( 1, pfeilAbChar );
	// Pfeil nach oben
	halLcdDefineChar( 2, pfeilAufChar );
}


void halLcdInit(lcdCursorMode cursor, uint8 fnc)
/* cursor:   0 = off, 2 = on, 3 = blinking */
/* fnc: see LCD_FUNCTION_xxx */
{
    uint8 wait[] = { 250, 78, 1, 1 };
    register uint8 i;

#if LCD_IO_MODE
    /* configure control line pins as output */
    sbi(DDR(LCD_E_PORT),  LCD_E_PIN);
    sbi(DDR(LCD_RS_PORT), LCD_RS_PIN);
    sbi(DDR(LCD_RW_PORT), LCD_RW_PIN);
#if LCD_HW_ENABLE
    /* configure RD line as output and low */
    sbi(DDR(PORTD), 7);
    cbi(PORTD,      7); 
#else
    /* set enable line high */
    sbi(LCD_E_PORT, LCD_E_PIN);
#endif    
#else
    /* enable external SRAM (memory mapped lcd) and wait states */
    outp((1<<SRE)|(1<<SRW), MCUCR);
#endif

    fnc |= (1<<LCD_FUNCTION);
    /* reset lcd */
    for (i=0; i<4; i++) {
        delay(wait[i]<<6);                     /* 16ms, 5ms, 64us, 64us */
        halLcdWrite(fnc, 0);                     /* reset function */    
    }
    halLcdCommand(1<<LCD_ON);
    halLcdClrscr();
    halLcdCommand(LCD_MODE_DEFAULT);
    halLcdCommand( (1<<LCD_ON) | (1<<LCD_ON_DISPLAY) | (uint8)cursor );
} 


void halLcdPuts( const char s[])
/* print string on lcd (no auto linefeed) */
{
    if ( s == 0 ) return;

    while ( s[0] != 0 ) 
    {
        halLcdPutChar( s[0] );    
        s++;
    }
}

void halLcdDefineChar( uint8 nr, const uint8 data[8] )
{
	register uint8 i;

	halLcdGotocgram( nr << 3 );

	for ( i = 0; i < 8; i++ )
		halLcdPutData( data[i] );

	halLcdGoto();
}

char* int2str(uint16 i, char *s, uint8 d)
/* connverts 16 bit unsigned integer to string */
{
    register uint16 divisor;
    register uint8 n;
    register char c;

	if ( i == 0 )
		return "0";

	if ( d == 1 )
	{
		s[0] = (char)i + '0';
		n = 1;
	}
	else
	{
		divisor = 10; 
		for ( n = 1; n < d; n++ )
			divisor *= 10;

		n = 0;

		do {
			c = (char)(i/divisor);
			if ( (c > 0) | (n > 0) ) 
				s[n++] = c + '0';
			i -= c*divisor;
			divisor /= 10;
		} while (divisor);
	}
    s[n] = 0;

    return s;
}

void int2hex( char * buf, uint8 i )
{
	uint8 ch;

	ch = (i & 0xF0) >> 4;
	
	if ( ch < 10 )
		buf[0] = ch + '0';
	else
		buf[0] = ch - 10 + 'A';

	i &= 0x0F;
	if ( i < 10 )
		buf[1] = i + '0';
	else
		buf[1] = i - 10 + 'A';
}

void delay(uint16 us) 
{
    uint16 delay_loops;
    register uint16 i;

    delay_loops = (us + 3) / 5  *CYCLES_PER_US; /* +3 for rounding up (dirty) */
    /* one loop takes 5 cpu cycles */

    for (i = 0; i < delay_loops; i++);
} 
                             