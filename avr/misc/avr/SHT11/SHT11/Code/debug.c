#ifdef DEBUG

#include "debug.h"

static volatile const uint8 	*pu8TxDataPtr;
static volatile uint8 			u8TxCounter;

void _Assert(const char *cpArg, const char *cpFile, int16 nLine)
{
	debug_str( "\r\nAssert failure: " );
	debug_str( cpArg );
	debug_str( "\r\nIn " );
	debug_str( cpFile );
	debug_str( " at line " );
	debug_u16( nLine );
	debug_str( "!\r\n" );

	// stop here
	while (1);
}

void debug_init( eBaudRate br )
{
    /* enable TxD and ints */
    // outp( (1 << RXCIE) | (1 << TXCIE) | (1 << RXEN) | (1 << TXEN), UCR );       
    outp( (1 << TXCIE) | (1 << TXEN), UCR );       

	/* set baud rate */
    outp( (uint8)br, UBRR);  
	u8TxCounter = 0;
}

void debug_str( const char * str )
{
	uartSendStr( str );
	
	// Wait for the packet to be sent completely
	uartSendStr( NULL );
}

void debug_u8( const uint8 i )
{
	char str[] = "0x-0";

	u8tohexStr( i, str + 2);
	uartSendStr( str );

	// Wait for the packet to be sent completely
	uartSendStr( NULL );
}

void debug_u16( const uint16 i )
{
	char str[] = "0x--00";

	u16tohexStr( i, str + 2);
	uartSendStr( str );

	// Wait for the packet to be sent completely
	uartSendStr( NULL );
}

char * u16tohexStr( uint16 i, char * buf )
{
	int8 bufCnt;					// Buffer counter
	uint8 tempVal;

	bufCnt = 7;
	do
	{
		tempVal = (uint8)(i & 0x000F);
		i >>= 4;

		if ( tempVal < 10 )
			buf[(uint8)bufCnt] = tempVal + '0';
		else
			buf[(uint8)bufCnt] = tempVal + 'A' - 0x0A;

		bufCnt--;
	} while ( bufCnt >= 0 );

	return buf;
}

char * u8tohexStr( uint8 i, char * buf )
{
	int8 bufCnt;					// Buffer counter
	uint8 tempVal;

	bufCnt = 1;
	do
	{
		tempVal = i & 0x0F;
		i >>= 4;

		if ( tempVal < 10 )
			buf[(uint8)bufCnt] = tempVal + '0';
		else
			buf[(uint8)bufCnt] = tempVal + 'A' - 0x0A;

		bufCnt--;
	} while ( bufCnt >= 0 );

	return buf;
}
			
/* signal handler for uart txd ready interrupt */
SIGNAL(SIG_UART_TRANS)      
{
    pu8TxDataPtr++;

    if (--u8TxCounter)
        outp(*pu8TxDataPtr, UDR);       // write byte to data buffer 
}

void uartSendStr( const char * str )
{
	while ( u8TxCounter != 0 );

	if ( str != NULL )
	{
		/* write first byte to data buffer */
		pu8TxDataPtr  = str;
		u8TxCounter   = strlen( str );
		outp(*str, UDR);
	}
}

#endif
