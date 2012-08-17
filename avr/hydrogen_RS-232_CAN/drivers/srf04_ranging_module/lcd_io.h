/*******************************************************************************************************
 Title  :   C include file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      25/Oct/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display
       Based on Volker Oth's lcd library (http://members.xoom.com/volkeroth),
       Peter Fleury's work <pfleury@gmx.ch>  http://jump.to/fleury .
       and the very good lcd page at www.myke.com

       With this driver you can select from 2 different I/O modes.
       1) MODE 6 - 6 or 7  pin I/O mode
       2) MODE 2 - 2 pin I/O mode with an additional 74LS174
       3) MODE 7 - multiple lcd unit mode 

MODE 6 INFORMATION
       mode 6 now needs only 6 pins as long the pins can function as output
       freeing two bits to be used as general i/o.
       R/W pin on the lcd must be connected to ground.
       Also the pins can be scattered all over the place (any pin in any port).
       ( the other pins of the port(s) are not affected )
       Be carefull not to change the status of the E lcd pin by using the outp()
       command (both in PORT(X) and DDR(X) ) elsewhere in your  program  otherwise
       your program will crash and you will have a lot of fun for the whole family.
       Use sbi() and cbi() commands instead.

MODE 7 INFORMATION
       mode 7 supports up to 3 lcd units and all the characteristics of mode 6
       except that lcd reading is not possible.
       The lcd diplays can be of different type.


MODE 2 INFORMATION
       mode 2 uses only 2 pins in any combination you want as long the pin
       can function as output.
       You also need to make a very simple cirquit (really silly) with 74LS174.
       See the included JPG photo for the schematic.
       Also the pins can be scattered all over the place (any pin in any port).
       ( the other pins of the port(s) are not affected )
       YOU MAY NOT! MULTIPLEX THE DATA AND CLOCK PINS WITH OTHER DEVICES 
       The pins are automatically set as outputs when you call an lcd function
       Be carefull not to change the status of the CLOCK and DATA pins by using the outp()
       command (both in PORT(X) and DDR(X) ) elsewhere in your  program  otherwise
       your program will crash and you will have a lot of fun for the whole family (again!).
       Use sbi() and cbi() commands instead.     
                         
       MEMORY MAPPED MODE IS NOT SUPPORTED BY THIS DRIVER !!!
       
CAUTION!!!  FOR SLOW LCD UNITS INCREASE THE "LCD_DELAY_TIME_US"  VALUE!!!  
            The driver does not read anything back from the lcd because that way i only need 6
            I/O lines and furthermore i can use the output only port found in MEGA103.
            Since i dont read anything back, i just wait for commands to be executed
            so below i define the time to wait for the two categories of commands of HD44780 .    
            The two CATEGORIES are:
            1) 2ms for clear screen and return home commands (nominal value=1,52 ms)
               and it is derived from "LCD_DELAY_TIME_US" .  
            2) 50us for all other commands (nominal value=37 us) 
  
            Even the slowest units will work reliably with LCD_DELAY_TIME_US=160. 
            The Longer delay needed for CATEGORY 1 is calculated from the "LCD_DELAY_TIME_US" value.
            The specifications for HD44780 @ 270KHZ are 37 us for CATEGORY 2 and 1,52 ms for CATEGORY 1 

LCD MODULE INFORMATION!
       The maximum time required by each instruction for the slowest lcd units is
       4.1 msecs for clearing the display or moving the cursor/display to the "home position",
       160 usecs for all other commands. (i use 50 because i use good units)  

       Usual HD44780 Pins connections 
       1 = Ground 
       2 = Vcc 
       3 = Contrast Voltage 
       4 = "R/S" _Instruction/Register Select 
       5 = "R/W" _Read/Write LCD Registers (Connected to GND for this driver to work!) 
       6 = "E" Clock 
       7 - 14 = Data I/O Pins 0 to 7 (0=7, 1=8,... 7=14 )
       15 - GND for the BACKLIGHTING (Check to be sure because some units have it for VCC)
       16 - VCC for the BACKLIGHTING (Check to be sure because some units have it for GND)

CONNECTION TABLE AS USED IN THIS lcd_io.h FILE       
      |^^^^^^^^^^^^^^^^^^^^^^^^^^^^^|  
      |  port_pin0  -> lcd_D4_pin11 |
      |  port_pin1  -> lcd_D5_pin12 |
      |  port_pin2  -> lcd_D6_pin13 |
      |  port_pin3  -> lcd_D7_pin14 |
      |  port_pin4  -> lcd_RS_pin4  |
      |  port_pin5  -> lcd_E_pin6   |
      |  GND        -> lcd_RW_pin5  |
      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  
*******************************************************************************************************/
#ifndef LCD_IO_H
#define LCD_IO_H

