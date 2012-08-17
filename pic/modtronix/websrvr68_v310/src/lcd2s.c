 /**
 * @brief           Serial LCD Display handler.
 * @file            lcd2s.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_lcd2s
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
 * 2007-07-16, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#define THIS_IS_LCD2S

#include "projdefs.h"
#include "lcd2s.h"
#include "net\i2c.h"
#include "net\tick.h"


//Keypad buffer - contains received keypad data
#define KPAD_BUF_SIZE_MASK (KPAD_BUF_SIZE-1)
static BYTE kpadBuf[LCD2S_MAX_NUMBER][KPAD_BUF_SIZE];
BYTE kpadBufGet[LCD2S_MAX_NUMBER];    //get index
BYTE kpadBufPut[LCD2S_MAX_NUMBER];    //put index


//LCD2S Commands
#define LCD2S_CMD_GET_STATUS    0xD0ul
#define LCD2S_CMD_GET_KEY       0xD1ul
#define LCD2S_CMD_GET_GPIO      0xD2ul

/**
 * Initialise the serint module. After calling this function, the serEnable() function must be
 * called to enable the serial port.
 */
void lcdInit(void) {
    BYTE i;
    
    //Empty all keypad buffers
    for (i=0; i<LCD2S_MAX_NUMBER; i++) {
        kpadBufGet[i] = 0;
        kpadBufPut[i] = 0;
    }
}

/**
 * Get a single byte from the LCD display.
 *
 * @param lcddNum The number of the lcd to read status byte from. Is a number from 0-n.
 *        If, for example, the system is configured for 4 keypads, it is a number from 0-3.
 *
 * @param cmd LCD command for byte to get, for example 0xD0 is status byte, 0xD1 is key, 0xD3 is GPIO bytes.....
 *
 * @param val Read byte is returned in this parameter
 *
 * @return Returns 0 if OK, 1 if no ACK received
 */
BYTE lcdGetByte(BYTE lcdNum, BYTE cmd, BYTE* val) {
    //Read LCD status byte
    i2cWaitForIdle();

    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete

    while(1) {
        //Write module address, lcdNum is multiplied by two because LSB bit is Read/Write bit! Must be 0 for write
        i2cPutByteAndWait(LCD2S_I2C_ADR + (lcdNum*2));

        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
        if(i2cWasAckReceived() == 0) break;

        //Write get status byte command
        i2cPutByteAndWait(cmd);

        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
        if(i2cWasAckReceived() == 0) break;

        //Send repeated I2C start condition
        i2cRestartAndWait();

        //Send module address, and read/write bit (LSB bit) set - this is a read message
        i2cPutByteAndWait(LCD2S_I2C_ADR + 1 + (lcdNum*2));

        //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
        if(i2cWasAckReceived() == 0) break;

        //Shift in byte from I2C port
        *val = i2cGetByte(0);   	//ACK each byte except for last one. Only one byte, so no ACK

        i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete

        return 0;   //Return OK
    }

    //Error, reset I2C bus.
    i2cPutStopAndWait();    //Reset I2C bus
    return 1;               //Return error
}


/**
 * Service routine. Must be regulary called by main program (NOT IN ISR!).
 * It does the following:
 * - Check if receive bufer contains anything.
 */
void lcdService(void) {
    static TICK8 tLcd = 0;
    static BYTE lcdNum=0; //Keypad to service
    BYTE c;

    //Enter every 40ms
    if ( TickGetDiff8bit(tLcd) >= ((TICK8)TICKS_PER_SECOND / (TICK8)25) )
    {
        tLcd = TickGet8bit();

        //Empty all keypad buffers
        //Get status byte from LCD
        if (lcdGetByte(lcdNum, LCD2S_CMD_GET_STATUS, &c) == 0) {
            //Has the LCD got keys to be read
            if (c & 0x80) {
                //Check if more space left in buffer - last byte of buffer can not be used, because that is when put=get, which indicates empty
                if ( ((kpadBufPut[lcdNum]+1)&KPAD_BUF_SIZE_MASK) != kpadBufGet[lcdNum]) {
                    //Get key from LCD
                    lcdGetByte(lcdNum, LCD2S_CMD_GET_KEY, &c);

                    //Add byte to kpadBuf, and increment put pointer
                    kpadBuf[lcdNum][ kpadBufPut[lcdNum]++ ] = c;
                    kpadBufPut[lcdNum] &= KPAD_BUF_SIZE_MASK;
                }
            }
        }

        //Increment keypad to service
        if (++lcdNum >= LCD2S_MAX_NUMBER) {
            lcdNum = 0; //Reset to first keypad
        }
    }
}


