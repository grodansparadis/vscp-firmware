; Issue: 0.02/25-Nov-91
;
; Purpose: Minimal, standalone, C-library kernel.
;
; Copyright (C) 1991 Advanced RISC Machines Limited. All rights reserved.
;
; Advanced RISC Machines Limited does not assume any liability arising out
; of this program or use thereof neither does it convey any licence under
; its intellectual property rights.
;
; Conditions of use:
;
; The terms and conditions under which this software is supplied to you and
; under which you may use it are described in your licence agreement with
; your supplier.
;
;----------------------------------------------------------------------------;
; ABOUT THIS CODE                                                            ;
;                                                                            ;
; This code shows you how to write your own minimal, standalone, run-time    ;
; support system for code compiled by Advanced RISC Machines's C Compiler.   ;
; It can be assembled using Advanced RISC Machines's ARM assembler (ARMasm)  ;
; or any assembler comaptible with it.                                       ;
;                                                                            ;
; This example code has been written to run under Advanced RISC Machines's   ;
; ARM emulation system (ARMulator). It can also run without modification     ;
; under Acorm Computer's "RISC OS" operating system for its ARM-based        ;
; personal workstations.                                                     ;
;                                                                            ;
; In fact, this code depends hardly at all on its target environment and is  ;
; designed to be very easy to adapt to your particular ARM-based system.     ;
; You can expect it to take about a couple of hours to re-target.            ;
;                                                                            ;
; Much of the code below is generic to the ARM processor and is completely   ;
; independent of your ARM-based hardware or any operating system kernel that ;
; may run on it. To get going, you need write only 4 simple fns.             ;
;                                                                            ;
; WHAT THIS CODE PROVIDES:                                                   ;
;                                                                            ;
;  - Divide (and remainder) functions.                                       ;
;                                                                            ;
; WHAT THIS CODE DOES NOT PROVIDE                                            ;
;                                                                            ;
; Support for handling traps, faults, escapes, exceptions or interrupts.     ;
;                                                                            ;
;----------------------------------------------------------------------------;


;----------------------------------------------------------------------------;
; And, finally, generic ARM functions, referred to by the C compiler.        ;
; You should not need to alter any of these unless you wish to incorporate   ;
; them in your operating system kernel. See also later comments.             ;
;----------------------------------------------------------------------------;

	INCLUDE	platform.s
	GET	retmacro.s			; Thumb/ARM interwork

        IMPORT  PutString
        IMPORT  |__rt_exit|
        IMPORT  |__err_handler|, WEAK

 IF USE_C_LIBRARY = 0
	     EXPORT |__16__rt_div0|
   	 EXPORT |__16__rt_udiv|
      	 EXPORT |__16__rt_udiv10|
      	 EXPORT |__16__rt_sdiv|
        EXPORT |__16__rt_sdiv10|

	     EXPORT |__rt_div0|
        EXPORT |__rt_udiv|
        EXPORT |__rt_udiv10|
        EXPORT |__rt_sdiv|
        EXPORT |__rt_sdiv10|
        EXPORT |__rt_divtest|

;----------------------------------------------------------------------------;
; If __err_handler exists, errors are passed to it; otherwise, we print a    ;
; simple diagnostic message and exit.                                        ;
;----------------------------------------------------------------------------;


        AREA    |C$$code|, CODE, READONLY  ; This module's code area.        ;

;----------------------------------------------------------------------------;
; This code has to run in but 26-bit ARM modes and 32-bit modes. To allow    ;
; for this, the code is carefully written so that all PSR restoration in     ;
; 26-bit mode is via the following macro.                                    ;
;----------------------------------------------------------------------------;

        MACRO
        RET     $cond
    IF      {CONFIG} = 26
        MOV$cond.S    pc, lr
    ELSE

	; just return back to boot up code
	RETURN_COND	$cond


        MOV$cond      pc, lr
    ENDIF
        MEND

;----------------------------------------------------------------------------;
; Support for low-level failures - currently stack overflow, divide by 0 and ;
; floating-point exceptions. If there is a higher level handler, call it;    ;
; otherwise, print a message and exit gracefully.                            ;
;                                                                            ;
; NOTES                                                                      ;
;                                                                            ;
; typedef struct { unsigned code; char message[252];} __rt_error;            ;
; typedef struct { unsigned r[16];} __rt_registers;                          ;
;                                                                            ;
;----------------------------------------------------------------------------;

|__rt_trap|
;
; void __rt_trap(__rt_error *e, __rt_registers *r);

        STMFD   sp!, {a1}             ; save e in case handler returns...
        LDR     ip, err_handler
        CMP     ip, #0
        MOVNE   lr, pc
    IF      {CONFIG} = 26
        MOVNES  pc, ip                ; if got a handler, use it and
    ELSE
        MOVNE   pc, ip                ; if got a handler, use it and
    ENDIF
        LDMFD   sp!, {v1}             ; hope not to return...

        ADR     a1, RTErrorHead
        BL      PutString             ; write preamble...
        ADD     a1, v1, #4
        BL      PutString             ; write error diagnosis
        ADR     a1, RTErrorTail
        BL      PutString             ; write postlude
        MOV     a1, #255
        B       |__rt_exit|           ; and terminate with non-zero exit code
