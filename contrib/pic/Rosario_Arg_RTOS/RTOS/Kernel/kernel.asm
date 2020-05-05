;/**********************************************************************/
;/*                                                                    */
;/* File name: kernel.asm                                              */
;/*                                                                    */
;/* Since:     2004-Sept-20                                            */
;/*                                                                    */
;/* Version:   PICos18 v2.10                                           */
;/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
;/*                                                                    */
;/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
;/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
;/*            NIELSEN  Peter  [PN]                   pnielsen@tuug.fi */
;/*                                                                    */
;/* Purpose:   Kernel core.                                            */
;/*                                                                    */
;/* Distribution: This file is part of PICos18.                        */
;/*            PICos18 is free software; you can redistribute it       */
;/*            and/or modify it under the terms of the GNU General     */
;/*            Public License as published by the Free Software        */
;/*            Foundation; either version 2, or (at your option)       */
;/*            any later version.                                      */
;/*                                                                    */
;/*            PICos18 is distributed in the hope that it will be      */
;/*            useful, but WITHOUT ANY WARRANTY; without even the      */
;/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
;/*            PARTICULAR PURPOSE.  See the GNU General Public         */
;/*            License for more details.                               */
;/*                                                                    */
;/*            You should have received a copy of the GNU General      */
;/*            Public License along with gpsim; see the file           */
;/*            COPYING.txt. If not, write to the Free Software         */
;/*            Foundation, 59 Temple Place - Suite 330,                */
;/*            Boston, MA 02111-1307, USA.                             */
;/*                                                                    */
;/*          > A special exception to the GPL can be applied should    */
;/*            you wish to distribute a combined work that includes    */
;/*            PICos18, without being obliged to provide the source    */
;/*            code for any proprietary components.                    */
;/*                                                                    */
;/* History:                                                           */
;/* 2004/09/20 [XM]  Create this file.                                 */
;/* 2005/03/27 [XM]  Added "movlb 0" after every GLOBAL symbol.        */
;/* 2005/10/29 [XM]  Added the RESET_ON_KERNEL compiler option         */
;/*                  (used in case of kernel panic - stack overflow).  */
;/* 2005/11/12 [XM]  Added "clrf    POSTINC0" during Init_RAM process. */
;/* 2006/05/21 [GW]  Refactor the code, and enrich the comments.       */
;/* 2007/01/01 [PN]  Set INTCON2=0x80 as by default.                   */
;/*                                                                    */
;/**********************************************************************/


;   #include "device.inc"
#include <p18cxxx.inc>

;/*  Should be equal to .tmpdata + MATH_DATA size (from map file)      */
#define TEMP_SIZE   0x20

;/**********************************************************************
; * Kernel RAM area.
; **********************************************************************/
KERNEL_RAM      UDATA
id_tsk_run      RES 1                   ; ID of the running task
temp            RES 1
temp2           RES 1
permut          RES 1                   ; flag on continue permut or not
kernelState     RES 1
kernelPanic     RES 1                   ; Left nibble holds the taskID in
                                        ; which stack overflow occurred
                                        ; Right nibble: neighbor task got
                                        ; impacted by the stack overflow
startAddressL   RES 1                   ; Holds the task's code address
startAddressH   RES 1


;/**********************************************************************
; * Share global variables with other sections.
; **********************************************************************/
    GLOBAL  id_tsk_run  
    GLOBAL  kernelState
    GLOBAL  kernelPanic

;/**********************************************************************
; * Shared variables defined in other section.
; **********************************************************************/
    extern  tsk_1_state_ID              ; @ pro_man.c
    extern  tsk_16_state_ID

    extern  tsk_1_activ_prio            ; @ pro_man.c
    extern  tsk_16_activ_prio

    extern  FSR1H_MAIN                  ; @ pro_man.c
    extern  FSR1L_MAIN

    extern  taskdesc_addr               ; @ taskdesc.c (User Defined)

    extern  stack_low                   ; @ pro_man.c
    extern  stack_high

;/**********************************************************************
; * Hook routines activated by the -D compiler option.
; **********************************************************************/
#IFDEF STARTUPHOOK
    extern StartupHook
#ENDIF

#IFDEF PRETASKHOOK
    extern PreTaskHook
