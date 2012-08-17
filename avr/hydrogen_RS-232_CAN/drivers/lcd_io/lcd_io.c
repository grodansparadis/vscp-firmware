
/****************************************************************************
 Title  :   C  file for the HD44780U LCD library (lcd_io.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      25/Oct/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.
 
*****************************************************************************/
#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif

#include <io.h>
#include <progmem.h>
#include <eeprom.h>
#include "lcd_io.h"

/* constants/macros */
#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif


#define LCD_E_PULSE_CYCLES       ( (LCD_E_PULSE_WIDTH_US*100)/(100000000/F_CPU) )

#if   LCD_E_PULSE_CYCLES <= 1

#define LCD_CLOCK_DELAY() { __asm__ volatile("nop"); })

#elif LCD_E_PULSE_CYCLES == 2 

#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); )}

#elif LCD_E_PULSE_CYCLES == 3 

#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("nop"); }

#elif LCD_E_PULSE_CYCLES == 4 

#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0"); }

#elif LCD_E_PULSE_CYCLES == 5 

#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0");  \
                            __asm__ volatile("nop");                                           \
                          }
#elif LCD_E_PULSE_CYCLES == 6 

#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0");  \
                            __asm__ volatile("rjmp _PC_+0");                                   \
                          }


#elif LCD_E_PULSE_CYCLES == 7 
#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0");  \
                            __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("nop");          \
                          }

#elif LCD_E_PULSE_CYCLES == 8 
#define LCD_CLOCK_DELAY() { __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0");  \
                            __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0");  \
                          }

#elif LCD_E_PULSE_CYCLES >= 9

#define DELAY_C_ACCURATE(x)   ({ unsigned char number_of_loops=(unsigned char)x;  \
                               	 __asm__ volatile ( "L_%=:             \n\t"      \
                                                    "dec %A0           \n\t"      \
                                                    "brne L_%=         \n\t"      \
                                                    : /* NO OUTPUT */             \
                                                    : "r" (number_of_loops)       \
		                                  );                              \
                              })

#define LCD_CLOCK_DELAY1()        DELAY_C_ACCURATE(LCD_E_PULSE_CYCLES/3) 

#define LCD_DELAY_REMAINDER       ( LCD_E_PULSE_CYCLES-((LCD_E_PULSE_CYCLES/3)*3) ) 

#if   LCD_DELAY_REMAINDER == 0

#define  LCD_CLOCK_DELAY()        { LCD_CLOCK_DELAY1(); } 

#elif LCD_DELAY_REMAINDER == 1
 
#define  LCD_CLOCK_DELAY()        { LCD_CLOCK_DELAY1(); __asm__ volatile("nop"); } 

#elif LCD_DELAY_REMAINDER == 2

#define  LCD_CLOCK_DELAY()        { LCD_CLOCK_DELAY1(); __asm__ volatile("rjmp _PC_+0"); }

#endif 

#endif /* #if   LCD_E_PULSE_CYCLES == 1 */                                 

#define LCD_POWER_ON_DELAY ( (50*(F_CPU/6000))-3 )    /* 50 milliseconds */
#define LCD_INIT_DELAY     ( (10*(F_CPU/6000))-3 )    /* 10 milliseconds */

#define INIT_MODE   2
#define DATA_MODE   1
#define CMD_MODE    0


/* CONVERSION OF LCD_DELAY_TIME_US TO TRUE MICROSECONDS */
#if LCD_READ_REQUIRED == 0 || LCD_IO_MODE == 2 || LCD_IO_MODE == 7

#define LCD_SMALL_DELAY    ( ((LCD_DELAY_TIME_US*(F_CPU/60000))/100)-3 )
#define LCD_LONG_DELAY     ( LCD_SMALL_DELAY*40 )

#endif



/*######################################################################################################*/
#if LCD_IO_MODE == 2
/*######################################################################################################*/

#define toggle_clock() ({ sbi(LCD_CLOCK_PORT, LCD_CLOCK_PIN); LCD_CLOCK_DELAY(); \
                          cbi(LCD_CLOCK_PORT, LCD_CLOCK_PIN); }) 
#define data(x)        ({ if(x) sbi(LCD_DATA_PORT, LCD_DATA_PIN); else cbi(LCD_DATA_PORT, LCD_DATA_PIN); })        
#define toggle_data()  ({ data(1); LCD_CLOCK_DELAY() data(0); })

#define LCD_DATA_PORT_IS_IO   1
#define LCD_CLOCK_PORT_IS_IO  1

#define LCD_DATA_PORT         CONCAT1(PORT,LCD_DATA_SIGNAL_PORT)
#define LCD_DATA_DDR_REG      CONCAT1(DDR,LCD_DATA_SIGNAL_PORT)

#define LCD_CLOCK_PORT        CONCAT1(PORT,LCD_CLOCK_SIGNAL_PORT) 
#define LCD_CLOCK_DDR_REG     CONCAT1(DDR,LCD_CLOCK_SIGNAL_PORT) 

#define LCD_READ_REQUIRED 0



#if defined(__AVR_ATmega103__)

#if LCD_DATA_PORT == PORTC 
#undef  LCD_DATA_PORT_IS_IO
#undef  LCD_DATA_DDR_REG
#define LCD_DATA_PORT_IS_IO   0
#endif

#if LCD_CLOCK_PORT == PORTC 
#undef  LCD_CLOCK_PORT_IS_IO
#undef  LCD_CLOCK_DDR_REG
#define LCD_CLOCK_PORT_IS_IO  0
#endif
#endif /* #if defined(__AVR_ATmega103__) */

/*######################################################################################################*/
#elif LCD_IO_MODE == 6
/*######################################################################################################*/
#define lcd_toggle_e()   ({ cbi(LCD_E_PORT, LCD_E_PIN); LCD_CLOCK_DELAY(); sbi(LCD_E_PORT, LCD_E_PIN);  })

#if LCD_READ_REQUIRED == 1  
#undef  LCD_MULTIPLEX_REQUIRED
#define LCD_MULTIPLEX_REQUIRED   0
#define BUSY_FLAG   0
#define DATA        1
#endif

