/** 
 ******************************************************************************
 * @file 	hal_lcd.h
 *
 * @author	Alex Raimondi <araimondgmx.net>
 *
 * @brief	Print text on an lcd text display
 *
 * 
 * Memory mapped mode uses the following pins:
 * 
 *    DATA = PORTA
 *    R S  = PORTC BIT 6
 *    R/W  = PORTD BIT 6
 *   (E    = PORTC BIT 7)
 * 
 * IO mode can use any pins.
 * 4bit mode is not supported.
 *
 ******************************************************************************
 */

#ifndef HAL_LCD_H
#define HAL_LCD_H

#include "typedefs.h"

#define LCD_LINES           4     /* visible lines */
#define LCD_LINE_LENGTH    20     /* line length */

#define LCD_8BIT_MODE       1     /* 0: 4bit, 1: 8bit */
#define LCD_IO_MODE         0     /* 0: memory mapped mode, 1: IO port mode */
#define LCD_HW_ENABLE       0     /* 0: normal IO, 1: IO mode on STK200/300 */  

#if LCD_IO_MODE                    /* only necessary in IO mode */
#define LCD_DATA_PORT       PORTA  /* port for lines D0-7 (8bit), D4-7 (4bit) */
#define LCD_E_PORT          PORTC  /* port for Enable line */
#define LCD_E_PIN           7
#define LCD_RS_PORT         PORTC  /* port for RS line */
#define LCD_RS_PIN          6
#define LCD_RW_PORT         PORTD  /* port for RW line */
#define LCD_RW_PIN          6
#endif


/* you shouldn't need to change anything below this line */

#define LCD_CLR             0      /* DB0: clear display */
#define LCD_HOME            1      /* DB1: return to home position */
#define LCD_ENTRY_MODE      2      /* DB2: set entry mode */
#define LCD_ENTRY_INC       1      /*   DB1: increment ? */
#define LCD_ENTRY_SHIFT     0      /*   DB2: shift ? */
#define LCD_ON              3      /* DB3: turn lcd/cursor on */
#define LCD_ON_DISPLAY      2      /*   DB2: turn display on */
#define LCD_ON_CURSOR       1      /*   DB1: turn cursor on */
#define LCD_ON_BLINK        0      /*     DB0: blinking cursor ? */
#define LCD_MOVE            4      /* DB4: move cursor/display */
#define LCD_MOVE_DISP       3      /*   DB3: move display (0-> cursor) ? */
#define LCD_MOVE_RIGHT      2      /*   DB2: move right (0-> left) ? */
#define LCD_FUNCTION        5      /* DB5: function set */
#define LCD_FUNCTION_8BIT   4      /*   DB4: set 8BIT mode (0->4BIT mode) */
#define LCD_FUNCTION_2LINES 3      /*   DB3: two lines (0->one line) */
#define LCD_FUNCTION_10DOTS 2      /*   DB2: 5x10 font (0->5x7 font) */
#define LCD_CGRAM           6      /* DB6: set CG RAM address */
#define LCD_DDRAM           7      /* DB7: set DD RAM address */

#define LCD_BUSY            7      /* DB7: LCD is busy */


#if LCD_IO_MODE == 0               /* only necessary in memory mapped mode */
#define LCD_IO_DATA         0xC000
#define LCD_IO_FUNCTION     0x8000
#endif


#if LCD_8BIT_MODE
#define LCD_PORT_MASK 0xff
#define LCD_FDEF_1    (1<<LCD_FUNCTION_8BIT)
#else
#define LCD_PORT_MASK 0xf0
#define LCD_FDEF_1    0
#endif


#if LCD_LINES == 1
#define LCD_FDEF_2    0
#else
#define LCD_FDEF_2    (1 << LCD_FUNCTION_2LINES)
#endif

typedef enum _lcdCursorMode
{
	cursorOff = 0,
	cursorOn = 2,
	cursorBlink = 3
} lcdCursorMode;

#define LCD_FUNCTION_DEFAULT ((1 << LCD_FUNCTION) | LCD_FDEF_1 | LCD_FDEF_2)

#define LCD_MODE_DEFAULT     ((1 << LCD_ENTRY_MODE) | (1 << LCD_ENTRY_INC))

#define lcd_e_high()    sbi(LCD_E_PORT, LCD_E_PIN); asm volatile ("nop" ::)
#define lcd_e_low()     cbi(LCD_E_PORT, LCD_E_PIN)


extern const uint8 menuPfeilChar[8];

/* prototypes */

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdCommand(uint8 cmd);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdGotoXY(uint8 x, uint8 y);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdSetCursor( lcdCursorMode c );

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdClrscr(void);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdHome(void);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdPutChar(uint8 data);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLCDSetupUserCharacters( void );

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdInit(lcdCursorMode cursor, uint8 fnc);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdPuts( const char s[]);

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
void halLcdDefineChar( uint8 nr, const uint8 data[8] );

/**
 ******************************************************************************
 * @brief
 *
 * @param
 * @retval
 * @return 
 ******************************************************************************
 */  
char* int2str(uint16 i, char *s, uint8 d);

void int2hex( char * buf, uint8 i );

/* auto calculation doesn't work for prescalers /1 (1) and /8 (2) */
#define CYCLES_PER_US ((F_CPU+500000)/1000000) /* cpu cycles per microsecond */

void delay(uint16 us);


#endif
