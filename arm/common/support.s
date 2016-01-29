;/***************************************************************************
; Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
; Copyright ARM Limited 1998 - 2001.  All rights reserved.
;****************************************************************************
;
;  C library initialization code and general routines to return memory
;	locations and sizes.
;
;	$Id: support.s,v 1.1 2005/12/19 11:45:57 akhe Exp $
;
;****************************************************************************/

	INCLUDE bits.s
	INCLUDE sizes.s
	INCLUDE except_h.s
	INCLUDE platform.s
	GET	retmacro.s			; Thumb/ARM interworking

	EXPORT	uHALir_InitBSSMemory		; Init 'C' variable space
	EXPORT	uHALir_InitStacks
	EXPORT  uHALr_LibraryInit
	EXPORT	__rt_stackheap_init
	EXPORT	_memcpy				; Required by 2.50 SDT ARMCC 
	IMPORT	uHALr_memcpy			; Required by _memcpy

	IMPORT	uHALir_StackHeapInit
	IMPORT	uHALiv_TopOfHeap		; Variables holding above values
	IMPORT	uHALiv_TopOfMemory
	IMPORT	uHALiv_BaseOfMemory
	IMPORT  uHALiv_FileCount
	IMPORT	uHALiv_MemorySize		; Memory Size

	IMPORT	uHALir_FloatInit
	IMPORT	uHALir_EnterSvcMode	; Enter Supervisor mode.
	IMPORT	uHALir_ExitSvcMode	; Exit Supervisor mode.
	IMPORT 	uHALr_InitHeap

	IMPORT	printf, WEAK
	IMPORT	uHALir_Defaultprintf
	EXPORT	uHALr_printf

 IF USE_C_LIBRARY = 0
	EXPORT	|_main_arg|			; Program arguments
	EXPORT	__rt_stkovf_split_small
	EXPORT	__rt_stkovf_split_big		; ..but they do nothing.
 ENDIF

 IF :LNOT: :DEF: SEMIHOSTED :LAND: ADS_BUILD = 1
 IF {FPU} = "VFP"
	IMPORT	VFPir_Init
 ENDIF
 ENDIF

	IMPORT  _init_alloc, WEAK
	IMPORT  _initio, WEAK
	IMPORT	__rt_malloc, WEAK
	IMPORT	Chainir_Init, WEAK

 IF uHAL_HEAP = 1 :LAND: USE_C_LIBRARY = 1 :LAND: ADS_BUILD = 0
	EXPORT __heap_base
	EXPORT __heap_limit
	EXPORT __stack_base
	EXPORT __stack_limit
 ENDIF	

	KEEP

	AREA	uHAL_SupportCode, CODE, READONLY

	INCLUDE linkdata.s		; Any compiler dependencies are in here
	EXPORT	uHAL_EndOfBSS

; Initializes the heap limits for ADS and uHAL heap management.
; Returns heap base and heap limit in r0 and r1 respectively.
__rt_stackheap_init
	STMFD	sp!, {r2 - r4, r12, lr}

	LDR	r0, uHAL_StartOfROM
	LDR	r1, uHAL_TopOfROM
	LDR	r2, uHAL_StartOfBSS
	LDR	r3, uHAL_EndOfBSS
	
	BL	uHALir_StackHeapInit

	LDR	r4, =uHALiv_BaseOfMemory
	LDR	r0, [r4]		; Here's the top of free RAM
	LDR	r4, =uHALiv_TopOfHeap	; Top of installed memory
	LDR	r1, [r4]

	POP_RETURN_EXTEND	r2-r4, r12

; Generic uHAL / ADS (SDT) / VFP / Chaining library
; initialization. Use this routine to init all the 
; required libraries used in the image. 
uHALr_LibraryInit
	STMFD	sp!, {r0, r2 - r4, lr}		; save working registers

 IF ADS_BUILD = 1
; Must keep the next two routine calls in order, init_alloc
; needs the heap base and limit in r0 and r1 respectively.
 	BL 	__rt_stackheap_init

	LDR	r4, =_init_alloc
	TEQ 	r4, #0x00000000
	BLNE	_init_alloc
	BLEQ	uHALr_InitHeap
	MOV	r2,#0x00		; Clear the number of files opened.
	LDR	r0,=uHALiv_FileCount	; stderr, stdout, stdin unopened !.
	STR 	r2, [r0]		

	LDR	r4, =_initio
	TEQ 	r4, #0x00000000
	BLNE 	_initio			; Opens stdin, stdout and stderr
	
 IF :LNOT: :DEF: SEMIHOSTED :LAND: ADS_BUILD = 1
 IF {FPU} = "VFP"
	; Only init'd if standalone image and built for VFP hardware.
	BL	VFPir_Init		
 ENDIF
 ENDIF
 ELSE ; SDT Build
        BL	uHALir_SDTLibraryInit
 ENDIF	
	BL	uHALir_FloatInit

	LDR	r1, =Chainir_Init	; Initialize the vector chains
	TEQ 	r1, #0x00000000		; if built to use them.
	BLNE	Chainir_Init

	POP_RETURN_EXTEND	r0, r2-r4


