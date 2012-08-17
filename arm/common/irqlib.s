;/*****************************************************************************
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2000.  All rights reserved.
;******************************************************************************
;
;  General IRQ change code. NewIRQ installs the given 'C' routine as the
; high level handler for interrupt as well as the actual vector. Routines
; to en/disable IRQs.
;
;	$Id: irqlib.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;******************************************************************************/

	INCLUDE	sizes.s
	INCLUDE except_h.s
	INCLUDE mmumacro.s
	GET	retmacro.s

	IMPORT	uHALp_HandleIRQ
	IMPORT	uHALir_TrapIRQ		; Simple IRQ Trap Handler
	IMPORT	uHALir_CleanDCacheEntry
	IMPORT	uHALir_EnterSvcMode	; Enter Supervisor mode.
	IMPORT	uHALir_ExitSvcMode	; Exit Supervisor mode.

	EXPORT	uHALir_NewVector
	EXPORT	uHALir_ReadVector
	EXPORT	uHALir_DisableInt	; Disable IRQ.
	EXPORT	uHALir_EnableInt
	EXPORT  uHALir_DoSWI


; Make sure status is kept consistent in this code:
HALi_VECT_ERR	EQU	0
HALi_VECT_LDR	EQU	1
HALi_VECT_B	EQU	2


; ------------------------------------------------------------------
; Check instruction at given address & decode to read the vector to
; the routine to be executed.
;
;	$val - status:	HALi_VECT_LDR - Instruction is LDR PC,..
;			HALi_VECT_B   - Instruction is B(ranch forward)
;			HALi_VECT_ERR - Neither of the above
;	$addr - address for any new vector/branch instruction
;	$vect - pointer to exception routine

	MACRO
$label  READ_VECTOR	$addr, $vect, $val, $mask, $tmp

	LDR	$vect, [$addr]		; Read the instruction

	LDR	$mask, =0x0FFF		; Offset is bottom 12 bits
	LDR	$val,  =0xE59FF000	; Bit pattern for LDR PC, ... 
	BIC	$tmp, $vect, $mask	; Clear the offset
	CMP	$val, $tmp		; Does the instruction match?
	BNE	%F2

	; To calculate real address: clear all but the offset, add
	; address of vector & pipeline offset.
	AND	$tmp, $vect, $mask
	ADD	$tmp, $tmp, $addr
	ADD	$addr, $tmp, #8
	LDR	$vect, [$addr]		; Read the old address
	MOV	$val, #HALi_VECT_LDR	; Set return status
	B	%F4

	; The exception vector was not a LDR PC so try a Branch.
	; NOTE: According to the Architectural Reference Manual, branch back
	; past 0 is unpredictable.
2
	LDR	$mask, =0x00FFFFFF	; Offset is bottom 24 bits
	LDR	$val,  =0xEA000000	; Bit pattern for B(ranch forward)
	BIC	$tmp, $vect, $mask	; Clear the offset
	CMP	$val, $tmp		; Does the instruction match?
	MOVNE	$val, #HALi_VECT_ERR	; No, flag error -> retval = 0
	BNE	%F4

	; To calculate real address: clear all but the offset, change
	; word offset to bytes, add address of vector & pipeline offset.
	AND	$vect, $vect, $mask
	TST	$vect, #0x800000		; Backwards or forwards branch ?.
	MVNNE	$vect, $vect			; Backwards branch so invert offset.
	ADDNE	$vect, $vect, #1		; Add addtional pipeline offset.
	BIC	$vect, $vect, #0xFF000000
	MOV	$tmp, $vect, LSL #2
	SUBNE	$tmp, $addr, $tmp		; Backwards branch requires a subtraction
	ADDEQ	$tmp, $tmp, $addr		; from the vector to exception routine
	ADD	$vect, $tmp, #8			; vice versa for forwards branch.
	MOV	$val, #HALi_VECT_B		; Set return status

4
	MEND


	AREA	uHAL_IRQs, CODE, READONLY


; ------------------------------------------------------------------
; int uHALir_ReadVector(void *vector, prVoid *oldHandler)

