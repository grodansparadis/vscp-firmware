 /**
 * @brief           Interrupt driven Serial receive and transmit handler for USART2
 * @file            busser2.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_busser2
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

#define THIS_IS_BUSSER2

#include "projdefs.h"
#include "busser2.h"


//Get our bus id. This is defined in buses.h. It needed to access TX and RX buffers that are
//defined in the buses module
#define BUSID BUSID_SER2

/**
 * Initialise the busser2 module. After calling this function, the ser2Enable() function must be
 * called to enable the serial port.
 */
void ser2Init(void) {
    //xxxx x1xx - High BRG Baud rata
    //xxx0 xxxx - Async mode
    //xx0x xxxx - TX disable
    //x0xx xxxx - 8-bit mode
    TXSTA2 = 0b00000100;     //High BRG speed
    
    //xxxx 0xxx - Disable address detection
    //xxx0 xxxx - Disable receiver
    //x0xx xxxx - 8-bit receive
    //0xxx xxxx - Serial port disable
    RCSTA2 = 0b00010000;

    //Interrupts
    PIE3_RC2IE = 0;   //Disable USART Receive interrupt
    PIE3_TX2IE = 0;   //Disable USART Transmit interrupt - is only enabled once a TXion is started
}


/**
 * Enables the serial port AND . Can only be called after ser2Init() has been called.
 */
void ser2Enable(void) {
    TXSTA2_TXEN = 1;     //Transmit enable, this also sets TXIF flag!!!!
    RCSTA2_CREN = 1;     //Receive Enable
    RCSTA2_SPEN = 1;     //Serial port enable
    
    //Interrupts
    PIE3_RC2IE = 1;   //Enable USART Receive interrupt
    
    busEnable(BUSID);
}


/**
 * Disables the serial port
 */
void ser2Disable(void) {
    TXSTA2_TXEN = 0;     //Transmit disable
    RCSTA2_CREN = 0;     //Receive disable
    RCSTA2_SPEN = 0;     //Serial port disable
    
    //Disable Interrupts
    PIE3_RC2IE = 0;
    PIE3_TX2IE = 0;
    
    //Disable Bus - set disable status bit
    busDisabled(BUSID);
    
}


/**
 * Resets this module, and empties all buffers.
 */
void ser2Reset(void) {
    //Set transmit buffer to empty
    busEmptyTxBuf(BUSID);

    //Set receive buffer to empty
    busEmptyRxBuf(BUSID);
}


/**
 * Receive ISR function
 */
void ser2RxIsr(void) {
    //Check for "overun error" (OERR bit 1) or "Framming Error" (FERR bit 2).
    if (RCSTA2 & 0x06)
    {
        RCSTA2_CREN = 0;
        RCSTA2_CREN = 1;
    }

    //Add received byte to buffer
    while (PIR3_RC2IF) {
        if (!busIsRxBufFull(BUSID)) {
            //Add byte to RX buffer, and update buffer pointers
            busPutByteRxBuf(BUSID, RCREG2);
        }
        else {
            busInfo.stat[BUSID] |= BUSSTAT_RX_OVERRUN;

            //Read byte to clear RCIF. Read to EEDATA just because EEDATA is not used. If we declare a temp byte
            //in this function, and read RCREG to it, code optimization will remove that code!
            EEDATA = RCREG2;
        }
    }
}


/**
 * Transmit ISR function. This function should be called from the ISR if "(TXIF && TXIE)" evaluates to true.
 */
void ser2TxIsr(void)
{
    //Transmit next byte contained in transmit buffer. If whole buffer has been transmitted, disable
    //transmit interrupts.
    if (busIsTxBufEmpty(BUSID)) {
        //Clear TXING flag when nothing more to TX
        busInfo.stat[BUSID] &= ~BUSSTAT_TXING;
        
        //Clear TXIE too. This must be done because TXIF is set as long as TXREG is empty. TXIF is only cleared
        //by writing to TXREG. In the ISR we only call serTxIsr if both TXIE and TXIF are set.
        PIE3_TX2IE = 0;
    }
    else {
        TXREG2 = busPeekByteTxBuf(BUSID);
        busRemoveByteTxBuf(BUSID);
    }
}


