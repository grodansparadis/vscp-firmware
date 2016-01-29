/*
 *	Interrupt driven Serial receive and transmit handler.
 *
 *  To use this module:
 *  - Customize defines specified in "Serinit Configuration".
 *  - Call serInit() to initialize, but NOT enable module
 *  - Call serEnable() to enable module
 *  - Module is now active. Use funtions to write and read serial port.
 */

/*******************************************************
-------------- Serint Configuration --------------------
********************************************************
The following should be defined in the projdefs.h file OR on the command line
#define SER_RXBUF_SIZE  32  //Size of Rx buffer, must be 8,16,32,64,128 or 256
#define SER_TXBUF_SIZE  32	//Size of TX buffer, must be 8,16,32,64,128 or 256
//Comment this line if the transmit routines should NOT wait for the bytes to be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF
//Uncomment this line if the application does NOT configure the USART
//#define BAUD_RATE	9600	//USART BAUD rate
//Comment this line if the application does NOT configures the USART
#define APP_CONFIGURES_SERPORT  //Our application does all serial port configuration!
********************************************************/

#ifndef _SERINT_H_
#define _SERINT_H_

#include "projdefs.h"

//ANSI commands
static ROM char AnsiEscClearScreen[] = "\x1B[2J";

/////////////////////////////////////////////////
//Global defines
#define SER_USE_INTERRUPT

#ifndef SER_RXBUF_SIZE
#define SER_RXBUF_SIZE  32  //Size of Rx buffer's data area
#endif

#ifndef SER_TXBUF_SIZE
#define SER_TXBUF_SIZE  32  //Size of Tx buffer's data area
#endif


/////////////////////////////////////////////////
//USART CONFIG - only done if application does not do it
#if !defined(APP_CONFIGURES_SERPORT)
    #define USART_USE_BRGH_LOW
    #if defined(USART_USE_BRGH_LOW)
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
    #else
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
    #endif

    #if SPBRG_VAL > 255
        #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
    #endif
#endif


/////////////////////////////////////////////////
//serStat defines
#define SERINT_RXBUF_OVERRUN	0x01	//The receive buffer had overrun. Must be cleared by the user
#define SERINT_TXBUF_OVERRUN	0x10	//The transmit buffer had overrun. Must be cleared by the user


/////////////////////////////////////////////////
//Global data
extern BYTE idxSerRxbufGet;   //get index
extern BYTE idxSerRxbufPut;   //put index


/////////////////////////////////////////////////
//Function prototypes


/**
 * Called in the USART TX ISR
 */
void serTxIsr(void);

/**
 * Called in the USART RX ISR
 */
void serRxIsr(void);

/**
 * Service this module
 */
void serService(void);

/**
 * Initialize this module
 */
void serInit(void);

/**
 * Enables the serial port AND . Can only be called after serInit() has been called.
 */
void serEnable(void);

/**
 * Resets this module, and empties all buffers.
 */
void serReset(void);

/**
 * Get the current status of this module
 * @return The status of this module. Consists out of SER_STAT_XXX flags
 */
BYTE serGetStat(void);

/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define serRxBufEmpty() (idxSerRxbufPut == idxSerRxbufGet)

/**
 * Are there any bytes in the receive buffer.
 * @return 1 if true, else 0
 */
#define serIsGetReady() (idxSerRxbufPut != idxSerRxbufGet)

/**
 * Get the next byte in the RX buffer.
 */
BYTE serGetByte(void);

/**
 * Transmit the given byte on the USART. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutByte(BYTE c);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutString(BYTE* s);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutRomString(ROM char* str);


#endif	//_SERINT_H_




