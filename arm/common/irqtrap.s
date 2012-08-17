;/*****************************************************************************
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2000.  All rights reserved.
;******************************************************************************
; IRQ trap handling code. TrapIRQ filters the interrupt before passing it on.
;
;	$Id: irqtrap.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;******************************************************************************/


	INCLUDE platform.s
	INCLUDE except_h.s
	INCLUDE target.s
	GET	retmacro.s			; Thumb/ARM interworking

	IMPORT	uHALp_StartIRQ		; Pointers to 'C' routines.
	IMPORT	uHALp_HandleIRQ
	IMPORT	uHALp_FinishIRQ
	IMPORT	uHALir_FlagsTestIRQ

	EXPORT	uHALir_TrapIRQ		; Simple IRQ Trap Handler
	EXPORT	uHALir_IRQProcess	; Dummy handler installed @ boot
	EXPORT  uHALir_TestIRQ

   IF :DEF: CHAIN_VECTORS 
   IF CHAIN_VECTORS = 1
    IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
   ENDIF
   ENDIF

	AREA	uHal_Traps, CODE, READONLY


; Indirect pointers to IRQ handling routines.
_pStartIRQ	DCD uHALp_StartIRQ
_pHandleIRQ	DCD uHALp_HandleIRQ
_pFinishIRQ	DCD uHALp_FinishIRQ

;	uHALir_TrapIRQ - IRQ trap handler
;
; Wrapper to service the trap and call a high-level handler. This routine 
; saves all the registers in an APCS compliant manner, calls a StartIRQ
; function (if defined), reads the interrupt mask & calls the high-level
; handler; calls a FinishIRQ routine (if defined) and if FinishIRQ returns a
; value, jump to it as an address to finish IRQ processing.
;
; This mechanism provides an interface to 'C' for quite complex processing
; based on IRQs, such as simple context-switching using the timer IRQ ticks.
; By not defining StartIRQ/FinishIRQ, simple IRQ processing is automatic.
;
uHALir_TrapIRQ
	; We take the cycle hit of modifying the return address here,
	; to simplify the code needed to return to the interrupted
	; thread, possibly performing a context switch on the way.
	SUB	lr, lr, #4

	; At this points IRQs are disabled, so we know that the SPSR
	; register will not be trashed by another interrupt.
        STMFD   sp!, {r0-r12, lr}       ; save registers
        MRS     v1, SPSR		; push SPSR to allow nested interrupts
        STMFD   sp!, {v1}

	LDR	a3, _pStartIRQ		; Indirect pointer to StartIRQ
	LDR	a2, [a3]		; Check for a StartIRQ routine
	CMP	a2, #0
	MOVNE	lr, pc			; arrange for a safe return here
	                     ; Ensure that this and the next instruction
	                     ; are not separated (either of the next options)
	; Some generic task which is required at the start of an FIQ
	RETURN_COND_EXTEND	NE, a2

 	; Read which interrupt(s) is active (returns result in a1)
	READ_INT	a1, a2, a3

	LDR	a3, _pHandleIRQ		; Indirect pointer to IRQ Handler
	LDR	a3, [a3]		; load pointer to a register
	MOV     lr, pc			; arrange for a safe return here
	
	; The high-level handler dispatches the FIQ to the proper
	; user defined ISR.
	RETURN	a3			; call high-level handler
			    ; The above two instructions must not be split

	LDR	a3, _pFinishIRQ		; Indirect pointer to FinishIRQ
	LDR	a1, [a3]		; Check for a FinishIRQ routine
	CMP	a1, #0
	MOVNE	lr, pc			; arrange for a safe return here
	; Some generic task which is required at the end of an IRQ
	RETURN_COND_EXTEND	NE, a1

;; !!!! NOTE: If FinishIRQ does not return 0, jump to the returned address.
;; !!!!	      So make sure your target routine matches this stack format &
;; !!!!       recovers from irq-mode properly.

	CMP	a1, #0			; a1 is non-zero for IRQs which need
	RETURN_COND_EXTEND	NE, a1	; to do further processing
	
	LDMFD	sp!, {v1}		; recover SPSR value from stack
	MSREQ	SPSR_c, v1		; restore the SPSR
	LDMFD	sp!, {r0-r12, pc}^	; Restore saved registers

; /* End of TrapIRQ
;	!!!! look at uCOS/subr.s for an example which does a context switch. */


; Initial dummy IRQ handler - gets overwritten when interrupts are installed
uHALir_IRQProcess
 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
	STMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x18]
	CMP		r0, #0
	BEQ		_uHAL_IRQ			; No chain so use uHAL's interrupt handler.
	BL		Chainir_ChainWalk
_uHAL_IRQ
	LDMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
 ENDIF	
 ENDIF

	STMFD	sp!, {r4}
	MRS	r4, cpsr		; Re-enable interrupt
	BIC	r4, r4, #NoIRQ
	MSR	cpsr_c, r4
	LDMFD	sp!, {r4}
	SUBS	pc, r14, #4		; return from IRQ


uHALir_TestIRQ
	STMFD 	sp!, {r1 - r4, lr}
 	; Read which interrupt(s) is active (returns result in a1)
	READ_INT	a1, a2, a3
	BL		uHALir_FlagsTestIRQ

	LDMFD	sp!, {r1 - r4, pc}

	END