;****************************************************************
; The 2.50 C compiler makes references to _memcpy,
; we don't have this routine, therefore will map this
; to uHALr_memcpy.
_memcpy
	B	uHALr_memcpy
;****************************************************************


 IF uHAL_HEAP = 1 :LAND: USE_C_LIBRARY = 1 :LAND: ADS_BUILD = 0

	EXPORT	uHALir_SDTLibraryInit

	IMPORT	__rt_malloc, WEAK
	IMPORT	uHALr_malloc
	IMPORT	uHALir_InitTargetMem

; Only included for compatibility with SDT 2.50, initializes
; the function pointer for malloc, if its there.
uHALir_SDTLibraryInit
	STMFD	sp!, {r0 - r4, r12, lr}
	BL	uHALir_EnterSvcMode
	STMFD	sp!, {r0}		; save working registers

	BL	uHALir_InitTargetMem
	LDR	r1, =uHALiv_TopOfMemory
	SUB	r0, r0, #1
	STR	r0, [r1]

	LDR	r0, =uHALr_malloc
	LDR	r1, =__rt_malloc
	TEQ 	r1, #0x00000000
	STRNE	r0, [r1]
	BLNE	uHALr_InitHeap

	LDMFD	sp!, {r0}		; restore registers & return
	BL	uHALir_ExitSvcMode	; needs old mode in r0 

	POP_RETURN_EXTEND	r0 - r4, r12

	LTORG

	AREA HAL_Heap, DATA, READONLY

__heap_base	DCD	(uHAL_HEAP_BASE)
__heap_limit	DCD	(uHAL_HEAP_BASE + uHAL_HEAP_SIZE)
__stack_base	DCD	(uHAL_STACK_BASE)
__stack_limit	DCD	(uHAL_STACK_BASE - uHAL_STACK_SIZE)

 ENDIF	

; Included here to provide support for either the C library
; version of printf or the uHAL version (uHALir_Defaultprintf)
uHALr_printf
	STMFD	sp!, {r0}

	LDR	r0,=printf
   	TEQ     r0, #0x00000000
	LDMFD	sp!, {r0}

   	BNE    	printf
	B	uHALir_Defaultprintf

;*******************************************************************
; The remainder of this file provides generic support code for
; stack initialization, C memory initialization and other functions
; required by the compiler.
;*******************************************************************

