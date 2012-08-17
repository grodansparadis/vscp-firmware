/*************************************************************
* 7-pins LCD interface code 
*
* Drives an LCD in 4-bit mode, using the three control
* signals. This has the advantage that the code is
* independent of the LCD, since there are no
* timing loops in the code. The code is blocking
* and used the LCD busy signal to determine if the 
* display is ready to accept new data.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _LCD_H_
#define _LCD_H_

#include <system.h>

// Define the LCD connections here
#define LCD_E_PORT			PORTB
#define LCD_RS_PORT			PORTB
#define LCD_RW_PORT			PORTC
#define LCD_DATA4_PORT		PORTB
#define LCD_DATA5_PORT		PORTB
#define LCD_DATA6_PORT		PORTB
#define LCD_DATA7_PORT		PORTB

#define LCD_E_TRIS			TRISB
#define LCD_RS_TRIS			TRISB
#define LCD_RW_TRIS			TRISC
#define LCD_DATA4_TRIS		TRISB
#define LCD_DATA5_TRIS		TRISB
#define LCD_DATA6_TRIS		TRISB
#define LCD_DATA7_TRIS		TRISB

#define LCD_E_PIN			5
#define LCD_RS_PIN			4
#define LCD_RW_PIN			0
#define LCD_DATA4_PIN		0
#define LCD_DATA5_PIN		1
#define LCD_DATA6_PIN		2
#define LCD_DATA7_PIN		3

///////////////////////////////////////////////////////////////////
// Don't change below this line
volatile bit lcd_e			@ LCD_E_PORT     . LCD_E_PIN;
volatile bit lcd_rs			@ LCD_RS_PORT    . LCD_RS_PIN;
volatile bit lcd_rw			@ LCD_RW_PORT    . LCD_RW_PIN;
volatile bit lcd_d4			@ LCD_DATA4_PORT . LCD_DATA4_PIN;
volatile bit lcd_d5			@ LCD_DATA5_PORT . LCD_DATA5_PIN;
volatile bit lcd_d6			@ LCD_DATA6_PORT . LCD_DATA6_PIN;
volatile bit lcd_d7			@ LCD_DATA7_PORT . LCD_DATA7_PIN;

volatile bit lcd_tris_e 	@ LCD_E_TRIS     . LCD_E_PIN;
volatile bit lcd_tris_rs	@ LCD_RS_TRIS    . LCD_RS_PIN;
volatile bit lcd_tris_rw	@ LCD_RW_TRIS    . LCD_RW_PIN;

bit lcd_tris_d4	@ LCD_DATA4_TRIS . LCD_DATA4_PIN;
bit lcd_tris_d5	@ LCD_DATA5_TRIS . LCD_DATA5_PIN;
bit lcd_tris_d6	@ LCD_DATA6_TRIS . LCD_DATA6_PIN;
bit lcd_tris_d7	@ LCD_DATA7_TRIS . LCD_DATA7_PIN;

// LCD command set
#define	LINE1   		0x80   	// set display to line 1 character 0
#define	LINE2           0xC0   	// set display to line 2 character 0
#define	FUNCTION_SET 	0x28   	// 4 bits, 2 lines, 5x7 Font 
#define	DISP_ON         0x0C   	// display on
#define	DISP_ON_C       0x0E   	// display on, Cursor on
#define	DISP_ON_B       0x0F   	// display on, Cursor on, Blink cursor
#define	DISP_OFF        0x08  	// display off
#define	DISP_CLR        0x01   	// clear the Display
#define	ENTRY_INC       0x06   	// increment-mode, display shift OFF
#define	ENTRY_INC_S     0x07   	// increment-mode, display shift ON
#define	ENTRY_DEC       0x04   	// decrement-mode, display shift OFF
#define	ENTRY_DEC_S     0x05   	// decrement-mode, display shift ON
#define	DD_RAM_ADDR     0x80   	// Least Significant 7-bit are for address

// Function definitions
void lcd_init();
void lcd_send_byte(char data);
void lcd_data_mode();
void lcd_cmd_mode();
void lcd_send_cmd(char command);
void lcd_printf(const char* line);
void lcd_printf(char value);
void lcd_print_hex(char value);
void lcd_print_hex(short value);

// Macro's
#define lcd_data_mode lcd_rs = 1
#define lcd_cmd_mode  lcd_rs = 0
#define lcd_e_hi	  lcd_e  = 1
#define lcd_e_lo	  lcd_e  = 0
#define lcd_toggle_e  lcd_e_hi ; lcd_e_lo 

#endif // _LCD_H_