#if LCD_LINES == 1 || LCD_LINES == 2 || LCD_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif

#define LCD_D4_PORT_IS_IO     1   
#define LCD_D5_PORT_IS_IO     1   
#define LCD_D6_PORT_IS_IO     1 
#define LCD_D7_PORT_IS_IO     1
#define LCD_RS_PORT_IS_IO     1
#define LCD_RW_PORT_IS_IO     1
#define LCD_E_PORT_IS_IO      1

#define LCD_D4_PORT           CONCAT1(PORT, LCD_DATA4_PORT)
#define LCD_D5_PORT           CONCAT1(PORT, LCD_DATA5_PORT)
#define LCD_D6_PORT           CONCAT1(PORT, LCD_DATA6_PORT)
#define LCD_D7_PORT           CONCAT1(PORT, LCD_DATA7_PORT)
#define LCD_RS_PORT           CONCAT1(PORT, LCD_RS_SIGNAL_PORT)
#define LCD_RW_PORT           CONCAT1(PORT, LCD_RW_SIGNAL_PORT)
#define LCD_E_PORT            CONCAT1(PORT, LCD_E_SIGNAL_PORT)

#define LCD_D4_DDR_REG        CONCAT1(DDR, LCD_DATA4_PORT)
#define LCD_D5_DDR_REG        CONCAT1(DDR, LCD_DATA5_PORT)
#define LCD_D6_DDR_REG        CONCAT1(DDR, LCD_DATA6_PORT)
#define LCD_D7_DDR_REG        CONCAT1(DDR, LCD_DATA7_PORT)
#define LCD_RS_DDR_REG        CONCAT1(DDR, LCD_RS_SIGNAL_PORT)
#define LCD_RW_DDR_REG        CONCAT1(DDR, LCD_RW_SIGNAL_PORT)
#define LCD_E_DDR_REG         CONCAT1(DDR, LCD_E_SIGNAL_PORT)



#if   LCD_READ_REQUIRED == 1

#define LCD_D4_PIN_REG        CONCAT1(PIN, LCD_DATA4_PORT)
#define LCD_D5_PIN_REG        CONCAT1(PIN, LCD_DATA5_PORT)
#define LCD_D6_PIN_REG        CONCAT1(PIN, LCD_DATA6_PORT)
#define LCD_D7_PIN_REG        CONCAT1(PIN, LCD_DATA7_PORT)

#elif LCD_READ_REQUIRED == 0

#undef LCD_RW_PORT
#undef LCD_RW_DDR_REG
#undef LCD_RW_PORT_IS_IO

#endif



#if defined(__AVR_ATmega103__)

#if LCD_D4_PORT == PORTC 
#if LCD_READ_REQUIRED == 1
#error THE PORT FOR LCD_D4 IS OUTPUT ONLY!
#endif
#undef  LCD_D4_PORT_IS_IO
#undef  LCD_D4_DDR_REG 
#define LCD_D4_PORT_IS_IO   0
#endif

#if LCD_D5_PORT == PORTC 
#if LCD_READ_REQUIRED == 1
#error THE PORT FOR LCD_D5 IS OUTPUT ONLY!
#endif
#undef  LCD_D5_PORT_IS_IO
#undef  LCD_D5_DDR_REG 
#define LCD_D5_PORT_IS_IO   0
#endif

#if LCD_D6_PORT == PORTC 
#if LCD_READ_REQUIRED == 1
#error THE PORT FOR LCD_D6 IS OUTPUT ONLY!
#endif
#undef  LCD_D6_PORT_IS_IO
#undef  LCD_D6_DDR_REG 
#define LCD_D6_PORT_IS_IO   0
#endif

#if LCD_D7_PORT == PORTC 
#if LCD_READ_REQUIRED == 1
#error THE PORT FOR LCD_D7 IS OUTPUT ONLY!
#endif
#undef  LCD_D7_PORT_IS_IO
#undef  LCD_D7_DDR_REG 
#define LCD_D7_PORT_IS_IO   0
#endif

#if LCD_RS_PORT == PORTC 
#undef  LCD_RS_PORT_IS_IO
#undef  LCD_RS_DDR_REG 
#define LCD_RS_PORT_IS_IO   0
#endif

#if LCD_READ_REQUIRED == 1 
#if LCD_RW_PORT == PORTC 
#undef  LCD_RW_PORT_IS_IO
#undef  LCD_RW_DDR_REG 
#define LCD_RW_PORT_IS_IO   0
#endif 
#endif

#if LCD_E_PORT == PORTC 
#undef  LCD_E_PORT_IS_IO
#undef  LCD_E_DDR_REG 
#define LCD_E_PORT_IS_IO    0
#endif

#endif /* #if defined(__AVR_ATmega103__) */

/*######################################################################################################*/
#elif LCD_IO_MODE == 7
/*######################################################################################################*/

#define lcd_toggle_e()   ({ cbivar(lcd_E_port, lcd_E_pin); LCD_CLOCK_DELAY() \
                            sbivar(lcd_E_port, lcd_E_pin);  })
#if SREG > 0X3F
#define IO_TO_MEM_OFFSET  0
#else
#define IO_TO_MEM_OFFSET  0X20
#endif

#define LCD_READ_REQUIRED 0  

#if LCD_0_LINES == 1 || LCD_0_LINES == 2 || LCD_0_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif

#if LCD_1_LINES == 1 || LCD_1_LINES == 2 || LCD_1_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif

#if NUMBER_OF_LCD_UNITS >= 3
#if LCD_2_LINES == 1 || LCD_2_LINES == 2 || LCD_2_LINES == 4
#else
#error THE LCD LINES MUST BE 1 OR 2 OR 4 !
#endif
#endif

#define LCD_D4_PORT_IS_IO     1   
#define LCD_D5_PORT_IS_IO     1   
#define LCD_D6_PORT_IS_IO     1 
#define LCD_D7_PORT_IS_IO     1
#define LCD_RS_PORT_IS_IO     1
#define LCD_0_E_PORT_IS_IO    1
#define LCD_1_E_PORT_IS_IO    1

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_PORT_IS_IO    1
#endif