/*############################################################################################*/
/* CONFIGURATION BLOCK STARTS HERE. Change these definitions to adapt setting */
/*############################################################################################*/
/* GLOBAL SETTINGS (settings described here are aplyied everywhere) */

#ifndef F_CPU
#define F_CPU                     3686400   /* CPU CLOCK FREQUENCY         */
#endif

#define LCD_IO_MODE               6         /* 6 = 6 PIN I/O, 2 = 2 PIN I/O, 7 = multi lcd mode */

#define LCD_AUTO_LINE_FEED        0         /* 1 = Auto line feed, 0 = no Auto line feed */

#define LCD_DELAY_TIME_US         100       /* THE NOMINAL TIME THE LCD NEEDS TO COMPLETE A COMMAND  */ 
#define LCD_E_PULSE_WIDTH_US      1         /* THE E PULSE WIDTH IN MICROSECONDS (Timing is accurate)*/

#define LCD_DECIMAL_POINT         ','       /* The decimal point punctuation mark char */
/***********************************************************************************************/
#if LCD_IO_MODE == 2     
/* 
   CONFIGURATION OF BELOW LINES ONLY NECESSARY IN 2 PIN MODE  
   If you plan to use just one port for all pins then just edit "LCD_PORT" 
   otherwise you must specify the port of each lcd signal. the port(s) must be able
   to function as output.  It can be any combination!
   USE CAPITAL LETTER FOR PORT DESIGNATION! (A,B,C,D...etc.)
*/                             
#define LCD_CHARS_PER_LINE       20        /* visible chars per lcd line */
#define LCD_LINES                4         /* visible lines */

#define LCD_PORT                 C  

#define LCD_CLOCK_SIGNAL_PORT    LCD_PORT      /* Put your lcd clock port here (A,B,C...etc.)  */
#define LCD_CLOCK_PIN            0             /* Put your lcd clock pin here */

#define LCD_DATA_SIGNAL_PORT     LCD_PORT      /* Put your lcd data port here (A,B,C...etc.)    */
#define LCD_DATA_PIN             1             /* Put your lcd data pin here */

#endif
/***********************************************************************************************/
/* END OF 2 PIN CONFIGURATION BLOCK  */
/***********************************************************************************************/
#if LCD_IO_MODE == 6 
/* 
    CONFIGURATION OF BELOW LINES ONLY NECESSARY IN MODE 6 
    If you plan to use just one port for all pins then just edit "LCD_PORT" 
    otherwise you must specify the port of each lcd signal. the port(s) must be able
    to function as output.   It can be any combination!  
    PUT YOUR LCD PORT LETTER HERE USING CAPITAL LETTER (A,B,C,D...etc)
*/
#define LCD_CHARS_PER_LINE       20        /* visible chars per lcd line */
#define LCD_LINES                4         /* visible lines */

#define LCD_MULTIPLEX_ENABLE     0                /* 1= the DDR's used are saved and restored */
#define LCD_READ_REQUIRED        0                /* 0=use delay, 1=read busy flag (7 pins needed) */



#define LCD_PORT                 C   

#define LCD_DATA4_PORT           LCD_PORT         /* port for data 0 pin  */
#define LCD_D4_PIN               0                /* AVR port pin number */

#define LCD_DATA5_PORT           LCD_PORT         /* port for data 1 pin  */ 
#define LCD_D5_PIN               1                /* AVR port pin number */

#define LCD_DATA6_PORT           LCD_PORT         /* port for data 2 pin  */
#define LCD_D6_PIN               2                /* AVR port pin number */       

#define LCD_DATA7_PORT           LCD_PORT         /* port for data 3 pin  */
#define LCD_D7_PIN               3                /* AVR port pin number */

#define LCD_RS_SIGNAL_PORT       LCD_PORT         /* port for RS line */
#define LCD_RS_PIN               4                /* AVR port pin number */

#define LCD_E_SIGNAL_PORT        LCD_PORT         /* port for Enable line */
#define LCD_E_PIN                5                /* AVR port pin number */



/* YOU NEED TO EDIT  "LCD_RW_SIGNAL_PORT" AND "LCD_RW_PIN" ONLY IF "LCD_READ_REQUIRED == 1" */
#if LCD_READ_REQUIRED == 1
#define LCD_RW_SIGNAL_PORT       LCD_PORT         /* port for R/W line */
#define LCD_RW_PIN               6                /* AVR port pin number */
#endif

#endif
/***********************************************************************************************/
/* END OF 6 PIN CONFIGURATION BLOCK  */
/***********************************************************************************************/
#if LCD_IO_MODE == 7
/* 
    CONFIGURATION OF BELOW LINES ONLY NECESSARY IN MODE 7
    If you plan to use just one port for all pins then just edit "LCD_PORT" 
    otherwise you must specify the port of each lcd signal. the port(s) must be able
    to function as output.   It can be any combination!  
    PUT YOUR LCD PORT LETTER HERE USING CAPITAL LETTER (A,B,C,D...etc)
*/
#define NUMBER_OF_LCD_UNITS      2         /* 2 or 3. if you set it to 1, mode 6 will be selected */

