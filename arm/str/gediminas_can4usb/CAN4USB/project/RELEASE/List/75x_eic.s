///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:18 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\src\75x_eic.c                                   /
//    Command line =  D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\src\75x_eic.c -lC D:\development\gediminas\CAN4 /
//                    USB\project\RELEASE\List\ -lA                           /
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
//                    5x_eic.s                                                /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_eic`

        PUBLIC EIC_ClearFIQPendingBit
        PUBLIC EIC_CurrentIRQPriorityConfig
        PUBLIC EIC_DeInit
        PUBLIC EIC_FIQCmd
        PUBLIC EIC_FIQInit
        PUBLIC EIC_FIQStructInit
        PUBLIC EIC_GetCurrentFIQChannel
        PUBLIC EIC_GetCurrentIRQChannel
        PUBLIC EIC_GetCurrentIRQChannelPriority
        PUBLIC EIC_IRQCmd
        PUBLIC EIC_IRQInit
        PUBLIC EIC_IRQStructInit
        
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
        
// D:\development\gediminas\CAN4USB\project\STR75xLibrary\library\src\75x_eic.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_eic.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the EIC software functions.
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
//   20 #include "75x_eic.h"
//   21 
//   22 /* Private typedef -----------------------------------------------------------*/
//   23 /* Private define ------------------------------------------------------------*/
//   24 #define EIC_IRQEnable_Mask     0x00000001
//   25 #define EIC_IRQDisable_Mask    0xFFFFFFFE
//   26 
//   27 #define EIC_FIQEnable_Mask     0x00000002
//   28 #define EIC_FIQDisable_Mask    0xFFFFFFFD
//   29 
//   30 #define EIC_SIPL_Mask          0x0000000F
//   31 #define EIC_SIPL_Reset_Mask    0xFFFFFFF0
//   32 
//   33 /* Private macro -------------------------------------------------------------*/
//   34 /* Private variables ---------------------------------------------------------*/
//   35 /* Private function prototypes -----------------------------------------------*/
//   36 /* Private functions ---------------------------------------------------------*/
//   37 
//   38 /*******************************************************************************
//   39 * Function Name  : EIC_DeInit
//   40 * Description    : Deinitializes the EIC peripheral registers to their default
//   41 *                  reset values.
//   42 * Input          : None
//   43 * Output         : None
//   44 * Return         : None
//   45 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function EIC_DeInit
        THUMB
