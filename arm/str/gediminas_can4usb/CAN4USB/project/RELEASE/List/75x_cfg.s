///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:17 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\Program\IAR Systems\Embedded Workbench               /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_cfg.c                                                 /
//    Command line =  "D:\Program\IAR Systems\Embedded Workbench              /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_cfg.c" -lC D:\development\gediminas\CAN4USB\project\R /
//                    ELEASE\List\ -lA D:\development\gediminas\CAN4USB\proje /
//                    ct\RELEASE\List\ -o D:\development\gediminas\CAN4USB\pr /
//                    oject\RELEASE\Obj\ --no_cse --no_unroll --no_inline     /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\7 /
//                    5x_cfg.s                                                /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_cfg`

        PUBLIC CFG_BootSpaceConfig
        PUBLIC CFG_FLASHBurstConfig
        PUBLIC CFG_GetFlagStatus
        PUBLIC CFG_USBFilterConfig
        
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
        
// D:\Program\IAR Systems\Embedded Workbench 5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75x_cfg.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_cfg.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the CFG software functions.
//    6 ********************************************************************************
//    7 * History:
//    8 * 07/17/2006 : V1.0
//    9 * 03/10/2006 : V0.1
//   10 ********************************************************************************
//   11 * THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
//   12 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
//   13 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
//   14 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
//   15 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
//   16 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
//   17 *******************************************************************************/
//   18 
//   19 /* Includes ------------------------------------------------------------------*/
//   20 #include "75x_cfg.h"
//   21 
//   22 /* Private typedef -----------------------------------------------------------*/
//   23 /* Private define ------------------------------------------------------------*/
//   24 #define CFG_SWBOOT_Mask	      0xFFFFFFFC
//   25 #define CFG_FLASHBusy_Mask    0x00000080
//   26 
//   27 /* Private macro -------------------------------------------------------------*/
//   28 /* Private variables ---------------------------------------------------------*/
//   29 /* Private function prototypes -----------------------------------------------*/
//   30 /* Private functions ---------------------------------------------------------*/
//   31 
//   32 /*******************************************************************************
//   33 * Function Name  : CFG_BootSpaceConfig
//   34 * Description    : Selects which memory space will be remapped at address 0x00.
//   35 * Input          : - CFG_BootSpace: specifies the memory space to be remapped
//   36 *                    at address 0x00.
//   37 *                    This parameter can be one of the following values:
//   38 *                          - CFG_BootSpace_FLASH
//   39 *                          - CFG_BootSpace_SRAM
//   40 *                          - CFG_BootSpace_ExtSMI
//   41 * Output         : None
//   42 * Return         : None
//   43 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function CFG_BootSpaceConfig
        THUMB
//   44 void CFG_BootSpaceConfig(u32 CFG_BootSpace)
//   45 {
//   46   u32 Temp = 0;
CFG_BootSpaceConfig:
        MOVS     R2,#+0
        MOVS     R1,R2