err_handler
        DCD     |__err_handler|


save_regs_and_trap
        STMFD   sp!, {sp, lr, pc}
        STMFD   sp!, {r0-r12}
        STR     lr, [sp, #4*15]       ; caller's pc is my lr
        MOV     a2, sp
        MOV     a1, ip
        B       |__rt_trap|

dividebyzero
        ADR     ip, DivideByZeroError
        B       save_regs_and_trap

stackoverflow
        ADR     ip, StackOverflowError
        B       save_regs_and_trap

DivideByZeroError
        DCD     1
        DCB     "divide by 0", 0
        ALIGN

StackOverflowError
        DCD     3
        DCB     "stack overflow", 0
        ALIGN

RTErrorHead
        DCB     10, 13, "run time error: ", 0

RTErrorTail
        DCB     10, 13, "program terminated", 10, 13, 10, 13, 0

        ALIGN


;----------------------------------------------------------------------------;
; GENERIC ARM FUNCTIONS - divide and remainder.                              ;
;                                                                            ;
; NOTES                                                                      ;
;                                                                            ;
; 1/ You may wish to make these functions part of your O/S kernel, replacing ;
;    the implementations here by branches to the relevant entry addresses.   ;
;                                                                            ;
; 2/ Each divide function is a div-rem function, returning the quotient in   ;
;    r0 and the remainder in r1. Thus (r0, r1) -> (r0/r1, r0%r1). This is    ;
;    understood by the C compiler.                                           ;
;                                                                            ;
; 3/ Because of its importance in many applications, divide by 10 is treated ;
;    as a special case. The C compiler recognises divide by 10 and generates ;
;    calls to __rt_{u,s}div10, as appropriate.                               ;
;                                                                            ;
; 4/ In each of these implementations we make a careful compromise between   ;
;    fastest and smallest. In some cases (e.g. divide), further unrolling of ;
;    loops will yield a speed-up. However, this affects cache and working-   ;
;    set locality and can result in a slow-down for some applications on     ;
;    some hardware. You are advised not to unroll further before considering ;
;    carefully your application and its interaction with your hardware.      ;
;    (e.g. ARM-2 vs ARM-3, external cache vs no external cache, fraction of  ;
;     run-time spent executing divide, etc.).                                ;
;                                                                            ;
;----------------------------------------------------------------------------;

|__16__rt_udiv|
|__rt_udiv|                                                           ; udiv ;
;
; Unsigned divide of a2 by a1: returns quotient in a1, remainder in a2.
; Also destroys a3, a4 and ip

        MOVS    a3, a1
        BEQ     dividebyzero
        MOV     a4, #0
        MOV     ip, #&80000000
        CMP     a2, ip
        MOVLO   ip, a2
u_loop
        CMP     ip, a3, ASL #0
        BLS     u_shifted0mod8
        CMP     ip, a3, ASL #1
        BLS     u_shifted1mod8
        CMP     ip, a3, ASL #2
        BLS     u_shifted2mod8
        CMP     ip, a3, ASL #3
        BLS     u_shifted3mod8
        CMP     ip, a3, ASL #4
        BLS     u_shifted4mod8
        CMP     ip, a3, ASL #5
        BLS     u_shifted5mod8
        CMP     ip, a3, ASL #6
        BLS     u_shifted6mod8
        CMP     ip, a3, ASL #7
        MOVHI   a3, a3, ASL #8
        BHI     u_loop
u_loop2
u_shifted7mod8
        CMP     a2, a3, ASL #7
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #7
u_shifted6mod8
        CMP     a2, a3, ASL #6
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #6
u_shifted5mod8
        CMP     a2, a3, ASL #5
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #5
u_shifted4mod8
        CMP     a2, a3, ASL #4
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #4
u_shifted3mod8
        CMP     a2, a3, ASL #3
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #3
u_shifted2mod8
        CMP     a2, a3, ASL #2
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #2
u_shifted1mod8
        CMP     a2, a3, ASL #1
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #1
u_shifted0mod8
        CMP     a2, a3, ASL #0
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #0
        CMP     a1, a3, LSR #1
        MOVLS   a3, a3, LSR #8
        BLS     u_loop2
        MOV     a1, a4
        RET

|__16__rt_udiv10|
|__rt_udiv10|                                                       ; udiv10 ;
;
; Fast unsigned divide by 10: dividend in a1, divisor in a2.
; Returns quotient in a1, remainder in a2.
; Also destroys a3.
;
; Calculate x / 10 as (x * 2**32/10) / 2**32.
; That is, we calculate the most significant word of the double-length
; product. In fact, we calculate an approximation which may be 1 off
; because we've ignored a carry from the least significant word we didn't
; calculate. We correct for this by insisting that the remainder < 10
; and by incrementing the quotient if it isn't.

        MOV     a2, a1
        MOV     a1, a1, LSR #1
        ADD     a1, a1, a1, LSR #1
        ADD     a1, a1, a1, LSR #4
        ADD     a1, a1, a1, LSR #8
        ADD     a1, a1, a1, LSR #16
        MOV     a1, a1, LSR #3
        ADD     a3, a1, a1, ASL #2
        SUB     a2, a2, a3, ASL #1
        CMP     a2, #10
        ADDGE   a1, a1, #1
        SUBGE   a2, a2, #10
        RET

|__16__rt_sdiv|
|__rt_sdiv|                                                           ; sdiv ;
;
; Signed divide of a2 by a1: returns quotient in a1, remainder in a2
; Quotient is truncated (rounded towards zero).
; Sign of remainder = sign of dividend.
; Also destroys a3, a4 and ip.
; Makes dividend and divisor non-negative, then does an unsigned divide;
; the proper signs get sorted out again at the end.
; The code is mostly as for udiv, except that the justification part is
; slightly simplified by knowledge that the dividend is in the range
; [0..#x80000000]. One register may be gained thereby.

        MOVS    ip, a1
        BEQ     dividebyzero
        RSBMI   a1, a1, #0              ; absolute value of divisor
        EOR     ip, ip, a2
        ANDS    a4, a2, #&80000000
        ORR     ip, a4, ip, LSR #1
        ; ip bit 31  sign of dividend (= sign of remainder)
        ;    bit 30  sign of dividend EOR sign of divisor (= sign of quotient)
        RSBNE   a2, a2, #0              ; absolute value of dividend

        MOV     a3, a1
        MOV     a4, #0
s_loop
        CMP     a2, a3, ASL #0
        BLS     s_shifted0mod8
        CMP     a2, a3, ASL #1
        BLS     s_shifted1mod8
        CMP     a2, a3, ASL #2
        BLS     s_shifted2mod8
        CMP     a2, a3, ASL #3
        BLS     s_shifted3mod8
        CMP     a2, a3, ASL #4
        BLS     s_shifted4mod8
        CMP     a2, a3, ASL #5
        BLS     s_shifted5mod8
        CMP     a2, a3, ASL #6
        BLS     s_shifted6mod8
        CMP     a2, a3, ASL #7
        MOVHI   a3, a3, ASL #8
        BHI     s_loop
s_loop2
        CMP     a2, a3, ASL #7
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #7
        CMP     a2, a3, ASL #6
s_shifted6mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #6
        CMP     a2, a3, ASL #5
s_shifted5mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #5
        CMP     a2, a3, ASL #4
s_shifted4mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #4
        CMP     a2, a3, ASL #3
s_shifted3mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #3
        CMP     a2, a3, ASL #2
s_shifted2mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #2
        CMP     a2, a3, ASL #1
s_shifted1mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #1
        CMP     a2, a3, ASL #0
s_shifted0mod8
        ADC     a4, a4, a4
        SUBHS   a2, a2, a3, ASL #0
        CMP     a1, a3, LSR #1
        MOVLS   a3, a3, LSR #8
        BLS     s_loop2
        MOV     a1, a4
        TST     ip, #&40000000
        RSBNE   a1, a1, #0
        TST     ip, #&80000000
        RSBNE   a2, a2, #0
        RET

|__16__rt_sdiv10|
|__rt_sdiv10|                                                       ; sdiv10 ;
;
; Fast signed divide by 10: dividend in a1, divisor in a2.
; Returns quotient in a1, remainder in a2.
; Also destroys a3 and a4.
; Quotient is truncated (rounded towards zero).

        MOVS    a4, a1
        RSBMI   a1, a1, #0
        MOV     a2, a1
        MOV     a1, a1, LSR #1
        ADD     a1, a1, a1, LSR #1
        ADD     a1, a1, a1, LSR #4
        ADD     a1, a1, a1, LSR #8
        ADD     a1, a1, a1, LSR #16
        MOV     a1, a1, LSR #3
        ADD     a3, a1, a1, ASL #2
        SUB     a2, a2, a3, ASL #1
        CMP     a2, #10
        ADDGE   a1, a1, #1
        SUBGE   a2, a2, #10
        CMP     a4, #0
        RSBMI   a1, a1, #0
        RSBMI   a2, a2, #0
        RET

|__16__rt_div0|
|__rt_div0|
|__rt_divtest|                                                     ; divtest ;
;
; Test for division by zero (used when division is voided).
        CMPS    a1, #0
        RET     NE
        B       dividebyzero

 ELSE
	; Export something to stop librarian from issuing warning
	EXPORT |__16__rt_dummy|

        AREA    |C$$code|, CODE, READONLY  ; This module's code area.        ;

|__16__rt_dummy|

 ENDIF
        END