#ENDIF

#IFDEF POSTTASKHOOK
    extern PostTaskHook
#ENDIF

;/**********************************************************************
; * Macros used by KERNEL.
; **********************************************************************/
setupTBLPTR  macro   pointer2rom
    clrf    TBLPTRU
    movff   pointer2rom+1, TBLPTRH      ; Load TBLPTR with the address of
    movff   pointer2rom, TBLPTRL        ; taskdesc table located in ROM 
    endm

locateTaskIdField   macro
    movlw   6
    addwf   TBLPTRL, f                  ; Adjust TBLPTR to point to the
    clrf    WREG                        ; Task ID field of the current
    addwfc  TBLPTRH, f                  ; entry of the taskdesc table
    endm

locateStackAddrField   macro
    movlw   6                           ; After extracting the task ID,
    subwf   TBLPTRL, f                  ; Rewind the pointer back, to  
    clrf    WREG                        ; point to stack address field
    subwfb  TBLPTRH, f                  ; of the current taskdesc entry
    endm

swapTaskCtrlEntry  macro
    movff   POSTDEC0, temp2             ; SWAP 2 successive task entries
    movf    INDF0, w                    ; same macro can be used for
    movff   temp2, POSTINC0             ; various TCT(task control table)
    movwf   INDF0
    endm

setupFSR0    macro   taskCtrlTable
    clrf    FSR0H                       ; Setup FSR0 to point to the
    movlw   taskCtrlTable               ; 1st entry of one of the four
    movwf   FSR0L                       ; TCTs (task control tables)
    endm
    
loadNextAddrTo  macro   RegisterL, RegisterH
    TBLRD   *+                          ; Load Regsiter with the next 2
    movf    TABLAT, w                   ; bytes of the task descriptor,
    movwf   RegisterL                   ; taskdesc entry pointed to by
    TBLRD   *+                          ; TBLPTR 
    movf    TABLAT, w
    movwf   RegisterH
    endm

addStackSizeTo  macro   RegisterL, RegisterH
    TBLRD   *+                          ; By adding the stack size
    movf    TABLAT, w                   ; specified in the taskdesc,
    addwf   RegisterL, f                ; the designated Register will
    clrf    WREG                        ; point to the end of stack
    addwfc  RegisterH, f
    TBLRD   *+ 
    endm

setupStackSentinel    macro
    movf    POSTDEC1, w                 ; Setup sentinel 0xadde, 2 bytes
    movlw   0xad                        ; ahead of where FSR1 points to.
    movwf   POSTDEC1                    ; This is the sentinel to mark
    movlw   0xde                        ; both start and end of stack,
    movwf   INDF1                       ; destroy of signature suggests
    endm                                ; stack overflow occurred, i.e.
                                        ; kernel panic
locateTaskDescEntry   macro
    local   _matchTaskID, _nextTaskEntry, _found
    setupTBLPTR   taskdesc_addr         ; Locate the Task ID field of
    locateTaskIdField                   ; the 1st Task Descriptor entry
    movlb   0
_matchTaskID  
    movf    id_tsk_run, w               ; Get current running Task ID
    TBLRD   *+                          ; Get the Task ID from taskdesc
    cpfseq  TABLAT                      ; entry, see if it's current Task
    bra     _nextTaskEntry              ; If not, proceed with next Task
    bra     _found                      ; else exit from the macro
_nextTaskEntry
    movlw   8                           ; Move on to next Task entry by
    addwf   TBLPTRL, f                  ; offsetting TBLPTR with +8
    clrf    WREG
    addwfc  TBLPTRH, f
    bra     _matchTaskID                ; Go back to match task ID
_found  
    endm

ifSentinelDestroyGoto   macro place
    movff   POSTDEC0, WREG              ; Check stack Sentinel, 0xadde
    movlw   0xad
    movff   POSTDEC0, FSR1L             ; If the signature was not  
    cpfseq  FSR1L                       ; there any more then 
    goto    place                       ; go to the 'place' designated
    movlw   0xde
    movff   POSTDEC0, FSR1L
    cpfseq  FSR1L
    goto    place
    endm

markRunningTaskSuspended macro
    local   _nextTableEntry
    setupFSR0    tsk_1_state_ID-1
