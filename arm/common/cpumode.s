;/***************************************************************************
; Copyright ARM Limited 1999 - 2000.  All rights reserved.
;****************************************************************************
;
;  General processor mode change code. 
;  Routines to read/write Current Processor Status Register;
;	and Enter/Exit Supervisor mode.
;
;	$Id: cpumode.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;****************************************************************************/

	INCLUDE except_h.s
	INCLUDE retmacro.s

	EXPORT	uHALir_ReadMode		; Read Execution mode & IRQ.
	EXPORT	uHALir_WriteMode
	EXPORT	uHALir_EnterSvcMode	; Enter Supervisor mode.
	EXPORT	uHALir_EnterLockedSvcMode ; Enter Supervisor, disable ints.
	EXPORT	uHALir_ExitSvcMode	; Exit Supervisor mode.

	AREA	uHAL_MODE, CODE, READONLY

; ------------------------------------------------------------------
; int uHALr_ReadMode(void)
; void uHALr_WriteMode(int)
;
;	Read and write current processor mode.
;
uHALir_ReadMode
	MRS	r0, cpsr
	RETURN	lr

uHALir_WriteMode
	MSR	cpsr_c, r0		; change current psr
	RETURN	lr


; ------------------------------------------------------------------
; int uHALir_EnterSvcMode(void)
;
;	Routine to enter supervisor mode. Copies original CPSR to
;	SPSR for proper return.
;
;	Calling routine should protect CPSR and call
;	uHALir_ExitSvcMode() when done.
;
;	Corrupts r1, r2 and r3.
;
;	Standalone version Returns original CPSR in r0.
;
uHALir_EnterSvcMode
 IF :DEF: SEMIHOSTED
	STMFD	sp!, {r4-r6, lr}		; save working registers

	MRS	r6, cpsr

	LDR	r4, =uHALiv_SVCStackCount	; increment the SVC count
	LDR	r5, [r4]
	ADD	r5, r5, #1
	STR	r5, [r4]
	TEQ	r5, #1				; Is this the first time?
	BNE	%F1
	
	MOV	r0, #angel_SWIreason_EnterSVC	; Enter SVC mode
	SWI	SWI_Angel			; Returns EnterUSR routine in r0
	LDR	r4, =uHALiv_SVCExitRoutine	; Save return routine address
	STR	r0, [r4]

	MRS	r0, cpsr
	BIC	r0, r0, #MaskINTS
	AND r6, r6, #MaskINTS
	ORR r0, r6, r0
	MSR	cpsr_c, r0
1
	POP_RETURN	r4-r6

 ELSE
	MRS	r2, cpsr			; Protect saved psr
	MOV	r3, lr			; and return address

	MOV	r0, #angel_SWIreason_EnterSVC	; Enter SVC mode
	SWI	SWI_Angel		; Returns EnterUSR routine in r0

	MOV	r0, r2			; return old cpsr

	RETURN	r3

  ENDIF


; ------------------------------------------------------------------
; int uHALir_EnterLockedSvcMode(void)
;
;	Routine to enter supervisor mode & Lock out interrupts. Works
;	the same as uHALir_EnterSvcMode, but locks out IRQs at the
;	same time (saves having to call uHALir_DisableInt after
;	uHALir_EnterSvcMode.
;
;	Calling routine should protect CPSR and call
;	uHALir_ExitSvcMode when done.
;
;	Standalone version Corrupts r1, r2 and r3.
;
;	Returns original CPSR in r0.
;
uHALir_EnterLockedSvcMode
 IF :DEF: SEMIHOSTED
	STMFD	sp!, {r4-r6, lr}		; save working registers

	MRS	r6, cpsr

	LDR	r4, =uHALiv_SVCStackCount	; increment the SVC count
	LDR	r5, [r4]
	ADD	r5, r5, #1
	STR	r5, [r4]
	TEQ	r5, #1				; Is this the first time?
	BNE	%F2

	MOV	r0, #angel_SWIreason_EnterSVC	; Enter SVC mode
	SWI	SWI_Angel			; Returns EnterUSR routine in r0
	LDR	r4, =uHALiv_SVCExitRoutine	; Save return routine address
	STR	r0, [r4]

	AND r0, r6, #MaskINTS
2
	POP_RETURN	r4-r6

 ELSE
	MRS	r2, cpsr			; Protect saved psr
	MOV	r3, lr				; and return address

	MOV	r0, #angel_SWIreason_EnterSVC	; Enter SVC mode
	SWI	SWI_Angel			; Returns EnterUSR routine in r0
	MOV	lr, r3

	MRS	r3, cpsr			; get current psr
	ORR	r3, r3, #NoIRQ			; ..and Disable Interrupts
	ORR	r3, r3, #NoFIQ			; ..and FIQs
	MSR	cpsr_c, r3			; change current psr
	MOV	r0, r2				; return old cpsr

	RETURN	lr

 ENDIF


; ------------------------------------------------------------------
; void uHALir_ExitSvcMode(int spsr)
;
;	Routine to exit supervisor mode. Switches out of supervisor
;	& restores CPSR to given value. NOTE: no check is made on
;	the given spsr, so it must be valid! Interrupts are restored
;	to whatever was current before call to uHALir_EnterSvcMode().
;
;	Expects original CPSR in r0.
;
uHALir_ExitSvcMode
 IF :DEF: SEMIHOSTED
	STMFD	sp!, {r4-r6, lr}		; save working registers

	MRS	r6, cpsr
	BIC	r6, r6, #MaskINTS
	ORR r0, r6, r0
	MSR	cpsr_c, r0

	LDR	r4, =uHALiv_SVCStackCount	; decrement the SVC count
	LDR	r5, [r4]
	SUB	r5, r5, #1
	STR	r5, [r4]
	TEQ	r5, #0				; Should we stay in SVC mode?
	BNE	%F4

	; Get the address of the exit routine
	LDR	r4, =uHALiv_SVCExitRoutine
	LDR	r4, [r4]
	; we didn't use the SWI to get into SVC mode
	BEQ	%F4
	
	LDR	lr, =%F4
	MOV	pc, r4				; ...and call it
4
	POP_RETURN	r4-r6

 ELSE
	STMFD	sp!, {r1,r4-r5, lr}	; save working registers
	MSR		cpsr_c, r0		; change current psr

	POP_RETURN_EXTEND	r1, r4-r5

  ENDIF

 IF :DEF: SEMIHOSTED
	LTORG

	AREA uHAL_Irq, DATA

uHALiv_SVCStackCount	% 4	; count of times we've 'entered' SVC mode
uHALiv_SVCExitRoutine	% 4	; Semihosted's exit routine to leave SVC mode
 
 ENDIF

	END				; End of file