#define LCD_D4_PORT           CONCAT1(PORT, LCD_DATA4_PORT)
#define LCD_D5_PORT           CONCAT1(PORT, LCD_DATA5_PORT)
#define LCD_D6_PORT           CONCAT1(PORT, LCD_DATA6_PORT)
#define LCD_D7_PORT           CONCAT1(PORT, LCD_DATA7_PORT)
#define LCD_RS_PORT           CONCAT1(PORT, LCD_RS_SIGNAL_PORT)
#define LCD_0_E_PORT          CONCAT1(PORT, LCD_0_E_SIGNAL_PORT)
#define LCD_1_E_PORT          CONCAT1(PORT, LCD_1_E_SIGNAL_PORT)

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_PORT          CONCAT1(PORT, LCD_2_E_SIGNAL_PORT)
#endif

#define LCD_D4_DDR_REG        CONCAT1(DDR, LCD_DATA4_PORT)
#define LCD_D5_DDR_REG        CONCAT1(DDR, LCD_DATA5_PORT)
#define LCD_D6_DDR_REG        CONCAT1(DDR, LCD_DATA6_PORT)
#define LCD_D7_DDR_REG        CONCAT1(DDR, LCD_DATA7_PORT)
#define LCD_RS_DDR_REG        CONCAT1(DDR, LCD_RS_SIGNAL_PORT)
#define LCD_0_E_DDR_REG       CONCAT1(DDR, LCD_0_E_SIGNAL_PORT)
#define LCD_1_E_DDR_REG       CONCAT1(DDR, LCD_1_E_SIGNAL_PORT)

#if NUMBER_OF_LCD_UNITS >=3
#define LCD_2_E_DDR_REG       CONCAT1(DDR, LCD_2_E_SIGNAL_PORT)
#endif


#if defined(__AVR_ATmega103__)

#if LCD_D4_PORT == PORTC 
#undef  LCD_D4_PORT_IS_IO
#undef  LCD_D4_DDR_REG 
#define LCD_D4_PORT_IS_IO   0
#endif

#if LCD_D5_PORT == PORTC 
#undef  LCD_D5_PORT_IS_IO
#undef  LCD_D5_DDR_REG 
#define LCD_D5_PORT_IS_IO   0
#endif

#if LCD_D6_PORT == PORTC 
#undef  LCD_D6_PORT_IS_IO
#undef  LCD_D6_DDR_REG 
#define LCD_D6_PORT_IS_IO   0
#endif

#if LCD_D7_PORT == PORTC 
#undef  LCD_D7_PORT_IS_IO
#undef  LCD_D7_DDR_REG 
#define LCD_D7_PORT_IS_IO   0
#endif

#if LCD_RS_PORT == PORTC 
#undef  LCD_RS_PORT_IS_IO
#undef  LCD_RS_DDR_REG 
#define LCD_RS_PORT_IS_IO   0
#endif

#if LCD_0_E_PORT == PORTC 
#undef  LCD_0_E_PORT_IS_IO
#undef  LCD_0_E_DDR_REG 
#define LCD_0_E_PORT_IS_IO  0
#endif

#if LCD_1_E_PORT == PORTC 
#undef  LCD_1_E_PORT_IS_IO
#undef  LCD_1_E_DDR_REG 
#define LCD_1_E_PORT_IS_IO  0
#endif

#if NUMBER_OF_LCD_UNITS >=3
#if LCD_2_E_PORT == PORTC 
#undef  LCD_2_E_PORT_IS_IO
#undef  LCD_2_E_DDR_REG 
#define LCD_2_E_PORT_IS_IO  0
#endif
#endif  /*  #if NUMBER_OF_LCD_UNITS >=3  */

#endif  /*  #if defined(__AVR_ATmega103__)  */

#endif  /*  #elif LCD_IO_MODE == 7 */   
/*######################################################################################################*/

/* type definitions */

typedef unsigned char  u08;
typedef unsigned short u16;

/*######################################################################################################*/
/* FUNCTION PROTOTYPES                                                                                  */
/*######################################################################################################*/

#if LCD_IO_MODE == 7
static void sbivar(unsigned char port, unsigned char bit);
static void cbivar(unsigned char port, unsigned char bit);
#endif

#if  LCD_READ_REQUIRED == 1
static unsigned char lcd_read(unsigned char rs);
static inline unsigned char lcd_waitbusy(void);
#endif

static void delay(unsigned long int number_of_loops);

/*######################################################################################################*/
/* GLOBAL variables                                                                                     */
/*######################################################################################################*/

static unsigned char x,y, putc_lock=0;
static unsigned char lcd_init_lock=0;

#if LCD_IO_MODE == 7
static unsigned char lcd_E_port=0, lcd_E_pin=0, lcd_chars_per_line=0, lcd_lines=0, current_lcd_unit=0;
static struct xy {
                   unsigned char x;
                   unsigned char y;
                 } xy_coordinates[NUMBER_OF_LCD_UNITS];

#endif

/*######################################################################################################*/
/* local functions */
/*######################################################################################################*/
/* USE THE ASM DELAY FUNCTION IF YOU EXPERIENCE ANY PROBLEMS. I HAVEN'T FOUND ONE YET. */
/*
void delay(unsigned long number_of_loops) 
{
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "cpc %C0,__zero_reg__ \n\t"  \
                     "cpc %D0,__zero_reg__ \n\t"  \
                     "breq L_EXIT_%=       \n\t"  \
                     "L_%=:                \n\t"  \
                     "subi %A0,lo8(-(-1))  \n\t"  \
                     "sbci %B0,hi8(-(-1))  \n\t"  \
                     "sbci %C0,hlo8(-(-1)) \n\t"  \
                     "sbci %D0,hhi8(-(-1)) \n\t"  \
                     "brne L_%=            \n\t"  \
                     "L_EXIT_%=:           \n\t"  \
                     :                            \
                     : "w" (number_of_loops)      \
                   );                             \
                                              
return;
}
*/