;	uHALir_InitStacks
;
; 'Routine' called from boot-up to set all stacks.
; Although we have no stack yet, r14 contains return address. r0 contains
; top of memory (don't corrupt!) and all stacks are relative to this value.
;
; FIQ stack isn't set in this example.
;
uHALir_InitStacks
	MOV	r6, r14			; !!! Protect the return address !!!

	; Any modes which are only used to cause a reset can all work in
	; the same stack space, since only one can occur at any one time.
	MRS	r4, cpsr
	ORR	r4, r4, #NoIRQ		; No IRQs & blank mode
	ORR	r4, r4, #NoFIQ		; Or FIQs
	BIC	r4, r4, #ModeMask

	; Undefined & Abort modes can share same space.
	ORR	r5, r4, #Undef32Mode
	MOV	r2, #User32Mode
	MSR	cpsr_c, r5
	MSR	spsr_c, r2
	BIC	r7, r0, #3		; Must be word-aligned
	MOV	sp, r7			; Initial Undefined stack pointer...
	ORR	r5, r4, #Abort32Mode
	MSR	cpsr_c, r5
	MSR	spsr_c, r2

	; You can save space by combining the stacks for abort & Undefined.

 IF :LNOT: :DEF: MINI_STACKS

	SUB	r7, r7, #AbortStackSize
	BIC	r7, r7, #3		; Must be word-aligned

 ENDIF

	MOV	sp, r7			; Initial Abort stack pointer...
	SUB	r7, r7, #UndefStackSize
	BIC	r7, r7, #3		; Must be word-aligned

	; Setup FIQ stacks.
	; This is optional. Most uHAL components do not use FIQ's
	; If you want to use FIQ's, define UHAL_USE_FIQ = 1
	; in platform.s

 IF :DEF: UHAL_USE_FIQ
  
  IF UHAL_USE_FIQ = 1
	ORR	r5, r4, #(NoFIQ | FIQ32Mode)
	MSR	cpsr_c, r5
	MSR	spsr_c, r2
	MOV	sp, r7			; Initial FIQ stack pointer...
	SUB	r7, r7, #FIQStackSize
	BIC	r7, r7, #3		; Must be word-aligned
  ENDIF
 
 ENDIF

	; Setup IRQ stacks.
	ORR	r5, r4, #IRQ32Mode
	MSR	cpsr_c, r5
	MSR	spsr_c, r2
	MOV	sp, r7			; Initial IRQ stack pointer...
	SUB	r7, r7, #IRQStackSize
	BIC	r7, r7, #3		; Must be word-aligned

	; Setup Supervisor stacks.
	ORR	r5, r4, #SVC32Mode
	MSR	cpsr_c, r5
	MSR	spsr_c, r2
	MOV	sp, r7			; Initial SVC stack pointer...
	SUB	r7, r7, #SVCStackSize
	BIC	r7, r7, #3		; Must be word-aligned
	MOV	r10, r7			; Bottom of SVC stack

 IF :LNOT: :DEF: REAL_USERMODE

	; Switch into System mode (v4 architecture only)
	ORR	r5, r4, #Sys32Mode
	MSR	cpsr_c, r5
	SUB	r7, r7, #UserStackSize
	BIC	r7, r7, #3		; Must be word-aligned
	MOV	r10, r7			; Bottom of stacks

	; Save the top of free RAM = TopOfMemory - all stacks
	MOV	r1, r7			; Return top of free RAM in r1

	ORR	r5, r4, #SVC32Mode	; Switch back to SVC mode
	MSR	cpsr_c, r5

 ELSE

	; If the target isn't v4, or the application _really_ wants to
	; switch into (and out of) User mode, define REAL_USERMODE=1
	;-----------------------------------------------------------------
	; Need to protect stack pointer from the SWI. When switching
	; modes Semihosted also switches stack so we end up with the user
	; stack. Semihosted SWIs promise to protect r1, r2 and r3, so copy
	; TopOfMemory to r2 and sp to r3.
	MOV	r2, r0			; Protect TopOfMemory
	MOV	r3, sp			; Protect SVC stack

	; Setup User/System mode stacks.
	ORR	r5, r4, #User32Mode
	MSR	cpsr_c, r5
	MOV	sp, r7			; Initial User stack pointer...
	SUB	r7, r7, #UserStackSize
	BIC	r7, r7, #3		; Must be word-aligned
	MOV	r10, r7			; Bottom of stacks

	; Save the top of free RAM = TopOfMemory - all stacks
	MOV	r1, r7			; Return top of free RAM in r1

	; Switch back to supervisor & recover stack & TopOfMemory.
	MOV	r0, #angel_SWIreason_EnterSVC
	SWI	SWI_Angel		; Returns EnterUSR routine in r0
	MOV	sp, r3			; Restore SVC stack
	MOV	r0, r2			; Restore TopOfMemory

 ENDIF

	MOV	r11, #0			; No previous frame, so fp=0
	; Just return back to boot up code
 	RETURN	r6


;	uHALir_InitBSSMemory
;
; Routine called from boot-up to initialise all memory used by 'C' and any
; predefined assembler data areas. Use of indirect links to real linker
; variables allows any memory map to be used & hides linker specifics.
; This is a real subroutine, called after stack initialisation.

uHALir_InitBSSMemory
	STMFD	sp!, {r4-r8, r14}
	LDR	r4, uHAL_TopOfROM	; Copy predefined variables from here..
	LDR	r5, uHAL_StartOfBSS	; ..to here
	LDR	r6, uHAL_ZeroBSS	; 'C' BSS starts here
	CMP	r4, r5			; Make sure there are some..
	BEQ	%8
7
	CMP	r5, r6			; Check if done..
	LDRCC	r7, [r4], #4		; if not, get word and store to RAM
	STRCC	r7, [r5], #4
	BCC	%7
8
	LDR	r4, uHAL_EndOfBSS	; Top of zero init memory

; By default all pre-defined RAM areas are zero-initialized. Some MMU data
; is stored in RAM, so skip over this portion of the ZI area.
; This can be avoided using scatter-loading.

	LDR	r7, uHAL_NonZeroBSS	; Start of MMU tables
	LDR	r8, uHAL_EndOfNonZ	; End of MMU tables

	MOV	r5, #0
9
	; Don't zero NonZero area!
	CMP	r6, r7			; >= uHAL_NonZeroBSS
	CMPCS	r8, r6			; & < uHAL_EndOfNonZ ?
	MOVCS	r6, r8			; Yes, just skip completely

	CMP	r6, r4			; Check if done..
	STRCC	r5, [r6], #4		; if not, clear another word
	BCC	%9

	; Save the Base of Free RAM.
	; This our best guess so far the end of the vectors and fiq code.
	LDR	r5, =uHALiv_BaseOfMemory
	MOV	r4, #0x400		; Beyond vectors and FIQ code.
	LDR	r8, =RAM_BASE		; Add RAM BASE offset.
	ADD	r4, r4, r8
	STR	r4, [r5]

	; restore registers and return
	POP_RETURN 	r4-r8


; STacK OVerFlow handling routine - minimum required by ARM Linker without
; defining no stack checking.
; void __rt_stkovf_split_big(void);
|__rt_stkovf_split_big|
 	; just return
	RETURN	lr


; STacK OVerFlow handling routine - minimum required by ARM Linker without
; defining no stack checking.
; void __rt_stkovf_split_small(void);
|__rt_stkovf_split_small|
 	; just return
	RETURN	lr


|_main_arg|
 	; just return
	RETURN	lr


        END

; End of file - support.s

