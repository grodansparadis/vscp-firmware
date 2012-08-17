/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : lcd.c                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>

U8 rc = 0;	//lcd row_counter
/***************************************************************************
 * Init
 ***************************************************************************/
//INIT1:	; 18 bytes
//const uint8_t init1[] = {
const U8 init1[] = {
0x07, 0x00, 0x00,0x02, 0x04, 0x00,
0x04, 0x00, 0x00,0x0c, 0x00, 0x01,
0x0d, 0x06, 0x16,0x0e, 0x10, 0x10
};

//INIT2:	; 6 bytes
const U8 init2[] = {
0x03, 0x00, 0x0c,0x0e, 0x2d, 0x1f
};

//INIT3:	; 3 bytes
const U8 init3[] = {
0x0d, 0x06, 0x16//, 0x00
};

//INIT4:	; 63 bytes
const U8 init4[] = {
0x01, 0x02, 0x15,0x0f, 0x00, 0x00,
0x0b, 0x00, 0x00,0x11, 0x00, 0x00,
0x06, 0x00, 0x00,0x05, 0x00, 0x30,
0x14, 0xaf, 0x00,0x15, 0x00, 0x00,
0x16, 0x83, 0x00,0x17, 0xaf, 0x00,
0x20, 0x00, 0x00,0x30, 0x00, 0x00,
0x31, 0x04, 0x00,0x32, 0x02, 0x07,
0x33, 0x07, 0x00,0x34, 0x00, 0x05,
0x35, 0x07, 0x03,0x36, 0x07, 0x07,
0x37, 0x00, 0x07,0x3a, 0x12, 0x00,
0x3b, 0x00, 0x09//,0x00
};
/***************************************************************************
 * ascii table, starting with character blank (32)
 * size is 8x14 pixel
 ***************************************************************************/
const U8 ascii_tab[96][14] = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // space (32)
{ 0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, //!
{ 0x00, 0x66, 0x66, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //"
{ 0x00, 0x00, 0x00, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0x00, 0x00}, //#
{ 0x00, 0x18, 0x18, 0x7c, 0xc6, 0xc2, 0xc0, 0x7c, 0x06, 0x86, 0xc6, 0x7c, 0x18, 0x18}, //$
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xc6, 0x0c, 0x18, 0x30, 0x66, 0xc6, 0x00, 0x00}, // %
{ 0x00, 0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0xcc, 0x76, 0x00, 0x00}, // &
{ 0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '
{ 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00, 0x00}, // (
{ 0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00}, // )
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00}, // *
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00}, // +
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x30, 0x00}, // ´
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // -
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00}, // .
{ 0x00, 0x00, 0x00, 0x02, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00}, // /
{ 0x00, 0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xd6, 0xc6, 0xc6, 0x6c, 0x38, 0x00, 0x00}, // 0 (48-32)
{ 0x00, 0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc6, 0xfe, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x06, 0x3c, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x1e, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xfe, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x06, 0x0c, 0x78, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x00, 0x00},// <
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x0c, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xde, 0xde, 0xde, 0xdc, 0xc0, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00, 0x00},// A
{ 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0xfc, 0x00, 0x00},// B
{ 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xc0, 0xc2, 0x66, 0x3c, 0x00, 0x00},// C
{ 0x00, 0x00, 0x00, 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00, 0x00},// D
{ 0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x62, 0x66, 0xfe, 0x00, 0x00},// E
{ 0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x60, 0xf0, 0x00, 0x00},// F
{ 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xde, 0xc6, 0x66, 0x3a, 0x00, 0x00},// G
{ 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00},// H
{ 0x00, 0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xe6, 0x66, 0x6c, 0x6c, 0x78, 0x6c, 0x6c, 0x66, 0xe6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xf0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x0e, 0x00},
{ 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0xe6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x60, 0x38, 0x0c, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xd6, 0xfe, 0x6c, 0x6c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x7c, 0x38, 0x7c, 0xc6, 0xc6, 0xc6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xfe, 0xc6, 0x8c, 0x18, 0x30, 0x60, 0xc2, 0xc6, 0xfe, 0x00, 0x00},// Z
{ 0x00, 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x06, 0x02, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00},
{ 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},
{ 0x00, 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x78, 0x6c, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x1c, 0x0c, 0x0c, 0x3c, 0x6c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x1c, 0x36, 0x32, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xcc, 0x78},
{ 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x6c, 0x76, 0x66, 0x66, 0x66, 0xe6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c},
{ 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0xfe, 0xd6, 0xd6, 0xd6, 0xd6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0x0c, 0x1e},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0x60, 0xf0, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x70, 0x1c, 0xc6, 0x7c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x10, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x36, 0x1c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xd6, 0xd6, 0xfe, 0x6c, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 0x78},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xcc, 0x18, 0x30, 0x66, 0xfe, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x0e, 0x18, 0x18, 0x18, 0x70, 0x18, 0x18, 0x18, 0x0e, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x70, 0x18, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00},
{ 0x00, 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x00, 0x00, 0x00}
};