_nextTableEntry
    incf    FSR0L                       ; Proceed all the way down to
    movlw   0x1F                        ; the matched task ID inside
    andwf   INDF0, w                    ; tsk_xx_state_ID task control
    cpfseq  id_tsk_run                  ; table
    bra     _nextTableEntry
    bcf     INDF0, 5                    ; Mark the task as SUSPENDED
    bcf     INDF0, 6
    bcf     INDF0, 7
    endm    

;/**********************************************************************
; * ------------------------- KERNEL INIT ------------------------------
; *
; * Here is the init phase of the kernel, it means the main access to 
; * the kernel after StartOS call.
; *
; **********************************************************************/
KERNEL_ROM  CODE
_kernel
    GLOBAL  _kernel                     ; Beginning of kernel code
 
    clrf    FSR0H
    clrf    FSR0L
    clrf    FSR1L
    clrf    FSR1H
    clrf    TBLPTRU
    clrf    PCLATH

    movlw   0xFF                        ; Clear the first RAM bank
_kInitBank0
    clrf    POSTINC0
    cpfseq  FSR0L
    bra     _kInitBank0
    clrf    POSTINC0                    ; Clear last byte of the bank
    
    clrf    FSR0H                       ; Point FSR0 back to 0x0000
    
    clrf    STKPTR                      ; Clear the hardware stack
    movlw   0x1F                        ; Total 31 of H/W stack entries
_kInitHardwareStack
    incf    STKPTR
    clrf    TOSU
    clrf    TOSH
    clrf    TOSL
    cpfseq  STKPTR
    bra     _kInitHardwareStack
    
    clrf    STKPTR                      ; Is this redunant? later on
    incf    STKPTR                      ; inside _parser STKPTR will
                                        ; be set to 1 anyway
    movlw   0x88                        ; No prescaler for Timer 0
    movwf   T0CON

    clrf    PIE1
    clrf    PIE2
    clrf    RCON
    clrf    IPR1
    clrf    IPR2
    movlw   0x80
    movwf   INTCON2                     
    bsf     RCON, 7                     ; IPEN enabled
    movlw   0xA0                        ; GIEH enable
    movwf   INTCON                      ; GIEL disabled (TMR0 enabled)
    bsf     INTCON, 2                   ; Set TM0IF, force update on TMR0

_parser
    movlb   0                           ; Init
    movlw   0x01
    movwf   STKPTR              
    clrf    temp                
    movwf   permut                      ; Setup initial permutaion flag              
    clrf    id_tsk_run                  ; No task running to begin with
    setupFSR0    tsk_1_activ_prio       ; Locate tsk_xx_activ_prio table
    setupTBLPTR  taskdesc_addr          ; Locate 1st Task Desc. entry

;/* ------------ TASKDESC.C CONTENT PARSING -------------------------- */
_parserInit
    TBLRD   *+
    movf    TABLAT, w
    btfsc   STATUS, Z                   ; Task Desc. ends with an entry
    bra     _parserUpdate               ; of all-Zero fileds
    movff   TABLAT, INDF0               ; Extract & Setup task priority

    loadNextAddrTo FSR1L, FSR1H         ; Extract & setup stack pointer
    setupStackSentinel                  ; Put a signature 0xadde in
                                        ; front of the stack
    movf    POSTINC1, w                 ; Restore stack pointer FSR1
    movf    POSTINC1, w
    
    TBLRD   *+                          ; Skip over the TASK addr field
    TBLRD   *+

    bcf     FSR0L, 4                    ; Switch to tsk_xx_state_ID table
                                        ; by clearing bit 4, 0x0B0->0x0A0
    TBLRD   *+                          ; Extract & Setup task's state
    movf    TABLAT, w
    addwf   INDF0, f

    TBLRD   *+                          ; Extract & Setup task ID
    movf    TABLAT, w
    addwf   INDF0, f

    addStackSizeTo  FSR1L, FSR1H

    setupStackSentinel                  ; Put a signature, 0xadde at
                                        ; the end of the stack
    incf    FSR0L                       ; Step to next task entry
    bsf     FSR0L, 4                    ; Point back to tsk_xx_activ_prio
    bra     _parserInit                 ; Loop back for next task
                                        ; in task descriptor
