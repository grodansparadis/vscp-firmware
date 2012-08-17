/************************************************************************************************
 Title  :   C Test program for the HEX KEYBOARD FUNCTIONS library (test_hex_keyb.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      6/Jun/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

************************************************************************************************/

#include <io.h>
#include <eeprom.h>
#include "hex_keyb.h"
/* THE LCD DRIVER IS INCLUDED AUTOMATICALLY  */

void main(void)
{
#if HKEYB_SCAN_FUNCTION_ONLY == 0 

  unsigned char key=0, c=0, x=0, test=0, key_buffer[11];
  unsigned int  number_buf=0, number=0;
    
   LCD_INIT();
   LCD_CLRSCR();
static struct bat_lib
             { char battery_name[11];
               unsigned char lib_battery_voltage;
               unsigned char lib_battery_capacity;
               unsigned char lib_charging_time;
             } 
             battery_lib[9] __attribute__((section(".eeprom")));    


while(1)
    {
      
      while(1)
          {   
            LCD_CLRSCR();
            LCD_GOTOXY(0,0);  LCD_PUTS_P( "Press a key         " );
            key=kgetc();
            LCD_GOTOXY(0,1);  LCD_PUTS_P( "You pressed: " );
            LCD_GOTOXY(15,1); LCD_PUTC(key);
            LCD_GOTOXY(0,2);  LCD_PUTS_P( "Hit ESCAPE to exit\nor ENTER to repeat" );
            while(1)
                {
                   key=kgetc();  if(key==ENTER) break; else if(key==ESCAPE) goto TEST2;
                }  
            
          }          
      
TEST2:
        while(1)
           {
              key=0;
              LCD_CLRSCR();  LCD_GOTOXY(0,0); LCD_PUTS_P("Enter a number: ");
              test=kgeti(&number_buf,3,0,1);
              LCD_GOTOXY(0,2); LCD_PUTS_P("You typed: "); 
              /* if the user did not pressed ESCAPE */
              if(test) number=number_buf; LCD_PUTI(number,0);
              LCD_PUTS_P("\nPress any key!");
              key=kgetc();
              key=0;
              while(1)
                  {
                    LCD_CLRSCR();  LCD_GOTOXY(0,0); LCD_PUTS_P("Enter the password!");
                    kgeti(&number_buf,0,0,1);
                    if(number_buf==1234)
                     {
                       LCD_GOTOXY(0,2); LCD_PUTS_P("CORRECT!\nPress any key!");
                       key=kgetc(); break;
                     }
                    else
                       {
                         LCD_GOTOXY(0,2); LCD_PUTS_P("WRONG TRY AGAIN!\nPress any key!");
                         key=kgetc();
                       }
                  }
              key=0;
              LCD_CLRSCR(); 
              LCD_PUTS_P("Hit ENTER to repeat\nor hit ESCAPE to \nproceed to the \nnext TEST"); 
              while(1)
                  {
                    key=kgetc();  if(key==ENTER) break; else if(key==ESCAPE) goto TEST3;
                  }
           }
      
TEST3:   
      LCD_CLRSCR(); LCD_GOTOXY(0,0); LCD_PUTS_P( "Type something: " );
      kgets(key_buffer,8,0,1);
      LCD_CLRSCR();
      LCD_GOTOXY(0,0); LCD_PUTS_P("You typed: "); LCD_PUTS(key_buffer);
 /* write buffer to eeprom structure and display it to lcd */     
     x=0; key=0;
     while( (c=key_buffer[x]) ) 
          {
            eeprom_wb((unsigned int)&battery_lib[0].battery_name[x],c);
            x++;
          }
      eeprom_wb((unsigned int)&battery_lib[0].battery_name[x],'\0');  
     
      LCD_GOTOXY(0,1); LCD_PUTS_P("Eeprom:    "); LCD_PUTS_E((char*)&battery_lib[0].battery_name);
      LCD_GOTOXY(0,3); LCD_PUTS_P("Press any key!");
      key=kgetc();
      LCD_CLRSCR();
      LCD_GOTOXY(0,0);
              LCD_PUTS_P("Hit ENTER to repeat\nthe TESTS one more\ntime !"); 
              while(1)
                  {
                    key=kgetc();  if(key==ENTER) break; 
                  }
      
    }

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

