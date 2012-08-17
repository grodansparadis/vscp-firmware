/* $Id: c018_reloc.c,v 1.1 2006/03/22 17:05:18 akhe Exp $ */

/* Copyright (c)1999 Microchip Technology */

/* MPLAB-C18 startup code */

/* external reference to the user's main routine */
extern void main (void);
/* prototype for the startup function */
void _entry (void);
void _startup (void);
extern near char __FPFLAGS;
#define RND 6

#pragma code _entry_scn=0x000200
void
_entry (void)
{
_asm goto _startup _endasm

}

#pragma code _startup_scn
void
_startup (void)
{
  _asm
    // Initialize the stack pointer
    lfsr 1, _stack
    lfsr 2, _stack

    clrf TBLPTRU, 0 // 1st silicon doesn't do this on POR

    bcf __FPFLAGS,RND,0 // Initalize rounding flag for floating point libs
_endasm loop:

  // Call the user's main routine
  main ();

  goto loop;
}                               /* end _startup() */
