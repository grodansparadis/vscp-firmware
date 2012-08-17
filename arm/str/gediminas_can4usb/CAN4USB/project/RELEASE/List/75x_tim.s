///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:25 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\Program\IAR Systems\Embedded Workbench               /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_tim.c                                                 /
//    Command line =  "D:\Program\IAR Systems\Embedded Workbench              /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_tim.c" -lC D:\development\gediminas\CAN4USB\project\R /
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
//                    5x_tim.s                                                /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_tim`

        EXTERN MRCC_PeripheralSWResetConfig

        PUBLIC TIM_ClearFlag
        PUBLIC TIM_ClearITPendingBit
        PUBLIC TIM_ClockSourceConfig
        PUBLIC TIM_Cmd
        PUBLIC TIM_CounterModeConfig
        PUBLIC TIM_DMACmd
        PUBLIC TIM_DMAConfig
        PUBLIC TIM_DeInit
        PUBLIC TIM_DebugCmd
        PUBLIC TIM_ForcedOCConfig
        PUBLIC TIM_GetFlagStatus
        PUBLIC TIM_GetICAP1
        PUBLIC TIM_GetICAP2
        PUBLIC TIM_GetITStatus
        PUBLIC TIM_GetPWMIPeriod
        PUBLIC TIM_GetPWMIPulse
        PUBLIC TIM_ITConfig
        PUBLIC TIM_Init
        PUBLIC TIM_PreloadConfig
        PUBLIC TIM_ResetCounter
        PUBLIC TIM_SetPeriod
        PUBLIC TIM_SetPrescaler
        PUBLIC TIM_SetPulse
        PUBLIC TIM_StructInit
        PUBLIC TIM_SynchroConfig
        
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
        
// D:\Program\IAR Systems\Embedded Workbench 5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75x_tim.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_tim.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the TIM software functions.
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
//   20 #include "75x_tim.h" 
//   21 #include "75x_mrcc.h"
//   22 
//   23 /* Private typedef -----------------------------------------------------------*/
//   24 /* Private define ------------------------------------------------------------*/
//   25 /* Private macro -------------------------------------------------------------*/
//   26 /* Private variables ---------------------------------------------------------*/
//   27 /* TIM interrupt masks */
//   28 #define TIM_IT_Clear_Mask   0x7FFF
//   29 #define TIM_IT_Enable_Mask  0x7FFF
//   30 
//   31 /* TIM Input Capture Selection Set/Reset */
//   32 #define TIM_IC1S_Set    0x0001
//   33 #define TIM_IC1S_Reset  0x003E
//   34 
//   35 /* TIM Input Capture Selection Set/Reset */
//   36 #define TIM_IC2S_Set    0x0002
//   37 #define TIM_IC2S_Reset  0x003D
//   38 
//   39 /* TIM_SCR Masks bit */
//   40 #define TIM_Encoder_Mask                   0x731C
//   41 #define TIM_SlaveModeSelection_Mask        0x7307
//   42 #define TIM_TriggerSelection_Mask          0x701F
//   43 #define TIM_InternalTriggerSelection_Mask  0x031F
//   44 
//   45 /* TIM Encoder mode Set value */
//   46 #define TIM_Encoder1_Set  0x0001
//   47 #define TIM_Encoder2_Set  0x0002
//   48 #define TIM_Encoder3_Set  0x0003
//   49 
//   50 /* TIM Slave Mode Enable Set/Reset value */
//   51 #define TIM_SME_Reset  0x731B
//   52 #define TIM_SME_Set    0x0004
//   53 
//   54 /* TIM Internal Trigger Selection value */
//   55 #define TIM_ITS_TIM0  0x1000
//   56 #define TIM_ITS_TIM1  0x2000
//   57 #define TIM_ITS_TIM2  0x3000
//   58 #define TIM_ITS_PWM   0x4000
//   59 
//   60 /* TIM Trigger Selection value */
//   61 #define TIM_TS_IC1_Set  0x0200
//   62 #define TIM_TS_IC2_Set  0x0300
//   63 
//   64 /* TIM Slave Mode selction external clock Set value */
//   65 #define TIM_SMS_EXTCLK_Set    0x0008
//   66 #define TIM_SMS_RESETCLK_Set  0x0000
//   67 
//   68 /* TIM_CR Masks bit */
//   69 #define TIM_DBASE_Mask                0x077F
//   70 #define TIM_MasterModeSelection_Mask  0xFC7F
//   71 #define TIM_CounterMode_Mask          0xFF8F
//   72 
//   73 /* TIM Update flag selection Set/Reset value */
//   74 #define TIM_UFS_Reset  0xFFFE
//   75 #define TIM_UFS_Set    0x0001
//   76 
//   77 /* TIM Counter value */
//   78 #define TIM_COUNTER_Reset  0x0002
//   79 #define TIM_COUNTER_Start  0x0004
//   80 #define TIM_COUNTER_Stop   0xFFFB
//   81 
//   82 /* TIM One pulse Mode set value */
//   83 #define TIM_OPM_Set    0x0008
//   84 #define TIM_OPM_Reset  0xFFF7
//   85 
//   86 /* TIM Debug Mode Set/Reset value */
//   87 #define TIM_DBGC_Set    0x0400
//   88 #define TIM_DBGC_Reset  0xFB7F
//   89 
//   90 /* TIM Input Capture Enable/Disable value */
//   91 #define TIM_IC1_Enable  0x0004
//   92 #define TIM_IC2_Enable  0x0010
//   93 
//   94 /* TIM Input Capture Polarity Set/Reset value */
//   95 #define TIM_IC1P_Set    0x0008
//   96 #define TIM_IC2P_Set    0x0020
//   97 #define TIM_IC1P_Reset  0x0037
//   98 #define TIM_IC2P_Reset  0x001F
//   99 
//  100 /* TIM Output Compare Polarity Set/Reset value */
//  101 #define TIM_OC1P_Set    0x0020
//  102 #define TIM_OC2P_Set    0x2000
//  103 #define TIM_OC1P_Reset  0x3F1F
//  104 #define TIM_OC2P_Reset  0x1F3F
//  105 
//  106 /* TIM Output Compare control mode constant */
//  107 #define TIM_OCControl_PWM         0x000C
//  108 #define TIM_OCControl_OCToggle    0x0006
//  109 #define TIM_OCControl_OCInactive  0x0004
//  110 #define TIM_OCControl_OCActive    0x0002
//  111 #define TIM_OCControl_OCTiming    0x0000
//  112 
//  113 /* TIM Output Compare mode Enable value */
//  114 #define TIM_OC1_Enable  0x0010
//  115 #define TIM_OC2_Enable  0x1000
//  116 
//  117 /* TIM Output Compare mode Mask value */
//  118 #define TIM_OC1C_Mask  0x3F31
//  119 #define TIM_OC2C_Mask  0x313F
//  120 
//  121 /* TIM Preload bit Set/Reset value */
//  122 #define TIM_PLD1_Set    0x0001
//  123 #define TIM_PLD1_Reset  0xFFFE
//  124 
//  125 #define TIM_PLD2_Set    0x0100
//  126 #define TIM_PLD2_Reset  0xFEFF
//  127 
//  128 /* TIM OCRM Set/Reset value */
//  129 #define TIM_OCRM_Set    0x0080
//  130 #define TIM_OCRM_Reset  0x030D
//  131 
//  132 /* Reset Register Masks */
//  133 #define TIM_Pulse2_Reset_Mask     0x0000
//  134 #define TIM_Prescaler_Reset_Mask  0x0000
//  135 #define TIM_Pulse1_Reset_Mask     0x0000
//  136 #define TIM_Period_Reset_Mask     0xFFFF
//  137 #define TIM_Counter_Reset         0x0002
//  138 
//  139 /* Private function prototypes -----------------------------------------------*/
//  140 static void ICAP_ModuleConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct);
//  141 static void Encoder_ModeConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct);
//  142 static void OCM_ModuleConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct);
//  143 
//  144 /* Private functions ---------------------------------------------------------*/
//  145 
//  146 /******************************************************************************
//  147 * Function Name  : TIM_DeInit
//  148 * Description    : Deinitializes TIM peripheral registers to their default reset
//  149 *                  values.
//  150 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  151 * Output         : None
//  152 * Return         : None
//  153 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function TIM_DeInit
        THUMB
//  154 void TIM_DeInit(TIM_TypeDef *TIMx)
//  155 { 
TIM_DeInit:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  156   if(TIMx == TIM0)
        LDR      R0,??DataTable9  ;; 0xffff8c00
        CMP      R4,R0
        BNE      ??TIM_DeInit_0
//  157   {
//  158     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+4
        BL       MRCC_PeripheralSWResetConfig
//  159     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+4
        BL       MRCC_PeripheralSWResetConfig
        B        ??TIM_DeInit_1
//  160   }
//  161   else if(TIMx == TIM1)
??TIM_DeInit_0:
        LDR      R0,??DataTable1  ;; 0xffff9000
        CMP      R4,R0
        BNE      ??TIM_DeInit_2
//  162   {
//  163     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM1,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+8
        BL       MRCC_PeripheralSWResetConfig
//  164     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM1,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+8
        BL       MRCC_PeripheralSWResetConfig
        B        ??TIM_DeInit_1
//  165   }
//  166   else if(TIMx == TIM2)
??TIM_DeInit_2:
        LDR      R0,??DataTable2  ;; 0xffff9400
        CMP      R4,R0
        BNE      ??TIM_DeInit_1
//  167   {
//  168     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM2,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+16
        BL       MRCC_PeripheralSWResetConfig
//  169     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM2,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+16
        BL       MRCC_PeripheralSWResetConfig
//  170   }
//  171 }
??TIM_DeInit_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock0

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable1:
        DC32     0xffff9000

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable2:
        DC32     0xffff9400
//  172 
//  173 /*******************************************************************************
//  174 * Function Name  : TIM_Init
//  175 * Description    : Initializes the TIMx peripheral according to the specified
//  176 *                  parameters in the TIM_InitStruct .
//  177 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  178 *                  - TIM_InitStruct: pointer to a TIM_InitTypeDef structure that
//  179 *                    contains the configuration information for the specified TIM
//  180 *                    peripheral.
//  181 * Output         : None
//  182 * Return         : None
//  183 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function TIM_Init
        THUMB
