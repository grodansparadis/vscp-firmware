
/*********************************************************************************************************
 Title  :   Header file for the ATMega103 non i/o ports  (MEGA103.h)
 Author:    Chris efstathiou 
 E-mail:    hendrix@otenet.gr
 Homepage:  .......................
 Date:      28/Oct/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  AVR ATMEGA103
 Comments:  This software is FREE.

*********************************************************************************************************/

/*
    THE BELOW DEFINITIONS MAKE FAKE I/O REGISTERS IN ORDER TO USE THE MEGA103 MPU PORTS MORE EASILY.
    IT DOES THAT BY DEFINING THOSE PORTS AT I/O LOCATION 0 WHICH BELONG TO THE PINF REGISTER.
    PINF REGISTER IS READ ONLY SO NO HARM IS DONE. AN EXCEPTION IS IF YOU TRY TO READ PINC
    YOU WILL BE READING PINF INSTEAD.
    OF COURSE THE BEST THING IS TO AVOID USING THEM AT ALL IF POSSIBLE.
*/

#ifndef MEGA103_H
#define MEGA103_H

#if defined(__AVR_ATmega103__)

/* IF GCC 3.2 IS USED */
#if !defined(_SFR_ASM_COMPAT) && !defined(__ASSEMBLER__) && defined(_SFR_IO8)

#define DDRC   _SFR_IO8(0x00)
#define DDRF   _SFR_IO8(0x00)
#define PINC   _SFR_IO8(0x00)
#define PORTF  _SFR_IO8(0x00)

/* OR ELSE IF AN OLDER VERSION OF GCC IS USED */
#else

#if SREG > 0X3F

#define DDRC   0X20
#define DDRF   0X20
#define PINC   0X20
#define PORTF  0X20

#elif SREG <= 0X3F 

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#endif  /* #if SREG > 0X3F */ 

#endif  /* #if !defined(_SFR_ASM_COMPAT) && !defined(__ASSEMBLER__) */

#endif  /* #if defined(__AVR_ATmega103__) */

#endif /* #ifndef MEGA103_H */
