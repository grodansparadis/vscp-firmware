///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:17 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\vscp\firmware\arm\str\cantest_str75x_iar /
//                    \str75x_lib\src\75x_can.c                               /
//    Command line =  D:\development\vscp\firmware\arm\str\cantest_str75x_iar /
//                    \str75x_lib\src\75x_can.c -lC                           /
//                    D:\development\gediminas\CAN4USB\project\RELEASE\List\  /
//                    -lA D:\development\gediminas\CAN4USB\project\RELEASE\Li /
//                    st\ -o D:\development\gediminas\CAN4USB\project\RELEASE /
//                    \Obj\ --no_cse --no_unroll --no_inline                  /
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
//                    5x_can.s                                                /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME `75x_can`

        EXTERN MRCC_PeripheralSWResetConfig

        PUBLIC CAN_BasicReceiveMessage
        PUBLIC CAN_BasicSendMessage
        PUBLIC CAN_DeInit
        PUBLIC CAN_EnterInitMode
        PUBLIC CAN_EnterTestMode
        PUBLIC CAN_Init
        PUBLIC CAN_InvalidateAllMsgObj
        PUBLIC CAN_IsInterruptPending
        PUBLIC CAN_IsMessageWaiting
        PUBLIC CAN_IsObjectValid
        PUBLIC CAN_IsTransmitRequested
        PUBLIC CAN_LeaveInitMode
        PUBLIC CAN_LeaveTestMode
        PUBLIC CAN_ReceiveMessage
        PUBLIC CAN_ReleaseMessage
        PUBLIC CAN_ReleaseRxMessage
        PUBLIC CAN_ReleaseTxMessage
        PUBLIC CAN_SendMessage
        PUBLIC CAN_SetBitrate
        PUBLIC CAN_SetRxMsgObj
        PUBLIC CAN_SetTiming
        PUBLIC CAN_SetTxMsgObj
        PUBLIC CAN_SetUnusedMsgObj
        PUBLIC CAN_StructInit
        PUBLIC CAN_WaitEndOfTx
        PUBLIC CanTimings
        
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
        
// D:\development\vscp\firmware\arm\str\cantest_str75x_iar\str75x_lib\src\75x_can.c
//    1 /******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
//    2 * File Name          : 75x_can.c
//    3 * Author             : MCD Application Team
//    4 * Date First Issued  : 03/10/2006
//    5 * Description        : This file provides all the CAN software functions.
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
//   20 #include "75x_can.h"
//   21 #include "75x_mrcc.h"
//   22 
//   23 /* Private typedef -----------------------------------------------------------*/
//   24 /* Private define ------------------------------------------------------------*/
//   25 /* Private macro -------------------------------------------------------------*/
//   26 /*----------------------------------------------------------------------------*/
//   27 /* Macro Name     : xxx_ID_MSK, xxx_ID_ARB                                    */
//   28 /* Description    : Form the Mask and Arbitration registers value to filter   */
//   29 /*                  a range of identifiers or a fixed identifier, for standard*/
//   30 /*                  and extended IDs                                          */
//   31 /*----------------------------------------------------------------------------*/
//   32 #define RANGE_ID_MSK(range_start, range_end)	(~((range_end) - (range_start)))
//   33 #define RANGE_ID_ARB(range_start, range_end)	((range_start) & (range_end))
//   34 
//   35 #define FIXED_ID_MSK(id)	RANGE_ID_MSK((id), (id))
//   36 #define FIXED_ID_ARB(id)	RANGE_ID_ARB((id), (id))
//   37 
//   38 #define STD_RANGE_ID_MSK(range_start, range_end)	((u16)((RANGE_ID_MSK((range_start), (range_end)) & 0x7FF) << 2))
//   39 #define STD_RANGE_ID_ARB(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) << 2))
//   40 
//   41 #define STD_FIXED_ID_MSK(id)	((u16)((FIXED_ID_MSK(id) & 0x7FF) << 2))
//   42 #define STD_FIXED_ID_ARB(id)	((u16)(FIXED_ID_ARB(id) << 2))
//   43 
//   44 #define EXT_RANGE_ID_MSK_L(range_start, range_end)	((u16)(RANGE_ID_MSK((range_start), (range_end)) >> 11))
//   45 #define EXT_RANGE_ID_MSK_H(range_start, range_end)	((u16)(STD_RANGE_ID_MSK((range_start), (range_end)) | ((RANGE_ID_MSK((range_start), (range_end)) >> 27) & 0x03)))
//   46 #define EXT_RANGE_ID_ARB_L(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) >> 11))
//   47 #define EXT_RANGE_ID_ARB_H(range_start, range_end)	((u16)(STD_RANGE_ID_ARB((range_start), (range_end)) | ((RANGE_ID_ARB((range_start), (range_end)) >> 27) & 0x03)))
//   48 
//   49 #define EXT_FIXED_ID_MSK_L(id)	((u16)(FIXED_ID_MSK(id) >> 11))
//   50 #define EXT_FIXED_ID_MSK_H(id)	((u16)(STD_FIXED_ID_MSK(id) | ((FIXED_ID_MSK(id) >> 27) & 0x03)))
//   51 #define EXT_FIXED_ID_ARB_L(id)	((u16)(FIXED_ID_ARB(id) >> 11))
//   52 #define EXT_FIXED_ID_ARB_H(id)	((u16)(STD_FIXED_ID_ARB(id) | ((FIXED_ID_ARB(id) >> 27) & 0x03)))
//   53 
//   54 /* macro to format the timing register value from the timing parameters*/
//   55 #define CAN_TIMING(tseg1, tseg2, sjw, brp)	((((tseg2-1) & 0x07) << 12) | (((tseg1-1) & 0x0F) << 8) | (((sjw-1) & 0x03) << 6) | ((brp-1) & 0x3F))
//   56 
//   57 /* Private variables ---------------------------------------------------------*/
//   58 /* array of pre-defined timing parameters for standard bitrates*/

        SECTION `.data`:DATA:NOROOT(2)
//   59 u16 CanTimings[] = {        /* value   bitrate     NTQ  TSEG1  TSEG2  SJW  BRP Sample point  */
CanTimings:
        DATA
        DC16 15044, 7000, 6985, 15044, 15043, 9091, 7168, 7168, 8960
        DC8 0, 0
//   60   CAN_TIMING(11, 4, 4, 5),  /*          10 kbit/s  24   16     7      4    32  70.8%         */
//   61   CAN_TIMING(12, 2, 2, 25), /*          20 kbit/s  16   12     2      1    25  87.5%         */
//   62   CAN_TIMING(12, 2, 2, 10), /*          50 kbit/s  16   12     2      1    10  87.5%         */
//   63   CAN_TIMING(11, 4, 4, 5),  /* 0x3AC4  100 kbit/s  16   11     4      4    5   75%           */
//   64   CAN_TIMING(11, 4, 4, 4),  /* 0x3AC3  125 kbit/s  16   11     4      4    4   75%           */
//   65   CAN_TIMING( 4, 3, 3, 4),  /* 0x2383  250 kbit/s   8    4     3      3    4   62.5%         */
//   66   CAN_TIMING(13, 2, 1, 1),  /* 0x1C00  500 kbit/s  16   13     2      1    1   87.5%         */
//   67   CAN_TIMING(13, 2, 1, 1),  /*         800 kbit/s  10    7     2      1    1   80%           */
//   68   CAN_TIMING( 4, 3, 1, 1),  /* 0x2300  1 Mbit/s     8    4     3      1    1   62.5%         */
//   69 };
//   70 
//   71 /* Private function prototypes -----------------------------------------------*/
//   72 static u32 GetFreeIF(void);
//   73 /* Private functions ---------------------------------------------------------*/
//   74 
//   75 /*******************************************************************************
//   76 * Function Name  : CAN_DeInit                                                
//   77 * Description    : Deinitializes the CAN peripheral registers to their default     
//   78 *                  reset values.                                     
//   79 * Input          : None                                                      
//   80 * Output         : None                                                      
//   81 * Return         : None                                                      
//   82 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function CAN_DeInit
        THUMB
//   83 void CAN_DeInit (void)
//   84 {
CAN_DeInit:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   85   /* Reset the CAN registers values*/
//   86   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_CAN,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+9        ;; #+65536
        BL       MRCC_PeripheralSWResetConfig
//   87   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_CAN,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+9        ;; #+65536
        BL       MRCC_PeripheralSWResetConfig
//   88 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock0
//   89 
//   90 /*******************************************************************************
//   91 * Function Name  : CAN_Init                                                  
//   92 * Description    : Initializes the CAN peripheral according to the specified 
//   93 *                  parameters in the CAN_InitStruct.                                            
//   94 * Input          : CAN_InitStruct: pointer to a CAN_InitTypeDef structure that
//   95 *                  contains the configuration information for the CAN peripheral. 
//   96 * Output         : None                                                      
//   97 * Return         : None                                                      
//   98 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function CAN_Init
        THUMB
