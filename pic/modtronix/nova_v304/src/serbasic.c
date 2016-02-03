/*********************************************************************
 * serbasic.c    -  Simple USART1 routines
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Simple USART1 routines.
 *
 *********************************************************************
 * File History
 *
 * 2005-01-07, David Hosken (DH):
 *    - Initial version
 *
 *********************************************************************
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
 *
 ********************************************************************/

#include "projdefs.h"


/**
 * Send the given byte to the USART. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutByte(BYTE c)
{
    while( !TXSTA_TRMT);
    TXREG = c;
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
void serPutRomStringAndNull(ROM char* str) {
    BYTE c;

    do {
        c = *s++;
        serPutByte(c);
    } while (c);
}

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is ALSO sent!
 *
 * @param str   Null terminated string to write out on the serial port
 */
void serPutStringAndNull(BYTE* str) {
    BYTE c;

    do {
        c = *s++;
        serPutByte(c);
    } while (c);
}
#endif


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 */
void serPutString(BYTE* s)
{
    BYTE c;

    while( (c = *s++) )
        serPutByte(c);
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutRomString(ROM char* str)
{
    BYTE v;

    while( v = *str++ )
        serPutByte(v);
}
