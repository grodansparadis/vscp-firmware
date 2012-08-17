///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:26 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\Program\IAR Systems\Embedded Workbench               /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_uart.c                                                /
//    Command line =  "D:\Program\IAR Systems\Embedded Workbench              /
//                    5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75 /
//                    x_uart.c" -lC D:\development\gediminas\CAN4USB\project\ /
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
//                    5x_uart.s                                               /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_uart`

        EXTERN MRCC_GetClocksStatus
        EXTERN MRCC_PeripheralSWResetConfig
        EXTERN __aeabi_uidivmod

        PUBLIC UART_ClearFlag
        PUBLIC UART_ClearITPendingBit
        PUBLIC UART_Cmd
        PUBLIC UART_DMACmd
        PUBLIC UART_DMAConfig
        PUBLIC UART_DeInit
        PUBLIC UART_GetFlagStatus
        PUBLIC UART_GetITStatus
        PUBLIC UART_ITConfig
        PUBLIC UART_Init
        PUBLIC UART_LINCmd
        PUBLIC UART_LINConfig
        PUBLIC UART_LoopBackConfig
        PUBLIC UART_RTSConfig
        PUBLIC UART_ReceiveData
        PUBLIC UART_SendBreak
        PUBLIC UART_SendData
        PUBLIC UART_StructInit
        
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
        
// D:\Program\IAR Systems\Embedded Workbench 5.0\ARM\examples\ST\STR75x\STR75xLibrary\library\src\75x_uart.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_uart.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the UART software functions.
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
//   20 #include "75x_uart.h"
//   21 #include "75x_mrcc.h"
//   22 
//   23 /* Private typedef -----------------------------------------------------------*/
//   24 /* Private define ------------------------------------------------------------*/
//   25 /* UART LIN Mask */
//   26 #define UART_LIN_Disable_Mask           0xFEFF /* LIN Disable Mask */
//   27 #define UART_LIN_Enable_Mask            0x0100 /* LIN Enable Mask */
//   28 
//   29 /* UART Mask */
//   30 #define UART_Enable_Mask                0x0001 /* UART Enable Mask */
//   31 #define UART_Disable_Mask               0xFFFE /* UART Disable Mask */
//   32 
//   33 /* UART LoopBack */
//   34 #define UART_LoopBack_Disable_Mask      0xFF7F/* LoopBack Disable Mask */
//   35 #define UART_LoopBack_Enable_Mask       0x0080/* LoopBack Enable Mask */
//   36 
//   37 #define UART_WordLength_Mask            0xFF9F  /* UART Word Length Mask */
//   38 #define UART_Parity_Mask                0xFF79  /* UART Parity Mask */
//   39 #define UART_HardwareFlowControl_Mask   0x3FFF  /* UART Hardware Flow Control Mask */
//   40 #define UART_TxRxFIFOLevel_Mask         0xFFC0  /* UART Tx Rx FIFO Level Mask */
//   41 #define UART_LINBreakLength_Mask        0xE1FF  /* UART LIN Break Length Mask */
//   42 #define UART_BreakChar_Mask             0x0001  /* UART Break Character send Mask */
//   43 #define UART_FLAG_Mask                  0x1F    /* UART Flag Mask */
//   44 #define UART_Mode_Mask                  0xFCFF  /* UART Mode Mask */
//   45 #define UART_RTSSET_Mask                0xF7FF  /* RTS signal is high */
//   46 #define UART_RTSRESET_Mask              0x0800  /* RTS signal is low */
//   47 
//   48 /* Private macro -------------------------------------------------------------*/
//   49 /* Private variables ---------------------------------------------------------*/
//   50 /* Private function prototypes -----------------------------------------------*/
//   51 /* Private functions ---------------------------------------------------------*/
//   52 
//   53 /*******************************************************************************
//   54 * Function Name  : UART_DeInit
//   55 * Description    : Deinitializes the UARTx peripheral registers to their default
//   56 *                  reset values.
//   57 * Input          : UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//   58 * Output         : None
//   59 * Return         : None
//   60 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function UART_DeInit
        THUMB
//   61 void UART_DeInit(UART_TypeDef* UARTx)
//   62 {
UART_DeInit:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//   63   /* Reset the UARTx registers values */
//   64   if(UARTx == UART0)
        LDR      R0,??UART_DeInit_0  ;; 0xffffd400
        CMP      R4,R0
        BNE      ??UART_DeInit_1
//   65   {
//   66     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART0,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+13       ;; #+1048576
        BL       MRCC_PeripheralSWResetConfig
//   67     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART0,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+13       ;; #+1048576
        BL       MRCC_PeripheralSWResetConfig
        B        ??UART_DeInit_2
//   68   }
//   69   else if(UARTx == UART1)
??UART_DeInit_1:
        LDR      R0,??UART_DeInit_0+0x4  ;; 0xffffd800
        CMP      R4,R0
        BNE      ??UART_DeInit_3
//   70   {
//   71     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART1,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+14       ;; #+2097152
        BL       MRCC_PeripheralSWResetConfig
//   72     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART1,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+14       ;; #+2097152
        BL       MRCC_PeripheralSWResetConfig
        B        ??UART_DeInit_2
//   73   }
//   74   else if(UARTx == UART2)
??UART_DeInit_3:
        LDR      R0,??UART_DeInit_0+0x8  ;; 0xffffdc00
        CMP      R4,R0
        BNE      ??UART_DeInit_2
