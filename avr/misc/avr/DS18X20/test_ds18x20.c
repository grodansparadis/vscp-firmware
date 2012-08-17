
/*********************************************************************************************************
 Title  :   C test file for the ONE_WIRE FUNCTIONS and the DS1820 libraries (test_ds1820.c)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      2/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/


#include <io.h>
#include <eeprom.h>
#include "ds18x20.h"
#include "lcd_io.h"
#include "one_wire.h"





/********************************************************************************************************/
/*                                   MAIN FUNCTION                                                      */
/********************************************************************************************************/
void main(void)
{
static unsigned char eeprom_rom_codes[8][8]__attribute__((section(".eeprom")));
unsigned char        rom_code[8];
signed int           temperature=0;
unsigned char        x=0, temp1=0;


/*------------------------------------------------------------------------------------------------------*/

/*
   BEWARE! CHANGE OF PORT!!!!  if you dont use the ow_config_io() function or it is not enabled
                               then the settings in one_wire.h apply. 
*/
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
ow_config_io(PORTB,0);
#endif
/* First let's check for any error */
ow_reset();
if(ow_get_msg()==OW_BUS_ERROR) 
 {
   lcd_clrscr();
   lcd_puts_P("1 WIRE BUS ERROR ");  
   eeprom_wb((unsigned int)(eeprom_rom_codes[0]), 0);
   return;
 } 
else if(ow_get_msg()==OW_ALARM_DETECTED) 
      {
        lcd_clrscr();
        lcd_puts_P("OW_NO_DEVICE_PRESENT ");  
        eeprom_wb((unsigned int)(eeprom_rom_codes[0]), 0);
        return;
      }
else if(ow_get_msg()==OW_ALARM_DETECTED) 
      {
        lcd_clrscr();
        lcd_puts_P("ALARM DETECTED");  
        eeprom_wb((unsigned int)(eeprom_rom_codes[0]), 0);
        return;
      } 
/*------------------------------------------------------------------------------------------------------*/

/* 
   Now let's find out the rom codes of the DS18X20 devices. The rom codes are saved in the eeprom 
   I intensionally use different methods for reading the eeprom so i can test better.
   Up to whatever the eeprom array can hold, rom codes are saved (sizeof(eeprom_rom_codes)/8).
*/ 

/* if SEARCH_ROM has not been performed yet ( that way search_rom is done only once) */
temp1=eeprom_rb((unsigned int)(&eeprom_rom_codes[0][0])); 

if(temp1 == 0 || temp1 == 0xFF)
 {
    /* Reset the search_rom() function (It is not needed if it is the very first function call) */
    ow_search_rom(rom_code, OW_RESET_SEARCH_ROM);

    do{
         if(x >= (sizeof(eeprom_rom_codes)/8)) { ow_search_rom(rom_code, OW_RESET_SEARCH_ROM); break; }
         x=ow_search_rom(rom_code, OW_SEARCH_ROM);
         if(x==0) { break; }

         /* save the key found in the eeprom (max sizeof(eeprom_rom_codes)/8 rom codes) */
         for(temp1=0; temp1<8; temp1++)
           {
              eeprom_wb((unsigned int)(eeprom_rom_codes[x-1]+temp1), rom_code[temp1] );
           }

      }while(ow_get_msg()!= OW_SEARCH_ROM_FINISHED);

    lcd_clrscr();
    lcd_clrline(0);
    lcd_puts_P("Rom Codes found= ");
    lcd_puti(x,0);
    lcd_clrline(1);
    lcd_puts_P("First Rom Code is  ");
    lcd_gotoxy(0,2);
    for(temp1=0; temp1<4; temp1++)
      {
         lcd_put_i(eeprom_rb((unsigned int)(eeprom_rom_codes[0]+temp1)),0,3);
         if(temp1<3) { lcd_putc('-'); }
      }
    lcd_gotoxy(0,3);
    for(temp1=4; temp1<8; temp1++)
      {
         lcd_put_i(eeprom_rb((unsigned int)(eeprom_rom_codes[0]+temp1)),0,3);
         if(temp1<7) { lcd_putc('-'); }
      }    

    DELAY_MS(10000);  /* delay 10 seconds so you can read the SEARCH_ROM screen */
 }
/*------------------------------------------------------------------------------------------------------*/

/*
   The eeprom_rom_codes array now contains the just found rom codes. So we will now check the family
   code of the first rom code in search of DS18X20 (only one ds18x20 is connected in my test setup).
*/
temp1=eeprom_rb((unsigned int)(&eeprom_rom_codes[0][0])); 
if(temp1 != DS18B20 && temp1 != DS18S20 )       
 {
    if(x==0) { lcd_clrscr(); lcd_gotoxy(0,1); lcd_puts_P("NO 1W DEVICE FOUND "); }
    else{ lcd_clrline(0); lcd_puts_P("  NO DS1820 FOUND  "); }
    eeprom_wb((unsigned int)(eeprom_rom_codes[0]), 0);
    return;
 } 
/*------------------------------------------------------------------------------------------------------*/

/* Now lets set some DS18X20 registers that set varius parameters */

/* ds18x20_select_device(address, LOCATION_IS_X);  X= RAM, EEPROM, FLASH) */
#if ONLY_1_DEVICE_ON_THE_BUS == 0
ds18x20_select_device(eeprom_rom_codes[0], LOCATION_IS_EEPROM);
#endif   

/* Set high and low thresholds and the resolution if applicable */
ds18x20_set_reg(DS18X20_T_HIGH, 60);       
ds18x20_set_reg(DS18X20_T_LOW,  10);
ds18x20_set_reg(DS18X20_CONFIG, DS18X20_12_BIT);

/*------------------------------------------------------------------------------------------------------*/

/* It is time to configure the visual human interface (yes i mean the LCD display) */

lcd_clrscr();
lcd_clrline(0);
lcd_puts_P("Temperature Celsius");

/* Display the number of the rom code selected. */
lcd_clrline(3);
#if ONLY_1_DEVICE_ON_THE_BUS == 0
lcd_puts_P("First Rom is used! ");
#else
lcd_puts_P("1 device mode !    ");
#endif

/*------------------------------------------------------------------------------------------------------*/

/* Finally we can make a loop that takes temperature measurements for ever */

while(1)
    {
       temperature=ds18x20_get_temp();
       lcd_gotoxy(8,1); 
       lcd_puti(temperature,1);

    }


return;
}

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