_parser_organize
    GLOBAL  _parser_organize            ; If so requested externally
    movlb   0                           ; Prioritize the TCTs
    clrf    permut
    incf    permut      

;/* - REARRANGE TASK ORDER IN EACH TCT, BASED ON DESCENDING PRIORITY - */
_parserUpdate
    setupFSR0    tsk_1_activ_prio
    movf    permut, w                   ; Is there a bubble in the last
    btfsc   STATUS, Z                   ; iteration?
    bra     _isHook                     ; If No, means TCTs prioritized
    clrf    permut                      ; If Yes, sort still in progress
                                        ; Clear permutation flag for this
                                        ; iteration (Bubble sort is used)
_parserUpdateLoop 
    movlw   0x0F
    andwf   INDF0, w
    movwf   temp                        ; Extract task's priority
    btfsc   STATUS, Z                   ; 0 signifies the end of TCT
    bra     _parserUpdate               ; If end of task atble, loop back
                                        ; for next bubble sort iteration 
    movff   INDF0, temp2                ; Else backup (activ + prio) byte
    movlw   0x0F
    andwf   PREINC0, w                  ; Extract next task's priority

    cpfseq  temp
    bra     _parserUpdateDiff
    bra     _parserUpdateLoop           ; Equal priority, Loopback for
                                        ; the next control table entry
_parserUpdateDiff
    cpfsgt  temp
    bra     _parserUpdateSwap           ; Go swap on low priority
    bra     _parserUpdateLoop           ; Loopback if priority in place

_parserUpdateSwap
    movf    POSTDEC0, w                 ; Swap entry in tsk_xx_activ_prio
    movwf   POSTINC0                    ; located at 0x0B0
    movff   temp2, INDF0

    bcf     FSR0L, 4                    ; Swap entry in tsk_xx_state_ID
    swapTaskCtrlEntry                   ; located at 0x0A0

    bcf     FSR0L, 5                    ; Swap the task entry in event_xx
    swapTaskCtrlEntry                   ; located at 0x080

    bsf     FSR0L, 4                    ; Swap the task entry in wait_xx
    swapTaskCtrlEntry                   ; located at 0x090

    bsf     FSR0L, 5                    ; Point back to tsk_xx_activ_prio
    incf    permut                      ; Raise the permutation flag to
                                        ; signify a swap has occurred
    bra     _parserUpdateLoop 

_isHook                                 ; Hook kernel service routine
    #IFDEF  STARTUPHOOK
        call    StartupHook
    #ENDIF

;/**********************************************************************
; * --------------------- KERNEL TASK SCHEDULER ------------------------
; *
; * The scheduler manage the task switching during application running.
; *
; **********************************************************************/
_sched
    GLOBAL  _sched
    #IFDEF  POSTTASKHOOK
        call    PostTaskHook
    #ENDIF

    movlb   0
    bcf     kernelState, 0              ; Switch to KERNEL mode
    movf    id_tsk_run, w
    btfsc   STATUS, Z                   ; Is running task 0 or not?
    bra     _schedPickNext2Run          ; Branch if 0, no task RUNNING,
                                        ; Legal taskID: 1~15, 0 -> none
    setupFSR0    tsk_1_state_ID-1       ; Locate tsk_1_state_ID table

_schedFindRunningTaskEntry
    incf    FSR0L
    movlw   0x1F
    andwf   INDF0, w
    cpfseq  id_tsk_run                  ; Locate RUNNING task's entry
    bra     _schedFindRunningTaskEntry  ; Proceed to next task entry
    bcf     INDF0, 6                    ; Mark the task: no more RUNNING
    clrf    id_tsk_run                  ; Set running task ID to "none"

    bsf     FSR0L, 4                    ; Point to tsk_xx_activ_prio TCT
    movlw   0x0F                        ; Original code uses 0x0E, why?
    andwf   INDF0, w
    movwf   temp                        ; Extract & save task priority
                                        ; for later matching use
_testIfSamePriority
    movlw   0x0F                        ; Original code uses 0x0E, why?
    andwf   PREINC0, w
    cpfseq  temp                        ; Is it with the same priority?
    bra     _schedPickNext2Run          ; Branch to schedule if not
    bra     _RoundRobin4SamePriority    ; Else Proceed with Round Robin
                                        ; setup for the task
