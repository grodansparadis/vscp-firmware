
/****************************************************************************
 Title  :   C  test file for the SRF04 FUNCTIONS library (test_srf04.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#include <io.h>
#include "lcd_io.h"
#include "srf04.h"


#define AVERAGING_FILTER_SAMPLES   5  /* each measurement is taking 67 ms */

void main(void)
{
unsigned char counter1=0;
unsigned int range=0;

/* LCD init */
lcd_init();
lcd_clrscr();
lcd_gotoxy(0,0); lcd_puts_P("RANGE (m) = ");

/* ALL SRF04 UNITS WILL BE INTIALIZED! */
srf04_init();


while(1)
    {
              /* AVERAGING FILTER */
       for(counter1=0,range=0; counter1<AVERAGING_FILTER_SAMPLES; counter1++)
         {
#if SRF04_UNITS_USED == 1
            range+=srf04_ping();
#elif SRF04_UNITS_USED == 1
            range+=srf04_ping(SRF04_0);
#endif
         } 
       range /= AVERAGING_FILTER_SAMPLES;
 
       /* DISPLAY TO LCD */
       lcd_gotoxy(12,0); lcd_puti(range, 2);

       


    }

return;
}
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

       