/**********************************************************************
* lcd init
***********************************************************************/

void lcd_init(void){
	lcd_reset();
        lcd_ssp_init();
	clrb_LCD_CS();	//select LCD
	lcd_wrcmd(0x00);//i0
	lcd_wrdata(0x01);
        send_lcd_init_data();//i1,2,3,4
        lcd_wrcmd(0x07);//i5
	lcd_wrdata(0x05);
	lcd_wrcmd(0x07);//i5
	lcd_wrdata(0x25);
	lcd_wrcmd(0x07);//i5
	lcd_wrdata(0x27);
	lcd_wrcmd(0x07);//i5
	lcd_wrdata(0x37);//
	setb_LCD_CS();	//deselect LCD
        glcdctrl_backlight_init();
        glcdctrl_SetContrast(10);
        fill_screen(0xffff);//white
	}
/**********************************************************************
* init SSP Interface
**********************************************************************/
void lcd_ssp_init(void) {
SSP_CR0 |= (DSS_08_BIT | (FRF_SPI<<4) & ~(CPOL<<6) & ~(CPHA<<7) | (SCR<<8)); // 8Bit Modus,SPI Modus,Clock Out Polarity,Clock Out Phase, 
SSP_CR1  = 0x00;            // SSP Master
SSP_CPSR = CPSDVSR;         // Clock Prescale Register This even value between 2 and 254
PINSEL1 |=  (2<<2) | (2<<4) | (2<<6);// | (2<<8) ;      // Map the SPI functionality to pins
SSP_CR1 |= SSE;             // Enable SSP module
SSP_IMSC = (RORIM|RTIM|RXIM|TXIM); //This register controls whether each of the four possible interrupt conditions in the SSP controller are enabled.
for (int i = 0; i < 8; i++ ){
  U8 Dummy = SSP_DR; /* clear the RxFIFO */
  }
}
/**********************************************************************
* send lcd init data
***********************************************************************/
void send_lcd_init_data(void){
U16 var3;
U16 var4;
	for (U8 i = 0; i <= 17; i++) { 
		U8 var = (U8)init1[i];
		lcd_wrcmd(var);
		i++;
		var = (U8)init1[i];
		U8 var2 = (U8)init1[i+1];
var3 = (U16)var;
var3 = (U16)var3<<8;
var4 = (U16)var2;
var3 = (U16)var3 + var4;
		lcd_wrdata(var3);
		i++;
	}
	for (U8 i = 0; i <= 5; i++) { 
		U8 var = (U8)init2[i];
		lcd_wrcmd(var);
		i++;
		var = (U8)init2[i];
		U8 var2 = (U8)init2[i+1];
var3 = (U16)var;
var3 = (U16)var3<<8;
var4 = (U16)var2;
var3 = (U16)var3 + var4;
		lcd_wrdata(var3);
		i++;
	}
	for (U8 i = 0; i <= 2; i++) { 
		U8 var = (U8)init3[i];
		lcd_wrcmd(var);
		i++;
		var = (U8)init3[i];
		U8 var2 = (U8)init3[i+1];
var3 = (U16)var;
var3 = (U16)var3<<8;
var4 = (U16)var2;
var3 = (U16)var3 + var4;

		lcd_wrdata(var3);
		i++;
	}
	for (U8 i = 0; i <= 62; i++) { 
		U8 var = (U8)init4[i];
		lcd_wrcmd(var);
		i++;
		var = (U8)init4[i];
		U8 var2 = (U8)init4[i+1];
var3 = (U16)var;
var3 = (U16)var3<<8;
var4 = (U16)var2;
var3 = (U16)var3 + var4;
		lcd_wrdata(var3);
		i++;
	}
}
/**********************************************************************
 * LCD
***********************************************************************/