static void delay(unsigned long number_of_loops)
{
   while ( number_of_loops ) { number_of_loops--; }  
} 

/*######################################################################################################*/
#if LCD_IO_MODE == 7   /* lcd_write() function for 6 bit i/o one lcd unit */
/*######################################################################################################*/


static void sbivar(unsigned char port, unsigned char bit)
{
    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) |= (1<<bit);

return;
}
/*######################################################################################################*/

static void cbivar(unsigned char port, unsigned char bit)
{
    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) &= (~(1<<bit));

return;
}
/*######################################################################################################*/

static void lcd_write(unsigned char data, unsigned char mode) 
{
register unsigned char rs=0; 

if(lcd_init_lock == 0) { lcd_init(); }

       if(mode==DATA_MODE) rs=1; else rs=0;          /* set the rs value */
       /* output high nibble first */ 
       if(data&0x10) sbi(LCD_D4_PORT, LCD_D4_PIN);   else cbi(LCD_D4_PORT, LCD_D4_PIN);
       if(data&0x20) sbi(LCD_D5_PORT, LCD_D5_PIN);   else cbi(LCD_D5_PORT, LCD_D5_PIN);
       if(data&0x40) sbi(LCD_D6_PORT, LCD_D6_PIN);   else cbi(LCD_D6_PORT, LCD_D6_PIN);
       if(data&0x80) sbi(LCD_D7_PORT, LCD_D7_PIN);   else cbi(LCD_D7_PORT, LCD_D7_PIN);
       if(rs) sbi(LCD_RS_PORT, LCD_RS_PIN);          else cbi(LCD_RS_PORT, LCD_RS_PIN);
       /* Strobe E pin (Time is defined in lcd_io.h) */
       lcd_toggle_e();
       /* output low nibble */
       /* if INIT_MODE skip this section else execute it */
       if(mode!=INIT_MODE)
        {
          if(data&0x01) sbi(LCD_D4_PORT, LCD_D4_PIN);   else cbi(LCD_D4_PORT, LCD_D4_PIN);
          if(data&0x02) sbi(LCD_D5_PORT, LCD_D5_PIN);   else cbi(LCD_D5_PORT, LCD_D5_PIN);
          if(data&0x04) sbi(LCD_D6_PORT, LCD_D6_PIN);   else cbi(LCD_D6_PORT, LCD_D6_PIN);
          if(data&0x08) sbi(LCD_D7_PORT, LCD_D7_PIN);   else cbi(LCD_D7_PORT, LCD_D7_PIN);
          if(rs) sbi(LCD_RS_PORT, LCD_RS_PIN);          else cbi(LCD_RS_PORT, LCD_RS_PIN);
          /* Strobe E pin (Time is defined in lcd_io.h) */
          lcd_toggle_e();
        }

 delay(LCD_SMALL_DELAY);

}
/*------------------------------------------------------------------------------------------------------*/
void  select_lcd(unsigned char lcd_unit)
{

if(lcd_init_lock == 0) { lcd_init(); }

/* Save the current lcd unit x,y coordinates */
   xy_coordinates[current_lcd_unit].x=x;
   xy_coordinates[current_lcd_unit].y=y;

   if(lcd_unit==0)
    {
       lcd_E_port= LCD_0_E_PORT;
       lcd_E_pin = LCD_0_E_PIN;
       lcd_chars_per_line = LCD_0_CHARS_PER_LINE;
       lcd_lines = LCD_0_LINES;
       current_lcd_unit=lcd_unit;     /* Make the requested lcd unit current (active) */
       x=xy_coordinates[lcd_unit].x;  /* Load the saved x,y coordinates of the specified lcd unit */
       y=xy_coordinates[lcd_unit].y;
    }
   else if(lcd_unit==1)
         {
            lcd_E_port= LCD_1_E_PORT;
            lcd_E_pin = LCD_1_E_PIN;
            lcd_chars_per_line = LCD_1_CHARS_PER_LINE;
            lcd_lines = LCD_1_LINES;
            current_lcd_unit=lcd_unit;
            x=xy_coordinates[lcd_unit].x;
            y=xy_coordinates[lcd_unit].y;
         }
#if NUMBER_OF_LCD_UNITS >=3
   else if(lcd_unit==2)
         {
            lcd_E_port= LCD_2_E_PORT;
            lcd_E_pin = LCD_2_E_PIN;
            lcd_chars_per_line = LCD_2_CHARS_PER_LINE;
            lcd_lines = LCD_2_LINES;
            current_lcd_unit=lcd_unit;
            x=xy_coordinates[lcd_unit].x;
            y=xy_coordinates[lcd_unit].y;
         }
#endif
return;
}
/*######################################################################################################*/
#elif LCD_IO_MODE == 6   /* lcd_write() function for 6 bit i/o  with multi lcd units */
/*######################################################################################################*/

#if LCD_READ_REQUIRED == 1 

#define LCD_READ_DELAY  ( (10*(F_CPU/60000))/100 )

