/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : lcd.h                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef LCD_H
#define LCD_H

/* do not do timeout while waiting for spi transfer completed */
#define SPI_TIMEOUT 1

#define DISP_W 132
#define DISP_H 176
#define CHAR_H 14
#define CHAR_W 8
#define h_row1 4 // horizontal first row
#define h_row2 CHAR_H // horizontal frist row
#define white 0xffff//white
#define black 0x0000//black
#define blue  0x001f//blue
#define green 0x07e0//green
#define red   0xf800//red

void setb_LCD_CS(void);
void clrb_LCD_CS(void);
void lcd_pulse(void);
void lcd_wrcmd(U8 data);
void lcd_wrdata(U16 data);
void send_lcd_init_data(void);
void put_char(U8 x, U8 y, char c, U16 textcolor,U8 rotation);
void lcd_send_string(char txt_string[]);
void fill_screen(U16 color);
void lcd_reset(void);
void lcd_ssp_init(void);
void lcd_init(void);
void line_horizontal(U8 x,U8 y, U8 x2, U16 color);
void line_vertical(U8 x,U8 y, U8 y2, U16 color);
void version(void);
void start_pic(void);
void send_pic_data(void);
void stop_pic(void);
U8 spiSendByte(U8 data);
void spiTransferWord(U16 data);
void rectangle(U8 hight, U16 color);
void lcd_clock(U8 x, U8 y, U8 mode, U16 color);
void bar(U8 x, U8 y, U16 val, U8 size, U16 color,U8 with, U8 rotation);
void lcd_wr_val(char prefix_text[], U32 val, U16 color);
void send_pic(char file[]);
void send_pic_dir(void);
void send_anim_icon(char file[]);
bool lcd_powersave(U8 mode);
#endif
