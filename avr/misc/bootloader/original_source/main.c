/*****************************************************************************
*
* Copyright (C) 1996-1998 Atmel Corporation
*
* File          : main.c
* Compiler      : IAR EWAAVR 2.26c
* Output size   : 
* Created       : 16-jun-99
* Modified      : 5-jan-2000
* Modified      : 17-April-2002 by AS
* Modified      : 1-March-2002 by RAA
*
* Support mail  : avr@atmel.com
*
* Description   : This Program allows an AVR with bootloader capabilities to 
*                 Read/write its own Flash/EEprom. To enter Programming mode   
*                 PD4 is checked. If this pin is pulled low, programming mode  
*                 is entered. If not, normal execution is done from $0000 
*                 "reset" vector in Application area.
*                 The PD4 pin should be pulled HIGH by an external Pull-up 
*                 resistor.
*
* Other info    : The defines.h file must be set up for individual devices
*                 using the excel-sheet preprocessor
*
*                 In general the linker file should always be verifyed to match
*                 the used part's boot address and size. Note that memory size
*                 is specified in bytes in the linker file.
****************************************************************************/
#include "defines.h"
#include "serial.h"
#include "assembly.h"


__C_task void main(void)
{
    unsigned int intval,address,data;
    unsigned char val, ldata;

    void (*funcptr)( void ) = 0x0000;       // Set up function pointer to RESET vector
    initbootuart();                           // initialize uart
    PROGPORT |= PROGCTRL;                   // enable pull-up on PROGCTRL line on PROGPORT


    if( PROGMODE )                          // If PROGPIN is pulled low: programmingmode. 
    {  
      for(;;)                           
      {
        val=recchar();

        if(val=='a')                        //Autoincrement?
        {
          sendchar('Y');		    //Autoincrement is quicker
        }
 
        else if(val=='A')                   //write address 
        {
          address=recchar();                //read address 8 MSB
          address=(address<<8)|recchar();
          
          #ifdef LARGE_MEMORY
          if(address >= 0x7FFF)
            RAMPZ = (1<<RAMPZ0);
          else
            RAMPZ = 0;
          #endif            
        
          address = address << 1;            //convert from word address to byte address
          sendchar('\r');
        }

        else if(val=='c')                   //Write program memory, low byte
        {       
          ldata=recchar();
          sendchar('\r');
        }

        else if(val== 'C')                  //Write program memory, high byte 
        {
          data=ldata | (recchar() << 8);
          fill_temp_buffer(data,(address)); //call asm routine. 
          address += 2;  
          sendchar('\r');
        }
        
        else if(val=='e')                   //Chip erase 
        {
          #ifdef LARGE_MEMORY               //SPM uses Z pointer but the pointer is only 16bit and
          RAMPZ = 0x00;                     //can only address up to 64Kbytes FLASH to adresse higher 
                                            //location it require the use of RAMPZ

          for(address=0;((address < (unsigned int)(APP_END&0xFFFF))|(RAMPZ == 0x00));address += PAGESIZE)
          {
            write_page(address,(1<<PGERS) + (1<<SPMEN));              //Perform page erase
            write_page(address, (1<<REENABLE_RWW_BIT) + (1<<SPMEN));  //Re-enable the RWW section

            if(address >=(0xFFFF-PAGESIZE)) //Last section on lower 64k segment is erased
               RAMPZ = (1<<RAMPZ0);         //RAMPZ has to incremented into upper 64k segment
          }
          RAMPZ = 0x00;                     //Clear RAMPZ pointer
          
          #else

          for(address=0;address < APP_END;address += PAGESIZE)  //Application section = 60 pages
          {
            write_page(address,(1<<PGERS) + (1<<SPMEN));       //Perform page erase
            write_page(address, (1<<REENABLE_RWW_BIT) + (1<<SPMEN));  //Re-enable the RWW section
          }

          #endif          
          sendchar('\r');        
        }

        else if(val=='l')                   // write lockbits 
        {
          write_lock_bits(recchar());
          sendchar('\r');
        }
       
        else if(val== 'm')                  // write page
        {
          write_page(address, (1<<PGWRT) + (1<<SPMEN));       //Perform page write
          write_page(address, (1<<REENABLE_RWW_BIT) + (1<<SPMEN));  //Re-enable the RWW section

          sendchar('\r');
        }
        
        else if((val=='P')||(val=='L'))     // Enter programming mode 
        {
          sendchar('\r');
        }

        else if (val=='p')
        {
          sendchar('S');
        }        
        
        else if(val=='R')                   //Read program memory 
        {        
          intval = read_program_memory(address,0x00);
          sendchar((char)(intval>>8));      //send MSB  
          sendchar((char)intval);           //send LSB

          address+=2;                       //SPM uses Z pointer but the pointer is only 16bit
          #ifdef LARGE_MEMORY               //and can only address up to 64Kbytes FLASH to adresse
          if(address == 0x0000)             //higher location it require the use of RAMPZ
          {
            RAMPZ++;                       //RAMPZ has to incremented into upper 64k of FLASH memory
            RAMPZ &=(1<<RAMPZ0);           //Mask away any non used bits
          }
          #endif

        }
        
        else if (val == 'D')
        {
          EEARL = address;
          EEARH = (address >> 8);
          address++;
          EEDR = recchar();
          EECR |= (1<<EEMWE);
          EECR |= (1<<EEWE);
          while (EECR & (1<<EEWE))
            ;
          sendchar('\r');
        }

        else if (val == 'd')
        {
          EEARL = address;
          EEARH = (address >> 8);
          address++;
          EECR |= (1<<EERE);
          sendchar(EEDR);
        }
               
        else if(val=='F')                   // read fuse bits
        {
          #ifdef LARGE_MEMORY
          RAMPZ = 0;
          #endif
          sendchar(read_program_memory(0x0000,0x09));
        }        

        else if(val=='r')                   // read lock bits
        {
          #ifdef LARGE_MEMORY
          RAMPZ = 0;
          #endif
          sendchar(read_program_memory(0x0001,0x09));
        }        

        else if(val=='N')                   // read high fuse bits
        {
          #ifdef LARGE_MEMORY
          RAMPZ = 0;
          #endif
          sendchar(read_program_memory(0x0003,0x09));
        }        

        else if(val=='t')                   // Return programmer type 
        {
          sendchar( PARTCODE );
          sendchar( 0 );
        }

        else if ((val=='x')||(val=='y')||(val=='T'))
        {
          recchar();
          sendchar('\r');
        }
       
        else if (val=='S')                  // Return software identifier 
        {
          sendchar('A');
          sendchar('V');
          sendchar('R');
          sendchar('B');
          sendchar('O');
          sendchar('O');
          sendchar('T');
        }                
        
        else if (val=='V')                  // Return Software Version
        {
          sendchar('1');
          sendchar('0');
        }        

        else if (val=='s')                  // Return Signature Byte
        {							
          sendchar( SIGNATURE_BYTE_1 );
          sendchar( SIGNATURE_BYTE_2 );
          sendchar( SIGNATURE_BYTE_3 );
        }       

        else if(val!=0x1b)                  // if not esc
        {
          sendchar('?');
        }
      }
    }
    else
    {
      funcptr();        					// Jump to Reset vector 0x0000 in Application Section
    }
}
