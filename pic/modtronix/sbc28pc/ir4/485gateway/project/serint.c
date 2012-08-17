/*
 * Interrupt driven Serial receive and transmit handler.
 *
 * Contains a receive and transmit buffer. All bytes received via the USART are placed in the
 * receive buffer. All bytes that have to be transmitted, are placed in the transmit buffer.
 *
 */
#define THIS_IS_SERINT

#include "serint.h"

//Transmit buffer - contains data that has to be transmitted
#define SER_TXBUF_MASK ( SER_TXBUF_SIZE - 1 )
static BYTE txBuf[ SER_TXBUF_SIZE ];

//Receive buffer - contains received data
#define SER_RXBUF_MASK ( SER_RXBUF_SIZE - 1 )
static BYTE rxBuf[ SER_RXBUF_SIZE ];

// RX Buffer
volatile BYTE idxSerRxbufGet;   		// get index
volatile BYTE idxSerRxbufPut;   		// put index

// TX Buffer
volatile BYTE idxSerTxbufGet;   		// get index
volatile BYTE idxSerTxbufPut;   		// put index

static BYTE serStat;


///////////////////////////////////////////////////////////////////////////////
// serInit
//
// Initialise the serint module. After calling this function, the serEnable() 
// function must be called to enable the serial port.
//
 
void serInit( void ) 
{
    serStat = 0;

    // Set receive buffer to empty
    idxSerRxbufGet = 0;
    idxSerRxbufPut = 0;
    
    // Set transmit buffer to empty
    idxSerTxbufGet = 0;
	idxSerTxbufPut = 0;

    // Serial port configuration
    TXSTA = 0b00100100; 	// 8 bits, TX enable, Asynch mode, high BRG speed
    RCSTA = 0b10010000; 	// USART Enable, 8 bits, RX enable, Disable address detection
    SPBRG = SPBRG_VAL;  	// Set BAUD rate

    // Configure port direction
    TRISC_RC6 = 0;  // Set RC6 as output
    TRISC_RC7 = 1;  // Set RC7 as input

    // Interrupts
    PIE1_RCIE = 0;  // Disable USART Receive interrupt
    PIE1_TXIE = 0;  // Disable USART Transmit interrupt - 
    				// 		is only enabled once a TXion is started
}

///////////////////////////////////////////////////////////////////////////////
// serEnable
//
// Enables the serial port AND . Can only be called after serInit() has been called.
//

void serEnable( void ) {
    //Interrupts
    PIE1_RCIE = 1;  // Enable USART Receive interrupt
    PIE1_TXIE = 0;  // Disable USART Transmit interrupt - 
    				// 		is only enabled once a TXion is started
}

///////////////////////////////////////////////////////////////////////////////
// serReset
//
// Resets this module, and empties all buffers.
//
 
void serReset( void ) 
{
    // Set transmit buffer to empty
    idxSerTxbufGet = idxSerTxbufPut = 0;

    // Set receive buffer to empty
    idxSerRxbufGet = idxSerRxbufPut = 0;
}


///////////////////////////////////////////////////////////////////////////////
// serRxIsr
//
// Receive ISR function
//
 
void serRxIsr( void ) 
{
	// Check for any error, and clear it if there is any.
    if ( RCSTA & 0x06 ) {
        RCSTA_CREN = 0;
        RCSTA_CREN = 1;
    }

    // Add received byte to buffer
    while ( PIR1_RCIF ) {
        rxBuf[ idxSerRxbufPut ] = RCREG;	// Get received byte. 
        									// RCIF is cleared once there are no more bytes left
        idxSerRxbufPut = ( ++idxSerRxbufPut ) & SER_RXBUF_MASK;
    }
}


///////////////////////////////////////////////////////////////////////////////
// serTxIsr
//
// Transmit ISR function.
// This function should be called from the ISR if "(TXIF && TXIE)" evaluates to true.
//
 
void serTxIsr( void )
{
    // Transmit next byte contained in transmit buffer. If whole buffer has been transmitted, 
    // disable transmit interrupts.
    if ( idxSerTxbufPut != idxSerTxbufGet ) {
        TXREG = txBuf[ idxSerTxbufGet ];
        idxSerTxbufGet = ( ++idxSerTxbufGet ) & SER_TXBUF_MASK;
    }
    else {
        PIE1_TXIE = 0;   // Disable USART Transmit interrupt when nothing more to TX
    }
}


///////////////////////////////////////////////////////////////////////////////
// serService
//
// Service routine. Must be regulary called by main program (NOT IN ISR!).
// It does the following:
// - Check if receive bufer contains anything
 
void serService( void ) 
{
	;
}

///////////////////////////////////////////////////////////////////////////////
// serGetStat
//

BYTE serGetStat( void ) 
{
    return serStat;
}

///////////////////////////////////////////////////////////////////////////////
// serGetByte
//

BYTE serGetByte( void ) 
{
    BYTE c = rxBuf[ idxSerRxbufGet ];
    idxSerRxbufGet = ( ++idxSerRxbufGet ) & SER_RXBUF_MASK;
    return c;
}


///////////////////////////////////////////////////////////////////////////////
// serPutByte
//

void serPutByte( BYTE c ) 
{

#ifdef SER_WAIT_FOR_TXBUF

    // Check if buffer is full. If this is the case, wait until a byte is transmitted by the
    // interrupt routine and buffer has place again.
    while ( ( ( idxSerTxbufPut + 1 ) & SER_TXBUF_MASK ) == idxSerTxbufGet ) {
        CLRWDT();
    }

#endif

    // Enter critical section
    DISBALE_INTERRUPTS();

    // If we are not currently transmitting, the TX buffer will be empty. 
    // 	No need to transmit via buffer,
    // just write direct to TXREG
    if ( !PIE1_TXIE ) {
        TXREG = c;      // Send byte
        PIE1_TXIE = 1;  // Indicate that we are currently TXing
        ENABLE_INTERRUPTS();
    }
    // We are currently trsanmitting. This means that the TXIF will soon be set after the current byte
    // has been transmitted, and the TX buffer will be checked for any unsent data. Add current
    // byte to TX buffer.
    else {
        // Seeing that an TX interrupt can happen any time during this function, only update idxTxbufPut
        // at the end of the routine
        txBuf[ idxSerTxbufPut ] = c;

        idxSerTxbufPut = ( ++idxSerTxbufPut ) & SER_TXBUF_MASK;
        ENABLE_INTERRUPTS();
    }
}

///////////////////////////////////////////////////////////////////////////////
// serPutString
//

void serPutString( BYTE* s ) 
{
    char c;

    while ( ( c = *s++ ) ) {
        serPutByte( c );
    }
}

///////////////////////////////////////////////////////////////////////////////
// serPutRomString
//

void serPutRomString( ROM char* s ) 
{
    char c;

    while( ( c = *s++ ) ) {
        serPutByte( c );
    }
}


///////////////////////////////////////////////////////////////////////////////
// serGetRxBufFree
//

BYTE serGetRxBufFree( void ) 
{
	if ( idxSerRxbufGet > idxSerRxbufPut ) {
		return  ( idxSerRxbufGet - idxSerRxbufPut );
	}
	else {
		return 	( idxSerRxbufPut - idxSerRxbufGet );
	}
}


///////////////////////////////////////////////////////////////////////////////
// serGetTxBufFree
//

BYTE serGetTxBufFree( void ) 
{
	if ( idxSerTxbufGet > idxSerTxbufPut ) {
		return  ( idxSerTxbufGet - idxSerTxbufPut );
	}
	else {
		return 	( idxSerTxbufPut - idxSerTxbufGet );
	}
}
