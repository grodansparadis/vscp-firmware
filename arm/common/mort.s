;/**************************************************************************
;  ARM Firmware Suite: mort.s
;
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2002.  All rights reserved.
;***************************************************************************
;
;  Post Mortem - Try and report what happened after a crash.
;  Assumes memory system (uses stacks) is working and OS_COMPORT has been
;  initialized.
;
;	Revision $Revision: 1.1 $
;	Checkin $Date: 2005/12/19 11:45:57 $
;	Revising $Author: akhe $
;
;***************************************************************************/

	INCLUDE bits.s
	INCLUDE sizes.s
	INCLUDE platform.s
	INCLUDE except_h.s
	GET	target.s			; Target specific macros
	GET	retmacro.s			; Processor return definitions

	EXPORT	uHALir_PostMortem

	EXPORT	uHALir_DataAbort
	EXPORT  uHALir_PrefetchAbort


 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
   	IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
 ENDIF
 ENDIF


	AREA	HAL_SelfDebug, CODE, READONLY

 IF :DEF: DEBUG

	IMPORT	uHALir_ResetGo, WEAK

	EXPORT	spitword
	IMPORT	sputchar

;===========================================================================
; Ths routine will put a word value via sputchar

PutWord
	MOV	r4, r14
	MOV	r5, r0
	MOV	r0, #0x20		; ' '
	BL	sputchar
PutWord2
	MOV	r3, #28
01	MOV	r0, r5, LSR r3
	ANDS	r0, r0, #0xf
	BNE	%f02			; the first non-zero
;	SUBS	r3, r3, #4
;	BNE	%b01			; all but the last digit
;	MOV	r0, r5			; Get the only digit
02	CMP	r0, #10
	ADDLO	r0, r0, #0x30		; 0 - 9
	ADDHS	r0, r0, #0x61 - 10	; a - f
	BL	sputchar
	SUBS	r3, r3, #4
	MOV	r0, r5, LSR r3
	AND	r0, r0, #0xf
	BPL	%b02
	RETURN	r4


; 'C' callable version of PutWord
spitword
	STMFD	sp!, {r3-r5, r14}
	BL	PutWord
 	POP_RETURN	r3-r5


spitstr
	MOV	r4, lr
	MOV	r5, r0
3
	LDRB	r0, [r5], #1
	CMP	r0, #0
	MOVEQ	lr, r4
	RETURN_COND	EQ	; return if r0 == 0

	BL	sputchar
	B	%B03


;===========================================================================
; It's dead Jim, dead. Who, what where, when, how?
;
; 'Routine' to print out r0-r12, r14, spsr, fsr & far, ttb, tlb & contents
; Called only when debug system crashes (doesn't return). Uses stack so
; memory must be working and sputchar() - serial-only version of putchar().
; Make sure 'DEBUG' is defined for 'C' too.

uHALir_PostMortem
	STMFD	sp!, {r9-r12, r14}	; X marks the spot
	MOV	r9, r0
	MOV	r10, r3
	MOV	r11, r4
	MOV	r12, r5

	LDR	r0, =Mort0		; Tell user what kind of error
	BL	spitstr
	MRS	r0, cpsr
	BL	PutWord

	LDR	r0, =Mort1
	BL	spitstr
	MOV	r0, r9
	BL	PutWord			; r0
	MOV	r0, r1
	BL	PutWord			; r1
	MOV	r0, r2
	BL	PutWord			; r2
	MOV	r0, r10
	BL	PutWord			; r3
	MOV	r0, r11
	BL	PutWord			; r4
	MOV	r0, r12
	BL	PutWord			; r5
	MOV	r0, r6
	BL	PutWord			; r6
	LDR	r0, =Mort2
	BL	spitstr
	MOV	r0, r7
	BL	PutWord			; r7
	MOV	r0, r8
	BL	PutWord			; r8
	LDMFD	sp!, {r9-r12, r14}	; X marks the spot
	MOV	r8, r14
	MOV	r0, r9
	BL	PutWord			; r9
	MOV	r0, r10
	BL	PutWord			; r10
	MOV	r0, r11
	BL	PutWord			; r11
	MOV	r0, r12
	BL	PutWord			; r12
	MOV	r0, r13
	STMFD	r0!, {r13, r14}^	; Assembler warns about this!
	LDMFD	r0!, {r6, r7}
	MOV	r0, r6
	BL	PutWord			; r13
	LDR	r0, =Mort3
	BL	spitstr
	MOV	r0, r7
	BL	PutWord			; r14
	MOV	r0, r8
	BL	PutWord			; pc

	; Mode before exception
	LDR	r0, =Mort4
	BL	spitstr
	MRS	r0, spsr
	BL	PutWord

	; Read Fault Status register and Fault Address register
	LDR	r0, =Mort5
	BL	spitstr
	RDMMU_FaultStatus	r0
	BL	PutWord
	LDR	r0, =Mort6
	BL	spitstr
	RDMMU_FaultAddress	r0
	MOV	r8, r0, LSR #20		; Strip irrelevent bits
	BL	PutWord
	; Read Translation Table Base reg.
	LDR	r0, =Mort7
	BL	spitstr
	RDMMU_TTBase 	 r0
	MOV	r9, r0, LSR #14		; Strip irrelevent bits
	BL	PutWord

	; Now rebuild address of page table entry
	MOV	r9, r9, LSL #14
	ORR	r9, r9, r8, LSL #2
	LDR	r0, =Mort8
	BL	spitstr
	MOV	r0, r9
	BL	PutWord
	LDR	r0, =':'
	BL	sputchar
	LDR	r0, [r9]
	BL	PutWord
	LDR	r0, ='\n'
	BL	sputchar		; Newline
	LDR	r0, ='\n'
	BL	sputchar		; Newline

	; Try again?
	B	uHALir_ResetGo		; If no ResetGo, this'll go to 0

Mort0	DCB	"\ncpsr\t", 0
Mort1	DCB	"\nr0-r6\t", 0
Mort2	DCB	"\nr7-r13\t", 0
Mort3	DCB	"\nr14, pc\t", 0
Mort4	DCB	"\nspsr\t", 0
Mort5	DCB	"\nfsr", 0
Mort6	DCB	"\tfar", 0
Mort7	DCB	"\nttb\t", 0
Mort8	DCB	"\ttlb", 0

 ELSE

uHALir_PostMortem
	; If Debug isn't defined, cant do much - try lighting all LEDS
	MOV	r4, #0xFF
	DO_DEBUG	r4, r5, r6

	b	.			; Loop forever.

 ENDIF


uHALir_DataAbort
 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
	; We must handle this since nobody else cares.
	STMFD	sp!, {r0, r1, lr}
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x10]
	CMP		r0, #0
	; No chain so use uHAL's exception handler.
	BEQ		_uHAL_DataAbt
	BL		Chainir_ChainWalk
_uHAL_DataAbt
	; We must handle this since nobody else cares.
	LDMFD	sp!, {r0, r1, lr}
 ENDIF	
 ENDIF
	B	uHALir_PostMortem

uHALir_PrefetchAbort
 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
	; We must handle this since nobody else cares.
	STMFD	sp!, {r0, r1, lr}
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0xC]
	CMP		r0, #0
	; No chain so use uHAL's exception handler.
	BEQ		_uHAL_PrefetchAbt
	BL		Chainir_ChainWalk
_uHAL_PrefetchAbt
	; We must handle this since nobody else cares.
	LDMFD	sp!, {r0, r1, lr}
 ENDIF	
 ENDIF
	B	uHALir_PostMortem


	END

; End of file - mort.s

