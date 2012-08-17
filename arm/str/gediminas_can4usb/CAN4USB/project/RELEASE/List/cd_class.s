///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:36 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\cd_class.c /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\cd_class.c /
//                     -lC D:\development\gediminas\CAN4USB\project\RELEASE\L /
//                    ist\ -lA D:\development\gediminas\CAN4USB\project\RELEA /
//                    SE\List\ -o D:\development\gediminas\CAN4USB\project\RE /
//                    LEASE\Obj\ --no_cse --no_unroll --no_inline             /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\c /
//                    d_class.s                                               /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME cd_class

        RTMODEL "__dlib_file_descriptor", "0"

        EXTERN EIC_IRQCmd
        EXTERN EIC_IRQInit
        EXTERN MRCC_GetClocksStatus
        EXTERN MRCC_PeripheralClockConfig
        EXTERN MRCC_PeripheralSWResetConfig
        EXTERN TIM_ClearFlag
        EXTERN TIM_ClearITPendingBit
        EXTERN TIM_Cmd
        EXTERN TIM_ITConfig
        EXTERN TIM_Init
        EXTERN TIM_ResetCounter
        EXTERN USB_EpRead
        EXTERN USB_EpValidate
        EXTERN USB_EpWrite
        EXTERN USB_RealizeEp
        EXTERN USB_UserFuncRegistering
        EXTERN UsbCoreReq
        EXTERN __aeabi_memcpy
        EXTERN __aeabi_uidivmod

        PUBWEAK AtomicExchange
        PUBLIC BreakCntr
        PUBLIC CDC_BreakCallBack
        PUBLIC CDC_Configure
        PUBLIC CDC_LineCoding
        PUBLIC CDC_LineState
        PUBLIC CDC_ReceiveBuffer
        PUBLIC CDC_ReceiveIndx
        PUBLIC CDC_ReceiveIndxHold
        PUBLIC CDC_TransmitBuffer
        PUBLIC CDC_TransmitSize
        PUBLIC CdcReqPacket
        PUBLIC LineCodingDelta
        PUBLIC LineStateDelta
        PUBLIC SerialStateDelta
        PUBLIC Tim1Handler
        PUBLIC USB_CDC_OutEpBufferNotEmpty
        PUBLIC UsbCdcConfigure
        PUBLIC UsbCdcData
        PUBLIC UsbCdcGetBreakState
        PUBLIC UsbCdcGetLineCodingSettings
        PUBLIC UsbCdcGetLineStateSettings
        PUBLIC UsbCdcInHadler
        PUBLIC UsbCdcInit
        PUBLIC UsbCdcIsCdcConfigure
        PUBLIC UsbCdcIsNewLineCodingSettings
        PUBLIC UsbCdcIsNewLineStateSettings
        PUBLIC UsbCdcIsSendProgess
        PUBLIC UsbCdcOutHadler
        PUBLIC UsbCdcRead
        PUBLIC UsbCdcReportHadler
        PUBLIC UsbCdcReportSerialCommState
        PUBLIC UsbCdcRequest
        PUBLIC UsbCdcWrite
        PUBLIC pCDC_TransmitUserBuffer
        
        CFI Names cfiNames0
        CFI StackFrame CFA R13 DATA
        CFI Resource R0:32, R1:32, R2:32, R3:32, R4:32, R5:32, R6:32, R7:32
        CFI Resource R8:32, R9:32, R10:32, R11:32, R12:32, R13:32, R14:32
        CFI EndNames cfiNames0
        
        CFI Common cfiCommon0 Using cfiNames0
        CFI CodeAlign 4
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
        
        
        CFI Common cfiCommon1 Using cfiNames0
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
        CFI EndCommon cfiCommon1
        
// D:\development\gediminas\CAN4USB\project\app\cd_class.c
//    1 /*************************************************************************
//    2  *
//    3  *    Used with ICCARM and AARM.
//    4  *
//    5  *    (c) Copyright IAR Systems 2006
//    6  *
//    7  *    File name   : cd_class.c
//    8  *    Description : Communication device class module
//    9  *
//   10  *    History :
//   11  *    1. Date        : June 28, 2006
//   12  *       Author      : Stanimir Bonev
//   13  *       Description : Create
//   14  *
//   15  *    $Revision: 16170 $
//   16 **************************************************************************/
//   17 #define CD_CLASS_GLOBAL
//   18 #include "cd_class.h"

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function AtomicExchange
        ARM
// __??Code32?? __code __interwork Int32U AtomicExchange(Int32U, pInt32U)
AtomicExchange:
        swp  r0, r0, [r1] 
        BX       LR               ;; return
        CFI EndBlock cfiBlock0
//   19 
//   20 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//   21 CDC_LineCoding_t CDC_LineCoding;
CDC_LineCoding:
        DS8 8
//   22 
//   23 #if CDC_DEVICE_SUPPORT_LINE_CODING > 0

        SECTION `.bss`:DATA:NOROOT(2)
//   24 volatile Int32U LineCodingDelta;
LineCodingDelta:
        DS8 4
//   25 #endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
//   26 
//   27 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//   28 CDC_LineState_t  CDC_LineState;
CDC_LineState:
        DS8 4
//   29 
//   30 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0

        SECTION `.bss`:DATA:NOROOT(2)
//   31 volatile Int32U SerialStateDelta;
SerialStateDelta:
        DS8 4
//   32 

        SECTION `.bss`:DATA:NOROOT(2)
//   33 volatile Int32U LineStateDelta;
LineStateDelta:
        DS8 4
//   34 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//   35 
//   36 #if CDC_DEVICE_SUPPORT_BREAK > 0

        SECTION `.bss`:DATA:NOROOT(2)
//   37 volatile Int32U BreakCntr;
BreakCntr:
        DS8 4
//   38 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//   39 
//   40 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//   41 UsbSetupPacket_t CdcReqPacket;
CdcReqPacket:
        DS8 8
//   42 

        SECTION `.bss`:DATA:NOROOT(2)
//   43 Int32U CDC_ReceiveIndx, CDC_ReceiveIndxHold;
CDC_ReceiveIndx:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
CDC_ReceiveIndxHold:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   44 Int32U CDC_TransmitSize;
CDC_TransmitSize:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(0)
//   45 volatile Boolean USB_CDC_OutEpBufferNotEmpty;
USB_CDC_OutEpBufferNotEmpty:
        DS8 1

        SECTION `.bss`:DATA:NOROOT(2)
//   46 pInt8U volatile pCDC_TransmitUserBuffer;
pCDC_TransmitUserBuffer:
        DS8 4
//   47 
//   48 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//   49 Int8U CDC_ReceiveBuffer[CommOutEpMaxSize];
CDC_ReceiveBuffer:
        DS8 64
//   50 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//   51 Int8U CDC_TransmitBuffer[CommInEpMaxSize];
CDC_TransmitBuffer:
        DS8 64
//   52 

        SECTION `.bss`:DATA:NOROOT(0)
//   53 volatile Boolean CDC_Configure;
CDC_Configure:
        DS8 1
//   54 
//   55 /*************************************************************************
//   56  * Function Name: Tim1Handler
//   57  * Parameters: void
//   58  *
//   59  * Return: none
//   60  *
//   61  * Description: Timer 1 interrupt handler
//   62  *
//   63  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon1
        CFI Function Tim1Handler
        THUMB