//   47   
//   48   /* Clear SW_BOOT[1:0] bits */ 
//   49   Temp = CFG->GLCONF & CFG_SWBOOT_Mask;
        LDR      R2,??DataTable10  ;; 0x60000010
        LDR      R2,[R2, #+0]
        MOVS     R3,#+3
        BICS     R2,R2,R3
        MOVS     R1,R2
//   50   
//   51   /* Set SW_BOOT[1:0] bits according to CFG_BootSpace parameter value */ 
//   52   Temp |= CFG_BootSpace;
        ORRS     R1,R1,R0
//   53   
//   54   /* Store the new value */ 
//   55   CFG->GLCONF = Temp;   
        LDR      R2,??DataTable10  ;; 0x60000010
        STR      R1,[R2, #+0]
//   56 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock0
//   57 
//   58 /*******************************************************************************
//   59 * Function Name  : CFG_FLASHBurstConfig
//   60 * Description    : Enables or disables the FLASH Burst mode.
//   61 * Input          : - CCFG_FLASHBurst: specifies the new state of the FLASH Burst
//   62 *                    mode.
//   63 *                    This parameter can be one of the following values:
//   64 *                          - CFG_FLASHBurst_Disable
//   65 *                          - CFG_FLASHBurst_Enable
//   66 * Output         : None
//   67 * Return         : None
//   68 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function CFG_FLASHBurstConfig
        THUMB
//   69 void CFG_FLASHBurstConfig(u32 CFG_FLASHBurst)
//   70 {
CFG_FLASHBurstConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   71   if(CFG_FLASHBurst == CFG_FLASHBurst_Enable)
        MOVS     R1,#+128
        LSLS     R1,R1,#+1        ;; #+256
        CMP      R0,R1
        BNE      ??CFG_FLASHBurstConfig_0
//   72   {
//   73     CFG->GLCONF |= CFG_FLASHBurst_Enable;
        LDR      R1,??DataTable10  ;; 0x60000010
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+1        ;; #+256
        ORRS     R2,R2,R1
        LDR      R1,??DataTable10  ;; 0x60000010
        STR      R2,[R1, #+0]
        B        ??CFG_FLASHBurstConfig_1
//   74   }
//   75   else
//   76   {
//   77     CFG->GLCONF &= CFG_FLASHBurst_Disable;
??CFG_FLASHBurstConfig_0:
        LDR      R1,??DataTable10  ;; 0x60000010
        LDR      R1,[R1, #+0]
        LDR      R2,??CFG_FLASHBurstConfig_2  ;; 0xfffffeff
        ANDS     R2,R2,R1
        LDR      R1,??DataTable10  ;; 0x60000010
        STR      R2,[R1, #+0]
//   78   }
//   79 }
??CFG_FLASHBurstConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??CFG_FLASHBurstConfig_2:
        DC32     0xfffffeff
        CFI EndBlock cfiBlock1
//   80 
//   81 /*******************************************************************************
//   82 * Function Name  : CFG_USBFilterConfig
//   83 * Description    : Enables or disables the USB Filter.
//   84 * Input          : - CFG_USBFilter: specifies the new state of the USB Filter.
//   85 *                    This parameter can be one of the following values:
//   86 *                          - CFG_USBFilter_Disable
//   87 *                          - CFG_USBFilter_Enable
//   88 * Output         : None
//   89 * Return         : None
//   90 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function CFG_USBFilterConfig
        THUMB
//   91 void CFG_USBFilterConfig(u32 CFG_USBFilter)
//   92 {
CFG_USBFilterConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   93   if(CFG_USBFilter == CFG_USBFilter_Enable)
        MOVS     R1,#+128
        LSLS     R1,R1,#+2        ;; #+512
        CMP      R0,R1
        BNE      ??CFG_USBFilterConfig_0
//   94   {
//   95     CFG->GLCONF |= CFG_USBFilter_Enable;
        LDR      R1,??DataTable10  ;; 0x60000010
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+2        ;; #+512
        ORRS     R2,R2,R1
        LDR      R1,??DataTable10  ;; 0x60000010
        STR      R2,[R1, #+0]
        B        ??CFG_USBFilterConfig_1
//   96   }
//   97   else
//   98   {
//   99     CFG->GLCONF &= CFG_USBFilter_Disable;
??CFG_USBFilterConfig_0:
        LDR      R1,??DataTable10  ;; 0x60000010
        LDR      R1,[R1, #+0]
        LDR      R2,??CFG_USBFilterConfig_2  ;; 0xfffffdff
        ANDS     R2,R2,R1
        LDR      R1,??DataTable10  ;; 0x60000010
        STR      R2,[R1, #+0]
//  100   }
//  101 }
??CFG_USBFilterConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??CFG_USBFilterConfig_2:
        DC32     0xfffffdff
        CFI EndBlock cfiBlock2
//  102 
//  103 /*******************************************************************************
//  104 * Function Name  : CFG_GetFlagStatus
//  105 * Description    : Checks whether the FLASH Busy flag is set or not.
//  106 * Input          : None
//  107 * Output         : None
//  108 * Return         : The new state of FLASH Busy flag (SET or RESET).
//  109 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function CFG_GetFlagStatus
        THUMB
//  110 FlagStatus CFG_GetFlagStatus(void)
//  111 {
//  112   if((CFG->GLCONF & CFG_FLASHBusy_Mask) != RESET)
CFG_GetFlagStatus:
        LDR      R0,??DataTable10  ;; 0x60000010
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  113   {
//  114     return SET;
//  115   }
//  116   else
//  117   {
//  118     return RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//  119   }
//  120 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable10:
        DC32     0x60000010

        END
//  121 
//  122 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 144 bytes in section .text
// 
// 144 bytes of CODE memory
//
//Errors: none
//Warnings: none
