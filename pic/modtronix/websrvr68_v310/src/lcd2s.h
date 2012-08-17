/**
 * @brief           Serial LCD Display handler.
 * @file            lcd2s.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_lcd2s
 *
 *
 * @section description Description
 **********************************
 * This module implements a driver for the LCD2S range of serial LCD Displays.
 *  To use this module:
 *  - Customize defines specified in "Serinit Configuration".
 *  - Call lcdInit() to initialize LCD display
 *  - Use funtions to write and read LCD display
 *
 *
 * @subsection lcd2s_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- Serint Configuration --------------------
 //*********************************************************************
 //Define the default I2C address for the LCD display
 #define LCD2S_DEFAULT_I2C_ADR (80ul)

 //Define the I2C address for the LCD display
 #define LCD2S_I2C_ADR LCD2S_DEFAULT_I2C_ADR

 //Define number of serial LCD displays supported
 #define LCD2S_MAX_NUMBER 1

 //Define size of keypad buffer for each serial LCD display, must be 8,16,32 or 64
 #define KPAD_BUF_SIZE  8ul
 @endcode
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

/**
 * @defgroup mod_sys_lcd2s Serial Port
 * @ingroup mod_sys
 *********************************************************************/


#ifndef _LCD2S_H_
#define _LCD2S_H_

/////////////////////////////////////////////////
//Global defines
#ifndef LCD2S_I2C_ADR
#define LCD2S_I2C_ADR  80ul  //LCD2S I2C Address
#endif



/////////////////////////////////////////////////
//Defines
//#define ....


/////////////////////////////////////////////////
//Global data
extern BYTE kpadBufGet[LCD2S_MAX_NUMBER];    //get index
extern BYTE kpadBufPut[LCD2S_MAX_NUMBER];    //put index


/////////////////////////////////////////////////
//Function prototypes


/**
 * Called in the USART TX ISR
 */
void lcdInit(void);


/**
 * Service this module
 */
void lcdService(void);


/**
 * Are there any keys in the keypad buffer.
 * @return 1 if true, else 0
 */
#define kpadHasKeys(kpadNum) (kpadBufPut[kpadNum] != kpadBufGet[kpadNum])


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
BYTE kpadGetKey(BYTE kpadNum);


/**
 * Send the given byte to the LCD Display. It is appended to the current LCD display.
 * The following bytes have special meanings:
 * - 0x0a ('\\n') = Go to beginning of next line.
 * - 0x0c ('\\f') = Clear display and go to beginning of first line.
 * - 0x0d ('\\r') = Go to beginning of first line
 * - 0x08 ('\\b') = Cursor left
 * - 0x09 ('\\t') = Cursor Right
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c     Byte to write out on the serial port
 */
void lcdPutByte(BYTE lcdNum, BYTE c);


/**
 * Send the ASCII hex value of the given byte to the LCD Display. It is appended to the
 * current LCD Display. For example, if c=11, then "0B" will be sent to the LCD Display
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param c     Byte to write out on the serial port
 */
void lcdPutByteHex(BYTE lcdNum, BYTE c);


/**
 * Writes given number of characters from given buffer to the LCD Display.
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 * @param s         Buffer containing data to send to LCD Display
 * @param len       Number of bytes to send from given buffer
 *
 * @return 0 if OK, else error
 */
BYTE lcdPutArray(BYTE lcdNum, BYTE* s, BYTE len);


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
BYTE lcdPutString(BYTE lcdNum, BYTE* s);


/**
 * Sends a NULL terminated string to the LCD Display. It is appended to the current LCD
 * Display string. The NULL is NOT sent!
 *
 * @param lcdNum The number of the LCD you want to access. Is a number from 0-n.
 *        If, for example, the system is configured for 4 LCDs, it is a number from 0-3.
 *
 * @param s   Null terminated string to write out to the LCD Display
 *
 * @return 0 if OK, else error
 */
BYTE lcdPutRomString(BYTE lcdNum, ROM char* s);


#endif    //_LCD2S_H_




