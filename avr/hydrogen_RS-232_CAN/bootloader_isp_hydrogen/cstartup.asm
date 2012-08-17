;	.DEVICE ATMEGA161
	.DSEG
	.BYTE 100
STACKTOP:
	.Cseg
	.ORG 0x1f00
RESTART:
	;; Return address stack (RSTACK)
	LDI		R16,LOW( STACKTOP )
	OUT		0x3D,R16
	LDI		R16,HIGH( STACKTOP )
	OUT		0x3E,R16
	RJMP	main

	
	.include "main.asm"
	.include "serial.asm"
	.include "assembly.asm"