//   99 void CAN_Init(CAN_InitTypeDef* CAN_InitStruct)
//  100 {
CAN_Init:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  101   CAN_EnterInitMode(CAN_CR_CCE | CAN_InitStruct->CAN_ConfigParameters);
        LDRB     R1,[R4, #+0]
        MOVS     R0,#+64
        ORRS     R0,R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       CAN_EnterInitMode
//  102   CAN_SetBitrate(CAN_InitStruct->CAN_Bitrate);
        LDR      R0,[R4, #+4]
        BL       CAN_SetBitrate
//  103   CAN_LeaveInitMode();
        BL       CAN_LeaveInitMode
//  104   CAN_LeaveTestMode();
        BL       CAN_LeaveTestMode
//  105 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock1
//  106 
//  107 /*******************************************************************************
//  108 * Function Name  : CAN_StructInit		                        
//  109 * Description    : Fills each CAN_InitStruct member with its reset value.	      
//  110 * Input          : CAN_InitStruct : pointer to a CAN_InitTypeDef structure which       
//  111 *                  will be initialized. 
//  112 * Output         : None                  
//  113 * Return         : None.						      
//  114 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function CAN_StructInit
        THUMB
//  115 void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct)
//  116 {
//  117 /* Reset CAN init structure parameters values */
//  118   CAN_InitStruct->CAN_ConfigParameters = 0x0;
CAN_StructInit:
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  119   CAN_InitStruct->CAN_Bitrate = 0x2301;
        LDR      R1,??CAN_StructInit_0  ;; 0x2301
        STR      R1,[R0, #+4]
//  120 }
        BX       LR               ;; return
        Nop      
        DATA
??CAN_StructInit_0:
        DC32     0x2301
        CFI EndBlock cfiBlock2
//  121 
//  122 /*******************************************************************************
//  123 * Function Name  : CAN_SetBitrate                                            
//  124 * Description    : Setups a standard CAN bitrate.                              
//  125 * Input          : bitrate: specifies the bit rate.                       
//  126 * Output         : None                                                      
//  127 * Return         : None                                                                         
//  128 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function CAN_SetBitrate
        THUMB
//  129 void CAN_SetBitrate(u32 bitrate)
//  130 {
//  131   CAN->BTR = CanTimings[bitrate];     /* write the predefined timing value */
CAN_SetBitrate:
        MOVS     R1,#+2
        MULS     R1,R0,R1
        LDR      R2,??CAN_SetBitrate_0  ;; CanTimings
        LDRH     R1,[R2, R1]
        LDR      R2,??DataTable2  ;; 0xffffc40c
        STRH     R1,[R2, #+0]
//  132   CAN->BRPR = 0; 		       /* clear the Extended Baud Rate Prescaler */
        LDR      R1,??DataTable3  ;; 0xffffc418
        MOVS     R2,#+0
        STRH     R2,[R1, #+0]
//  133 }
        BX       LR               ;; return
        DATA
??CAN_SetBitrate_0:
        DC32     CanTimings
        CFI EndBlock cfiBlock3
//  134 
//  135 /*******************************************************************************
//  136 * Function Name  : CAN_SetTiming                                             
//  137 * Description    : Setups the CAN timing with specific parameters             
//  138 * Input          : - tseg1: specifies Time Segment before the sample point.
//  139 *                    This parameter must be a number between 1 and 16.       
//  140 *                  - tseg2: Time Segment after the sample point. This parameter 
//  141 *                    must be a number between 1 and 8.        
//  142 *                  - sjw: Synchronisation Jump Width. This parameter must be                 
//  143 *                     a number between 1 and 4.
//  144 *                  - brp: Baud Rate Prescaler. This parameter must be a number
//  145 *                    between 1 and 1024.                                         
//  146 * Output         : None                                                      
//  147 * Return         : None                                                                       
//  148 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function CAN_SetTiming
        THUMB
//  149 void CAN_SetTiming(u32 tseg1, u32 tseg2, u32 sjw, u32 brp)
//  150 {
CAN_SetTiming:
        PUSH     {R4,R5}
        CFI R5 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  151   CAN->BTR = CAN_TIMING(tseg1, tseg2, sjw, brp);
        MOVS     R4,R1
        SUBS     R4,R4,#+1
        LSLS     R4,R4,#+29       ;; ZeroExtS R4,R4,#+29,#+29
        LSRS     R4,R4,#+29
        LSLS     R4,R4,#+12
        MOVS     R5,R0
        SUBS     R5,R5,#+1
        LSLS     R5,R5,#+28       ;; ZeroExtS R5,R5,#+28,#+28
        LSRS     R5,R5,#+28
        LSLS     R5,R5,#+8
        ORRS     R5,R5,R4
        MOVS     R4,R2
        SUBS     R4,R4,#+1
        LSLS     R4,R4,#+30       ;; ZeroExtS R4,R4,#+30,#+30
        LSRS     R4,R4,#+30
        LSLS     R4,R4,#+6
        ORRS     R4,R4,R5
        MOVS     R5,R3
        SUBS     R5,R5,#+1
        LSLS     R5,R5,#+26       ;; ZeroExtS R5,R5,#+26,#+26
        LSRS     R5,R5,#+26
        ORRS     R5,R5,R4
        LDR      R4,??DataTable2  ;; 0xffffc40c
        STRH     R5,[R4, #+0]
//  152   CAN->BRPR = ((brp-1) >> 6) & 0x0F;
        SUBS     R4,R3,#+1
        LSRS     R4,R4,#+6
        LSLS     R4,R4,#+28       ;; ZeroExtS R4,R4,#+28,#+28
        LSRS     R4,R4,#+28
        LDR      R5,??DataTable3  ;; 0xffffc418
        STRH     R4,[R5, #+0]
//  153 }
        POP      {R4,R5}
        CFI R4 SameValue
        CFI R5 SameValue
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock4

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable2:
        DC32     0xffffc40c

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable3:
        DC32     0xffffc418
//  154 
//  155 /*******************************************************************************
//  156 * Function Name  : GetFreeIF                                             
//  157 * Description    : Searchs the first free message interface, starting from 0.  
//  158 * Input          : None                                                      
//  159 * Output         : None                                                      
//  160 * Return         : A free message interface number (0 or 1) if found, else 2 
//  161 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function GetFreeIF
        THUMB
//  162 static u32 GetFreeIF(void)
//  163 {
GetFreeIF:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  164   if ((CAN->sMsgObj[0].CRR & CAN_CRR_BUSY) == 0)
        LDR      R0,??DataTable29  ;; 0xffffc420
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BMI      ??GetFreeIF_0
//  165     return 0;
        MOVS     R0,#+0
        B        ??GetFreeIF_1
//  166   else if ((CAN->sMsgObj[1].CRR & CAN_CRR_BUSY) == 0)
??GetFreeIF_0:
        LDR      R0,??DataTable5  ;; 0xffffc480
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BMI      ??GetFreeIF_2
//  167     return 1;
        MOVS     R0,#+1
        B        ??GetFreeIF_1
//  168   else
//  169    return 2;
??GetFreeIF_2:
        MOVS     R0,#+2
??GetFreeIF_1:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock5
//  170 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable5:
        DC32     0xffffc480
//  171 
//  172 /*******************************************************************************
//  173 * Function Name  : CAN_SetUnusedMsgObj                                       
//  174 * Description    : Configures the message object as unused                   
//  175 * Input          : msgobj: specifies the Message object number, from 0 to 31.                      
//  176 * Output         : None                                                      
//  177 * Return         : An ErrorStatus enumuration value:
//  178 *                         - SUCCESS: Interface to treat the message
//  179 *                         - ERROR: No interface found to treat the message
//  180 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function CAN_SetUnusedMsgObj
        THUMB
//  181 ErrorStatus CAN_SetUnusedMsgObj(u32 msgobj)
//  182 {
CAN_SetUnusedMsgObj:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  183   u32 msg_if=0;
        MOVS     R0,#+0
        MOVS     R5,R0
//  184 
//  185   if ((msg_if = GetFreeIF()) == 2)
        BL       GetFreeIF
        MOVS     R5,R0
        CMP      R0,#+2
        BNE      ??CAN_SetUnusedMsgObj_0
//  186   {
//  187     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_SetUnusedMsgObj_1
//  188   }
//  189 
//  190   CAN->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
//  191                            | CAN_CMR_MASK
//  192                            | CAN_CMR_ARB
//  193                            | CAN_CMR_CONTROL
//  194                            | CAN_CMR_DATAA
//  195                            | CAN_CMR_DATAB;
??CAN_SetUnusedMsgObj_0:
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+243
        STRH     R1,[R0, #+4]
//  196 
//  197   CAN->sMsgObj[msg_if].M1R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  198   CAN->sMsgObj[msg_if].M2R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+12]
//  199 
//  200   CAN->sMsgObj[msg_if].A1R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+16]
//  201   CAN->sMsgObj[msg_if].A2R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+20]
//  202 
//  203   CAN->sMsgObj[msg_if].MCR = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+24]
//  204 
//  205   CAN->sMsgObj[msg_if].DA1R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+28]
//  206   CAN->sMsgObj[msg_if].DA2R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+32]
//  207   CAN->sMsgObj[msg_if].DB1R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+36]
//  208   CAN->sMsgObj[msg_if].DB2R = 0;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+40]
//  209 
//  210  CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        MOVS     R2,R4
        ADDS     R2,R2,#+1
        STRH     R2,[R1, R0]
//  211  
//  212  return SUCCESS;
        MOVS     R0,#+1
??CAN_SetUnusedMsgObj_1:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock6
//  213 }
//  214 
//  215 /*******************************************************************************
//  216 * Function Name  : CAN_SetTxMsgObj                                           
//  217 * Description    : Configures the message object as TX.                        
//  218 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                      
//  219 *                  - idType: specifies the identifier type of the frames that
//  220 *                    will be transmitted using this message object.
//  221 *                    This parameter can be one of the following values:
//  222 *                          - CAN_STD_ID (standard ID, 11-bit)
//  223 *                          - CAN_EXT_ID (extended ID, 29-bit)                                
//  224 * Output         : None                                                      
//  225 * Return         : An ErrorStatus enumuration value:
//  226 *                         - SUCCESS: Interface to treat the message
//  227 *                         - ERROR: No interface found to treat the message
//  228 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function CAN_SetTxMsgObj
        THUMB
//  229 ErrorStatus CAN_SetTxMsgObj(u32 msgobj, u32 idType)
//  230 {
CAN_SetTxMsgObj:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
//  231   u32 msg_if=0;
        MOVS     R0,#+0
        MOVS     R6,R0
//  232 
//  233   if ((msg_if = GetFreeIF()) == 2)
        BL       GetFreeIF
        MOVS     R6,R0
        CMP      R0,#+2
        BNE      ??CAN_SetTxMsgObj_0
//  234   {
//  235     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_SetTxMsgObj_1
//  236   }
//  237   
//  238   CAN->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
//  239                            | CAN_CMR_MASK
//  240                            | CAN_CMR_ARB
//  241                            | CAN_CMR_CONTROL
//  242                            | CAN_CMR_DATAA
//  243                            | CAN_CMR_DATAB;
??CAN_SetTxMsgObj_0:
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+243
        STRH     R1,[R0, #+4]
//  244 
//  245   CAN->sMsgObj[msg_if].M1R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  246   CAN->sMsgObj[msg_if].A1R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+16]
//  247 
//  248   if (idType == CAN_STD_ID)
        CMP      R5,#+0
        BNE      ??CAN_SetTxMsgObj_2
//  249   {
//  250     CAN->sMsgObj[msg_if].M2R = CAN_M2R_MDIR;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+128
        LSLS     R1,R1,#+7        ;; #+16384
        STRH     R1,[R0, #+12]
//  251     CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+160
        LSLS     R1,R1,#+8        ;; #+40960
        STRH     R1,[R0, #+20]
        B        ??CAN_SetTxMsgObj_3
//  252   }
//  253   else
//  254   {
//  255     CAN->sMsgObj[msg_if].M2R = CAN_M2R_MDIR | CAN_M2R_MXTD;
??CAN_SetTxMsgObj_2:
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+192
        LSLS     R1,R1,#+8        ;; #+49152
        STRH     R1,[R0, #+12]
//  256     CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR | CAN_A2R_XTD;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+224
        LSLS     R1,R1,#+8        ;; #+57344
        STRH     R1,[R0, #+20]
//  257   }
//  258 
//  259   CAN->sMsgObj[msg_if].MCR = CAN_MCR_TXIE | CAN_MCR_EOB;
??CAN_SetTxMsgObj_3:
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+136
        LSLS     R1,R1,#+4        ;; #+2176
        STRH     R1,[R0, #+24]
//  260 
//  261   CAN->sMsgObj[msg_if].DA1R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+28]
//  262   CAN->sMsgObj[msg_if].DA2R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+32]
//  263   CAN->sMsgObj[msg_if].DB1R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+36]
//  264   CAN->sMsgObj[msg_if].DB2R = 0;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+40]
//  265 
//  266   CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
        MOVS     R0,#+96
        MULS     R0,R6,R0
        LDR      R1,??DataTable29  ;; 0xffffc420
        MOVS     R2,R4
        ADDS     R2,R2,#+1
        STRH     R2,[R1, R0]
//  267   
//  268   return SUCCESS;
        MOVS     R0,#+1
??CAN_SetTxMsgObj_1:
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock7
//  269 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable29:
        DC32     0xffffc420
//  270 
//  271 /*******************************************************************************
//  272 * Function Name  : CAN_SetRxMsgObj                                           
//  273 * Description    : Configures the message object as RX.                        
//  274 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                    
//  275 *                  - idType: specifies the identifier type of the frames that
//  276 *                    will be transmitted using this message object.
//  277 *                    This parameter can be one of the following values:
//  278 *                          - CAN_STD_ID (standard ID, 11-bit)
//  279 *                          - CAN_EXT_ID (extended ID, 29-bit)                               
//  280 *                  - idLow: specifies the low part of the identifier range used      
//  281 *                    for acceptance filtering.
//  282 *                  - idHigh: specifies the high part of the identifier range    
//  283 *                    used for acceptance filtering.
//  284 *                  - singleOrFifoLast: specifies the end-of-buffer indicator.
//  285 *                    This parameter can be one of the following values:
//  286 *                          - TRUE: for a single receive object or a FIFO receive
//  287 *                            object that is the last one of the FIFO. 
//  288 *                          - FALSE: for a FIFO receive object that is not the 
//  289 *                            last one. 
//  290 * Output         : None                                                      
//  291 * Return         : An ErrorStatus enumuration value:
//  292 *                         - SUCCESS: Interface to treat the message
//  293 *                         - ERROR: No interface found to treat the message
//  294 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function CAN_SetRxMsgObj
        THUMB
//  295 ErrorStatus CAN_SetRxMsgObj(u32 msgobj, u32 idType, u32 idLow, u32 idHigh, bool singleOrFifoLast)
//  296 {
CAN_SetRxMsgObj:
        PUSH     {R0,R1,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+28
        SUB      SP,SP,#+4
        CFI CFA R13+32
        MOVS     R4,R2
        MOVS     R5,R3
        ADD      R0,SP,#+32
        LDRB     R6,[R0, #+0]
//  297   u32 msg_if=0;
        MOVS     R0,#+0
        MOVS     R7,R0
//  298 
//  299   if ((msg_if = GetFreeIF()) == 2)
        BL       GetFreeIF
        MOVS     R7,R0
        CMP      R0,#+2
        BNE      ??CAN_SetRxMsgObj_0
//  300   {
//  301     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_SetRxMsgObj_1
//  302   }
//  303   
//  304   CAN->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
//  305                            | CAN_CMR_MASK
//  306                            | CAN_CMR_ARB
//  307                            | CAN_CMR_CONTROL
//  308                            | CAN_CMR_DATAA
//  309                            | CAN_CMR_DATAB;
??CAN_SetRxMsgObj_0:
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+243
        STRH     R1,[R0, #+4]
//  310 
//  311   if (idType == CAN_STD_ID)
        LDR      R0,[SP, #+8]
        CMP      R0,#+0
        BNE      ??CAN_SetRxMsgObj_3
//  312   {
//  313     CAN->sMsgObj[msg_if].M1R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  314     CAN->sMsgObj[msg_if].M2R = STD_RANGE_ID_MSK(idLow, idHigh);
        MOVS     R0,R5
        MOVS     R1,R4
        SUBS     R0,R0,R1
        MVNS     R1,R0
        LSLS     R0,R1,#+21       ;; ZeroExtS R0,R1,#+21,#+21
        LSRS     R0,R0,#+21
        LSLS     R0,R0,#+2
        MOVS     R1,#+96
        MULS     R1,R7,R1
        LDR      R2,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R1,R2,R1
        STRH     R0,[R1, #+12]
//  315 
//  316     CAN->sMsgObj[msg_if].A1R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+16]
//  317     CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | STD_RANGE_ID_ARB(idLow, idHigh);
        MOVS     R0,R4
        MOVS     R1,R5
        ANDS     R1,R1,R0
        LSLS     R1,R1,#+2
        MOVS     R0,#+128
        LSLS     R0,R0,#+8        ;; #+32768
        ORRS     R0,R0,R1
        MOVS     R1,#+96
        MULS     R1,R7,R1
        LDR      R2,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R1,R2,R1
        STRH     R0,[R1, #+20]
        B        ??CAN_SetRxMsgObj_4
//  318   }
//  319   else
//  320   {
//  321     CAN->sMsgObj[msg_if].M1R = EXT_RANGE_ID_MSK_L(idLow, idHigh);
??CAN_SetRxMsgObj_3:
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        SUBS     R1,R5,R4
        MVNS     R2,R1
        MOVS     R1,R2
        LSRS     R1,R1,#+11
        STRH     R1,[R0, #+8]
//  322     CAN->sMsgObj[msg_if].M2R = CAN_M2R_MXTD | EXT_RANGE_ID_MSK_H(idLow, idHigh);
        MOVS     R0,R5
        MOVS     R1,R4
        SUBS     R0,R0,R1
        MVNS     R1,R0
        LSLS     R0,R1,#+21       ;; ZeroExtS R0,R1,#+21,#+21
        LSRS     R0,R0,#+21
        LSLS     R0,R0,#+2
        SUBS     R1,R5,R4
        MVNS     R2,R1
        MOVS     R1,R2
        LSRS     R1,R1,#+27
        LSLS     R1,R1,#+30       ;; ZeroExtS R1,R1,#+30,#+30
        LSRS     R1,R1,#+30
        ORRS     R1,R1,R0
        MOVS     R0,#+128
        LSLS     R0,R0,#+8        ;; #+32768
        ORRS     R0,R0,R1
        MOVS     R1,#+96
        MULS     R1,R7,R1
        LDR      R2,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R1,R2,R1
        STRH     R0,[R1, #+12]
//  323 
//  324     CAN->sMsgObj[msg_if].A1R = EXT_RANGE_ID_ARB_L(idLow, idHigh);
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,R5
        ANDS     R1,R1,R4
        LSRS     R1,R1,#+11
        STRH     R1,[R0, #+16]
//  325     CAN->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_XTD | EXT_RANGE_ID_ARB_H(idLow, idHigh);
        MOVS     R0,R4
        MOVS     R1,R5
        ANDS     R1,R1,R0
        LSLS     R0,R1,#+2
        MOVS     R1,R5
        ANDS     R1,R1,R4
        LSRS     R1,R1,#+27
        LSLS     R1,R1,#+30       ;; ZeroExtS R1,R1,#+30,#+30
        LSRS     R1,R1,#+30
        ORRS     R1,R1,R0
        MOVS     R0,#+192
        LSLS     R0,R0,#+8        ;; #+49152
        ORRS     R0,R0,R1
        MOVS     R1,#+96
        MULS     R1,R7,R1
        LDR      R2,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R1,R2,R1
        STRH     R0,[R1, #+20]
//  326   }
//  327 
//  328   CAN->sMsgObj[msg_if].MCR = CAN_MCR_RXIE | CAN_MCR_UMASK | (singleOrFifoLast ? CAN_MCR_EOB : 0);
??CAN_SetRxMsgObj_4:
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        CMP      R6,#+0
        BEQ      ??CAN_SetRxMsgObj_5
        MOVS     R0,#+128
        B        ??CAN_SetRxMsgObj_6
??CAN_SetRxMsgObj_5:
        MOVS     R0,#+0
??CAN_SetRxMsgObj_6:
        MOVS     R1,#+96
        MULS     R1,R7,R1
        LDR      R2,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R1,R2,R1
        MOVS     R2,#+160
        LSLS     R2,R2,#+5        ;; #+5120
        ORRS     R2,R2,R0
        STRH     R2,[R1, #+24]
//  329 
//  330   CAN->sMsgObj[msg_if].DA1R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+28]
//  331   CAN->sMsgObj[msg_if].DA2R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+32]
//  332   CAN->sMsgObj[msg_if].DB1R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+36]
//  333   CAN->sMsgObj[msg_if].DB2R = 0;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+40]
//  334 
//  335   CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
        MOVS     R0,#+96
        MULS     R0,R7,R0
        LDR      R1,??CAN_SetRxMsgObj_2  ;; 0xffffc420
        LDR      R2,[SP, #+4]
        ADDS     R2,R2,#+1
        STRH     R2,[R1, R0]
//  336   
//  337   return SUCCESS;
        MOVS     R0,#+1
??CAN_SetRxMsgObj_1:
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_SetRxMsgObj_2:
        DC32     0xffffc420
        CFI EndBlock cfiBlock8
//  338 }
//  339 
//  340 /*******************************************************************************
//  341 * Function Name  : CAN_InvalidateAllMsgObj				      
//  342 * Description    : Configures all the message objects as unused.               
//  343 * Input          : None                                                      
//  344 * Output         : None                                                      
//  345 * Return         : None                                                      
//  346 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function CAN_InvalidateAllMsgObj
        THUMB
//  347 void CAN_InvalidateAllMsgObj(void)
//  348 {
CAN_InvalidateAllMsgObj:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  349   u32 i=0;
        MOVS     R0,#+0
        MOVS     R4,R0
//  350   for (i = 0; i < 32; i++)
        MOVS     R0,#+0
        MOVS     R4,R0
??CAN_InvalidateAllMsgObj_0:
        CMP      R4,#+32
        BCS      ??CAN_InvalidateAllMsgObj_1
//  351     CAN_SetUnusedMsgObj(i);
        MOVS     R0,R4
        BL       CAN_SetUnusedMsgObj
        ADDS     R4,R4,#+1
        B        ??CAN_InvalidateAllMsgObj_0
//  352 }
??CAN_InvalidateAllMsgObj_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock9
//  353 
//  354 
//  355 /*******************************************************************************
//  356 * Function Name  : CAN_ReleaseMessage					      
//  357 * Description    : Releases the message object                                
//  358 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                     
//  359 * Output         : None                                                      
//  360 * Return         : An ErrorStatus enumuration value:
//  361 *                         - SUCCESS: Interface to treat the message
//  362 *                         - ERROR: No interface found to treat the message
//  363 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function CAN_ReleaseMessage
        THUMB
//  364 ErrorStatus CAN_ReleaseMessage(u32 msgobj)
//  365 {
CAN_ReleaseMessage:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  366   u32 msg_if=0;
        MOVS     R0,#+0
        MOVS     R5,R0
//  367 
//  368   if ((msg_if = GetFreeIF()) == 2)
        BL       GetFreeIF
        MOVS     R5,R0
        CMP      R0,#+2
        BNE      ??CAN_ReleaseMessage_0
//  369   {
//  370     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_ReleaseMessage_1
//  371   }
//  372 
//  373   CAN->sMsgObj[msg_if].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
??CAN_ReleaseMessage_0:
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??CAN_ReleaseMessage_2  ;; 0xffffc420
        ADDS     R0,R1,R0
        MOVS     R1,#+12
        STRH     R1,[R0, #+4]
//  374   CAN->sMsgObj[msg_if].CRR = 1 + msgobj;
        MOVS     R0,#+96
        MULS     R0,R5,R0
        LDR      R1,??CAN_ReleaseMessage_2  ;; 0xffffc420
        MOVS     R2,R4
        ADDS     R2,R2,#+1
        STRH     R2,[R1, R0]
//  375   
//  376   return SUCCESS;
        MOVS     R0,#+1
??CAN_ReleaseMessage_1:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_ReleaseMessage_2:
        DC32     0xffffc420
        CFI EndBlock cfiBlock10
//  377 }
//  378 
//  379 /*******************************************************************************
//  380 * Function Name  : CAN_SendMessage                                           
//  381 * Description    : Start transmission of a message                           
//  382 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                    
//  383 *                : - pCanMsg: pointer to the message structure containing data     
//  384 *                    to transmit.
//  385 * Output         : None                                                      
//  386 * Return         : An ErrorStatus enumuration value:
//  387 *                         - SUCCESS: Transmission OK
//  388 *                         - ERROR: No transmission
//  389 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function CAN_SendMessage
        THUMB
//  390 ErrorStatus CAN_SendMessage(u32 msgobj, canmsg* pCanMsg)
//  391 {
CAN_SendMessage:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R2,R0
//  392   if (CAN->sMsgObj[0].CRR & CAN_CRR_BUSY)
        LDR      R0,??CAN_SendMessage_0  ;; 0xffffc420
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BPL      ??CAN_SendMessage_1
//  393   {
//  394     return ERROR;                    
        MOVS     R0,#+0
        B        ??CAN_SendMessage_2
//  395   }
//  396 
//  397   CAN->SR &= ~CAN_SR_TXOK;
??CAN_SendMessage_1:
        LDR      R0,??CAN_SendMessage_0+0x4  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LDR      R3,??CAN_SendMessage_0+0x8  ;; 0xfff7
        ANDS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x4  ;; 0xffffc404
        STRH     R3,[R0, #+0]
//  398 
//  399   /* read the Arbitration and Message Control*/
//  400   CAN->sMsgObj[0].CMR = CAN_CMR_ARB | CAN_CMR_CONTROL;
        LDR      R0,??CAN_SendMessage_0+0xC  ;; 0xffffc424
        MOVS     R3,#+48
        STRH     R3,[R0, #+0]
//  401 
//  402   CAN->sMsgObj[0].CRR = 1 + msgobj;
        MOVS     R0,R2
        ADDS     R0,R0,#+1
        LDR      R3,??CAN_SendMessage_0  ;; 0xffffc420
        STRH     R0,[R3, #+0]
//  403 
//  404   if (CAN->sMsgObj[0].CRR & CAN_CRR_BUSY)
        LDR      R0,??CAN_SendMessage_0  ;; 0xffffc420
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BPL      ??CAN_SendMessage_3
//  405   {
//  406     return ERROR;                    
        MOVS     R0,#+0
        B        ??CAN_SendMessage_2
//  407   }
//  408 
//  409   /* update the contents needed for transmission*/
//  410   CAN->sMsgObj[0].CMR = CAN_CMR_WRRD
//  411                       | CAN_CMR_ARB
//  412                       | CAN_CMR_CONTROL
//  413                       | CAN_CMR_DATAA
//  414                       | CAN_CMR_DATAB;
??CAN_SendMessage_3:
        LDR      R0,??CAN_SendMessage_0+0xC  ;; 0xffffc424
        MOVS     R3,#+179
        STRH     R3,[R0, #+0]
//  415 
//  416   if ((CAN->sMsgObj[0].A2R & CAN_A2R_XTD) == 0)
        LDR      R0,??CAN_SendMessage_0+0x10  ;; 0xffffc434
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+17
        BMI      ??CAN_SendMessage_4
//  417   {
//  418     /* standard ID*/
//  419     CAN->sMsgObj[0].A1R = 0;
        LDR      R0,??CAN_SendMessage_0+0x14  ;; 0xffffc430
        MOVS     R3,#+0
        STRH     R3,[R0, #+0]
//  420     CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
        LDR      R0,??CAN_SendMessage_0+0x10  ;; 0xffffc434
        LDRH     R0,[R0, #+0]
        MOVS     R3,#+224
        LSLS     R3,R3,#+8        ;; #+57344
        ANDS     R3,R3,R0
        LDR      R0,[R1, #+4]
        LDR      R4,[R1, #+4]
        ANDS     R4,R4,R0
        LSLS     R0,R4,#+2
        ORRS     R0,R0,R3
        LDR      R3,??CAN_SendMessage_0+0x10  ;; 0xffffc434
        STRH     R0,[R3, #+0]
        B        ??CAN_SendMessage_5
//  421   }
//  422   else
//  423   {
//  424     /* extended ID AKHE*/
//  425     //CAN->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
//  426     //CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
//  427     CAN->sMsgObj[0].A1R = pCanMsg->Id & 0xffff;
??CAN_SendMessage_4:
        LDR      R0,[R1, #+4]
        LDR      R3,??CAN_SendMessage_0+0x14  ;; 0xffffc430
        STRH     R0,[R3, #+0]
//  428     CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | ( ( pCanMsg->Id >> 16 ) & 0xffff);
        LDR      R0,??CAN_SendMessage_0+0x10  ;; 0xffffc434
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R3,#+224
        LSLS     R3,R3,#+8        ;; #+57344
        ANDS     R3,R3,R0
        LDR      R0,[R1, #+4]
        LSRS     R0,R0,#+16
        ORRS     R0,R0,R3
        LDR      R3,??CAN_SendMessage_0+0x10  ;; 0xffffc434
        STRH     R0,[R3, #+0]
//  429   }
//  430 
//  431   CAN->sMsgObj[0].MCR = (CAN->sMsgObj[0].MCR & 0xFEF0) | CAN_MCR_NEWDAT | CAN_MCR_TXRQST | pCanMsg->Dlc;
??CAN_SendMessage_5:
        LDR      R0,??CAN_SendMessage_0+0x18  ;; 0xffffc438
        LDRH     R0,[R0, #+0]
        LDR      R3,??CAN_SendMessage_0+0x1C  ;; 0xfef0
        ANDS     R3,R3,R0
        LDRB     R0,[R1, #+8]
        ORRS     R0,R0,R3
        MOVS     R3,#+129
        LSLS     R3,R3,#+8        ;; #+33024
        ORRS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x18  ;; 0xffffc438
        STRH     R3,[R0, #+0]
//  432 
//  433   CAN->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
        LDRB     R0,[R1, #+10]
        LSLS     R0,R0,#+8
        LDRB     R3,[R1, #+9]
        ORRS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x20  ;; 0xffffc43c
        STRH     R3,[R0, #+0]
//  434   CAN->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
        LDRB     R0,[R1, #+12]
        LSLS     R0,R0,#+8
        LDRB     R3,[R1, #+11]
        ORRS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x24  ;; 0xffffc440
        STRH     R3,[R0, #+0]
//  435   CAN->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
        LDRB     R0,[R1, #+14]
        LSLS     R0,R0,#+8
        LDRB     R3,[R1, #+13]
        ORRS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x28  ;; 0xffffc444
        STRH     R3,[R0, #+0]
//  436   CAN->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];
        LDRB     R0,[R1, #+16]
        LSLS     R0,R0,#+8
        LDRB     R3,[R1, #+15]
        ORRS     R3,R3,R0
        LDR      R0,??CAN_SendMessage_0+0x2C  ;; 0xffffc448
        STRH     R3,[R0, #+0]
//  437 
//  438   CAN->sMsgObj[0].CRR = 1 + msgobj;
        MOVS     R0,R2
        ADDS     R0,R0,#+1
        LDR      R3,??CAN_SendMessage_0  ;; 0xffffc420
        STRH     R0,[R3, #+0]
//  439 
//  440   return SUCCESS;
        MOVS     R0,#+1
??CAN_SendMessage_2:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??CAN_SendMessage_0:
        DC32     0xffffc420
        DC32     0xffffc404
        DC32     0xfff7
        DC32     0xffffc424
        DC32     0xffffc434
        DC32     0xffffc430
        DC32     0xffffc438
        DC32     0xfef0
        DC32     0xffffc43c
        DC32     0xffffc440
        DC32     0xffffc444
        DC32     0xffffc448
        CFI EndBlock cfiBlock11
//  441 }
//  442 
//  443 /*******************************************************************************
//  444 * Function Name  : CAN_ReceiveMessage                                        
//  445 * Description    : Gets the message, if received.
//  446 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                     
//  447 *                  - release: specifies the message release indicator.
//  448 *                    This parameter can be one of the following values:
//  449 *                          - TRUE: the message object is released when getting  
//  450 *                            the data.
//  451 *                          - FALSE: the message object is not released.
//  452 *                  - pCanMsg: pointer to the message structure where received   
//  453 *                    data is copied.
//  454 * Output         : None                                                      
//  455 * Return         : An ErrorStatus enumuration value:
//  456 *                         - SUCCESS: Reception OK
//  457 *                         - ERROR: No message pending
//  458 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function CAN_ReceiveMessage
        THUMB
//  459 ErrorStatus CAN_ReceiveMessage(u32 msgobj, bool release, canmsg* pCanMsg)
//  460 {
CAN_ReceiveMessage:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
        MOVS     R6,R2
//  461   if (!CAN_IsMessageWaiting(msgobj))
        MOVS     R0,R4
        BL       CAN_IsMessageWaiting
        CMP      R0,#+0
        BNE      ??CAN_ReceiveMessage_0
//  462   {
//  463     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_ReceiveMessage_1
//  464   }
//  465 
//  466   CAN->SR &= ~CAN_SR_RXOK;
??CAN_ReceiveMessage_0:
        LDR      R0,??CAN_ReceiveMessage_2  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_ReceiveMessage_2+0x4  ;; 0xffef
        ANDS     R1,R1,R0
        LDR      R0,??CAN_ReceiveMessage_2  ;; 0xffffc404
        STRH     R1,[R0, #+0]
//  467 
//  468   /* read the message contents*/
//  469   CAN->sMsgObj[1].CMR = CAN_CMR_MASK
//  470                       | CAN_CMR_ARB
//  471                       | CAN_CMR_CONTROL
//  472                       | CAN_CMR_CLRINTPND
//  473                       | (release ? CAN_CMR_TXRQSTNEWDAT : 0)
//  474                       | CAN_CMR_DATAA
//  475                       | CAN_CMR_DATAB;
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+0
        BEQ      ??CAN_ReceiveMessage_3
        MOVS     R0,#+4
        B        ??CAN_ReceiveMessage_4
??CAN_ReceiveMessage_3:
        MOVS     R0,#+0
??CAN_ReceiveMessage_4:
        MOVS     R1,#+123
        ORRS     R1,R1,R0
        LDR      R0,??CAN_ReceiveMessage_2+0x8  ;; 0xffffc484
        STRH     R1,[R0, #+0]
//  476 
//  477   CAN->sMsgObj[1].CRR = 1 + msgobj;
        MOVS     R0,R4
        ADDS     R0,R0,#+1
        LDR      R1,??CAN_ReceiveMessage_2+0xC  ;; 0xffffc480
        STRH     R0,[R1, #+0]
//  478 
//  479   if (CAN->sMsgObj[1].CRR & CAN_CRR_BUSY)
        LDR      R0,??CAN_ReceiveMessage_2+0xC  ;; 0xffffc480
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BPL      ??CAN_ReceiveMessage_5
//  480   {
//  481     return ERROR;                    
        MOVS     R0,#+0
        B        ??CAN_ReceiveMessage_1
//  482   }
//  483   
//  484   if ((CAN->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
??CAN_ReceiveMessage_5:
        LDR      R0,??CAN_ReceiveMessage_2+0x10  ;; 0xffffc494
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+17
        BMI      ??CAN_ReceiveMessage_6
//  485   {
//  486     /* standard ID*/
//  487     pCanMsg->IdType = CAN_STD_ID;
        MOVS     R0,#+0
        STR      R0,[R6, #+0]
//  488     pCanMsg->Id = (CAN->sMsgObj[1].A2R >> 2) & 0x07FF;
        LDR      R0,??CAN_ReceiveMessage_2+0x10  ;; 0xffffc494
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        ASRS     R0,R0,#+2
        LSLS     R0,R0,#+21       ;; ZeroExtS R0,R0,#+21,#+21
        LSRS     R0,R0,#+21
        STR      R0,[R6, #+4]
        B        ??CAN_ReceiveMessage_7
//  489   }
//  490   else
//  491   {
//  492     /* extended ID AKHE*/
//  493     pCanMsg->IdType = CAN_EXT_ID;
??CAN_ReceiveMessage_6:
        MOVS     R0,#+1
        STR      R0,[R6, #+0]
//  494     //pCanMsg->Id  = ((CAN->sMsgObj[1].A2R >> 2) & 0x07FF); 
//  495     //pCanMsg->Id |= ((u32)CAN->sMsgObj[1].A1R << 11);
//  496     //pCanMsg->Id |= (((u32)CAN->sMsgObj[1].A2R & 0x0003) << 27);
//  497     pCanMsg->Id = CAN->sMsgObj[1].A1R + ( ( CAN->sMsgObj[1].A2R & 0x1fff ) >> 16 );
        LDR      R0,??CAN_ReceiveMessage_2+0x14  ;; 0xffffc490
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_ReceiveMessage_2+0x10  ;; 0xffffc494
        LDRH     R1,[R1, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        STR      R0,[R6, #+4]
//  498   }
//  499 
//  500   pCanMsg->Dlc = CAN->sMsgObj[1].MCR & 0x0F;
??CAN_ReceiveMessage_7:
        LDR      R0,??CAN_ReceiveMessage_2+0x18  ;; 0xffffc498
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+28       ;; ZeroExtS R0,R0,#+28,#+28
        LSRS     R0,R0,#+28
        STRB     R0,[R6, #+8]
//  501 
//  502   pCanMsg->Data[0] = (u8) CAN->sMsgObj[1].DA1R;
        LDR      R0,??CAN_ReceiveMessage_2+0x1C  ;; 0xffffc49c
        LDRH     R0,[R0, #+0]
        STRB     R0,[R6, #+9]
//  503   pCanMsg->Data[1] = (u8)(CAN->sMsgObj[1].DA1R >> 8);
        LDR      R0,??CAN_ReceiveMessage_2+0x1C  ;; 0xffffc49c
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+10]
//  504   pCanMsg->Data[2] = (u8) CAN->sMsgObj[1].DA2R;
        LDR      R0,??CAN_ReceiveMessage_2+0x20  ;; 0xffffc4a0
        LDRH     R0,[R0, #+0]
        STRB     R0,[R6, #+11]
//  505   pCanMsg->Data[3] = (u8)(CAN->sMsgObj[1].DA2R >> 8);
        LDR      R0,??CAN_ReceiveMessage_2+0x20  ;; 0xffffc4a0
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+12]
//  506   pCanMsg->Data[4] = (u8) CAN->sMsgObj[1].DB1R;
        LDR      R0,??CAN_ReceiveMessage_2+0x24  ;; 0xffffc4a4
        LDRH     R0,[R0, #+0]
        STRB     R0,[R6, #+13]
//  507   pCanMsg->Data[5] = (u8)(CAN->sMsgObj[1].DB1R >> 8);
        LDR      R0,??CAN_ReceiveMessage_2+0x24  ;; 0xffffc4a4
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+14]
//  508   pCanMsg->Data[6] = (u8) CAN->sMsgObj[1].DB2R;
        LDR      R0,??CAN_ReceiveMessage_2+0x28  ;; 0xffffc4a8
        LDRH     R0,[R0, #+0]
        STRB     R0,[R6, #+15]
//  509   pCanMsg->Data[7] = (u8)(CAN->sMsgObj[1].DB2R >> 8);
        LDR      R0,??CAN_ReceiveMessage_2+0x28  ;; 0xffffc4a8
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+16]
//  510 
//  511   return SUCCESS;
        MOVS     R0,#+1
??CAN_ReceiveMessage_1:
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_ReceiveMessage_2:
        DC32     0xffffc404
        DC32     0xffef
        DC32     0xffffc484
        DC32     0xffffc480
        DC32     0xffffc494
        DC32     0xffffc490
        DC32     0xffffc498
        DC32     0xffffc49c
        DC32     0xffffc4a0
        DC32     0xffffc4a4
        DC32     0xffffc4a8
        CFI EndBlock cfiBlock12
//  512 }
//  513 
//  514 /*******************************************************************************
//  515 * Function Name  : CAN_WaitEndOfTx                                           
//  516 * Description    : Waits until current transmission is finished.               
//  517 * Input          : None                                                      
//  518 * Output         : None                                                      
//  519 * Return         : An ErrorStatus enumuration value:
//  520 *                         - SUCCESS: Transmission ended
//  521 *                         - ERROR: Transmission did not occur yet
//  522 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function CAN_WaitEndOfTx
        THUMB
//  523 ErrorStatus CAN_WaitEndOfTx(void)
//  524 {
CAN_WaitEndOfTx:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  525   // Changed by AKHE
//  526   while ( 0 == ( CAN->SR & CAN_SR_TXOK ) );
??CAN_WaitEndOfTx_0:
        LDR      R0,??CAN_WaitEndOfTx_1  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+28
        BPL      ??CAN_WaitEndOfTx_0
//  527   CAN->SR &= ~CAN_SR_TXOK;
        LDR      R0,??CAN_WaitEndOfTx_1  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_WaitEndOfTx_1+0x4  ;; 0xfff7
        ANDS     R1,R1,R0
        LDR      R0,??CAN_WaitEndOfTx_1  ;; 0xffffc404
        STRH     R1,[R0, #+0]
//  528 /*        
//  529   if ((CAN->SR & CAN_SR_TXOK) == 0)
//  530   {
//  531     return ERROR;
//  532   }
//  533   CAN->SR &= ~CAN_SR_TXOK;
//  534 */  
//  535   return SUCCESS;
        MOVS     R0,#+1
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_WaitEndOfTx_1:
        DC32     0xffffc404
        DC32     0xfff7
        CFI EndBlock cfiBlock13
//  536 }
//  537 
//  538 /*******************************************************************************
//  539 * Function Name  : CAN_BasicSendMessage                                      
//  540 * Description    : Starts transmission of a message in BASIC mode. This mode 
//  541 *                  does not use the message RAM.             
//  542 * Input          : pCanMsg: Pointer to the message structure containing data to       
//  543 *                  transmit.                                                  
//  544 * Output         : None                                                      
//  545 * Return         : An ErrorStatus enumuration value:
//  546 *                         - SUCCESS: Transmission OK
//  547 *                         - ERROR: No transmission
//  548 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function CAN_BasicSendMessage
        THUMB
//  549 ErrorStatus CAN_BasicSendMessage(canmsg* pCanMsg)
//  550 {
CAN_BasicSendMessage:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  551   /* clear NewDat bit in IF2 to detect next reception*/
//  552   CAN->sMsgObj[1].MCR &= ~CAN_MCR_NEWDAT;
        LDR      R0,??CAN_BasicSendMessage_0  ;; 0xffffc498
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+17       ;; ZeroExtS R0,R0,#+17,#+17
        LSRS     R0,R0,#+17
        LDR      R2,??CAN_BasicSendMessage_0  ;; 0xffffc498
        STRH     R0,[R2, #+0]
//  553 
//  554   CAN->SR &= ~CAN_SR_TXOK;
        LDR      R0,??CAN_BasicSendMessage_0+0x4  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LDR      R2,??CAN_BasicSendMessage_0+0x8  ;; 0xfff7
        ANDS     R2,R2,R0
        LDR      R0,??CAN_BasicSendMessage_0+0x4  ;; 0xffffc404
        STRH     R2,[R0, #+0]
//  555   CAN->sMsgObj[0].CMR = CAN_CMR_WRRD
//  556                       | CAN_CMR_ARB
//  557                       | CAN_CMR_CONTROL
//  558                       | CAN_CMR_DATAA
//  559                       | CAN_CMR_DATAB;
        LDR      R0,??CAN_BasicSendMessage_0+0xC  ;; 0xffffc424
        MOVS     R2,#+179
        STRH     R2,[R0, #+0]
//  560 
//  561   if (pCanMsg->IdType == CAN_STD_ID)
        LDR      R0,[R1, #+0]
        CMP      R0,#+0
        BNE      ??CAN_BasicSendMessage_1
//  562   {
//  563     /* standard ID*/
//  564     CAN->sMsgObj[0].A1R = 0;
        LDR      R0,??CAN_BasicSendMessage_0+0x10  ;; 0xffffc430
        MOVS     R2,#+0
        STRH     R2,[R0, #+0]
//  565     CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
        LDR      R0,??CAN_BasicSendMessage_0+0x14  ;; 0xffffc434
        LDRH     R0,[R0, #+0]
        MOVS     R2,#+224
        LSLS     R2,R2,#+8        ;; #+57344
        ANDS     R2,R2,R0
        LDR      R0,[R1, #+4]
        LDR      R3,[R1, #+4]
        ANDS     R3,R3,R0
        LSLS     R0,R3,#+2
        ORRS     R0,R0,R2
        LDR      R2,??CAN_BasicSendMessage_0+0x14  ;; 0xffffc434
        STRH     R0,[R2, #+0]
        B        ??CAN_BasicSendMessage_2
//  566   }
//  567   else
//  568   {
//  569     /* extended ID AKHE*/
//  570     //CAN->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
//  571     //CAN->sMsgObj[0].A2R = ((CAN->sMsgObj[0].A2R) & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
//  572     CAN->sMsgObj[0].A1R = pCanMsg->Id & 0xffff;
??CAN_BasicSendMessage_1:
        LDR      R0,[R1, #+4]
        LDR      R2,??CAN_BasicSendMessage_0+0x10  ;; 0xffffc430
        STRH     R0,[R2, #+0]
//  573     CAN->sMsgObj[0].A2R = (CAN->sMsgObj[0].A2R & 0xE000) | ( ( pCanMsg->Id >> 16 ) & 0xffff);
        LDR      R0,??CAN_BasicSendMessage_0+0x14  ;; 0xffffc434
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R2,#+224
        LSLS     R2,R2,#+8        ;; #+57344
        ANDS     R2,R2,R0
        LDR      R0,[R1, #+4]
        LSRS     R0,R0,#+16
        ORRS     R0,R0,R2
        LDR      R2,??CAN_BasicSendMessage_0+0x14  ;; 0xffffc434
        STRH     R0,[R2, #+0]
//  574   }
//  575 
//  576   CAN->sMsgObj[0].MCR = (CAN->sMsgObj[0].MCR & 0xFCF0) | pCanMsg->Dlc;
??CAN_BasicSendMessage_2:
        LDR      R0,??CAN_BasicSendMessage_0+0x18  ;; 0xffffc438
        LDRH     R0,[R0, #+0]
        LDR      R2,??CAN_BasicSendMessage_0+0x1C  ;; 0xfcf0
        ANDS     R2,R2,R0
        LDRB     R0,[R1, #+8]
        ORRS     R0,R0,R2
        LDR      R2,??CAN_BasicSendMessage_0+0x18  ;; 0xffffc438
        STRH     R0,[R2, #+0]
//  577 
//  578   CAN->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
        LDRB     R0,[R1, #+10]
        LSLS     R0,R0,#+8
        LDRB     R2,[R1, #+9]
        ORRS     R2,R2,R0
        LDR      R0,??CAN_BasicSendMessage_0+0x20  ;; 0xffffc43c
        STRH     R2,[R0, #+0]
//  579   CAN->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
        LDRB     R0,[R1, #+12]
        LSLS     R0,R0,#+8
        LDRB     R2,[R1, #+11]
        ORRS     R2,R2,R0
        LDR      R0,??CAN_BasicSendMessage_0+0x24  ;; 0xffffc440
        STRH     R2,[R0, #+0]
//  580   CAN->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
        LDRB     R0,[R1, #+14]
        LSLS     R0,R0,#+8
        LDRB     R2,[R1, #+13]
        ORRS     R2,R2,R0
        LDR      R0,??CAN_BasicSendMessage_0+0x28  ;; 0xffffc444
        STRH     R2,[R0, #+0]
//  581   CAN->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];
        LDRB     R0,[R1, #+16]
        LSLS     R0,R0,#+8
        LDRB     R2,[R1, #+15]
        ORRS     R2,R2,R0
        LDR      R0,??CAN_BasicSendMessage_0+0x2C  ;; 0xffffc448
        STRH     R2,[R0, #+0]
//  582 
//  583   /* request transmission*/
//  584   if (CAN->sMsgObj[0].CRR == CAN_CRR_BUSY )
        LDR      R0,??CAN_BasicSendMessage_0+0x30  ;; 0xffffc420
        LDRH     R0,[R0, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+8        ;; #+32768
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R0,R2
        BNE      ??CAN_BasicSendMessage_3
//  585   {
//  586     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_BasicSendMessage_4
//  587   }
//  588 
//  589   return SUCCESS;
??CAN_BasicSendMessage_3:
        MOVS     R0,#+1
??CAN_BasicSendMessage_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_BasicSendMessage_0:
        DC32     0xffffc498
        DC32     0xffffc404
        DC32     0xfff7
        DC32     0xffffc424
        DC32     0xffffc430
        DC32     0xffffc434
        DC32     0xffffc438
        DC32     0xfcf0
        DC32     0xffffc43c
        DC32     0xffffc440
        DC32     0xffffc444
        DC32     0xffffc448
        DC32     0xffffc420
        CFI EndBlock cfiBlock14
//  590 }
//  591 
//  592 /*******************************************************************************
//  593 * Function Name  : CAN_BasicReceiveMessage                                   
//  594 * Description    : Gets the message in BASIC mode, if received. This mode does
//  595 *                  not use the message RAM.                
//  596 * Input          : pCanMsg: pointer to the message structure where message is copied.    
//  597 * Output         : None                                                      
//  598 * Return         : An ErrorStatus enumuration value:
//  599 *                         - SUCCESS: Reception OK
//  600 *                         - ERROR: No message pending
//  601 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function CAN_BasicReceiveMessage
        THUMB
//  602 ErrorStatus CAN_BasicReceiveMessage(canmsg* pCanMsg)
//  603 {
CAN_BasicReceiveMessage:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  604   if ((CAN->sMsgObj[1].MCR & CAN_MCR_NEWDAT) == 0)
        LDR      R0,??CAN_BasicReceiveMessage_0  ;; 0xffffc498
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16
        BMI      ??CAN_BasicReceiveMessage_1
//  605   {
//  606     return ERROR;
        MOVS     R0,#+0
        B        ??CAN_BasicReceiveMessage_2
//  607   }
//  608 
//  609   CAN->SR &= ~CAN_SR_RXOK;
??CAN_BasicReceiveMessage_1:
        LDR      R0,??CAN_BasicReceiveMessage_0+0x4  ;; 0xffffc404
        LDRH     R0,[R0, #+0]
        LDR      R2,??CAN_BasicReceiveMessage_0+0x8  ;; 0xffef
        ANDS     R2,R2,R0
        LDR      R0,??CAN_BasicReceiveMessage_0+0x4  ;; 0xffffc404
        STRH     R2,[R0, #+0]
//  610 
//  611   CAN->sMsgObj[1].CMR = CAN_CMR_ARB
//  612                       | CAN_CMR_CONTROL
//  613                       | CAN_CMR_DATAA
//  614                       | CAN_CMR_DATAB;
        LDR      R0,??CAN_BasicReceiveMessage_0+0xC  ;; 0xffffc484
        MOVS     R2,#+51
        STRH     R2,[R0, #+0]
//  615 
//  616   if ((CAN->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
        LDR      R0,??CAN_BasicReceiveMessage_0+0x10  ;; 0xffffc494
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+17
        BMI      ??CAN_BasicReceiveMessage_3
//  617   {
//  618     /* standard ID*/
//  619     pCanMsg->IdType = CAN_STD_ID;
        MOVS     R0,#+0
        STR      R0,[R1, #+0]
//  620     pCanMsg->Id = (CAN->sMsgObj[1].A2R >> 2) & 0x07FF;
        LDR      R0,??CAN_BasicReceiveMessage_0+0x10  ;; 0xffffc494
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        ASRS     R0,R0,#+2
        LSLS     R0,R0,#+21       ;; ZeroExtS R0,R0,#+21,#+21
        LSRS     R0,R0,#+21
        STR      R0,[R1, #+4]
        B        ??CAN_BasicReceiveMessage_4
//  621   }
//  622   else
//  623   {
//  624     /* extended ID*/
//  625     pCanMsg->IdType = CAN_EXT_ID;
??CAN_BasicReceiveMessage_3:
        MOVS     R0,#+1
        STR      R0,[R1, #+0]
//  626     //pCanMsg->Id  = ((CAN->sMsgObj[1].A2R >> 2) & 0x07FF);
//  627     //pCanMsg->Id |= ((u32)CAN->sMsgObj[1].A1R << 11);
//  628     //pCanMsg->Id |= (((u32)CAN->sMsgObj[1].A2R & 0x0003) << 27);
//  629     pCanMsg->Id = CAN->sMsgObj[1].A1R + ( ( CAN->sMsgObj[1].A2R & 0x1fff ) >> 16 );
        LDR      R0,??CAN_BasicReceiveMessage_0+0x14  ;; 0xffffc490
        LDRH     R0,[R0, #+0]
        LDR      R2,??CAN_BasicReceiveMessage_0+0x10  ;; 0xffffc494
        LDRH     R2,[R2, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        STR      R0,[R1, #+4]
//  630   }
//  631 
//  632   pCanMsg->Dlc = CAN->sMsgObj[1].MCR & 0x0F;
??CAN_BasicReceiveMessage_4:
        LDR      R0,??CAN_BasicReceiveMessage_0  ;; 0xffffc498
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+28       ;; ZeroExtS R0,R0,#+28,#+28
        LSRS     R0,R0,#+28
        STRB     R0,[R1, #+8]
//  633 
//  634   pCanMsg->Data[0] = (u8) CAN->sMsgObj[1].DA1R;
        LDR      R0,??CAN_BasicReceiveMessage_0+0x18  ;; 0xffffc49c
        LDRH     R0,[R0, #+0]
        STRB     R0,[R1, #+9]
//  635   pCanMsg->Data[1] = (u8)(CAN->sMsgObj[1].DA1R >> 8);
        LDR      R0,??CAN_BasicReceiveMessage_0+0x18  ;; 0xffffc49c
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R1, #+10]
//  636   pCanMsg->Data[2] = (u8) CAN->sMsgObj[1].DA2R;
        LDR      R0,??CAN_BasicReceiveMessage_0+0x1C  ;; 0xffffc4a0
        LDRH     R0,[R0, #+0]
        STRB     R0,[R1, #+11]
//  637   pCanMsg->Data[3] = (u8)(CAN->sMsgObj[1].DA2R >> 8);
        LDR      R0,??CAN_BasicReceiveMessage_0+0x1C  ;; 0xffffc4a0
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R1, #+12]
//  638   pCanMsg->Data[4] = (u8) CAN->sMsgObj[1].DB1R;
        LDR      R0,??CAN_BasicReceiveMessage_0+0x20  ;; 0xffffc4a4
        LDRH     R0,[R0, #+0]
        STRB     R0,[R1, #+13]
//  639   pCanMsg->Data[5] = (u8)(CAN->sMsgObj[1].DB1R >> 8);
        LDR      R0,??CAN_BasicReceiveMessage_0+0x20  ;; 0xffffc4a4
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R1, #+14]
//  640   pCanMsg->Data[6] = (u8) CAN->sMsgObj[1].DB2R;
        LDR      R0,??CAN_BasicReceiveMessage_0+0x24  ;; 0xffffc4a8
        LDRH     R0,[R0, #+0]
        STRB     R0,[R1, #+15]
//  641   pCanMsg->Data[7] = (u8)(CAN->sMsgObj[1].DB2R >> 8);
        LDR      R0,??CAN_BasicReceiveMessage_0+0x24  ;; 0xffffc4a8
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+8
        STRB     R0,[R1, #+16]
//  642 
//  643   return SUCCESS;
        MOVS     R0,#+1
??CAN_BasicReceiveMessage_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??CAN_BasicReceiveMessage_0:
        DC32     0xffffc498
        DC32     0xffffc404
        DC32     0xffef
        DC32     0xffffc484
        DC32     0xffffc494
        DC32     0xffffc490
        DC32     0xffffc49c
        DC32     0xffffc4a0
        DC32     0xffffc4a4
        DC32     0xffffc4a8
        CFI EndBlock cfiBlock15
//  644 }
//  645 
//  646 /*******************************************************************************
//  647 * Function Name  : CAN_EnterInitMode                                         
//  648 * Description    : Switchs the CAN into initialization mode. This function must
//  649 *                  be used in conjunction with CAN_LeaveInitMode().                 
//  650 * Input          : InitMask: specifies the CAN configuration in normal mode.      
//  651 * Output         : None                                                      
//  652 * Return         : None                                                          
//  653 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function CAN_EnterInitMode
        THUMB
//  654 void CAN_EnterInitMode(u8 InitMask)
//  655 {
//  656   CAN->CR = InitMask | CAN_CR_INIT;
CAN_EnterInitMode:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        MOVS     R1,#+1
        ORRS     R1,R1,R0
        LDR      R2,??CAN_EnterInitMode_0  ;; 0xffffc400
        STRH     R1,[R2, #+0]
//  657   CAN->SR = 0;					/* reset the status*/
        LDR      R1,??CAN_EnterInitMode_0+0x4  ;; 0xffffc404
        MOVS     R2,#+0
        STRH     R2,[R1, #+0]
//  658 }
        BX       LR               ;; return
        DATA
??CAN_EnterInitMode_0:
        DC32     0xffffc400
        DC32     0xffffc404
        CFI EndBlock cfiBlock16
//  659 
//  660 /*******************************************************************************
//  661 * Function Name  : CAN_LeaveInitMode                                         
//  662 * Description    : Leaves the initialization mode (switch into normal mode).
//  663 *                  This function must be used in conjunction with CAN_EnterInitMode().  
//  664 * Input          : None                                                      
//  665 * Output         : None                                                      
//  666 * Return         : None                                                      
//  667 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function CAN_LeaveInitMode
        THUMB
//  668 void CAN_LeaveInitMode(void)
//  669 {
//  670   CAN->CR &= ~(CAN_CR_INIT | CAN_CR_CCE);
CAN_LeaveInitMode:
        LDR      R0,??CAN_LeaveInitMode_0  ;; 0xffffc400
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_LeaveInitMode_0+0x4  ;; 0xffbe
        ANDS     R1,R1,R0
        LDR      R0,??CAN_LeaveInitMode_0  ;; 0xffffc400
        STRH     R1,[R0, #+0]
//  671 }
        BX       LR               ;; return
        Nop      
        DATA
??CAN_LeaveInitMode_0:
        DC32     0xffffc400
        DC32     0xffbe
        CFI EndBlock cfiBlock17
//  672 
//  673 /*******************************************************************************
//  674 * Function Name  : CAN_EnterTestMode                                         
//  675 * Description    : Switchs the CAN into test mode. This function must be used in
//  676 *                  conjunction with CAN_LeaveTestMode().                            
//  677 * Input          : TestMask: specifies the configuration in test modes.     
//  678 * Output         : None                                                      
//  679 * Return         : None                                                            
//  680 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function CAN_EnterTestMode
        THUMB
//  681 void CAN_EnterTestMode(u8 TestMask)
//  682 {
//  683   CAN->CR |= CAN_CR_TEST;
CAN_EnterTestMode:
        LDR      R1,??CAN_EnterTestMode_0  ;; 0xffffc400
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+128
        ORRS     R2,R2,R1
        LDR      R1,??CAN_EnterTestMode_0  ;; 0xffffc400
        STRH     R2,[R1, #+0]
//  684   CAN->TESTR |= TestMask;
        LDR      R1,??CAN_EnterTestMode_0+0x4  ;; 0xffffc414
        LDRH     R1,[R1, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        ORRS     R1,R1,R0
        LDR      R2,??CAN_EnterTestMode_0+0x4  ;; 0xffffc414
        STRH     R1,[R2, #+0]
//  685 }
        BX       LR               ;; return
        DATA
??CAN_EnterTestMode_0:
        DC32     0xffffc400
        DC32     0xffffc414
        CFI EndBlock cfiBlock18
//  686 
//  687 /*******************************************************************************
//  688 * Function Name  : CAN_LeaveTestMode                                         
//  689 * Description    : Leaves the current test mode (switch into normal mode).
//  690 *                  This function must be used in conjunction with CAN_EnterTestMode().    
//  691 * Input          : None                                                      
//  692 * Output         : None                                                      
//  693 * Return         : None                                                      
//  694 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon0
        CFI Function CAN_LeaveTestMode
        THUMB
//  695 void CAN_LeaveTestMode(void)
//  696 {
//  697   CAN->CR |= CAN_CR_TEST;
CAN_LeaveTestMode:
        LDR      R0,??CAN_LeaveTestMode_0  ;; 0xffffc400
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+128
        ORRS     R1,R1,R0
        LDR      R0,??CAN_LeaveTestMode_0  ;; 0xffffc400
        STRH     R1,[R0, #+0]
//  698   CAN->TESTR &= ~(CAN_TESTR_LBACK | CAN_TESTR_SILENT | CAN_TESTR_BASIC);
        LDR      R0,??CAN_LeaveTestMode_0+0x4  ;; 0xffffc414
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_LeaveTestMode_0+0x8  ;; 0xffe3
        ANDS     R1,R1,R0
        LDR      R0,??CAN_LeaveTestMode_0+0x4  ;; 0xffffc414
        STRH     R1,[R0, #+0]
//  699   CAN->CR &= ~CAN_CR_TEST;
        LDR      R0,??CAN_LeaveTestMode_0  ;; 0xffffc400
        LDRH     R0,[R0, #+0]
        LDR      R1,??CAN_LeaveTestMode_0+0xC  ;; 0xff7f
        ANDS     R1,R1,R0
        LDR      R0,??CAN_LeaveTestMode_0  ;; 0xffffc400
        STRH     R1,[R0, #+0]
//  700 }
        BX       LR               ;; return
        Nop      
        DATA
??CAN_LeaveTestMode_0:
        DC32     0xffffc400
        DC32     0xffffc414
        DC32     0xffe3
        DC32     0xff7f
        CFI EndBlock cfiBlock19
//  701 
//  702 /*******************************************************************************
//  703 * Function Name  : CAN_ReleaseTxMessage                                      
//  704 * Description    : Releases the transmit message object.
//  705 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                     
//  706 * Output         : None                                                      
//  707 * Return         : None                                                                        
//  708 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock20 Using cfiCommon0
        CFI Function CAN_ReleaseTxMessage
        THUMB
//  709 void CAN_ReleaseTxMessage(u32 msgobj)
//  710 {
//  711   CAN->sMsgObj[0].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
CAN_ReleaseTxMessage:
        LDR      R1,??CAN_ReleaseTxMessage_0  ;; 0xffffc424
        MOVS     R2,#+12
        STRH     R2,[R1, #+0]
//  712   CAN->sMsgObj[0].CRR = 1 + msgobj;
        MOVS     R1,R0
        ADDS     R1,R1,#+1
        LDR      R2,??CAN_ReleaseTxMessage_0+0x4  ;; 0xffffc420
        STRH     R1,[R2, #+0]
//  713 }
        BX       LR               ;; return
        DATA
??CAN_ReleaseTxMessage_0:
        DC32     0xffffc424
        DC32     0xffffc420
        CFI EndBlock cfiBlock20
//  714 
//  715 /*******************************************************************************
//  716 * Function Name  : CAN_ReleaseRxMessage                                      
//  717 * Description    : Releases the receive message object.                        
//  718 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                      
//  719 * Output         : None                                                      
//  720 * Return         : None                                                                      
//  721 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock21 Using cfiCommon0
        CFI Function CAN_ReleaseRxMessage
        THUMB
//  722 void CAN_ReleaseRxMessage(u32 msgobj)
//  723 {
//  724   CAN->sMsgObj[1].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
CAN_ReleaseRxMessage:
        LDR      R1,??CAN_ReleaseRxMessage_0  ;; 0xffffc484
        MOVS     R2,#+12
        STRH     R2,[R1, #+0]
//  725   CAN->sMsgObj[1].CRR = 1 + msgobj;
        MOVS     R1,R0
        ADDS     R1,R1,#+1
        LDR      R2,??CAN_ReleaseRxMessage_0+0x4  ;; 0xffffc480
        STRH     R1,[R2, #+0]
//  726 }
        BX       LR               ;; return
        DATA
??CAN_ReleaseRxMessage_0:
        DC32     0xffffc484
        DC32     0xffffc480
        CFI EndBlock cfiBlock21
//  727 
//  728 /*******************************************************************************
//  729 * Function Name  : CAN_IsMessageWaiting                                      
//  730 * Description    : Tests the waiting status of a received message.             
//  731 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                       
//  732 * Output         : None                                                      
//  733 * Return         : A non-zero value if the corresponding message object has  
//  734 *                  received a message waiting to be copied, else 0.           
//  735 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock22 Using cfiCommon0
        CFI Function CAN_IsMessageWaiting
        THUMB
//  736 u32 CAN_IsMessageWaiting(u32 msgobj)
//  737 {
CAN_IsMessageWaiting:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  738   return (msgobj < 16 ? CAN->ND1R & (1 << msgobj) : CAN->ND2R & (1 << (msgobj-16)));
        CMP      R0,#+16
        BCS      ??CAN_IsMessageWaiting_0
        LDR      R1,??CAN_IsMessageWaiting_1  ;; 0xffffc520
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
        B        ??CAN_IsMessageWaiting_2
??CAN_IsMessageWaiting_0:
        LDR      R1,??CAN_IsMessageWaiting_1+0x4  ;; 0xffffc524
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        SUBS     R0,R0,#+16
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
??CAN_IsMessageWaiting_2:
        MOVS     R0,R1
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_IsMessageWaiting_1:
        DC32     0xffffc520
        DC32     0xffffc524
        CFI EndBlock cfiBlock22
//  739 }
//  740 
//  741 /*******************************************************************************
//  742 * Function Name  : CAN_IsTransmitRequested                                   
//  743 * Description    : Tests the request status of a transmitted message.          
//  744 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                      
//  745 * Output         : None                                                      
//  746 * Return         : A non-zero value if the corresponding message is requested
//  747 *                  to transmit, else 0.                                       
//  748 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock23 Using cfiCommon0
        CFI Function CAN_IsTransmitRequested
        THUMB
//  749 u32 CAN_IsTransmitRequested(u32 msgobj)
//  750 {
CAN_IsTransmitRequested:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  751   return (msgobj < 16 ? CAN->TXR1R & (1 << msgobj) : CAN->TXR2R & (1 << (msgobj-16)));
        CMP      R0,#+16
        BCS      ??CAN_IsTransmitRequested_0
        LDR      R1,??CAN_IsTransmitRequested_1  ;; 0xffffc500
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
        B        ??CAN_IsTransmitRequested_2
??CAN_IsTransmitRequested_0:
        LDR      R1,??CAN_IsTransmitRequested_1+0x4  ;; 0xffffc504
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        SUBS     R0,R0,#+16
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
??CAN_IsTransmitRequested_2:
        MOVS     R0,R1
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_IsTransmitRequested_1:
        DC32     0xffffc500
        DC32     0xffffc504
        CFI EndBlock cfiBlock23
//  752 }
//  753 
//  754 /*******************************************************************************
//  755 * Function Name  : CAN_IsInterruptPending                                    
//  756 * Description    : Tests the interrupt status of a message object.             
//  757 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                      
//  758 * Output         : None                                                      
//  759 * Return         : A non-zero value if the corresponding message has an      
//  760 *                  interrupt pending, else 0.                                 
//  761 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock24 Using cfiCommon0
        CFI Function CAN_IsInterruptPending
        THUMB
//  762 u32 CAN_IsInterruptPending(u32 msgobj)
//  763 {
CAN_IsInterruptPending:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  764   return (msgobj < 16 ? CAN->IP1R & (1 << msgobj) : CAN->IP2R & (1 << (msgobj-16)));
        CMP      R0,#+16
        BCS      ??CAN_IsInterruptPending_0
        LDR      R1,??CAN_IsInterruptPending_1  ;; 0xffffc540
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
        B        ??CAN_IsInterruptPending_2
??CAN_IsInterruptPending_0:
        LDR      R1,??CAN_IsInterruptPending_1+0x4  ;; 0xffffc544
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        SUBS     R0,R0,#+16
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
??CAN_IsInterruptPending_2:
        MOVS     R0,R1
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_IsInterruptPending_1:
        DC32     0xffffc540
        DC32     0xffffc544
        CFI EndBlock cfiBlock24
//  765 }
//  766 
//  767 /*******************************************************************************
//  768 * Function Name  : CAN_IsObjectValid                                         
//  769 * Description    : Tests the validity of a message object (ready to use).      
//  770 * Input          : - msgobj: specifies the Message object number, from 0 to 31.                      
//  771 * Output         : None                                                      
//  772 * Return         : A non-zero value if the corresponding message object is   
//  773 *                  valid, else 0.                                             
//  774 *******************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock25 Using cfiCommon0
        CFI Function CAN_IsObjectValid
        THUMB
//  775 u32 CAN_IsObjectValid(u32 msgobj)
//  776 {
CAN_IsObjectValid:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  777   return (msgobj < 16 ? CAN->MV1R & (1 << msgobj) : CAN->MV2R & (1 << (msgobj-16)));
        CMP      R0,#+16
        BCS      ??CAN_IsObjectValid_0
        LDR      R1,??CAN_IsObjectValid_1  ;; 0xffffc560
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
        B        ??CAN_IsObjectValid_2
??CAN_IsObjectValid_0:
        LDR      R1,??CAN_IsObjectValid_1+0x4  ;; 0xffffc564
        LDRH     R2,[R1, #+0]
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        MOVS     R1,#+1
        SUBS     R0,R0,#+16
        LSLS     R1,R1,R0
        ANDS     R1,R1,R2
??CAN_IsObjectValid_2:
        MOVS     R0,R1
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??CAN_IsObjectValid_1:
        DC32     0xffffc560
        DC32     0xffffc564
        CFI EndBlock cfiBlock25
//  778 }

        END
//  779 /******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
// 
//    20 bytes in section .data
// 2 458 bytes in section .text
// 
// 2 458 bytes of CODE memory
//    20 bytes of DATA memory
//
//Errors: none
//Warnings: 2