static unsigned char lcd_read(unsigned char rs) 
{
register unsigned char data=0;
/* RS=1: read data, RS=0: read busy flag, RW=1  read mode */  
    if (rs) sbi(LCD_RS_PORT, LCD_RS_PIN); else cbi(LCD_RS_PORT, LCD_RS_PIN);    

    /* configure data pins as input */
    cbi(LCD_D4_DDR_REG, LCD_D4_PIN );
    cbi(LCD_D5_DDR_REG, LCD_D5_PIN );
    cbi(LCD_D6_DDR_REG, LCD_D6_PIN );
    cbi(LCD_D7_DDR_REG, LCD_D7_PIN );

    /* set R/W pin for reading from LCD */
    sbi(LCD_RW_PORT, LCD_RW_PIN); 
    delay(LCD_READ_DELAY);

    if(bit_is_set(LCD_D7_PIN_REG, LCD_D7_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D6_PIN_REG, LCD_D6_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D5_PIN_REG, LCD_D5_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D4_PIN_REG, LCD_D4_PIN)) { data+=1; }
    data=(data<<1);

    lcd_toggle_e();
    delay(LCD_READ_DELAY);

    if(bit_is_set(LCD_D7_PIN_REG, LCD_D7_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D6_PIN_REG, LCD_D6_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D5_PIN_REG, LCD_D5_PIN)) { data+=1; }
    data=(data<<1);
    if(bit_is_set(LCD_D4_PIN_REG, LCD_D4_PIN)) { data+=1; }
    
    lcd_toggle_e();

    /* clear R/W pin for writting to LCD */
    cbi(LCD_RW_PORT, LCD_RW_PIN);      

    /* configure data pins as outputs */
    sbi(LCD_D4_DDR_REG, LCD_D4_PIN );
    sbi(LCD_D5_DDR_REG, LCD_D5_PIN );
    sbi(LCD_D6_DDR_REG, LCD_D6_PIN );
    sbi(LCD_D7_DDR_REG, LCD_D7_PIN );
    

return (data);
}
/*######################################################################################################*/
static inline unsigned char lcd_waitbusy(void)
/* loops while lcd is busy, reads address counter */
{
register unsigned char c;
    
    while ( (c=lcd_read(BUSY_FLAG) & (1<<LCD_BUSY)) ); 

    return (c);  // return address counter
}

#endif  /* #if LCD_READ_REQUIRED == 1  */
/*######################################################################################################*/

static void lcd_write(unsigned char data, unsigned char mode) 
{
register unsigned char rs=0; 

if(lcd_init_lock == 0) { lcd_init(); }

 /* save and set DDR(X) register(s) in case another program altered those values */
#if LCD_MULTIPLEX_ENABLE == 1

#if LCD_D4_PORT_IS_IO==1
unsigned char d4_is_output=0;
if(inp(LCD_D4_DDR_REG)&(1<<LCD_D4_PIN)) { d4_is_output=1; }
sbi(LCD_D4_DDR_REG, LCD_D4_PIN );
#endif
#if LCD_D5_PORT_IS_IO==1
unsigned char d5_is_output=0;
if(inp(LCD_D5_DDR_REG)&(1<<LCD_D5_PIN)) { d5_is_output=1; }
sbi(LCD_D5_DDR_REG, LCD_D5_PIN );
#endif
#if LCD_D6_PORT_IS_IO==1
unsigned char d6_is_output=0;
if(inp(LCD_D6_DDR_REG)&(1<<LCD_D6_PIN)) { d6_is_output=1; }
sbi(LCD_D6_DDR_REG, LCD_D6_PIN );
#endif
#if LCD_D7_PORT_IS_IO==1
unsigned char d7_is_output=0;
if(inp(LCD_D7_DDR_REG)&(1<<LCD_D7_PIN)) { d7_is_output=1; }
sbi(LCD_D7_DDR_REG, LCD_D7_PIN );
#endif
#if LCD_RS_PORT_IS_IO==1
unsigned char rs_is_output=0;
if(inp(LCD_RS_DDR_REG)&(1<<LCD_RS_PIN)) { rs_is_output=1; }
sbi(LCD_RS_DDR_REG, LCD_RS_PIN );
#endif

#endif  /* LCD_MULTIPLEX_ENABLE == 1 */

       if(mode==DATA_MODE) rs=1; else rs=0;          /* set the rs value */
       /* output high nibble first */ 
       if(data&0x10) sbi(LCD_D4_PORT, LCD_D4_PIN);   else cbi(LCD_D4_PORT, LCD_D4_PIN);
       if(data&0x20) sbi(LCD_D5_PORT, LCD_D5_PIN);   else cbi(LCD_D5_PORT, LCD_D5_PIN);
       if(data&0x40) sbi(LCD_D6_PORT, LCD_D6_PIN);   else cbi(LCD_D6_PORT, LCD_D6_PIN);
       if(data&0x80) sbi(LCD_D7_PORT, LCD_D7_PIN);   else cbi(LCD_D7_PORT, LCD_D7_PIN);
       if(rs) sbi(LCD_RS_PORT, LCD_RS_PIN);          else cbi(LCD_RS_PORT, LCD_RS_PIN);
       /* Strobe E pin (Time is defined in lcd_io.h) */
       lcd_toggle_e();
       /* output low nibble */
       /* if INIT_MODE skip this section else execute it */
       if(mode!=INIT_MODE)
        {
          if(data&0x01) sbi(LCD_D4_PORT, LCD_D4_PIN);   else cbi(LCD_D4_PORT, LCD_D4_PIN);
          if(data&0x02) sbi(LCD_D5_PORT, LCD_D5_PIN);   else cbi(LCD_D5_PORT, LCD_D5_PIN);
          if(data&0x04) sbi(LCD_D6_PORT, LCD_D6_PIN);   else cbi(LCD_D6_PORT, LCD_D6_PIN);
          if(data&0x08) sbi(LCD_D7_PORT, LCD_D7_PIN);   else cbi(LCD_D7_PORT, LCD_D7_PIN);
          if(rs) sbi(LCD_RS_PORT, LCD_RS_PIN);          else cbi(LCD_RS_PORT, LCD_RS_PIN);
          /* Strobe E pin (Time is defined in lcd_io.h) */
          lcd_toggle_e();
        }
#if   LCD_READ_REQUIRED == 0
 delay(LCD_SMALL_DELAY);
#elif LCD_READ_REQUIRED == 1
 lcd_waitbusy();
#endif

/* Restore the DDR registers, if multiplexing wanted */     
#if LCD_MULTIPLEX_ENABLE == 1 

#if LCD_D4_PORT_IS_IO==1
if(!d4_is_output) { cbi(LCD_D4_DDR_REG, LCD_D4_PIN ); } 
#endif
#if LCD_D5_PORT_IS_IO==1
if(!d5_is_output) { cbi(LCD_D5_DDR_REG, LCD_D5_PIN ); }
#endif
#if LCD_D6_PORT_IS_IO==1
if(!d6_is_output) { cbi(LCD_D6_DDR_REG, LCD_D6_PIN ); }
#endif
#if LCD_D7_PORT_IS_IO==1
if(!d7_is_output) { cbi(LCD_D7_DDR_REG, LCD_D7_PIN ); }
#endif
#if LCD_RS_PORT_IS_IO==1
if(!rs_is_output) { cbi(LCD_RS_DDR_REG, LCD_RS_PIN ); }
#endif

#endif  /* LCD_MULTIPLEX_ENABLE == 1 */
}
/*######################################################################################################*/
#elif LCD_IO_MODE == 2  /* lcd_write() function for 2 bit i/o */
/*######################################################################################################*/

