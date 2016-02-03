 /**
 * @brief           Interrupt driven Serial receive and transmit handler.
 * @file            serint.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_serint
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#define THIS_IS_BUSSER1

#include "projdefs.h"
#include "busser1.h"


BYTE udp1Stat;
BYTE udp2Stat;


//Get our bus id. This is defined in buses.h. It needed to access TX and RX buffers that are
//defined in the buses module
#define BUSID BUSID_SER1

/**
 * Initialise the serint module. After calling this function, the serEnable() function must be
 * called to enable the serial port.
 */
void serInit(void) {
    serStat = 0;

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
    busEmptyTxBuf(BUSID);

    //Set receive buffer to empty
    busEmptyRxBuf(BUSID);
}


/**
 * Receive ISR function
 */
void serRxIsr(void) {
    //Check for "overun error" (OERR bit 1) or "Framming Error" (FERR bit 2).
    if (RCSTA & 0x06)
    {
        RCSTA_CREN = 0;
        RCSTA_CREN = 1;
    }

    //Add received byte to buffer
    while (PIR1_RCIF) {
        if (!busIsRxBufFull(BUSID)) {
            //Add byte to RX buffer, and update buffer pointers
            busPutByteRxBuf(BUSID, RCREG);
        }
        else {
            serStat |= SER1_RXBUF_OVERRUN;
        }
    }
}


/**
 * Transmit ISR function. This function should be called from the ISR if "(TXIF && TXIE)" evaluates to true.
 */
void serTxIsr(void)
{
    //Transmit next byte contained in transmit buffer. If whole buffer has been transmitted, disable
    //transmit interrupts.
    if (busIsTxBufEmpty(BUSID)) {
        PIE1_TXIE = 0;   //Disable USART Transmit interrupt when nothing more to TX
    }
    else {
        TXREG = busPeekByteTxBuf(BUSID);
        busRemoveByteTxBuf(BUSID);
    }
}


/**
 * Service routine. Must be regulary called by main program (NOT IN ISR!).
 * It does the following:
 * - Check if receive bufer contains anything.
 */
void serService(void) {
}


/**
 * Get the current status of this module. The following flags are cleared
 * during this function:
 * - SER1_TXBUF_OVERRUN
 * - SER1_RXBUF_OVERRUN
 *
 * @return The status of this module. Consists out of SER_STAT_XXX flags
 */
BYTE serGetStat(void) {

    //Clear SER1_TXBUF_OVERRUN and SER1_RXBUF_OVERRUN flags
    serStat = serStat & (~(SER1_TXBUF_OVERRUN | SER1_RXBUF_OVERRUN));

    return serStat;
}


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * serIsGetReady() function should be called to check if there is any
 * data available on the serial port.
 *
 * @preCondition serIsGetReady() must be called before calling this function to
 * confirm the receive buffer has data! 
 *
 * @return Returns byte received via the serial port.
 */
BYTE serGetByte(void) {
    BYTE c;

    if (busRxBufHasData(BUSID)) {
        c = busPeekByteRxBuf(BUSID);
        busRemoveByteRxBuf(BUSID);
    }
    
    return c;
}


/**
 * Send the given byte to the USART. It is added to the transmit buffer, and asynchronously
 * transmitted.
 *
 * @param c     Byte to write out on the serial port
 */
void serPutByte(BYTE c) {
    //Check if buffer is full.
    #ifdef SER_WAIT_FOR_TXBUF
    //Wait until a byte is transmitted by the interrupt routine and buffer has place again.
    while (busIsTxBufFull(BUSID)) {
        FAST_USER_PROCESS();
    }
    #else
    if (busIsTxBufFull(BUSID)) {
        serStat |= SER1_TXBUF_OVERRUN;
        return;
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
        //Add byte to TX buffer, and update buffer pointers
        busPutByteTxBuf(BUFID, c);
        
        ENABLE_INTERRUPTS();
    }
}

/**
 * Send the ASCII hex value of the given byte to the USART. It is added to the transmit buffer, and
 * asynchronously transmitted. For example, if c=11, then "0B" will be sent to the USART
 *
 * @param c     Byte to write out on the serial port
 */
void serPutByteHex(BYTE c) {
    serPutByte( (c > 0x9f) ? ((c>>4)&0x0f)+0x57 : ((c>>4)&0x0f)+'0' );
    serPutByte( ((c&0x0f)>9) ? (c&0x0f)+0x57 : (c&0x0f)+'0' );
}

//The following functions are only required for debugging
#if !defined(DEBUG_OFF)

/**
 * Sent the given two bytes via the USART.
 *
 * @param b1 The first byte to be sent.
 * @param b2 The second byte to be sent.
 */
void serPut2Bytes(BYTE b1, BYTE b2) {
    serPutByte(b1);
    serPutByte(b2);
}

/**
 * Sent the given debug message to the USART. The following is sent:
 * [debugCode][msgCode][msgString]
 *
 * @param debugCode The Debug Message's "Debug Code". This will be the first byte sent.
 * @param msgCode The Debug Message's "Message Code". This will be the second byte sent.
 * @param msgString The Debug Message's "Message String", has to be a NULL terminated string. This will be sent after the msgCode.
 */
void serPutDebugMsgRomStr(BYTE debugCode, BYTE msgCode, ROM char* msgString) {
    serPutByte(debugCode);
    serPutByte(msgCode);
    serPutRomString(msgString);
}

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is ALSO sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutRomStringAndNull(ROM char* s) {
    BYTE c;

    do {
        c = *s++;
        serPutByte(c);
        FAST_USER_PROCESS();
    } while (c);
}

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is ALSO sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutStringAndNull(BYTE* s) {
    BYTE c;

    do {
        c = *s++;
        serPutByte(c);
        FAST_USER_PROCESS();
    } while (c);
}
#endif


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param s     Null terminated string to write out on the serial port
 */
void serPutString(BYTE* s) {
    char c;

    while((c = *s++)) {
        serPutByte(c);
        FAST_USER_PROCESS();
    }
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT transmitted.
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutRomString(ROM char* s) {
    char c;

    while((c = *s++)) {
        serPutByte(c);
        FAST_USER_PROCESS();
    }
}
