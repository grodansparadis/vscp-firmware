
/****************************************************************************
 Title  :   C  file for the var-io FUNCTIONS library (var-io.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      24/Oct/2002
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

#include <io.h>
#include "var-io.h"


#ifndef IO_TO_MEM_OFFSET
#if SREG > 0X3F
#define  IO_TO_MEM_OFFSET  0x00
#else
#define  IO_TO_MEM_OFFSET  0x20
#endif
#endif







/*####################################################################################################*/ 

void outpvar(unsigned char value, unsigned char port)
{
 
    *((unsigned char*)(port+IO_TO_MEM_OFFSET))=value;

return;
}
/*####################################################################################################*/ 

void sbivar(unsigned char port, unsigned char bit)
{

    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) |= (1<<bit);

return;
}
/*####################################################################################################*/

void cbivar(unsigned char port, unsigned char bit)
{

    *((unsigned char*)(port+IO_TO_MEM_OFFSET)) &= (~(1<<bit));

return;
}
/*####################################################################################################*/

void loop_until_bit_is_set_var(unsigned char port, unsigned char bit)
{
register unsigned char temp=0;

   do
    {
       temp=*((unsigned char*)(port+IO_TO_MEM_OFFSET));
    }
    while(!(temp & (1<<bit)) );
return;
}
/*####################################################################################################*/

void loop_until_bit_is_clear_var(unsigned char port, unsigned char bit)
{
register unsigned char temp=0;

   do
    {
       temp=*((unsigned char*)(port+IO_TO_MEM_OFFSET));
    }
    while( (temp&(1<<bit)) );
return;
}
/*####################################################################################################*/
unsigned char bit_is_set_var(unsigned char port, unsigned char bit)
{


       if( *((unsigned char*)(port+IO_TO_MEM_OFFSET)) & (1<<bit) ) { return(1); }
   
return(0);
}
/*####################################################################################################*/

unsigned char bit_is_clear_var(unsigned char port, unsigned char bit)
{

    if( *((unsigned char*)(port+IO_TO_MEM_OFFSET)) & (1<<bit) ) { return(0); }
   
return(1);
}
/*####################################################################################################*/

unsigned char inpvar(unsigned char port)
{

return(*((unsigned char*)(port+IO_TO_MEM_OFFSET)));
}


/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

