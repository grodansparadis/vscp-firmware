///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:36 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\delay.c    /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\delay.c    /
//                    -lC D:\development\gediminas\CAN4USB\project\RELEASE\Li /
//                    st\ -lA D:\development\gediminas\CAN4USB\project\RELEAS /
//                    E\List\ -o D:\development\gediminas\CAN4USB\project\REL /
//                    EASE\Obj\ --no_cse --no_unroll --no_inline              /
//                    --no_code_motion --no_tbaa --no_clustering              /
//                    --no_scheduling --debug --endian little --cpu           /
//                    ARM7TDMI-S -e --fpu None --dlib_config "D:\Program\IAR  /
//                    Systems\Embedded Workbench                              /
//                    5.0\ARM\INC\DLib_Config_Normal.h" -I                    /
//                    D:\development\gediminas\CAN4USB\project\ -I            /
//                    D:\development\gediminas\CAN4USB\project\app\ -I        /
//                    D:\development\gediminas\CAN4USB\project\board\ -I      /
//                    D:\development\gediminas\CAN4USB\project\module\ -I     /
//                    D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\inc\ -I j:\common\ -I "D:\Program\IAR           /
//                    Systems\Embedded Workbench 5.0\ARM\INC\" --interwork    /
//                    --cpu_mode thumb -On                                    /
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\d /
//                    elay.s                                                  /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME delay

        EXTERN __aeabi_d2uiz
        EXTERN __aeabi_dadd
        EXTERN __aeabi_dmul
        EXTERN __aeabi_ui2d
        EXTERN pause

        PUBLIC delay_ms
        PUBLIC delay_us_
        
        CFI Names cfiNames0
        CFI StackFrame CFA R13 DATA
        CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
        CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
        CFI EndNames cfiNames0
        
        CFI Common cfiCommon0 Using cfiNames0
        CFI CodeAlign 2
        CFI DataAlign 4
        CFI ReturnAddress R14 CODE
        CFI CFA R13+0
        CFI R0 Undefined
        CFI R1 Undefined
        CFI R2 Undefined
        CFI R3 Undefined
        CFI R4 SameValue
        CFI R5 SameValue
        CFI R6 SameValue
        CFI R7 SameValue
        CFI R8 SameValue
        CFI R9 SameValue
        CFI R10 SameValue
        CFI R11 SameValue
        CFI R12 Undefined
        CFI R14 SameValue
        CFI EndCommon cfiCommon0
        
// D:\development\gediminas\CAN4USB\project\app\delay.c
//    1 /*
//    2    Precise Delay Functions for LPC2100
//    3    Martin Thomas, 11/2004
//    4 */
//    5 
//    6 #include "systime.h"
//    7 #include "delay.h"
//    8 
//    9 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function delay_us_
        THUMB
//   10 void delay_us_( unsigned long delayval )
//   11 {
//   12   // Not implemented yet
//   13 }
delay_us_:
        BX       LR               ;; return
        CFI EndBlock cfiBlock0
//   14 
//   15 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function delay_ms
        THUMB
//   16 void delay_ms( unsigned long delayval )
//   17 {
delay_ms:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//   18   unsigned long d = (unsigned long)( (delayval * 1e-3 * sysTICSperSEC ) + .5 );
        MOVS     R0,R4
        BL       __aeabi_ui2d
        MOVS     R2,R0
        MOVS     R3,R1
        LDR      R0,??delay_ms_0  ;; 0xd2f1a9fc
        LDR      R1,??delay_ms_0+0x4  ;; 0x3f50624d
        BL       __aeabi_dmul
        MOVS     R2,R0
        MOVS     R3,R1
        MOVS     R0,#+0
        LDR      R1,??delay_ms_0+0x8  ;; 0x408f4000
        BL       __aeabi_dmul
        MOVS     R2,R0
        MOVS     R3,R1
        MOVS     R0,#+0
        LDR      R1,??delay_ms_0+0xC  ;; 0x3fe00000
        BL       __aeabi_dadd
        BL       __aeabi_d2uiz
        MOVS     R5,R0
//   19   pause( d );
        MOVS     R0,R5
        BL       pause
//   20 }
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??delay_ms_0:
        DC32     0xd2f1a9fc
        DC32     0x3f50624d
        DC32     0x408f4000
        DC32     0x3fe00000
        CFI EndBlock cfiBlock1

        END
// 
// 82 bytes in section .text
// 
// 82 bytes of CODE memory
//
//Errors: none
//Warnings: none