#define LCD_0_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_0_LINES              4         /* visible lines */

#define LCD_1_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_1_LINES              4         /* visible lines */

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_CHARS_PER_LINE     20        /* visible chars per lcd line */
#define LCD_2_LINES              4         /* visible lines */
#endif



#define LCD_PORT                 C   

#define LCD_DATA4_PORT           LCD_PORT         /* port for data 0 pin  */
#define LCD_D4_PIN               0                /* AVR port pin number */

#define LCD_DATA5_PORT           LCD_PORT         /* port for data 1 pin  */ 
#define LCD_D5_PIN               1                /* AVR port pin number */

#define LCD_DATA6_PORT           LCD_PORT         /* port for data 2 pin  */
#define LCD_D6_PIN               2                /* AVR port pin number */       

#define LCD_DATA7_PORT           LCD_PORT         /* port for data 3 pin  */
#define LCD_D7_PIN               3                /* AVR port pin number */

#define LCD_RS_SIGNAL_PORT       LCD_PORT         /* port for RS line */
#define LCD_RS_PIN               4                /* AVR port pin number */

#define LCD_0_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_0_E_PIN              5                /* AVR port pin number */

#define LCD_1_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_1_E_PIN              6                /* AVR port pin number */

/* EDIT THE BELOW LINES IF YOU USE 3 LCD UNITS */
#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_SIGNAL_PORT      LCD_PORT         /* port for Enable line */
#define LCD_2_E_PIN              7                /* AVR port pin number */
#endif

#endif  /* LCD_IO_MODE == 7 */

/*############################################################################################*/
/*                            CONFIGURATION BLOCK ENDS HERE.                                  */
/*############################################################################################*/

/* you shouldn't need to change anything below this line */

/* Some clever (i hope!) thinking triks */
#if LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS == 1

#warning "MODE 7 USING 1 LCD UNIT. WILL REVERT TO MODE 6"
#undef   LCD_IO_MODE
#define  LCD_IO_MODE            6
#define  LCD_READ_REQUIRED      0
#define  LCD_E_SIGNAL_PORT      LCD_0_E_SIGNAL_PORT
#define  LCD_E_PIN              LCD_0_E_PIN 
#define  LCD_CHARS_PER_LINE     LCD_0_CHARS_PER_LINE
#define  LCD_LINES              LCD_0_LINES 
#undef   LCD_MULTIPLEX_ENABLE
#define  LCD_MULTIPLEX_ENABLE   0
#undef   LCD_READ_REQUIRED
#define  LCD_READ_REQUIRED      0
#undef   LCD_1_CHARS_PER_LINE     
#undef   LCD_1_LINES              
#undef   LCD_1_E_SIGNAL_PORT      
#undef   LCD_1_E_PIN              

           
#elif   LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS > 1

#if NUMBER_OF_LCD_UNITS > 3
#error "UP TO 3 LCD UNITS ARE ALLOWED!"
#endif

#define LCD_0                0 
#define LCD_1                1
#define LCD_2                2

#endif /* #if LCD_IO_MODE == 7 && NUMBER_OF_LCD_UNITS == 1 */

/*-----------------------------------------------------------------------------------*/
/*  HD44780 PARAMETERS */
/*-----------------------------------------------------------------------------------*/

#define LCD_LINE_LENGTH          0x40     /* internal line length */
#define LCD_START_LINE1          0x00     /* DDRAM address of first char of line 1 */
#define LCD_START_LINE2          0x40     /* DDRAM address of first char of line 2 */
#define LCD_START_LINE3          0x14     /* DDRAM address of first char of line 3 */
#define LCD_START_LINE4          0x54     /* DDRAM address of first char of line 4 */

/* instruction register bit positions */
#define LCD_CLR                  0      /* DB0: clear display */
#define LCD_HOME                 1      /* DB1: return to home position */
#define LCD_ENTRY_MODE           2      /* DB2: set entry mode */
#define LCD_ENTRY_INC            1      /*   DB1: 1=increment, 0=decrement  */
#define LCD_ENTRY_SHIFT          0      /*   DB2: 1=display shift on        */
#define LCD_ON                   3      /* DB3: turn lcd/cursor on */
#define LCD_ON_DISPLAY           2      /*   DB2: turn display on */
#define LCD_ON_CURSOR            1      /*   DB1: turn cursor on */
#define LCD_ON_BLINK             0      /*     DB0: blinking cursor ? */
#define LCD_MOVE                 4      /* DB4: move cursor/display */
#define LCD_MOVE_DISP            3      /*   DB3: move display (0-> cursor) ? */
#define LCD_MOVE_RIGHT           2      /*   DB2: move right (0-> left) ? */
#define LCD_FUNCTION             5      /* DB5: function set */
#define LCD_FUNCTION_8BIT        4      /*   DB4: set 8BIT mode (0->4BIT mode) */
#define LCD_FUNCTION_2LINES      3      /*   DB3: two lines (0->one line) */
#define LCD_FUNCTION_10DOTS      2      /*   DB2: 5x10 font (0->5x7 font) */
#define LCD_CGRAM                6      /* DB6: set CG RAM address */
#define LCD_DDRAM                7      /* DB7: set DD RAM address */
#define LCD_BUSY                 7      /* DB7: LCD is busy */

