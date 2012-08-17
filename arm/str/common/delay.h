
#ifndef _delay_h_
#define _delay_h_

#define TUNE_FAC 0.96154

//#include <types.h>

//#define delay_us(delayval) delay_us_( (uint32_t)( (FOSC*PLL_MUL) / (1000000/delayval) /2 ) )
#define delay_us(delayval) delay_us_( (uint32_t)( (FOSC*PLL_MUL) / (1000000/delayval) /4 * TUNE_FAC) )
void delay_us_( unsigned long delayval );
void delay_ms( unsigned long delayval );

#endif
