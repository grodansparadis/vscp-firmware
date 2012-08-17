///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:19 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\Program\IAR Systems\Embedded Workbench               /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_gpio.c                                                /
//    Command line =  "D:\Program\IAR Systems\Embedded Workbench              /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_gpio.c" -lC D:\development\gediminas\CAN4USB\project\ /
//                    RELEASE\List\ -lA D:\development\gediminas\CAN4USB\proj /
//                    ect\RELEASE\List\ -o D:\development\gediminas\CAN4USB\p /
//                    roject\RELEASE\Obj\ --no_cse --no_unroll --no_inline    /
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
//                    5x_gpio.s                                               /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_gpio`

        PUBLIC GPIO_DeInit
        PUBLIC GPIO_GetPortMask
        PUBLIC GPIO_Init
        PUBLIC GPIO_PinMaskConfig
        PUBLIC GPIO_PinRemapConfig
        PUBLIC GPIO_Read
        PUBLIC GPIO_ReadBit
        PUBLIC GPIO_StructInit
        PUBLIC GPIO_Write
        PUBLIC GPIO_WriteBit
        
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
        
// D:\Program\IAR Systems\Embedded Workbench 5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75x_gpio.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_gpio.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the GPIO software functions.
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
//   20 #include "75x_gpio.h"
//   21 #include "75x_mrcc.h"
//   22 
//   23 /* Private typedef -----------------------------------------------------------*/
//   24 /* Private define ------------------------------------------------------------*/
//   25 #define GPIO_Remap_Mask    0x1F       /* GPIO remapping mask */
//   26 #define GPIO_Pin_Mask      0x000FFFFF /* GPIO1 and GPIO2 all pins mask */
//   27 
//   28 /* Private macro -------------------------------------------------------------*/
//   29 /* Private variables ---------------------------------------------------------*/
//   30 /* Private function prototypes -----------------------------------------------*/
//   31 /* Private functions ---------------------------------------------------------*/
//   32 
//   33 /*******************************************************************************
//   34 * Function Name  : GPIO_DeInit
//   35 * Description    : Deinitializes the GPIOx peripheral registers to their default
//   36 *                  reset values.
//   37 *                  The I/O remapping register 0 and 1 are not reset by this function.
//   38 * Input          : GPIOx: where x can be 0,1 or 2 to select the GPIO peripheral.
//   39 * Output         : None
//   40 * Return         : None
//   41 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function GPIO_DeInit
        THUMB