_RoundRobin4SamePriority
    swapTaskCtrlEntry                   ; Tasks having the same priority,
                                        ; Round Robin method used, and
                                        ; current task will be swapped
                                        ; all the way to the bottom of
                                        ; the tasks with same priority
    bcf     FSR0L, 4                    ; Swap tsk_xx_state_ID task entry
    swapTaskCtrlEntry
    bcf     FSR0L, 5                    ; Swap event_xx task entry
    swapTaskCtrlEntry
    bsf     FSR0L, 4                    ; Swap wait_xx task entry
    swapTaskCtrlEntry
    bsf     FSR0L, 5                    ; Point back to tsk_xx_activ_prio
    bra     _testIfSamePriority
    
_schedPickNext2Run
    setupFSR0    tsk_1_state_ID-1       ; Top down and hence prioritized,
                                        ; fetching next candiadte to run    
_schedTaskReadyOrNot
    incf    FSR0L
    btfss   INDF0, 5                    ; READY ?
    bra     _schedTestEndOfCtrlTable    ; Proceed to next if not READY
    btfsc   INDF0, 7                    ; WAITING?
    bra     _schedTestEndOfCtrlTable    ; It's READY, however WAITING
                                        ; for certain event
    movlb   0                           ; If READY and not WAITING then
    bsf     INDF0, 6                    ; Mark the task in RUNNING state
    movlw   0x1F
    andwf   INDF0, w
    movwf   id_tsk_run                  ; Designate the task in RUNNING

    #IFDEF  PRETASKHOOK                 ; Hook kernel service routine
        call    PreTaskHook
    #ENDIF
    
;/**********************************************************************
; * ------------------------- RESTORE CONTEXT --------------------------
; *
; * Copy the content of the context (task ram area) into Special File 
; * Registers.
; *
; **********************************************************************/
_restore_ctx
    GLOBAL  _restore_ctx
    
    movlb   0
    bsf     kernelState, 0              ; Change the kernel to USER mode
    locateTaskDescEntry
    locateStackAddrField   
    loadNextAddrTo FSR0L, FSR0H         ; Extract task's stack addr
    loadNextAddrTo startAddressL, startAddressH
                                        ; Extract task's code addr
; Go chech whether the stack overflow occurred
    goto    _checkPanic

; If the stack remains intact, restore the task's context
_restore_now
    GLOBAL  _restore_now
    movlb   0
    movff   POSTDEC1, temp
    movff   POSTDEC1, temp              ; Extract # of H/W stack entries
    clrf    STKPTR                      ; backed up previously
    
restoreNextHardwareStack
    push                                ; push to make room for restoring
    movf    POSTDEC1, w                 ; hardware stack entries
    movwf   TOSU
    movf    POSTDEC1, w
    movwf   TOSH
    movf    POSTDEC1, w
    movwf   TOSL                        ; Restore one H/W stack entry
    movf    STKPTR, w
    cpfseq  temp                        ; Is H/W stack restore done? 
    bra     restoreNextHardwareStack    ; Loopback to next restore If not

    setupFSR0    TEMP_SIZE-1            ; Load FSR0, to point to the end
                                        ; of .tmpdata + MATH_DATA section 
restoreNextTmpdataByte
    movff   POSTDEC1, POSTDEC0          ; Restore .tmpdata + MATH_DATA
    movf    FSR0L, w                    ; section
    btfss   STATUS, N
    bra     restoreNextTmpdataByte

    movff   POSTDEC1, PRODL             ; Restore the rest of SFRs saved
    movff   POSTDEC1, PRODH             ; in previously task swapping out
    movff   POSTDEC1, TABLAT
    movff   POSTDEC1, TBLPTRL
    movff   POSTDEC1, TBLPTRH
    movff   POSTDEC1, TBLPTRU
    movff   POSTDEC1, FSR0H
    movff   POSTDEC1, FSR0L
    movff   POSTDEC1, FSR2H
    movff   POSTDEC1, FSR2L
    movff   POSTDEC1, BSR
    movff   POSTDEC1, WREG
    movff   POSTDEC1, STATUS

    bsf     INTCON, 6                   ; Enable OS/low prior. interrupt
    retfie                              ; Exit to where TOS pointed at