void setb_LCD_RESET(void){
  F_IO0SET = LCD_RESET;
}
void clrb_LCD_RESET(void){
  F_IO0CLR = LCD_RESET;
}
void setb_LCD_RS(void){
  F_IO0SET = LCD_RS;
}
void clrb_LCD_RS(void){
  F_IO0CLR = LCD_RS;
}
void setb_LCD_CS(void){
  F_IO0SET = LCD_CS;
}
void clrb_LCD_CS(void){
  F_IO0CLR = LCD_CS;
}
void setb_LCD_CLK(void){
  F_IO0SET = LCD_CLK;
}
void clrb_LCD_CLK(void){
  F_IO0CLR = LCD_CLK;
}
void setb_LCD_DAT(void){
  F_IO0SET = LCD_DAT;
}
void clrb_LCD_DAT(void){
  F_IO0CLR = LCD_DAT;
}
/************************************************
 * LCD Reset
 ************************************************/
void lcd_reset(void){
	setb_LCD_RS();
        setb_LCD_CS();	
	clrb_LCD_RESET();
	clrb_LCD_CLK();
	clrb_LCD_DAT();
        pause(ONE_MS*2);
	setb_LCD_RESET();
        pause(ONE_MS*10);
	}
/************************************************
* LCD Powersave Mode & Wakeup
* Mode 1 = minimal power needed
* Mode 0 = wakeup LCD
***********************************************************************/
bool lcd_powersave(U8 mode){
  bool result = false;
    switch(mode){
    case 1://maximum Powersave Mode
      fill_screen(0xffff);//clearscreen
      pause(ONE_MS * 100);
      lcd_wrcmd(0x07);
      lcd_wrdata(0x0000);
      lcd_wrcmd(0x03);
      lcd_wrdata(0x0001);
      result = true;
      break;
    case 0:
      lcd_wrcmd(0x07);
      lcd_wrdata(0x37);
      lcd_wrcmd(0x03);
      lcd_wrdata(0x000c);
      lcd_wrcmd(0x05);
      lcd_wrdata(0x0008); // rotation
      send_pic("images/logo.bmp");
      result = true;
      break;
  }
  return result;
}
/***************************************************************************
 *
 ***************************************************************************/
void lcd_pulse(void){
	setb_LCD_CS();
        __asm ("nop");
 	clrb_LCD_CS();
	}
/************************************************
;send an LCD command
;************************************************/
void lcd_wrcmd(U8 data){
        spiSendByte(0x74);
	spiSendByte(0x00);
        spiSendByte(data);
        lcd_pulse();
	}
/***********************************************
;send LCD data
;************************************************/
void lcd_wrdata(U16 data){
        spiSendByte(0x76);
	spiTransferWord(data);
	lcd_pulse();
}
/***********************************************************************
*
***********************************************************************/
void lcd_comdat(U8 dat1, U8 dat2)
{
  spiSendByte(0x76);
  spiSendByte(dat1);
  spiSendByte(dat2);
  lcd_pulse();
}
void spi_wait_busy(void){

#   ifdef SPI_TIMEOUT
    uint8_t timeout = 200;
    while (SSP_SR & BSY && timeout > 0)
        timeout--;

    if (timeout == 0){
      }
#   else
    while (SSP_SR & BSY);
#   endif

} 
/***********************************************************************
*
***********************************************************************/
U8 spiSendByte(U8 data){
        // write data
	SSP_DR = data;
        spi_wait_busy();
        U8 Dummy = SSP_DR; /* Flush the RxFIFO */
  return Dummy;
}
/***********************************************************************
*
***********************************************************************/
void spiTransferWord(U16 data){
	SSP_DR = (data>>8);
        spi_wait_busy();
        U8 Dummy = SSP_DR; // Flush the RxFIFO
	SSP_DR = data;
        spi_wait_busy();
        Dummy = SSP_DR; // Flush the RxFIFO
  }