//   42 void GPIO_DeInit(GPIO_TypeDef* GPIOx)
//   43 {
//   44   /* Reset the GPIOx registers values */
//   45   GPIOx->PC0 = 0xFFFFFFFF;
GPIO_DeInit:
        MOVS     R1,#+0
        MVNS     R1,R1            ;; #-1
        STR      R1,[R0, #+0]
//   46   GPIOx->PC1 = 0x0;
        MOVS     R1,#+0
        STR      R1,[R0, #+4]
//   47   GPIOx->PC2 = 0x0;
        MOVS     R1,#+0
        STR      R1,[R0, #+8]
//   48   GPIOx->PM = 0x0;
        MOVS     R1,#+0
        STR      R1,[R0, #+16]
//   49 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock0
//   50 
//   51 /*******************************************************************************
//   52 * Function Name  : GPIO_Init
//   53 * Description    : Initializes the GPIOx peripheral according to the specified
//   54 *                  parameters in the GPIO_InitStruct. This function will not
//   55 *                  change the configuration for a pin if the corresponding mask
//   56 *                  bit is set, except pins configured as input pull-up or pull-down.
//   57 *                  These pins are automatically masked after each configuration.
//   58 * Input          :- GPIOx: where x can be (0..2) to select the GPIO peripheral.
//   59 *                 - GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
//   60 *                   contains the configuration information for the specified GPIO
//   61 *                   peripheral.
//   62 * Output         : None
//   63 * Return         : None
//   64 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function GPIO_Init
        THUMB
//   65 void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
//   66 {
GPIO_Init:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   67   /* GPIOx Mode and Pins Set */
//   68   if((GPIOx != GPIO0) && (GPIO_InitStruct->GPIO_Pin == GPIO_Pin_All))
        LDR      R2,??GPIO_Init_0  ;; 0xffffe400
        CMP      R0,R2
        BEQ      ??GPIO_Init_1
        LDR      R2,[R1, #+0]
        MOVS     R3,#+0
        MVNS     R3,R3            ;; #-1
        CMP      R2,R3
        BNE      ??GPIO_Init_1
//   69   {
//   70     GPIO_InitStruct->GPIO_Pin = GPIO_Pin_Mask;
        LDR      R2,??GPIO_Init_0+0x4  ;; 0xfffff
        STR      R2,[R1, #+0]
//   71   }
//   72 
//   73   switch(GPIO_InitStruct->GPIO_Mode)
??GPIO_Init_1:
        LDRB     R2,[R1, #+4]
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_2
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_3
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_4
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_5
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_6
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_7
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_8
        SUBS     R2,R2,#+1
        BEQ      ??GPIO_Init_9
        B        ??GPIO_Init_10
//   74   {
//   75     case GPIO_Mode_AIN:
//   76       GPIOx->PC0 &= ~GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_2:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+0]
//   77       GPIOx->PC1 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+4]
//   78       GPIOx->PC2 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+8]
        B        ??GPIO_Init_11
//   79       break;
//   80 
//   81     case GPIO_Mode_IN_FLOATING:
//   82       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_3:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//   83       GPIOx->PC1 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+4]
//   84       GPIOx->PC2 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+8]
        B        ??GPIO_Init_11
//   85       break;
//   86 
//   87     case GPIO_Mode_IPD:
//   88       GPIOx->PM  &= ~GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_4:
        LDR      R2,[R0, #+16]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+16]
//   89       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//   90       GPIOx->PC1 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+4]
//   91       GPIOx->PC2 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+8]
//   92       GPIOx->PD  &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+12]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+12]
//   93       GPIOx->PM  |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+16]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+16]
        B        ??GPIO_Init_11
//   94       break;
//   95 
//   96     case GPIO_Mode_IPU:
//   97       GPIOx->PM  &= ~GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_5:
        LDR      R2,[R0, #+16]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+16]
//   98       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//   99       GPIOx->PC1 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+4]
//  100       GPIOx->PC2 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+8]
//  101       GPIOx->PD  |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+12]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+12]
//  102       GPIOx->PM  |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+16]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+16]
        B        ??GPIO_Init_11
//  103       break;
//  104 
//  105     case GPIO_Mode_Out_OD:
//  106       GPIOx->PC0 &= ~GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_6:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+0]
//  107       GPIOx->PC1 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+4]
//  108       GPIOx->PC2 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+8]
        B        ??GPIO_Init_11
//  109       break;
//  110 
//  111     case GPIO_Mode_Out_PP:
//  112       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_7:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//  113       GPIOx->PC1 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+4]
//  114       GPIOx->PC2 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+8]
        B        ??GPIO_Init_11
//  115       break;
//  116 
//  117     case GPIO_Mode_AF_OD:
//  118       GPIOx->PD  |=  GPIO_InitStruct->GPIO_Pin;          
??GPIO_Init_8:
        LDR      R2,[R0, #+12]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+12]
//  119       GPIOx->PC1 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+4]
//  120       GPIOx->PC0 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+0]
//  121       GPIOx->PC2 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+8]
        B        ??GPIO_Init_11
//  122       break;
//  123 
//  124     case GPIO_Mode_AF_PP:
//  125       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_9:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//  126       GPIOx->PC1 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+4]
//  127       GPIOx->PC2 |=  GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+8]
        B        ??GPIO_Init_11
//  128       break;
//  129 
//  130     default :
//  131       GPIOx->PC0 |=  GPIO_InitStruct->GPIO_Pin;
??GPIO_Init_10:
        LDR      R2,[R0, #+0]
        LDR      R3,[R1, #+0]
        ORRS     R3,R3,R2
        STR      R3,[R0, #+0]
//  132       GPIOx->PC1 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+4]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+4]
//  133       GPIOx->PC2 &= ~GPIO_InitStruct->GPIO_Pin;
        LDR      R2,[R0, #+8]
        LDR      R3,[R1, #+0]
        BICS     R2,R2,R3
        STR      R2,[R0, #+8]
//  134       break;
//  135   }
//  136 }
??GPIO_Init_11:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??GPIO_Init_0:
        DC32     0xffffe400
        DC32     0xfffff
        CFI EndBlock cfiBlock1
//  137 
//  138 /*******************************************************************************
//  139 * Function Name  : GPIO_StructInit
//  140 * Description    : Fills each GPIO_InitStruct member with its default value.
//  141 * Input          : GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure
//  142 *                  which will be initialized.
//  143 * Output         : None
//  144 * Return         : None
//  145 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function GPIO_StructInit
        THUMB
//  146 void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
//  147 {
//  148   /* Reset GPIO init structure parameters values */
//  149   GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
GPIO_StructInit:
        MOVS     R1,#+0
        MVNS     R1,R1            ;; #-1
        STR      R1,[R0, #+0]
//  150   GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_FLOATING;
        MOVS     R1,#+2
        STRB     R1,[R0, #+4]
//  151 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock2
//  152 
//  153 /*******************************************************************************
//  154 * Function Name  : GPIO_Read
//  155 * Description    : Reads the specified GPIO data port.
//  156 * Input          : GPIOx: where x can be 0,1 or 2 to select the GPIO peripheral.
//  157 * Output         : None
//  158 * Return         : GPIO data port word value.
//  159 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function GPIO_Read
        THUMB
//  160 u32 GPIO_Read(GPIO_TypeDef* GPIOx)
//  161 {
//  162   return GPIOx->PD;
GPIO_Read:
        LDR      R0,[R0, #+12]
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//  163 }
//  164 
//  165 /*******************************************************************************
//  166 * Function Name  : GPIO_ReadBit
//  167 * Description    : Reads the specified data port bit.
//  168 * Input          : - GPIOx: where x can be (0..2) to select the GPIO peripheral.
//  169 *                : - GPIO_Pin:  specifies the port bit to read.
//  170 *                    This parameter can be GPIO_Pin_x where x can be (0..31) for
//  171 *                    GPIO0 and x(0..19) for GPIO1 and GPIO2.
//  172 * Output         : None
//  173 * Return         : The port pin value
//  174 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function GPIO_ReadBit
        THUMB
//  175 u8 GPIO_ReadBit(GPIO_TypeDef* GPIOx, u32 GPIO_Pin)
//  176 {
GPIO_ReadBit:
        MOVS     R2,R0
//  177   if ((GPIOx->PD & GPIO_Pin) != Bit_RESET)
        LDR      R0,[R2, #+12]
        ANDS     R0,R0,R1
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  178   {
//  179     return Bit_SET;
//  180   }
//  181   else
//  182   {
//  183     return Bit_RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock4
//  184   }
//  185 }
//  186 
//  187 /*******************************************************************************
//  188 * Function Name  : GPIO_Write
//  189 * Description    : Writes data to the specified GPIO data port.
//  190 * Input          :- GPIOx: where x can be 0,1 or 2 to select the GPIO peripheral.
//  191 *                 - PortVal: specifies the value to be written to the data port
//  192 *                   register.
//  193 * Output         : None
//  194 * Return         : None
//  195 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function GPIO_Write
        THUMB
//  196 void GPIO_Write(GPIO_TypeDef* GPIOx, u32 PortVal)
//  197 {
//  198   GPIOx->PD = PortVal;
GPIO_Write:
        STR      R1,[R0, #+12]
//  199 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock5
//  200 
//  201 /*******************************************************************************
//  202 * Function Name  : GPIO_WriteBit
//  203 * Description    : Sets or clears the selected data port bit.
//  204 * Input          : - GPIOx: where x can be (0..2) to select the GPIO peripheral.
//  205 *                  - GPIO_Pin: specifies the port bit to be written.
//  206 *                    This parameter can be GPIO_Pin_x where x can be (0..31) for
//  207 *                    GPIO0 and x(0..19) for GPIO1 and GPIO2.
//  208 *                  - BitVal: specifies the value to be written to the selected bit.
//  209 *                    This parameter must be one of the BitAction enum values:
//  210 *                       - Bit_RESET: to clear the port pin
//  211 *                       - Bit_SET: to set the port pin
//  212 * Output         : None
//  213 * Return         : None
//  214 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function GPIO_WriteBit
        THUMB
//  215 void GPIO_WriteBit(GPIO_TypeDef* GPIOx, u32 GPIO_Pin, BitAction BitVal)
//  216 {
GPIO_WriteBit:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  217   if(BitVal != Bit_RESET)
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,#+0
        BEQ      ??GPIO_WriteBit_0
//  218   {
//  219     GPIOx->PD |= GPIO_Pin;
        LDR      R3,[R0, #+12]
        ORRS     R3,R3,R1
        STR      R3,[R0, #+12]
        B        ??GPIO_WriteBit_1
//  220   }
//  221   else
//  222   {
//  223     GPIOx->PD &= ~GPIO_Pin;
??GPIO_WriteBit_0:
        LDR      R3,[R0, #+12]
        BICS     R3,R3,R1
        STR      R3,[R0, #+12]
//  224   }
//  225 }
??GPIO_WriteBit_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock6
//  226 
//  227 /*******************************************************************************
//  228 * Function Name  : GPIO_PinMaskConfig
//  229 * Description    : Enables or disables write protection to the selected bits in
//  230 *                  the I/O port registers (PxC2, PxC1, PxC0 and PxD).
//  231 * Input          :- GPIOx: where x can be 0,1 or 2 to select the GPIO peripheral.
//  232 *                 - GPIO_Pin: specifies the port bit to be protected.
//  233 *                   This parameter can be GPIO_Pin_x where x can be (0..31) for
//  234 *                   GPIO0 and x(0..19) for GPIO1 and GPIO2.
//  235 *                 - NewState: new state of the port pin.
//  236 *                   This parameter can be: ENABLE or DISABLE.
//  237 * Output         : None
//  238 * Return         : None
//  239 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function GPIO_PinMaskConfig
        THUMB
//  240 void GPIO_PinMaskConfig(GPIO_TypeDef* GPIOx, u32 GPIO_Pin, FunctionalState NewState)
//  241 {
GPIO_PinMaskConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  242   if(NewState == ENABLE)
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,#+1
        BNE      ??GPIO_PinMaskConfig_0
//  243   {
//  244     GPIOx->PM |= GPIO_Pin;
        LDR      R3,[R0, #+16]
        ORRS     R3,R3,R1
        STR      R3,[R0, #+16]
        B        ??GPIO_PinMaskConfig_1
//  245   }
//  246   else
//  247   {
//  248     GPIOx->PM &= ~GPIO_Pin;
??GPIO_PinMaskConfig_0:
        LDR      R3,[R0, #+16]
        BICS     R3,R3,R1
        STR      R3,[R0, #+16]
//  249   }
//  250 }
??GPIO_PinMaskConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock7
//  251 
//  252 /*******************************************************************************
//  253 * Function Name  : GPIO_GetPortMask
//  254 * Description    : Gets the GPIOx port mask value.
//  255 * Input          : GPIOx: where x can be 0,1 or 2 to select the GPIO peripheral.
//  256 * Output         : None
//  257 * Return         : GPIO port mask value.
//  258 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function GPIO_GetPortMask
        THUMB
//  259 u32 GPIO_GetPortMask(GPIO_TypeDef* GPIOx)
//  260 {
//  261   return GPIOx->PM;
GPIO_GetPortMask:
        LDR      R0,[R0, #+16]
        BX       LR               ;; return
        CFI EndBlock cfiBlock8
//  262 }
//  263 
//  264 /*******************************************************************************
//  265 * Function Name  : GPIO_PinRemapConfig
//  266 * Description    : Changes the mapping of the specified pin.
//  267 * Input          :- GPIO_Remap: selects the pin to remap.
//  268 *                   This parameter can be one of the following values:
//  269 *                     - GPIO_Remap_SMI_CS3_EN: Enable SMI CS3 
//  270 *                     - GPIO_Remap_SMI_CS2_EN: Enable SMI CS2
//  271 *                     - GPIO_Remap_SMI_CS1_EN: Enable SMI CS1
//  272 *                     - GPIO_Remap_SMI_EN: Enable SMI Alternate Functions: 
//  273 *                       SMI_CS0, SMI_CK, SMI_DIN and SMI_DOUT
//  274 *                     - GPIO_Remap_DBGOFF: JTAG Disable
//  275 *                     - GPIO_Remap_UART1: UART1 Alternate Function mapping
//  276 *                     - GPIO_Remap_UART2: UART2 Alternate Function mapping
//  277 *                     - GPIO_Remap_SSP1: SSP1 Alternate Function mapping
//  278 *                     - GPIO_Remap_TIM2: TIM2 Alternate Function mapping
//  279 *                     - GPIO_Remap_TIM0: TIM0 Alternate Function mapping
//  280 *                 - NewState: new state of the port pin.
//  281 *                   This parameter can be: ENABLE or DISABLE.
//  282 * Output         : None
//  283 * Return         : None
//  284 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function GPIO_PinRemapConfig
        THUMB
//  285 void GPIO_PinRemapConfig(u16 GPIO_Remap, FunctionalState NewState)
//  286 {
GPIO_PinRemapConfig:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
//  287   u32 GPIOReg = 0;
        MOVS     R4,#+0
        MOVS     R2,R4
//  288   u32 PinPos = 0;
        MOVS     R4,#+0
        MOVS     R3,R4
//  289 
//  290   /* Get the GPIO register index */
//  291   GPIOReg = GPIO_Remap >> 5;
        MOVS     R4,R0
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        LSRS     R4,R4,#+5
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        MOVS     R2,R4
//  292 
//  293   /* Get the pin position */
//  294   PinPos = GPIO_Remap & GPIO_Remap_Mask;
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R4,R0,#+27       ;; ZeroExtS R4,R0,#+27,#+27
        LSRS     R4,R4,#+27
        MOVS     R3,R4
//  295 
//  296   if(GPIOReg == 1) /* The pin to remap is in REMAP0R register */
        CMP      R2,#+1
        BNE      ??GPIO_PinRemapConfig_0
//  297   {
//  298     if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??GPIO_PinRemapConfig_1
//  299     {
//  300       GPIOREMAP->REMAP0R |= (1 << PinPos);
        LDR      R4,??GPIO_PinRemapConfig_2  ;; 0xffffe420
        LDR      R4,[R4, #+0]
        MOVS     R5,#+1
        LSLS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??GPIO_PinRemapConfig_2  ;; 0xffffe420
        STR      R5,[R4, #+0]
        B        ??GPIO_PinRemapConfig_3
//  301     }
//  302     else
//  303     {
//  304       GPIOREMAP->REMAP0R &= ~(1 << PinPos);
??GPIO_PinRemapConfig_1:
        LDR      R4,??GPIO_PinRemapConfig_2  ;; 0xffffe420
        LDR      R4,[R4, #+0]
        MOVS     R5,#+1
        LSLS     R5,R5,R3
        BICS     R4,R4,R5
        LDR      R5,??GPIO_PinRemapConfig_2  ;; 0xffffe420
        STR      R4,[R5, #+0]
        B        ??GPIO_PinRemapConfig_3
//  305     }
//  306   }
//  307   else if(GPIOReg == 2) /* The pin to remap is in REMAP1R register */
??GPIO_PinRemapConfig_0:
        CMP      R2,#+2
        BNE      ??GPIO_PinRemapConfig_3
//  308   {
//  309     if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??GPIO_PinRemapConfig_4
//  310     {
//  311       GPIOREMAP->REMAP1R |= (1 << PinPos);
        LDR      R4,??GPIO_PinRemapConfig_2+0x4  ;; 0xffffe424
        LDR      R4,[R4, #+0]
        MOVS     R5,#+1
        LSLS     R5,R5,R3
        ORRS     R5,R5,R4
        LDR      R4,??GPIO_PinRemapConfig_2+0x4  ;; 0xffffe424
        STR      R5,[R4, #+0]
        B        ??GPIO_PinRemapConfig_3
//  312     }
//  313     else
//  314     {
//  315       GPIOREMAP->REMAP1R &= ~(1 << PinPos);
??GPIO_PinRemapConfig_4:
        LDR      R4,??GPIO_PinRemapConfig_2+0x4  ;; 0xffffe424
        LDR      R4,[R4, #+0]
        MOVS     R5,#+1
        LSLS     R5,R5,R3
        BICS     R4,R4,R5
        LDR      R5,??GPIO_PinRemapConfig_2+0x4  ;; 0xffffe424
        STR      R4,[R5, #+0]
//  316     }
//  317   }
//  318 }
??GPIO_PinRemapConfig_3:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??GPIO_PinRemapConfig_2:
        DC32     0xffffe420
        DC32     0xffffe424
        CFI EndBlock cfiBlock9

        END
//  319 
//  320 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 620 bytes in section .text
// 
// 620 bytes of CODE memory
//
//Errors: none
//Warnings: none
