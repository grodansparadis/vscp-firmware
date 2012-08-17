
/****************************************************************************
 Title  :   C  include file for the var-io FUNCTIONS library (var-io.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      20/Jul/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#ifndef VAR_IO_H
#define VAR_IO_H

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif


extern void              outpvar(unsigned char value, unsigned char port);
extern void              sbivar(unsigned char port, unsigned char bit);
extern void              cbivar(unsigned char port, unsigned char bit);
extern void              loop_until_bit_is_set_var(unsigned char port, unsigned char bit);
extern void              loop_until_bit_is_clear_var(unsigned char port, unsigned char bit);
extern unsigned char     bit_is_set_var(unsigned char port, unsigned char bit);
extern unsigned char     bit_is_clear_var(unsigned char port, unsigned char bit);
extern unsigned char     inpvar(unsigned char port);

#endif
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