uHALir_ReadVector
	STMFD	sp!, {r4-r8, lr}	; save working registers

	TEST_HIGHVECTORS r8
	ADDEQ	r0, r0, #HighVectorsLocation

	READ_VECTOR	r0, r4, r5, r6, r7

	; Move retval to r0 & check for unknown vector
	; Less code & less clear:	MOVS	r0, r5
	MOV	r0, r5
	CMP	r0, #HALi_VECT_ERR
	BEQ	%1

	; Only save address of old handler if pointer is non-zero
	CMP	r1, #0
	BEQ	%1

	; Vector is valid, save in given location
	STR	r4, [r1]

1
	POP_RETURN 	r4-r8


; ------------------------------------------------------------------
; int uHALir_NewVector(void *vector, pHandler newHandler)

; Vector substitution mechanism (eg NewIRQ), replaces the specified
; exception vector handler with the newHandler. Returns 0 for fail.
;
; NOTE: This routine must be called in Supervisor mode.

; On return:
;	r0 - status:	0 couldn't write new exception vector
;			1 Old exception was an LDR PC,..
;			2 Old exception was a branch

uHALir_NewVector
	STMFD	sp!, {r3-r8, lr}	; save working registers

	TEST_HIGHVECTORS r8
	ADDEQ	r0, r0, #HighVectorsLocation

	MOV	r3, r0
	READ_VECTOR	r0, r4, r5, r6, r7

	; If instruction was LDR, just write address of newHandler
	CMP	r5, #HALi_VECT_LDR
	BEQ	%F5

	; The exception vector was not a LDR PC so there is no indirect
	; vector to overwrite. Instead we will try and construct a branch
	; instruction and write this into the exception vector. This will
	; only work if handler routine we are trying to install is within
	; the first 32M of memory.
	; According to the Architectural Reference Manual, branch back
	; past 0 is unpredictable.
	CMP	r3, r1			; Branch forward to backwards ?.
	SUBCC   r7, r1, r3		; Subtract vector & pipeline offset
	SUBCC   r7, r7, #8
	SUBCS   r7, r3, r1		
	ADDCS   r7, r7, #4

        CMP     r7, #SZ_32M             ; Is the routine within first 32M?
	MOVHS	r0, #HALi_VECT_ERR	; No, can't branch to it - retval 0
	BHS	%F6
	; Build branch instruction: offset is in words. No need to mask
	; as target is in range.

	MOV   	r7, r7, LSR #2          ; Shift right and mask
	CMP	r3, r1			; Test for forwards or backwards branch.
	BCC	%55

	MVN	r7, r7			; Backward branch so invert offset.
	BIC	r7, r7, #0xFF000000
55
        ORR     r1, r7, #0xEA000000
	MOV	r5, #HALi_VECT_B	; Set return status

5
	; Now write the address for the LDR or the new B instruction
        STR     r1, [r0]
	MOV     r7, #0

	; Drain write buffer & caches to make sure everything is flushed.
	WRCACHE_CleanDCentry	r0

	WRCACHE_DrainWriteBuffer	r7

	WRCACHE_FlushIC		r0	; Flush ICache

	; Now check that instruction wrote correctly
	LDR	r6, [r0]
	CMP	r6, r1
	MOVNE	r5, #HALi_VECT_ERR	; No, must be read-only retval = 0
	MOV	r0, r5			; retval in r0

6
	POP_RETURN 	r3-r8


; ------------------------------------------------------------------
;	void uHALir_DisableInt(void)
;	void uHALir_EnableInt(void)
;
;	Disable and enable IRQ, preserving current CPU mode.
;
uHALir_DisableInt
	STMFD	sp!, {r4}
	MRS	r4, cpsr
	ORR	r4, r4, #NoIRQ
	MSR	cpsr_c, r4
	LDMFD	sp!, {r4}
	RETURN	lr


uHALir_EnableInt
	STMFD	sp!, {r4}
	MRS	r4, cpsr
	BIC	r4, r4, #NoIRQ
	MSR	cpsr_c, r4
	LDMFD	sp!, {r4}
	RETURN	lr


; ------------------------------------------------------------------
; Routine to process known SWIs from 'C'
; Expects SWI in r0 & parameter (if any) in r1
uHALir_DoSWI
	STMFD	sp!, {r4-r12, lr}	; save working registers

	CMP	r0, #angel_SWI_SYS_READC	; Is it a GetByte?
	SWIEQ	SWI_Angel

	CMP	r0, #angel_SWI_SYS_WRITEC	; Is it a PutByte?
	SWIEQ	SWI_Angel

	POP_RETURN 	r4-r12


	END				; End of file

