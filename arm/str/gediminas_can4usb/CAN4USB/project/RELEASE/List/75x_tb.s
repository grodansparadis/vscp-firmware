///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:24 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\src\75x_tb.c                                    /
//    Command line =  D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\src\75x_tb.c -lC D:\development\gediminas\CAN4U /
//                    SB\project\RELEASE\List\ -lA                            /
//                    D:\development\gediminas\CAN4USB\project\RELEASE\List\  /
//                    -o D:\development\gediminas\CAN4USB\project\RELEASE\Obj /
//                    \ --no_cse --no_unroll --no_inline --no_code_motion     /
//                    --no_tbaa --no_clustering --no_scheduling --debug       /
//                    --endian little --cpu ARM7TDMI-S -e --fpu None          /
//                    --dlib_config "D:\Program\IAR Systems\Embedded          /
//                    Workbench 5.0\ARM\INC\DLib_Config_Normal.h" -I          /
//                    D:\development\gediminas\CAN4USB\project\ -I            /
//                    D:\development\gediminas\CAN4USB\project\app\ -I        /
//                    D:\development\gediminas\CAN4USB\project\board\ -I      /
//                    D:\development\gediminas\CAN4USB\project\module\ -I     /
//                    D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\inc\ -I j:\common\ -I "D:\Program\IAR           /
//                    Systems\Embedded Workbench 5.0\ARM\INC\" --interwork    /
//                    --cpu_mode thumb -On                                    /
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\7 /
//                    5x_tb.s                                                 /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_tb`

        EXTERN MRCC_PeripheralSWResetConfig

        PUBLIC TB_ClearFlag
        PUBLIC TB_ClearITPendingBit
        PUBLIC TB_Cmd
        PUBLIC TB_CounterModeConfig
        PUBLIC TB_DeInit
        PUBLIC TB_DebugCmd
        PUBLIC TB_GetCounter
        PUBLIC TB_GetFlagStatus
        PUBLIC TB_GetICAP1
        PUBLIC TB_GetITStatus
        PUBLIC TB_ITConfig
        PUBLIC TB_Init
        PUBLIC TB_ResetCounter
        PUBLIC TB_SLaveModeConfig
        PUBLIC TB_SetCounter
        PUBLIC TB_SetPrescaler
        PUBLIC TB_StructInit
        
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
        
// D:\development\gediminas\CAN4USB\project\STR75xLibrary\library\src\75x_tb.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_tb.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the TB software functions.
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
//   20 #include "75x_tb.h"
//   21 #include "75x_mrcc.h"
//   22 
//   23 /* Private typedef -----------------------------------------------------------*/
//   24 /* Private define ------------------------------------------------------------*/
//   25 /* Private macro -------------------------------------------------------------*/
//   26 /* Private variables ---------------------------------------------------------*/
//   27 #define TB_IT_Enable_Mask   0x7FFF
//   28 #define TB_IT_Clear_Mask    0x7FFF
//   29 #define TB_IC_Enable        0x0004
//   30 #define TB_ICPolarity_Set   0x0008
//   31 #define TB_ICPolarity_Reset 0xFFF7
//   32 #define TB_UFS_Reset        0xFFFE
//   33 #define TB_UFS_Set          0x0001
//   34 
//   35 /* TB debug state */
//   36 #define TB_DBGC_Set    0x0400
//   37 #define TB_DBGC_Reset  0xFB7F
//   38 
//   39 /* TB counter state */
//   40 #define TB_COUNTER_Reset  0x0002
//   41 #define TB_COUNTER_Start  0x0004
//   42 #define TB_COUNTER_Stop   0xFFFB
//   43 
//   44 #define TB_SMS_EXTCLK_Set   0x0008
//   45 #define TB_SMS_RESETCLK_Set 0x0000
//   46 
//   47 /* TB Slave Mode Enable Set/Reset value */
//   48 #define TB_SME_Reset  0x731B
//   49 #define TB_SME_Set    0x0004
//   50 
//   51 /* TB Trigger Selection value */
//   52 #define TB_TS_IC1_Set  0x0200
//   53 
//   54 /* TB SCR Masks bit */
//   55 #define TB_SlaveModeSelection_Mask        0x7307
//   56 #define TB_TriggerSelection_Mask          0x701F
//   57 
//   58 /* Reset Register Masks */
//   59 #define TB_Prescaler_Reset_Mask   0x0000
//   60 #define TB_CounterMode_Mask       0xFF8F
//   61 #define TB_AutoReload_Reset_Mask  0xFFFF
//   62 
//   63 /* Private function prototypes -----------------------------------------------*/
//   64 /* Private functions ---------------------------------------------------------*/
//   65 
//   66  /******************************************************************************
//   67 * Function Name  : TB_DeInit
//   68 * Description    : Deinitializes the TB peripheral registers to their default
//   69 *                  reset values.
//   70 * Input          : None
//   71 * Output         : None
//   72 * Return         : None
//   73 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function TB_DeInit
        THUMB
