;/*****************************************************************************
; Copyright ARM Limited 1999 - 2000.  All rights reserved.
;******************************************************************************
;  General FIQ and mode change code. NewFIQ installs the given
;  'C' routine as the high level handler for interrupt as well as the actual
;  vector. Routines to en/disable FIQs; 
;
;	$Id: fiqlib.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;******************************************************************************/

	INCLUDE	sizes.s
	INCLUDE	except_h.s
	INCLUDE	mmumacro.s
	GET	retmacro.s

	IMPORT	uHALir_CleanDCache

	EXPORT	uHALir_DisableFiq		; Disable FIQ.
	EXPORT	uHALir_EnableFiq


	AREA	uHAL_FIQs, CODE, READONLY

;	void uHALir_DisableFiq(void)
;	void uHALir_EnableFiq(void)
;
;	Disable and enable FIQ, preserving current CPU mode.
;
uHALir_DisableFiq
	STMFD	sp!, {v1}
	MRS	v1, cpsr
	ORR	v1, v1, #NoFIQ
	MSR	cpsr_c, v1
	LDMFD	sp!, {v1}
	RETURN	lr

 
uHALir_EnableFiq
	STMFD	sp!, {v1}
	MRS	v1, cpsr
	BIC	v1, v1, #NoFIQ
	MSR	cpsr_c, v1
	LDMFD	sp!, {v1}
	RETURN	lr


	END

