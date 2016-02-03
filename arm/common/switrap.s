;/**************************************************************************
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2000.  All rights reserved.
;***************************************************************************
; SWI trap handling code. TrapSWI filters the interrupt before passing it on.
;
;	$Id: switrap.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;***************************************************************************/


	INCLUDE except_h.s

	EXPORT	uHALir_TrapSWI		; SWI exception handler

	IMPORT	uHALr_putchar
	IMPORT  print_sys_buf
	IMPORT 	fil_sys_buf
	IMPORT 	clock, WEAK

	IMPORT  uHALiv_FileCount	; Count of opened files.
	IMPORT	uHALiv_TopOfHeap	; Variables holding above values

 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
   	IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
	IMPORT	Chainir_InfoSWI
	IMPORT	Chainir_ChainEntry
 ENDIF
 ENDIF

	AREA	uHal_Traps, CODE, READONLY


; SWI handler - currently only handles switch to SVC mode.
uHALir_TrapSWI
	; SWI switches to SVC mode, with no IRQs (ARM Architecture Reference)
	STMFD	sp!, {r0-r1}

 IF :DEF: THUMB_AWARE   ; SWIs are different in THUMB mode
	MRS	r0, spsr		; Get spsr
	TST	r0, #PSR_T_bit		; Occurred in Thumb state?
	LDRNEH	r0, [lr,#-2]		; Yes: Load halfword and...
	BICNE	r0, r0, #0xFF00		; ...extract comment field
	LDREQ	r0, [lr,#-4]		; No: Load word and...
	BICEQ	r0, r0, #0xFF000000	; ...extract comment field
   
        ; r0 now contains SWI number
        
	LDR	r1, =SWI_Angel		; Is it an ARM State Angel SWI?
	CMP	r0, r1
	BEQ	swi_correct

	LDR	r1, =SWI_Angel_Thumb
	CMP	r0, r1
	BEQ	swi_correct

	; If we get here we can't service the SWI so return unknown
	
	LDMFD	sp!, {r0-r1}		; restore register
	B	uHALil_ExitSWI		; Unknown SWI

swi_correct
	LDMFD	sp!, {r0-r1}		; restore register


 ELSE                    ; some processors are not Thumb aware
	LDR	r0, [lr, #-4]		; Read SWI instruction
	BIC	r0, r0, #0xff000000	; extract the SWI number

	LDR	r1, =SWI_Angel
	CMP	r0, r1
	LDMFD	sp!, {r0-r1}		; restore register
	BNE	uHALil_ExitSWI		; Unknown SWI
 ENDIF

	CMP	r0, #angel_SWIreason_EnterSVC
	BEQ	uHALir_SWIEnterOS	; return in SVC mode

  IF :DEF: CHAIN_VECTORS 
  IF CHAIN_VECTORS = 1
	CMP		r0, #SYS_AGENTINFO
	BEQ		Chainir_InfoSWI

	CMP		r0, #SYS_VECTORCHAIN
	BEQ		Chainir_ChainEntry

	STMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x8]
	CMP		r0, #0
	BEQ		_uHAL_SWI			; No chain so use uHAL's exception handler.
	BL		Chainir_ChainWalk
_uHAL_SWI
	LDMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.

  ENDIF	
  ENDIF

	CMP r0, #SYS_FILE_OPEN
	BEQ uHAL_Sys_FileOpenSWI

	CMP r0, #SYS_READ_SWI
	BEQ uHAL_Sys_ReadSWI

	CMP r0, #SYS_WRITE_SWI
	BEQ uHAL_Sys_WriteSWI

	CMP r0, #angel_SWI_SYS_WRITEC
	BEQ uHAL_ttywrch_SWI

	CMP r0, #SYS_CLOCK
	BEQ uHALil_readtime_SWI

	CMP	r0, #angel_SWI_SYS_HEAPINFO
	BNE	uHALil_ExitSWI		; Unknown SWI


	; Load area pointed to by r1 with heap and stack info
	STMFD	sp!, {r0-r4}
	LDR	r0, [r1]		; Pointer to variables
	LDR	r4, =uHALiv_TopOfHeap
	LDR	r3, [r4], #4		; top of free RAM
	LDR	r2, [r4], #8		; base of free RAM
	STR	r2, [r0], #4
	STR	r3, [r0], #4
	LDR	r2, [r4], #4		; base of stack
	LDR	r3, [r4]
	STR	r2, [r0], #4
	STR	r3, [r0]
	LDMFD	sp!, {r0-r4}		; restore register

uHALil_ExitSWI
	MOV	r0, #0			; Pretend it was all okay
	MOVS	pc, lr			; just ignore SWI
      ; MOVS pc.. will return the Thumb state if called
      ;           from that state
      
uHALir_SWIEnterOS
	STMFD	sp!, {r0}
	MRS	r0, SPSR		; Get spsr
	TST	r0, #PSR_T_bit		; Occurred in Thumb state?
	ORRNE	r0, r0, #0x13		; Put into SVC mode
	MSRNE	SPSR_cxsf, r0		; Save SPSR
	LDMFD	sp!, {r0}		; restore register
	MOVNES	pc, lr
	MOV	pc, lr

uHAL_Sys_FileOpenSWI
	STMFD	sp!, {r4, lr}
	LDR	r0, =uHALiv_FileCount
	LDR	r4, [r0]		; Test the number of opened files
	ADD 	r4, r4,#0x01
	STR	r4, [r0]
	CMP	r4, #0x03		; If < 3 must be stdin, stdout, stderr
	MOVGT	r0, #0xffffffff 
	MOVLT	r0, #0x00000000
	LDMFD	sp!, {r4, lr}		; restore registers and return
	MOVS	pc, lr

uHAL_ttywrch_SWI
	STMFD	sp!, {r0, lr}
	LDR	r0, [r1]
	BL	uHALr_putchar		; Print single character from message.
	LDMFD	sp!, {r0, lr}		; restore registers and return
	MOVS	pc, lr

uHAL_Sys_WriteSWI
	STMFD	sp!, {r1-r3, lr}
	MOV	r0, r2
	LDR 	r1, [r1,#4]
	BL 	print_sys_buf		; Print the stream buffer.
	LDMFD	sp!, {r1-r3, lr}	; restore registers and return
	MOVS	pc,lr

uHAL_Sys_ReadSWI
	STMFD	sp!, {lr}
	MOV	r0, r2
	LDR 	r1, [r1,#4]
	BL 	fil_sys_buf		; Fill the stream buffer.
	LDMFD	sp!, {lr}		; restore registers and return
	MOVS	pc, lr

uHALil_readtime_SWI
	STMFD	sp!, {r1-r3, lr}
	BL	clock			; What time is it, Mr. Wolf?
	LDMFD	sp!, {r1-r3, lr}	; restore registers and return
	MOVS	pc, lr

	END