/***********************************************************************
* string out
***********************************************************************/
void lcd_send_string(char txt_string[]){
  U8 i=0;
  i = strlen(txt_string);
  if(rc >= (9*CHAR_H)){
  	rc=0;//rowcounter
	fill_screen(0xffff);//clearscreen
	}
  while (i != 0)
  {
    put_char(160-i*CHAR_W,h_row1 + rc,txt_string[i-1],0x0000,2);   // X ,Y , Text, rotation ok 0° Normal, lesbar von unten
	i--;
  };
	rc+=h_row2;
}

/***********************************************************************
* display a clock on lcd
*   U16 color = 0x00f2;
*   U8 x = 0-175
*   U8 y = 0-131
*   hanging point top right
*   U8 mode = 1 - hh:mm:ss
*   lcd_clock(x,y,mode,color);
*   lcd_clock(10,10,1,0x00f2);
***********************************************************************/
void lcd_clock(U8 x, U8 y, U8 mode, U16 color){
U8 sec1 = RTC_GetSEC();

//seconds 00
  if ( sec1 < 10){
        put_char(x+1*CHAR_W,y,'0',color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+0*CHAR_W,y,sec1+48,color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }else{
        char sec[2];
        utoa(sec1,sec,10);   // integer to string
        put_char(x+1*CHAR_W,y,sec[0],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+0*CHAR_W,y,sec[1],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }
//:
      put_char(x+2*CHAR_W,y,':',color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
//min 00
U8 min1 = RTC_GetMIN();
  if ( min1 < 10){
        put_char(x+4*CHAR_W,y,'0',color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+3*CHAR_W,y,min1+48,color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }else{
        char min[2];
        utoa(min1,min,10);   // integer to string
        put_char(x+4*CHAR_W,y,min[0],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+3*CHAR_W,y,min[1],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }
  //:
      put_char(x+5*CHAR_W,y,':',color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
//hour 00
U8 hour1 = RTC_GetHOUR();
  if ( hour1 < 10){
        put_char(x+7*CHAR_W,y,'0',color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+6*CHAR_W,y,hour1+48,color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }else{
        char hour[2];
        utoa(hour1,hour,10);   // integer to string
        put_char(x+7*CHAR_W,y,hour[0],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
        put_char(x+6*CHAR_W,y,hour[1],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
  }
}
/***********************************************************************
* version to lcd
***********************************************************************/
void version(void){
	lcd_send_string(model_txt);
}
/***********************************************************************
* value to lcd
*   U16 color = 0x00f2;
*	lcd_wr_val("ADC:",adcval,color);
***********************************************************************/
void lcd_wr_val(char prefix_text[], U32 val, U16 color){
  char temp[15];
  U8 i,j=0;

 utoa(val,temp,10);   // integer to string
 i= strlen(temp);
 j= strlen(prefix_text);

  while (j != 0){
    put_char(160-j*CHAR_W,h_row2,prefix_text[j-1],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
	j--;
  };

  while (i != 0){
    put_char(160-(strlen(prefix_text)*CHAR_W+CHAR_W)-(i*CHAR_W),h_row2,temp[i-1],color,lcd_rotation);   // X ,Y , Text, color, rotation ok 0° Normal, lesbar von unten
	i--;
  };
}
/***********************************************************************
* bar
* x,y
* showed value on the bar
* size of the bar
* color
* width of the bar
* rotation
* bar(50,50,38,60,blue,9,2);
***********************************************************************/
void bar(U8 x, U8 y,U16 val, U8 size, U16 color,U8 with, U8 rotation){
if (with < 2 || with > 170)
 with = 9; //
    if (with < 5){
    }else{
      line_vertical(x,y,y+with,black);// 
      line_vertical(x+size,y,y+with,black); // 
      line_horizontal(x,y,x+size,black); // 
      line_horizontal(x,y+with-1,x+size+1,black); // 
      with -= 3;//
    }
size -= 2;
val = (float)(val/(float)size)*5;// val in %
  clrb_LCD_CS();
  lcd_wrcmd(0x16);
  lcd_comdat(y+with,y+2);
  lcd_wrcmd(0x17);      
  lcd_comdat(x+val,x+2);
  lcd_wrcmd(0x21);      
  lcd_comdat(x+2,y+2);  
  lcd_wrcmd(0x22);      
  spiSendByte(0x76);
  // start data transmission  
    for (int i=0; i<with*val; i++){
     spiTransferWord(color);
  }
 	setb_LCD_CS();
        clrb_LCD_CS();
  lcd_wrcmd(0x16);            //Horizontal RAM Address Position (R16h)
  lcd_comdat(y+with,y+2);   //y1,y2 (HEA,HSA) Ensure 00h ?HSA7-0 ?HEA7-0 ?83h
  lcd_wrcmd(0x17);            //Vertical RAM Address Position (R17h)
  lcd_comdat(x+size, x+val+1);      //x1,x2(VEA,VSA) Ensure ?00?h ?VSA7-0 ?VEA7-0 ??AF?h
  lcd_wrcmd(0x21);            //Initially set GRAM addresses to the address counter (AC)
  lcd_comdat(x+val+1,y+2);            //"AF83"h (VSA,HSA) Vertical,Horizontal
  lcd_wrcmd(0x22);            //Write 16-bit data to the GRAM;
  spiSendByte(0x76);

  // start data transmission  
  for (int i=0; i< (with-1)*(size-val); i++){
    spiTransferWord(white);
	}
 	setb_LCD_CS();
}

/***********************************************************************
* rectangle
***********************************************************************/
void rectangle(U8 hight, U16 color){ // fill rectangle
 	clrb_LCD_CS();
  int x = 50;
  int y = 50;
  int W = 10;
  int H = hight;
      lcd_wrcmd(0x16);
      lcd_comdat(x+W-1, x);//y1,y2
      lcd_wrcmd(0x17);
      lcd_comdat(y+H-1, y);//x1,x2 
      lcd_wrcmd(0x21);
      lcd_comdat(y,x);
      lcd_wrcmd(0x22);

      spiSendByte(0x76);
  // start data transmission  
  for (int i=0; i<W*H; i++){
    spiTransferWord(color);
	}
 	setb_LCD_CS();
}
/***********************************************************************
* line_horizontal
***********************************************************************/
void line_horizontal(U8 x,U8 y, U8 x2, U16 color){ // 
 	clrb_LCD_CS();// select display
        int y2 = 1;
      lcd_wrcmd(0x16);
      lcd_comdat(y+y2-1, y);//y1,y2
      lcd_wrcmd(0x17);
      lcd_comdat(x+x2-1, x);//x1,x2 
      lcd_wrcmd(0x21);
      lcd_comdat(x,y);
      lcd_wrcmd(0x22);

      spiSendByte(0x76);
  // start data transmission  
  for (int i=0; i<x2-x; i++){
    spiTransferWord(color);
	}
      setb_LCD_CS(); // deselect display  
}
/***********************************************************************
* line_vertical
* line_vertical(10,10,20,red);
* paint a line from 10,10 to 10,20
***********************************************************************/
void line_vertical(U8 x,U8 y, U8 y2, U16 color){ 
      clrb_LCD_CS(); // select display
      lcd_wrcmd(0x16);
      lcd_comdat(y2,y);
      lcd_wrcmd(0x17);
      lcd_comdat(x,x );
      lcd_wrcmd(0x21);
      lcd_comdat(x,y);
      lcd_wrcmd(0x22);

      spiSendByte(0x76);
  // start data transmission  
  for (U8 i=0; i<y2-y; i++){
    spiTransferWord(color);
	}
	setb_LCD_CS();// deselect display 
}
/***********************************************************************
* send_pics in directory
* send a pic in format 5-6-5 to the lcd
* get it from the sd card
***********************************************************************/
    FIL fsrc;          // file objects
    BYTE buffer[102];  // file copy buffer, 3x bigger (3 color values)
    FRESULT res;       // FatFs function common result code
    WORD br;           // File R/W count
    DIR dir;	       /* Directory object */
    FILINFO finfo;

void send_pic_dir(void){
int en=ctl_global_interrupts_disable();
    f_mountdrv();

    res = f_opendir(&dir, "images");
      for(;;) {
      pause(ONE_MS*70);//pause between pics
              res = f_readdir(&dir, &finfo);
              if ((res != FR_OK) || \
              !finfo.fname[0] || \
              !finfo.fattrib & AM_HID || \
              !finfo.fattrib & AM_SYS || \
              !finfo.fattrib & AM_VOL || \
              !finfo.fattrib & AM_DIR || \
              !finfo.fattrib & AM_ARC \
                        ) break;

char str[20];
    strcpy(str, "");
    strcat(str, "images/");
    strcat(str, &(finfo.fname[0]));
          send_pic( str );
      }
ctl_global_interrupts_set(en);
}
/***********************************************************************
* send_anim_icon
* send a Animated standart icon.bmp to the lcd
* get it from the sd card inons folder
***********************************************************************/
void send_anim_icon(char file[]){
int x = 50;//x position on lcd
int y = 50;//y position on lcd
int W = 36;//must divide by 4
int H = 36;//must divide by 4
    
    res = f_open(&fsrc, file , FA_OPEN_EXISTING | FA_READ);
    if (res) res=NULL;

  clrb_LCD_CS();
      lcd_wrcmd(0x05);
      lcd_comdat(0x00,0x38); // rotation
      lcd_wrcmd(0x16);
      lcd_comdat(x+W-1, x);//y1,y2
      lcd_wrcmd(0x17);
      lcd_comdat(y+H-1, y);//x1,x2 
      lcd_wrcmd(0x21);
      lcd_comdat(y,x);
      lcd_wrcmd(0x22);
      spiSendByte(0x76); 

  // start data transmission  
  setb_LCD_CS();   
        res = f_read(&fsrc, buffer, 54, &br);//read 54 byte header , trash it
  clrb_LCD_CS();

for (;;) {
  setb_LCD_CS();   
        res = f_read(&fsrc, buffer, sizeof(buffer), &br);
        if (res || br == 0) break;      // error or eof
  clrb_LCD_CS();
  spiSendByte(0x76);// send futher pic data  

for (U16 i=0;i<br;i+=3){//br = number of bytes read 
    int p, r, g, b, c1, c2;
        r = ((buffer[2+i])<<16 & 0x00FF0000) >> 19;
        g = ((buffer[1+i]<<8) & 0x0000FF00) >> 10;
        b = (buffer[0+i] & 0x000000FF) >> 3;
        c1 = r << 3 | g >> 3;
        c2 = g << 5 | b;         
        spiTransferWord(c1<<8|c2);
}
         }
  setb_LCD_CS();   
// Close all files
    f_close(&fsrc);
}
/***********************************************************************
* send_pic
* send a standart pic.bmp from SD Card, convert it to 5-6-5 format 
* the dimmension must have 132x176 pixel
* send_pic("images/logo.bmp");
***********************************************************************/
void send_pic(char file[]){
    f_mountdrv();
    res = f_open(&fsrc, file , FA_OPEN_EXISTING | FA_READ);
    if (res) res=NULL;
  clrb_LCD_CS();
  lcd_wrcmd(0x05);
  lcd_comdat(0x00,0x08); // rotation
  lcd_wrcmd(0x16); // 
  lcd_comdat(131, 00);// 131,00
  lcd_wrcmd(0x17); // 
  lcd_comdat(175, 00);// 175,00
  lcd_wrcmd(0x21); // 
  lcd_comdat(0x00, 0x00);//
  lcd_wrcmd(0x22);

  spiSendByte(0x76);  
  // start data transmission  
  setb_LCD_CS();   
        res = f_read(&fsrc, buffer, 54, &br);//read header
  clrb_LCD_CS();
  for (;;) {
  setb_LCD_CS();   
        res = f_read(&fsrc, buffer, sizeof(buffer), &br);
        if (res || br == 0) break;      // error or eof
  clrb_LCD_CS();
  spiSendByte(0x76);// send futher pic data  

for (U16 i=0;i<br;i+=3){
    int p, r, g, b, c1, c2;
        r = ((buffer[2+i])<<16 & 0x00FF0000) >> 19;
        g = ((buffer[1+i]<<8) & 0x0000FF00) >> 10;
        b = (buffer[0+i] & 0x000000FF) >> 3;
        c1 = r << 3 | g >> 3;
        c2 = g << 5 | b;         
        spiTransferWord(c1<<8|c2);
}
         }

  setb_LCD_CS();   
// Close all files
    f_close(&fsrc);
}
/***********************************************************************
* fill_screen with the color
***********************************************************************/
void fill_screen(U16 color)
{
  U16 i;
  clrb_LCD_CS();
  lcd_wrcmd(0x05);
  lcd_comdat(0x00,0x38); // rotation
  lcd_wrcmd(0x16); // 
  lcd_comdat(131, 00);// 
  lcd_wrcmd(0x17); // 
  lcd_comdat(175, 00);// 
  lcd_wrcmd(0x21); // 
  lcd_comdat(0x00, 0x00);//
  lcd_wrcmd(0x22);

  spiSendByte(0x76);  
  
  // start data transmission  
  for (i=0; i<DISP_W*DISP_H; i++){
     spiTransferWord(color);
	 }
  setb_LCD_CS();   
}

/***********************************************************************
* writes a character to the lcd 
* put_char(10,10,"A",red,2); 
***********************************************************************/

void put_char(U8 x, U8 y, char c, U16 textcolor,U8 rotation)
{
  U16 backcolor=0xFFFF;    // color of background
  U8 h,ch,p,mask;
   	clrb_LCD_CS();
  switch(rotation)
  {
    default:
    case 1:
    case 2:
      lcd_wrcmd(0x05);
      lcd_comdat(0x00,0x38);
      lcd_wrcmd(0x16);
      lcd_comdat(y+CHAR_H-1, y);
      lcd_wrcmd(0x17);
      lcd_comdat(x+CHAR_W-1, x);
      lcd_wrcmd(0x21);
      lcd_comdat(x,y);
      lcd_wrcmd(0x22);
      spiSendByte(0x76);  
      break;
    case 3://
    case 4://
      lcd_wrcmd(0x05);
      lcd_comdat(0x00,0x30);
      lcd_wrcmd(0x16);
      lcd_comdat(x+CHAR_W-1, x);
      lcd_wrcmd(0x17);
      lcd_comdat(y+CHAR_H-1, y);
      lcd_wrcmd(0x21);
      lcd_comdat(y,x);
      lcd_wrcmd(0x22);
      spiSendByte(0x76);  
      break;
  }

  if (rotation==1||rotation==4){ 
	  for (h=0; h<CHAR_H; h++) // every column of the character
  	{        
       if (rotation==1)
          ch = (int)ascii_tab[ c-32 ][CHAR_H-h-1];
       if (rotation==4)
          ch = (int)ascii_tab[ c-32 ][h];	

        mask=0x80;
        for (p=0; p<CHAR_W; p++)  // write the pixels
        {
          if (ch&mask)
          {
            spiTransferWord(textcolor);                
          }
          else
          {
            spiTransferWord(backcolor);
          }
          mask=mask/2;
        }
  }
}
  if (rotation==2){ // writes from left to right
	  for (h=0; h<CHAR_H; h++) // every column of the character
	  {        
        ch = (int)(ascii_tab[ c-32 ][h]);	
        mask=0x01;
        for (p=0; p<CHAR_W; p++)  // write the pixels
        {
          if (ch&mask)
          {
            spiTransferWord(textcolor);                
          }
          else
          {
            spiTransferWord(backcolor);
          }
          mask=mask*2;
        }
  }
}
  if (rotation==3){
	  for (h=CHAR_H; h>0; h--) // every column of the character
 	 {        
        ch = (int)(ascii_tab[ c-32 ][h]);	
        mask=0x01;
        for (p=0; p<CHAR_W; p++)  // write the pixels
        {
          if (ch&mask)
          {
            spiTransferWord(textcolor);                
          }
          else
          {
            spiTransferWord(backcolor);
          }
          mask=mask*2;
        }
  }
}
 	setb_LCD_CS();
}
/***********************************************************************
* receives the pic data from udp port 86
* function is used from cmd_server.c 
***********************************************************************/

void start_pic(void){
  clrb_LCD_CS();
  lcd_wrcmd(0x05);
  lcd_comdat(0x00,0x30); //
  lcd_wrcmd(0x16); // 
  lcd_comdat(131, 00);// 
  lcd_wrcmd(0x17); // 
  lcd_comdat(175, 00);// 
  lcd_wrcmd(0x21); // 
  lcd_comdat(0x00, 0x00);// 
  lcd_wrcmd(0x22);
  spiSendByte(0x76);  
  setb_LCD_CS();
}

void send_pic_data(void){

        clrb_LCD_CS();
        lcd_wrcmd(0x22);
  	spiSendByte(0x76);  
 	for (int i=0; i <= uip_datalen(); i++){ //UDP_DATA
	 spiSendByte(uip_buf[i+54]);
        }
         setb_LCD_CS();
}


