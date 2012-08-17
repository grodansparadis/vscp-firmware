
/****************************************************************************
 Title	:   C  file for the I2C FUNCTIONS library (i2c.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:	    22/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1	   /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1	   /* This is for GCC 3.2 */
#endif


#include <io.h>
#include "i2c.h"
 



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

/* 
   Conversion of microseconds and HERTZ to the right value for the delay function 
   Some tricks are performed in order to increase accuracy without reverting to floating numbers
*/

#define I2C_CLOCK          CONCAT1(I2C_CLOCK_IN_HERTZ, L)
#define I2C_TIMEOUT        CONCAT1(I2C_TIMEOUT_IN_MILLISECONDS, L)

#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                      

#define TIME_L1_MS    ( 1*(F_CPU/6000) )                 /* MUST USE A LONG FOR COUNTING TO BE ACCURATE  */
#define DELAY_MS(ms)  DELAY_L_ACCURATE((TIME_L1_MS*ms))

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   I2C_WILL_USE_A_TIMER == 1
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if defined(__AVR_ATmega103__) || defined(__AVR_ATmega128__) || \
    defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)

#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
       defined(__AVR_ATmega64__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     0
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) || defined(__AVR_AT90S8515__) || \
       defined(__AVR_AT90S8535__) || defined(__AVR_AT90S4434__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#if defined(__AVR_ATmega161__)
#define I2C_OCIE            OCIE0
#else
#define I2C_OCIE            0
#endif
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_AT90S4433__)  || defined(__AVR_AT90S2343__) || defined(__AVR_AT90S2323__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 1

#if I2C_USE_TIMER > 0
#undef I2C_USE_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define I2C_USE_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/* TIMER SELECTION RELATED ERROR MESSAGES */
#if    I2C_WILL_USE_A_TIMER == 1 &&  NUMBER_OF_8_BIT_TIMERS == 2 

#if    I2C_USE_TIMER == 1 || I2C_USE_TIMER > 2 
#error "SELECT TIMER 0 OR 2 PLEASE! " 
#endif

#endif /* I2C_WILL_USE_A_TIMER == 1 &&  NUMBER_OF_8_BIT_TIMERS == 2  */



#define I2C_DELAY_ACCURATE          (((100000000/I2C_CLOCK)/(100000000/F_CPU))/2)

#if I2C_DELAY_ACCURATE <= 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_1
#define TIMER_DIVISION_FACTOR     1

#elif I2C_DELAY_ACCURATE > 255

#if   TIMER_DIV_BY_8 > 0 && (I2C_DELAY_ACCURATE/8) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_8
#define TIMER_DIVISION_FACTOR     8

#elif TIMER_DIV_BY_32 > 0 && (I2C_DELAY_ACCURATE/32) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_32
#define TIMER_DIVISION_FACTOR     32

#elif TIMER_DIV_BY_64 > 0 && (I2C_DELAY_ACCURATE/64) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_64
#define TIMER_DIVISION_FACTOR     64

#elif TIMER_DIV_BY_128 > 0 && (I2C_DELAY_ACCURATE/128) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_128
#define TIMER_DIVISION_FACTOR     128

#elif TIMER_DIV_BY_256 > 0 && (I2C_DELAY_ACCURATE/256) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_256
#define TIMER_DIVISION_FACTOR     256

#elif TIMER_DIV_BY_1024 > 0 && (I2C_DELAY_ACCURATE/1024) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_1024
#define TIMER_DIVISION_FACTOR     1024

#endif  /* #if   TIMER_DIV_BY_8 > 0 && (I2C_DELAY_ACCURATE/8) < 255 */
#endif  /* #elif I2C_DELAY_ACCURATE > 255 */

#define I2C_DELAY  ( I2C_DELAY_ACCURATE/TIMER_DIVISION_FACTOR )
#define DELAY()    { outp(0,I2C_TCNT); while(inp(I2C_TCNT) < I2C_DELAY); }  

 

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif I2C_WILL_USE_A_TIMER == 0 /* #if   I2C_WILL_USE_A_TIMER == 1 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#define I2C_CLOCK_CYCLES       (((100000000/I2C_CLOCK)/(100000000/F_CPU))/2)

#define I2C_DELAY              ( I2C_CLOCK_CYCLES / 3 )

#if  I2C_CLOCK_CYCLES <= 0
#error " I2C CLOCK TOO HIGH "
#elif  I2C_CLOCK_CYCLES == 1
#define DELAY()       __asm__ volatile("nop")
#warning "I2C DELAY IS 1 CPU CYCLE"
#elif I2C_CLOCK_CYCLES == 2
#define DELAY()       __asm__ volatile("rjmp _PC_+0")
#warning "I2C DELAY IS 2 CPU CYCLES"
#elif I2C_CLOCK_CYCLES == 3
#define DELAY()       { __asm__ volatile("nop"); __asm__ volatile("rjmp _PC_+0"); }
#warning "I2C DELAY IS 3 CPU CYCLES"
#elif I2C_CLOCK_CYCLES == 4
#define DELAY()       {  __asm__ volatile("rjmp _PC_+0"); __asm__ volatile("rjmp _PC_+0"); }
#warning "I2C DELAY IS 4 CPU CYCLES"
#elif I2C_CLOCK_CYCLES == 5
#define DELAY()       {  __asm__ volatile("nop"); __asm__ volatile("rjmp _PC_+0"); \
                         __asm__ volatile("rjmp _PC_+0"); }
#warning "I2C DELAY IS 5 CPU CYCLES"

#elif  I2C_CLOCK_CYCLES > 5

#define DELAY_CHAR(x)      ({  __asm__ volatile ( "ldi r25,%0 \n\t"           \
                                                  "L_%=: \n\t"                \
                                                  "dec r25 \n\t"              \
                                                  "brne L_%= \n\t"            \
                                                  : /* NO OUTPUT */           \
                                                  : "g" ((unsigned char)(x))  \
		                                  : "r25"	       	      \
           	                                );                            \
                           })                                        