//   64 void Tim1Handler(void)
//   65 {
Tim1Handler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   66 #if CDC_DEVICE_SUPPORT_BREAK > 0
//   67   if(BreakCntr)
        LDR      R0,??DataTable26  ;; BreakCntr
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??Tim1Handler_0
//   68   {
//   69     --BreakCntr;
        LDR      R0,??DataTable26  ;; BreakCntr
        LDR      R0,[R0, #+0]
        SUBS     R0,R0,#+1
        LDR      R1,??DataTable26  ;; BreakCntr
        STR      R0,[R1, #+0]
        B        ??Tim1Handler_1
//   70   }
//   71   else
//   72   {
//   73     CDC_BreakCallBack(NULL);
??Tim1Handler_0:
        MOVS     R0,#+0
        BL       CDC_BreakCallBack
//   74     // Disable TIM1 counter
//   75     TIM_Cmd(TIM1, DISABLE);
        MOVS     R1,#+0
        LDR      R0,??DataTable15  ;; 0xffff9000
        BL       TIM_Cmd
//   76   }
//   77 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//   78   // Clear the Update pending Bit
//   79   TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
??Tim1Handler_1:
        MOVS     R1,#+1
        LDR      R0,??DataTable15  ;; 0xffff9000
        BL       TIM_ClearITPendingBit
//   80 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock1
//   81 
//   82 /*************************************************************************
//   83  * Function Name: UsbCdcInit
//   84  * Parameters: Int32U IntrPriority
//   85  *
//   86  * Return: none
//   87  *
//   88  * Description: USB communication device class init
//   89  *
//   90  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon1
        CFI Function UsbCdcInit
        THUMB
//   91 void UsbCdcInit (Int32U IntrPriority)
//   92 {
UsbCdcInit:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        SUB      SP,SP,#+64
        CFI CFA R13+72
        MOVS     R4,R0
//   93 #if CDC_DEVICE_SUPPORT_BREAK > 0
//   94 TIM_InitTypeDef TIM_InitStructure;
//   95 EIC_IRQInitTypeDef EIC_IRQInitStructure;
//   96 MRCC_ClocksTypeDef MRCC_Clocks;
//   97 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//   98 
//   99   // Init CD Class variables
//  100   CDC_Configure               = FALSE;
        LDR      R0,??DataTable27  ;; CDC_Configure
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  101 
//  102   CDC_LineCoding.dwDTERate    = CDC_DATA_RATE;
        LDR      R0,??DataTable6  ;; CDC_LineCoding
        MOVS     R1,#+225
        LSLS     R1,R1,#+9        ;; #+115200
        STR      R1,[R0, #+0]
//  103   CDC_LineCoding.bDataBits    = CDC_DATA_BITS;
        LDR      R0,??UsbCdcInit_0  ;; CDC_LineCoding + 6
        MOVS     R1,#+8
        STRB     R1,[R0, #+0]
//  104   CDC_LineCoding.bParityType  = CDC_PARITY;
        LDR      R0,??UsbCdcInit_0+0x4  ;; CDC_LineCoding + 5
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  105   CDC_LineCoding.bCharFormat  = CDC_STOP_BITS;
        LDR      R0,??UsbCdcInit_0+0x8  ;; CDC_LineCoding + 4
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  106 
//  107 #if CDC_DEVICE_SUPPORT_LINE_CODING > 0
//  108   // Update the line coding
//  109   LineCodingDelta             = TRUE;
        LDR      R0,??DataTable7  ;; LineCodingDelta
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  110 #endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
//  111 
//  112   CDC_LineState.DTR_State     = CDC_LINE_DTR;
        LDR      R0,??DataTable11  ;; CDC_LineState
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+1
        ORRS     R1,R1,R0
        LDR      R0,??DataTable11  ;; CDC_LineState
        STRB     R1,[R0, #+0]
//  113   CDC_LineState.RTS_State     = CDC_LINE_RTS;
        LDR      R0,??DataTable11  ;; CDC_LineState
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+2
        ORRS     R1,R1,R0
        LDR      R0,??DataTable11  ;; CDC_LineState
        STRB     R1,[R0, #+0]
//  114 
//  115 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  116   // Update the line state
//  117   LineStateDelta              = TRUE;
        LDR      R0,??DataTable12  ;; LineStateDelta
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  118   SerialStateDelta            = FALSE;
        LDR      R0,??DataTable28  ;; SerialStateDelta
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  119 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  120 
//  121 #if CDC_DEVICE_SUPPORT_BREAK > 0
//  122 
//  123   BreakCntr = 0;
        LDR      R0,??DataTable26  ;; BreakCntr
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  124   // Init Break timer resolution 1 ms
//  125   // Enable TIM1 clocks
//  126   MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM1, ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+8
        BL       MRCC_PeripheralClockConfig
//  127   // Release TIM1 reset
//  128   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM1,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+8
        BL       MRCC_PeripheralSWResetConfig
//  129   // Calculate counter value
//  130   MRCC_GetClocksStatus(&MRCC_Clocks);
        ADD      R0,SP,#+4
        BL       MRCC_GetClocksStatus
//  131 
//  132   TIM_InitStructure.TIM_Period = (MRCC_Clocks.CKTIM_Frequency / 1000);
        LDR      R0,[SP, #+20]
        MOVS     R1,#+250
        LSLS     R1,R1,#+2        ;; #+1000
        BL       __aeabi_uidivmod
        ADD      R1,SP,#+28
        STRH     R0,[R1, #+10]
//  133   TIM_InitStructure.TIM_Prescaler = 0;
        ADD      R0,SP,#+28
        MOVS     R1,#+0
        STRH     R1,[R0, #+2]
//  134   TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
        MOV      R0,SP
        MOVS     R1,#+1
        STRH     R1,[R0, #+28]
//  135   TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
        ADD      R0,SP,#+28
        MOVS     R1,#+1
        STRH     R1,[R0, #+4]
//  136   TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        ADD      R0,SP,#+28
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  137   TIM_InitStructure.TIM_Channel = TIM_Channel_2;
        ADD      R0,SP,#+28
        MOVS     R1,#+2
        STRH     R1,[R0, #+12]
//  138   TIM_Init(TIM1, &TIM_InitStructure);
        ADD      R1,SP,#+28
        LDR      R0,??DataTable15  ;; 0xffff9000
        BL       TIM_Init
//  139 
//  140   // Enable and configure the priority of the TIM1 Update IRQ Channel
//  141   EIC_IRQInitStructure.EIC_IRQChannel = TIM1_UP_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+8
        STRB     R1,[R0, #+0]
//  142   EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
        MOV      R0,SP
        MOVS     R1,R4
        STRB     R1,[R0, #+1]
//  143   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  144   EIC_IRQInit(&EIC_IRQInitStructure);
        MOV      R0,SP
        BL       EIC_IRQInit
//  145 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//  146 
//  147   CDC_ReceiveIndx      = \ 
//  148   CDC_ReceiveIndxHold  = \ 
//  149   CDC_TransmitSize     = 0;
        MOVS     R0,#+0
        LDR      R1,??DataTable41  ;; CDC_TransmitSize
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable23  ;; CDC_ReceiveIndxHold
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable31  ;; CDC_ReceiveIndx
        STR      R0,[R1, #+0]
//  150 
//  151   USB_CDC_OutEpBufferNotEmpty = FALSE;
        LDR      R0,??DataTable33  ;; USB_CDC_OutEpBufferNotEmpty
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  152 
//  153   pCDC_TransmitUserBuffer = NULL;
        LDR      R0,??DataTable43  ;; pCDC_TransmitUserBuffer
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  154 
//  155   // Registered the class request
//  156   USB_UserFuncRegistering(UsbCdcRequest,UsbUserClass);
        MOVS     R1,#+35
        LDR      R0,??UsbCdcInit_0+0xC  ;; UsbCdcRequest
        BL       USB_UserFuncRegistering
//  157   // Registered the function for a data receive of the class request
//  158   USB_UserFuncRegistering(UsbCdcData,UsbClassEp0OutPacket);
        MOVS     R1,#+32
        LDR      R0,??UsbCdcInit_0+0x10  ;; UsbCdcData
        BL       USB_UserFuncRegistering
//  159 }
        ADD      SP,SP,#+64
        CFI CFA R13+8
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbCdcInit_0:
        DC32     CDC_LineCoding + 6
        DC32     CDC_LineCoding + 5
        DC32     CDC_LineCoding + 4
        DC32     UsbCdcRequest
        DC32     UsbCdcData
        CFI EndBlock cfiBlock2

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable6:
        DC32     CDC_LineCoding

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable7:
        DC32     LineCodingDelta

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable11:
        DC32     CDC_LineState

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable12:
        DC32     LineStateDelta

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable15:
        DC32     0xffff9000
//  160 
//  161 /*************************************************************************
//  162  * Function Name: UsbCdcConfigure
//  163  * Parameters:  void * pArg
//  164  *
//  165  * Return: void *
//  166  *
//  167  * Description: USB communication device class configure
//  168  *
//  169  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon1
        CFI Function UsbCdcConfigure
        THUMB
//  170 void * UsbCdcConfigure (void * pArg)
//  171 {
UsbCdcConfigure:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  172 UsbDevCtrl_t * pUsbDevCtrl = (UsbDevCtrl_t *)pArg;
        MOVS     R5,R4
//  173   if(pUsbDevCtrl == NULL)
        CMP      R5,#+0
        BNE      ??UsbCdcConfigure_0
//  174   {
//  175     CDC_Configure = FALSE;
        LDR      R0,??DataTable27  ;; CDC_Configure
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  176     if(UsbCoreReq(UsbCoreReqConfiquration) != 0)
        MOVS     R0,#+2
        BL       UsbCoreReq
        CMP      R0,#+0
        BEQ      ??UsbCdcConfigure_1
//  177     {
//  178       // disable all class EPs
//  179       USB_RealizeEp((USB_Endpoint_t)CommOutEp,0,EP_BULK_SING_BUFF,EP_SLOT2);
        MOVS     R3,#+2
        MOVS     R2,#+0
        MOVS     R1,#+0
        MOVS     R0,#+4
        BL       USB_RealizeEp
//  180       USB_UserFuncRegistering(NULL,CommOutEp);
        MOVS     R1,#+4
        MOVS     R0,#+0
        BL       USB_UserFuncRegistering
//  181       USB_RealizeEp((USB_Endpoint_t)CommInEp,0,EP_BULK_SING_BUFF,EP_SLOT2);
        MOVS     R3,#+2
        MOVS     R2,#+0
        MOVS     R1,#+0
        MOVS     R0,#+5
        BL       USB_RealizeEp
//  182       USB_UserFuncRegistering(NULL,CommInEp);
        MOVS     R1,#+5
        MOVS     R0,#+0
        BL       USB_UserFuncRegistering
//  183       USB_RealizeEp((USB_Endpoint_t)ReportEp,0,EP_INTERRUPT,EP_SLOT1);
        MOVS     R3,#+1
        MOVS     R2,#+3
        MOVS     R1,#+0
        MOVS     R0,#+3
        BL       USB_RealizeEp
//  184       USB_UserFuncRegistering(NULL,ReportEp);
        MOVS     R1,#+3
        MOVS     R0,#+0
        BL       USB_UserFuncRegistering
        B        ??UsbCdcConfigure_1
//  185     }
//  186   }
//  187   else
//  188   {
//  189     // Init variables
//  190     CDC_ReceiveIndx      = \ 
//  191     CDC_ReceiveIndxHold  = \ 
//  192     CDC_TransmitSize     = 0;
??UsbCdcConfigure_0:
        MOVS     R0,#+0
        LDR      R1,??DataTable41  ;; CDC_TransmitSize
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable23  ;; CDC_ReceiveIndxHold
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable31  ;; CDC_ReceiveIndx
        STR      R0,[R1, #+0]
//  193 
//  194     USB_CDC_OutEpBufferNotEmpty = FALSE;
        LDR      R0,??DataTable33  ;; USB_CDC_OutEpBufferNotEmpty
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  195 
//  196 #if CDC_DEVICE_SUPPORT_BREAK > 0
//  197     BreakCntr = 0;
        LDR      R0,??DataTable26  ;; BreakCntr
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  198 #endif  // CDC_DEVICE_SUPPORT_BREAK > 0
//  199 
//  200     // enable all class EPs
//  201     USB_UserFuncRegistering((UserFunc_t)UsbCdcInHadler,CommOutEp);
        MOVS     R1,#+4
        LDR      R0,??UsbCdcConfigure_2  ;; UsbCdcInHadler
        BL       USB_UserFuncRegistering
//  202     USB_RealizeEp((USB_Endpoint_t)CommOutEp,CommOutEpMaxSize,EP_BULK_SING_BUFF,EP_SLOT2);
        MOVS     R3,#+2
        MOVS     R2,#+0
        MOVS     R1,#+64
        MOVS     R0,#+4
        BL       USB_RealizeEp
//  203     USB_UserFuncRegistering((UserFunc_t)UsbCdcOutHadler,CommInEp);
        MOVS     R1,#+5
        LDR      R0,??UsbCdcConfigure_2+0x4  ;; UsbCdcOutHadler
        BL       USB_UserFuncRegistering
//  204     USB_RealizeEp((USB_Endpoint_t)CommInEp,CommInEpMaxSize,EP_BULK_SING_BUFF,EP_SLOT2);
        MOVS     R3,#+2
        MOVS     R2,#+0
        MOVS     R1,#+64
        MOVS     R0,#+5
        BL       USB_RealizeEp
//  205 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  206     USB_UserFuncRegistering((UserFunc_t)UsbCdcReportHadler,ReportEp);
        MOVS     R1,#+3
        LDR      R0,??UsbCdcConfigure_2+0x8  ;; UsbCdcReportHadler
        BL       USB_UserFuncRegistering
//  207     USB_RealizeEp((USB_Endpoint_t)ReportEp,ReportEpMaxSize,EP_INTERRUPT,EP_SLOT1);
        MOVS     R3,#+1
        MOVS     R2,#+3
        MOVS     R1,#+16
        MOVS     R0,#+3
        BL       USB_RealizeEp
//  208 #else
//  209     USB_UserFuncRegistering(NULL,ReportEp);
//  210     USB_RealizeEp((USB_Endpoint_t)ReportEp,ReportEpMaxSize,EP_INTERRUPT,EP_SLOT1);
//  211 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  212 
//  213     CDC_Configure = TRUE;
        LDR      R0,??DataTable27  ;; CDC_Configure
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
//  214   }
//  215   return(NULL);
??UsbCdcConfigure_1:
        MOVS     R0,#+0
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbCdcConfigure_2:
        DC32     UsbCdcInHadler
        DC32     UsbCdcOutHadler
        DC32     UsbCdcReportHadler
        CFI EndBlock cfiBlock3
//  216 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable23:
        DC32     CDC_ReceiveIndxHold

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable26:
        DC32     BreakCntr

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable27:
        DC32     CDC_Configure
//  217 
//  218 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  219 /*************************************************************************
//  220  * Function Name: UsbCdcReportHadler
//  221  * Parameters:  void * pArg
//  222  *
//  223  * Return: none
//  224  *
//  225  * Description: USB Communication Device Class Report (In) EP handler
//  226  *
//  227  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon1
        CFI Function UsbCdcReportHadler
        THUMB
//  228 void UsbCdcReportHadler (void *Arg)
//  229 {
//  230   SerialStateDelta = FALSE;
UsbCdcReportHadler:
        LDR      R1,??DataTable28  ;; SerialStateDelta
        MOVS     R2,#+0
        STR      R2,[R1, #+0]
//  231 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock4

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable28:
        DC32     SerialStateDelta
//  232 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  233 
//  234 /*************************************************************************
//  235  * Function Name: UsbCdcInHadler
//  236  * Parameters:  void * pArg
//  237  *
//  238  * Return: none
//  239  *
//  240  * Description: USB Communication Device Class Out EP handler
//  241  *
//  242  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon1
        CFI Function UsbCdcInHadler
        THUMB
//  243 void UsbCdcInHadler (void *Arg)
//  244 {
UsbCdcInHadler:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  245     if(CDC_ReceiveIndx == 0)
        LDR      R0,??DataTable31  ;; CDC_ReceiveIndx
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcInHadler_0
//  246     {
//  247       CDC_ReceiveIndx = CommOutEpMaxSize;
        LDR      R0,??DataTable31  ;; CDC_ReceiveIndx
        MOVS     R1,#+64
        STR      R1,[R0, #+0]
//  248       USB_EpRead((USB_Endpoint_t)CommOutEp,
//  249                  (pInt8U)CDC_ReceiveBuffer,
//  250                  &CDC_ReceiveIndx);
        LDR      R2,??DataTable31  ;; CDC_ReceiveIndx
        LDR      R1,??DataTable32  ;; CDC_ReceiveBuffer
        MOVS     R0,#+4
        BL       USB_EpRead
//  251       USB_EpValidate((USB_Endpoint_t)CommOutEp,
//  252                      TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+4
        BL       USB_EpValidate
        B        ??UsbCdcInHadler_1
//  253     }
//  254     else
//  255     {
//  256       USB_CDC_OutEpBufferNotEmpty = TRUE;
??UsbCdcInHadler_0:
        LDR      R0,??DataTable33  ;; USB_CDC_OutEpBufferNotEmpty
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
//  257     }
//  258 }
??UsbCdcInHadler_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock5

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable31:
        DC32     CDC_ReceiveIndx

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable32:
        DC32     CDC_ReceiveBuffer

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable33:
        DC32     USB_CDC_OutEpBufferNotEmpty
//  259 
//  260 /*************************************************************************
//  261  * Function Name: UsbCdcOutHadler
//  262  * Parameters:  void * pArg
//  263  *
//  264  * Return: none
//  265  *
//  266  * Description: USB Communication Device Class In EP handler
//  267  *
//  268  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon1
        CFI Function UsbCdcOutHadler
        THUMB
//  269 void UsbCdcOutHadler (void *Arg)
//  270 {
UsbCdcOutHadler:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  271 Int32U Counter, CurrWriteSize;
//  272   if(CDC_TransmitSize != 0)
        LDR      R0,??DataTable41  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcOutHadler_0
//  273   {
//  274     CurrWriteSize = MIN(CDC_TransmitSize,CommInEpMaxSize);
        LDR      R0,??DataTable41  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        CMP      R0,#+64
        BCS      ??UsbCdcOutHadler_1
        LDR      R0,??DataTable41  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+0]
        B        ??UsbCdcOutHadler_2
??UsbCdcOutHadler_1:
        MOVS     R0,#+64
        STR      R0,[SP, #+0]
//  275     for(Counter = 0; Counter < CurrWriteSize; ++Counter)
??UsbCdcOutHadler_2:
        MOVS     R0,#+0
        MOVS     R5,R0
??UsbCdcOutHadler_3:
        LDR      R0,[SP, #+0]
        CMP      R5,R0
        BCS      ??UsbCdcOutHadler_4
//  276     {
//  277       CDC_TransmitBuffer[Counter] = *pCDC_TransmitUserBuffer++;
        LDR      R0,??DataTable43  ;; pCDC_TransmitUserBuffer
        LDR      R0,[R0, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+1
        LDR      R2,??DataTable43  ;; pCDC_TransmitUserBuffer
        STR      R1,[R2, #+0]
        LDR      R1,??DataTable42  ;; CDC_TransmitBuffer
        LDRB     R0,[R0, #+0]
        STRB     R0,[R1, R5]
//  278     }
        ADDS     R5,R5,#+1
        B        ??UsbCdcOutHadler_3
//  279     CDC_TransmitSize -= CurrWriteSize;
??UsbCdcOutHadler_4:
        LDR      R0,??DataTable41  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        LDR      R1,[SP, #+0]
        SUBS     R0,R0,R1
        LDR      R1,??DataTable41  ;; CDC_TransmitSize
        STR      R0,[R1, #+0]
//  280     USB_EpWrite((USB_Endpoint_t)CommInEp,CDC_TransmitBuffer,&CurrWriteSize);
        MOV      R2,SP
        LDR      R1,??DataTable42  ;; CDC_TransmitBuffer
        MOVS     R0,#+5
        BL       USB_EpWrite
        B        ??UsbCdcOutHadler_5
//  281   }
//  282   else
//  283   {
//  284     pCDC_TransmitUserBuffer = NULL;
??UsbCdcOutHadler_0:
        LDR      R0,??DataTable43  ;; pCDC_TransmitUserBuffer
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  285   }
//  286 }
??UsbCdcOutHadler_5:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock6

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable41:
        DC32     CDC_TransmitSize

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable42:
        DC32     CDC_TransmitBuffer

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable43:
        DC32     pCDC_TransmitUserBuffer
//  287 
//  288 /*************************************************************************
//  289  * Function Name: UsbCdcRequest
//  290  * Parameters:  void * pArg
//  291  *
//  292  * Return: void *
//  293  *
//  294  * Description: The class requests processing
//  295  *
//  296  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon1
        CFI Function UsbCdcRequest
        THUMB
//  297 void * UsbCdcRequest (void * pArg)
//  298 {
UsbCdcRequest:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  299 UsbEpCtrl_t * pCdcReqCtrl = (UsbEpCtrl_t *) pArg;
        MOVS     R5,R4
//  300   CdcReqPacket = *(UsbSetupPacket_t *)pCdcReqCtrl->pData;
        LDR      R0,??UsbCdcRequest_0  ;; CdcReqPacket
        LDR      R1,[R5, #+0]
        MOVS     R2,#+8
        BL       __aeabi_memcpy
//  301   // Validate Request
//  302   if (CdcReqPacket.mRequestType.Recipient == UsbRecipientInterface)
        LDR      R0,??UsbCdcRequest_0  ;; CdcReqPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        CMP      R0,#+1
        BEQ      .+4
        B        ??UsbCdcRequest_1
//  303   {
//  304     switch (CdcReqPacket.bRequest)
        LDR      R0,??UsbCdcRequest_0+0x4  ;; CdcReqPacket + 1
        LDRB     R0,[R0, #+0]
        SUBS     R0,R0,#+32
        BEQ      ??UsbCdcRequest_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbCdcRequest_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbCdcRequest_4
        SUBS     R0,R0,#+1
        BEQ      ??UsbCdcRequest_5
        B        ??UsbCdcRequest_1
//  305     {
//  306     case SET_LINE_CODING:
//  307       if ((CdcReqPacket.wValue.Word == 0) &&
//  308           (CdcReqPacket.wIndex.Word == 0))
??UsbCdcRequest_2:
        LDR      R0,??UsbCdcRequest_0+0x8  ;; CdcReqPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      .+4
        B        ??UsbCdcRequest_1
        LDR      R0,??UsbCdcRequest_0+0xC  ;; CdcReqPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcRequest_1
//  309       {
//  310         pCdcReqCtrl->Counter = MIN(sizeof(CDC_LineCoding_t),
//  311                                   ((CdcReqPacket.wLength.Hi << 8) + CdcReqPacket.wLength.Lo));
        LDR      R0,??UsbCdcRequest_0+0x10  ;; CdcReqPacket + 7
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+8
        LDR      R1,??UsbCdcRequest_0+0x14  ;; CdcReqPacket + 6
        LDRB     R1,[R1, #+0]
        ADDS     R0,R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R0,#+8
        BCC      ??UsbCdcRequest_6
        MOVS     R0,#+7
        B        ??UsbCdcRequest_7
??UsbCdcRequest_6:
        LDR      R0,??UsbCdcRequest_0+0x10  ;; CdcReqPacket + 7
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+8
        LDR      R1,??UsbCdcRequest_0+0x14  ;; CdcReqPacket + 6
        LDRB     R1,[R1, #+0]
        ADDS     R0,R0,R1
??UsbCdcRequest_7:
        STR      R0,[R5, #+4]
//  312         pCdcReqCtrl->pData = (pInt8U)&CDC_LineCoding;
        LDR      R0,??UsbCdcRequest_0+0x18  ;; CDC_LineCoding
        STR      R0,[R5, #+0]
//  313         return((void *)UsbPass);
        MOVS     R0,#+0
        B        ??UsbCdcRequest_8
//  314       }
//  315       break;
//  316     case GET_LINE_CODING:
//  317       if ((CdcReqPacket.wValue.Word == 0) &&
//  318           (CdcReqPacket.wIndex.Word == 0))
??UsbCdcRequest_3:
        LDR      R0,??UsbCdcRequest_0+0x8  ;; CdcReqPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcRequest_1
        LDR      R0,??UsbCdcRequest_0+0xC  ;; CdcReqPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcRequest_1
//  319       {
//  320         pCdcReqCtrl->Counter = sizeof(CDC_LineCoding_t);
        MOVS     R0,#+7
        STR      R0,[R5, #+4]
//  321         pCdcReqCtrl->pData   = (pInt8U)&CDC_LineCoding;
        LDR      R0,??UsbCdcRequest_0+0x18  ;; CDC_LineCoding
        STR      R0,[R5, #+0]
//  322         return((void*)UsbPass);
        MOVS     R0,#+0
        B        ??UsbCdcRequest_8
//  323       }
//  324       break;
//  325     case SET_CONTROL_LINE_STATE:
//  326       if ((CdcReqPacket.wLength.Word == 0) &&
//  327           (CdcReqPacket.wIndex.Word == 0))
??UsbCdcRequest_4:
        LDR      R0,??UsbCdcRequest_0+0x14  ;; CdcReqPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcRequest_1
        LDR      R0,??UsbCdcRequest_0+0xC  ;; CdcReqPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcRequest_1
//  328       {
//  329         CDC_LineState.DTR_State = ((CdcReqPacket.wValue.Lo & 1) != 0);
        LDR      R0,??UsbCdcRequest_0+0x1C  ;; CDC_LineState
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+254
        ANDS     R1,R1,R0
        LDR      R0,??UsbCdcRequest_0+0x8  ;; CdcReqPacket + 2
        LDRB     R0,[R0, #+0]
        MOVS     R2,#+1
        ANDS     R2,R2,R0
        MOVS     R0,#+1
        ANDS     R0,R0,R2
        ORRS     R0,R0,R1
        LDR      R1,??UsbCdcRequest_0+0x1C  ;; CDC_LineState
        STRB     R0,[R1, #+0]
//  330         CDC_LineState.RTS_State = ((CdcReqPacket.wValue.Lo & 2) != 0);
        LDR      R0,??UsbCdcRequest_0+0x8  ;; CdcReqPacket + 2
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+2
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LDR      R1,??UsbCdcRequest_0+0x1C  ;; CDC_LineState
        LDRB     R1,[R1, #+0]
        MOVS     R2,#+253
        ANDS     R2,R2,R1
        LSLS     R0,R0,#+1
        MOVS     R1,#+2
        ANDS     R1,R1,R0
        ORRS     R1,R1,R2
        LDR      R0,??UsbCdcRequest_0+0x1C  ;; CDC_LineState
        STRB     R1,[R0, #+0]
//  331         // Send AKN
//  332         pCdcReqCtrl->Counter = 0;
        MOVS     R0,#+0
        STR      R0,[R5, #+4]
//  333 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  334         LineStateDelta = TRUE;
        LDR      R0,??UsbCdcRequest_0+0x20  ;; LineStateDelta
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  335 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  336         return((void*)UsbPass);
        MOVS     R0,#+0
        B        ??UsbCdcRequest_8
//  337       }
//  338       break;
//  339 
//  340 #if CDC_DEVICE_SUPPORT_BREAK > 0
//  341     case SEND_BREAK:
//  342 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  343       BreakCntr = CdcReqPacket.wValue.Word;
??UsbCdcRequest_5:
        LDR      R0,??UsbCdcRequest_0+0x8  ;; CdcReqPacket + 2
        LDRH     R0,[R0, #+0]
        LDR      R1,??UsbCdcRequest_0+0x24  ;; BreakCntr
        STR      R0,[R1, #+0]
//  344       LineStateDelta = TRUE;
        LDR      R0,??UsbCdcRequest_0+0x20  ;; LineStateDelta
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  345       if(BreakCntr != 0 && BreakCntr != 0xFFFF)
        LDR      R0,??UsbCdcRequest_0+0x24  ;; BreakCntr
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcRequest_9
        LDR      R0,??UsbCdcRequest_0+0x24  ;; BreakCntr
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCdcRequest_0+0x28  ;; 0xffff
        CMP      R0,R1
        BEQ      ??UsbCdcRequest_9
//  346       {
//  347         // Disable TIM1 counter
//  348         TIM_Cmd(TIM1, DISABLE);
        MOVS     R1,#+0
        LDR      R0,??UsbCdcRequest_0+0x2C  ;; 0xffff9000
        BL       TIM_Cmd
//  349         // Reset TIM1 counter
//  350         TIM_ResetCounter(TIM1);
        LDR      R0,??UsbCdcRequest_0+0x2C  ;; 0xffff9000
        BL       TIM_ResetCounter
//  351         // Clear TIM1 flags
//  352         TIM_ClearFlag(TIM1, TIM_FLAG_Update);
        MOVS     R1,#+1
        LDR      R0,??UsbCdcRequest_0+0x2C  ;; 0xffff9000
        BL       TIM_ClearFlag
//  353         // Enable TIM1 Update interrupt
//  354         TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
        MOVS     R2,#+1
        MOVS     R1,#+1
        LDR      R0,??UsbCdcRequest_0+0x2C  ;; 0xffff9000
        BL       TIM_ITConfig
//  355         // Enable TIM1 counter
//  356         TIM_Cmd(TIM1, ENABLE);
        MOVS     R1,#+1
        LDR      R0,??UsbCdcRequest_0+0x2C  ;; 0xffff9000
        BL       TIM_Cmd
//  357       }
//  358       // Send AKN
//  359       pCdcReqCtrl->Counter = 0;
??UsbCdcRequest_9:
        MOVS     R0,#+0
        STR      R0,[R5, #+4]
//  360 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  361       return((void*)UsbPass);
        MOVS     R0,#+0
        B        ??UsbCdcRequest_8
//  362 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//  363     }
//  364   }
//  365   CdcReqPacket.wLength.Word = 0;
??UsbCdcRequest_1:
        LDR      R0,??UsbCdcRequest_0+0x14  ;; CdcReqPacket + 6
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  366   return((void *)UsbFault);
        MOVS     R0,#+1
??UsbCdcRequest_8:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbCdcRequest_0:
        DC32     CdcReqPacket
        DC32     CdcReqPacket + 1
        DC32     CdcReqPacket + 2
        DC32     CdcReqPacket + 4
        DC32     CdcReqPacket + 7
        DC32     CdcReqPacket + 6
        DC32     CDC_LineCoding
        DC32     CDC_LineState
        DC32     LineStateDelta
        DC32     BreakCntr
        DC32     0xffff
        DC32     0xffff9000
        CFI EndBlock cfiBlock7
//  367 }
//  368 
//  369 /*************************************************************************
//  370  * Function Name: UsbCdcData
//  371  * Parameters:  void * pArg
//  372  *
//  373  * Return: void *
//  374  *
//  375  * Description: USB Communication Device Class Data receive
//  376  *
//  377  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon1
        CFI Function UsbCdcData
        THUMB
//  378 void * UsbCdcData (void * pArg)
//  379 {
UsbCdcData:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  380   if ((CdcReqPacket.bRequest == SET_LINE_CODING) &&
//  381       (CdcReqPacket.wLength.Word != 0))
        LDR      R0,??UsbCdcData_0  ;; CdcReqPacket + 1
        LDRB     R0,[R0, #+0]
        CMP      R0,#+32
        BNE      ??UsbCdcData_1
        LDR      R0,??UsbCdcData_0+0x4  ;; CdcReqPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcData_1
//  382   {
//  383 #if CDC_DEVICE_SUPPORT_LINE_CODING > 0
//  384     LineCodingDelta = TRUE;
        LDR      R0,??UsbCdcData_0+0x8  ;; LineCodingDelta
        MOVS     R2,#+1
        STR      R2,[R0, #+0]
//  385 #endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
//  386     return((void*)UsbPass);
        MOVS     R0,#+0
        B        ??UsbCdcData_2
//  387   }
//  388   return((void*)UsbFault);
??UsbCdcData_1:
        MOVS     R0,#+1
??UsbCdcData_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbCdcData_0:
        DC32     CdcReqPacket + 1
        DC32     CdcReqPacket + 6
        DC32     LineCodingDelta
        CFI EndBlock cfiBlock8
//  389 }
//  390 
//  391 /*************************************************************************
//  392  * Function Name: UsbCdcRead
//  393  * Parameters: pInt8U pBuffer, Int32U Size
//  394  *
//  395  * Return: Int32U
//  396  *
//  397  * Description: USB Communication Device Class data read. Return number
//  398  * of received the bytes.
//  399  *
//  400  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon1
        CFI Function UsbCdcRead
        THUMB
//  401 Int32U UsbCdcRead (pInt8U pBuffer, Int32U Size)
//  402 {
UsbCdcRead:
        PUSH     {R0-R2,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+32
        MOVS     R4,R0
        MOVS     R5,R1
//  403   Int32U ReadSize = 0, CurrReadSize = 0, Counter;
        MOVS     R0,#+0
        MOVS     R6,R0
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
//  404   FunctionalState cpu_sr;
//  405   
//  406   while(Size)
??UsbCdcRead_0:
        CMP      R5,#+0
        BEQ      ??UsbCdcRead_1
//  407   {
//  408     if(CDC_ReceiveIndx)
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcRead_3
//  409     {
//  410       CurrReadSize = MIN((CDC_ReceiveIndx - CDC_ReceiveIndxHold), Size);
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        LDR      R1,[R1, #+0]
        SUBS     R0,R0,R1
        CMP      R0,R5
        BCS      ??UsbCdcRead_4
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        LDR      R1,[R1, #+0]
        SUBS     R0,R0,R1
        STR      R0,[SP, #+0]
        B        ??UsbCdcRead_5
??UsbCdcRead_4:
        STR      R5,[SP, #+0]
//  411       Size     -= CurrReadSize;
??UsbCdcRead_5:
        LDR      R0,[SP, #+0]
        SUBS     R5,R5,R0
//  412       ReadSize += CurrReadSize;
        LDR      R0,[SP, #+0]
        ADDS     R6,R6,R0
//  413       for(Counter = 0; Counter < CurrReadSize; ++Counter)
        MOVS     R0,#+0
        MOVS     R7,R0
??UsbCdcRead_6:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??UsbCdcRead_7
//  414       {
//  415         *pBuffer++ = CDC_ReceiveBuffer[CDC_ReceiveIndxHold++];
        LDR      R0,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCdcRead_2+0x8  ;; CDC_ReceiveBuffer
        LDRB     R0,[R1, R0]
        STRB     R0,[R4, #+0]
        LDR      R0,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        STR      R0,[R1, #+0]
        ADDS     R4,R4,#+1
//  416       }
        ADDS     R7,R7,#+1
        B        ??UsbCdcRead_6
//  417       if(CDC_ReceiveIndxHold == CDC_ReceiveIndx)
??UsbCdcRead_7:
        LDR      R0,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R1,[R1, #+0]
        CMP      R0,R1
        BNE      ??UsbCdcRead_0
//  418       {
//  419         CDC_ReceiveIndxHold = 0;
        LDR      R0,??UsbCdcRead_2+0x4  ;; CDC_ReceiveIndxHold
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  420         if (USB_CDC_OutEpBufferNotEmpty)
        LDR      R0,??UsbCdcRead_2+0xC  ;; USB_CDC_OutEpBufferNotEmpty
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcRead_8
//  421         {
//  422           EIC_IRQDisCmd(cpu_sr);
        MOV      R0,SP
        LDR      R1,??UsbCdcRead_2+0x10  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        STRB     R2,[R0, #+4]
        MOVS     R0,#+0
        BL       EIC_IRQCmd
//  423           CDC_ReceiveIndx = CommOutEpMaxSize;
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        MOVS     R1,#+64
        STR      R1,[R0, #+0]
//  424           USB_EpRead((USB_Endpoint_t)CommOutEp,
//  425                       CDC_ReceiveBuffer,
//  426                      &CDC_ReceiveIndx);
        LDR      R2,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R1,??UsbCdcRead_2+0x8  ;; CDC_ReceiveBuffer
        MOVS     R0,#+4
        BL       USB_EpRead
//  427           USB_EpValidate((USB_Endpoint_t)CommOutEp,
//  428                          TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+4
        BL       USB_EpValidate
//  429           USB_CDC_OutEpBufferNotEmpty = FALSE;
        LDR      R0,??UsbCdcRead_2+0xC  ;; USB_CDC_OutEpBufferNotEmpty
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  430           EIC_IRQRestoreCmd(cpu_sr);
        MOV      R0,SP
        LDRB     R0,[R0, #+4]
        BL       EIC_IRQCmd
        B        ??UsbCdcRead_0
//  431         }
//  432         else
//  433         {
//  434           CDC_ReceiveIndx = 0;
??UsbCdcRead_8:
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        B        ??UsbCdcRead_0
//  435         }
//  436       }
//  437     }
//  438     else if (USB_CDC_OutEpBufferNotEmpty)
??UsbCdcRead_3:
        LDR      R0,??UsbCdcRead_2+0xC  ;; USB_CDC_OutEpBufferNotEmpty
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcRead_1
//  439     {
//  440       EIC_IRQDisCmd(cpu_sr);
        MOV      R0,SP
        LDR      R1,??UsbCdcRead_2+0x10  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        STRB     R2,[R0, #+4]
        MOVS     R0,#+0
        BL       EIC_IRQCmd
//  441       CDC_ReceiveIndx = CommOutEpMaxSize;
        LDR      R0,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        MOVS     R1,#+64
        STR      R1,[R0, #+0]
//  442       USB_EpRead((USB_Endpoint_t)CommOutEp,
//  443                   CDC_ReceiveBuffer,
//  444                  &CDC_ReceiveIndx);
        LDR      R2,??UsbCdcRead_2  ;; CDC_ReceiveIndx
        LDR      R1,??UsbCdcRead_2+0x8  ;; CDC_ReceiveBuffer
        MOVS     R0,#+4
        BL       USB_EpRead
//  445       USB_EpValidate((USB_Endpoint_t)CommOutEp,
//  446                      TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+4
        BL       USB_EpValidate
//  447       USB_CDC_OutEpBufferNotEmpty = FALSE;
        LDR      R0,??UsbCdcRead_2+0xC  ;; USB_CDC_OutEpBufferNotEmpty
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  448       EIC_IRQRestoreCmd(cpu_sr);
        MOV      R0,SP
        LDRB     R0,[R0, #+4]
        BL       EIC_IRQCmd
        B        ??UsbCdcRead_0
//  449     }
//  450     else
//  451     {
//  452       break;
//  453     }
//  454   }
//  455   return(ReadSize);
??UsbCdcRead_1:
        MOVS     R0,R6
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbCdcRead_2:
        DC32     CDC_ReceiveIndx
        DC32     CDC_ReceiveIndxHold
        DC32     CDC_ReceiveBuffer
        DC32     USB_CDC_OutEpBufferNotEmpty
        DC32     0xfffff800
        CFI EndBlock cfiBlock9
//  456 }
//  457 
//  458 /*************************************************************************
//  459  * Function Name: UsbCdcWrite
//  460  * Parameters:  pInt8U pBuffer, Int32U Size
//  461  *
//  462  * Return: CdcStatus_t
//  463  *
//  464  * Description: USB Communication Device Class data send.
//  465  *
//  466  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon1
        CFI Function UsbCdcWrite
        THUMB
//  467 Boolean UsbCdcWrite (pInt8U pBuffer, Int32U Size)
//  468 {
UsbCdcWrite:
        PUSH     {R0-R2,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+32
        MOVS     R4,R0
        MOVS     R5,R1
//  469 Boolean Status = !UsbCdcIsSendProgess();
        BL       UsbCdcIsSendProgess
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        MOVS     R6,R0
//  470 Int32U Counter, CurrWriteSize;
//  471 FunctionalState cpu_sr;
//  472   if(Status == TRUE)
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        CMP      R6,#+1
        BNE      ??UsbCdcWrite_0
//  473   {
//  474     pCDC_TransmitUserBuffer = pBuffer;
        LDR      R0,??UsbCdcWrite_1  ;; pCDC_TransmitUserBuffer
        STR      R4,[R0, #+0]
//  475     CDC_TransmitSize = Size;
        LDR      R0,??UsbCdcWrite_1+0x4  ;; CDC_TransmitSize
        STR      R5,[R0, #+0]
//  476     CurrWriteSize = MIN(CDC_TransmitSize,CommInEpMaxSize);
        LDR      R0,??UsbCdcWrite_1+0x4  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        CMP      R0,#+64
        BCS      ??UsbCdcWrite_2
        LDR      R0,??UsbCdcWrite_1+0x4  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+0]
        B        ??UsbCdcWrite_3
??UsbCdcWrite_2:
        MOVS     R0,#+64
        STR      R0,[SP, #+0]
//  477     for(Counter = 0; Counter < CurrWriteSize; ++Counter)
??UsbCdcWrite_3:
        MOVS     R0,#+0
        MOVS     R7,R0
??UsbCdcWrite_4:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??UsbCdcWrite_5
//  478     {
//  479       CDC_TransmitBuffer[Counter] = *pCDC_TransmitUserBuffer++;
        LDR      R0,??UsbCdcWrite_1  ;; pCDC_TransmitUserBuffer
        LDR      R0,[R0, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+1
        LDR      R2,??UsbCdcWrite_1  ;; pCDC_TransmitUserBuffer
        STR      R1,[R2, #+0]
        LDR      R1,??UsbCdcWrite_1+0x8  ;; CDC_TransmitBuffer
        LDRB     R0,[R0, #+0]
        STRB     R0,[R1, R7]
//  480     }
        ADDS     R7,R7,#+1
        B        ??UsbCdcWrite_4
//  481     CDC_TransmitSize -= CurrWriteSize;
??UsbCdcWrite_5:
        LDR      R0,??UsbCdcWrite_1+0x4  ;; CDC_TransmitSize
        LDR      R0,[R0, #+0]
        LDR      R1,[SP, #+0]
        SUBS     R0,R0,R1
        LDR      R1,??UsbCdcWrite_1+0x4  ;; CDC_TransmitSize
        STR      R0,[R1, #+0]
//  482     EIC_IRQDisCmd(cpu_sr);
        MOV      R0,SP
        LDR      R1,??UsbCdcWrite_1+0xC  ;; 0xfffff800
        LDR      R1,[R1, #+0]
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        STRB     R2,[R0, #+4]
        MOVS     R0,#+0
        BL       EIC_IRQCmd
//  483     USB_EpWrite((USB_Endpoint_t)CommInEp,CDC_TransmitBuffer,&CurrWriteSize);
        MOV      R2,SP
        LDR      R1,??UsbCdcWrite_1+0x8  ;; CDC_TransmitBuffer
        MOVS     R0,#+5
        BL       USB_EpWrite
//  484     EIC_IRQRestoreCmd(cpu_sr);
        MOV      R0,SP
        LDRB     R0,[R0, #+4]
        BL       EIC_IRQCmd
//  485   }
//  486   return(Status);
??UsbCdcWrite_0:
        MOVS     R0,R6
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbCdcWrite_1:
        DC32     pCDC_TransmitUserBuffer
        DC32     CDC_TransmitSize
        DC32     CDC_TransmitBuffer
        DC32     0xfffff800
        CFI EndBlock cfiBlock10
//  487 }
//  488 
//  489 /*************************************************************************
//  490  * Function Name: UsbCdcIsSendProgess
//  491  * Parameters:  none
//  492  *
//  493  * Return: Boolean
//  494  *
//  495  * Description: Is all data send?
//  496  *
//  497  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon1
        CFI Function UsbCdcIsSendProgess
        THUMB
//  498 Boolean UsbCdcIsSendProgess (void)
//  499 {
UsbCdcIsSendProgess:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  500   return(pCDC_TransmitUserBuffer != NULL);
        LDR      R0,??UsbCdcIsSendProgess_0  ;; pCDC_TransmitUserBuffer
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcIsSendProgess_1
        MOVS     R0,#+1
        B        ??UsbCdcIsSendProgess_2
??UsbCdcIsSendProgess_1:
        MOVS     R0,#+0
??UsbCdcIsSendProgess_2:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbCdcIsSendProgess_0:
        DC32     pCDC_TransmitUserBuffer
        CFI EndBlock cfiBlock11
//  501 }
//  502 
//  503 /*************************************************************************
//  504  * Function Name: UsbCdcIsCdcConfigure
//  505  * Parameters:  none
//  506  *
//  507  * Return: Boolean
//  508  *
//  509  * Description:  Is CDC configure yet?
//  510  *
//  511  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon1
        CFI Function UsbCdcIsCdcConfigure
        THUMB
//  512 Boolean UsbCdcIsCdcConfigure (void)
//  513 {
//  514   return(CDC_Configure);
UsbCdcIsCdcConfigure:
        LDR      R0,??UsbCdcIsCdcConfigure_0  ;; CDC_Configure
        LDRB     R0,[R0, #+0]
        BX       LR               ;; return
        Nop      
        DATA
??UsbCdcIsCdcConfigure_0:
        DC32     CDC_Configure
        CFI EndBlock cfiBlock12
//  515 }
//  516 
//  517 #if CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  518 /*************************************************************************
//  519  * Function Name: UsbCdcIsNewLineStateSettings
//  520  * Parameters:  none
//  521  *
//  522  * Return: Boolean
//  523  *
//  524  * Description: Is there a new modem settings received?
//  525  * RTS and DTR signals
//  526  *
//  527  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon1
        CFI Function UsbCdcIsNewLineStateSettings
        THUMB
//  528 Boolean UsbCdcIsNewLineStateSettings(void)
//  529 {
UsbCdcIsNewLineStateSettings:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  530 Boolean StateHold;
//  531   StateHold = (AtomicExchange(FALSE,(pInt32U)&LineStateDelta) != 0);
        LDR      R1,??UsbCdcIsNewLineStateSettings_0  ;; LineStateDelta
        MOVS     R0,#+0
        BL       AtomicExchange
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        MOVS     R4,R0
//  532   return(StateHold);
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbCdcIsNewLineStateSettings_0:
        DC32     LineStateDelta
        CFI EndBlock cfiBlock13
//  533 }
//  534 
//  535 /*************************************************************************
//  536  * Function Name: UsbCdcGetLineStateSettings
//  537  * Parameters:  none
//  538  *
//  539  * Return: CDC_LineState_t
//  540  *
//  541  * Description: Return the Line Signals states structure
//  542  * RTS and DTR signals
//  543  *
//  544  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon1
        CFI Function UsbCdcGetLineStateSettings
        THUMB
//  545 CDC_LineState_t UsbCdcGetLineStateSettings(void)
//  546 {
UsbCdcGetLineStateSettings:
        SUB      SP,SP,#+8
        CFI CFA R13+8
        LDR      R0,??UsbCdcGetLineStateSettings_0  ;; CDC_LineState
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+0]
//  547   return(CDC_LineState);
        LDR      R0,[SP, #+0]
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        Nop      
        DATA
??UsbCdcGetLineStateSettings_0:
        DC32     CDC_LineState
        CFI EndBlock cfiBlock14
//  548 }
//  549 
//  550 /*************************************************************************
//  551  * Function Name: UsbCdcReportSerialCommState
//  552  * Parameters: SerialState_t SerialState
//  553  *
//  554  * Return: none
//  555  *
//  556  * Description: Report the current state of serial communication channel
//  557  * Overrun Error,  Parity Error, Framing Error, Ring Signal, Break,
//  558  * Tx Carrier, Rx Carrier
//  559  *
//  560  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon1
        CFI Function UsbCdcReportSerialCommState
        THUMB
//  561 void UsbCdcReportSerialCommState(SerialState_t SerialState)
//  562 {
UsbCdcReportSerialCommState:
        PUSH     {R0,R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+12
        SUB      SP,SP,#+20
        CFI CFA R13+32
//  563 FunctionalState cpu_sr;
//  564 Int32U Size;
//  565 SerialStatePacket_t SerialStatePacket;
//  566   if(CDC_Configure == FALSE)
        LDR      R0,??UsbCdcReportSerialCommState_0  ;; CDC_Configure
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCdcReportSerialCommState_1
//  567   {
//  568     return;
//  569   }
//  570   SerialStatePacket.UsbSetupPacket.mRequestType.mRequestTypeData = 0xA1;
        MOV      R0,SP
        MOVS     R1,#+161
        STRB     R1,[R0, #+4]
//  571   SerialStatePacket.UsbSetupPacket.bRequest = SERIAL_STATE;
        ADD      R0,SP,#+4
        MOVS     R1,#+32
        STRB     R1,[R0, #+1]
//  572   SerialStatePacket.UsbSetupPacket.wValue.Word = 0;
        ADD      R0,SP,#+4
        MOVS     R1,#+0
        STRH     R1,[R0, #+2]
//  573   SerialStatePacket.UsbSetupPacket.wIndex.Word = CDC_DATA_INTERFACE_IND;
        ADD      R0,SP,#+4
        MOVS     R1,#+1
        STRH     R1,[R0, #+4]
//  574   SerialStatePacket.UsbSetupPacket.wLength.Word = sizeof(SerialState_t);
        ADD      R0,SP,#+4
        MOVS     R1,#+2
        STRH     R1,[R0, #+6]
//  575   SerialStatePacket.SerialState = SerialState;
        ADD      R0,SP,#+4
        ADDS     R0,R0,#+8
        ADD      R1,SP,#+20
        LDRB     R2,[R1, #0]
        STRB     R2,[R0, #0]
        LDRB     R2,[R1, #+1]
        STRB     R2,[R0, #+1]
//  576 
//  577   SerialStateDelta = TRUE;
        LDR      R0,??UsbCdcReportSerialCommState_0+0x4  ;; SerialStateDelta
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  578   Size = sizeof(SerialStatePacket_t);
        MOVS     R0,#+10
        STR      R0,[SP, #+0]
//  579   // Disable interrupt and save current state of the interrupt flags
//  580   EIC_IRQDisCmd(cpu_sr);
        LDR      R0,??UsbCdcReportSerialCommState_0+0x8  ;; 0xfffff800
        LDR      R0,[R0, #+0]
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOVS     R4,R1
        MOVS     R0,#+0
        BL       EIC_IRQCmd
//  581   USB_EpWrite((USB_Endpoint_t)ReportEp,(pInt8U)&SerialStatePacket,&Size);
        MOV      R2,SP
        ADD      R1,SP,#+4
        MOVS     R0,#+3
        BL       USB_EpWrite
//  582   EIC_IRQRestoreCmd(cpu_sr);
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       EIC_IRQCmd
//  583   while(SerialStateDelta);
??UsbCdcReportSerialCommState_2:
        LDR      R0,??UsbCdcReportSerialCommState_0+0x4  ;; SerialStateDelta
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCdcReportSerialCommState_2
??UsbCdcReportSerialCommState_1:
        ADD      SP,SP,#+24
        CFI CFA R13+8
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbCdcReportSerialCommState_0:
        DC32     CDC_Configure
        DC32     SerialStateDelta
        DC32     0xfffff800
        CFI EndBlock cfiBlock15
//  584 }
//  585 
//  586 #endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0
//  587 
//  588 #if CDC_DEVICE_SUPPORT_BREAK > 0
//  589 /*************************************************************************
//  590  * Function Name: CDC_BreakCallBack
//  591  * Parameters: void* arg
//  592  *
//  593  * Return: none
//  594  *
//  595  * Description: Break Timer callback routine that is called form the timer
//  596  * interrupt routine after expire of break period.
//  597  *
//  598  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon1
        CFI Function CDC_BreakCallBack
        THUMB
//  599 void CDC_BreakCallBack (void* arg)
//  600 {
//  601   BreakCntr = 0;
CDC_BreakCallBack:
        LDR      R1,??CDC_BreakCallBack_0  ;; BreakCntr
        MOVS     R2,#+0
        STR      R2,[R1, #+0]
//  602 }
        BX       LR               ;; return
        DATA
??CDC_BreakCallBack_0:
        DC32     BreakCntr
        CFI EndBlock cfiBlock16
//  603 
//  604 /*************************************************************************
//  605  * Function Name: UsbCdcGetBreakState
//  606  * Parameters:  none
//  607  *
//  608  * Return: Boolean
//  609  *
//  610  * Description: Return Break event state
//  611  *
//  612  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon1
        CFI Function UsbCdcGetBreakState
        THUMB
//  613 Boolean UsbCdcGetBreakState(void)
//  614 {
//  615   return(BreakCntr != 0);
UsbCdcGetBreakState:
        LDR      R0,??UsbCdcGetBreakState_0  ;; BreakCntr
        LDR      R0,[R0, #+0]
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BX       LR               ;; return
        Nop      
        DATA
??UsbCdcGetBreakState_0:
        DC32     BreakCntr
        CFI EndBlock cfiBlock17
//  616 }
//  617 #endif // CDC_DEVICE_SUPPORT_BREAK > 0
//  618 
//  619 #if CDC_DEVICE_SUPPORT_LINE_CODING > 0
//  620 /*************************************************************************
//  621  * Function Name: UsbCdcIsNewLineCtrlSettings
//  622  * Parameters:  none
//  623  *
//  624  * Return: Boolean
//  625  *
//  626  * Description: Is there a new line settings received?
//  627  * Baud rate, Data bits, Stop bits and Parity
//  628  *
//  629  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon1
        CFI Function UsbCdcIsNewLineCodingSettings
        THUMB
//  630 Boolean UsbCdcIsNewLineCodingSettings(void)
//  631 {
UsbCdcIsNewLineCodingSettings:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  632 Boolean StateHold;
//  633   StateHold = (AtomicExchange(FALSE,(pInt32U)&LineCodingDelta) != 0);
        LDR      R1,??UsbCdcIsNewLineCodingSettings_0  ;; LineCodingDelta
        MOVS     R0,#+0
        BL       AtomicExchange
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        MOVS     R4,R0
//  634   return(StateHold);
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbCdcIsNewLineCodingSettings_0:
        DC32     LineCodingDelta
        CFI EndBlock cfiBlock18
//  635 }
//  636 
//  637 /*************************************************************************
//  638  * Function Name: UsbCdcGetLineCodingSettings
//  639  * Parameters:  none
//  640  *
//  641  * Return: CDC_LineCoding_t
//  642  *
//  643  * Description: Return the Line Coding structure
//  644  * Baud rate, Data bits, Stop bits and Parity
//  645  *
//  646  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon1
        CFI Function UsbCdcGetLineCodingSettings
        THUMB
//  647 CDC_LineCoding_t UsbCdcGetLineCodingSettings(void)
//  648 {
UsbCdcGetLineCodingSettings:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  649   return(CDC_LineCoding);
        LDR      R1,??UsbCdcGetLineCodingSettings_0  ;; CDC_LineCoding
        MOVS     R2,#+7
        BL       __aeabi_memcpy
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbCdcGetLineCodingSettings_0:
        DC32     CDC_LineCoding
        CFI EndBlock cfiBlock19
//  650 }

        END
//  651 #endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0
// 
//   182 bytes in section .bss
// 1 940 bytes in section .text
// 
// 1 932 bytes of CODE memory (+ 8 bytes shared)
//   182 bytes of DATA memory
//
//Errors: none
//Warnings: none