static void lcd_write(unsigned char lcd_data,unsigned char mode) 
{
register unsigned char x=0, rs=0;

if(lcd_init_lock == 0) { lcd_init(); }

       /* INITIALIZATION */
       /* Set clock and data pins as outputs, at low state and set rs value */ 
#if LCD_CLOCK_PORT_IS_IO
  sbi(LCD_CLOCK_DDR_REG, LCD_CLOCK_PIN); 
#endif
#if LCD_DATA_PORT_IS_IO
  sbi(LCD_DATA_DDR_REG, LCD_DATA_PIN);
#endif
       cbi(LCD_CLOCK_PORT, LCD_CLOCK_PIN); cbi(LCD_DATA_PORT, LCD_DATA_PIN); 
       if(mode==DATA_MODE) rs=1; else rs=0;
       
       /* send high nibble first */
       x=6; while(x--)   {  toggle_clock();  }           /* clear shift register */           
       data(1);             toggle_clock();              /* send E "AND" signal */
       data(rs);            toggle_clock();              /* send RS signal */
       x=0x80; while(x>=0x10) { data(lcd_data&x); toggle_clock(); x=(x>>1); }
       /* Strobe E pin (Time is defined in lcd_io.h) */
       toggle_data();   

       /* if INIT_MODE do NOT strobe the E pin else strobe it */
       if(mode!=INIT_MODE)
        {       
           /* send low nibble  Clock and Data are already low */
           x=6; while(x--)   {  toggle_clock();  }           /* clear shift register */       
           data(1);             toggle_clock();              /* send E "AND" signal */
           data(rs);            toggle_clock();              /* send RS signal */
           x=0x08; while(x>=0x01) { data(lcd_data&x); toggle_clock(); x=(x>>1); }
           /* Strobe E pin (Time is defined in lcd_io.h) */
           toggle_data();
        }        
       
       delay(LCD_SMALL_DELAY);     /* wait for command to execute  */     
      
}
/*######################################################################################################*/
#endif /* #elif LCD_IO_MODE == 2 */
/*######################################################################################################*/

/*######################################################################################################*/
/* PUBLIC FUNCTIONS */
/*######################################################################################################*/
void lcd_command(unsigned char cmd)
/* send command <cmd> to LCD */
{
    lcd_write(cmd,CMD_MODE);
    
}
/*######################################################################################################*/

void lcd_gotoxy(unsigned char lcd_x, unsigned char lcd_y)
/* goto position (x,y) */
{
#if LCD_IO_MODE == 7 

if(lcd_x >= lcd_chars_per_line || lcd_y >= lcd_lines) { putc_lock=1; return; }
else putc_lock=0;

    if (lcd_y==0 )    { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+lcd_x); }
    else if(lcd_y==1) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+lcd_x); }
    else if(lcd_y==2) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+lcd_x); }
    else              { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+lcd_x); }
/*------------------------------------------------------------------------------------------------------*/
#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2
/*------------------------------------------------------------------------------------------------------*/

if(lcd_x >= LCD_CHARS_PER_LINE || lcd_y >= LCD_LINES) { putc_lock=1; return; }
else putc_lock=0;
    
#if LCD_LINES==1
    if (lcd_y==0 ) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+lcd_x); }
#endif
#if LCD_LINES==2
    if (lcd_y==0 ) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+lcd_x);  }
    else           { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+lcd_x);  }
#endif

#if LCD_LINES==4
    if (lcd_y==0 )    { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+lcd_x); }
    else if(lcd_y==1) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+lcd_x); }
    else if(lcd_y==2) { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+lcd_x); }
    else              { lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+lcd_x); }
#endif
#endif
    x=lcd_x;
    y=lcd_y;

}/* lcd_gotoxy */
/*######################################################################################################*/

unsigned int lcd_getxy(void)
{
  return((y*100)+x); 
}
/*######################################################################################################*/

void lcd_clrscr(void)
/* clear lcd and set cursor to home position */
{
   lcd_command((1<<LCD_CLR));
#if LCD_READ_REQUIRED == 0 || LCD_IO_MODE == 2 || LCD_IO_MODE == 7
   delay(LCD_LONG_DELAY);       /* this command needs more waiting time to execute */
#elif LCD_READ_REQUIRED == 1 && LCD_IO_MODE == 6  
   lcd_waitbusy();
#endif
               
   x=0; y=0;
}
/*######################################################################################################*/

void lcd_clrline(unsigned char line)
/* clear a specific lcd line and set cursor at the start of the line */
{
#if LCD_IO_MODE == 7
   if(lcd_lines==1)
    {
       lcd_gotoxy(0, 0);
       while(x<lcd_chars_per_line) { lcd_putc(' '); }
       lcd_gotoxy(0, 0);
    }    
   
   if(line<lcd_lines)
    {
       lcd_gotoxy(0, line);
       while(x<lcd_chars_per_line) { lcd_putc(' '); }
       lcd_gotoxy(0, line);
    }   

#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2

#if LCD_LINES == 1
  lcd_gotoxy(0, 0);
  while(x<LCD_CHARS_PER_LINE) { lcd_putc(' '); }
  lcd_gotoxy(0, 0);
#else  /* #if LCD_LINES == 1 */
  if(line<LCD_LINES)
   {
      lcd_gotoxy(0, line);
      while(x<LCD_CHARS_PER_LINE) { lcd_putc(' '); }
      lcd_gotoxy(0, line);
   }   
#endif  /*  #if LCD_LINES == 1  */
#endif  /*  #if LCD_IO_MODE == 7 */
}
/*######################################################################################################*/