//   46 void EIC_DeInit(void)
//   47 {
//   48   EIC->ICR = 0x00;
EIC_DeInit:
        LDR      R0,??DataTable20  ;; 0xfffff800
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//   49   EIC->CIPR = 0x00;
        LDR      R0,??DataTable18  ;; 0xfffff808
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//   50   EIC->FIR = 0x0C;
        LDR      R0,??EIC_DeInit_0  ;; 0xfffff81c
        MOVS     R1,#+12
        STR      R1,[R0, #+0]
//   51   EIC->IER = 0x00;
        LDR      R0,??DataTable6  ;; 0xfffff820
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//   52   EIC->IPR = 0xFFFFFFFF;
        LDR      R0,??EIC_DeInit_0+0x4  ;; 0xfffff840
        MOVS     R1,#+0
        MVNS     R1,R1            ;; #-1
        STR      R1,[R0, #+0]
//   53 }
        BX       LR               ;; return
        Nop      
        DATA
??EIC_DeInit_0:
        DC32     0xfffff81c
        DC32     0xfffff840
        CFI EndBlock cfiBlock0
//   54 
//   55 /*******************************************************************************
//   56 * Function Name  : EIC_IRQInit
//   57 * Description    : Configures the IRQ channels according to the specified
//   58 *                  parameters in the EIC_IRQInitStruct.
//   59 * Input          : EIC_IRQInitStruct: pointer to a EIC_IRQInitTypeDef structure.
//   60 * Output         : None
//   61 * Return         : None
//   62 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function EIC_IRQInit
        THUMB
//   63 void EIC_IRQInit(EIC_IRQInitTypeDef* EIC_IRQInitStruct)
//   64 {
EIC_IRQInit:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//   65   u32 Tmp = 0;
        MOVS     R2,#+0
        MOVS     R1,R2
//   66 
//   67   if(EIC_IRQInitStruct->EIC_IRQChannelCmd == ENABLE)
        LDRB     R2,[R0, #+2]
        CMP      R2,#+1
        BNE      ??EIC_IRQInit_0
//   68   {
//   69     /* Enable the selected IRQ channel */
//   70     EIC->IER |= 1 << EIC_IRQInitStruct->EIC_IRQChannel;
        LDR      R2,??DataTable6  ;; 0xfffff820
        LDR      R2,[R2, #+0]
        MOVS     R3,#+1
        LDRB     R4,[R0, #+0]
        LSLS     R3,R3,R4
        ORRS     R3,R3,R2
        LDR      R2,??DataTable6  ;; 0xfffff820
        STR      R3,[R2, #+0]
//   71 
//   72     /* Configure the selected IRQ channel priority ***************************/
//   73     /* Clear SIPL[3:0] bits */
//   74     EIC->SIRn[EIC_IRQInitStruct->EIC_IRQChannel] &= EIC_SIPL_Reset_Mask;
        LDRB     R2,[R0, #+0]
        MOVS     R3,#+4
        MULS     R2,R3,R2
        LDR      R3,??EIC_IRQInit_1  ;; 0xfffff860
        LDR      R2,[R3, R2]
        MOVS     R3,#+15
        BICS     R2,R2,R3
        LDRB     R3,[R0, #+0]
        MOVS     R4,#+4
        MULS     R3,R4,R3
        LDR      R4,??EIC_IRQInit_1  ;; 0xfffff860
        STR      R2,[R4, R3]
//   75 
//   76     /* Configure SIPL[3:0] bits according to EIC_IRQChannelPriority parameter */
//   77     Tmp = EIC_IRQInitStruct->EIC_IRQChannelPriority & EIC_SIPL_Mask;
        LDRB     R2,[R0, #+1]
        LSLS     R2,R2,#+28       ;; ZeroExtS R2,R2,#+28,#+28
        LSRS     R2,R2,#+28
        MOVS     R1,R2
//   78     EIC->SIRn[EIC_IRQInitStruct->EIC_IRQChannel] |= Tmp;
        LDRB     R2,[R0, #+0]
        MOVS     R3,#+4
        MULS     R2,R3,R2
        LDR      R3,??EIC_IRQInit_1  ;; 0xfffff860
        LDR      R2,[R3, R2]
        ORRS     R2,R2,R1
        LDRB     R3,[R0, #+0]
        MOVS     R4,#+4
        MULS     R3,R4,R3
        LDR      R4,??EIC_IRQInit_1  ;; 0xfffff860
        STR      R2,[R4, R3]
        B        ??EIC_IRQInit_2
//   79   }
//   80   else
//   81   {
//   82     /* Disable the select IRQ channel */
//   83     EIC->IER &=~ (1 << EIC_IRQInitStruct->EIC_IRQChannel);
??EIC_IRQInit_0:
        LDR      R2,??DataTable6  ;; 0xfffff820
        LDR      R2,[R2, #+0]
        MOVS     R3,#+1
        LDRB     R4,[R0, #+0]
        LSLS     R3,R3,R4
        BICS     R2,R2,R3
        LDR      R3,??DataTable6  ;; 0xfffff820
        STR      R2,[R3, #+0]
//   84   }
//   85 }
??EIC_IRQInit_2:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??EIC_IRQInit_1:
        DC32     0xfffff860
        CFI EndBlock cfiBlock1

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable6:
        DC32     0xfffff820
//   86 
//   87 /*******************************************************************************
//   88 * Function Name  : EIC_FIQInit
//   89 * Description    : Configures the FIQ channels according to the specified
//   90 *                  parameters in the EIC_FIQInitStruct.
//   91 * Input          : EIC_FIQInitStruct: pointer to a EIC_FIQInitTypeDef structure.
//   92 * Output         : None
//   93 * Return         : None
//   94 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function EIC_FIQInit
        THUMB
//   95 void EIC_FIQInit(EIC_FIQInitTypeDef* EIC_FIQInitStruct)
//   96 {
EIC_FIQInit:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   97   if(EIC_FIQInitStruct->EIC_FIQChannelCmd == ENABLE)
        LDRB     R1,[R0, #+1]
        CMP      R1,#+1
        BNE      ??EIC_FIQInit_0
//   98   {
//   99     /* Enable the selected FIQ channel */
//  100     EIC->FIER |= EIC_FIQInitStruct->EIC_FIQChannel ;
        LDR      R1,??EIC_FIQInit_1  ;; 0xfffff810
        LDR      R1,[R1, #+0]
        LDRB     R2,[R0, #+0]
        ORRS     R2,R2,R1
        LDR      R1,??EIC_FIQInit_1  ;; 0xfffff810
        STR      R2,[R1, #+0]
        B        ??EIC_FIQInit_2
//  101   }
//  102   else
//  103   {
//  104     /* Disable the selected FIQ channel */
//  105     EIC->FIER &= ~EIC_FIQInitStruct->EIC_FIQChannel;
??EIC_FIQInit_0:
        LDR      R1,??EIC_FIQInit_1  ;; 0xfffff810
        LDR      R1,[R1, #+0]
        LDRB     R2,[R0, #+0]
        BICS     R1,R1,R2
        LDR      R2,??EIC_FIQInit_1  ;; 0xfffff810
        STR      R1,[R2, #+0]
//  106   }
//  107 }
??EIC_FIQInit_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??EIC_FIQInit_1:
        DC32     0xfffff810
        CFI EndBlock cfiBlock2
//  108 
//  109 /*******************************************************************************
//  110 * Function Name  : EIC_IRQStructInit
//  111 * Description    : Fills each EIC_IRQInitStruct member with its default value.
//  112 * Input          : EIC_IRQInitStruct: pointer to a EIC_IRQInitTypeDef structure
//  113 *                  which will be initialized.
//  114 * Output         : None
//  115 * Return         : None
//  116 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function EIC_IRQStructInit
        THUMB
//  117 void EIC_IRQStructInit(EIC_IRQInitTypeDef* EIC_IRQInitStruct)
//  118 {
//  119   EIC_IRQInitStruct->EIC_IRQChannel = 0x1F;
EIC_IRQStructInit:
        MOVS     R1,#+31
        STRB     R1,[R0, #+0]
//  120   EIC_IRQInitStruct->EIC_IRQChannelPriority = 0;
        MOVS     R1,#+0
        STRB     R1,[R0, #+1]
//  121   EIC_IRQInitStruct->EIC_IRQChannelCmd = DISABLE;
        MOVS     R1,#+0
        STRB     R1,[R0, #+2]
//  122 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//  123 
//  124 /*******************************************************************************
//  125 * Function Name  : EIC_FIQStructInit
//  126 * Description    : Fills each EIC_FIQInitStruct member with its default value.
//  127 * Input          : EIC_FIQInitStruct: pointer to a EIC_FIQInitTypeDef structure
//  128 *                  which will be initialized.
//  129 * Output         : None
//  130 * Return         : None
//  131 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function EIC_FIQStructInit
        THUMB
//  132 void EIC_FIQStructInit(EIC_FIQInitTypeDef* EIC_FIQInitStruct)
//  133 {
//  134   EIC_FIQInitStruct->EIC_FIQChannel = 0x03;
EIC_FIQStructInit:
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
//  135   EIC_FIQInitStruct->EIC_FIQChannelCmd = DISABLE;
        MOVS     R1,#+0
        STRB     R1,[R0, #+1]
//  136 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock4
//  137 
//  138 /*******************************************************************************
//  139 * Function Name  : EIC_IRQCmd
//  140 * Description    : Enables or disables EIC IRQ output request to CPU.
//  141 * Input          : NewState: new state of the EIC IRQ output request to CPU.
//  142 *                  This parameter can be: ENABLE or DISABLE.
//  143 * Output         : None
//  144 * Return         : None
//  145 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function EIC_IRQCmd
        THUMB
//  146 void EIC_IRQCmd(FunctionalState NewState)
//  147 {
EIC_IRQCmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  148   if(NewState == ENABLE)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??EIC_IRQCmd_0
//  149   {
//  150     /* Enable EIC IRQ output request to CPU */
//  151     EIC->ICR |= EIC_IRQEnable_Mask;
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        ORRS     R2,R2,R1
        LDR      R1,??DataTable20  ;; 0xfffff800
        STR      R2,[R1, #+0]
        B        ??EIC_IRQCmd_1
//  152   }
//  153   else
//  154   {
//  155     /* Disable EIC IRQ output request to CPU */
//  156     EIC->ICR &= EIC_IRQDisable_Mask;
??EIC_IRQCmd_0:
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        BICS     R1,R1,R2
        LDR      R2,??DataTable20  ;; 0xfffff800
        STR      R1,[R2, #+0]
//  157   }
//  158 }
??EIC_IRQCmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock5
//  159 
//  160 /*******************************************************************************
//  161 * Function Name  : EIC_FIQCmd
//  162 * Description    : Enables or disables EIC FIQ output request to CPU.
//  163 * Input          : NewState: new state of the EIC FIQ output request to CPU.
//  164 *                  This parameter can be: ENABLE or DISABLE.
//  165 * Output         : None
//  166 * Return         : None
//  167 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function EIC_FIQCmd
        THUMB
//  168 void EIC_FIQCmd(FunctionalState NewState)
//  169 {
EIC_FIQCmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  170   if(NewState == ENABLE)
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??EIC_FIQCmd_0
//  171   {
//  172     /* Enable EIC FIQ output request to CPU */
//  173     EIC->ICR |= EIC_FIQEnable_Mask;
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+2
        ORRS     R2,R2,R1
        LDR      R1,??DataTable20  ;; 0xfffff800
        STR      R2,[R1, #+0]
        B        ??EIC_FIQCmd_1
//  174   }
//  175   else
//  176   {
//  177     /* Disable EIC FIQ output request to CPU */
//  178     EIC->ICR &= EIC_FIQDisable_Mask;
??EIC_FIQCmd_0:
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+2
        BICS     R1,R1,R2
        LDR      R2,??DataTable20  ;; 0xfffff800
        STR      R1,[R2, #+0]
//  179   }
//  180 }
??EIC_FIQCmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock6
//  181 
//  182 /*******************************************************************************
//  183 * Function Name  : EIC_GetCurrentIRQChannel
//  184 * Description    : Returns the current served IRQ channel identifier.
//  185 * Input          : None
//  186 * Output         : None
//  187 * Return         : The current served IRQ channel.
//  188 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function EIC_GetCurrentIRQChannel
        THUMB
//  189 u8 EIC_GetCurrentIRQChannel(void)
//  190 {
//  191   /* Read and return the CIC[4:0] bits of CICR register */
//  192   return ((u8) (EIC->CICR));
EIC_GetCurrentIRQChannel:
        LDR      R0,??EIC_GetCurrentIRQChannel_0  ;; 0xfffff804
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BX       LR               ;; return
        Nop      
        DATA
??EIC_GetCurrentIRQChannel_0:
        DC32     0xfffff804
        CFI EndBlock cfiBlock7
//  193 }
//  194 
//  195 /*******************************************************************************
//  196 * Function Name  : EIC_GetCurrentIRQChannelPriority
//  197 * Description    : Returns the priority level of the current served IRQ channel.
//  198 * Input          : None
//  199 * Output         : None
//  200 * Return         : The priority level of the current served IRQ channel.
//  201 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function EIC_GetCurrentIRQChannelPriority
        THUMB
//  202 u8 EIC_GetCurrentIRQChannelPriority(void)
//  203 {
//  204   /* Read and return the CIP[3:0] bits of CIPR register */
//  205   return ((u8) (EIC->CIPR));
EIC_GetCurrentIRQChannelPriority:
        LDR      R0,??DataTable18  ;; 0xfffff808
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BX       LR               ;; return
        CFI EndBlock cfiBlock8
//  206 }
//  207 
//  208 /*******************************************************************************
//  209 * Function Name  : EIC_CurrentIRQPriorityConfig
//  210 * Description    : Changes the priority of the current served IRQ channel.
//  211 *                  The new priority value must be higher, or equal, than the
//  212 *                  priority value associated to the interrupt channel currently
//  213 *                  serviced.
//  214 * Input          : NewPriority: new priority value of the IRQ interrupt routine
//  215 *                  currently serviced.
//  216 * Output         : None
//  217 * Return         : None
//  218 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function EIC_CurrentIRQPriorityConfig
        THUMB
//  219 void EIC_CurrentIRQPriorityConfig(u8 NewPriority)
//  220 {
//  221   /* Disable EIC IRQ output request to CPU */
//  222   EIC->ICR &= EIC_IRQDisable_Mask;
EIC_CurrentIRQPriorityConfig:
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        BICS     R1,R1,R2
        LDR      R2,??DataTable20  ;; 0xfffff800
        STR      R1,[R2, #+0]
//  223 
//  224   /* Change the current priority */
//  225   EIC->CIPR = NewPriority;
        LDR      R1,??DataTable18  ;; 0xfffff808
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        STR      R0,[R1, #+0]
//  226 
//  227   /* Enable EIC IRQ output request to CPU  */
//  228   EIC->ICR |= EIC_IRQEnable_Mask;
        LDR      R1,??DataTable20  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        ORRS     R2,R2,R1
        LDR      R1,??DataTable20  ;; 0xfffff800
        STR      R2,[R1, #+0]
//  229 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock9

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable18:
        DC32     0xfffff808

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable20:
        DC32     0xfffff800
//  230 
//  231 /*******************************************************************************
//  232 * Function Name  : EIC_GetCurrentFIQChannel
//  233 * Description    : Returns the current served FIQ channel identifier.
//  234 * Input          : None
//  235 * Output         : None
//  236 * Return         : The current served FIQ channel.
//  237 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function EIC_GetCurrentFIQChannel
        THUMB
//  238 u8 EIC_GetCurrentFIQChannel(void)
//  239 {
//  240   /* Read and return the FIP[1:0] bits of FIPR register */
//  241   return ((u8) (EIC->FIPR));
EIC_GetCurrentFIQChannel:
        LDR      R0,??DataTable22  ;; 0xfffff814
        LDR      R0,[R0, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BX       LR               ;; return
        CFI EndBlock cfiBlock10
//  242 }
//  243 
//  244 /*******************************************************************************
//  245 * Function Name  : EIC_ClearFIQPendingBit
//  246 * Description    : Clears the pending bit of the selected FIQ Channel.
//  247 * Input          : EIC_FIQChannel: specifies the FIQ channel to clear its
//  248 *                  pending bit.
//  249 * Output         : None
//  250 * Return         : None
//  251 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function EIC_ClearFIQPendingBit
        THUMB
//  252 void EIC_ClearFIQPendingBit(u8 EIC_FIQChannel)
//  253 {
//  254   /* Clear the correspondent FIQ pending bit */
//  255   EIC->FIPR = EIC_FIQChannel ;
EIC_ClearFIQPendingBit:
        LDR      R1,??DataTable22  ;; 0xfffff814
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        STR      R0,[R1, #+0]
//  256 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock11

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable22:
        DC32     0xfffff814

        END
//  257 
//  258 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 404 bytes in section .text
// 
// 404 bytes of CODE memory
//
//Errors: none
//Warnings: none