immediat_ret
    movff   FSR0L, FSR1L                ; Here FSR0 seems to have task's
    movff   FSR0L, FSR2L                ; stack address + 2
    movff   FSR0H, FSR1H
    movff   FSR0H, FSR2H

    clrf    STKPTR                      ; Faking TOS with a pointer to
    incf    STKPTR                      ; task's start Address as if
    movf    startAddressL, w            ; there was a CALL right before
    movwf   TOSL                        ; task's start Address
    movf    startAddressH, w
    movwf   TOSH
    clrf    TOSU
    bsf     INTCON, 6                   ; Enable OS/low prior. interrupt
    retfie                              ; Branch to Task's code
    
_schedTestEndOfCtrlTable
    movlw   tsk_16_state_ID             ; Check for task ctrl table end
    subwf   FSR0L, w
    bnz     _schedTaskReadyOrNot

    movff   FSR1H_MAIN, FSR1H           ; No more READY to run, swing
    movff   FSR1L_MAIN, FSR1L           ; stack pointer back to point
                                        ; to main AP stack
_whileIdle
    movlb   0                           ; No task running
    bsf     INTCON, 6                   ; Enable OS/low prior. interrupt
    clrf    STKPTR                      ; waiting for any event to occur
    bra     _whileIdle

;/**********************************************************************
; * --------------------- CHECK STACK OVERFLOW -------------------------
; *
; **********************************************************************/
_checkPanic

    movff   FSR0L, stack_low            ; Save a copy of stack pointer
    movff   FSR0H, stack_high           ; for later recovery

; Check 0xadde signature/sentinel before the start of the task's stack
    ifSentinelDestroyGoto   _kernelPanicTrashedByNeighbor

    locateTaskDescEntry
    movff   stack_low, FSR0L
    movff   stack_high, FSR0H
    addStackSizeTo  FSR0L, FSR0H

; Check 0xadde signature/sentinel before the end of the task's stack
    ifSentinelDestroyGoto   _kernelPanicStackOverflow

    movff   stack_low, FSR0L            ; Restore FSR0 to task's 
    movff   stack_high, FSR0H           ; stack address
    movff   POSTINC0, FSR1L             ; Popup from stack top,
    movff   POSTINC0, FSR1H             ; and setup FSR1 to point to
                                        ; where context were saved
    movf    FSR1L, w                    ; Non-null means there is a
    btfss   STATUS, Z                   ; previous saved context to
    bra     _restore_now                ; be restored
    movf    FSR1H, w
    btfsc   STATUS, Z
    bra     immediat_ret                ; Return immediately if FSR1==0
    bra     _restore_now                ; else restore the context

_kernelPanicTrashedByNeighbor
    markRunningTaskSuspended            ; Chances are stack overflow of
    movff   kernelPanic, WREG           ; another task is jeopardizing
    andlw   0xF0                        ; current task
    addwf   id_tsk_run, w
    movff   WREG, kernelPanic           ; Log the task in right nibble
    bra     _sched                      ; of the kernelpanic

_kernelPanicStackOverflow
    markRunningTaskSuspended            ; Stack overflow found
    #IFDEF  RESET_ON_PANIC              ; Reset in case of KERNEL PANIC
        reset                           ; if so requested
    #ENDIF
    movff   kernelPanic, WREG
    swapf   WREG, w
    andlw   0xF0
    addwf   id_tsk_run, w
    swapf   WREG, w
    movff   WREG, kernelPanic           ; Log the task ID in left nibble
    bra     _sched                      ; of the kernelpanic

;/**********************************************************************
; * ----------------------- FOR BASIC TASK ONLY ------------------------
; *
; * After terminating a task, we loose the start address of the task, 
; * first located in the first place of the stack.
; *
; **********************************************************************/
_restore_start_adr
    GLOBAL  _restore_start_adr
    locateTaskDescEntry
    locateStackAddrField   
    loadNextAddrTo FSR0L, FSR0H

    clrf    WREG
    movwf   POSTINC0
    movwf   INDF0                       ; Set the value into STKPTR
    goto    _sched                      ; Determine the next RUNNING task

    END                                 ; Directive 'end of program'

;/* End of File : kernel.asm */