#define DELAY_DIV_REMAINDER     ( I2C_CLOCK_CYCLES - (I2C_DELAY*3) )

#if   DELAY_DIV_REMAINDER <= 0
#define DELAY()                  DELAY_CHAR(I2C_DELAY)   
#elif DELAY_DIV_REMAINDER == 1
#define DELAY()                 { DELAY_CHAR(I2C_DELAY); __asm__ volatile("nop"); }  
#elif DELAY_DIV_REMAINDER == 2
#define DELAY()                 { DELAY_CHAR(I2C_DELAY); __asm__ volatile("rjmp _PC_+0"); }    
#endif



#endif  /* I2C_CLOCK_CYCLES <= 0 */

/*---------------------------------------------------------------------------------------------------*/

#if     I2C_DELAY > 255  


#undef  DELAY 
#undef  DELAY_CHAR
#undef	I2C_DELAY
#define I2C_DELAY	   ((I2C_CLOCK_CYCLES-1)/4)

#define DELAY_SHORT(x)    ({ __asm__ volatile ( "L_%=:           \n\t"      \
                                                "sbiw %A0,1      \n\t"      \
                                                "brne L_%=       \n\t"      \
                                                : /* NO OUTPUT */           \
                                                : "w" ((unsigned short)(x)) \
                                              );                            \
                          })                                     

#define DELAY()            DELAY_SHORT(I2C_DELAY) 

#endif
/*---------------------------------------------------------------------------------------------------*/
#if	I2C_DELAY > 65535 

#undef  DELAY
#undef  DELAY_LONG
#undef	I2C_DELAY
#define I2C_DELAY	   ( (I2C_CLOCK_CYCLES/6)-1 )
#define DELAY()            DELAY_L_ACCURATE(I2C_DELAY)

#endif

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif I2C_WILL_USE_A_TIMER == 0 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if    I2C_TIMEOUT <= 0

#undef I2C_TIMEOUT 
#define I2C_TIMEOUT   1
#warning " I2C_TIMEOUT TIME IS SET TO 1 MILLISECOND "

#endif    

/* Register name forming */
#define I2C_SDA_OUT_REG   CONCAT1(PORT, I2C_SDA_PORT)
#define I2C_SCL_OUT_REG   CONCAT1(PORT, I2C_SCL_PORT)
#define I2C_SDA_DDR_REG   CONCAT1(DDR, I2C_SDA_PORT)
#define I2C_SCL_DDR_REG   CONCAT1(DDR, I2C_SCL_PORT)
#define I2C_SDA_PIN_REG   CONCAT1(PIN, I2C_SDA_PORT)
#define I2C_SCL_PIN_REG   CONCAT1(PIN, I2C_SCL_PORT)


