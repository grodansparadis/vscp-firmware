;/***************************************************************************
; Copyright ARM Limited 1999 - 2000.  All rights reserved.
;****************************************************************************
;
;    fiqtrap.s
;
;	$Id: fiqtrap.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;  FIQ trap handling code. TrapFIQ filters interrupt before passing it on.
;	NOTE: Need to define READ_FIQ macro to use this (and change READ_INT
;	to READ_FIQ).
;
;****************************************************************************/


	INCLUDE platform.s
	INCLUDE except_h.s
	INCLUDE target.s
	GET	retmacro.s			; Thumb/ARM interworking

	IMPORT	uHALp_StartFIQ		; Pointers to 'C' routines.
	IMPORT	uHALp_HandleFIQ
	IMPORT	uHALp_FinishFIQ
	IMPORT	uHALir_FlagsTestFIQ

	EXPORT	uHALir_TrapFIQ		; Simple FIQ Trap Handler
	EXPORT  uHALir_FIQProcess
	EXPORT	uHALir_TestFIQ

   IF :DEF: CHAIN_VECTORS 
   IF CHAIN_VECTORS = 1
    IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
   ENDIF
   ENDIF

	AREA	uHal_Traps, CODE, READONLY


; Indirect pointers to FIQ handling routines.
_pStartFIQ	DCD uHALp_StartFIQ
_pHandleFIQ	DCD uHALp_HandleFIQ
_pFinishFIQ	DCD uHALp_FinishFIQ

;	uHALir_TrapFIQ - FIQ trap handler
;
; Wrapper to service the trap and call a high-level handler. This routine
; saves all the registers in an APCS compliant manner, calls a StartFIQ
; function (if defined), reads the interrupt mask & calls the high-level
; handler; calls a FinishFIQ routine (if defined) and if FinishFIQ returns a
; value, jump to it as an address to finish FIQ processing.
;
; This mechanism provides an interface to 'C' for quite complex processing
; based on FIQs, such as simple context-switching using the timer FIQ ticks.
; By not defining StartFIQ/FinishFIQ, simple FIQ processing is automatic.
;
uHALir_TrapFIQ
	; We take the cycle hit of modifying the return address here,
	; to simplify the code needed to return to the interrupted
	; thread, possibly performing a context switch on the way.
	SUB	lr, lr, #4

	; At this points FIQs are disabled, so we know that the SPSR
	; register will not be trashed by another interrupt.
        STMFD   sp!, {r0-r12, lr}       ; save registers
        MRS     v1, SPSR		; push SPSR to allow nested interrupts
        STMFD   sp!, {v1}

	LDR	a3, _pStartFIQ		; Indirect pointer to StartFIQ
	LDR	a2, [a3]		; Check for a StartFIQ routine
	CMP	a2, #0
	MOVNE	lr, pc			; arrange for a safe return here
	                     ; Ensure that this and the next instruction
	                     ; are not separated (either of the next options)
	; Some generic task which is required at the start of an FIQ
	RETURN_COND_EXTEND	NE, a2

 	; Read which interrupt(s) is active (returns result in a1)
	READ_FIQ	a1, a2, a3

	LDR	a3, _pHandleFIQ		; Indirect pointer to FIQ Handler
	LDR	a3, [a3]		; load pointer to a register
	MOV     lr, pc			; arrange for a safe return here
	
	; The high-level handler dispatches the FIQ to the proper
	; user defined ISR.
	RETURN	a3			; call high-level handler
			    ; The above two instructions must not be split

	LDR	a3, _pFinishFIQ		; Indirect pointer to FinishFIQ
	LDR	a1, [a3]		; Check for a FinishFIQ routine
	CMP	a1, #0
	MOVNE	lr, pc			; arrange for a safe return here
	; Some generic task which is required at the end of an FIQ
	RETURN_COND_EXTEND	NE, a1

;; !!!! NOTE: If FinishFIQ does not return 0, jump to the returned address.
;; !!!!	      So make sure your target routine matches this stack format &
;; !!!!       recovers from fiq-mode properly.

	CMP	a1, #0			; a1 is non-zero for FIQs which need
	RETURN_COND_EXTEND	NE, a1	; to do further processing

	LDMFD	sp!, {v1}		; recover SPSR value from stack
	MSREQ	SPSR_c, v1		; restore the SPSR
	LDMFD	sp!, {r0-r12, pc}^	; Restore saved registers

; /* End of TrapFIQ
;	!!!! look at uCOS/subr.s for an example which does a context switch. */


; Initial dummy FIQ handler - gets overwritten when interrupts are installed
uHALir_FIQProcess
 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
	STMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x1C]
	CMP		r0, #0
	BEQ		_uHAL_FIQ			; No chain so use uHAL's exception handler.
	BL		Chainir_ChainWalk
_uHAL_FIQ
	LDMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
 ENDIF	
 ENDIF

	STMFD	sp!, {r4}
	MRS	r4, cpsr		; Re-enable interrupt
	BIC	r4, r4, #NoFIQ
	MSR	cpsr_c, r4
	LDMFD	sp!, {r4}
	SUBS	pc, r14, #4		; return from FIQ


uHALir_TestFIQ
	STMFD 	sp!, {r1 - r4, lr}
 	; Read which interrupt(s) is active (returns result in a1)
	READ_FIQ	a1, a2, a3
	BL		uHALir_FlagsTestFIQ

	LDMFD	sp!, {r1 - r4, pc}

	END
               