/* set entry mode: display shift on/off, dec/inc cursor move direction */
#define LCD_ENTRY_DEC            0x04   /* display shift off, dec cursor move dir */
#define LCD_ENTRY_DEC_SHIFT      0x05   /* display shift on,  dec cursor move dir */
#define LCD_ENTRY_INC_           0x06   /* display shift off, inc cursor move dir */
#define LCD_ENTRY_INC_SHIFT      0x07   /* display shift on,  inc cursor move dir */

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF             0x08   /* display off                            */
#define LCD_DISP_ON              0x0C   /* display on, cursor off                 */
#define LCD_DISP_ON_BLINK        0x0D   /* display on, cursor off, blink char     */
#define LCD_DISP_ON_CURSOR       0x0E   /* display on, cursor on                  */
#define LCD_DISP_ON_CURSOR_BLINK 0x0F   /* display on, cursor on, blink char      */

/* move cursor/shift display */
#define LCD_MOVE_CURSOR_LEFT     0x10   /* move cursor left  (decrement)          */
#define LCD_MOVE_CURSOR_RIGHT    0x14   /* move cursor right (increment)          */
#define LCD_MOVE_DISP_LEFT       0x18   /* shift display left                     */
#define LCD_MOVE_DISP_RIGHT      0x1C   /* shift display right                    */

/* function set: set interface data length and number of display lines */
#define LCD_FUNCTION_4BIT_1LINE  0x20   /* 4-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_4BIT_2LINES 0x28   /* 4-bit interface, dual line,   5x7 dots */
#define LCD_FUNCTION_8BIT_1LINE  0x30   /* 8-bit interface, single line, 5x7 dots */
#define LCD_FUNCTION_8BIT_2LINES 0x38   /* 8-bit interface, dual line,   5x7 dots */


#define LCD_MODE_DEFAULT         ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )

#define RAM                      1
#define EEPROM                   2
#define FLASH                    3
#define ALL                      0xFF

/*-----------------------------------------------------------------------------------*/
/*  function prototypes */
/*-----------------------------------------------------------------------------------*/

extern void         lcd_init(void);
extern void         lcd_command(unsigned char cmd);
extern void         lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y);
extern void         lcd_clrscr(void);
extern void         lcd_clrline(unsigned char line);
extern void         lcd_home(void);
extern void         lcd_putc(unsigned char c);
extern void         lcd_putc_cgram(const unsigned char *user_char, unsigned char char_position);
extern void         lcd_puts(const unsigned char *s);
extern void         lcd_puts_p(const unsigned char *progmem_s);
extern void         lcd_puts_e(unsigned char *eeprom_s); 
extern void         lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars) ;
extern unsigned int lcd_getxy(void);
#if   LCD_IO_MODE == 7 
extern void         select_lcd(unsigned char lcd_unit);
#endif

/*
   Suppose we want to display a 16 bit var with a rvalue 0f 325
   We give the command lcd_puti(var_name, 0); were var=325
   The display will show 325 
   We could also use "lcd_put_i(325,0,3);". The third argument reserves 3 digits in the lcd display.
   If there is nothing to show in a particular lcd char position a 0 will show up.

More examples:

   a) We give the command lcd_puti(var_name,1);
      The display will show 32,5 
   b) We give the command lcd_puti(var_name,2);
      The display will show 3,25 
   c) We give the command lcd_put_i(var_name,3,5);
      The display will show 0,325 
   d) We give the command lcd_put_i(var_name,4,4);
      The display will show 0,03 
   e) We give the command lcd_put_i(var_name,1,5); var=-325
      The display will show -32,5
   f) We give the command lcd_put_i(var_name,1,4); var=-325
      The display will show  -32,

 */
/*
** macros for automatically storing string constant in program memory
*/
#ifndef P
#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
#endif
#define lcd_puts_P(__s)         lcd_puts_p(P(__s))

#define lcd_puti(x,y)           lcd_put_i(x,y,0xFF)

#define lcd_fill_cgram(array)   lcd_putc_cgram((const unsigned char*)array, sizeof(array))

#endif //LCD_IO_H
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


