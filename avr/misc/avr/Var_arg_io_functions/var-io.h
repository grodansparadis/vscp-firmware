
/*********************************************************************************************************
 Title  :   C header file for the Variable argument io library (var-io.h)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      10/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/
/* 
    The purpose of this header file is to provide the usuall sbi, cbi, etc. macros
    but with variables or not in all arguments.
    EXAMPLE 1: port=PORTB; pin=7; sbivar(port, pin);                (applies to GCC ver < 3.2)
    EXAMPLE 2: port=&PORTB; pin=7; sbivar(port, pin);               (applies to GCC ver >= 3.2)
    EXAMPLE 3: port=_SFR_MEM_ADDR(PORTB); pin=7; sbivar(port, pin); (applies to GCC ver >= 3.2)    
*/



#ifndef VAR_IO_H
#define VAR_IO_H



#if  defined(_IO_REG_MACRO_MODE_) || defined(_SFR_ASM_COMPAT) || defined(__ASSEMBLER__)  \
     || !defined(_SFR_DEFS_H_) 

#ifndef IO_TO_MEM_OFFSET
#if SREG > 0X3F
#define  IO_TO_MEM_OFFSET  0x00
#else
#define  IO_TO_MEM_OFFSET  0x20
#endif
#endif

#else

#ifndef IO_TO_MEM_OFFSET
#define  IO_TO_MEM_OFFSET  0x00
#endif

#endif



#define inpvar(addr)               (*((unsigned char*)(addr+IO_TO_MEM_OFFSET)))

#define outpvar(value,addr)        { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) = value; }

#define sbivar(addr,pin)           { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) |= (1<<pin); }

#define cbivar(addr,pin)           { (*((unsigned char*)(addr+IO_TO_MEM_OFFSET))) &= (~(1<<pin)); }

#define bit_is_set_var(addr,pin)   ( *((unsigned char*)(addr+IO_TO_MEM_OFFSET)) & (1<<pin) )

#define bit_is_clear_var(addr,pin) ( !(*((unsigned char*)(addr+IO_TO_MEM_OFFSET)) & (1<<pin)) )







#endif
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