/**
 * Service routine. Must be regulary called by main program (NOT IN ISR!).
 * It does the following:
 * - Transmits anything contained in transmit buffer
 * - Check if receive bufer contains anything.
 */
void ser2Service(void) {
    BYTE c;

    //If we are not currently TXing, check if there is data to transmit in the TX buffer
    if (!busIsTxing(BUSID))
    {
        //Transmit next byte contained in transmit buffer. If whole buffer has been transmitted, disable
        //transmit interrupts.
        if (busTxBufHasData(BUSID)) {
            c = busPeekByteTxBuf(BUSID);
            busRemoveByteTxBuf(BUSID);

            //Enter critical section
            DISBALE_INTERRUPTS();
            TXREG2 = c;

            //Set TXING flag to indicate that we are currently TXing
            busInfo.stat[BUSID] |= BUSSTAT_TXING;
            
            //Set TXIE. This must be done because TXIF is set as long as TXREG is empty. TXIF is only cleared
            //by writing to TXREG. By setting TXIE we enable serTxIsr to be called from the ISR.
            PIE3_TX2IE = 1;
            
            //Leave critical section
            ENABLE_INTERRUPTS();
        }
    }
}


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * ser2IsGetReady() function should be called to check if there is any
 * data available on the serial port.
 *
 * @preCondition ser2IsGetReady() must be called before calling this function to
 * confirm the receive buffer has data! 
 *
 * @return Returns byte received via the serial port.
 */
BYTE ser2GetByte(void) {
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
void ser2PutByte(BYTE c) {
    //Check if buffer is full.
    #ifdef SER_WAIT_FOR_TXBUF
    //Wait until a byte is transmitted by the interrupt routine and buffer has place again.
    while (busIsTxBufFull(BUSID)) {
        FAST_USER_PROCESS();
    }
    #else
    if (busIsTxBufFull(BUSID)) {
        ser2Stat |= BUSSTAT_TX_OVERRUN;
        return;
    }
    #endif

    //Enter critical section
    DISBALE_INTERRUPTS();

    //If we are not currently TXing, the TX buffer will be empty. No need to transmit via buffer,
    //just write direct to TXREG2
    if (!busIsTxing(BUSID))
    {
        TXREG2 = c;      //Send byte
        
        //Set TXING flag to indicate that we are currently TXing
        busInfo.stat[BUSID] |= BUSSTAT_TXING;

        //Set TXIE. This must be done because TXIF is set as long as TXREG is empty. TXIF is only cleared
        //by writing to TXREG. By setting TXIE we enable serTxIsr to be called from the ISR.
        PIE3_TX2IE = 1;  //Indicate that we are currently TXing
        
        ENABLE_INTERRUPTS();
    }
    //We are currently TXing. This means that the TXIF will soon be set after the current byte
    //has been transmitted, and the TX buffer will be checked for any unsent data. Add current
    //byte to TX buffer.
    else {
        //Add byte to TX buffer, and update buffer pointers
        busPutByteTxBuf(BUSID, c);
        
        ENABLE_INTERRUPTS();
    }
}

/**
 * Send the ASCII hex value of the given byte to the USART. It is added to the transmit buffer, and
 * asynchronously transmitted. For example, if c=11, then "0B" will be sent to the USART
 *
 * @param c     Byte to write out on the serial port
 */
void ser2PutByteHex(BYTE c) {
    ser2PutByte( (c > 0x9f) ? ((c>>4)&0x0f)+0x57 : ((c>>4)&0x0f)+'0' );
    ser2PutByte( ((c&0x0f)>9) ? (c&0x0f)+0x57 : (c&0x0f)+'0' );
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param s     Null terminated string to write out on the serial port
 */
void ser2PutString(BYTE* s) {
    char c;

    while((c = *s++)) {
        ser2PutByte(c);
        FAST_USER_PROCESS();
    }
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT transmitted.
 *
 * @param str   Null terminated string to write out on the serial port
 */
void ser2PutRomString(ROM char* s) {
    char c;

    while((c = *s++)) {
        ser2PutByte(c);
        FAST_USER_PROCESS();
    }
}