/**
 * Get the next key from the keypad buffer. Before calling this function, the
 * kpadHasKeys() function should be called to check if there are any
 * keys to be read from the keypad buffer.
 *
 * @param kpadNum The number of the keypad you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 keypads, it is a number from 0-3.
 *
 * @return Returns next key code from the keypad buffer.
 */
BYTE kpadGetKey(BYTE kpadNum) {
    BYTE ret;
    
    //Check kpadNum is a valid number
    if (kpadNum >= LCD2S_MAX_NUMBER)
        return 0;

    //Read byte from kpadBuf, and increment get pointer
    ret = kpadBuf[kpadNum][ kpadBufGet[kpadNum]++ ];
    kpadBufGet[kpadNum] &= KPAD_BUF_SIZE_MASK;
    return ret;
}


/**
 * Send the given byte to the LCD Display. It is appended to the current LCD display.
 * The following bytes have special meanings:
 * - 0x0a ('\n') = Go to beginning of next line.
 * - 0x0c ('\f') = Clear display and go to beginning of first line.
 * - 0x0d ('\r') = Go to beginning of first line
 * - 0x08 ('\b') = Cursor left
 * - 0x09 ('\t') = Cursor Right
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c     Byte to write out on the serial port
 */
void lcdPutByte(BYTE lcdNum, BYTE c) {
    
}


/**
 * Send the ASCII hex value of the given byte to the LCD Display. It is appended to the
 * current LCD Display. For example, if c=11, then "0B" will be sent to the LCD Display
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c     Byte to write out on the serial port
 */
void lcdPutByteHex(BYTE lcdNum, BYTE c) {
    lcdPutByte(lcdNum,  (c > 0x9f) ? ((c>>4)&0x0f)+0x57 : ((c>>4)&0x0f)+'0' );
    lcdPutByte(lcdNum,  ((c&0x0f)>9) ? (c&0x0f)+0x57 : (c&0x0f)+'0' );
}


/**
 * Writes given number of characters from given buffer to the LCD Display.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param s         Buffer containing data to send to LCD Display
 * @param len       Number of bytes to send from given buffer
 *
 * @return 0 if OK, else error
 */
BYTE lcdPutArray(BYTE lcdNum, BYTE* s, BYTE len) {
    BYTE i;
    BYTE ret = 0;

    if (len == 0) return 0;
    
    i2cWaitForIdle();

    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete

    //Write module address
    i2cPutByteAndWait(LCD2S_I2C_ADR + (lcdNum*2));

    //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
    if(i2cWasAckReceived()) {
        //Write array
        for (i=0; i<len; i++) {
            //Write next byte to LCD display
            i2cPutByteAndWait(*s++);

            if(i2cWasAckReceived() == 0) {
                ret = 1;    //Error
                break; //Break if slave does not sent ACK
            }
        }
    }
    else {
        ret = 1;    //Error
    }

    i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete
    
    return ret;
}


/**
 * Sends a NULL terminated string to the LCD Display. It is appended to the current LCD
 * Display string. The NULL is NOT sent!
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param s     Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
BYTE lcdPutString(BYTE lcdNum, BYTE* s) {
    char c;

    i2cWaitForIdle();

    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete

    //Write module address
    i2cPutByteAndWait(LCD2S_I2C_ADR + (lcdNum*2));

    //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
    if(i2cWasAckReceived()==0) {
        i2cPutStopAndWait();    //Reset I2C bus
        return 1;               //Error
    }

    //Write "Write Parsed String" command
    i2cPutByteAndWait(0x80);

    while((c = *s++)) {
        //Write next character to LCD display
        i2cPutByteAndWait(c);
    }

    i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete
    
    return 0;   //OK
}


/**
 * Sends a NULL terminated string to the LCD Display. It is appended to the current LCD
 * Display string. The NULL is NOT sent!
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param str   Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
BYTE lcdPutRomString(BYTE lcdNum, ROM char* s) {
    char c;

    i2cWaitForIdle();

    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete

    //Write module address
    i2cPutByteAndWait(LCD2S_I2C_ADR + (lcdNum*2));

    //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
    if(i2cWasAckReceived()==0) {
        i2cPutStopAndWait();    //Reset I2C bus
        return 1;               //Error
    }

    //Write "Write Parsed String" command
    i2cPutByteAndWait(0x80);

    while((c = *s++)) {
        //Write next character to LCD display
        i2cPutByteAndWait(c);
    }

    i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete
    
    return 0;
}

