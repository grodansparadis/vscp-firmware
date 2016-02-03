;/**************************************************************************
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2000.  All rights reserved.
;***************************************************************************
;
;  uHAL Undef Handler
;  NOT USED. Currently just tries to write some debug data to memory
;
;	$Id: undeftrap.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;***************************************************************************/

	INCLUDE except_h.s

	EXPORT	uHALir_TrapUndef		; Undef exception handler

	IMPORT	uHALir_DebugUndefinedInst	; in irq.c

	IMPORT	uHALip_FirstVector		; From boot.s
	IMPORT	uHALip_LastVector

 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
   	IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
 ENDIF
 ENDIF

	AREA	HAL_UndefTrap, CODE, READONLY

; Try and capture some sensible data after undefined instruction.
uHALir_TrapUndef
 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
	STMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x4]
	CMP		r0, #0
	BEQ		_uHAL_UNDEF			; No chain so use uHAL's exception handler.
	BL		Chainir_ChainWalk
_uHAL_UNDEF
	LDMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
 ENDIF
 ENDIF	

	STMFD	sp!, {r1-r2}

	; Should be done properly to a system defined address.
	MOV	r1, #0x00010000		; Debug - make a copy of originals
	STR	lr, [r1], #4		; Old return address (if any)
	MRS	r2, cpsr
	STR	r2, [r1], #4		; CPSR

	;
	; The is no coprocessor on a ARM7TDMI
	;
 IF :LNOT: :DEF: ARM7T

	MRC	p15, 0, r2, c5, c0, 0	; Coproc 5 - 1 byte of status
	STR	r2, [r1], #4
	MRC	p15, 0, r2, c6, c0, 0	; Coproc 6 - Fault address
	STR	r2, [r1],#4

 ENDIF

	MOV	lr, r1			; Move dump pointer to lr
	LDMFD	sp!, {r1-r2}
	STR	sp, [lr],#4		; Old Stack pointer
	STMFD	lr!, {r0-r7}		; Registers

	LDMFD	sp!, {r0-r7}		; Unload a small amount of stack
	STMFD	lr!, {r0-r7}		; onto the dump
	LDMFD	sp!, {r0-r7}
	STMFD	lr!, {r0-r7}
	LDMFD	sp!, {r0-r7}
	STMFD	lr!, {r0-r7}

	; Okay, we've done enough, add further specific initialisation here
	BL	uHALir_DebugUndefinedInst
_Undef4ever
	B	_Undef4ever		; Might try and restart, we just loop


	END