/*******************************************************************************************************/
/* SOME ERROR CHECKING WHEN USING MEGA 103 */

#if defined(__AVR_ATmega103__)

#if I2C_SDA_DDR_REG == 0 
#error "SORRY THE I2C SDA PIN PORT MUST! BE BIDIRECTIONAL"
#endif

#if I2C_SCL_DDR_REG == 0 
#error "SORRY THE I2C SCL PIN PORT MUST! BE BIDIRECTIONAL"
#endif

#endif /* #if defined(__AVR_ATmega103__) */
/*******************************************************************************************************/

/* Pin states */
#define SCL_1() 	  cbi(I2C_SCL_DDR_REG, SCL_PIN)
#define SCL_0() 	  sbi(I2C_SCL_DDR_REG, SCL_PIN)
#define SDA_1() 	  cbi(I2C_SDA_DDR_REG, SDA_PIN)
#define SDA_0() 	  sbi(I2C_SDA_DDR_REG, SDA_PIN)

#define RELEASE_I2C_BUS() { SDA_1(); SCL_1(); }
#define TOGGLE_SCL()      { SCL_1(); DELAY(); SCL_0(); DELAY(); }  

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if I2C_ERROR_CHECK_ENABLE == 1
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* Global variables */
static unsigned char i2c_error=0;


/*#################################################################################################*/

void i2c_start(void)
{

#if I2C_WILL_USE_A_TIMER == 1
        outp(I2C_TIMER_PRESCALER_BITS, I2C_TCCR);
#endif	

unsigned char x=0, y=0;

        i2c_error=I2C_NO_ERROR;

        RELEASE_I2C_BUS();
	cbi(I2C_SDA_OUT_REG, SDA_PIN);
	cbi(I2C_SCL_OUT_REG, SCL_PIN);

        while(1) 
            {
               DELAY();
               if( bit_is_set(I2C_SCL_PIN_REG,SCL_PIN) && bit_is_set(I2C_SDA_PIN_REG,SDA_PIN) )
                { 
                   x++;
                }    
               else{ x=0; y++; }

               if(x >= 10) { SDA_0(); DELAY(); SCL_0(); DELAY(); break; }
               if(y >= I2C_NUMBER_OF_START_ATTEMPTS) { i2c_error=I2C_ERROR_BUS_OCCUPIED; break; }
            }


return;
}
/*#################################################################################################*/

void i2c_stop(void)
{

unsigned char x=0;

        i2c_error=I2C_NO_ERROR;

        RELEASE_I2C_BUS();
        DELAY();

        /* Make sure tha SCL is free so we can give the required pulses */
        if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
         {    
            /* Wait the timeout period and check again */
            DELAY_MS(I2C_TIMEOUT);
            if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
             {
               i2c_error=I2C_ERROR_DEVICE_BUSY;
               return;
             }
         }   

        /* If SDA is low then 9 pulses to fake a byte receive and Nack. This way the bus might set free */  
        if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
         {    
            for(x=0; x<9; x++) { TOGGLE_SCL(); }

            /* Wait the timeout period and check again */
            if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
             {
                DELAY_MS(I2C_TIMEOUT);
                if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
                 {
                    i2c_error=I2C_ERROR_DEVICE_BUSY;
                    return;
                 }
             } 
         } 

        SCL_0();
	SDA_0();
  	DELAY();
	SCL_1();
	DELAY();
        SDA_1();
        DELAY();



return;
}
/*#################################################################################################*/

void i2c_transmit(unsigned char data)
{

register unsigned char bit=0;

i2c_error=I2C_NO_ERROR;

	for(bit=0; bit<=7; bit++)
	  {
	      if( data & 0x80 ) { SDA_1(); } else { SDA_0(); }
              TOGGLE_SCL();
	      data = (data<<1);
	  }

/* Look for a proper AKNOWLEDGE sequence*/

/* Set SDA and SCL high */ 
	RELEASE_I2C_BUS();
        DELAY();

/* Check that SCL line is high, otherwise the slave is holding it low and that means wait... */ 

        if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
         {    
            /* Wait the timeout period and check again */
            DELAY_MS(I2C_TIMEOUT);
            if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
             {
               i2c_error=I2C_ERROR_DEVICE_BUSY;
               return;
             }
         }   

/* Now we must check for a SDA low state */		 

        if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) )
         {    
            /* Wait the timeout period and check again */
            DELAY_MS(I2C_TIMEOUT);
            if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) )
             {
               i2c_error=I2C_ERROR_DEVICE_NOT_RESPONDING;
               return;
             }
         }   