//  184 void TIM_Init(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct)
//  185 {
TIM_Init:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
//  186   /* Set the prescaler value */
//  187   TIMx->PSC = TIM_InitStruct->TIM_Prescaler;
        LDRH     R0,[R5, #+2]
        STRH     R0,[R4, #+40]
//  188 
//  189   /* Select the clock source */
//  190   TIM_ClockSourceConfig(TIMx, TIM_InitStruct->TIM_ClockSource,
//  191                            TIM_InitStruct->TIM_ExtCLKEdge);
        LDRH     R2,[R5, #+6]
        LDRH     R1,[R5, #+4]
        MOVS     R0,R4
        BL       TIM_ClockSourceConfig
//  192 
//  193   /* Select the counter mode */
//  194   TIMx->CR &= TIM_CounterMode_Mask;
        LDRH     R0,[R4, #+0]
        LDR      R1,??DataTable3  ;; 0xff8f
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+0]
//  195   TIMx->CR |= TIM_InitStruct->TIM_CounterMode;
        LDRH     R0,[R4, #+0]
        LDRH     R1,[R5, #+8]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+0]
//  196 
//  197   /* Set the period value */
//  198   TIMx->ARR = TIM_InitStruct->TIM_Period;
        LDRH     R0,[R5, #+10]
        STRH     R0,[R4, #+48]
//  199 
//  200   switch(TIM_InitStruct->TIM_Mode)
        LDRH     R0,[R5, #+0]
        SUBS     R0,R0,#+1
        CMP      R0,#+4
        BLS      ??TIM_Init_0
        SUBS     R0,R0,#+5
        CMP      R0,#+1
        BLS      ??TIM_Init_1
        SUBS     R0,R0,#+2
        CMP      R0,#+2
        BLS      ??TIM_Init_2
        SUBS     R0,R0,#+3
        CMP      R0,#+2
        BLS      ??TIM_Init_3
        B        ??TIM_Init_4
//  201   {
//  202     case TIM_Mode_OCTiming: case TIM_Mode_OCActive: case TIM_Mode_OCInactive:
//  203     case TIM_Mode_OCToggle: case TIM_Mode_PWM:
//  204       OCM_ModuleConfig(TIMx, TIM_InitStruct);
??TIM_Init_0:
        MOVS     R1,R5
        MOVS     R0,R4
        BL       OCM_ModuleConfig
        B        ??TIM_Init_4
//  205     break;
//  206 
//  207     case TIM_Mode_PWMI: case TIM_Mode_IC:
//  208       ICAP_ModuleConfig(TIMx, TIM_InitStruct);
??TIM_Init_1:
        MOVS     R1,R5
        MOVS     R0,R4
        BL       ICAP_ModuleConfig
        B        ??TIM_Init_4
//  209     break;
//  210 
//  211     case TIM_Mode_Encoder1: case TIM_Mode_Encoder2: case TIM_Mode_Encoder3:
//  212       Encoder_ModeConfig(TIMx, TIM_InitStruct);
??TIM_Init_2:
        MOVS     R1,R5
        MOVS     R0,R4
        BL       Encoder_ModeConfig
        B        ??TIM_Init_4
//  213     break;
//  214 
//  215     case TIM_Mode_OPM_PWM: case TIM_Mode_OPM_Toggle: case TIM_Mode_OPM_Active:
//  216 
//  217       /* Output module configuration */
//  218       OCM_ModuleConfig(TIMx, TIM_InitStruct);
??TIM_Init_3:
        MOVS     R1,R5
        MOVS     R0,R4
        BL       OCM_ModuleConfig
//  219 
//  220       /* Input module configuration */
//  221       ICAP_ModuleConfig(TIMx, TIM_InitStruct);
        MOVS     R1,R5
        MOVS     R0,R4
        BL       ICAP_ModuleConfig
//  222       
//  223       /* Set the slave mode to trigger Mode */
//  224       TIMx->SCR |= TIM_SynchroMode_Trigger;
        LDRH     R0,[R4, #+4]
        MOVS     R1,#+24
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+4]
//  225 
//  226       /* Repetitive pulse state selection */
//  227       if(TIM_InitStruct->TIM_RepetitivePulse == TIM_RepetitivePulse_Disable)
        LDRH     R0,[R5, #+18]
        CMP      R0,#+5
        BNE      ??TIM_Init_5
//  228       {
//  229         TIMx->CR |= TIM_OPM_Set;
        LDRH     R0,[R4, #+0]
        MOVS     R1,#+8
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+0]
        B        ??TIM_Init_4
//  230       }
//  231       else
//  232       {
//  233         TIMx->CR &= TIM_OPM_Reset;
??TIM_Init_5:
        LDRH     R0,[R4, #+0]
        LDR      R1,??TIM_Init_6  ;; 0xfff7
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+0]
//  234       }
//  235     break;
//  236 
//  237     default:
//  238     break;
//  239   }
//  240 }
??TIM_Init_4:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TIM_Init_6:
        DC32     0xfff7
        CFI EndBlock cfiBlock1

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable3:
        DC32     0xff8f
//  241 
//  242 /*******************************************************************************
//  243 * Function Name  : TIM_StructInit
//  244 * Description    : Fills each TIM_InitStruct member with its default value.
//  245 * Input          : TIM_InitStruct : pointer to a TIM_InitTypeDef structure
//  246 *                  which will be initialized.
//  247 * Output         : None                        
//  248 * Return         : None.
//  249 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function TIM_StructInit
        THUMB
//  250 void TIM_StructInit(TIM_InitTypeDef *TIM_InitStruct)
//  251 {
//  252   /* Set the default configuration */
//  253   TIM_InitStruct->TIM_Mode = TIM_Mode_OCTiming;
TIM_StructInit:
        MOVS     R1,#+1
        STRH     R1,[R0, #+0]
//  254   TIM_InitStruct->TIM_Prescaler = TIM_Prescaler_Reset_Mask;
        MOVS     R1,#+0
        STRH     R1,[R0, #+2]
//  255   TIM_InitStruct->TIM_ClockSource = TIM_ClockSource_Internal;
        MOVS     R1,#+1
        STRH     R1,[R0, #+4]
//  256   TIM_InitStruct->TIM_ExtCLKEdge = TIM_ExtCLKEdge_Rising;
        MOVS     R1,#+2
        STRH     R1,[R0, #+6]
//  257   TIM_InitStruct->TIM_CounterMode = TIM_CounterMode_Up;
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  258   TIM_InitStruct->TIM_Period = TIM_Period_Reset_Mask;
        LDR      R1,??TIM_StructInit_0  ;; 0xffff
        STRH     R1,[R0, #+10]
//  259   TIM_InitStruct->TIM_Channel = TIM_Channel_ALL;
        MOVS     R1,#+3
        STRH     R1,[R0, #+12]
//  260   TIM_InitStruct->TIM_Pulse1 = TIM_Pulse1_Reset_Mask;
        MOVS     R1,#+0
        STRH     R1,[R0, #+14]
//  261   TIM_InitStruct->TIM_Pulse2 = TIM_Pulse2_Reset_Mask;
        MOVS     R1,#+0
        STRH     R1,[R0, #+16]
//  262   TIM_InitStruct->TIM_RepetitivePulse = TIM_RepetitivePulse_Disable;
        MOVS     R1,#+5
        STRH     R1,[R0, #+18]
//  263   TIM_InitStruct->TIM_Polarity1 = TIM_Polarity1_Low;
        MOVS     R1,#+2
        STRH     R1,[R0, #+20]
//  264   TIM_InitStruct->TIM_Polarity2 = TIM_Polarity2_Low;
        MOVS     R1,#+2
        STRH     R1,[R0, #+22]
//  265   TIM_InitStruct->TIM_IC1Selection = TIM_IC1Selection_TI1;
        MOVS     R1,#+1
        STRH     R1,[R0, #+24]
//  266   TIM_InitStruct->TIM_IC2Selection = TIM_IC2Selection_TI1;
        MOVS     R1,#+1
        STRH     R1,[R0, #+26]
//  267   TIM_InitStruct->TIM_IC1Polarity = TIM_IC1Polarity_Rising;
        MOVS     R1,#+2
        STRH     R1,[R0, #+28]
//  268   TIM_InitStruct->TIM_IC2Polarity = TIM_IC2Polarity_Rising;
        MOVS     R1,#+2
        STRH     R1,[R0, #+30]
//  269   TIM_InitStruct->TIM_PWMI_ICSelection = TIM_PWMI_ICSelection_TI1;
        MOVS     R1,#+1
        STRH     R1,[R0, #+32]
//  270   TIM_InitStruct->TIM_PWMI_ICPolarity = TIM_PWMI_ICPolarity_Rising;
        MOVS     R1,#+4
        STRH     R1,[R0, #+34]
//  271 }
        BX       LR               ;; return
        Nop      
        DATA
??TIM_StructInit_0:
        DC32     0xffff
        CFI EndBlock cfiBlock2
//  272 
//  273 /*******************************************************************************
//  274 * Function Name  : TIM_Cmd
//  275 * Description    : Enables or disables the specified TIM peripheral.
//  276 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  277 *                  - Newstate: new state of the TIMx peripheral.
//  278 *                    This parameter can be: ENABLE or DISABLE.
//  279 * Output         : None
//  280 * Return         : None
//  281 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function TIM_Cmd
        THUMB
//  282 void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState Newstate)
//  283 {
TIM_Cmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  284  if(Newstate == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??TIM_Cmd_0
//  285   { 
//  286     TIMx->CR |= TIM_COUNTER_Start;
        LDRH     R2,[R0, #+0]
        MOVS     R3,#+4
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+0]
        B        ??TIM_Cmd_1
//  287   }
//  288   else
//  289   {
//  290     TIMx->CR &= TIM_COUNTER_Stop;
??TIM_Cmd_0:
        LDRH     R2,[R0, #+0]
        LDR      R3,??TIM_Cmd_2   ;; 0xfffb
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+0]
//  291   }
//  292 }
??TIM_Cmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TIM_Cmd_2:
        DC32     0xfffb
        CFI EndBlock cfiBlock3
//  293 
//  294 /*******************************************************************************
//  295 * Function Name  : TIM_ITConfig
//  296 * Description    : Enables or disables the TIM interrupts.
//  297 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  298 *                  - TIM_IT: specifies the TIM interrupts sources to be enabled
//  299 *                    or disabled.
//  300 *                    This parameter can be any combination of the following values:
//  301 *                         - TIM_IT_IC1: Input Capture 1 Interrupt 
//  302 *                         - TIM_IT_OC1: Output Compare 1 Interrupt 
//  303 *                         - TIM_IT_Update: Timer update Interrupt 
//  304 *                         - TIM_IT_GlobalUpdate: Timer global update Interrupt 
//  305 *                         - TIM_IT_IC2: Input Capture 2 Interrupt 
//  306 *                         - TIM_IT_OC2: Output Compare 2 Interrupt 
//  307 *                  - Newstate: new state of the specified TIMx interrupts. 
//  308 *                    This parameter can be: ENABLE or DISABLE.
//  309 * Output         : None
//  310 * Return         : None
//  311 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function TIM_ITConfig
        THUMB
//  312 void TIM_ITConfig(TIM_TypeDef *TIMx, u16 TIM_IT, FunctionalState Newstate)
//  313 { 
TIM_ITConfig:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
//  314   u16 TIM_IT_Enable = 0;
        MOVS     R4,#+0
        MOVS     R3,R4
//  315 
//  316   TIM_IT_Enable = TIM_IT & TIM_IT_Enable_Mask;
        LSLS     R4,R1,#+17       ;; ZeroExtS R4,R1,#+17,#+17
        LSRS     R4,R4,#+17
        MOVS     R3,R4
//  317 
//  318   if(Newstate == ENABLE)
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,#+1
        BNE      ??TIM_ITConfig_0
//  319   {
//  320     /* Update interrupt global source: overflow/undeflow, counter reset operation
//  321     or slave mode controller in reset mode */
//  322     if((TIM_IT & TIM_IT_GlobalUpdate) == TIM_IT_GlobalUpdate)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LDR      R4,??TIM_ITConfig_1  ;; 0x1001
        ANDS     R4,R4,R1
        LDR      R5,??TIM_ITConfig_1  ;; 0x1001
        CMP      R4,R5
        BNE      ??TIM_ITConfig_2
//  323     {
//  324       TIMx->CR &= TIM_UFS_Reset;
        LDRH     R4,[R0, #+0]
        LDR      R5,??DataTable5  ;; 0xfffe
        ANDS     R5,R5,R4
        STRH     R5,[R0, #+0]
        B        ??TIM_ITConfig_3
//  325     }
//  326     /* Update interrupt source: counter overflow/underflow */
//  327     else if((TIM_IT & TIM_IT_Update) == TIM_IT_Update)
??TIM_ITConfig_2:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSLS     R4,R1,#+31
        BPL      ??TIM_ITConfig_3
//  328     {
//  329       TIMx->CR |= TIM_UFS_Set;
        LDRH     R4,[R0, #+0]
        MOVS     R5,#+1
        ORRS     R5,R5,R4
        STRH     R5,[R0, #+0]
//  330     }
//  331     /* Select and enable the interrupts requests */
//  332     TIMx->RSR |= TIM_IT_Enable;
??TIM_ITConfig_3:
        LDRH     R4,[R0, #+24]
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+24]
//  333     TIMx->RER |= TIM_IT_Enable;
        LDRH     R4,[R0, #+28]
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+28]
        B        ??TIM_ITConfig_4
//  334   }
//  335   /* Disable the interrupts requests */
//  336   else
//  337   {
//  338     TIMx->RSR &= ~TIM_IT_Enable;
??TIM_ITConfig_0:
        LDRH     R4,[R0, #+24]
        BICS     R4,R4,R3
        STRH     R4,[R0, #+24]
//  339     TIMx->RER &= ~TIM_IT_Enable;
        LDRH     R4,[R0, #+28]
        BICS     R4,R4,R3
        STRH     R4,[R0, #+28]
//  340   }
//  341 }
??TIM_ITConfig_4:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TIM_ITConfig_1:
        DC32     0x1001
        CFI EndBlock cfiBlock4
//  342 
//  343 /*******************************************************************************
//  344 * Function Name  : TIM_PreloadConfig
//  345 * Description    : Enables or disables TIM peripheral Preload register on OCRx.
//  346 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  347 *                  - TIM_Channel: specifies the TIM channel to be used.
//  348 *                    This parameter can be one of the following values:
//  349 *                         - TIM_Channel_1: TIM Channel 1 is used
//  350 *                         - TIM_Channel_2: TIM Channel 2 is used
//  351 *                         - TIM_Channel_ALL: TIM Channel 1and 2 are used
//  352 *                  - Newstate: new state of the TIMx peripheral Preload register
//  353 *                    This parameter can be: ENABLE or DISABLE.
//  354 * Output         : None
//  355 * Return         : None
//  356 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function TIM_PreloadConfig
        THUMB
//  357 void TIM_PreloadConfig(TIM_TypeDef *TIMx, u16 TIM_Channel, FunctionalState Newstate)
//  358 {
TIM_PreloadConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  359   if(Newstate == ENABLE)
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,#+1
        BNE      ??TIM_PreloadConfig_0
//  360   {
//  361     switch (TIM_Channel)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        MOVS     R3,R1
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_1
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_2
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_3
        B        ??TIM_PreloadConfig_4
//  362     {
//  363       case TIM_Channel_1:
//  364       TIMx->OMR1 |= TIM_PLD1_Set;
??TIM_PreloadConfig_1:
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+1
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_PreloadConfig_4
//  365       break;
//  366    
//  367       case TIM_Channel_2:
//  368       TIMx->OMR1 |= TIM_PLD2_Set;
??TIM_PreloadConfig_2:
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+128
        LSLS     R4,R4,#+1        ;; #+256
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_PreloadConfig_4
//  369       break;
//  370 
//  371       case TIM_Channel_ALL:
//  372       TIMx->OMR1 |= TIM_PLD1_Set | TIM_PLD2_Set;
??TIM_PreloadConfig_3:
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+255
        ADDS     R4,R4,#+2        ;; #+257
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_PreloadConfig_4
//  373       break;
//  374 
//  375       default:
//  376       break;
//  377    }
//  378   }
//  379   else
//  380   {
//  381     switch (TIM_Channel)
??TIM_PreloadConfig_0:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        MOVS     R3,R1
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_5
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_6
        SUBS     R3,R3,#+1
        BEQ      ??TIM_PreloadConfig_7
        B        ??TIM_PreloadConfig_4
//  382     {
//  383       case TIM_Channel_1:
//  384       TIMx->OMR1 &= TIM_PLD1_Reset;
??TIM_PreloadConfig_5:
        LDRH     R3,[R0, #+12]
        LDR      R4,??DataTable5  ;; 0xfffe
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_PreloadConfig_4
//  385       break;
//  386    
//  387       case TIM_Channel_2:
//  388       TIMx->OMR1 &= TIM_PLD2_Reset;
??TIM_PreloadConfig_6:
        LDRH     R3,[R0, #+12]
        LDR      R4,??TIM_PreloadConfig_8  ;; 0xfeff
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_PreloadConfig_4
//  389       break;
//  390 
//  391       case TIM_Channel_ALL:
//  392       TIMx->OMR1 &= TIM_PLD1_Reset & TIM_PLD2_Reset;
??TIM_PreloadConfig_7:
        LDRH     R3,[R0, #+12]
        LDR      R4,??TIM_PreloadConfig_8+0x4  ;; 0xfefe
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
//  393       break;
//  394 
//  395       default:
//  396       break;
//  397     }
//  398   }  
//  399 }
??TIM_PreloadConfig_4:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TIM_PreloadConfig_8:
        DC32     0xfeff
        DC32     0xfefe
        CFI EndBlock cfiBlock5

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable5:
        DC32     0xfffe
//  400 
//  401 /*******************************************************************************
//  402 * Function Name  : TIM_DMAConfig
//  403 * Description    : Configures the TIM0’s DMA interface.
//  404 * Input          : - TIM_DMASources: specifies the DMA Request sources.
//  405 *                    This parameter can be any combination of the following values:
//  406 *                         - TIM_DMASource_OC1: Output Compare 1 DMA source
//  407 *                         - TIM_DMASource_OC2: Output Compare 2 DMA source
//  408 *                         - TIM_DMASource_IC1: Input Capture 1 DMA source
//  409 *                         - TIM_DMASource_IC2: Input Capture 2 DMA source
//  410 *                         - TIM_DMASource_Update: Timer Update DMA source
//  411 *                  - TIM_OCRMState: the state of output compare request mode.
//  412 *                    This parameter can be one of the following values:
//  413 *                         - TIM_OCRMState_Enable 
//  414 *                         - TIM_OCRMState_Disable 
//  415 *                  - TIM_DMABase:DMA Base address.
//  416 *                    This parameter can be one of the following values:
//  417 *                    TIM_DMABase_CR, TIM_DMABase_SCR, TIM_DMABase_IMCR,
//  418 *                    TIM_DMABase_OMR1, TIM_DMABase_RSR,
//  419 *                    TIM_DMABase_RER, TIM_DMABase_ISR, TIM_DMABase_CNT, 
//  420 *                    TIM_DMABase_PSC, TIM_DMABase_ARR, TIM_DMABase_OCR1, 
//  421 *                    TIM_DMABase_OCR2, TIM_DMABase_ICR1, TIM_DMABase_ICR2
//  422 * Output         : None
//  423 * Return         : None
//  424 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function TIM_DMAConfig
        THUMB
//  425 void TIM_DMAConfig(u16 TIM_DMASources, u16 TIM_OCRMState, u16 TIM_DMABase)
//  426 {
TIM_DMAConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  427   /* Select the DMA requests */
//  428   TIM0->RSR &= TIM_DMASources;
        LDR      R3,??TIM_DMAConfig_0  ;; 0xffff8c18
        LDRH     R3,[R3, #+0]
        ANDS     R3,R3,R0
        LDR      R4,??TIM_DMAConfig_0  ;; 0xffff8c18
        STRH     R3,[R4, #+0]
//  429 
//  430   /* Set the OCRM state */
//  431   if(TIM_OCRMState == TIM_OCRMState_Enable)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+5
        BNE      ??TIM_DMAConfig_1
//  432   {
//  433     TIM0->RSR |= TIM_OCRM_Set;
        LDR      R3,??TIM_DMAConfig_0  ;; 0xffff8c18
        LDRH     R3,[R3, #+0]
        MOVS     R4,#+128
        ORRS     R4,R4,R3
        LDR      R3,??TIM_DMAConfig_0  ;; 0xffff8c18
        STRH     R4,[R3, #+0]
        B        ??TIM_DMAConfig_2
//  434   }
//  435   else
//  436   {
//  437     TIM0->RSR &= TIM_OCRM_Reset;
??TIM_DMAConfig_1:
        LDR      R3,??TIM_DMAConfig_0  ;; 0xffff8c18
        LDRH     R3,[R3, #+0]
        LDR      R4,??TIM_DMAConfig_0+0x4  ;; 0x30d
        ANDS     R4,R4,R3
        LDR      R3,??TIM_DMAConfig_0  ;; 0xffff8c18
        STRH     R4,[R3, #+0]
//  438   }
//  439 
//  440   /* Set the DMA Base address */
//  441   TIM0->CR &= TIM_DBASE_Mask;
??TIM_DMAConfig_2:
        LDR      R3,??DataTable9  ;; 0xffff8c00
        LDRH     R3,[R3, #+0]
        LDR      R4,??TIM_DMAConfig_0+0x8  ;; 0x77f
        ANDS     R4,R4,R3
        LDR      R3,??DataTable9  ;; 0xffff8c00
        STRH     R4,[R3, #+0]
//  442   TIM0->CR |= TIM_DMABase;
        LDR      R3,??DataTable9  ;; 0xffff8c00
        LDRH     R3,[R3, #+0]
        ORRS     R3,R3,R2
        LDR      R4,??DataTable9  ;; 0xffff8c00
        STRH     R3,[R4, #+0]
//  443 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TIM_DMAConfig_0:
        DC32     0xffff8c18
        DC32     0x30d
        DC32     0x77f
        CFI EndBlock cfiBlock6

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable9:
        DC32     0xffff8c00
//  444 
//  445 /*******************************************************************************
//  446 * Function Name  : TIM_DMACmd
//  447 * Description    : Enables or disables the TIM0’s DMA interface.
//  448 * Input          : - TIM_DMASources: specifies the DMA Request sources.
//  449 *                    This parameter can be any combination of the following values:
//  450 *                         - TIM_DMASource_OC1: Output Compare 1 DMA source
//  451 *                         - TIM_DMASource_OC2: Output Compare 2 DMA source
//  452 *                         - TIM_DMASource_IC1: Input Capture 1 DMA source
//  453 *                         - TIM_DMASource_IC2: Input Capture 2 DMA source
//  454 *                         - TIM_DMASource_Update: Timer Update DMA source
//  455 *                  - Newstate: new state of the DMA Request sources.
//  456 *                    This parameter can be: ENABLE or DISABLE.
//  457 * Output         : None
//  458 * Return         : None
//  459 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function TIM_DMACmd
        THUMB
//  460 void TIM_DMACmd(u16 TIM_DMASources, FunctionalState Newstate)
//  461 {
TIM_DMACmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  462   if(Newstate == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??TIM_DMACmd_0
//  463   {
//  464     TIM0->RER |= TIM_DMASources;
        LDR      R2,??TIM_DMACmd_1  ;; 0xffff8c1c
        LDRH     R2,[R2, #+0]
        ORRS     R2,R2,R0
        LDR      R3,??TIM_DMACmd_1  ;; 0xffff8c1c
        STRH     R2,[R3, #+0]
        B        ??TIM_DMACmd_2
//  465   }
//  466   else
//  467   {
//  468     TIM0->RER &= ~TIM_DMASources;
??TIM_DMACmd_0:
        LDR      R2,??TIM_DMACmd_1  ;; 0xffff8c1c
        LDRH     R2,[R2, #+0]
        BICS     R2,R2,R0
        LDR      R3,??TIM_DMACmd_1  ;; 0xffff8c1c
        STRH     R2,[R3, #+0]
//  469   }
//  470 }
??TIM_DMACmd_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TIM_DMACmd_1:
        DC32     0xffff8c1c
        CFI EndBlock cfiBlock7
//  471 
//  472 /*******************************************************************************
//  473 * Function Name  : TIM_ClockSourceConfig
//  474 * Description    : Configures the TIM clock source.
//  475 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  476 *                  - TIM_ClockSource: specifies the TIM clock source to be 
//  477 *                    selected.
//  478 *                    This parameter can be one of the following values:
//  479 *                         - TIM_ClockSource_Internal: CK_TIM internal clock
//  480 *                         - TIM_ClockSource_TI11: External input pin TI1 
//  481 *                           connected to IC1 channel.
//  482 *                         - TIM_ClockSource_TI12: External input pin TI1
//  483 *                           connected to IC2 channel.
//  484 *                         - TIM_ClockSource_TI22: External input pin TI2
//  485 *                           connected to IC2 channel.
//  486 *                         - TIM_ClockSource_TI21: External input pin TI2
//  487 *                           connected to IC1 channel.
//  488 *                  - TIM_ExtCLKEdge: specifies the External input signal edge.
//  489 *                    This parameter can be one of the following values:
//  490 *                         - TIM_ExtCLKEdge_Falling : Falling edge selected.
//  491 *                         - TIM_ExtCLKEdge_Rising : Rising edge selected.
//  492 * Output         : None
//  493 * Return         : None
//  494 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function TIM_ClockSourceConfig
        THUMB
//  495 void TIM_ClockSourceConfig(TIM_TypeDef *TIMx, u16 TIM_ClockSource,
//  496                            u16 TIM_ExtCLKEdge)
//  497 {
TIM_ClockSourceConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  498   if(TIM_ClockSource == TIM_ClockSource_Internal)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+1
        BNE      ??TIM_ClockSourceConfig_0
//  499   {
//  500     /* CK_TIM is used as clock source */
//  501     TIMx->SCR &= TIM_SME_Reset & TIM_SlaveModeSelection_Mask & TIM_TriggerSelection_Mask;
        LDRH     R3,[R0, #+4]
        LDR      R4,??TIM_ClockSourceConfig_1  ;; 0x7003
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+4]
        B        ??TIM_ClockSourceConfig_2
//  502   }
//  503   else
//  504   /* Input Captures are used as TIM external clock */
//  505   {
//  506     TIMx->SCR &= TIM_SME_Reset & TIM_SlaveModeSelection_Mask & TIM_TriggerSelection_Mask;
??TIM_ClockSourceConfig_0:
        LDRH     R3,[R0, #+4]
        LDR      R4,??TIM_ClockSourceConfig_1  ;; 0x7003
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+4]
//  507     TIMx->SCR |= TIM_SMS_EXTCLK_Set | TIM_SME_Set;
        LDRH     R3,[R0, #+4]
        MOVS     R4,#+12
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+4]
//  508 
//  509     if((TIM_ClockSource == TIM_ClockSource_TI11) ||
//  510       (TIM_ClockSource == TIM_ClockSource_TI21))
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+2
        BEQ      ??TIM_ClockSourceConfig_3
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+5
        BNE      ??TIM_ClockSourceConfig_4
//  511     /* Input Capture 1 is selected */
//  512     {
//  513      /* Input capture  Enable */
//  514       TIMx->IMCR |= TIM_IC1_Enable;
??TIM_ClockSourceConfig_3:
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+4
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
//  515       TIMx->SCR |= TIM_TS_IC1_Set;
        LDRH     R3,[R0, #+4]
        MOVS     R4,#+128
        LSLS     R4,R4,#+2        ;; #+512
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+4]
//  516 
//  517       if(TIM_ExtCLKEdge == TIM_ExtCLKEdge_Falling)
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        CMP      R2,#+1
        BNE      ??TIM_ClockSourceConfig_5
//  518       /* Set the corresponding polarity */
//  519       {
//  520         TIMx->IMCR |= TIM_IC1P_Set;
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+8
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
        B        ??TIM_ClockSourceConfig_6
//  521       }
//  522       else
//  523       {   
//  524         TIMx->IMCR &= TIM_IC1P_Reset;
??TIM_ClockSourceConfig_5:
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+55
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+8]
//  525       }
//  526       if(TIM_ClockSource == TIM_ClockSource_TI11)
??TIM_ClockSourceConfig_6:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+2
        BNE      ??TIM_ClockSourceConfig_7
//  527       {
//  528         /* External signal TI1 connected to IC1 channel */
//  529         TIMx->IMCR &= TIM_IC1S_Reset;
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+62
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+8]
        B        ??TIM_ClockSourceConfig_2
//  530       }
//  531       else
//  532       {
//  533         /* External signal TI2 connected to IC1 channel */
//  534         TIMx->IMCR |= TIM_IC1S_Set;
??TIM_ClockSourceConfig_7:
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+1
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
        B        ??TIM_ClockSourceConfig_2
//  535       }
//  536     }
//  537     else
//  538     /* Input Capture 2 is selected */
//  539     {
//  540       /* Input capture  Enable */
//  541       TIMx->IMCR |= TIM_IC2_Enable;
??TIM_ClockSourceConfig_4:
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+16
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
//  542       TIMx->SCR |= TIM_TS_IC2_Set;
        LDRH     R3,[R0, #+4]
        MOVS     R4,#+192
        LSLS     R4,R4,#+2        ;; #+768
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+4]
//  543 
//  544       if(TIM_ExtCLKEdge == TIM_ExtCLKEdge_Falling)
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        CMP      R2,#+1
        BNE      ??TIM_ClockSourceConfig_8
//  545       /* Set the corresponding polarity */
//  546       {
//  547         TIMx->IMCR |= TIM_IC2P_Set;
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+32
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
        B        ??TIM_ClockSourceConfig_9
//  548       }
//  549       else
//  550       {
//  551          TIMx->IMCR &= TIM_IC2P_Reset;
??TIM_ClockSourceConfig_8:
        LDRH     R3,[R0, #+8]
        LSLS     R3,R3,#+27       ;; ZeroExtS R3,R3,#+27,#+27
        LSRS     R3,R3,#+27
        STRH     R3,[R0, #+8]
//  552       }
//  553       if(TIM_ClockSource == TIM_ClockSource_TI22)
??TIM_ClockSourceConfig_9:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+4
        BNE      ??TIM_ClockSourceConfig_10
//  554       {
//  555         /* External signal TI2 connected to IC2 channel */
//  556         TIMx->IMCR &= TIM_IC2S_Reset;
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+61
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+8]
        B        ??TIM_ClockSourceConfig_2
//  557       }
//  558       else
//  559       {
//  560         /* External signal TI1 connected to IC2 channel */
//  561         TIMx->IMCR |= TIM_IC2S_Set;
??TIM_ClockSourceConfig_10:
        LDRH     R3,[R0, #+8]
        MOVS     R4,#+2
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+8]
//  562       }
//  563     }
//  564   }
//  565 }
??TIM_ClockSourceConfig_2:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TIM_ClockSourceConfig_1:
        DC32     0x7003
        CFI EndBlock cfiBlock8
//  566 
//  567 /*******************************************************************************
//  568 * Function Name  : TIM_SetPrescaler
//  569 * Description    : Sets the TIM prescaler value.
//  570 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  571 *                  - Prescaler: TIM prescaler new value.
//  572 * Output         : None
//  573 * Return         : None
//  574 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function TIM_SetPrescaler
        THUMB
//  575 void TIM_SetPrescaler(TIM_TypeDef* TIMx, u16 Prescaler)
//  576 {
//  577   TIMx->PSC = Prescaler;
TIM_SetPrescaler:
        STRH     R1,[R0, #+40]
//  578 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock9
//  579 
//  580 /*******************************************************************************
//  581 * Function Name  : TIM_SetPeriod
//  582 * Description    : Sets the TIM period value.
//  583 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  584 *                  - Period: TIM period new value.
//  585 * Output         : None
//  586 * Return         : None
//  587 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function TIM_SetPeriod
        THUMB
//  588 void TIM_SetPeriod(TIM_TypeDef* TIMx, u16 Period)
//  589 {
//  590   TIMx->ARR = Period;
TIM_SetPeriod:
        STRH     R1,[R0, #+48]
//  591 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock10
//  592 
//  593 /*******************************************************************************
//  594 * Function Name  : TIM_SetPulse
//  595 * Description    : Sets the TIM pulse value.
//  596 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  597 *                  - TIM_Channel: specifies the TIM channel to be used.
//  598 *                    This parameter can be one of the following values:
//  599 *                         - TIM_Channel_1: TIM Channel 1 is used
//  600 *                         - TIM_Channel_2: TIM Channel 2 is used
//  601 *                         - TIM_Channel_ALL: TIM Channel 1and 2 are used
//  602 *                  - Pulse: TIM pulse new value.
//  603 * Output         : None
//  604 * Return         : None
//  605 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function TIM_SetPulse
        THUMB
//  606 void TIM_SetPulse(TIM_TypeDef* TIMx, u16 TIM_Channel, u16 Pulse)
//  607 {
TIM_SetPulse:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  608   /* Set Channel 1 pulse value */
//  609   if(TIM_Channel == TIM_Channel_1)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+1
        BNE      ??TIM_SetPulse_0
//  610   {
//  611     TIMx->OCR1 = Pulse;
        STRH     R2,[R0, #+52]
        B        ??TIM_SetPulse_1
//  612   }
//  613   /* Set Channel 2 pulse value */
//  614   else if(TIM_Channel == TIM_Channel_2)
??TIM_SetPulse_0:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+2
        BNE      ??TIM_SetPulse_2
//  615   {
//  616    TIMx->OCR2 = Pulse;
        STRH     R2,[R0, #+56]
        B        ??TIM_SetPulse_1
//  617   }
//  618   /* Set Channel 1 and Channel 2 pulse values */
//  619   else if(TIM_Channel == TIM_Channel_ALL)
??TIM_SetPulse_2:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+3
        BNE      ??TIM_SetPulse_1
//  620   {
//  621     TIMx->OCR1 = Pulse;
        STRH     R2,[R0, #+52]
//  622     TIMx->OCR2 = Pulse;
        STRH     R2,[R0, #+56]
//  623   }
//  624 }
??TIM_SetPulse_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock11
//  625 
//  626 /*******************************************************************************
//  627 * Function Name  : TIM_GetICAP1
//  628 * Description    : Gets the Input Capture 1 value. 
//  629 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  630 * Output         : None
//  631 * Return         : Input Capture 1 Register value.
//  632 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function TIM_GetICAP1
        THUMB
//  633 u16 TIM_GetICAP1(TIM_TypeDef *TIMx)
//  634 {
//  635   return TIMx->ICR1;
TIM_GetICAP1:
        ADDS     R0,R0,#+76
        LDRH     R0,[R0, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock12
//  636 }
//  637 
//  638 /*******************************************************************************
//  639 * Function Name  : TIM_GetICAP2
//  640 * Description    : Gets the Input Capture 2 value.
//  641 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  642 * Output         : None
//  643 * Return         : Input Capture 2 Register value
//  644 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function TIM_GetICAP2
        THUMB
//  645 u16 TIM_GetICAP2(TIM_TypeDef *TIMx)
//  646 {
//  647   return TIMx->ICR2;
TIM_GetICAP2:
        ADDS     R0,R0,#+80
        LDRH     R0,[R0, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock13
//  648 }
//  649 
//  650 /*******************************************************************************
//  651 * Function Name  : TIM_GetPWMIPulse
//  652 * Description    : Gets the PWM Input pulse value.
//  653 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  654 * Output         : None
//  655 * Return         : Input Capture 2 Register value
//  656 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function TIM_GetPWMIPulse
        THUMB
//  657 u16 TIM_GetPWMIPulse(TIM_TypeDef *TIMx)
//  658 {
//  659   return TIMx->ICR2;
TIM_GetPWMIPulse:
        ADDS     R0,R0,#+80
        LDRH     R0,[R0, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock14
//  660 }
//  661 
//  662 /*******************************************************************************
//  663 * Function Name  : TIM_GetPWMIPeriod
//  664 * Description    : Gets the PWM Input period value.
//  665 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  666 * Output         : None
//  667 * Return         : Input Capture 1 Register value
//  668 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function TIM_GetPWMIPeriod
        THUMB
//  669 u16 TIM_GetPWMIPeriod(TIM_TypeDef *TIMx)
//  670 {
//  671   return TIMx->ICR1;
TIM_GetPWMIPeriod:
        ADDS     R0,R0,#+76
        LDRH     R0,[R0, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock15
//  672 }
//  673 
//  674 /*******************************************************************************
//  675 * Function Name  : TIM_DebugCmd
//  676 * Description    : Enables or disables the specified TIM peripheral Debug control.
//  677 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  678 *                  - Newstate: new state of the TIMx Debug control.
//  679                      This parameter can be: ENABLE or DISABLE.
//  680 * Output         : None
//  681 * Return         : None
//  682 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function TIM_DebugCmd
        THUMB
//  683 void TIM_DebugCmd(TIM_TypeDef *TIMx, FunctionalState Newstate)
//  684 {
TIM_DebugCmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  685   if(Newstate == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??TIM_DebugCmd_0
//  686   {
//  687     TIMx->CR |= TIM_DBGC_Set;
        LDRH     R2,[R0, #+0]
        MOVS     R3,#+128
        LSLS     R3,R3,#+3        ;; #+1024
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+0]
        B        ??TIM_DebugCmd_1
//  688   }
//  689   else
//  690   {
//  691     TIMx->CR &= TIM_DBGC_Reset;
??TIM_DebugCmd_0:
        LDRH     R2,[R0, #+0]
        LDR      R3,??TIM_DebugCmd_2  ;; 0xfb7f
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+0]
//  692   }
//  693 }
??TIM_DebugCmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TIM_DebugCmd_2:
        DC32     0xfb7f
        CFI EndBlock cfiBlock16
//  694 
//  695 /*******************************************************************************
//  696 * Function Name  : TIM_CounterModeConfig
//  697 * Description    : Specifies the Counter Mode to be used.
//  698 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  699 *                  - TIM_CounterMode: specifies the Counter Mode to be used
//  700 *                    This parameter can be one of the following values:
//  701 *                       - TIM_CounterMode_Up: TIM Up Counting Mode
//  702 *                       - TIM_CounterMode_Down: TIM Down Counting Mode
//  703 *                       - TIM_CounterMode_CenterAligned1: TIM Center Aligned Mode1
//  704 *                       - TIM_CounterMode_CenterAligned2: TIM Center Aligned Mode2
//  705 *                       - TIM_CounterMode_CenterAligned3: TIM Center Aligned Mode3
//  706 * Output         : None
//  707 * Return         : None
//  708 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function TIM_CounterModeConfig
        THUMB
//  709 void TIM_CounterModeConfig(TIM_TypeDef* TIMx, u16 TIM_CounterMode)
//  710 {
//  711   /* Counter mode configuration */
//  712   TIMx->CR &= TIM_CounterMode_Mask;
TIM_CounterModeConfig:
        LDRH     R2,[R0, #+0]
        LDR      R3,??TIM_CounterModeConfig_0  ;; 0xff8f
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+0]
//  713   TIMx->CR |= TIM_CounterMode;
        LDRH     R2,[R0, #+0]
        ORRS     R2,R2,R1
        STRH     R2,[R0, #+0]
//  714 }
        BX       LR               ;; return
        DATA
??TIM_CounterModeConfig_0:
        DC32     0xff8f
        CFI EndBlock cfiBlock17
//  715 
//  716 /*******************************************************************************
//  717 * Function Name  : TIM_ForcedOCConfig
//  718 * Description    : Forces the TIM output waveform to active or inactive level.
//  719 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  720 *                  - TIM_Channel: specifies the TIM channel to be used.
//  721 *                    This parameter can be one of the following values:
//  722 *                       - TIM_Channel_1: Timer Channel 1 is used
//  723 *                       - TIM_Channel_2: Timer Channel 2 is used
//  724 *                       - TIM_Channel_ALL: Timer Channel 1 and 2 are used
//  725 *                 - TIM_ForcedAction: specifies the forced Action to be set to
//  726 *                  the output waveform.
//  727 *                    This parameter can be one of the following values:
//  728 *                       - TIM_ForcedAction_Active: Force active level on OCxREF
//  729 *                       - TIM_ForcedAction_InActive: Force inactive level on 
//  730 *                         OCxREF.
//  731 * Output         : None
//  732 * Return         : None
//  733 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function TIM_ForcedOCConfig
        THUMB
//  734 void TIM_ForcedOCConfig(TIM_TypeDef* TIMx, u16 TIM_Channel,u16 TIM_ForcedAction)
//  735 {
TIM_ForcedOCConfig:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  736   /* Channel 1 Forced Output Compare mode configuration */
//  737   if(TIM_Channel == TIM_Channel_1)
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+1
        BNE      ??TIM_ForcedOCConfig_0
//  738   {
//  739     TIMx->OMR1 &= TIM_OC1C_Mask;
        LDRH     R3,[R0, #+12]
        LDR      R4,??TIM_ForcedOCConfig_1  ;; 0x3f31
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
//  740     TIMx->OMR1 |= TIM_ForcedAction;
        LDRH     R3,[R0, #+12]
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+12]
        B        ??TIM_ForcedOCConfig_2
//  741   }
//  742   /* Channel 2 Forced Output Compare mode configuration */
//  743   else
//  744   {
//  745     if(TIM_Channel == TIM_Channel_2)
??TIM_ForcedOCConfig_0:
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+2
        BNE      ??TIM_ForcedOCConfig_3
//  746     {
//  747       TIMx->OMR1 &= TIM_OC2C_Mask;
        LDRH     R3,[R0, #+12]
        LDR      R4,??TIM_ForcedOCConfig_1+0x4  ;; 0x313f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
//  748       TIMx->OMR1 |= (TIM_ForcedAction<<8);
        LDRH     R3,[R0, #+12]
        LSLS     R4,R2,#+8
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??TIM_ForcedOCConfig_2
//  749     }
//  750     /* Channel 1 and Channel 2 Forced Output Compare mode configuration */
//  751     else
//  752     {
//  753       TIMx->OMR1 &= TIM_OC1C_Mask & TIM_OC2C_Mask;
??TIM_ForcedOCConfig_3:
        LDRH     R3,[R0, #+12]
        LDR      R4,??TIM_ForcedOCConfig_1+0x8  ;; 0x3131
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
//  754       TIMx->OMR1 |= TIM_ForcedAction |(TIM_ForcedAction<<8);
        LDRH     R3,[R0, #+12]
        LSLS     R4,R2,#+8
        ORRS     R4,R4,R2
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
//  755     }
//  756   }
//  757 }
??TIM_ForcedOCConfig_2:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??TIM_ForcedOCConfig_1:
        DC32     0x3f31
        DC32     0x313f
        DC32     0x3131
        CFI EndBlock cfiBlock18
//  758 
//  759 /*******************************************************************************
//  760 * Function Name  : TIM_ResetCounter
//  761 * Description    : Re-intializes the TIM counter and generates an update of the
//  762 *                  registers.
//  763 * Input          : TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
//  764 * Output         : None
//  765 * Return         : None
//  766 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon0
        CFI Function TIM_ResetCounter
        THUMB
//  767 void TIM_ResetCounter(TIM_TypeDef* TIMx)
//  768 {
//  769   /* Re-intialize the TIM counter */
//  770   TIMx->CR |= TIM_COUNTER_Reset;
TIM_ResetCounter:
        LDRH     R1,[R0, #+0]
        MOVS     R2,#+2
        ORRS     R2,R2,R1
        STRH     R2,[R0, #+0]
//  771 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock19
//  772 
//  773 /*******************************************************************************
//  774 * Function Name  : TIM_SynchroConfig
//  775 * Description    : Synchronizes two Timers in a specified mode.
//  776 * Input          : - Master: specifies the peripheral master.
//  777 *                    This parameter can be one of the following values:
//  778 *                    PWM_Master, TIM0_Master, TIM1_Master or TIM2_Master.
//  779 *                  - Slave: specifies the peripheral slave.
//  780 *                    This parameter can be one of the following values:
//  781 *                    PWM_Slave, TIM0_Slave, TIM1_Slave or TIM2_Slave.
//  782 *                  - TIM_SynchroAction: specifies the synchronization Action to 
//  783 *                    be used.
//  784 *                    This parameter can be one of the following values:
//  785 *                         - TIM_SynchroAction_Enable: The CNT_EN bit is used as TRGO
//  786 *                         - TIM_SynchroAction_Update: The Update event is used as TRGO
//  787 *                         - TIM_SynchroAction_Reset: The CNT_RST bit is used as TRGO
//  788 *                         - TIM_SynchroAction_OC: The OC1 signal is used as TRGO
//  789 *                  - TIM_SynchroMode: specifies the synchronization Mode to be used.
//  790 *                    This parameter can be one of the following values:
//  791 *                         - TIM_SynchroMode_Gated: Both start and stop of the 
//  792 *                           counter is controlled.
//  793 *                         - TIM_SynchroMode_Trigger: Only the start of the 
//  794 *                           counter is controlled.
//  795 *                         - TIM_SynchroMode_External: The rising edge of selected trigger 
//  796 *                           clocks the counter.
//  797 *                         - TIM_SynchroMode_Reset: The rising edge of the selected trigger 
//  798 *                           signal resets the counter and generates an update of the registers.
//  799 * Output         : None
//  800 * Return         : None
//  801 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock20 Using cfiCommon0
        CFI Function TIM_SynchroConfig
        THUMB
//  802 void TIM_SynchroConfig(Master_TypeDef Master, Slave_TypeDef Slave,
//  803                        u16 TIM_SynchroAction, u16 TIM_SynchroMode)
//  804 {
TIM_SynchroConfig:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
//  805   switch (Slave)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R4,R1
        SUBS     R4,R4,#+5
        BEQ      ??TIM_SynchroConfig_0
        SUBS     R4,R4,#+1
        BEQ      ??TIM_SynchroConfig_1
        SUBS     R4,R4,#+1
        BNE      .+4
        B        ??TIM_SynchroConfig_2
        SUBS     R4,R4,#+1
        BNE      .+4
        B        ??TIM_SynchroConfig_3
        B        ??TIM_SynchroConfig_4
//  806   {
//  807     case PWM_Slave:
//  808     {
//  809       PWM->SCR &= TIM_SME_Reset & TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask &
//  810                   TIM_InternalTriggerSelection_Mask;
??TIM_SynchroConfig_0:
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        LDRH     R4,[R4, #+0]
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        LDR      R5,??TIM_SynchroConfig_5  ;; 0xffff9804
        STRH     R4,[R5, #+0]
//  811       PWM->SCR |= TIM_SynchroMode | TIM_SME_Set;
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+4
        ORRS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        STRH     R5,[R4, #+0]
//  812 
//  813       if(Master == TIM1_Master)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??TIM_SynchroConfig_6
//  814       {
//  815         /* Set the internal trigger */
//  816       	PWM->SCR |= TIM_ITS_TIM1;
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+6        ;; #+8192
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        STRH     R5,[R4, #+0]
//  817 
//  818         /* Set the synchronization action */
//  819         TIM1->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R5,[R4, #+0]
//  820         TIM1->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  821       }
//  822 
//  823       else if(Master == TIM0_Master)
??TIM_SynchroConfig_6:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+2
        BNE      ??TIM_SynchroConfig_7
//  824       {
//  825         /* Set the internal trigger */
//  826         PWM->SCR |= TIM_ITS_TIM0;
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+5        ;; #+4096
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        STRH     R5,[R4, #+0]
//  827 
//  828         /* Set the synchronization action */
//  829         TIM0->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R5,[R4, #+0]
//  830         TIM0->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  831       }
//  832 
//  833       else if(Master == TIM2_Master)
??TIM_SynchroConfig_7:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+4
        BEQ      .+4
        B        ??TIM_SynchroConfig_4
//  834       {
//  835         /* Set the internal trigger */
//  836         PWM->SCR |= TIM_ITS_TIM2;
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+192
        LSLS     R5,R5,#+6        ;; #+12288
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5  ;; 0xffff9804
        STRH     R5,[R4, #+0]
//  837 
//  838         /* Set the synchronization action */
//  839         TIM2->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R5,[R4, #+0]
//  840         TIM2->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  841       }
//  842     }
//  843     break;
//  844 
//  845     case TIM0_Slave:
//  846     {
//  847       TIM0->SCR &= TIM_SME_Reset & TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask &
//  848                    TIM_InternalTriggerSelection_Mask;
??TIM_SynchroConfig_1:
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        LDRH     R4,[R4, #+0]
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        LDR      R5,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        STRH     R4,[R5, #+0]
//  849       TIM0->SCR |= TIM_SynchroMode | TIM_SME_Set;
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+4
        ORRS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        STRH     R5,[R4, #+0]
//  850 
//  851       if(Master == PWM_Master)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??TIM_SynchroConfig_8
//  852       {
//  853         /* Set the internal trigger */
//  854         TIM0->SCR |= TIM_ITS_PWM;
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+7        ;; #+16384
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        STRH     R5,[R4, #+0]
//  855 
//  856         /* Set the synchronization action */
//  857         PWM->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R5,[R4, #+0]
//  858         PWM->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  859       }
//  860 
//  861       else if(Master == TIM1_Master)
??TIM_SynchroConfig_8:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??TIM_SynchroConfig_9
//  862       {
//  863         /* Set the internal trigger */
//  864         TIM0->SCR |= TIM_ITS_TIM1;
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+6        ;; #+8192
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        STRH     R5,[R4, #+0]
//  865 
//  866         /* Set the synchronization action */
//  867         TIM1->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R5,[R4, #+0]
//  868         TIM1->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  869       }
//  870 
//  871       else if(Master == TIM2_Master)
??TIM_SynchroConfig_9:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+4
        BEQ      .+4
        B        ??TIM_SynchroConfig_4
//  872       {
//  873         /* Set the internal trigger */
//  874         TIM0->SCR |= TIM_ITS_TIM2;
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+192
        LSLS     R5,R5,#+6        ;; #+12288
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x14  ;; 0xffff8c04
        STRH     R5,[R4, #+0]
//  875 
//  876         /* Set the synchronization action */
//  877         TIM2->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R5,[R4, #+0]
//  878         TIM2->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  879       }
//  880     }
//  881     break;
//  882 
//  883     case TIM1_Slave:
//  884     {
//  885 
//  886       TIM1->SCR &= TIM_SME_Reset & TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask &
//  887                    TIM_InternalTriggerSelection_Mask;
??TIM_SynchroConfig_2:
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        LDRH     R4,[R4, #+0]
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        LDR      R5,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        STRH     R4,[R5, #+0]
//  888       TIM1->SCR |= TIM_SynchroMode | TIM_SME_Set;
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+4
        ORRS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        STRH     R5,[R4, #+0]
//  889      
//  890       if(Master == PWM_Master)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??TIM_SynchroConfig_10
//  891       {
//  892       	 /* Set the internal trigger */
//  893       	 TIM1->SCR |= TIM_ITS_PWM;
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+7        ;; #+16384
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        STRH     R5,[R4, #+0]
//  894 
//  895         /* Set the synchronization action */
//  896         PWM->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R5,[R4, #+0]
//  897         PWM->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  898       }
//  899       else if(Master == TIM0_Master)
??TIM_SynchroConfig_10:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+2
        BNE      ??TIM_SynchroConfig_11
//  900       {
//  901         /* Set the internal trigger */
//  902         TIM1->SCR |= TIM_ITS_TIM0;
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+5        ;; #+4096
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        STRH     R5,[R4, #+0]
//  903 
//  904         /* Set the synchronization action */
//  905         TIM0->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R5,[R4, #+0]
//  906         TIM0->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  907       }
//  908 
//  909       else if(Master == TIM2_Master)
??TIM_SynchroConfig_11:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+4
        BNE      ??TIM_SynchroConfig_4
//  910       {
//  911         /* Set the internal trigger */
//  912         TIM1->SCR |= TIM_ITS_TIM2;
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+192
        LSLS     R5,R5,#+6        ;; #+12288
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x1C  ;; 0xffff9004
        STRH     R5,[R4, #+0]
//  913 
//  914         /* Set the synchronization action */
//  915         TIM2->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R5,[R4, #+0]
//  916         TIM2->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x10  ;; 0xffff9400
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  917       }
//  918     }
//  919     break;
//  920 
//  921     case TIM2_Slave:
//  922     {
//  923      
//  924       TIM2->SCR &= TIM_SME_Reset & TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask &
//  925                    TIM_InternalTriggerSelection_Mask;
??TIM_SynchroConfig_3:
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        LDRH     R4,[R4, #+0]
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        LDR      R5,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        STRH     R4,[R5, #+0]
//  926       TIM2->SCR |= TIM_SynchroMode | TIM_SME_Set;
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+4
        ORRS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        STRH     R5,[R4, #+0]
//  927 
//  928       if(Master == PWM_Master)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??TIM_SynchroConfig_12
//  929       {
//  930         /* Internal trigger selection */
//  931         TIM2->SCR |= TIM_ITS_PWM;
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+7        ;; #+16384
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        STRH     R5,[R4, #+0]
//  932 
//  933         /* Set the synchronization action */
//  934         PWM->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R5,[R4, #+0]
//  935         PWM->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x18  ;; 0xffff9800
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  936       }
//  937 
//  938       else if(Master == TIM1_Master)
??TIM_SynchroConfig_12:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??TIM_SynchroConfig_13
//  939       {
//  940         /* Internal trigger selection */
//  941         TIM2->SCR |= TIM_ITS_TIM1;
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+6        ;; #+8192
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        STRH     R5,[R4, #+0]
//  942 
//  943         /* Set the synchronization action */
//  944         TIM1->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R5,[R4, #+0]
//  945         TIM1->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0x4  ;; 0xffff9000
        STRH     R4,[R5, #+0]
        B        ??TIM_SynchroConfig_4
//  946       }
//  947 
//  948       else if(Master == TIM0_Master)
??TIM_SynchroConfig_13:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+2
        BNE      ??TIM_SynchroConfig_4
//  949       {
//  950         /* Internal trigger selection */
//  951         TIM2->SCR |= TIM_ITS_TIM0;
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        LDRH     R4,[R4, #+0]
        MOVS     R5,#+128
        LSLS     R5,R5,#+5        ;; #+4096
        ORRS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0x20  ;; 0xffff9404
        STRH     R5,[R4, #+0]
//  952 
//  953         /* Set the synchronization action */
//  954         TIM0->CR &= TIM_MasterModeSelection_Mask;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        LDR      R5,??TIM_SynchroConfig_5+0x8  ;; 0xfc7f
        ANDS     R5,R5,R4
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R5,[R4, #+0]
//  955         TIM0->CR |= TIM_SynchroAction;
        LDR      R4,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        LDRH     R4,[R4, #+0]
        ORRS     R4,R4,R2
        LDR      R5,??TIM_SynchroConfig_5+0xC  ;; 0xffff8c00
        STRH     R4,[R5, #+0]
//  956       }
//  957     }
//  958     break;
//  959 
//  960     default:
//  961     break;
//  962   }
//  963 }
??TIM_SynchroConfig_4:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??TIM_SynchroConfig_5:
        DC32     0xffff9804
        DC32     0xffff9000
        DC32     0xfc7f
        DC32     0xffff8c00
        DC32     0xffff9400
        DC32     0xffff8c04
        DC32     0xffff9800
        DC32     0xffff9004
        DC32     0xffff9404
        CFI EndBlock cfiBlock20
//  964 
//  965 /*******************************************************************************
//  966 * Function Name  : TIM_GetFlagStatus
//  967 * Description    : Checks whether the specified TIM flag is set or not.
//  968 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  969 *                  - TIM_FLAG: specifies the flag to check. 
//  970 *                    This parameter can be one of the following values:
//  971 *                         - TIM_FLAG_IC1: Input Capture 1 Flag
//  972 *                         - TIM_FLAG_OC1: Output Compare 1 Flag
//  973 *                         - TIM_FLAG_Update: Timer update Flag
//  974 *                         - TIM_FLAG_IC2: Input Capture 2 Flag
//  975 *                         - TIM_FLAG_OC2: Output Compare 2 Flag
//  976 * Output         : None
//  977 * Return         : The new state of TIM_FLAG (SET or RESET).
//  978 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock21 Using cfiCommon0
        CFI Function TIM_GetFlagStatus
        THUMB
//  979 FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, u16 TIM_FLAG)
//  980 {
TIM_GetFlagStatus:
        MOVS     R2,R0
//  981   if((TIMx->ISR & TIM_FLAG) != RESET )
        LDRH     R3,[R2, #+32]
        ANDS     R3,R3,R1
        MOVS     R0,R3
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  982   {
//  983     return SET;
//  984   }
//  985   else
//  986   {
//  987     return RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock21
//  988   }
//  989 }
//  990 
//  991 /*******************************************************************************
//  992 * Function Name  : TIM_ClearFlag
//  993 * Description    : Clears the TIMx's pending flags.
//  994 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
//  995 *                  - TIM_FLAG: specifies the flag bit to clear.
//  996 *                    This parameter can be any combination of the following values:
//  997 *                         - TIM_FLAG_IC1: Timer Input Capture 1 flag
//  998 *                         - TIM_FLAG_OC1: Timer Output Compare 1 flag
//  999 *                         - TIM_FLAG_Update: Timer update flag
// 1000 *                         - TIM_FLAG_IC2: Timer Input Capture 2 flag
// 1001 *                         - TIM_FLAG_OC2: Timer Output Compare 2 flag
// 1002 * Output         : None
// 1003 * Return         : None
// 1004 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock22 Using cfiCommon0
        CFI Function TIM_ClearFlag
        THUMB
// 1005 void TIM_ClearFlag(TIM_TypeDef* TIMx, u16 TIM_FLAG)
// 1006 {
// 1007   /* Clear the flags */
// 1008   TIMx->ISR &= ~TIM_FLAG;
TIM_ClearFlag:
        LDRH     R2,[R0, #+32]
        BICS     R2,R2,R1
        STRH     R2,[R0, #+32]
// 1009 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock22
// 1010 
// 1011 /*******************************************************************************
// 1012 * Function Name  : TIM_GetITStatus
// 1013 * Description    : Checks whether the specified TIM interrupt has occurred or not.
// 1014 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
// 1015 *                  - TIM_IT: specifies the TIM interrupt source to check.
// 1016 *                    This parameter can be one of the following values:
// 1017 *                         - TIM_IT_IC1: Input Capture 1 interrupt
// 1018 *                         - TIM_IT_OC1: Output Compare 1 interrupt
// 1019 *                         - TIM_IT_Update: Timer update interrupt
// 1020 *                         - TIM_IT_GlobalUpdate: Timer global update interrupt
// 1021 *                         - TIM_IT_IC2: Input Capture 2 interrupt
// 1022 *                         - TIM_IT_OC2: Output Compare 2 interrupt
// 1023 * Output         : None
// 1024 * Return         : The new state of TIM_IT(SET or RESET).
// 1025 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock23 Using cfiCommon0
        CFI Function TIM_GetITStatus
        THUMB
// 1026 ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, u16 TIM_IT)
// 1027 {
TIM_GetITStatus:
        PUSH     {R0,R4}
        CFI R4 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
// 1028   u16 TIM_IT_Check = 0;
        MOVS     R0,#+0
        MOVS     R3,R0
// 1029 
// 1030   /* Calculates the pending bits to be checked */
// 1031   TIM_IT_Check = TIM_IT & TIM_IT_Clear_Mask;
        LSLS     R0,R1,#+17       ;; ZeroExtS R0,R1,#+17,#+17
        LSRS     R0,R0,#+17
        MOVS     R3,R0
// 1032   
// 1033   if((TIMx->ISR & TIM_IT_Check) != RESET )
        LDRH     R4,[R2, #+32]
        ANDS     R4,R4,R3
        MOVS     R0,R4
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
// 1034   {
// 1035     return SET;
// 1036   }
// 1037   else
// 1038   {
// 1039     return RESET;
        POP      {R3,R4}
        CFI R4 SameValue
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock23
// 1040   }
// 1041 }
// 1042 
// 1043 /*******************************************************************************
// 1044 * Function Name  : TIM_ClearITPendingBit
// 1045 * Description    : Clears the TIM's interrupt pending bits.
// 1046 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral.
// 1047 *                  - TIM_IT: specifies the interrupt pending bit to clear.
// 1048 *                    This parameter can be one of the following values:
// 1049 *                         - TIM_IT_IC1: Input Capture 1 Interrupt 
// 1050 *                         - TIM_IT_OC1: Output Compare 1 Interrupt 
// 1051 *                         - TIM_IT_Update: Timer update Interrupt 
// 1052 *                         - TIM_IT_GlobalUpdate: Timer global update Interrupt 
// 1053 *                         - TIM_IT_IC2: Input Capture 2 Interrupt 
// 1054 *                         - TIM_IT_OC2: Output Compare 2 Interrupt 
// 1055 * Output         : None
// 1056 * Return         : None
// 1057 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock24 Using cfiCommon0
        CFI Function TIM_ClearITPendingBit
        THUMB
// 1058 void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, u16 TIM_IT)
// 1059 {
// 1060   u16 TIM_IT_Clear = 0;
TIM_ClearITPendingBit:
        MOVS     R3,#+0
        MOVS     R2,R3
// 1061 
// 1062   /* Calculate the pending bits to be cleared */
// 1063   TIM_IT_Clear = TIM_IT & TIM_IT_Clear_Mask;
        LSLS     R3,R1,#+17       ;; ZeroExtS R3,R1,#+17,#+17
        LSRS     R3,R3,#+17
        MOVS     R2,R3
// 1064 
// 1065   /* Clear the pending bits */
// 1066   TIMx->ISR &= ~TIM_IT_Clear;
        LDRH     R3,[R0, #+32]
        BICS     R3,R3,R2
        STRH     R3,[R0, #+32]
// 1067 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock24
// 1068 
// 1069 /*******************************************************************************
// 1070 * Function Name  : OCM_ModuleConfig
// 1071 * Description    : Output Compare Module configuration
// 1072 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
// 1073 *                  - TIM_InitStruct: pointer to a TIM_InitTypeDef structure that
// 1074 *                  contains the configuration information for the specified TIM
// 1075 *                  peripheral.
// 1076 * Output         : None
// 1077 * Return         : None
// 1078 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock25 Using cfiCommon0
        CFI Function OCM_ModuleConfig
        THUMB
// 1079 static void OCM_ModuleConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct)
// 1080 {
OCM_ModuleConfig:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
// 1081   u16 TIM_OCControl = 0x0000;
        MOVS     R3,#+0
        MOVS     R2,R3
// 1082 
// 1083   if(TIM_InitStruct->TIM_Mode == TIM_Mode_OCTiming)
        LDRH     R3,[R1, #+0]
        CMP      R3,#+1
        BNE      ??OCM_ModuleConfig_0
// 1084   {
// 1085     TIM_OCControl = TIM_OCControl_OCTiming;
        MOVS     R3,#+0
        MOVS     R2,R3
        B        ??OCM_ModuleConfig_1
// 1086   }
// 1087   else
// 1088   {
// 1089     if((TIM_InitStruct->TIM_Mode == TIM_Mode_OCActive) || 
// 1090        (TIM_InitStruct->TIM_Mode == TIM_Mode_OPM_Active))
??OCM_ModuleConfig_0:
        LDRH     R3,[R1, #+0]
        CMP      R3,#+2
        BEQ      ??OCM_ModuleConfig_2
        LDRH     R3,[R1, #+0]
        CMP      R3,#+13
        BNE      ??OCM_ModuleConfig_3
// 1091     {
// 1092       TIM_OCControl = TIM_OCControl_OCActive;
??OCM_ModuleConfig_2:
        MOVS     R3,#+2
        MOVS     R2,R3
        B        ??OCM_ModuleConfig_1
// 1093     }
// 1094     else
// 1095     {
// 1096       if(TIM_InitStruct->TIM_Mode == TIM_Mode_OCInactive)
??OCM_ModuleConfig_3:
        LDRH     R3,[R1, #+0]
        CMP      R3,#+3
        BNE      ??OCM_ModuleConfig_4
// 1097       {
// 1098         TIM_OCControl = TIM_OCControl_OCInactive;
        MOVS     R3,#+4
        MOVS     R2,R3
        B        ??OCM_ModuleConfig_1
// 1099       }
// 1100       else
// 1101       {
// 1102       	 if((TIM_InitStruct->TIM_Mode == TIM_Mode_OCToggle) ||
// 1103             (TIM_InitStruct->TIM_Mode == TIM_Mode_OPM_Toggle))
??OCM_ModuleConfig_4:
        LDRH     R3,[R1, #+0]
        CMP      R3,#+4
        BEQ      ??OCM_ModuleConfig_5
        LDRH     R3,[R1, #+0]
        CMP      R3,#+12
        BNE      ??OCM_ModuleConfig_6
// 1104         {
// 1105           TIM_OCControl = TIM_OCControl_OCToggle;
??OCM_ModuleConfig_5:
        MOVS     R3,#+6
        MOVS     R2,R3
        B        ??OCM_ModuleConfig_1
// 1106         }
// 1107         else
// 1108         {
// 1109           TIM_OCControl = TIM_OCControl_PWM;
??OCM_ModuleConfig_6:
        MOVS     R3,#+12
        MOVS     R2,R3
// 1110 
// 1111         }
// 1112       }
// 1113     }
// 1114   }
// 1115 
// 1116   if(TIM_InitStruct->TIM_Channel == TIM_Channel_1)
??OCM_ModuleConfig_1:
        LDRH     R3,[R1, #+12]
        CMP      R3,#+1
        BNE      ??OCM_ModuleConfig_7
// 1117   {
// 1118     /* Configure Channel 1 on Output Compare mode */
// 1119     TIMx->OMR1 &= TIM_OC1C_Mask;
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8  ;; 0x3f31
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1120     TIMx->OMR1 |= TIM_OCControl|TIM_OC1_Enable;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+16
        ORRS     R4,R4,R2
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1121     TIMx->OMR1 |= TIM_PLD1_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+1
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1122     TIMx->OCR1 = TIM_InitStruct->TIM_Pulse1;
        LDRH     R3,[R1, #+14]
        STRH     R3,[R0, #+52]
// 1123 
// 1124     /* Set the OC1 wave polarity */
// 1125     if(TIM_InitStruct->TIM_Polarity1 == TIM_Polarity1_Low)
        LDRH     R3,[R1, #+20]
        CMP      R3,#+2
        BNE      ??OCM_ModuleConfig_9
// 1126     {
// 1127       TIMx->OMR1 |= TIM_OC1P_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+32
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_10
// 1128     }
// 1129     else
// 1130     {
// 1131       TIMx->OMR1 &= TIM_OC1P_Reset;
??OCM_ModuleConfig_9:
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0x4  ;; 0x3f1f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_10
// 1132     }
// 1133   }
// 1134   else
// 1135   {
// 1136     if(TIM_InitStruct->TIM_Channel == TIM_Channel_2)
??OCM_ModuleConfig_7:
        LDRH     R3,[R1, #+12]
        CMP      R3,#+2
        BNE      ??OCM_ModuleConfig_11
// 1137     {
// 1138       /* Configure Channel 2 on Output Compare mode */
// 1139       TIMx->OMR1 &= TIM_OC2C_Mask;
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0x8  ;; 0x313f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1140       TIMx->OMR1 |= TIM_OCControl<<8|TIM_OC2_Enable;
        LDRH     R3,[R0, #+12]
        LSLS     R4,R2,#+8
        MOVS     R5,#+128
        LSLS     R5,R5,#+5        ;; #+4096
        ORRS     R5,R5,R4
        ORRS     R5,R5,R3
        STRH     R5,[R0, #+12]
// 1141       TIMx->OMR1 |= TIM_PLD2_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+128
        LSLS     R4,R4,#+1        ;; #+256
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1142       TIMx->OCR2 = TIM_InitStruct->TIM_Pulse2;
        LDRH     R3,[R1, #+16]
        STRH     R3,[R0, #+56]
// 1143 
// 1144       /* Set the OCB wave polarity */
// 1145       if(TIM_InitStruct->TIM_Polarity2 == TIM_Polarity2_Low)
        LDRH     R3,[R1, #+22]
        CMP      R3,#+2
        BNE      ??OCM_ModuleConfig_12
// 1146       {
// 1147         TIMx->OMR1 |= TIM_OC2P_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+128
        LSLS     R4,R4,#+6        ;; #+8192
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_10
// 1148       }
// 1149       else
// 1150       {
// 1151         TIMx->OMR1 &= TIM_OC2P_Reset;
??OCM_ModuleConfig_12:
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0xC  ;; 0x1f3f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_10
// 1152       }
// 1153     }
// 1154      /* Configure Channel 1 and Channel 2 on Output Compare mode */
// 1155     else
// 1156     {
// 1157       TIMx->OMR1 &= TIM_OC1C_Mask & TIM_OC2C_Mask; 
??OCM_ModuleConfig_11:
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0x10  ;; 0x3131
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1158       TIMx->OMR1 |= TIM_OCControl|(TIM_OCControl<<8)|TIM_OC1_Enable|TIM_OC2_Enable|
// 1159                    TIM_PLD1_Set|TIM_PLD2_Set;
        LDRH     R3,[R0, #+12]
        LSLS     R4,R2,#+8
        ORRS     R4,R4,R2
        LDR      R5,??OCM_ModuleConfig_8+0x14  ;; 0x1111
        ORRS     R5,R5,R4
        ORRS     R5,R5,R3
        STRH     R5,[R0, #+12]
// 1160 
// 1161       TIMx->OCR1 = TIM_InitStruct->TIM_Pulse1;
        LDRH     R3,[R1, #+14]
        STRH     R3,[R0, #+52]
// 1162       TIMx->OCR2 = TIM_InitStruct->TIM_Pulse2;
        LDRH     R3,[R1, #+16]
        STRH     R3,[R0, #+56]
// 1163 
// 1164       /* Set the OC1 wave polarity */
// 1165       if(TIM_InitStruct->TIM_Polarity1 == TIM_Polarity1_Low)
        LDRH     R3,[R1, #+20]
        CMP      R3,#+2
        BNE      ??OCM_ModuleConfig_13
// 1166       {
// 1167         TIMx->OMR1 |= TIM_OC1P_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+32
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_14
// 1168       }
// 1169       else
// 1170       {
// 1171         TIMx->OMR1 &= TIM_OC1P_Reset;
??OCM_ModuleConfig_13:
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0x4  ;; 0x3f1f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1172       }
// 1173 
// 1174       /* Set the OC2 wave polarity */
// 1175       if(TIM_InitStruct->TIM_Polarity2 == TIM_Polarity2_Low)
??OCM_ModuleConfig_14:
        LDRH     R3,[R1, #+22]
        CMP      R3,#+2
        BNE      ??OCM_ModuleConfig_15
// 1176       {
// 1177         TIMx->OMR1 |= TIM_OC2P_Set;
        LDRH     R3,[R0, #+12]
        MOVS     R4,#+128
        LSLS     R4,R4,#+6        ;; #+8192
        ORRS     R4,R4,R3
        STRH     R4,[R0, #+12]
        B        ??OCM_ModuleConfig_10
// 1178       }
// 1179       else
// 1180       {
// 1181         TIMx->OMR1 &= TIM_OC2P_Reset;
??OCM_ModuleConfig_15:
        LDRH     R3,[R0, #+12]
        LDR      R4,??OCM_ModuleConfig_8+0xC  ;; 0x1f3f
        ANDS     R4,R4,R3
        STRH     R4,[R0, #+12]
// 1182       }
// 1183     }
// 1184   }
// 1185 }
??OCM_ModuleConfig_10:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??OCM_ModuleConfig_8:
        DC32     0x3f31
        DC32     0x3f1f
        DC32     0x313f
        DC32     0x1f3f
        DC32     0x3131
        DC32     0x1111
        CFI EndBlock cfiBlock25
// 1186 
// 1187 /*******************************************************************************
// 1188 * Function Name  : ICAP_ModuleConfig
// 1189 * Description    : Input Capture Module configuration
// 1190 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
// 1191 *                  - TIM_InitStruct: pointer to a TIM_InitTypeDef structure that
// 1192 *                  contains the configuration information for the specified TIM
// 1193 *                  peripheral.
// 1194 * Output         : None
// 1195 * Return         : None
// 1196 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock26 Using cfiCommon0
        CFI Function ICAP_ModuleConfig
        THUMB
// 1197 static void ICAP_ModuleConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct)
// 1198 {
ICAP_ModuleConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1199   if(TIM_InitStruct->TIM_Mode == TIM_Mode_PWMI)
        LDRH     R2,[R1, #+0]
        CMP      R2,#+6
        BNE      ??ICAP_ModuleConfig_0
// 1200   { /* PWM input mode configuration */
// 1201     TIMx->SCR |= TIM_TS_IC1_Set|TIM_SMS_RESETCLK_Set|TIM_SME_Set;
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+129
        LSLS     R3,R3,#+2        ;; #+516
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1202 
// 1203     /* Channel 1 and channel 2 input selection */
// 1204     if(TIM_InitStruct->TIM_PWMI_ICSelection == TIM_PWMI_ICSelection_TI1)
        LDRH     R2,[R1, #+32]
        CMP      R2,#+1
        BNE      ??ICAP_ModuleConfig_1
// 1205     {
// 1206       TIMx->IMCR &= TIM_IC1S_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+62
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1207       TIMx->IMCR |= TIM_IC2S_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+2
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_2
// 1208     }
// 1209     else
// 1210     {
// 1211       TIMx->IMCR |= TIM_IC1S_Set;
??ICAP_ModuleConfig_1:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1212       TIMx->IMCR &= TIM_IC2S_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+61
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1213     }
// 1214 
// 1215     /* Channel polarity */
// 1216     if(TIM_InitStruct->TIM_PWMI_ICPolarity == TIM_PWMI_ICPolarity_Rising)
??ICAP_ModuleConfig_2:
        LDRH     R2,[R1, #+34]
        CMP      R2,#+4
        BNE      ??ICAP_ModuleConfig_3
// 1217     {
// 1218       TIMx->IMCR &= TIM_IC1P_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+55
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1219       TIMx->IMCR |= TIM_IC2P_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+32
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_4
// 1220     }
// 1221     else
// 1222     {
// 1223       TIMx->IMCR |= TIM_IC1P_Set;
??ICAP_ModuleConfig_3:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+8
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1224       TIMx->IMCR &= TIM_IC2P_Reset;
        LDRH     R2,[R0, #+8]
        LSLS     R2,R2,#+27       ;; ZeroExtS R2,R2,#+27,#+27
        LSRS     R2,R2,#+27
        STRH     R2,[R0, #+8]
// 1225     }
// 1226 
// 1227     /* Input capture  Enable */
// 1228     TIMx->IMCR |= TIM_IC1_Enable |TIM_IC2_Enable;
??ICAP_ModuleConfig_4:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+20
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_5
// 1229   }
// 1230   else
// 1231   {
// 1232     if(TIM_InitStruct->TIM_Channel == TIM_Channel_1)
??ICAP_ModuleConfig_0:
        LDRH     R2,[R1, #+12]
        CMP      R2,#+1
        BNE      ??ICAP_ModuleConfig_6
// 1233     {
// 1234       /* Input Capture 1 mode configuration */
// 1235       TIMx->SCR &= TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask;
        LDRH     R2,[R0, #+4]
        LDR      R3,??ICAP_ModuleConfig_7  ;; 0x7007
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1236       TIMx->SCR |= TIM_TS_IC1_Set|TIM_SMS_RESETCLK_Set|TIM_SME_Set;
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+129
        LSLS     R3,R3,#+2        ;; #+516
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1237       
// 1238       /* Channel 1 input selection */
// 1239       if(TIM_InitStruct->TIM_IC1Selection == TIM_IC1Selection_TI1)
        LDRH     R2,[R1, #+24]
        CMP      R2,#+1
        BNE      ??ICAP_ModuleConfig_8
// 1240       {
// 1241         TIMx->IMCR &= TIM_IC1S_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+62
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_9
// 1242       }
// 1243       else
// 1244       {
// 1245         TIMx->IMCR |= TIM_IC1S_Set;
??ICAP_ModuleConfig_8:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1246       }
// 1247       /* Channel 1 polarity */
// 1248       if(TIM_InitStruct->TIM_IC1Polarity == TIM_IC1Polarity_Rising)
??ICAP_ModuleConfig_9:
        LDRH     R2,[R1, #+28]
        CMP      R2,#+2
        BNE      ??ICAP_ModuleConfig_10
// 1249       {
// 1250         TIMx->IMCR &= TIM_IC1P_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+55
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_11
// 1251       }
// 1252       else
// 1253       {
// 1254         TIMx->IMCR |= TIM_IC1P_Set;
??ICAP_ModuleConfig_10:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+8
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1255       }
// 1256 
// 1257       /* Input capture  Enable */
// 1258       TIMx->IMCR |= TIM_IC1_Enable;
??ICAP_ModuleConfig_11:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+4
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_5
// 1259     }
// 1260     else
// 1261     {
// 1262       /* Input Capture 2 mode configuration */
// 1263       TIMx->SCR &= (TIM_TriggerSelection_Mask & TIM_SlaveModeSelection_Mask);
??ICAP_ModuleConfig_6:
        LDRH     R2,[R0, #+4]
        LDR      R3,??ICAP_ModuleConfig_7  ;; 0x7007
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1264       TIMx->SCR |= TIM_TS_IC2_Set|TIM_SMS_RESETCLK_Set|TIM_SME_Set;
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+193
        LSLS     R3,R3,#+2        ;; #+772
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1265 
// 1266       /* Channel 2 input selection */
// 1267       if(TIM_InitStruct->TIM_IC2Selection == TIM_IC2Selection_TI2)
        LDRH     R2,[R1, #+26]
        CMP      R2,#+2
        BNE      ??ICAP_ModuleConfig_12
// 1268       {
// 1269         TIMx->IMCR &= TIM_IC2S_Reset;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+61
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??ICAP_ModuleConfig_13
// 1270       }
// 1271       else
// 1272       {
// 1273         TIMx->IMCR |= TIM_IC2S_Set;
??ICAP_ModuleConfig_12:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+2
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1274       }
// 1275 
// 1276       /* Channel 2 polarity */
// 1277       if(TIM_InitStruct->TIM_IC2Polarity == TIM_IC2Polarity_Rising)
??ICAP_ModuleConfig_13:
        LDRH     R2,[R1, #+30]
        CMP      R2,#+2
        BNE      ??ICAP_ModuleConfig_14
// 1278       {
// 1279         TIMx->IMCR &= TIM_IC2P_Reset;
        LDRH     R2,[R0, #+8]
        LSLS     R2,R2,#+27       ;; ZeroExtS R2,R2,#+27,#+27
        LSRS     R2,R2,#+27
        STRH     R2,[R0, #+8]
        B        ??ICAP_ModuleConfig_15
// 1280       }
// 1281       else
// 1282       {
// 1283         TIMx->IMCR |= TIM_IC2P_Set;
??ICAP_ModuleConfig_14:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+32
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1284       }
// 1285 
// 1286       /* Input capture  Enable */
// 1287       TIMx->IMCR |= TIM_IC2_Enable;
??ICAP_ModuleConfig_15:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+16
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1288     }
// 1289   }
// 1290 }
??ICAP_ModuleConfig_5:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??ICAP_ModuleConfig_7:
        DC32     0x7007
        CFI EndBlock cfiBlock26
// 1291 
// 1292 /*******************************************************************************
// 1293 * Function Name  : Encoder_ModeConfig
// 1294 * Description    : Encoder Mode configuration
// 1295 * Input          : - TIMx: where x can be 0, 1 or 2 to select the TIM peripheral
// 1296 *                  - TIM_InitStruct: pointer to a TIM_InitTypeDef structure that
// 1297 *                  contains the configuration information for the specified TIM
// 1298 *                  peripheral.
// 1299 * Output         : None
// 1300 * Return         : None
// 1301 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock27 Using cfiCommon0
        CFI Function Encoder_ModeConfig
        THUMB
// 1302 static void Encoder_ModeConfig(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIM_InitStruct)
// 1303 {
Encoder_ModeConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1304   /* Set Encoder mode */
// 1305   TIMx->SCR &= TIM_Encoder_Mask;
        LDRH     R2,[R0, #+4]
        LDR      R3,??Encoder_ModeConfig_0  ;; 0x731c
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1306   
// 1307   if(TIM_InitStruct->TIM_Mode == TIM_Mode_Encoder1) 
        LDRH     R2,[R1, #+0]
        CMP      R2,#+8
        BNE      ??Encoder_ModeConfig_1
// 1308   {
// 1309     TIMx->SCR |= TIM_Encoder1_Set;
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
        B        ??Encoder_ModeConfig_2
// 1310   }
// 1311   else if (TIM_InitStruct->TIM_Mode == TIM_Mode_Encoder2)
??Encoder_ModeConfig_1:
        LDRH     R2,[R1, #+0]
        CMP      R2,#+9
        BNE      ??Encoder_ModeConfig_3
// 1312   {
// 1313     TIMx->SCR |= TIM_Encoder2_Set;
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+2
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
        B        ??Encoder_ModeConfig_2
// 1314   }
// 1315   else 
// 1316   {
// 1317     TIMx->SCR |= TIM_Encoder3_Set;
??Encoder_ModeConfig_3:
        LDRH     R2,[R0, #+4]
        MOVS     R3,#+3
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+4]
// 1318   }
// 1319 
// 1320   /* Channel 1 input selection */
// 1321   if(TIM_InitStruct->TIM_IC1Selection == TIM_IC1Selection_TI2)
??Encoder_ModeConfig_2:
        LDRH     R2,[R1, #+24]
        CMP      R2,#+2
        BNE      ??Encoder_ModeConfig_4
// 1322   {
// 1323     TIMx->IMCR |= TIM_IC1S_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??Encoder_ModeConfig_5
// 1324   }
// 1325   else
// 1326   {
// 1327     TIMx->IMCR &= TIM_IC1S_Reset;
??Encoder_ModeConfig_4:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+62
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1328   }
// 1329 
// 1330    /* Channel 2 input selection */
// 1331    if(TIM_InitStruct->TIM_IC2Selection == TIM_IC2Selection_TI1)
??Encoder_ModeConfig_5:
        LDRH     R2,[R1, #+26]
        CMP      R2,#+1
        BNE      ??Encoder_ModeConfig_6
// 1332    {
// 1333      TIMx->IMCR |= TIM_IC2S_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+2
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??Encoder_ModeConfig_7
// 1334    }
// 1335    else
// 1336    {
// 1337      TIMx->IMCR &= TIM_IC2S_Reset;
??Encoder_ModeConfig_6:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+61
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1338    }
// 1339 
// 1340    /* Channel 1 polarity */
// 1341    if(TIM_InitStruct->TIM_IC1Polarity == TIM_IC1Polarity_Falling)
??Encoder_ModeConfig_7:
        LDRH     R2,[R1, #+28]
        CMP      R2,#+1
        BNE      ??Encoder_ModeConfig_8
// 1342    {
// 1343      TIMx->IMCR |= TIM_IC1P_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+8
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??Encoder_ModeConfig_9
// 1344    }
// 1345    else
// 1346    {
// 1347      TIMx->IMCR &= TIM_IC1P_Reset;
??Encoder_ModeConfig_8:
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+55
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+8]
// 1348    }
// 1349 
// 1350    /* Channel 2 polarity */
// 1351    if(TIM_InitStruct->TIM_IC2Polarity == TIM_IC2Polarity_Falling)
??Encoder_ModeConfig_9:
        LDRH     R2,[R1, #+30]
        CMP      R2,#+1
        BNE      ??Encoder_ModeConfig_10
// 1352    {
// 1353      TIMx->IMCR |= TIM_IC2P_Set;
        LDRH     R2,[R0, #+8]
        MOVS     R3,#+32
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+8]
        B        ??Encoder_ModeConfig_11
// 1354    }
// 1355    else
// 1356    {
// 1357      TIMx->IMCR &= TIM_IC2P_Reset;
??Encoder_ModeConfig_10:
        LDRH     R2,[R0, #+8]
        LSLS     R2,R2,#+27       ;; ZeroExtS R2,R2,#+27,#+27
        LSRS     R2,R2,#+27
        STRH     R2,[R0, #+8]
// 1358    }
// 1359 }
??Encoder_ModeConfig_11:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??Encoder_ModeConfig_0:
        DC32     0x731c
        CFI EndBlock cfiBlock27

        END
// 1360 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 2 738 bytes in section .text
// 
// 2 738 bytes of CODE memory
//
//Errors: none
//Warnings: none