//   74 void TB_DeInit(void)
//   75 {
TB_DeInit:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   76  /* Enters and exits the TB peripheral to and from reset */
//   77  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TB,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+2
        BL       MRCC_PeripheralSWResetConfig
//   78  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TB,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+2
        BL       MRCC_PeripheralSWResetConfig
//   79 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock0
//   80 
//   81 /*******************************************************************************
//   82 * Function Name  : TB_Init
//   83 * Description    : Initializes TB  peripheral according to the specified
//   84 *                  parameters in the TB_InitStruct.
//   85 * Input          : TB_InitStruct: pointer to a TB_InitTypeDef structure that
//   86 *                  contains the configuration information for the TB peripheral.
//   87 * Output         : None
//   88 * Return         : None
//   89 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function TB_Init
        THUMB
//   90 void TB_Init(TB_InitTypeDef* TB_InitStruct)
//   91 {
TB_Init:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   92   /* Set the TB prescaler value */
//   93   TB->PSC = TB_InitStruct->TB_Prescaler;
        LDRH     R1,[R0, #+8]
        LDR      R2,??DataTable25  ;; 0xffff8828
        STRH     R1,[R2, #+0]
//   94 
//   95   /* Set the TB period value */
//   96   TB->ARR = TB_InitStruct->TB_AutoReload;
        LDRH     R1,[R0, #+10]
        LDR      R2,??TB_Init_0   ;; 0xffff8830
        STRH     R1,[R2, #+0]
//   97 
//   98   /* Set the corresponding counter mode */
//   99   TB->CR = (TB->CR & TB_CounterMode_Mask) | TB_InitStruct->TB_CounterMode;
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable33  ;; 0xff8f
        ANDS     R2,R2,R1
        LDRH     R1,[R0, #+4]
        ORRS     R1,R1,R2
        LDR      R2,??DataTable36  ;; 0xffff8800
        STRH     R1,[R2, #+0]
//  100 
//  101   /* Set the corresponding clock source */
//  102   if(TB_InitStruct->TB_ClockSource == TB_ClockSource_CKRTC)
        LDRH     R1,[R0, #+2]
        CMP      R1,#+2
        BNE      ??TB_Init_1
//  103   {  
//  104     TB->SCR &= TB_SME_Reset & TB_SlaveModeSelection_Mask & TB_TriggerSelection_Mask;
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable38  ;; 0x7003
        ANDS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  105     TB->SCR |= TB_SMS_EXTCLK_Set | TB_SME_Set | TB_TS_IC1_Set;
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+131
        LSLS     R2,R2,#+2        ;; #+524
        ORRS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
        B        ??TB_Init_2
//  106   }
//  107   else
//  108   {
//  109     TB->SCR &= TB_SME_Reset & TB_SlaveModeSelection_Mask & TB_TriggerSelection_Mask;
??TB_Init_1:
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable38  ;; 0x7003
        ANDS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  110   }
//  111 
//  112   if(TB_InitStruct->TB_Mode == TB_Mode_IC)
??TB_Init_2:
        LDRH     R1,[R0, #+0]
        CMP      R1,#+2
        BNE      ??TB_Init_3
//  113   {
//  114     /* Set the corresponding value in TB SCR register */
//  115     TB->SCR &= TB_SME_Reset & TB_SlaveModeSelection_Mask & TB_TriggerSelection_Mask;
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable38  ;; 0x7003
        ANDS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  116     TB->SCR |= TB_SMS_RESETCLK_Set | TB_SME_Set | TB_TS_IC1_Set;
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+129
        LSLS     R2,R2,#+2        ;; #+516
        ORRS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  117 
//  118     /* Set the IC1 enable bit */
//  119     TB->IMCR |= TB_IC_Enable;
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+4
        ORRS     R2,R2,R1
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        STRH     R2,[R1, #+0]
//  120 
//  121     /* Set the input signal polarity */
//  122     if (TB_InitStruct->TB_ICAPolarity == TB_ICAPolarity_Falling)
        LDRH     R1,[R0, #+6]
        MOVS     R2,#+128
        LSLS     R2,R2,#+8        ;; #+32768
        CMP      R1,R2
        BNE      ??TB_Init_4
//  123     {
//  124       TB->IMCR |= TB_ICPolarity_Set;
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+8
        ORRS     R2,R2,R1
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        STRH     R2,[R1, #+0]
        B        ??TB_Init_3
//  125     }
//  126     else
//  127     {
//  128       TB->IMCR &= TB_ICPolarity_Reset;
??TB_Init_4:
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        LDRH     R1,[R1, #+0]
        LDR      R2,??TB_Init_0+0x8  ;; 0xfff7
        ANDS     R2,R2,R1
        LDR      R1,??TB_Init_0+0x4  ;; 0xffff8808
        STRH     R2,[R1, #+0]
//  129     }
//  130   }
//  131 }
??TB_Init_3:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TB_Init_0:
        DC32     0xffff8830
        DC32     0xffff8808
        DC32     0xfff7
        CFI EndBlock cfiBlock1
//  132 
//  133 /*******************************************************************************
//  134 * Function Name  : TB_StructInit
//  135 * Description    : Fills each TB_InitStruct member with its default value
//  136 * Input          : TB_InitStruct : pointer to a TB_InitTypeDef structure which
//  137 *                  will be initialized.
//  138 * Output         : None
//  139 * Return         : None
//  140 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function TB_StructInit
        THUMB
//  141 void TB_StructInit(TB_InitTypeDef *TB_InitStruct)
//  142 {
//  143   TB_InitStruct->TB_Mode = TB_Mode_Timing;
TB_StructInit:
        MOVS     R1,#+1
        STRH     R1,[R0, #+0]
//  144   TB_InitStruct->TB_ClockSource = TB_ClockSource_CKTIM;
        MOVS     R1,#+1
        STRH     R1,[R0, #+2]
//  145   TB_InitStruct->TB_CounterMode = TB_CounterMode_Up;
        MOVS     R1,#+0
        STRH     R1,[R0, #+4]
//  146   TB_InitStruct->TB_ICAPolarity = TB_ICAPolarity_Rising;
        MOVS     R1,#+224
        LSLS     R1,R1,#+7        ;; #+28672
        STRH     R1,[R0, #+6]
//  147   TB_InitStruct->TB_Prescaler = TB_Prescaler_Reset_Mask;
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  148   TB_InitStruct->TB_AutoReload = TB_AutoReload_Reset_Mask;
        LDR      R1,??TB_StructInit_0  ;; 0xffff
        STRH     R1,[R0, #+10]
//  149 }
        BX       LR               ;; return
        DATA
??TB_StructInit_0:
        DC32     0xffff
        CFI EndBlock cfiBlock2
//  150 
//  151 /*******************************************************************************
//  152 * Function Name  : TB_Cmd
//  153 * Description    : Enables or disables the TB peripheral.
//  154 * Input          : Newstate: new state of the TB peripheral. This parameter can
//  155 *                  be: ENABLE or DISABLE.
//  156 * Output         : None
//  157 * Return         : None
//  158 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function TB_Cmd
        THUMB
//  159 void TB_Cmd(FunctionalState Newstate)
//  160 {
TB_Cmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  161   if(Newstate == ENABLE)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??TB_Cmd_0
//  162   {
//  163     TB->CR |= TB_COUNTER_Start;
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+4
        ORRS     R2,R2,R1
        LDR      R1,??DataTable36  ;; 0xffff8800
        STRH     R2,[R1, #+0]
        B        ??TB_Cmd_1
//  164   }
//  165   else
//  166   {
//  167     TB->CR &= TB_COUNTER_Stop;
??TB_Cmd_0:
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        LDR      R2,??TB_Cmd_2    ;; 0xfffb
        ANDS     R2,R2,R1
        LDR      R1,??DataTable36  ;; 0xffff8800
        STRH     R2,[R1, #+0]
//  168   }
//  169 }
??TB_Cmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TB_Cmd_2:
        DC32     0xfffb
        CFI EndBlock cfiBlock3
//  170 
//  171 /*******************************************************************************
//  172 * Function Name  : TB_ITConfig
//  173 * Description    : Enables or disables the specified TB interrupt.
//  174 * Input          : - TB_IT: specifies the TB interrupt sources to be enabled or
//  175 *                    disabled.
//  176 *                    This parameter can be any combination of the following values:
//  177 *                         - TB_IT_Update: TB Update interrupt
//  178 *                         - TB_IT_GlobalUpdate: TB Global Update interrupt
//  179 *                         - TB_IT_IC: TB Input Capture interrupt
//  180 *                  - Newstate:  new state of the specified TB interrupts.
//  181 *                    This parameter can be: ENABLE or DISABLE.
//  182 * Output         : None
//  183 * Return         : None
//  184 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function TB_ITConfig
        THUMB
//  185 void TB_ITConfig(u16 TB_IT, FunctionalState Newstate)
//  186 {
TB_ITConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  187   u16 TB_IT_Enable = 0;
        MOVS     R3,#+0
        MOVS     R2,R3
//  188 
//  189   TB_IT_Enable = TB_IT & TB_IT_Enable_Mask;
        LSLS     R3,R0,#+17       ;; ZeroExtS R3,R0,#+17,#+17
        LSRS     R3,R3,#+17
        MOVS     R2,R3
//  190 
//  191   if(Newstate == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??TB_ITConfig_0
//  192   {
//  193    /* Update interrupt global source: overflow/undeflow, counter reset operation
//  194    or slave mode controller in reset mode */
//  195    if ((TB_IT & TB_IT_GlobalUpdate) == TB_IT_GlobalUpdate)
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LDR      R3,??TB_ITConfig_1  ;; 0x8001
        ANDS     R3,R3,R0
        LDR      R4,??TB_ITConfig_1  ;; 0x8001
        CMP      R3,R4
        BNE      ??TB_ITConfig_2
//  196    {
//  197      TB->CR &= TB_UFS_Reset;
        LDR      R3,??DataTable36  ;; 0xffff8800
        LDRH     R3,[R3, #+0]
        LDR      R4,??TB_ITConfig_1+0x4  ;; 0xfffe
        ANDS     R4,R4,R3
        LDR      R3,??DataTable36  ;; 0xffff8800
        STRH     R4,[R3, #+0]
        B        ??TB_ITConfig_3
//  198     }
//  199    /* Update interrupt source: counter overflow/underflow */
//  200    else if ((TB_IT & TB_IT_Update) == TB_IT_Update)
??TB_ITConfig_2:
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R3,R0,#+31
        BPL      ??TB_ITConfig_3
//  201    {
//  202     TB->CR |= TB_UFS_Set;
        LDR      R3,??DataTable36  ;; 0xffff8800
        LDRH     R3,[R3, #+0]
        MOVS     R4,#+1
        ORRS     R4,R4,R3
        LDR      R3,??DataTable36  ;; 0xffff8800
        STRH     R4,[R3, #+0]
//  203    }
//  204    /* Select and enable the interrupts requests */
//  205    TB->RSR |= TB_IT_Enable;
??TB_ITConfig_3:
        LDR      R3,??TB_ITConfig_1+0x8  ;; 0xffff8818
        LDRH     R3,[R3, #+0]
        ORRS     R3,R3,R2
        LDR      R4,??TB_ITConfig_1+0x8  ;; 0xffff8818
        STRH     R3,[R4, #+0]
//  206    TB->RER |= TB_IT_Enable;
        LDR      R3,??TB_ITConfig_1+0xC  ;; 0xffff881c
        LDRH     R3,[R3, #+0]
        ORRS     R3,R3,R2
        LDR      R4,??TB_ITConfig_1+0xC  ;; 0xffff881c
        STRH     R3,[R4, #+0]
        B        ??TB_ITConfig_4
//  207   }
//  208   /* Disable the interrupts requests */
//  209   else
//  210   {
//  211    TB->RSR &= ~TB_IT_Enable;
??TB_ITConfig_0:
        LDR      R3,??TB_ITConfig_1+0x8  ;; 0xffff8818
        LDRH     R3,[R3, #+0]
        BICS     R3,R3,R2
        LDR      R4,??TB_ITConfig_1+0x8  ;; 0xffff8818
        STRH     R3,[R4, #+0]
//  212    TB->RER &= ~TB_IT_Enable;
        LDR      R3,??TB_ITConfig_1+0xC  ;; 0xffff881c
        LDRH     R3,[R3, #+0]
        BICS     R3,R3,R2
        LDR      R4,??TB_ITConfig_1+0xC  ;; 0xffff881c
        STRH     R3,[R4, #+0]
//  213   }
//  214 }
??TB_ITConfig_4:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TB_ITConfig_1:
        DC32     0x8001
        DC32     0xfffe
        DC32     0xffff8818
        DC32     0xffff881c
        CFI EndBlock cfiBlock4
//  215 
//  216 /*******************************************************************************
//  217 * Function Name  : TB_SetPrescaler
//  218 * Description    : Sets the TB Prescaler value.
//  219 * Input          : Prescaler: specifies the TB Prescaler value.
//  220 * Output         : None
//  221 * Return         : None
//  222 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function TB_SetPrescaler
        THUMB
//  223 void TB_SetPrescaler(u16 Prescaler)
//  224 {
//  225   /* Sets the prescaler value */
//  226   TB->PSC = Prescaler;
TB_SetPrescaler:
        LDR      R1,??DataTable25  ;; 0xffff8828
        STRH     R0,[R1, #+0]
//  227 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock5

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable25:
        DC32     0xffff8828
//  228 
//  229 /*******************************************************************************
//  230 * Function Name  : TB_ResetCounter
//  231 * Description    : Re-intializes the counter and generates an update of the
//  232 *                  registers.
//  233 * Input          : None
//  234 * Output         : None
//  235 * Return         : None
//  236 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function TB_ResetCounter
        THUMB
//  237 void TB_ResetCounter(void)
//  238 {
//  239   /* Re-intializes TB counter */
//  240   TB->CR |= TB_COUNTER_Reset;
TB_ResetCounter:
        LDR      R0,??DataTable36  ;; 0xffff8800
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+2
        ORRS     R1,R1,R0
        LDR      R0,??DataTable36  ;; 0xffff8800
        STRH     R1,[R0, #+0]
//  241 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock6
//  242 
//  243 /*******************************************************************************
//  244 * Function Name  : TB_DebugCmd
//  245 * Description    : Enables or disables TB peripheral Debug control.
//  246 * Input          : Newstate: new state of the TB Debug control.
//  247 *                  This parameter can be: ENABLE or DISABLE.
//  248 * Output         : None
//  249 * Return         : None
//  250 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function TB_DebugCmd
        THUMB
//  251 void TB_DebugCmd(FunctionalState Newstate)
//  252 {
TB_DebugCmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  253   if(Newstate == ENABLE)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??TB_DebugCmd_0
//  254   {
//  255     TB->CR |= TB_DBGC_Set;
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+3        ;; #+1024
        ORRS     R2,R2,R1
        LDR      R1,??DataTable36  ;; 0xffff8800
        STRH     R2,[R1, #+0]
        B        ??TB_DebugCmd_1
//  256   }
//  257   else
//  258   {
//  259     TB->CR &= TB_DBGC_Reset;
??TB_DebugCmd_0:
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        LDR      R2,??TB_DebugCmd_2  ;; 0xfb7f
        ANDS     R2,R2,R1
        LDR      R1,??DataTable36  ;; 0xffff8800
        STRH     R2,[R1, #+0]
//  260   }
//  261 }
??TB_DebugCmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TB_DebugCmd_2:
        DC32     0xfb7f
        CFI EndBlock cfiBlock7
//  262 
//  263 /*******************************************************************************
//  264 * Function Name  : TB_CounterModeConfig
//  265 * Description    : Configures the TB Counter Mode.
//  266 * Input          : TB_CounterMode: specifies the TB counter mode to be used.
//  267 *                  This parameter can be one of the following values:
//  268 *                         - TB_CounterMode_Up: TB Up Counting Mode
//  269 *                         - TB_CounterMode_Down: TB Down Counting Mode
//  270 *                         - TB_CounterMode_CenterAligned: TB Center Aligned Mode
//  271 * Output         : None
//  272 * Return         : None
//  273 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function TB_CounterModeConfig
        THUMB
//  274 void TB_CounterModeConfig(u16 TB_CounterMode)
//  275 {
//  276   /* Counter mode configuration */
//  277   TB->CR &= TB_CounterMode_Mask;
TB_CounterModeConfig:
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable33  ;; 0xff8f
        ANDS     R2,R2,R1
        LDR      R1,??DataTable36  ;; 0xffff8800
        STRH     R2,[R1, #+0]
//  278   TB->CR |= TB_CounterMode;
        LDR      R1,??DataTable36  ;; 0xffff8800
        LDRH     R1,[R1, #+0]
        ORRS     R1,R1,R0
        LDR      R2,??DataTable36  ;; 0xffff8800
        STRH     R1,[R2, #+0]
//  279 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock8

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable33:
        DC32     0xff8f

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable36:
        DC32     0xffff8800
//  280 
//  281 /*******************************************************************************
//  282 * Function Name  : TB_SLaveModeConfig
//  283 * Description    : Configures the TB slave Mode.
//  284 * Input          : TB_SMSMode: specifies the TB slave mode to be used.
//  285 *                  This parameter can be one of the following values:
//  286 *                         - TB_SMSMode_Trigger: The counter starts at a rising 
//  287 *                           edge of the trigger 
//  288 *                         - TB_SMSMode_Gated: The counter clock is enabled when 
//  289 *                           trigger signal is high
//  290 *                         - TB_SMSMode_External: The rising edge of selected trigger
//  291 *                           clocks the counter
//  292 *                         - TB_SMSMode_Reset: The rising edge of the selected 
//  293 *                           trigger signal resets the counter
//  294 * Output         : None
//  295 * Return         : None
//  296 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function TB_SLaveModeConfig
        THUMB
//  297 void TB_SLaveModeConfig(u16 TB_SMSMode)
//  298 {
//  299   TB->SCR &= TB_SME_Reset & TB_SlaveModeSelection_Mask & TB_TriggerSelection_Mask;
TB_SLaveModeConfig:
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        LDR      R2,??DataTable38  ;; 0x7003
        ANDS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  300   TB->SCR |= TB_SME_Set | TB_SMSMode | TB_TS_IC1_Set; 
        LDR      R1,??DataTable41  ;; 0xffff8804
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+129
        LSLS     R2,R2,#+2        ;; #+516
        ORRS     R2,R2,R0
        ORRS     R2,R2,R1
        LDR      R1,??DataTable41  ;; 0xffff8804
        STRH     R2,[R1, #+0]
//  301 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock9

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable38:
        DC32     0x7003

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable41:
        DC32     0xffff8804
//  302 /*******************************************************************************
//  303 * Function Name  : TB_GetCounter
//  304 * Description    : Gets the TB Counter value.
//  305 * Input          : None
//  306 * Output         : None
//  307 * Return         : The TB counter register value.
//  308 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function TB_GetCounter
        THUMB
//  309 u16 TB_GetCounter(void)
//  310 {
//  311   return TB->CNT;
TB_GetCounter:
        LDR      R0,??DataTable43  ;; 0xffff8824
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        BX       LR               ;; return
        CFI EndBlock cfiBlock10
//  312 }
//  313 
//  314 /*******************************************************************************
//  315 * Function Name  : TB_GetICAP1
//  316 * Description    : Gets the TB Input capture value.
//  317 * Input          : None
//  318 * Output         : None
//  319 * Return         : The TB ICR1 register value.
//  320 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function TB_GetICAP1
        THUMB
//  321 u16 TB_GetICAP1(void)
//  322 {
//  323   return TB->ICR1;
TB_GetICAP1:
        LDR      R0,??TB_GetICAP1_0  ;; 0xffff884c
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        BX       LR               ;; return
        Nop      
        DATA
??TB_GetICAP1_0:
        DC32     0xffff884c
        CFI EndBlock cfiBlock11
//  324 }
//  325 
//  326 /*******************************************************************************
//  327 * Function Name  : TB_SetCounter
//  328 * Description    : Sets the TB Counter value.
//  329 * Input          : Counter: specifies the TB Counter value.
//  330 * Output         : None
//  331 * Return         : None
//  332 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function TB_SetCounter
        THUMB
//  333 void TB_SetCounter(u16 Counter)
//  334 {
//  335   TB->CNT = Counter;
TB_SetCounter:
        LDR      R1,??DataTable43  ;; 0xffff8824
        STRH     R0,[R1, #+0]
//  336 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock12

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable43:
        DC32     0xffff8824
//  337 
//  338 /*******************************************************************************
//  339 * Function Name  : TB_GetFlagStatus
//  340 * Description    : Checks whether the specified TB flag is set or not.
//  341 * Input          : TB_FLAG: specifies the flag to check.
//  342 *                  This parameter can be one of the following values:
//  343 *                         - TB_FLAG_IC: TB Input Capture flag
//  344 *                         - TB_FLAG_Update: TB update flag
//  345 * Output         : None
//  346 * Return         : The new state of the TB_FLAG (SET or RESET).
//  347 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function TB_GetFlagStatus
        THUMB
//  348 FlagStatus TB_GetFlagStatus(u16 TB_FLAG)
//  349 {
TB_GetFlagStatus:
        MOVS     R1,R0
//  350   if((TB->ISR & TB_FLAG) != RESET )
        LDR      R0,??DataTable49  ;; 0xffff8820
        LDRH     R2,[R0, #+0]
        ANDS     R2,R2,R1
        MOVS     R0,R2
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  351   {
//  352    return SET;
//  353   }
//  354   else
//  355   {
//  356    return RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock13
//  357   }
//  358 }
//  359 
//  360 /*******************************************************************************
//  361 * Function Name  : TB_ClearFlag
//  362 * Description    : Clears the TB’s pending flags.
//  363 * Input          : TB_FLAG: specifies the flag to clear.
//  364 *                  This parameter can be any combination of the following values:
//  365 *                         - TB_FLAG_IC: TB Input Capture flag
//  366 *                         - TB_FLAG_Update: TB update flag
//  367 * Output         : None
//  368 * Return         : None
//  369 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function TB_ClearFlag
        THUMB
//  370 void TB_ClearFlag(u16 TB_FLAG)
//  371 {
//  372   /* Clears the flags */
//  373   TB->ISR &= ~TB_FLAG;
TB_ClearFlag:
        LDR      R1,??DataTable49  ;; 0xffff8820
        LDRH     R1,[R1, #+0]
        BICS     R1,R1,R0
        LDR      R2,??DataTable49  ;; 0xffff8820
        STRH     R1,[R2, #+0]
//  374 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock14
//  375 
//  376 /*******************************************************************************
//  377 * Function Name  : TB_GetITStatus
//  378 * Description    : Checks whether the specified TB interrupt has occurred or not.
//  379 * Input          : TB_IT: specifies the interrupt to check.
//  380 *                  This parameter can be one of the following values:
//  381 *                       - TB_IT_Update: TB Update interrupt
//  382 *                       - TB_IT_GlobalUpdate: TB Global Update interrupt
//  383 *                       - TB_IT_IC: TB Input Capture interrupt
//  384 * Output         : None
//  385 * Return         : The new state of the TB_IT (SET or RESET).
//  386 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function TB_GetITStatus
        THUMB
//  387 ITStatus TB_GetITStatus(u16 TB_IT)
//  388 {
TB_GetITStatus:
        MOVS     R1,R0
//  389   u16 TB_IT_Check = 0;
        MOVS     R0,#+0
        MOVS     R2,R0
//  390 
//  391   /* Calculates the pending bits to be checked */
//  392   TB_IT_Check = TB_IT & TB_IT_Clear_Mask;
        LSLS     R0,R1,#+17       ;; ZeroExtS R0,R1,#+17,#+17
        LSRS     R0,R0,#+17
        MOVS     R2,R0
//  393   
//  394   if((TB->ISR & TB_IT_Check) != RESET )
        LDR      R0,??DataTable49  ;; 0xffff8820
        LDRH     R3,[R0, #+0]
        ANDS     R3,R3,R2
        MOVS     R0,R3
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  395   {
//  396    return SET;
//  397   }
//  398   else
//  399   {
//  400    return RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock15
//  401   }
//  402 }
//  403 
//  404 /*******************************************************************************
//  405 * Function Name  : TB_ClearITPendingBit
//  406 * Description    : Clears the TB's interrupt pending bits.
//  407 * Input          : TB_IT: specifies the interrupt pending bit to clear.
//  408 *                  This parameter can be any combination of the following values:
//  409 *                         - TB_IT_Update: TB Update interrupt
//  410 *                         - TB_IT_GlobalUpdate: TB Global Update interrupt
//  411 *                         - TB_IT_IC: TB Input Capture interrupt
//  412 * Output         : None
//  413 * Return         : None
//  414 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function TB_ClearITPendingBit
        THUMB
//  415 void TB_ClearITPendingBit(u16 TB_IT)
//  416 {
//  417   u16 TB_IT_Clear = 0;
TB_ClearITPendingBit:
        MOVS     R2,#+0
        MOVS     R1,R2
//  418 
//  419   /* Calculates the pending bits to be cleared */
//  420   TB_IT_Clear = TB_IT & TB_IT_Clear_Mask;
        LSLS     R2,R0,#+17       ;; ZeroExtS R2,R0,#+17,#+17
        LSRS     R2,R2,#+17
        MOVS     R1,R2
//  421 
//  422   /* Clears the pending bits */
//  423   TB->ISR &= ~TB_IT_Clear;
        LDR      R2,??DataTable49  ;; 0xffff8820
        LDRH     R2,[R2, #+0]
        BICS     R2,R2,R1
        LDR      R3,??DataTable49  ;; 0xffff8820
        STRH     R2,[R3, #+0]
//  424 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock16

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable49:
        DC32     0xffff8820

        END
//  425 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 694 bytes in section .text
// 
// 694 bytes of CODE memory
//
//Errors: none
//Warnings: none
