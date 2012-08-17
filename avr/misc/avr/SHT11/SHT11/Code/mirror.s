;****************************************************************************
;* 
;* File name:	mirror.s
;* Author:		Alex Raimondi
;* Date:		7 Mai 2002
;*
;* Mirror byte. Exchanging bit 7 with bit 0, bit 6 with bit uint8 mirrorByte( uint8 value );1 and so on.
;*
;* uint8 mirrorByte( uint8 value );
;* 
;****************************************************************************

#include <ctoasm.inc>

/* uint8 value is passed in here */
#define value	r24


/* Temporary intermediate value (loop counter) */
#define loopCnt	rTI0

        .global mirrorByte
        .func	mirrorByte

mirrorByte:
        ldi     loopCnt,  0x04          ; loop counter to 4. We do 2 bit at once
        mov		__tmp_reg__, value		; move value to temp reg because rByte is also in r24
loop:
		ror		__tmp_reg__				; lsb into Carry
		rol		rByte					; Carry into lsb
		ror		__tmp_reg__				; lsb into Carry
		rol		rByte					; Carry into lsb

		dec		loopCnt					; loop
		brne	loop

        ret

        .endfunc