//   75   {
//   76     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART2,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+15       ;; #+4194304
        BL       MRCC_PeripheralSWResetConfig
//   77     MRCC_PeripheralSWResetConfig(MRCC_Peripheral_UART2,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+15       ;; #+4194304
        BL       MRCC_PeripheralSWResetConfig
//   78   }
//   79 }
??UART_DeInit_2:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UART_DeInit_0:
        DC32     0xffffd400
        DC32     0xffffd800
        DC32     0xffffdc00
        CFI EndBlock cfiBlock0
//   80 
//   81 /*******************************************************************************
//   82 * Function Name  : UART_Init
//   83 * Description    : Initializes the UARTx peripheral according to the specified
//   84 *                  parameters in the UART_InitStruct .
//   85 * Input          : - UARTx: where x can be 0,1or 2 to select the UART peripheral.
//   86 *                  - UART_InitStruct: pointer to a UART_InitTypeDef structure
//   87 *                    that contains the configuration information for the
//   88 *                    specified UART peripheral.
//   89 * Output         : None
//   90 * Return         : None
//   91 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function UART_Init
        THUMB
//   92 void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct)
//   93 {
UART_Init:
        PUSH     {R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+20
        SUB      SP,SP,#+28
        CFI CFA R13+48
        MOVS     R4,R0
        MOVS     R5,R1
//   94 
//   95   u32 APBClock = 0;
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
//   96   u32 IntegerDivider = 0;
        MOVS     R0,#+0
        MOVS     R6,R0
//   97   u32 FractionalDivider = 0;
        MOVS     R0,#+0
        MOVS     R7,R0
//   98   MRCC_ClocksTypeDef  MRCC_ClocksStatus;
//   99      
//  100   /* Clear the WLEN bits */
//  101   UARTx->LCR &= UART_WordLength_Mask;
        LDRH     R0,[R4, #+44]
        LDR      R1,??UART_Init_0  ;; 0xff9f
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  102   /* Set the WLEN bits according to UART_WordLength value */
//  103   UARTx->LCR |= UART_InitStruct->UART_WordLength;
        LDRH     R0,[R4, #+44]
        LDRH     R1,[R5, #+0]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  104 
//  105   /* Choose Stop Bits */
//  106   if(UART_InitStruct->UART_StopBits == UART_StopBits_1)
        LDRH     R0,[R5, #+2]
        LDR      R1,??DataTable6  ;; 0xfff7
        CMP      R0,R1
        BNE      ??UART_Init_1
//  107   {
//  108     /* One Stop Bit */
//  109     UARTx->LCR &= UART_StopBits_1;
        LDRH     R0,[R4, #+44]
        LDR      R1,??DataTable6  ;; 0xfff7
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+44]
        B        ??UART_Init_2
//  110   }
//  111   else
//  112   {
//  113     /* Two Stop Bits */
//  114     UARTx->LCR |= UART_StopBits_2;
??UART_Init_1:
        LDRH     R0,[R4, #+44]
        MOVS     R1,#+8
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  115   }
//  116 
//  117   /* Clear SPS, EPS and PEN bits */
//  118   UARTx->LCR &= UART_Parity_Mask;
??UART_Init_2:
        LDRH     R0,[R4, #+44]
        LDR      R1,??UART_Init_0+0x4  ;; 0xff79
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  119   /* Set PS, EPS and PEN bits according to UART_Parity value */
//  120   UARTx->LCR |= UART_InitStruct->UART_Parity;
        LDRH     R0,[R4, #+44]
        LDRH     R1,[R5, #+4]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  121 
//  122   /* Configure the BaudRate --------------------------------------------------*/
//  123   /* Get the APB frequency */
//  124   MRCC_GetClocksStatus(&MRCC_ClocksStatus);
        ADD      R0,SP,#+4
        BL       MRCC_GetClocksStatus
//  125   APBClock = MRCC_ClocksStatus.PCLK_Frequency;
        LDR      R0,[SP, #+24]
        STR      R0,[SP, #+0]
//  126   
//  127   /* Determine the integer part */
//  128   IntegerDivider = ((100) * (APBClock) / (16 * (UART_InitStruct->UART_BaudRate)));
        LDR      R0,[SP, #+0]
        MOVS     R1,#+100
        MULS     R0,R1,R0
        LDR      R1,[R5, #+8]
        MOVS     R2,#+16
        MULS     R1,R2,R1
        BL       __aeabi_uidivmod
        MOVS     R6,R0
//  129   UARTx->IBRD = IntegerDivider / 100; 
        MOVS     R0,R6
        MOVS     R1,#+100
        BL       __aeabi_uidivmod
        STRH     R0,[R4, #+36]
//  130 
//  131   /* Determine the fractional part */
//  132   FractionalDivider = IntegerDivider - (100 * (UARTx->IBRD));
        LDRH     R0,[R4, #+36]
        MOVS     R1,#+100
        MULS     R0,R1,R0
        SUBS     R0,R6,R0
        MOVS     R7,R0
//  133   UARTx->FBRD = ((((FractionalDivider * 64) + 50) / 100));
        MOVS     R0,#+64
        MULS     R0,R7,R0
        ADDS     R0,R0,#+50
        MOVS     R1,#+100
        BL       __aeabi_uidivmod
        STRH     R0,[R4, #+40]
//  134   
//  135   /* Choose the Hardware Flow Control */
//  136   /* Clear RTSEn and CTSEn bits */
//  137   UARTx->CR &=  UART_HardwareFlowControl_Mask;
        LDRH     R0,[R4, #+48]
        LSLS     R0,R0,#+18       ;; ZeroExtS R0,R0,#+18,#+18
        LSRS     R0,R0,#+18
        STRH     R0,[R4, #+48]
//  138   /* Set RTSEn and CTSEn bits according to UART_HardwareFlowControl value */
//  139   UARTx->CR |= UART_InitStruct->UART_HardwareFlowControl;
        LDRH     R0,[R4, #+48]
        LDRH     R1,[R5, #+12]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+48]
//  140 
//  141   /* Configure the UART mode */
//  142   /* Clear TXE and RXE bits */
//  143   UARTx->CR &= UART_Mode_Mask;
        LDRH     R0,[R4, #+48]
        LDR      R1,??UART_Init_0+0x8  ;; 0xfcff
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+48]
//  144   /* Set TXE and RXE bits according to UART_Mode value */
//  145   UARTx->CR |= UART_InitStruct->UART_Mode;
        LDRH     R0,[R4, #+48]
        LDRH     R1,[R5, #+14]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+48]
//  146 
//  147   /* Enable or disable the FIFOs */
//  148   /* Set the FIFOs Levels */
//  149   if(UART_InitStruct->UART_FIFO == UART_FIFO_Enable)
        LDRH     R0,[R5, #+16]
        CMP      R0,#+16
        BNE      ??UART_Init_3
//  150   {
//  151     /* Enable the FIFOs */
//  152     UARTx->LCR |= UART_FIFO_Enable;
        LDRH     R0,[R4, #+44]
        MOVS     R1,#+16
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  153     
//  154     /* Clear TXIFLSEL and RXIFLSEL bits */
//  155     UARTx->IFLS &=  UART_TxRxFIFOLevel_Mask;
        LDRH     R0,[R4, #+52]
        LDR      R1,??UART_Init_0+0xC  ;; 0xffc0
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+52]
//  156     
//  157     /* Set RXIFLSEL bits according to UART_RxFIFOLevel value */
//  158     UARTx->IFLS |= (UART_InitStruct->UART_RxFIFOLevel << 3);
        LDRH     R0,[R4, #+52]
        LDRB     R1,[R5, #+19]
        LSLS     R1,R1,#+3
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+52]
//  159     
//  160     /* Set TXIFLSEL bits according to UART_TxFIFOLevel value */
//  161     UARTx->IFLS |= UART_InitStruct->UART_TxFIFOLevel;
        LDRH     R0,[R4, #+52]
        LDRB     R1,[R5, #+18]
        ORRS     R1,R1,R0
        STRH     R1,[R4, #+52]
        B        ??UART_Init_4
//  162   }
//  163   else
//  164   {
//  165     /* Disable the FIFOs */
//  166     UARTx->LCR &= UART_FIFO_Disable;
??UART_Init_3:
        LDRH     R0,[R4, #+44]
        LDR      R1,??UART_Init_0+0x10  ;; 0xffef
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+44]
//  167   }
//  168 }
??UART_Init_4:
        ADD      SP,SP,#+28
        CFI CFA R13+20
        POP      {R4-R7}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UART_Init_0:
        DC32     0xff9f
        DC32     0xff79
        DC32     0xfcff
        DC32     0xffc0
        DC32     0xffef
        CFI EndBlock cfiBlock1
//  169 
//  170 /*******************************************************************************
//  171 * Function Name  : UART_StructInit
//  172 * Description    : Fills each UART_InitStruct member with its default value.
//  173 * Input          : UART_InitStruct: pointer to a UART_InitTypeDef structure which
//  174 *                  will be initialized.
//  175 * Output         : None
//  176 * Return         : None
//  177 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function UART_StructInit
        THUMB
//  178 void UART_StructInit(UART_InitTypeDef* UART_InitStruct)
//  179 {
//  180   /* UART_InitStruct members default value */
//  181   UART_InitStruct->UART_WordLength = UART_WordLength_8D;
UART_StructInit:
        MOVS     R1,#+96
        STRH     R1,[R0, #+0]
//  182   UART_InitStruct->UART_StopBits = UART_StopBits_1;
        LDR      R1,??DataTable6  ;; 0xfff7
        STRH     R1,[R0, #+2]
//  183   UART_InitStruct->UART_Parity = UART_Parity_Odd ;
        MOVS     R1,#+2
        STRH     R1,[R0, #+4]
//  184   UART_InitStruct->UART_BaudRate = 9600;
        MOVS     R1,#+150
        LSLS     R1,R1,#+6        ;; #+9600
        STR      R1,[R0, #+8]
//  185   UART_InitStruct->UART_HardwareFlowControl = UART_HardwareFlowControl_None;
        MOVS     R1,#+0
        STRH     R1,[R0, #+12]
//  186   UART_InitStruct->UART_Mode = UART_Mode_Tx_Rx;
        MOVS     R1,#+192
        LSLS     R1,R1,#+2        ;; #+768
        STRH     R1,[R0, #+14]
//  187   UART_InitStruct->UART_FIFO = UART_FIFO_Enable;
        MOVS     R1,#+16
        STRH     R1,[R0, #+16]
//  188   UART_InitStruct->UART_TxFIFOLevel = UART_FIFOLevel_1_2;
        MOVS     R1,#+2
        STRB     R1,[R0, #+18]
//  189   UART_InitStruct->UART_RxFIFOLevel = UART_FIFOLevel_1_2;
        MOVS     R1,#+2
        STRB     R1,[R0, #+19]
//  190 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock2
//  191 
//  192 /*******************************************************************************
//  193 * Function Name  : UART_Cmd
//  194 * Description    : Enables or disables the specified UART peripheral.
//  195 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral
//  196 *                  - NewState: new state of the UARTx peripheral.
//  197 *                    This parameter can be: ENABLE or DISABLE.
//  198 * Output         : None
//  199 * Return         : None
//  200 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function UART_Cmd
        THUMB
//  201 void UART_Cmd(UART_TypeDef* UARTx, FunctionalState NewState)
//  202 {
UART_Cmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  203   if (NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_Cmd_0
//  204   {
//  205     /* Enable the selected UART by setting the UARTEN bit in the CR register */
//  206     UARTx->CR |= UART_Enable_Mask;
        LDRH     R2,[R0, #+48]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+48]
        B        ??UART_Cmd_1
//  207   }
//  208   else
//  209   {
//  210     /* Disable the selected UART by clearing the UARTEN bit in the CR register */
//  211     UARTx->CR &= UART_Disable_Mask;
??UART_Cmd_0:
        LDRH     R2,[R0, #+48]
        LDR      R3,??DataTable21  ;; 0xfffe
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+48]
//  212   }
//  213 }
??UART_Cmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock3
//  214 
//  215 /*******************************************************************************
//  216 * Function Name  : UART_ITConfig
//  217 * Description    : Enables or disables the specified UART interrupts.
//  218 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral
//  219 *                  - UART_IT: specifies the UART interrupts sources to be 
//  220 *                    enabled or disabled. This parameter can be any combination 
//  221 *                    of the following values:                   
//  222 *                       - UART_IT_OverrunError: Overrun Error interrupt
//  223 *                       - UART_IT_BreakError: Break Error interrupt
//  224 *                       - UART_IT_ParityError: Parity Error interrupt
//  225 *                       - UART_IT_FrameError: Frame Error interrupt
//  226 *                       - UART_IT_ReceiveTimeOut: Receive Time Out interrupt
//  227 *                       - UART_IT_Transmit: Transmit interrupt
//  228 *                       - UART_IT_Receive: Receive interrupt
//  229 *                       - UART_IT_CTS: CTS interrupt 
//  230 *                  - NewState: new state of the UARTx peripheral.
//  231 *                  This parameter can be: ENABLE or DISABLE.
//  232 * Output         : None
//  233 * Return         : None
//  234 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function UART_ITConfig
        THUMB
//  235 void UART_ITConfig(UART_TypeDef* UARTx, u16 UART_IT, FunctionalState NewState)
//  236 {
UART_ITConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  237   if(NewState == ENABLE)
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,#+1
        BNE      ??UART_ITConfig_0
//  238   {
//  239     /* Enables the selected interrupts */
//  240     UARTx->IMSC |= UART_IT;
        LDRH     R3,[R0, #+56]
        ORRS     R3,R3,R1
        STRH     R3,[R0, #+56]
        B        ??UART_ITConfig_1
//  241   }
//  242   else
//  243   {
//  244     /* Disables the selected interrupts */
//  245     UARTx->IMSC &= ~UART_IT;
??UART_ITConfig_0:
        LDRH     R3,[R0, #+56]
        BICS     R3,R3,R1
        STRH     R3,[R0, #+56]
//  246   }
//  247 }
??UART_ITConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock4
//  248 
//  249 /*******************************************************************************
//  250 * Function Name  : UART_DMAConfig
//  251 * Description    : Configures the UART0 DMA interface.
//  252 * Input          : - UART0_DMAtransfer : specifies the configuration of DMA request.
//  253 *                    This parameter can be:
//  254 *                         - UART0_DMATransfer_Single: Single DMA transfer
//  255 *                         - UART0_DMATransfer_Burst: Burst DMA transfer
//  256 *                  - UART0_DMAOnError: specifies the DMA on error request.
//  257 *                    This parameter can be:
//  258 *                         - UART0_DMAOnError_Enable: DMA receive request enabled
//  259 *                           when the UART error interrupt is asserted.
//  260 *                         - UART0_DMAOnError_Disable: DMA receive request disabled
//  261 *                           when the UART error interrupt is asserted.
//  262 * Output         : None
//  263 * Return         : None
//  264 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function UART_DMAConfig
        THUMB
//  265 void UART_DMAConfig(u16 UART0_DMATransfer, u16 UART0_DMAOnError)
//  266 {
UART_DMAConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  267   if(UART0_DMATransfer == UART0_DMATransfer_Single)
        LDR      R2,??DataTable6  ;; 0xfff7
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R0,R2
        BNE      ??UART_DMAConfig_0
//  268   {
//  269     /* Configure the DMA request from the UART0 as single transfer */
//  270     UART0->DMACR &= UART0_DMATransfer_Single;
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        LDR      R3,??DataTable6  ;; 0xfff7
        ANDS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
        B        ??UART_DMAConfig_1
//  271   }
//  272   else
//  273   {
//  274     UART0->DMACR |= UART0_DMATransfer_Burst;
??UART_DMAConfig_0:
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        MOVS     R3,#+8
        ORRS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
//  275   }
//  276   
//  277   if(UART0_DMAOnError == UART0_DMAOnError_Enable)
??UART_DMAConfig_1:
        LDR      R2,??UART_DMAConfig_2  ;; 0xfffb
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,R2
        BNE      ??UART_DMAConfig_3
//  278   {
//  279     UART0->DMACR &= UART0_DMAOnError_Enable;
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        LDR      R3,??UART_DMAConfig_2  ;; 0xfffb
        ANDS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
        B        ??UART_DMAConfig_4
//  280   }
//  281   else
//  282   {
//  283     UART0->DMACR |= UART0_DMAOnError_Disable;
??UART_DMAConfig_3:
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        MOVS     R3,#+4
        ORRS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
//  284   }
//  285 }
??UART_DMAConfig_4:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UART_DMAConfig_2:
        DC32     0xfffb
        CFI EndBlock cfiBlock5

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable6:
        DC32     0xfff7
//  286 
//  287 /*******************************************************************************
//  288 * Function Name  : UART_DMACmd
//  289 * Description    : Enables or disables the UART0’s DMA interface.
//  290 * Input          : - UART0_DMAReq: specifies the DMA request.
//  291 *                    This parameter can be:
//  292 *                     - UART0_DMAReq_Tx: Transmit DMA request
//  293 *                     - UART0_DMAReq_Rx: Receive DMA request
//  294 *                  - NewState: new state of the UART0’s DMA request.
//  295 *                    This parameter can be: ENABLE or DISABLE.
//  296 * Output         : None
//  297 * Return         : None
//  298 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function UART_DMACmd
        THUMB
//  299 void UART_DMACmd(u16 UART0_DMAReq, FunctionalState NewState)
//  300 {
UART_DMACmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  301   if(UART0_DMAReq == UART0_DMAReq_Tx)
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R0,#+2
        BNE      ??UART_DMACmd_0
//  302   {
//  303     if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_DMACmd_1
//  304     {
//  305       UART0->DMACR |=  UART0_DMAReq_Tx;
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        MOVS     R3,#+2
        ORRS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
        B        ??UART_DMACmd_2
//  306     }
//  307     else
//  308     {
//  309       UART0->DMACR &= ~UART0_DMAReq_Tx;
??UART_DMACmd_1:
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        LDR      R3,??UART_DMACmd_3  ;; 0xfffd
        ANDS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
        B        ??UART_DMACmd_2
//  310     }
//  311   }
//  312   else
//  313   {
//  314     if(NewState == ENABLE)
??UART_DMACmd_0:
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_DMACmd_4
//  315     {
//  316       UART0->DMACR |=  UART0_DMAReq_Rx;
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        MOVS     R3,#+1
        ORRS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
        B        ??UART_DMACmd_2
//  317     }
//  318     else
//  319     {
//  320       UART0->DMACR &= ~UART0_DMAReq_Rx;
??UART_DMACmd_4:
        LDR      R2,??DataTable22  ;; 0xffffd448
        LDRH     R2,[R2, #+0]
        LDR      R3,??DataTable21  ;; 0xfffe
        ANDS     R3,R3,R2
        LDR      R2,??DataTable22  ;; 0xffffd448
        STRH     R3,[R2, #+0]
//  321     }
//  322   }
//  323 }
??UART_DMACmd_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UART_DMACmd_3:
        DC32     0xfffd
        CFI EndBlock cfiBlock6

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable21:
        DC32     0xfffe

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable22:
        DC32     0xffffd448
//  324 
//  325 /*******************************************************************************
//  326 * Function Name  : UART_LoopBackConfig
//  327 * Description    : Enables or disables LoopBack mode in UARTx.
//  328 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral
//  329 *                  - NewState: new state of the UARTx’s LoopBack mode.
//  330 *                    This parameter can be: ENABLE or DISABLE.
//  331 * Output         : None
//  332 * Return         : None
//  333 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function UART_LoopBackConfig
        THUMB
//  334 void UART_LoopBackConfig(UART_TypeDef* UARTx, FunctionalState NewState)
//  335 {
UART_LoopBackConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  336   if (NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_LoopBackConfig_0
//  337   {
//  338     /* Enable the LoopBack mode of the specified UART */
//  339     UARTx->CR |= UART_LoopBack_Enable_Mask;
        LDRH     R2,[R0, #+48]
        MOVS     R3,#+128
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+48]
        B        ??UART_LoopBackConfig_1
//  340   }
//  341   else
//  342   {
//  343     /* Disable the LoopBack mode of the specified UART */
//  344     UARTx->CR &= UART_LoopBack_Disable_Mask;
??UART_LoopBackConfig_0:
        LDRH     R2,[R0, #+48]
        LDR      R3,??UART_LoopBackConfig_2  ;; 0xff7f
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+48]
//  345   }
//  346 }
??UART_LoopBackConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UART_LoopBackConfig_2:
        DC32     0xff7f
        CFI EndBlock cfiBlock7
//  347 
//  348 /*******************************************************************************
//  349 * Function Name  : UART_LINConfig
//  350 * Description    : Sets the LIN break length.
//  351 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//  352 *                  - UART_LINBreakLength: Break length value.
//  353 *                    This parameter can be:
//  354 *                         - UART_LINBreakLength_10: 10 low bits
//  355 *                         - UART_LINBreakLength_11: 11 low bits
//  356 *                         - UART_LINBreakLength_12: 12 low bits
//  357 *                         - UART_LINBreakLength_13: 13 low bits
//  358 *                         - UART_LINBreakLength_14: 14 low bits
//  359 *                         - UART_LINBreakLength_15: 15 low bits
//  360 *                         - UART_LINBreakLength_16: 16 low bits
//  361 *                         - UART_LINBreakLength_17: 17 low bits
//  362 *                         - UART_LINBreakLength_18: 18 low bits
//  363 *                         - UART_LINBreakLength_19: 19 low bits
//  364 *                         - UART_LINBreakLength_20: 20 low bits
//  365 * Output         : None
//  366 * Return         : None
//  367 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function UART_LINConfig
        THUMB
//  368 void UART_LINConfig(UART_TypeDef* UARTx, u16 UART_LINBreakLength)
//  369 {
//  370   /* Clear LBKLEN bits */
//  371   UARTx->LCR &= UART_LINBreakLength_Mask;
UART_LINConfig:
        LDRH     R2,[R0, #+44]
        LDR      R3,??UART_LINConfig_0  ;; 0xe1ff
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+44]
//  372 
//  373   /* Set LBKLEN bits according to UART_LINBreakLength value */
//  374   UARTx->LCR |= UART_LINBreakLength;
        LDRH     R2,[R0, #+44]
        ORRS     R2,R2,R1
        STRH     R2,[R0, #+44]
//  375 }
        BX       LR               ;; return
        DATA
??UART_LINConfig_0:
        DC32     0xe1ff
        CFI EndBlock cfiBlock8
//  376 
//  377 /*******************************************************************************
//  378 * Function Name  : UART_LINCmd
//  379 * Description    : Enables or disables LIN master mode in UARTx.
//  380 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral
//  381 *                  - NewState: new state of the UARTx’s LIN interface. 
//  382 *                    This parameter can be: ENABLE or DISABLE.
//  383 * Output         : None
//  384 * Return         : None
//  385 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function UART_LINCmd
        THUMB
//  386 void UART_LINCmd(UART_TypeDef* UARTx, FunctionalState NewState)
//  387 {
UART_LINCmd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  388   if(NewState == ENABLE)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_LINCmd_0
//  389   {
//  390     /* Enable the LIN mode of the specified UART */
//  391     UARTx->LCR |= UART_LIN_Enable_Mask;
        LDRH     R2,[R0, #+44]
        MOVS     R3,#+128
        LSLS     R3,R3,#+1        ;; #+256
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+44]
        B        ??UART_LINCmd_1
//  392   }
//  393   else
//  394   {
//  395     /* Disable the LIN mode of the specified UART */
//  396     UARTx->LCR &= UART_LIN_Disable_Mask;
??UART_LINCmd_0:
        LDRH     R2,[R0, #+44]
        LDR      R3,??UART_LINCmd_2  ;; 0xfeff
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+44]
//  397   }
//  398 }
??UART_LINCmd_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UART_LINCmd_2:
        DC32     0xfeff
        CFI EndBlock cfiBlock9
//  399 
//  400 /*******************************************************************************
//  401 * Function Name  : UART_SendData
//  402 * Description    : Transmits a signle Byte of data through the UARTx peripheral.
//  403 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//  404 *                  - Data: the byte to transmit
//  405 * Output         : None
//  406 * Return         : None
//  407 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function UART_SendData
        THUMB
//  408 void UART_SendData(UART_TypeDef* UARTx, u8 Data)
//  409 {
//  410   /* Transmit one byte */
//  411   UARTx->DR = Data;
UART_SendData:
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        STRH     R1,[R0, #+0]
//  412 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock10
//  413 
//  414 /*******************************************************************************
//  415 * Function Name  : UART_ReceiveData
//  416 * Description    : Returns the most recent received Byte by the UARTx peripheral.
//  417 * Input          : UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//  418 * Output         : None
//  419 * Return         : The received data
//  420 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function UART_ReceiveData
        THUMB
//  421 u8 UART_ReceiveData(UART_TypeDef* UARTx)
//  422 {
//  423   /* Receive one byte */
//  424   return ((u8)UARTx->DR);
UART_ReceiveData:
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BX       LR               ;; return
        CFI EndBlock cfiBlock11
//  425 }
//  426 
//  427 /*******************************************************************************
//  428 * Function Name  : UART_SendBreak
//  429 * Description    : Transmits break characters.
//  430 * Input          : UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//  431 * Output         : None
//  432 * Return         : None
//  433 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function UART_SendBreak
        THUMB
//  434 void UART_SendBreak(UART_TypeDef* UARTx)
//  435 {
//  436   /* Send break characters */
//  437   UARTx->BKR |= UART_BreakChar_Mask;
UART_SendBreak:
        LDRH     R1,[R0, #+28]
        MOVS     R2,#+1
        ORRS     R2,R2,R1
        STRH     R2,[R0, #+28]
//  438 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock12
//  439 
//  440 /*******************************************************************************
//  441 * Function Name  : UART_RTSConfig
//  442 * Description    : Sets or Resets the RTS signal
//  443 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral.
//  444 *                  - RTSState: new state of the RTS signal.
//  445 *                    This parameter can be: RTSSET or RTSRESET
//  446 * Output         : None
//  447 * Return         : None
//  448 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function UART_RTSConfig
        THUMB
//  449 void UART_RTSConfig(UART_TypeDef* UARTx, UART_RTSTypeDef RTSState)
//  450 {
UART_RTSConfig:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  451   if(RTSState == RTSRESET)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+1
        BNE      ??UART_RTSConfig_0
//  452   {
//  453     UARTx->CR |= UART_RTSRESET_Mask;
        LDRH     R2,[R0, #+48]
        MOVS     R3,#+128
        LSLS     R3,R3,#+4        ;; #+2048
        ORRS     R3,R3,R2
        STRH     R3,[R0, #+48]
        B        ??UART_RTSConfig_1
//  454   }
//  455   else if(RTSState == RTSSET)
??UART_RTSConfig_0:
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+2
        BNE      ??UART_RTSConfig_1
//  456   {
//  457     UARTx->CR &= UART_RTSSET_Mask;
        LDRH     R2,[R0, #+48]
        LDR      R3,??UART_RTSConfig_2  ;; 0xf7ff
        ANDS     R3,R3,R2
        STRH     R3,[R0, #+48]
//  458   }
//  459 }
??UART_RTSConfig_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UART_RTSConfig_2:
        DC32     0xf7ff
        CFI EndBlock cfiBlock13
//  460 
//  461 /*******************************************************************************
//  462 * Function Name  : UART_GetFlagStatus
//  463 * Description    : Checks whether the specified UART flag is set or not.
//  464 * Input          : - UARTx: where x can be 0,1 or 2 to select the UART peripheral
//  465 *                  - UART_FLAG: specifies the flag to check.
//  466 *                    This parameter can be one of the following values:
//  467 *                     - UART_FLAG_OverrunError: Overrun error flag
//  468 *                     - UART_FLAG_Break: break error flag
//  469 *                     - UART_FLAG_ParityError: parity error flag
//  470 *                     - UART_FLAG_FrameError: frame error flag
//  471 *                     - UART_FLAG_TxFIFOEmpty: Transmit FIFO Empty flag
//  472 *                     - UART_FLAG_RxFIFOFull: Receive FIFO Full flag
//  473 *                     - UART_FLAG_TxFIFOFull: Transmit FIFO Full flag
//  474 *                     - UART_FLAG_RxFIFOEmpty: Receive FIFO Empty flag
//  475 *                     - UART_FLAG_Busy: Busy flag
//  476 *                     - UART_FLAG_CTS: CTS flag
//  477 *                     - UART_RawIT_OverrunError: Overrun Error interrupt flag
//  478 *                     - UART_RawIT_BreakError: Break Error interrupt flag
//  479 *                     - UART_RawIT_ParityError: Parity Error interrupt flag
//  480 *                     - UART_RawIT_FrameError: Frame Error interrupt flag
//  481 *                     - UART_RawIT_ReceiveTimeOut: ReceiveTimeOut interrupt flag
//  482 *                     - UART_RawIT_Transmit: Transmit interrupt flag
//  483 *                     - UART_RawIT_Receive: Receive interrupt flag
//  484 *                     - UART_RawIT_CTS: CTS interrupt flag
//  485 * Output         : None
//  486 * Return         : The new state of UART_FLAG (SET or RESET).
//  487 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function UART_GetFlagStatus
        THUMB
//  488 FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx, u16 UART_FLAG)
//  489 {
UART_GetFlagStatus:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R2,R0
//  490   u32 UARTReg = 0, FlagPos = 0;
        MOVS     R0,#+0
        MOVS     R3,R0
        MOVS     R0,#+0
        MOVS     R4,R0
//  491   u32 StatusReg = 0;
        MOVS     R0,#+0
        MOVS     R5,R0
//  492 
//  493   /* Get the UART register index */
//  494   UARTReg = UART_FLAG >> 5;
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+5
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R3,R0
//  495 
//  496   /* Get the flag position */
//  497   FlagPos = UART_FLAG & UART_FLAG_Mask;
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSLS     R0,R1,#+27       ;; ZeroExtS R0,R1,#+27,#+27
        LSRS     R0,R0,#+27
        MOVS     R4,R0
//  498 
//  499   if(UARTReg == 1) /* The flag to check is in RSR register */
        CMP      R3,#+1
        BNE      ??UART_GetFlagStatus_0
//  500   {
//  501     StatusReg = UARTx->RSR;
        LDRH     R0,[R2, #+4]
        MOVS     R5,R0
        B        ??UART_GetFlagStatus_1
//  502   }
//  503   else if (UARTReg == 2) /* The flag to check is in FR register */
??UART_GetFlagStatus_0:
        CMP      R3,#+2
        BNE      ??UART_GetFlagStatus_2
//  504   {
//  505     StatusReg = UARTx->FR;
        LDRH     R0,[R2, #+24]
        MOVS     R5,R0
        B        ??UART_GetFlagStatus_1
//  506   }
//  507   else if(UARTReg == 3) /* The flag to check is in RIS register */
??UART_GetFlagStatus_2:
        CMP      R3,#+3
        BNE      ??UART_GetFlagStatus_1
//  508   {
//  509     StatusReg = UARTx->RIS;
        LDRH     R0,[R2, #+60]
        MOVS     R5,R0
//  510   }
//  511 
//  512   if((StatusReg & (1 << FlagPos))!= RESET)
??UART_GetFlagStatus_1:
        MOVS     R0,#+1
        LSLS     R0,R0,R4
        ANDS     R0,R0,R5
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
//  513   {
//  514     return SET;
//  515   }
//  516   else
//  517   {
//  518     return RESET;
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock14
//  519   }
//  520 }
//  521 
//  522 /*******************************************************************************
//  523 * Function Name  : UART_ClearFlag
//  524 * Description    : Clears the UARTx’s pending flags.
//  525 * Input          : - UARTx: where x can be 0,1or 2 to select the UART peripheral.
//  526 *                  - UART_FLAG: specifies the flag to clear.
//  527 *                    This parameter can be one of the following values:
//  528 *                       - UART_FLAG_OverrunError: Overrun error flag
//  529 *                       - UART_FLAG_Break: break error flag
//  530 *                       - UART_FLAG_ParityError: parity error flag
//  531 *                       - UART_FLAG_FrameError: frame error flag
//  532 * Output         : None
//  533 * Return         : None
//  534 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function UART_ClearFlag
        THUMB
//  535 void UART_ClearFlag(UART_TypeDef* UARTx, u16 UART_FLAG)
//  536 {
UART_ClearFlag:
        PUSH     {R0,R4}
        CFI R4 Frame(CFA, -4)
        CFI CFA R13+8
//  537   u8 FlagPos = 0;
        MOVS     R3,#+0
        MOVS     R2,R3
//  538 
//  539   /* Get the flag position */
//  540   FlagPos = UART_FLAG & UART_FLAG_Mask;
        MOVS     R3,R1
        LSLS     R3,R3,#+27       ;; ZeroExtS R3,R3,#+27,#+27
        LSRS     R3,R3,#+27
        MOVS     R2,R3
//  541 
//  542   /* Clear the sepecified flag */
//  543   UARTx->RSR &= ~(1 << FlagPos);
        LDRH     R3,[R0, #+4]
        MOVS     R4,#+1
        LSLS     R4,R4,R2
        BICS     R3,R3,R4
        STRH     R3,[R0, #+4]
//  544 }
        POP      {R3,R4}
        CFI R4 SameValue
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock15
//  545 
//  546 /*******************************************************************************
//  547 * Function Name  : UART_GetITStatus
//  548 * Description    : Checks whether the specified UART interrupt has occurred or not.
//  549 * Input          : - UARTx: where x can be 0,1or 2 to select the UART peripheral.
//  550 *                  - UART_IT: specifies the interrupt source to check.
//  551 *                    This parameter can be one of the following values:
//  552 *                       - UART_IT_OverrunError: Overrun Error interrupt 
//  553 *                       - UART_IT_BreakError: Break Error interrupt 
//  554 *                       - UART_IT_ParityError: Parity Error interrupt 
//  555 *                       - UART_IT_FrameError: Frame Error interrupt 
//  556 *                       - UART_IT_ReceiveTimeOut: Receive Time Out interrupt 
//  557 *                       - UART_IT_Transmit: Transmit interrupt 
//  558 *                       - UART_IT_Receive: Receive interrupt 
//  559 *                       - UART_IT_CTS: CTS interrupt 
//  560 * Output         : None
//  561 * Return         : The new state of UART_IT (SET or RESET).
//  562 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function UART_GetITStatus
        THUMB
//  563 ITStatus UART_GetITStatus(UART_TypeDef* UARTx, u16 UART_IT)
//  564 {
UART_GetITStatus:
        MOVS     R2,R0
//  565   if((UARTx->MIS & UART_IT) != RESET)
        MOVS     R0,#+64
        LDRH     R3,[R2, R0]
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
//  566   {
//  567     return SET;
//  568   }
//  569   else
//  570   {
//  571     return RESET;
        BX       LR               ;; return
        CFI EndBlock cfiBlock16
//  572   }
//  573 }
//  574 
//  575 /*******************************************************************************
//  576 * Function Name  : UART_ClearITPendingBit
//  577 * Description    : Clears the UARTx’s interrupt pending bits.
//  578 * Input          : - UARTx: where x can be 0,1or 2 to select the UART peripheral.
//  579 *                  - UART_IT: specifies the interrupt pending bit to clear.
//  580 *                    More than one interrupt can be cleared using the “|” operator.
//  581 *                    This parameter can be:
//  582 *                       - UART_IT_OverrunError: Overrun Error interrupt
//  583 *                       - UART_IT_BreakError: Break Error interrupt
//  584 *                       - UART_IT_ParityError: Parity Error interrupt
//  585 *                       - UART_IT_FrameError: Frame Error interrupt
//  586 *                       - UART_IT_ReceiveTimeOut: Receive Time Out interrupt
//  587 *                       - UART_IT_Transmit: Transmit interrupt
//  588 *                       - UART_IT_Receive: Receive interrupt
//  589 *                       - UART_IT_CTS: CTS interrupt
//  590 * Output         : None
//  591 * Return         : None
//  592 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function UART_ClearITPendingBit
        THUMB
//  593 void UART_ClearITPendingBit(UART_TypeDef* UARTx, u16 UART_IT)
//  594 {
//  595   /* Clear the specified interrupt */
//  596   UARTx->ICR = UART_IT;
UART_ClearITPendingBit:
        MOVS     R2,#+68
        STRH     R1,[R0, R2]
//  597 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock17

        END
//  598 
//  599 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
// 978 bytes in section .text
// 
// 978 bytes of CODE memory
//
//Errors: none
//Warnings: none
