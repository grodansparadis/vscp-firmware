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
#define SER_TXBUF_MASK (SER_TXBUF_SIZE-1)
static BYTE txBuf[SER_TXBUF_SIZE];
static BYTE idxTxbufGet;   //get index
static BYTE idxTxbufPut;   //put index

//Receive buffer - contains received data
#define SER_RXBUF_MASK (SER_RXBUF_SIZE-1)
static BYTE rxBuf[SER_RXBUF_SIZE];
BYTE idxSerRxbufGet;   //get index
BYTE idxSerRxbufPut;   //put index

static BYTE serStat;


/**
 * Initialise the serint module. After calling this function, the serEnable() function must be
 * called to enable the serial port.
 */
void serInit(void) {
    serStat = 0;

    //Set transmit buffer to empty
    idxTxbufGet = 0;
    idxTxbufPut = 0;

    //Set receive buffer to empty
    idxSerRxbufGet = 0;
    idxSerRxbufPut = 0;

    //This configuration is done by the application for this project!!!
    #if !defined(APP_CONFIGURES_SERPORT)
    //Serial port configuration
    TXSTA = 0b00100000; //8 bits, TX enable, Asynch mode, low BRG speed
    RCSTA = 0b10010000; //USART Enable, 8 bits, RX enable, Disable address detection
    SPBRG = SPBRG_VAL;  //Set BAUD rate

    //Configure port direction
    TRISC_RC6 = 0;  //Set RC6 as output
    TRISC_RC7 = 1;  //Set RC7 as input
    #endif

    //Interrupts
    PIE1_RCIE = 0;   //Disable USART Receive interrupt
    PIE1_TXIE = 0;   //Disable USART Transmit interrupt - is only enabled once a TXion is started
}

/**
 * Enables the serial port AND . Can only be called after serInit() has been called.
 */
void serEnable(void) {
    //Interrupts
    PIE1_RCIE = 1;   //Enable USART Receive interrupt
    PIE1_TXIE = 0;   //Disable USART Transmit interrupt - is only enabled once a TXion is started
}

/**
 * Resets this module, and empties all buffers.
 */
void serReset(void) {
    //Set transmit buffer to empty
    idxTxbufGet = idxTxbufPut = 0;

    //Set receive buffer to empty
    idxSerRxbufGet = idxSerRxbufPut = 0;
}


/**
 * Receive ISR function
 */
void serRxIsr(void) {
    //Check for any error, and clear it if there is any.
    if (RCSTA & 0x06)
    {
        RCSTA_CREN = 0;
        RCSTA_CREN = 1;
    }

    //Add received byte to buffer
    while (PIR1_RCIF) {
        rxBuf[idxSerRxbufPut] = RCREG;    //Get received byte. RCIF is cleared once there are no more bytes left
        idxSerRxbufPut = (++idxSerRxbufPut) & SER_RXBUF_MASK;
    }
}

/**
 * Transmit ISR function. This function should be called from the ISR if "(TXIF && TXIE)" evaluates to true.
 */
void serTxIsr(void)
{
    //Transmit next byte contained in transmit buffer. If whole buffer has been transmitted, disable
    //transmit interrupts.
    if (idxTxbufPut != idxTxbufGet) {
        TXREG = txBuf[idxTxbufGet];
        idxTxbufGet = (++idxTxbufGet) & SER_TXBUF_MASK;
    }
    else {
        PIE1_TXIE = 0;   //Disable USART Transmit interrupt when nothing more to TX
    }
}

/**
 * Service routine. Must be regulary called by main program (NOT IN ISR!).
 * It does the following:
 * - Check if receive bufer contains anything.
 */
void serService(void) {
}

BYTE serGetStat(void) {
    return serStat;
}

BYTE serGetByte(void) {
    BYTE c = rxBuf[idxSerRxbufGet];
    idxSerRxbufGet = (++idxSerRxbufGet) & SER_RXBUF_MASK;
    return c;
}


void serPutByte(BYTE c) {
    #ifdef SER_WAIT_FOR_TXBUF
    //Check if buffer is full. If this is the case, wait until a byte is transmitted by the
    //interrupt routine and buffer has place again.
    while ( ((idxTxbufPut + 1) & SER_TXBUF_MASK) == idxTxbufGet) {
        CLRWDT();
    }
    #endif

    //Enter critical section
    DISBALE_INTERRUPTS();

    //If we are not currently TXing, the TX buffer will be empty. No need to transmit via buffer,
    //just write direct to TXREG
    if ( !PIE1_TXIE)
    {
        TXREG = c;      //Send byte
        PIE1_TXIE = 1;  //Indicate that we are currently TXing
        ENABLE_INTERRUPTS();
    }
    //We are currently TXing. This means that the TXIF will soon be set after the current byte
    //has been transmitted, and the TX buffer will be checked for any unsent data. Add current
    //byte to TX buffer.
    else {
        //Seeing that an TX interrupt can happen any time during this function, only update idxTxbufPut
        //at the end of the routine
        txBuf[idxTxbufPut] = c;

        idxTxbufPut = (++idxTxbufPut) & SER_TXBUF_MASK;
        ENABLE_INTERRUPTS();
    }
}

void serPutString(BYTE* s) {
    char c;

    while((c = *s++)) {
        serPutByte(c);
    }
}


void serPutRomString(ROM char* s) {
    char c;

    while((c = *s++)) {
        serPutByte(c);
    }
}