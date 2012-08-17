/****************************************************************************
 Title  :   C  include file for the HEX KEYBOARD FUNCTIONS library (hex_keyb.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/*
The keyborad driver emulates now close enough the ERICSSON cellular phones keyboard with a few 
alteration due to limited resources.
However you can alter the keyboard behaviour by editing certain parameters within this .h file.

The keyboard driver functions get input from the keyboard and simultaneously display the input on the
LCD screen at the predefined position given as an argument to the keyboard function.
The input can be multiline or whatever length you give as an argument the the keyboard function.

During operation the keyboard port acts as both output and input so you must choose a bidirectional port.
After the driver finishes the command that was executing sets all keyb port pins as inputs, pullups off, 
in case you are multiplexing the keyboard pins with other devices. You can multiplex only 4 pins 
(either the row or the column pins, not mixed!)
That is because if you multiplex all lines when someone hits a key the correspondig row and
column lines will be shorted thus altering the values on the pins. 

I use the PACTEC SELF ADHESIVE 4X4 keyboard that has the below connections 
(Thats the only one i could easily find here). You may use whatever keybord 
you want but you have to edit the definitions that define the keyboard connections.
*/
 

#ifndef HEX_KEYB_H
#define HEX_KEYB_H

/*##########################################################################################*/
/*                              START OF CONFIGURATION BLOCK                                */
/*##########################################################################################*/

#ifndef F_CPU
#define F_CPU                  3686400        /* CPU CLOCK FREQUENCY         */
#endif

#define KEYBOARD_TYPE             2              /* 0=your own type, 1=PACTEC, 2=VELLEMAN            */
#define KEYBOARD_PORT             B              /* PUT YOUR KEYBOARD PORT (A,B,C,D,etc.)            */ 
#define ACTIVATE_ARROWS           1              /* arrow keys <-- and --> ON or OFF                 */
#define ACTIVATE_AUTO_ADVANCE     1              /* Cursor auto advance ON or OFF                    */
#define CURSOR_ADVANCE_TIME       20             /* In tenth's of a second (max ~25 seconds)         */
#define HKEYB_SCAN_FUNCTION_ONLY  0              /* 1= only "scan_keyb(void)" is available           */

/* Lcd driver information (you only need to change it if you decide to use your own lcd driver)      */
/* THE BELOW DEFINITIONS ARE NEEDED BY BOTH THE TEST PROGRAM AND THE DRIVER! */
/* name your lcd driver header file below */
#if HKEYB_SCAN_FUNCTION_ONLY == 0

#include                       "lcd_io.h"         
#ifndef LCD_CHARS_PER_LINE
/* Define your lcd characters per line size below                       */
#define LCD_CHARS_PER_LINE     20
#endif
#ifndef LCD_LINES
/* Define your lcd lines number below                                   */
#define LCD_LINES              4
#endif
/* name your lcd driver goto_xy function below                          */
#define LCD_GOTOXY(x,y)        lcd_gotoxy(x,y)    
/* name your lcd function to make the cursor visible and blinking below */
#define LCD_CURSOR_ON_BLINK()  lcd_command(LCD_DISP_ON_BLINK)
/* name your lcd function to turn off the cursor  below                 */
#define LCD_CURSOR_OFF()       lcd_command(LCD_DISP_ON)
/* name your lcd put char function below                                */
#define LCD_PUTC(x)            lcd_putc(x)        

/*###########################################################################################*/
/* THE BELOW DEFINITIONS ARE NEEDED ONLY BY THE TEST PROGRAM AND NOT BY THE KEYBOARD DRIVER! */
/* EDIT THEM AS NEEDED BY YOUR ACTUAL APPLICATION OR USE THE ACTUAL COMMANDS!                */
/*###########################################################################################*/
/* name your lcd initialization function below                          */
#define LCD_INIT()            lcd_init()
/* name your lcd clear screen function below                            */
#define LCD_CLRSCR()           lcd_clrscr()
/* name your lcd put string function below                              */
#define LCD_PUTS(x)            lcd_puts(x)
/* name your lcd put integer function below                             */
#define LCD_PUTI(x,y)          lcd_puti(x,y)
/* name your lcd put progmem string function below                      */
#define LCD_PUTS_P(x)          lcd_puts_P(x)
/* name your lcd put eeprom string function below                       */
#define LCD_PUTS_E(x)          lcd_puts_e(x)

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/*###########################################################################################*/
/* KEYBOARD KEY DEFINITIONS                             */
/*###########################################################################################*/
/*
               THE KEYBOARD KEY LAYOUT IS SHOWN BELOW               
                       C1   C2   C3   C4 
                    ----------------------- 
                    |  1    2    3    4    | R1
                    |  5    6    7    8    | R2
                    |  9    10   11   12   | R3
                    |  13   14   15   16   | R4
                    -----------------------
*/

                  

#if KEYBOARD_TYPE == 0     /* YOUR OWN TYPE KEYBOARD ID SETTINGS (255) */
/* PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  */
/*    R or C      bit position                            */  
#define COL1            0
#define COL2            1     
#define COL3            2  
#define COL4            3
#define ROW1            4
#define ROW2            5
#define ROW3            6
#define ROW4            7   

