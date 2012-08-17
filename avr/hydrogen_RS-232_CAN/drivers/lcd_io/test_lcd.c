/****************************************************************************
 Title  :   C Test program for the LCD FUNCTIONS library (test_lcd.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      29/Sep/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#include <io.h>
#include "lcd_io.h"


  /* USER DEFINED CHARS ARRAY's */
static const unsigned char user_char1[8]__attribute__((progmem)) = {0x0E,0x11,0x0E,0x04,0x1F,0x04,0x0A,0x11}; 
static const unsigned char user_char2[8]__attribute__((progmem)) = {0x11,0x0A,0x04,0x1F,0x04,0x0E,0x11,0x0E};

/*
You could also define a char array like this:
static const unsigned char user_chars[2][8]__attribute__((progmem))={
                                                                      {0x0E,0x11,0x0E,0x04,0x1F,0x04,0x0A,0x11},
                                                                      {0x11,0x0A,0x04,0x1F,0x04,0x0E,0x11,0x0E}
                                                                    };
or                                                 

static const unsigned char user_chars[]__attribute__((progmem))={
                                                                  0x0E,0x11,0x0E,0x04,0x1F,0x04,0x0A,0x11,
                                                                  0x11,0x0A,0x04,0x1F,0x04,0x0E,0x11,0x0E
                                                                };
    
    and use the macro "lcd_fill_cgram(user_chars);"  instead of uploading each char separately.
    The array can have from 1 up to 8 used defined chars (max 64 bytes) but the macro will upload only
    as much as the array size so in the above example it will fill user defined char positions 0 and 1
    and will not overwrite user defined chars that might exist located at position 2 to 7.
    However it will always start uploading at position 0.

*/



void main(void)
{

/*
    Since we also test the eeprom DONT! forget to write the test_lcd.eep to AVR's eeprom 
    otherwise the lcd will not show the eeprom_string
*/
  static  unsigned char eeprom_string[]__attribute__((section(".eeprom")))={"lcd_puts_e testing"};
  unsigned char ram_string[]={"DONE TESTING..."};
  unsigned int pos=0;

/* 
    lcd(s) is(are) automatically initialized and if more than 1, lcd unit 0 gets active.
    You can always call "lcd_init()" if you wish.
 */

   lcd_clrscr();

/* 
   UPLOAD USER DEFINED CHARS TO LCD's CGRAM AND THEN RETURN TO THE PREVIUS DDRAM CURSOR POSITION
   (It can be made anywhere as long you upload them before you use them)
*/
   lcd_putc_cgram(user_char1,0);
   lcd_putc_cgram(user_char2,1);

   lcd_gotoxy(0,0);  lcd_puts_P("NOT VISIBLE LINE");
   lcd_clrline(0);   lcd_puts_P( "Hello World\nthis is line 2 " );

   /* DISPLAY USER DEFINED CHARS */
   lcd_putc(0);
   lcd_putc(1);

/*---------------------------------------------------------------------------------------------------*/
#if   LCD_IO_MODE == 7
   select_lcd(LCD_1);
   lcd_clrscr();
   lcd_clrline(0);   lcd_puts_P( "This is display #2" );
   select_lcd(LCD_0);
#endif
/*---------------------------------------------------------------------------------------------------*/

#if LCD_AUTO_LINE_FEED == 1
   lcd_gotoxy(0,2);  lcd_puts_e(eeprom_string); lcd_puts(ram_string);  
#elif LCD_AUTO_LINE_FEED == 0
/*---------------------------------------------------------------------------------------------------*/
/*
   I puted this command here to test the correct saving of the lcd x,y coordinates
   between lcd unit switching, if you select mode 7 
*/
   lcd_gotoxy(0,2);

   lcd_puts_e(eeprom_string);
   
   lcd_gotoxy(0,3); lcd_puts(ram_string);  
#endif /* #elif LCD_AUTO_LINE_FEED == 0 */
   
   lcd_gotoxy(16,3); lcd_puti(pos,2);  pos=lcd_getxy(); 
   lcd_gotoxy(16,3); lcd_put_i(pos,2,4);  
  
}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

