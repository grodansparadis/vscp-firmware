///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:40 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\sysTime75x /
//                    .c                                                      /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\sysTime75x /
//                    .c -lC D:\development\gediminas\CAN4USB\project\RELEASE /
//                    \List\ -lA D:\development\gediminas\CAN4USB\project\REL /
//                    EASE\List\ -o D:\development\gediminas\CAN4USB\project\ /
//                    RELEASE\Obj\ --no_cse --no_unroll --no_inline           /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\s /
//                    ysTime75x.s                                             /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME sysTime75x

        RTMODEL "__dlib_file_descriptor", "0"

        EXTERN EIC_IRQInit
        EXTERN MRCC_PeripheralClockConfig
        EXTERN TB_Cmd
        EXTERN TB_ITConfig
        EXTERN TB_Init

        PUBLIC getElapsedSysTICs
        PUBLIC getSysTICs
        PUBLIC initTimebase
        PUBLIC pause
        PUBLIC sysTICs
        
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
        
// D:\development\gediminas\CAN4USB\project\app\sysTime75x.c
//    1 /******************************************************************************
//    2  *
//    3  * $RCSfile: sysTime.c,v $
//    4  * $Revision: 1.4 $
//    5  *
//    6  * This module provides the interface routines for initializing and
//    7  * accessing the system timing functions.
//    8  * Copyright 2004, R O SoftWare
//    9  * No guarantees, warrantees, or promises, implied or otherwise.
//   10  * May be used for hobby or commercial purposes provided copyright
//   11  * notice remains intact.
//   12  *
//   13  *****************************************************************************/
//   14 
//   15 //
//   16 // The TB2_IRQHandler handler in vectors.c  must be implemented fot this to work
//   17 // This rotine should update the sysTICs value once each millisecond.
//   18 // Something like this is needed
//   19 //
//   20 // 	sysTICs++;
//   21 //	TB_FlagClear( TB2 );
//   22 //
//   23 
//   24 #include "includes.h"
//   25 #include "75x_conf.h"
//   26 #include "75x_lib.h"
//   27 #include "sysTime.h"
//   28 

        SECTION `.bss`:DATA:NOROOT(2)
//   29 volatile Int32U sysTICs;
sysTICs:
        DS8 4
//   30 
//   31 /******************************************************************************
//   32  *
//   33  * Function Name: initTimebase()
//   34  *
//   35  * Description:
//   36  *    This function initializes the TB2 for use as the system timer.
//   37  *
//   38  * Calling Sequence:
//   39  *    void
//   40  *
//   41  * Returns:
//   42  *    void
//   43  *
//   44  *****************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function initTimebase
        THUMB
//   45 void initTimebase( void )
//   46 {
initTimebase:
        PUSH     {LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+4
        SUB      SP,SP,#+20
        CFI CFA R13+24
//   47   TB_InitTypeDef    TB_InitStructure;
//   48   EIC_IRQInitTypeDef EIC_IRQInitStructure;
//   49 
//   50   sysTICs = 0;
        LDR      R0,??DataTable1  ;; sysTICs
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//   51 
//   52   MRCC_PeripheralClockConfig( MRCC_Peripheral_TB, ENABLE );
        MOVS     R1,#+1
        MOVS     R0,#+2
        BL       MRCC_PeripheralClockConfig
//   53 
//   54 	// TB2 Configuration to get timbase IRQ each 1ms
//   55   TB_InitStructure.TB_ClockSource = TB_ClockSource_CKTIM;
        ADD      R0,SP,#+4
        MOVS     R1,#+1
        STRH     R1,[R0, #+2]
//   56   TB_InitStructure.TB_Prescaler = 100;             // The internal TB Frequency = 160KHz
        ADD      R0,SP,#+4
        MOVS     R1,#+100
        STRH     R1,[R0, #+8]
//   57   TB_InitStructure.TB_AutoReload = 160;           // 160000Hz * 0.001 s  = 160
        ADD      R0,SP,#+4
        MOVS     R1,#+160
        STRH     R1,[R0, #+10]
//   58 
//   59   TB_Init( &TB_InitStructure );
        ADD      R0,SP,#+4
        BL       TB_Init
//   60   TB_ITConfig( TB_IT_Update, ENABLE );
        MOVS     R1,#+1
        MOVS     R0,#+1
        BL       TB_ITConfig
//   61 	
//   62   //EIC_IRQChannelConfig( TB_IRQChannel, ENABLE );  // Enable IRQ
//   63   
//   64   // Enable and configure the priority of the TIM0 Output compare IRQ Channel
//   65   EIC_IRQInitStructure.EIC_IRQChannel = TB_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+31
        STRB     R1,[R0, #+0]
//   66   EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+1]
//   67   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//   68   EIC_IRQInit( &EIC_IRQInitStructure );
        MOV      R0,SP
        BL       EIC_IRQInit
//   69 	
//   70   TB_Cmd( ENABLE );													// Enable the timer
        MOVS     R0,#+1
        BL       TB_Cmd
//   71 }
        ADD      SP,SP,#+20
        CFI CFA R13+4
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock0
//   72 
//   73 /******************************************************************************
//   74  *
//   75  * Function Name: getSysTICs()
//   76  *
//   77  * Description:
//   78  *    This function returns the current syetem time in TICs.
//   79  *
//   80  * Calling Sequence:
//   81  *    void
//   82  *
//   83  * Returns:
//   84  *    The current time in TICs as represented by sysTICs
//   85  *
//   86  *****************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function getSysTICs
        THUMB
//   87 u32 getSysTICs( void )
//   88 {
//   89   return sysTICs;
getSysTICs:
        LDR      R0,??DataTable1  ;; sysTICs
        LDR      R0,[R0, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock1
//   90 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable1:
        DC32     sysTICs
//   91 
//   92 
//   93 /******************************************************************************
//   94  *
//   95  * Function Name: getElapsedSysTICs()
//   96  *
//   97  * Description:
//   98  *    This function then returns the difference in TICs between the
//   99  *    given starting time and the current system time.
//  100  *
//  101  * Calling Sequence:
//  102  *    The starting time.
//  103  *
//  104  * Returns:
//  105  *    The time difference.
//  106  *
//  107  *****************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function getElapsedSysTICs
        THUMB
//  108 u32 getElapsedSysTICs( u32 startTime )
//  109 {
getElapsedSysTICs:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  110   return ( getSysTICs() - startTime );
        BL       getSysTICs
        SUBS     R0,R0,R4
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock2
//  111 }
//  112 
//  113 
//  114 /******************************************************************************
//  115  *
//  116  * Function Name: pause()
//  117  *
//  118  * Description:
//  119  *    This function does not return until the specified 'duration' in
//  120  *    TICs has elapsed.
//  121  *
//  122  * Calling Sequence:
//  123  *    duration - length of time in TICs to wait before returning
//  124  *
//  125  * Returns:
//  126  *    void
//  127  *
//  128  *****************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function pause
        THUMB
//  129 void pause( u32 duration )
//  130 {
pause:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  131   u32 startTime = getSysTICs();
        BL       getSysTICs
        MOVS     R5,R0
//  132 
//  133   while ( getElapsedSysTICs( startTime ) < duration ) ;
??pause_0:
        MOVS     R0,R5
        BL       getElapsedSysTICs
        CMP      R0,R4
        BCC      ??pause_0
//  134 	
//  135 }
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock3

        END
//  136 
//  137 
// 
//   4 bytes in section .bss
// 138 bytes in section .text
// 
// 138 bytes of CODE memory
//   4 bytes of DATA memory
//
//Errors: none
//Warnings: none
