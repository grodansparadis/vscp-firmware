/****************************************************************************
 Title  :   C  file for the HEX KEYBOARD FUNCTIONS library (hex_keyb.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif



#include  <io.h>
#include <progmem.h>
#include <stdlib.h>
#include "hex_keyb.h"


#if defined(__AVR_ATmega103__)
#define DDRC   0
#define DDRF   0
#endif

#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

#define KEYB_PORT_DDR  CONCAT1(DDR, KEYBOARD_PORT)
#define KEYB_PORT_PIN  CONCAT1(PIN, KEYBOARD_PORT)
#define KEYB_PORT      CONCAT1(PORT,KEYBOARD_PORT)

/* CONVERSION OF TRUE TIME TO A VALUE SUITABLE FOR THE kgets() FUNCTION */ 
#define CURSOR_ADVANCE_VALUE   CURSOR_ADVANCE_TIME*10

/* CONVERSION OF TRUE TIME TO A VALUE SUITABLE FOR THE DELAY FUNCTION */
#define DELAY_100_MS        ((100*(F_CPU/6000))-3)
#define DELAY_30_MS         ((30*(F_CPU/6000))-3)
#define DELAY_10_MS         ((10*(F_CPU/6000))-3)


/*******************************************************************************************************/
/* SOME ERROR CHECKING WHEN USING MEGA 103 */
#if defined(__AVR_ATmega103__)
#if KEYB_PORT_DDR == 0
#error "THE KEYBOARD PORT MUST BE BIDIRECTIONAL!"
#endif
#endif /* #if defined(__AVR_ATmega103__) */
/*******************************************************************************************************/

/* type definitions */
typedef unsigned char u08;
typedef char          s08;
typedef unsigned int  u16;
typedef int           s16;

/*-------------------------------------------------------------------------------*/
static void wait(unsigned long number_of_loops) 
{  
/* 
   6 cpu cycles per loop + 17 cycles overhead(18 are pre-subtracted in the definition of DELAY).
   (4 to load the us variable, 4 to call the delay function, 5 to test for us == 0 and 4 to return. 
*/
   while(number_of_loops) { number_of_loops--; }
   
}
/*------------------------------------------------------------------------------------------------------*/
unsigned char scan_keyb(void)
{ 

unsigned char     rows=0, columns=0, key=0, keyb_input=0;
   
   rows=(unsigned char)(~( 1<<ROW4 | 1<<ROW3 | 1<<ROW2 | 1<<ROW1 ));
   columns=(unsigned char)(~( 1<<COL4 | 1<<COL3 | 1<<COL2 | 1<<COL1 ));

      outp(rows,KEYB_PORT_DDR);                          /* CHECK ROWS FOR KEY DOWN */
      outp((~rows),KEYB_PORT);
      /* Give time for the pins to settle, due to keyboard and wiring capacitance */
      wait(DELAY_10_MS);    
      keyb_input=~( inp(KEYB_PORT_PIN) | rows);     
      if(keyb_input) 
       {                                                /* MAIN IF START (DEBOUNCE AND CHECK AGAIN) */
         wait(DELAY_30_MS);                             /* DEBOUNCE */
         outp(rows,KEYB_PORT_DDR);                      /* CHECK ROWS  */
         outp((~rows),KEYB_PORT);
         /* Give time for the pins to settle, due to keyboard and wiring capacitance */    
         wait(DELAY_10_MS);   
         keyb_input=~( inp(KEYB_PORT_PIN) | rows);
         
         outp(columns,KEYB_PORT_DDR);                     /* CHECK COLUMNS  */ 
         outp((~columns),KEYB_PORT);
         /* Give time for the pins to settle, due to keyboard and wiring capacitance */
         wait(DELAY_10_MS);   
         keyb_input|=~( inp(KEYB_PORT_PIN) | columns);
         switch(keyb_input)                           /* KEY CODE DEFINITION AND ROLLOVER FILTER   */
              {
                case(1<<COL1 | 1<<ROW1): key=KEY1;  break;
                case(1<<COL2 | 1<<ROW1): key=KEY2;  break; 
                case(1<<COL3 | 1<<ROW1): key=KEY3;  break;
                case(1<<COL4 | 1<<ROW1): key=KEY4;  break; 
                case(1<<COL1 | 1<<ROW2): key=KEY5;  break;
                case(1<<COL2 | 1<<ROW2): key=KEY6;  break; 
                case(1<<COL3 | 1<<ROW2): key=KEY7;  break;
                case(1<<COL4 | 1<<ROW2): key=KEY8;  break; 
                case(1<<COL1 | 1<<ROW3): key=KEY9;  break;
                case(1<<COL2 | 1<<ROW3): key=KEY10; break; 
                case(1<<COL3 | 1<<ROW3): key=KEY11; break;
                case(1<<COL4 | 1<<ROW3): key=KEY12; break; 
                case(1<<COL1 | 1<<ROW4): key=KEY13; break;
                case(1<<COL2 | 1<<ROW4): key=KEY14; break; 
                case(1<<COL3 | 1<<ROW4): key=KEY15; break;
                case(1<<COL4 | 1<<ROW4): key=KEY16; break;  
                default: key=0;                                                                                                                                                      
              }
       }     /* END OF MAIN IF  (DEBOUNCE AND CHECK AGAIN)       */     

/* set all keyb port pins as inputs, pullups off, in case you are multiplexing the keyboard pins
   with other devices. You can multiplex only 4 pins (either the row or the column pins, not mixed!)
   That is because if you multiplex all lines when someone hits a key the correspondig row and
   column lines will be shorted thus altering the values on the pins. */
outp(0, KEYB_PORT_DDR); outp(0, KEYB_PORT);      
return(key); 
}

