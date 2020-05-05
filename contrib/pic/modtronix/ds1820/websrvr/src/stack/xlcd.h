/*********************************************************************
 *
 *                  External LCD access routines defs
 *
 *********************************************************************
 * FileName:        XLCD.h
 * Dependencies:    compiler.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/8/02  Original        (Rev 1.0)
 * Nilesh Rajbharti     7/10/02 Improved
 ********************************************************************/
#ifndef __XLCD_H
#define __XLCD_H

#include "compiler.h"


/* XLCD peripheral routines.
 *
 *   Notes:
 *      - These libraries routines are written to support the
 *        Hitachi HD44780 LCD controller.
 *      - The user must define the following items:
 *          - The LCD interface type (4- or 8-bits)
 *          - If 4-bit mode
 *              - whether using the upper or lower nibble
 *          - The data port
 *              - The tris register for data port
 *              - The control signal ports and pins
 *              - The control signal port tris and pins
 *          - The user must provide three delay routines:
 *              - DelayFor18TCY() provides a 18 Tcy delay
 *              - DelayPORXLCD() provides at least 15ms delay
 *              - DelayXLCD() provides at least 5ms delay
 */

/* Interface type 8-bit or 4-bit
 * For 8-bit operation uncomment the #define BIT8
 */

/* When in 4-bit interface define if the data is in the upper
 * or lower nibble.  For lower nibble, comment the #define UPPER
 */
//#define UPPER

/* DATA_PORT defines the port to which the LCD data lines are connected */

#define DATA_PORT      PORTD
#define TRIS_DATA_PORT TRISD

/*
 * Uncomment this line if non-blocking functions are desired.
 * If following line is commented, caller must make sure that
 * XLCD is free ( !XLCDIsBusy() )before calling any get/put commands.
 */
#define XLCD_IS_BLOCKING

/*
 * Comment following line if there is no read back capability
 * for this LCD.
 */
#define XLCD_READ_BACK

/*
 * Uncomment following line if you want to read data from LCD
 */
//#define XLCD_ENABLE_LCD_READS

/*
 * Set your LCD type.
 */
#define XCLD_TYPE           (FOUR_BIT & LINES_5X8)

/*
 * This is how LCD will be setup on Init.
 */
#define XLCD_DISPLAY_SETUP  (CURSOR_OFF & BLINK_OFF)

/*
 * Uncomment following line if LCD data is to be read back.
 */
//#define XLCD_ENABLE_LCD_READS

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */

#define RW_PIN   PORTD_RD5   /* PORT for RW */
#define TRIS_RW  TRISD_RD5    /* TRIS for RW */
#define RS_PIN   PORTD_RD4   /* PORT for RS */
#define TRIS_RS  TRISD_RD4    /* TRIS for RS */
#define E_PIN    PORTA_RA5   /* PORT for E  */
#define TRIS_E   TRISA_RA5    /* TRIS for E  */

/* Display ON/OFF Control defines */
#define DON         0b00001111  /* Display on      */
#define DOFF        0b00001011  /* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00010011  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00010111  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00011011  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00011111  /* Display shifts to the right */
#define DISP_CLEAR        0b00000001

/* Function Set defines */
#define FOUR_BIT   0b00101111  /* 4-bit Interface               */
#define EIGHT_BIT  0b00111111  /* 8-bit Interface               */
#define LINE_5X7   0b00110011  /* 5x7 characters, single line   */
#define LINE_5X10  0b00110111  /* 5x10 characters               */
#define LINES_5X7  0b00111111  /* 5x7 characters, multiple line */
#define LINES_5X8  0b00111000  /* 5x8 characters, multiple line */


void XLCDInit(void);
void XLCDPut(char data);
void XLCDPutString(char *string);
void XLCDPutROMString(ROM char *string);
char XLCDIsBusy(void);
void XLCDCommand(unsigned char cmd);
unsigned char XLCDGetAddr(void);
char XLCDGet(void);

#define XLCDGoto(row, col)      XLCDCommand((col + (row * 0x40)) | 0x80)
#define XLCDClear()         XLCDCommand(DISP_CLEAR)
#define XLCDCursorRight()   XLCDCommand(SHIFT_CUR_RIGHT)
#define XLCDCursorLeft()    XLCDCommand(SHIFT_CUR_LEFT)
#define XLCDShiftLeft()     XLCDCommand(SHIFT_DISP_LEFT)
#define XLCDShiftRight()    XLCDCommand(SHIFT_DISP_RIGHT)

#define XLCDCGRAMAddr(addr) XLCDCommand(addr | 0b01000000)
#define XLCDDDRAMAddr(addr) XLCDCommand(addr | 0b10000000)

/* User defines these routines/macros according to the oscillator frequency */
#define XLCDDelay500ns()    XLCDDelay100us()

void XLCDDelay15ms(void);
void XLCDDelay4ms(void);
void XLCDDelay100us(void);

#endif
