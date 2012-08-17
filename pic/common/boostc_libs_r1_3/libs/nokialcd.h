/*************************************************************
* Nokia 3310 graphical LCD interface 
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _NLCD_H_
#define _NLCD_H_

#include <system.h>

// Define the LCD connections here
#define NLCD_SCLK_PORT		PORTA
#define NLCD_SDIN_PORT		PORTA
#define NLCD_DC_PORT		PORTE
#define NLCD_SCE_PORT		PORTE
#define NLCD_RST_PORT		PORTE

#define NLCD_SCLK_TRIS		TRISA
#define NLCD_SDIN_TRIS		TRISA
#define NLCD_DC_TRIS		TRISE
#define NLCD_SCE_TRIS		TRISE
#define NLCD_RST_TRIS		TRISE

#define NLCD_SCLK_PIN		4
#define NLCD_SDIN_PIN		5
#define NLCD_DC_PIN			0
#define NLCD_SCE_PIN		1
#define NLCD_RST_PIN		2

#define NLCD_SCREEN_WIDTH	14

///////////////////////////////////////////////////////////////////
// Don't change below this line
volatile bit nlcd_sclk	@ NLCD_SCLK_PORT	. NLCD_SCLK_PIN;
volatile bit nlcd_sdin	@ NLCD_SDIN_PORT    . NLCD_SDIN_PIN;
volatile bit nlcd_dc	@ NLCD_DC_PORT    	. NLCD_DC_PIN;
volatile bit nlcd_sce	@ NLCD_SCE_PORT 	. NLCD_SCE_PIN;
volatile bit nlcd_rst	@ NLCD_RST_PORT		. NLCD_RST_PIN;

bit nlcd_tris_sclk 		@ NLCD_SCLK_TRIS    . NLCD_SCLK_PIN;
bit nlcd_tris_sdin		@ NLCD_SDIN_TRIS    . NLCD_SDIN_PIN;
bit nlcd_tris_dc		@ NLCD_DC_TRIS    	. NLCD_DC_PIN;
bit nlcd_tris_sce		@ NLCD_SCE_TRIS		. NLCD_SCE_PIN;
bit nlcd_tris_rst		@ NLCD_RST_TRIS 	. NLCD_RST_PIN;

// LCD command set
#define NLCD_DATA			1
#define NLCD_CMD			0

#define NLCD_NORMAL_MODE	0x0C
#define NLCD_INVERSE_MODE	0x0D
#define NLCD_EXTENDED_CMDS	0x21
#define NLCD_NORMAL_CMDS	0x20

// Function definitions
void nlcd_init();
void nlcd_write_byte(char data, bit mode);
void nlcd_clear();
void nlcd_print_char(char data);
void nlcd_printf(const char* line);
void nlcd_goto(char x, char y);
void nlcd_init_scroll_line(char line_nr, const char* data);
void nlcd_display_scroll_line();
char nlcd_scroll_left();
char nlcd_scroll_right();
void nlcd_print_hex(char value);
void nlcd_print_hex(short value);
/*
void lcd_send_byte(char data);
void lcd_data_mode();
void lcd_cmd_mode();
void lcd_send_cmd(char command);
void lcd_print_hex(char value);
void lcd_print_hex(short value);
*/
// Macro's


#endif // _NLCD_H_