/********************************************************************************************************/
#if HKEYB_SCAN_FUNCTION_ONLY == 0 

void kgets(unsigned char *data, unsigned char max_chars, unsigned char x, unsigned char y)
{
 
unsigned char buffer=0, aux_buffer1=0, aux_buffer2=0;                          /* char buffers          */
unsigned char index=0, index_counter=0,  max=3, capital=0;                     /* char array controls   */
unsigned char backspace_hit_counter=0, keystroke_counter=0;                    /* char editor controls  */
                                          
#if ACTIVATE_AUTO_ADVANCE == 1 
unsigned char auto_advance=0;                                                 /* auto advance semaphore */
unsigned int  auto_advance_counter=0;                                         /* cursor advance timer   */
#endif

/* char array containing the keyboard characters */
static const char  key1[7]__attribute__((progmem))=  {' ','?','!',',','.',':','1' };
static const char  key2[4]__attribute__((progmem))=  {'a','b','c','2' };
static const char  key3[4]__attribute__((progmem))=  {'d','e','f','3' };
static const char  key4[4]__attribute__((progmem))=  {'g','h','i','4' };
static const char  key5[4]__attribute__((progmem))=  {'j','k','l','5' };
static const char  key6[4]__attribute__((progmem))=  {'m','n','o','6' };
static const char  key7[5]__attribute__((progmem))=  {'p','q','r','s','7' };
static const char  key8[4]__attribute__((progmem))=  {'t','u','v','8' };
static const char  key9[5]__attribute__((progmem))=  {'w','x','y','z','9' };
static const char key10[7]__attribute__((progmem))=  {'0','+','-','*','/','%','#' };

/* Because the lcd display advances automatically every time a char is displayed
   a lcd_gotoxy() command must be issued after any lcd_put command 
   in order to reposition the cursor to the desired position!!!                  */

/* Initialization */ 
for(index=0;index<=max_chars;index++) { *(data+index)='\0'; } /* Fill buffer with nulls */
if(max_chars) max_chars=max_chars-1; else return;             /* Adjust the char limit  */ 
index=0;
LCD_CURSOR_ON_BLINK(); 
     
/*------------------------------------------------------------------------------------------------------*/
while(1)  
    {  /*main loop start */
/* position cursor, wait for all keys to be released and wait 0,1 sec to prevent key double click */
      LCD_GOTOXY(x, y);  while((buffer=scan_keyb())); wait(DELAY_100_MS);       
                                                   
#if ACTIVATE_AUTO_ADVANCE == 1 
      while( !(buffer=scan_keyb()) )                         /* Cursor auto advance routine */
          {
            
            auto_advance_counter++;
            if( auto_advance && (auto_advance_counter>=CURSOR_ADVANCE_VALUE) && index<max_chars )
             {
               if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
               else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
               index++; 
               auto_advance=0;  keystroke_counter=0; backspace_hit_counter=1;
               aux_buffer1='\0'; index_counter=0;
             }
          }
#else
      while( !(buffer=scan_keyb()) );                              /* Just wait untill a key is pressed */
#endif                
/*------------------------------------------------------------------------------------------------------*/
      if( buffer==ENTER )                              /* ENTER */
       { 
         for(index=(max_chars+1);index;index--)        /* Trim trailing spaces  */
           { 
             if( *(data+index)=='\0' || *(data+index)==' ' ) { *(data+index)='\0'; } else { break; }
           }
         break;
         
       }     /* Terminate string and exit */
       
/*------------------------------------------------------------------------------------------------------*/
      /* ESCAPE  Turn off cursor and set string to zero */
      else if( buffer==ESCAPE ) { *data='\0'; break; }
             
/*------------------------------------------------------------------------------------------------------*/
      else if( buffer==CAPITAL ) { capital=~capital;  }  /* a->A */           

/*------------------------------------------------------------------------------------------------------*/
      else if(buffer==BACKSPACE)                         /* B  BACKSPACE                              */
            {                                     
              *(data+index)=' ';                         /* write a NULL to string and a space to lcd */
              LCD_PUTC(' '); 
              backspace_hit_counter++;                   /* BACKSPACE consecutive hit counter         */
              if(backspace_hit_counter==2 && index)      /* backwards movement control                */
               {
                 index--; 
                 if((x==0) && (y!=0)) { x=(LCD_CHARS_PER_LINE-1) ; y--; } else if(x>0) x--;
                 if(*(data+index)==' ') backspace_hit_counter=1; else backspace_hit_counter=0; 
               }
              aux_buffer1='\0'; index_counter=0;  keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
              auto_advance=0; auto_advance_counter=0;
#endif                           
              
            } 
/*-----------------------------------------------------------------------------------------------------*/
#if ACTIVATE_ARROWS == 1
      else if(buffer==MOVE_LEFT)                         /* LEFT ARROW <--  */
            {
              if(index)  
               {
                 index--;
                 if((x==0) && (y!=0)) { x=(LCD_CHARS_PER_LINE-1) ; y--; } else if(x>0) x--;
                 aux_buffer1='\0'; index_counter=0; keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
                 auto_advance=0; auto_advance_counter=0;
#endif                 
               }  
            }  
   
/*-----------------------------------------------------------------------------------------------------*/
      else if(buffer==MOVE_RIGHT)                       /* RIGHT ARROW -->  */
            {
              if( index<max_chars ) 
               {
                 if(*(data+index)=='\0') { *(data+index)=' '; }
                 index++; 
                 if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
                 else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
                 if(*(data+index)=='\0') { *(data+index)=' '; LCD_PUTC(' '); }
                 aux_buffer1='\0'; index_counter=0; 
                 keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
                 auto_advance=0; auto_advance_counter=0;
#endif                                  
               }     
            }  

#endif              
/*------------------------------------------------------------------------------------------------------*/
      else if(index<=max_chars)  /* start of write char to array if block */  
            { /* If the same key is pressed more than once */
              if(buffer==aux_buffer1) { index_counter++; if(index_counter>=max) index_counter=0;  } 
              else index_counter=0; /* else if not the same key print the first array char */ 
              switch(buffer)   /* copy the appropriate char from the array to aux_buffer2  */
                   {
                     case(KEY1): max=sizeof(key1);   aux_buffer2=PRG_RDB(&key1[index_counter]);   break; 
                     case(KEY2): max=sizeof(key2);   aux_buffer2=PRG_RDB(&key2[index_counter]);   break; 
                     case(KEY3): max=sizeof(key3);   aux_buffer2=PRG_RDB(&key3[index_counter]);   break;
                     case(KEY4): max=sizeof(key4);   aux_buffer2=PRG_RDB(&key4[index_counter]);   break;
                     case(KEY5): max=sizeof(key5);   aux_buffer2=PRG_RDB(&key5[index_counter]);   break;
                     case(KEY6): max=sizeof(key6);   aux_buffer2=PRG_RDB(&key6[index_counter]);   break;
                     case(KEY7): max=sizeof(key7);   aux_buffer2=PRG_RDB(&key7[index_counter]);   break;
                     case(KEY8): max=sizeof(key8);   aux_buffer2=PRG_RDB(&key8[index_counter]);   break;
                     case(KEY9): max=sizeof(key9);   aux_buffer2=PRG_RDB(&key9[index_counter]);   break;
                     case(KEY10): max=sizeof(key10); aux_buffer2=PRG_RDB(&key10[index_counter]);  break;
                     default  : continue; /* if other key start the loop again */
                        
                   }   
               /* if it is not the same key or it is the first keystroke */
               if(buffer!=aux_buffer1) 
                {
                  aux_buffer1=buffer; backspace_hit_counter=0; keystroke_counter++;
                  /* If it is not the first keystroke advance the cursor */
                  if( keystroke_counter>=2 && index<max_chars )
                   {
                     if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
                     else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
                     index++;   backspace_hit_counter=0;
                   }
                }
               /* If capital letters wanted convert small to capital */
               if( capital && (aux_buffer2>='a' && aux_buffer2<='z') ) aux_buffer2-=32;
               
               /* Write char to destination array and LCD screen */
               *(data+index)=aux_buffer2;             
               LCD_PUTC(*(data+index));
#if ACTIVATE_AUTO_ADVANCE == 1                
               auto_advance=1; auto_advance_counter=0;
#endif               
            }           /*  end of write char to array if block */ 
             
    }                   /*main loop end                         */            
LCD_CURSOR_OFF();       /* Turn off cursor and exit             */     
return;
}