/* Set SCL to low level */
	SCL_0();
	DELAY();
 

/* transaction complete. There were no ERRORS */
return;
}
/*#################################################################################################*/

unsigned char i2c_receive(unsigned char ack)
{
register unsigned char bit=0, data=0;

i2c_error=I2C_NO_ERROR;

	RELEASE_I2C_BUS();
        DELAY();

        /* Make sure tha SCL is free so we can give the required pulses */
        if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
         {    
            /* Wait the timeout period and check again */
            DELAY_MS(I2C_TIMEOUT);
            if( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
             {
               i2c_error=I2C_ERROR_DEVICE_BUSY;
               return(0xFF);
             }
         }   
    
	for(bit=0; bit<=7; bit++)
	  {
	      SCL_1();
	      DELAY();
	      data = (data<<1);
	      if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) ) { data++; }
	      SCL_0();
	      DELAY();
	  }

	/* Check that the slave has released the SDA line */    
        if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
         {    
            /* Wait the timeout period and check again */
            DELAY_MS(I2C_TIMEOUT);
            if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
             {
               i2c_error=I2C_ERROR_DEVICE_BUSY;
               return(data);
             }
         }   

        /* if CONTINUE then send AKNOWLEDGE else if QUIT do not send AKNOWLEDGE (send Nack) */	 
	if(ack==I2C_CONTINUE) { SDA_0(); } else{ SDA_1(); } 
        DELAY();
        TOGGLE_SCL();    

return(data);
}
/*#################################################################################################*/

unsigned char  i2c_get_error(void)
{

return(i2c_error);
}

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif I2C_ERROR_CHECK_ENABLE == 0
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

void i2c_start(void)
{

#if I2C_WILL_USE_A_TIMER == 1
        outp(I2C_TIMER_PRESCALER_BITS, I2C_TCCR);
#endif	

        RELEASE_I2C_BUS();
	cbi(I2C_SDA_OUT_REG, SDA_PIN);
	cbi(I2C_SCL_OUT_REG, SCL_PIN);   

        while( bit_is_set(I2C_SCL_PIN_REG,SCL_PIN) && bit_is_set(I2C_SDA_PIN_REG,SDA_PIN) );

        DELAY();  
	SDA_0();
	DELAY();
	SCL_0();
        DELAY();

return;
}
/*#################################################################################################*/

void i2c_stop(void)
{
        SCL_0();
	SDA_0();
  	DELAY();
	SCL_1();
	DELAY();
        SDA_1();
        DELAY();
  

return;
}
/*#################################################################################################*/

void i2c_transmit(unsigned char data)
{

register unsigned char bit=0;

	for(bit=0; bit<=7; bit++)
	  {
	      if( data & 0x80 ) { SDA_1(); } else { SDA_0(); }
              TOGGLE_SCL();
	      data = (data<<1);
	  }

/* Check that SCL line is high, otherwise the slave is holding it low and that means wait... */  
        RELEASE_I2C_BUS();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) );
       
        /* GIVE A PULSE TO STATE THAT AKNOWLEDGE HAS BEEN RECEIVED */
        DELAY();
        SCL_0();

return;
}
/*#################################################################################################*/

unsigned char i2c_receive(unsigned char ack)
{

register unsigned char bit=0, data=0;

	RELEASE_I2C_BUS();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) );

	for(bit=0; bit<=7; bit++)
	  {
	      SCL_1();
	      DELAY();
	      data = (data<<1);
	      if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) ) { data++; }
	      SCL_0();
	      DELAY();
	  }

        /* if CONTINUE then send AKNOWLEDGE else if QUIT do not send AKNOWLEDGE (send Nack) */	 
	if(ack==I2C_CONTINUE) { SDA_0(); } else{ SDA_1(); } 
        DELAY();
        TOGGLE_SCL();
        

return data;
}
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/