void lcd_home(void)
/* set cursor to home position */
{
    lcd_command(1<<LCD_HOME);
#if LCD_READ_REQUIRED == 0 || LCD_IO_MODE == 2 || LCD_IO_MODE == 7
    delay(LCD_LONG_DELAY);       /* this command needs more waiting time to execute */
#elif LCD_READ_REQUIRED == 1 && LCD_IO_MODE == 6 
    lcd_waitbusy();
#endif    
    x=0; y=0;
}
/*######################################################################################################*/

void lcd_putc(unsigned char c)
/* print character at current cursor position */
{
/*******************************************************************************************************/
#if LCD_IO_MODE == 7

 if(!putc_lock)
  {
   if(lcd_lines !=1)
    {  if(c=='\n') { if(y<lcd_lines-1) lcd_gotoxy(0,(y+1)); }
       else if(x<lcd_chars_per_line) { lcd_write(c, DATA_MODE); x++; }
#if LCD_AUTO_LINE_FEED == 1
       else if(y<lcd_lines-1) { lcd_gotoxy(0,(y+1)); lcd_write(c, DATA_MODE); x++; }
       else { lcd_gotoxy(0,0); lcd_write(c, DATA_MODE); x++; }
#endif
    }
   else{
         if(c=='\n') { return; }
         if(x<lcd_chars_per_line) { lcd_write(c, DATA_MODE); x++; }
       }
  }
/*******************************************************************************************************/
#elif LCD_IO_MODE == 6 || LCD_IO_MODE == 2
/*******************************************************************************************************/
 if(!putc_lock)
  {
#if LCD_LINES!=1
       if(c=='\n') { if(y<LCD_LINES-1) lcd_gotoxy(0,(y+1)); }
       else if(x<LCD_CHARS_PER_LINE) { lcd_write(c, DATA_MODE); x++; }
#if LCD_AUTO_LINE_FEED == 1
       else if(y<LCD_LINES-1) { lcd_gotoxy(0,(y+1)); lcd_write(c, DATA_MODE); x++; }
       else { lcd_gotoxy(0,0); lcd_write(c, DATA_MODE); x++; }
#endif

#else
       if(c=='\n') { return; }
       if(x<LCD_CHARS_PER_LINE) { lcd_write(c, DATA_MODE); x++; }
#endif
  }  

/*******************************************************************************************************/
#endif  /*   #if LCD_IO_MODE == 7  */    
/*******************************************************************************************************/

return;
}
/*######################################################################################################*/

void lcd_puts(const unsigned char *s)
/* print string on lcd (no auto linefeed) */
{
    register unsigned char c;

    while ( (c = *s++) ) { lcd_putc(c);  }

}
/*######################################################################################################*/

void lcd_puts_p(const unsigned char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register unsigned char c;

    while ( (c = PRG_RDB(progmem_s++)) ) { lcd_putc(c);  }

}
/*######################################################################################################*/

void lcd_puts_e(unsigned char *eeprom_s)
/* print string from eeprom on lcd (no auto linefeed) */
{
 register unsigned char c;
   
    while( (c=eeprom_rb((unsigned int)eeprom_s++))&& c!=0xFF ) { lcd_putc(c);  }
    /*{ if(c==0xFF) break; lcd_putc(c);  }*/               
}
/*######################################################################################################*/

void lcd_put_i(int value, unsigned char dot_position, unsigned char number_of_chars)
/* print signed integer on lcd with or without comma (no auto linefeed) */
{
unsigned char lcd_data[6]={'0','0','0','0','0','0' }, position=sizeof(lcd_data), radix=10; 
        
             

        /* convert int to ascii  */ 
        if(value<0) { lcd_putc('-'); value=-value; number_of_chars--; }    
        do { position--; *(lcd_data+position)=(value%radix)+'0'; value/=radix;  } while(value); 
          
        /* some fractional digit corrections  */
        if( dot_position>=sizeof(lcd_data) ) dot_position=sizeof(lcd_data)-1;
        while( (sizeof(lcd_data)-dot_position) <= position )  { position--; }  

         if(number_of_chars<=sizeof(lcd_data) && number_of_chars>(sizeof(lcd_data)-position)) 
          {
             position=sizeof(lcd_data)-(number_of_chars-1);
          } 

        /* start displaying the number */
        for(radix=1; position<=(sizeof(lcd_data)-1); position++,radix++)
          {
            if(radix > number_of_chars) { break; }
            if( position==sizeof(lcd_data)-dot_position ) { lcd_putc(LCD_DECIMAL_POINT); radix++; } 
            if(radix > number_of_chars) { break; }
            lcd_putc(lcd_data[position]);
          }

return;
}
/*######################################################################################################*/

void lcd_putc_cgram(const unsigned char *user_char, unsigned char char_position)
{
register unsigned char x1=0;

if(char_position<=7)
 {    
   lcd_command((1<<LCD_CGRAM)+(char_position*8));
   for(x1=0; x1<=7; x1++)
     {
        lcd_write(PRG_RDB(user_char++), DATA_MODE); 
     }
 }
else{
       lcd_command((1<<LCD_CGRAM));
       for(x1=0; x1<char_position; x1++)
         { 
            lcd_write(PRG_RDB(user_char++), DATA_MODE); 
         }
         

    }     

lcd_gotoxy(x,y);

return;
}
/*######################################################################################################*/


/*######################################################################################################*/
#if LCD_IO_MODE == 6 || LCD_IO_MODE == 2
/*######################################################################################################*/