/*******************************************************************************************************/
unsigned char kgeti(int *data, unsigned char digits, unsigned char lcd_x, unsigned char lcd_y)
{
 /* 1 for the munus sign, 5 for the number and one for the terminating char = 7 */ 
 unsigned char number[7]={'\0','\0','\0','\0','\0','\0','\0'}; 
 unsigned char index=0, buffer=0,counter1=0, password_mode=0;
  
  if(digits==0) { password_mode=1; digits=4; }
  LCD_CURSOR_ON_BLINK(); 
  while(1)
      { /* main loop start */     
/* position cursor, wait for all keys to be released and wait 0,1 sec to prevent key double click */
        LCD_GOTOXY(lcd_x, lcd_y);
        while((buffer=scan_keyb()));
        wait(DELAY_100_MS);
        while( !(buffer=scan_keyb()) );
/*-----------------------------------------------------------------------------------------------------*/        
        if( buffer==ENTER )     { LCD_CURSOR_OFF(); break; }
/*-----------------------------------------------------------------------------------------------------*/   
        else if(buffer==ESCAPE) { LCD_CURSOR_OFF(); return(0); }
/*-----------------------------------------------------------------------------------------------------*/          
        else if( buffer==BACKSPACE )
              { 
                LCD_CURSOR_ON_BLINK();
                if(lcd_x) { lcd_x--;  } 
                if(index) index--; 
                if(counter1) { counter1--; }
                number[index]='\0'; 
                LCD_GOTOXY(lcd_x, lcd_y); 
                LCD_PUTC(' ');   
              }
/*-----------------------------------------------------------------------------------------------------*/          
        else if(buffer==MINUS && index==0) 
              {
                number[index]='-';
                index++;
                LCD_PUTC('-'); lcd_x++;   
              }
/*-----------------------------------------------------------------------------------------------------*/                   
        else if( buffer>='0' && buffer<='9' )
              { 
                if(counter1<digits)
                 {
                   number[index]=(buffer);  index++; counter1++; 
                   if(password_mode) LCD_PUTC('*'); else LCD_PUTC(buffer);  
                   lcd_x++; 
                   if(counter1==digits) LCD_CURSOR_OFF();
                 }
              }
       
      } /* main loop end */   
/*-----------------------------------------------------------------------------------------------------*/   
 *data=atoi(number);
 return(counter1);
}

/*******************************************************************************************************/
unsigned char kgetc(void)
{
register unsigned char buffer=0;
/*  wait for all keys to be released, wait for a key pressed and return the scan code */  
    while((buffer=scan_keyb()));  while( !(buffer=scan_keyb()) );
  
return(buffer);
}

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


