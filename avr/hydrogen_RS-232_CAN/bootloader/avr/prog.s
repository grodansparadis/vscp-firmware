; Copyright (C) 2002 Jaroslaw Karwik, 
;   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
;
; CANDIP  
; Changes by akhe@eurosoure.se http://www.eurosource.se  
;	
; This file has its orgin in following sources:
;
;  - Atmel's AVR109 application note "Self programming" ( www.atmel.com )
;  - Andy's Hutchinson modifications/optimizations 
;    of the original Atmel's code
;  - Bryce Denney's patches  ( bryce@tlw.com)
;
; This file is free software; you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the
; Free Software Foundation; either version 2, or (at your option) any
; later version.

; In addition to the permissions in the GNU General Public License, the
; author gives you unlimited permission to link the
; compiled version of this file into combinations with other programs,
; and to distribute those combinations without any restriction coming
; from the use of this file.  (The General Public License restrictions
; do apply in other respects; for example, they cover modification of
; the file, and distribution when not linked into a combine
; executable.)
;
; This file is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; General Public License for more details.  
;	
	.global writePage
        .global	fillTempBuffer
        .global readProgramMemory
        .global runApplication
        .global lowFlashBank
        .global highFlashBank

;
; Note: GNU C/ASM calling convention (see e.g. "ctoasm.inc"
;	in library sources )	
;				

__stack = RAMEND	
	
writePage:
	
        RCALL   __WAIT_SPMEN__  ; Wait for SPMEN flag cleared
        MOVW    R30,R24         ; move adress to z pointer (R31=ZH R30=ZL) 
				;( R30,R31<-R24,R25 )
        MOV     R18,R22
        RJMP	__SPMSTUFF__

fillTempBuffer:

        RCALL   __WAIT_SPMEN__	; wait for SPMEN flag cleared
        MOVW    R30,R24         ; move adress to z pointer
				;  (R31=ZH R30=ZL) ( R30,R31<-R24,R25 )
        MOVW    R0,R22		; move data to reg 0 and 1 ( R0,R1<-R23,R22 ) 
        LDI     R18,(1<<0)	; load (1<<SPMEN) to R17

__SPMSTUFF__:
	
        STS     0x68,R18	; argument 2 decides function ( R18 to SPMCR )
        SPM			; store program memory
        .word   0xFFFF		; for future part compatibility,
				;   instruction alignment
        NOP
	CLR     R1		; has to be cleared as 'zero' register

__WAIT_SPMEN__:
	
        LDS     R18,0x68	; load SPMCR to R19
        SBRC    R18,0		; check SPMEN flag
        RJMP    __WAIT_SPMEN__	; wait for SPMEN flag cleared        
        RET

readProgramMemory:
	
        RCALL    __WAIT_SPMEN__

        MOVW     R30,R24	; move adress to z pointer ( R30,R31<-R24,R25 )
;        ELPM     R24,Z		; read one byte

        RET

lowFlashBank:
	
        CLR  R18		; 
        OUT  0x3B,R18		; Write 0 to RAMPZ - choose lower 64K area
        RET

highFlashBank:
	
        LDI     R18,(1<<0)	; load (1<<RAMPZ0) to R16
        OUT     0x3B,R18	; Write 1 to  RAMPZ - choose higher 64K area 
        RET

runApplication:

	WDR			; kick the watchdog last time
        JMP 0x0