void lcd_init(void)
{
/* initialize display and select type of cursor */
/* dispAttr: LCD_DISP_OFF, LCD_DISP_ON, LCD_DISP_ON_CURSOR, LCD_DISP_CURSOR_BLINK */

lcd_init_lock=1;

#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif 

#if LCD_IO_MODE == 6  

#if LCD_E_PORT_IS_IO==1
sbi(LCD_E_DDR_REG, LCD_E_PIN  );
#endif        
sbi(LCD_E_PORT, LCD_E_PIN  ); 

#if LCD_MULTIPLEX_ENABLE == 0 

#if LCD_D4_PORT_IS_IO==1
sbi(LCD_D4_DDR_REG, LCD_D4_PIN );
#endif
#if LCD_D5_PORT_IS_IO==1
sbi(LCD_D5_DDR_REG, LCD_D5_PIN );
#endif
#if LCD_D6_PORT_IS_IO==1
sbi(LCD_D6_DDR_REG, LCD_D6_PIN );
#endif
#if LCD_D7_PORT_IS_IO==1
sbi(LCD_D7_DDR_REG, LCD_D7_PIN );
#endif
#if LCD_RS_PORT_IS_IO==1
sbi(LCD_RS_DDR_REG, LCD_RS_PIN );
#endif

#if LCD_READ_REQUIRED == 1 
#if  LCD_RW_PORT_IS_IO == 1
sbi(LCD_RW_DDR_REG, LCD_RW_PIN  );
#endif
cbi(LCD_RW_PORT, LCD_RW_PIN  ); 
#endif

#endif  /* #if LCD_MULTIPLEX_ENABLE == 0 */

#endif  /* #if LCD_IO_MODE == 6 */
    
/*------ Initialize lcd to 4 bit i/o mode -------*/
/* initial write to lcd is 8bit using delay since busy flag can't be checked here anyway */

    delay(LCD_POWER_ON_DELAY);                          /* Wait 20 milliseconds  */
     
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    delay(LCD_INIT_DELAY);                              /* Wait 5 milliseconds */
    
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    
    lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
    
    lcd_write(LCD_FUNCTION_4BIT_1LINE, INIT_MODE);      /* set IO mode to 4bit */
             
    
    /* from now on the lcd accepts only 4 bit I/O, so we can use lcd_command() */    

    lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */
    lcd_command(LCD_DISP_OFF);              /* display off                  */
    lcd_clrscr();                           /* display clear                */ 
    lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
    lcd_command(LCD_DISP_ON);               /* LCD DISPLAY ON (DEFAULT)     */

}/* lcd_init */
/*######################################################################################################*/
#elif LCD_IO_MODE == 7 
/*######################################################################################################*/

void lcd_init(void)
{
unsigned char lcd_unit=0;

lcd_init_lock=1;

/* initialize display and select type of cursor */
/* dispAttr: LCD_DISP_OFF, LCD_DISP_ON, LCD_DISP_ON_CURSOR, LCD_DISP_CURSOR_BLINK */

#if LCD_D4_PORT_IS_IO == 1
sbi(LCD_D4_DDR_REG, LCD_D4_PIN );
#endif
#if LCD_D5_PORT_IS_IO == 1
sbi(LCD_D5_DDR_REG, LCD_D5_PIN );
#endif
#if LCD_D6_PORT_IS_IO == 1
sbi(LCD_D6_DDR_REG, LCD_D6_PIN );
#endif
#if LCD_D7_PORT_IS_IO == 1
sbi(LCD_D7_DDR_REG, LCD_D7_PIN );
#endif
#if LCD_RS_PORT_IS_IO == 1
sbi(LCD_RS_DDR_REG, LCD_RS_PIN );
#endif

#if LCD_0_E_PORT_IS_IO == 1
sbi(LCD_0_E_DDR_REG, LCD_0_E_PIN  );                 
#endif        
sbi(LCD_0_E_PORT, LCD_0_E_PIN  ); 

#if LCD_1_E_PORT_IS_IO == 1
sbi(LCD_1_E_DDR_REG, LCD_1_E_PIN  );                 
#endif        
sbi(LCD_1_E_PORT, LCD_1_E_PIN  ); 

#if NUMBER_OF_LCD_UNITS >= 3
#if LCD_2_E_PORT_IS_IO == 1
sbi(LCD_2_E_DDR_REG, LCD_2_E_PIN  );                 
#endif        
sbi(LCD_2_E_PORT, LCD_2_E_PIN  );
#endif 

/*------ Initialize lcd to 4 bit i/o mode -------*/
/* initial write to lcd is 8bit using delay since busy flag can't be checked here anyway */
    xy_coordinates[LCD_0].x=0;
    xy_coordinates[LCD_0].y=0;
    xy_coordinates[LCD_1].x=0;
    xy_coordinates[LCD_1].y=0;
#if NUMBER_OF_LCD_UNITS >=3
    xy_coordinates[LCD_2].x=0;
    xy_coordinates[LCD_2].y=0;
#endif


for(lcd_unit=0; lcd_unit<NUMBER_OF_LCD_UNITS; lcd_unit++)
  {
      select_lcd(lcd_unit);

      delay(LCD_POWER_ON_DELAY);                          /* Wait 20 milliseconds  */
      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      delay(LCD_INIT_DELAY);                              /* Wait 5 milliseconds */
      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      lcd_write(LCD_FUNCTION_8BIT_1LINE, INIT_MODE);
      lcd_write(LCD_FUNCTION_4BIT_1LINE, INIT_MODE);      /* set IO mode to 4bit */
             
      /* from now on the lcd accepts only 4 bit I/O, so we can use lcd_command() */    
      /* function set: set how many lines the lcd unit has.  */
      if(lcd_lines==1) { lcd_command(LCD_FUNCTION_4BIT_1LINE);  }  
      else { lcd_command(LCD_FUNCTION_4BIT_2LINES); }
    
      lcd_command(LCD_DISP_OFF);                               /* display off                  */
      lcd_clrscr();                                            /* display clear                */ 
      lcd_command(LCD_MODE_DEFAULT);                           /* set entry mode               */
      lcd_command(LCD_DISP_ON);                                /* Display on                   */

  } 
select_lcd(0);

return;  
}/* lcd_init */

#endif
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