/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            'A'
#define KEY5            '4'
#define KEY6            '5'
#define KEY7            '6'
#define KEY8            'B'
#define KEY9            '7'
#define KEY10           '8'
#define KEY11           '9'
#define KEY12           'C'
#define KEY13           '*'
#define KEY14           '0'
#define KEY15           '#'
#define KEY16           'D'                      

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'D'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       '*'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      '#'             /* DEFINE MOVE RIGHT arrow key for kgets()           */

#endif

#if KEYBOARD_TYPE == 1
/*
                       PACTEC SELF ADHESIVE 4X4 KEYBOARD
                |----------------------------------------------------|
                |   KEYB PINS  R3  R4 C3  C4  C1  C2   R2  R1        |
                |   PORT BITS  7   6   5   4   3   2    1   0        |    
                |----------------------------------------------------|        

PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  
      R or C      bit position                            */  
#define COL1            3
#define COL2            2     
#define COL3            5  
#define COL4            4      
#define ROW1            0
#define ROW2            1
#define ROW3            7
#define ROW4            6   

/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            '4'
#define KEY5            '5'
#define KEY6            '6'
#define KEY7            '7'
#define KEY8            '8'
#define KEY9            '9'
#define KEY10           '0'
#define KEY11           'A'
#define KEY12           'B'
#define KEY13           'C'
#define KEY14           'D'
#define KEY15           'E'
#define KEY16           'F'                

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'F'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       'D'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      'E'             /* DEFINE MOVE RIGHT arrow key for kgets()           */

#endif                    

#if KEYBOARD_TYPE == 2
/*
                       VELLEMAN PLASTIC 4X4 KEYBOARD
                |----------------------------------------------------|
                |   KEYB PINS  R4  R3 R2  R1  C4  C3   C2  C1        |
                |   PORT BITS  7   6   5   4   3   2    1   0        |    
                |----------------------------------------------------|        

PUT THE KEYBOARD CONNECTIONS HERE C = COLUMN, R = ROW  
      R or C      bit position                               */  
#define COL1            0
#define COL2            1     
#define COL3            2  
#define COL4            3
#define ROW1            4
#define ROW2            5
#define ROW3            6
#define ROW4            7   

/* definitions of the low level command scan_keyb() return codes       */
#define KEY1            '1'         
#define KEY2            '2'
#define KEY3            '3'
#define KEY4            'A'
#define KEY5            '4'
#define KEY6            '5'
#define KEY7            '6'
#define KEY8            'B'
#define KEY9            '7'
#define KEY10           '8'
#define KEY11           '9'
#define KEY12           'C'
#define KEY13           '*'
#define KEY14           '0'
#define KEY15           '#'
#define KEY16           'D'                      

/* KEY FUNCTION DEFINITIONS ( you can change them as you please )                            */
#define ENTER           'D'             /* DEFINE ENTER HERE   ( global)                     */
#define ESCAPE          'C'             /* DEFINE ESCAPE HERE  ( global)                     */
#define CAPITAL         'A'             /* DEFINE SMALL TO CAPITAL for kgets()               */
#define MINUS           'A'             /* DEFINE MINUS SIGN FOR kgeti()                     */ 
#define BACKSPACE       'B'             /* DEFINE BACKSPACE    ( global )                    */
#define MOVE_LEFT       '*'             /* DEFINE MOVE LEFT arrow key for kgets()            */
#define MOVE_RIGHT      '#'             /* DEFINE MOVE RIGHT arrow key for kgets()           */

#endif
/*##########################################################################################*/
/*                             END OF CONFIGURATION BLOCK                                   */
/*##########################################################################################*/ 

#if HKEYB_SCAN_FUNCTION_ONLY == 0

/* FUNCTION PROTOTYPES */
/* waits untill it gets a string + enter of n chars displayed at column lcd_x and row lcd_y    */
/* Returns the number of chars entered. Escape returns 0 so you can test if the data are valid.*/                                          
extern void          kgets( unsigned char *data, unsigned char max_chars,
                            unsigned char lcd_x, unsigned char lcd_y      );

/* 
waits untill it gets a number of n digits displayed at column lcd_x and row lcd_y 
If digits is set to 0 the function enters password mode where the user can input  
a 4 digit number but the display will show **** (stars) instead of the actual number */
extern unsigned char  kgeti(int *data, unsigned char digits, unsigned char lcd_x, unsigned char lcd_y);
                             
/* waits untill it gets a character */
extern unsigned char kgetc(void);                            

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/* The low level keyboard scanning function that scans the keyboard.            */
/* returns the scan codes defined earlier in the file or 0 if no key is pressed */
/* NOTE: IT DOES NOT WAIT FOR A KEY NOR USES THE LCD DRIVER                     */
extern unsigned char scan_keyb(void);

#if HKEYB_SCAN_FUNCTION_ONLY == 0 

/* MACRO DEFINITION  */
#define kgetS(w,x,y,z)  kgets(w,(sizeof(w)-1),y,z)

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */


#endif /* HEX_KEYB_H */
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

