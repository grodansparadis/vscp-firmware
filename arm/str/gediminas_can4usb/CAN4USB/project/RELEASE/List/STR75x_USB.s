///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:32 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\module\STR75x_ /
//                    USB.c                                                   /
//    Command line =  D:\development\gediminas\CAN4USB\project\module\STR75x_ /
//                    USB.c -lC D:\development\gediminas\CAN4USB\project\RELE /
//                    ASE\List\ -lA D:\development\gediminas\CAN4USB\project\ /
//                    RELEASE\List\ -o D:\development\gediminas\CAN4USB\proje /
//                    ct\RELEASE\Obj\ --no_cse --no_unroll --no_inline        /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\S /
//                    TR75x_USB.s                                             /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME STR75x_USB

        RTMODEL "__dlib_file_descriptor", "0"

        SECTION USB_PACKET_MEMORY:DATA:NOROOT(0)

        EXTERN CFG_USBFilterConfig
        EXTERN EIC_IRQInit
        EXTERN MRCC_CKUSBConfig
        EXTERN MRCC_PeripheralClockConfig
        EXTERN MRCC_PeripheralSWResetConfig
        EXTERN UsbLanguagesStr
        EXTERN UsbStandardConfigurationDescriptor
        EXTERN UsbStandardDeviceDescriptorStr
        EXTERN UsbString
        EXTERN __aeabi_memcpy
        EXTERN __aeabi_memcpy4

        PUBLIC DlyCnt
        PUBLIC EpCtrlClr_CTR_RX
        PUBLIC EpCtrlClr_CTR_TX
        PUBLIC EpCtrlDataBuf
        PUBWEAK EpCtrlGet_CTR_RX
        PUBWEAK EpCtrlGet_CTR_TX
        PUBWEAK EpCtrlGet_DTOG_RX
        PUBWEAK EpCtrlGet_DTOG_TX
        PUBWEAK EpCtrlGet_SETUP
        PUBWEAK EpCtrlGet_STAT_RX
        PUBWEAK EpCtrlGet_STAT_TX
        PUBLIC EpCtrlSet_DTOG_RX
        PUBLIC EpCtrlSet_DTOG_TX
        PUBLIC EpCtrlSet_EA
        PUBLIC EpCtrlSet_EP_KIND
        PUBLIC EpCtrlSet_EP_TYPE
        PUBLIC EpCtrlSet_STAT_RX
        PUBLIC EpCtrlSet_STAT_TX
        PUBLIC EpCtrlToggle_DTOG_RX
        PUBLIC EpCtrlToggle_DTOG_TX
        PUBLIC EpInfo
        PUBLIC PacketMemBuff
        PUBWEAK USB_BTABLE
USB_BTABLE EQU 4294944848
        PUBLIC USB_ConnectRes
        PUBLIC USB_EpLogToPhysAdd
        PUBLIC USB_EpRead
        PUBLIC USB_EpValidate
        PUBLIC USB_EpWrite
        PUBLIC USB_GetDevStatus
        PUBLIC USB_HIGH_ISR
        PUBLIC USB_HwReset
        PUBLIC USB_ISR
        PUBLIC USB_Init
        PUBLIC USB_PacketMemory
        PUBLIC USB_RealizeEp
        PUBLIC USB_SetAdd
        PUBLIC USB_SetStallEP
        PUBLIC USB_UserFuncRegistering
        PUBLIC UsbCoreReq
        PUBLIC UsbCoreT9Fun
        PUBLIC UsbDevCtrl
        PUBLIC UsbEp0Ctrl
        PUBLIC UsbEp0SetupPacket
        PUBLIC UsbUserFun
        PUBWEAK _A_USB_CNTR
_A_USB_CNTR EQU 4294944832
        PUBWEAK _A_USB_DADDR
_A_USB_DADDR EQU 4294944844
        PUBWEAK _A_USB_EP0R
_A_USB_EP0R EQU 4294944768
        PUBWEAK _A_USB_FNR
_A_USB_FNR EQU 4294944840
        PUBWEAK _A_USB_ISTR
_A_USB_ISTR EQU 4294944836
        PUBWEAK __iar_via_R1
        PUBWEAK __iar_via_R6
        PUBWEAK memcpy
        PUBLIC pPacketMemUse
        
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
        
// D:\development\gediminas\CAN4USB\project\module\STR75x_USB.c
//    1 /*************************************************************************
//    2  *
//    3  *    Used with ICCARM and AARM.
//    4  *
//    5  *    (c) Copyright IAR Systems 2006
//    6  *
//    7  *    File name   : STR75x_usb.c
//    8  *    Description : usb module (HAL)
//    9  *
//   10  *    History :
//   11  *    1. Date        : July 28, 2006
//   12  *       Author      : Stanimir Bonev
//   13  *       Description : Create
//   14  *    2. Date        : November 20, 2006
//   15  *       Author      : Stanimir Bonev
//   16  *       Description : Modify
//   17  *    Fix problems with double buffered EPs
//   18  *
//   19  *    $Revision: 16170 $
//   20  **************************************************************************/
//   21 
//   22 #define HAL_USB_GLOBAL
//   23 #include "STR75x_usb.h"

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock0 Using cfiCommon0
        CFI Function memcpy
        THUMB
// __??Code16?? __intrinsic __code __interwork void *memcpy(void *, void const *, size_t)
memcpy:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
        MOVS     R6,R2
        MOVS     R2,R6
        MOVS     R1,R5
        MOVS     R0,R4
        BL       __aeabi_memcpy
        MOVS     R0,R4
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock0
//   24 
//   25 #pragma section="USB_PACKET_MEMORY"
//   26 
//   27 #pragma location="USB_PACKET_MEMORY"

        SECTION USB_PACKET_MEMORY:DATA:ROOT(2)
//   28 __root __no_init Int32U USB_PacketMemory[256];
USB_PacketMemory:
        DS8 1024

        SECTION USB_PACKET_MEMORY:DATA:NOROOT(0)
//   29 

        SECTION `.bss`:DATA:NOROOT(2)
//   30 pPacketMemUse_t pPacketMemUse;
pPacketMemUse:
        DS8 4
//   31 

        SECTION `.bss`:DATA:NOROOT(2)
//   32 PacketMemUse_t PacketMemBuff[EP_MAX_SLOTS*2];
PacketMemBuff:
        DS8 192
//   33 

        SECTION `.bss`:DATA:NOROOT(2)
//   34 EpInfo_t EpInfo[ENP_MAX_NUMB];
EpInfo:
        DS8 640
//   35 

        SECTION `.bss`:DATA:NOROOT(2)
//   36 Int32U DlyCnt;
DlyCnt:
        DS8 4
//   37 

        SECTION `.bss`:DATA:NOROOT(2)
//   38 UserFunc_t UsbUserFun [UsbLastEvent] =
UsbUserFun:
        DS8 176
//   39 {
//   40   // Ep 0 Out
//   41   NULL,
//   42   // Ep 0 In
//   43   NULL,
//   44   // Ep 1 Out
//   45   NULL,
//   46   // Ep 1 In
//   47   NULL,
//   48   // Ep 2 Out
//   49   NULL,
//   50   // Ep 2 Int
//   51   NULL,
//   52   // Ep 3 Out
//   53   NULL,
//   54   // Ep 3 In
//   55   NULL,
//   56   // Ep 4 Out
//   57   NULL,
//   58   // Ep 4 In
//   59   NULL,
//   60   // Ep 5 Out
//   61   NULL,
//   62   // Ep 5 In
//   63   NULL,
//   64   // Ep 6 Out
//   65   NULL,
//   66   // Ep 6 In
//   67   NULL,
//   68   // Ep 7 Out
//   69   NULL,
//   70   // Ep 7 In
//   71   NULL,
//   72   // Ep 8 Out
//   73   NULL,
//   74   // Ep 8 In
//   75   NULL,
//   76   // Ep 9 Out
//   77   NULL,
//   78   // Ep 9 In
//   79   NULL,
//   80   // Ep 10 Out
//   81   NULL,
//   82   // Ep 10 In
//   83   NULL,
//   84   // Ep 11 Out
//   85   NULL,
//   86   // Ep 11 In
//   87   NULL,
//   88   // Ep 12 Out
//   89   NULL,
//   90   // Ep 12 In
//   91   NULL,
//   92   // Ep 13 Out
//   93   NULL,
//   94   // Ep 13 In
//   95   NULL,
//   96   // Ep 14 Out
//   97   NULL,
//   98   // Ep 14 In
//   99   NULL,
//  100   // Ep 15 Out
//  101   NULL,
//  102   // Ep 15 In
//  103   NULL,
//  104   // UsbClassEp0OutPacket
//  105   NULL,
//  106   // UsbVendorEp0OutPacket
//  107   NULL,
//  108   // UsbUserSofEvent
//  109   NULL,
//  110   // UsbUserClass
//  111   NULL,
//  112   // UsbUserVendor
//  113   NULL,
//  114   // UsbUserGetDescriptor
//  115   NULL,
//  116   // UsbUserConfigure
//  117   NULL,
//  118   // UsbUserReset
//  119   NULL,
//  120   // UsbUserConnect
//  121   NULL,
//  122   // UsbUserSuspend
//  123   NULL,
//  124   // UsbUserEpStall
//  125   NULL,
//  126   // UsbUserEpUnStall
//  127   NULL,
//  128 };
//  129 

        SECTION `.bss`:DATA:NOROOT(2)
//  130 UsbEpCtrl_t UsbEp0Ctrl;
UsbEp0Ctrl:
        DS8 12
//  131 

        SECTION `.bss`:DATA:NOROOT(2)
//  132 UsbSetupPacket_t UsbEp0SetupPacket;
UsbEp0SetupPacket:
        DS8 8
//  133 
//  134 #pragma data_alignment=4

        SECTION `.bss`:DATA:NOROOT(2)
//  135 Int8U EpCtrlDataBuf[Ep0MaxSize];
EpCtrlDataBuf:
        DS8 8
//  136 

        SECTION `.bss`:DATA:NOROOT(2)
//  137 UsbDevCtrl_t UsbDevCtrl = {{UsbDevStatusUnknow,UsbDevStatusNoSuspend},0,0,0,0};
UsbDevCtrl:
        DS8 24
//  138 

        SECTION `.bss`:DATA:NOROOT(2)
//  139 CommUserFpnt_t UsbCoreT9Fun;
UsbCoreT9Fun:
        DS8 4
//  140 
//  141 #include "usb_common.c"

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function USB_UserFuncRegistering
        THUMB
// __??Code16?? __code __interwork UserFunc_t USB_UserFuncRegistering(UserFunc_t, UsbUserEvent_t)
USB_UserFuncRegistering:
        PUSH     {R0,R4}
        CFI R4 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R3,#+4
        MULS     R3,R1,R3
        LDR      R4,??DataTable1  ;; UsbUserFun
        LDR      R3,[R4, R3]
        MOVS     R0,R3
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R3,#+4
        MULS     R3,R1,R3
        LDR      R4,??DataTable1  ;; UsbUserFun
        STR      R2,[R4, R3]
        POP      {R3,R4}
        CFI R4 SameValue
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock1

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable1:
        DC32     UsbUserFun

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function UsbCoreReq
        THUMB
// __??Code16?? __code __interwork Int32U UsbCoreReq(UsbCoreReqType_t)
UsbCoreReq:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R0,R1
        CMP      R0,#+0
        BEQ      ??UsbCoreReq_0
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_1
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_4
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_5
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_6
        SUBS     R0,R0,#+1
        BEQ      ??UsbCoreReq_7
        B        ??UsbCoreReq_8
??UsbCoreReq_0:
        LDR      R0,??DataTable30  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        B        ??UsbCoreReq_9
??UsbCoreReq_1:
        LDR      R0,??DataTable30  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+7
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        B        ??UsbCoreReq_9
??UsbCoreReq_2:
        LDR      R0,??DataTable23  ;; UsbDevCtrl + 4
        LDR      R0,[R0, #+0]
        B        ??UsbCoreReq_9
??UsbCoreReq_3:
        LDR      R0,??DataTable21  ;; UsbDevCtrl + 8
        LDR      R0,[R0, #+0]
        B        ??UsbCoreReq_9
??UsbCoreReq_4:
        LDR      R0,??DataTable22  ;; UsbDevCtrl + 12
        LDR      R0,[R0, #+0]
        B        ??UsbCoreReq_9
??UsbCoreReq_5:
        LDR      R0,??DataTable24  ;; UsbDevCtrl + 16
        LDR      R0,[R0, #+0]
        B        ??UsbCoreReq_9
??UsbCoreReq_6:
        LDR      R0,??DataTable34  ;; UsbDevCtrl + 20
        LDRB     R0,[R0, #+0]
        MOVS     R2,#+2
        ANDS     R2,R2,R0
        MOVS     R0,R2
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        B        ??UsbCoreReq_9
??UsbCoreReq_7:
        LDR      R0,??DataTable34  ;; UsbDevCtrl + 20
        LDRB     R2,[R0, #+0]
        MOVS     R0,#+1
        ANDS     R0,R0,R2
        B        ??UsbCoreReq_9
??UsbCoreReq_8:
        MOVS     R0,#+0
        MVNS     R0,R0            ;; #-1
??UsbCoreReq_9:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock2

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function UsbSetDevState
        THUMB
// static __??Code16?? __code __interwork void UsbSetDevState(UsbT9DevState_t)
UsbSetDevState:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
        LDR      R0,??DataTable30  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+248
        ANDS     R1,R1,R0
        LSLS     R0,R4,#+29       ;; ZeroExtS R0,R4,#+29,#+29
        LSRS     R0,R0,#+29
        ORRS     R0,R0,R1
        LDR      R1,??DataTable30  ;; UsbDevCtrl
        STRB     R0,[R1, #+0]
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        SUBS     R0,R0,#+1
        CMP      R0,#+2
        BLS      ??UsbSetDevState_0
        SUBS     R0,R0,#+3
        BEQ      ??UsbSetDevState_1
        SUBS     R0,R0,#+1
        BEQ      ??UsbSetDevState_2
        B        ??UsbSetDevState_3
??UsbSetDevState_0:
        MOVS     R0,#+0
        BL       USB_Configure
        MOVS     R0,#+0
        BL       USB_SetAdd
        MOVS     R0,#+0
        LDR      R1,??DataTable31  ;; UsbUserFun + 152
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        MOVS     R0,#+0
        LDR      R1,??DataTable21  ;; UsbDevCtrl + 8
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable22  ;; UsbDevCtrl + 12
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable23  ;; UsbDevCtrl + 4
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable24  ;; UsbDevCtrl + 16
        STR      R0,[R1, #+0]
        B        ??UsbSetDevState_4
??UsbSetDevState_1:
        MOVS     R0,#+0
        BL       USB_Configure
        MOVS     R0,#+0
        LDR      R1,??DataTable21  ;; UsbDevCtrl + 8
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable22  ;; UsbDevCtrl + 12
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable23  ;; UsbDevCtrl + 4
        STR      R0,[R1, #+0]
        B        ??UsbSetDevState_4
??UsbSetDevState_2:
        MOVS     R0,#+1
        BL       USB_Configure
        B        ??UsbSetDevState_4
??UsbSetDevState_3:
        MOVS     R0,#+0
        BL       USB_Configure
        MOVS     R0,#+0
        LDR      R1,??DataTable31  ;; UsbUserFun + 152
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        MOVS     R0,#+0
        LDR      R1,??DataTable21  ;; UsbDevCtrl + 8
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable22  ;; UsbDevCtrl + 12
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable23  ;; UsbDevCtrl + 4
        STR      R0,[R1, #+0]
        LDR      R1,??DataTable24  ;; UsbDevCtrl + 16
        STR      R0,[R1, #+0]
??UsbSetDevState_4:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock3

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable21:
        DC32     UsbDevCtrl + 8

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable22:
        DC32     UsbDevCtrl + 12

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable23:
        DC32     UsbDevCtrl + 4

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable24:
        DC32     UsbDevCtrl + 16

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function UsbSetDevSuspend
        THUMB
// static __??Code16?? __code __interwork void UsbSetDevSuspend(Boolean)
UsbSetDevSuspend:
        MOVS     R1,R0
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        SUBS     R1,R1,#+1
        SBCS     R1,R1,R1
        MVNS     R1,R1
        LSRS     R1,R1,#+31
        LDR      R2,??DataTable30  ;; UsbDevCtrl
        LDRB     R2,[R2, #+0]
        LSLS     R2,R2,#+25       ;; ZeroExtS R2,R2,#+25,#+25
        LSRS     R2,R2,#+25
        LSLS     R1,R1,#+7
        ORRS     R1,R1,R2
        LDR      R2,??DataTable30  ;; UsbDevCtrl
        STRB     R1,[R2, #+0]
        BX       LR               ;; return
        CFI EndBlock cfiBlock4

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function UsbFindInterface
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbFindInterface(struct _UsbStandardConfigurationDescriptor_t *, UsbDevCtrl_t *)
UsbFindInterface:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R2,R0
        MOVS     R3,R2
??UsbFindInterface_0:
        LDRB     R4,[R2, #+2]
        LDRB     R0,[R2, #+3]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+16
        LSRS     R0,R0,#+16
        ORRS     R4,R4,R0
        ADDS     R0,R2,R4
        CMP      R3,R0
        BCS      ??UsbFindInterface_1
        LDRB     R0,[R3, #+1]
        CMP      R0,#+4
        BNE      ??UsbFindInterface_2
        LDRB     R0,[R3, #+2]
        LDR      R4,[R1, #+8]
        CMP      R0,R4
        BNE      ??UsbFindInterface_2
        LDRB     R0,[R3, #+3]
        LDR      R4,[R1, #+12]
        CMP      R0,R4
        BNE      ??UsbFindInterface_2
        MOVS     R0,#+0
        B        ??UsbFindInterface_3
??UsbFindInterface_2:
        LDRB     R0,[R3, #+0]
        ADDS     R3,R3,R0
        B        ??UsbFindInterface_0
??UsbFindInterface_1:
        MOVS     R0,#+1
??UsbFindInterface_3:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock5

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function UsbFindConfiguration
        THUMB
// static __??Code16?? __code __interwork struct _UsbStandardConfigurationDescriptor_t *UsbFindConfiguration(Int32U)
UsbFindConfiguration:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
        LDR      R0,??DataTable27  ;; UsbStandardConfigurationDescriptor
        MOVS     R2,R0
??UsbFindConfiguration_0:
        LDRB     R0,[R2, #+0]
        CMP      R0,#+0
        BEQ      ??UsbFindConfiguration_1
        LDRB     R0,[R2, #+1]
        CMP      R0,#+2
        BNE      ??UsbFindConfiguration_2
        LDRB     R0,[R2, #+5]
        CMP      R0,R1
        BNE      ??UsbFindConfiguration_2
        MOVS     R0,R2
        B        ??UsbFindConfiguration_3
??UsbFindConfiguration_2:
        LDRB     R0,[R2, #+0]
        ADDS     R2,R2,R0
        B        ??UsbFindConfiguration_0
??UsbFindConfiguration_1:
        MOVS     R0,#+0
??UsbFindConfiguration_3:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock6

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable27:
        DC32     UsbStandardConfigurationDescriptor

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function UsbSetConfigurtonState
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbSetConfigurtonState(Int32U)
UsbSetConfigurtonState:
        PUSH     {R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+12
        SUB      SP,SP,#+28
        CFI CFA R13+40
        MOVS     R4,R0
        CMP      R4,#+0
        BEQ      ??UsbSetConfigurtonState_0
        MOVS     R0,R4
        BL       UsbFindConfiguration
        MOVS     R5,R0
        MOV      R0,SP
        LDR      R1,??DataTable30  ;; UsbDevCtrl
        MOVS     R2,#+24
        BL       __aeabi_memcpy4
        CMP      R5,#+0
        BNE      ??UsbSetConfigurtonState_1
        MOVS     R0,#+1
        B        ??UsbSetConfigurtonState_2
??UsbSetConfigurtonState_1:
        STR      R4,[SP, #+4]
        MOVS     R0,#+0
        STR      R0,[SP, #+12]
        STR      R0,[SP, #+8]
        MOV      R0,SP
        LDRB     R0,[R0, #+20]
        MOVS     R1,#+253
        ANDS     R1,R1,R0
        MOV      R0,SP
        STRB     R1,[R0, #+20]
        LDRB     R0,[R5, #+7]
        MOVS     R1,#+64
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        MOV      R1,SP
        LDRB     R1,[R1, #+20]
        MOVS     R2,#+254
        ANDS     R2,R2,R1
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        ORRS     R1,R1,R2
        MOV      R0,SP
        STRB     R1,[R0, #+20]
        MOV      R1,SP
        MOVS     R0,R5
        BL       UsbFindInterface
        CMP      R0,#+0
        BEQ      ??UsbSetConfigurtonState_3
        MOVS     R0,#+1
        B        ??UsbSetConfigurtonState_2
??UsbSetConfigurtonState_3:
        MOV      R0,SP
        LDR      R1,??DataTable31  ;; UsbUserFun + 152
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        LDR      R0,??DataTable30  ;; UsbDevCtrl
        MOV      R1,SP
        MOVS     R2,#+24
        BL       __aeabi_memcpy4
        MOVS     R0,#+5
        BL       UsbSetDevState
        B        ??UsbSetConfigurtonState_4
??UsbSetConfigurtonState_0:
        MOVS     R0,#+0
        LDR      R1,??DataTable31  ;; UsbUserFun + 152
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        MOVS     R0,#+4
        BL       UsbSetDevState
??UsbSetConfigurtonState_4:
        MOVS     R0,#+0
??UsbSetConfigurtonState_2:
        ADD      SP,SP,#+28
        CFI CFA R13+12
        POP      {R4,R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock7

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable30:
        DC32     UsbDevCtrl

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable31:
        DC32     UsbUserFun + 152

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function UsbDevStatus
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbDevStatus(Int8U *, Int16U)
UsbDevStatus:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+0
        BNE      ??UsbDevStatus_0
        LDR      R0,??DataTable35  ;; UsbEp0Ctrl
        STR      R2,[R0, #+0]
        LDR      R0,??DataTable36  ;; UsbEp0Ctrl + 4
        MOVS     R3,#+2
        STR      R3,[R0, #+0]
        LDR      R0,??DataTable34  ;; UsbDevCtrl + 20
        LDRB     R0,[R0, #+0]
        STRB     R0,[R2, #+0]
        ADDS     R2,R2,#+1
        MOVS     R0,#+0
        STRB     R0,[R2, #+0]
        MOVS     R0,#+0
        B        ??UsbDevStatus_1
??UsbDevStatus_0:
        MOVS     R0,#+1
??UsbDevStatus_1:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock8

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable34:
        DC32     UsbDevCtrl + 20

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function UsbInterfaceStatus
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbInterfaceStatus(Int8U *, Int16U)
UsbInterfaceStatus:
        MOVS     R2,R0
        LDR      R0,??DataTable35  ;; UsbEp0Ctrl
        STR      R2,[R0, #+0]
        LDR      R0,??DataTable36  ;; UsbEp0Ctrl + 4
        MOVS     R3,#+2
        STR      R3,[R0, #+0]
        MOVS     R0,#+0
        STRB     R0,[R2, #+0]
        ADDS     R2,R2,#+1
        MOVS     R0,#+0
        STRB     R0,[R2, #+0]
        MOVS     R0,#+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock9

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable35:
        DC32     UsbEp0Ctrl

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable36:
        DC32     UsbEp0Ctrl + 4

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function UsbEpStatus
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbEpStatus(Int8U *, Int16U)
UsbEpStatus:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
        LSLS     R5,R5,#+16       ;; ZeroExtS R5,R5,#+16,#+16
        LSRS     R5,R5,#+16
        LSLS     R0,R5,#+25       ;; ZeroExtS R0,R5,#+25,#+25
        LSRS     R0,R0,#+25
        CMP      R0,#+17
        BLT      ??UsbEpStatus_0
        MOVS     R0,#+1
        B        ??UsbEpStatus_1
??UsbEpStatus_0:
        LDR      R0,??UsbEpStatus_2  ;; UsbEp0Ctrl
        STR      R4,[R0, #+0]
        LDR      R0,??UsbEpStatus_2+0x4  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+2
        STR      R1,[R0, #+0]
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_EpLogToPhysAdd
        MOVS     R1,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_GetStallEP
        ADDS     R4,R4,#+1
        MOVS     R0,#+0
        STRB     R0,[R4, #+0]
        MOVS     R0,#+0
??UsbEpStatus_1:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbEpStatus_2:
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 4
        CFI EndBlock cfiBlock10

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function UsbGetStatus
        THUMB
// static __??Code16?? __code __interwork void UsbGetStatus(void)
UsbGetStatus:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbGetStatus_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbGetStatus_1
        LDR      R0,??UsbGetStatus_0+0x4  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+2
        BNE      ??UsbGetStatus_1
        LDR      R0,??UsbGetStatus_0+0x8  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24
        BPL      ??UsbGetStatus_1
        LDR      R0,??UsbGetStatus_0+0xC  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbGetStatus_1
        LDR      R0,??UsbGetStatus_0+0x8  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbGetStatus_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbGetStatus_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbGetStatus_4
        B        ??UsbGetStatus_5
??UsbGetStatus_2:
        LDR      R0,??UsbGetStatus_0+0x10  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbGetStatus_0+0x14  ;; EpCtrlDataBuf
        BL       UsbDevStatus
        CMP      R0,#+0
        BNE      ??UsbGetStatus_6
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_6:
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_3:
        LDR      R0,??UsbGetStatus_0+0x10  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbGetStatus_0+0x14  ;; EpCtrlDataBuf
        BL       UsbInterfaceStatus
        CMP      R0,#+0
        BNE      ??UsbGetStatus_8
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_8:
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_4:
        LDR      R0,??UsbGetStatus_0+0x10  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbGetStatus_0+0x14  ;; EpCtrlDataBuf
        BL       UsbEpStatus
        CMP      R0,#+0
        BNE      ??UsbGetStatus_9
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_9:
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_5:
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetStatus_7
??UsbGetStatus_1:
        LDR      R0,??UsbGetStatus_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbGetStatus_7:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbGetStatus_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0SetupPacket + 4
        DC32     EpCtrlDataBuf
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock11

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function UsbClearDevFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbClearDevFeature(Int16U, Int16U)
UsbClearDevFeature:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+0
        BNE      ??UsbClearDevFeature_0
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        CMP      R2,#+1
        BEQ      ??UsbClearDevFeature_1
??UsbClearDevFeature_0:
        MOVS     R0,#+1
        B        ??UsbClearDevFeature_2
??UsbClearDevFeature_1:
        LDR      R0,??UsbClearDevFeature_3  ;; UsbDevCtrl + 20
        LDRB     R0,[R0, #+0]
        MOVS     R3,#+253
        ANDS     R3,R3,R0
        LDR      R0,??UsbClearDevFeature_3  ;; UsbDevCtrl + 20
        STRB     R3,[R0, #+0]
        MOVS     R0,#+0
??UsbClearDevFeature_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbClearDevFeature_3:
        DC32     UsbDevCtrl + 20
        CFI EndBlock cfiBlock12

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function UsbClearInterfaceFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbClearInterfaceFeature(Int16U, Int16U)
UsbClearInterfaceFeature:
        MOVS     R2,R0
        MOVS     R0,#+1
        BX       LR               ;; return
        CFI EndBlock cfiBlock13

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function UsbClearEpFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbClearEpFeature(Int16U, Int16U)
UsbClearEpFeature:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
        LSLS     R5,R5,#+16       ;; ZeroExtS R5,R5,#+16,#+16
        LSRS     R5,R5,#+16
        LSLS     R0,R5,#+25       ;; ZeroExtS R0,R5,#+25,#+25
        LSRS     R0,R0,#+25
        CMP      R0,#+17
        BGE      ??UsbClearEpFeature_0
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        CMP      R4,#+0
        BEQ      ??UsbClearEpFeature_1
??UsbClearEpFeature_0:
        MOVS     R0,#+1
        B        ??UsbClearEpFeature_2
??UsbClearEpFeature_1:
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_EpLogToPhysAdd
        MOVS     R6,R0
        MOVS     R1,#+0
        MOVS     R0,R6
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_SetStallEP
        LDR      R0,??UsbClearEpFeature_3  ;; UsbUserFun + 172
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbClearEpFeature_4
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R0,R6
        LDR      R1,??UsbClearEpFeature_3  ;; UsbUserFun + 172
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
??UsbClearEpFeature_4:
        MOVS     R0,#+0
??UsbClearEpFeature_2:
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbClearEpFeature_3:
        DC32     UsbUserFun + 172
        CFI EndBlock cfiBlock14

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function UsbClearFeature
        THUMB
// static __??Code16?? __code __interwork void UsbClearFeature(void)
UsbClearFeature:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbClearFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbClearFeature_1
        LDR      R0,??UsbClearFeature_0+0x4  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbClearFeature_1
        LDR      R0,??UsbClearFeature_0+0x8  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbClearFeature_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbClearFeature_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbClearFeature_4
        B        ??UsbClearFeature_5
??UsbClearFeature_2:
        LDR      R0,??UsbClearFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbClearDevFeature
        CMP      R0,#+0
        BNE      ??UsbClearFeature_6
        LDR      R0,??UsbClearFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_6:
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_3:
        LDR      R0,??UsbClearFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BCC      ??UsbClearFeature_8
        LDR      R0,??UsbClearFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbClearInterfaceFeature
        CMP      R0,#+0
        BNE      ??UsbClearFeature_8
        LDR      R0,??UsbClearFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_8:
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_4:
        LDR      R0,??UsbClearFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BCS      ??UsbClearFeature_9
        LDR      R0,??UsbClearFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+25
        BNE      ??UsbClearFeature_10
??UsbClearFeature_9:
        LDR      R0,??UsbClearFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbClearEpFeature
        CMP      R0,#+0
        BNE      ??UsbClearFeature_10
        LDR      R0,??UsbClearFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_10:
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_5:
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbClearFeature_7
??UsbClearFeature_1:
        LDR      R0,??UsbClearFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbClearFeature_7:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbClearFeature_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock15

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function UsbSetDevFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbSetDevFeature(Int16U, Int16U)
UsbSetDevFeature:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R2,R0
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        CMP      R1,#+0
        BNE      ??UsbSetDevFeature_0
        LSLS     R2,R2,#+16       ;; ZeroExtS R2,R2,#+16,#+16
        LSRS     R2,R2,#+16
        CMP      R2,#+1
        BEQ      ??UsbSetDevFeature_1
??UsbSetDevFeature_0:
        MOVS     R0,#+1
        B        ??UsbSetDevFeature_2
??UsbSetDevFeature_1:
        LDR      R0,??UsbSetDevFeature_3  ;; UsbDevCtrl + 20
        LDRB     R0,[R0, #+0]
        MOVS     R3,#+2
        ORRS     R3,R3,R0
        LDR      R0,??UsbSetDevFeature_3  ;; UsbDevCtrl + 20
        STRB     R3,[R0, #+0]
        MOVS     R0,#+0
??UsbSetDevFeature_2:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbSetDevFeature_3:
        DC32     UsbDevCtrl + 20
        CFI EndBlock cfiBlock16

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function UsbSetInterfaceFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbSetInterfaceFeature(Int16U, Int16U)
UsbSetInterfaceFeature:
        MOVS     R2,R0
        MOVS     R0,#+1
        BX       LR               ;; return
        CFI EndBlock cfiBlock17

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function UsbSetEpFeature
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbSetEpFeature(Int16U, Int16U)
UsbSetEpFeature:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
        LSLS     R5,R5,#+16       ;; ZeroExtS R5,R5,#+16,#+16
        LSRS     R5,R5,#+16
        LSLS     R0,R5,#+25       ;; ZeroExtS R0,R5,#+25,#+25
        LSRS     R0,R0,#+25
        CMP      R0,#+17
        BGE      ??UsbSetEpFeature_0
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        CMP      R4,#+0
        BEQ      ??UsbSetEpFeature_1
??UsbSetEpFeature_0:
        MOVS     R0,#+1
        B        ??UsbSetEpFeature_2
??UsbSetEpFeature_1:
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_EpLogToPhysAdd
        MOVS     R6,R0
        MOVS     R1,#+1
        MOVS     R0,R6
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_SetStallEP
        LDR      R0,??UsbSetEpFeature_3  ;; UsbUserFun + 168
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbSetEpFeature_4
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R0,R6
        LDR      R1,??UsbSetEpFeature_3  ;; UsbUserFun + 168
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
??UsbSetEpFeature_4:
        MOVS     R0,#+0
??UsbSetEpFeature_2:
        POP      {R4-R6}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbSetEpFeature_3:
        DC32     UsbUserFun + 168
        CFI EndBlock cfiBlock18

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon0
        CFI Function UsbSetFeature
        THUMB
// static __??Code16?? __code __interwork void UsbSetFeature(void)
UsbSetFeature:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbSetFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbSetFeature_1
        LDR      R0,??UsbSetFeature_0+0x4  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetFeature_1
        LDR      R0,??UsbSetFeature_0+0x8  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbSetFeature_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbSetFeature_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbSetFeature_4
        B        ??UsbSetFeature_5
??UsbSetFeature_2:
        LDR      R0,??UsbSetFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbSetDevFeature
        CMP      R0,#+0
        BNE      ??UsbSetFeature_6
        LDR      R0,??UsbSetFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_6:
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_3:
        LDR      R0,??UsbSetFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BCC      ??UsbSetFeature_8
        LDR      R0,??UsbSetFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbSetInterfaceFeature
        CMP      R0,#+0
        BNE      ??UsbSetFeature_8
        LDR      R0,??UsbSetFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_8:
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_4:
        LDR      R0,??UsbSetFeature_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BCS      ??UsbSetFeature_9
        LDR      R0,??UsbSetFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+25
        BNE      ??UsbSetFeature_10
??UsbSetFeature_9:
        LDR      R0,??UsbSetFeature_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        BL       UsbSetEpFeature
        CMP      R0,#+0
        BNE      ??UsbSetFeature_10
        LDR      R0,??UsbSetFeature_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_10:
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_5:
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetFeature_7
??UsbSetFeature_1:
        LDR      R0,??UsbSetFeature_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbSetFeature_7:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbSetFeature_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock19

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock20 Using cfiCommon0
        CFI Function UsbSetAddress
        THUMB
// static __??Code16?? __code __interwork void UsbSetAddress(void)
UsbSetAddress:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbSetAddress_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+3
        BCC      ??UsbSetAddress_1
        LDR      R0,??UsbSetAddress_0+0x4  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27
        BNE      ??UsbSetAddress_1
        LDR      R0,??UsbSetAddress_0+0x8  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetAddress_1
        LDR      R0,??UsbSetAddress_0+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetAddress_1
        LDR      R0,??UsbSetAddress_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+128
        BCS      ??UsbSetAddress_1
        LDR      R0,??UsbSetAddress_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetAddress_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetAddress_2
??UsbSetAddress_1:
        LDR      R0,??UsbSetAddress_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbSetAddress_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbSetAddress_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock20

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock21 Using cfiCommon0
        CFI Function UsbGetDescriptorDevice
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbGetDescriptorDevice(Int8U)
UsbGetDescriptorDevice:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+0
        BNE      ??UsbGetDescriptorDevice_0
        LDR      R0,??UsbGetDescriptorDevice_1  ;; UsbEp0Ctrl + 4
        LDR      R2,??UsbGetDescriptorDevice_1+0x4  ;; UsbStandardDeviceDescriptorStr
        LDRB     R2,[R2, #+0]
        STR      R2,[R0, #+0]
        LDR      R0,??UsbGetDescriptorDevice_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R2,#+0
        STRB     R2,[R0, #+0]
        LDR      R0,??UsbGetDescriptorDevice_1  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R2,??UsbGetDescriptorDevice_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R2,[R2, #+0]
        CMP      R0,R2
        BCC      ??UsbGetDescriptorDevice_2
        LDR      R0,??UsbGetDescriptorDevice_1  ;; UsbEp0Ctrl + 4
        LDR      R2,??UsbGetDescriptorDevice_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R2,[R2, #+0]
        STR      R2,[R0, #+0]
        LDR      R0,??UsbGetDescriptorDevice_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R2,#+1
        STRB     R2,[R0, #+0]
??UsbGetDescriptorDevice_2:
        LDR      R0,??UsbGetDescriptorDevice_1+0x10  ;; UsbEp0Ctrl
        LDR      R2,??UsbGetDescriptorDevice_1+0x4  ;; UsbStandardDeviceDescriptorStr
        STR      R2,[R0, #+0]
        LDR      R0,??UsbGetDescriptorDevice_1+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R2,#+3
        STRB     R2,[R0, #+0]
        MOVS     R0,#+0
        B        ??UsbGetDescriptorDevice_3
??UsbGetDescriptorDevice_0:
        MOVS     R0,#+1
??UsbGetDescriptorDevice_3:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??UsbGetDescriptorDevice_1:
        DC32     UsbEp0Ctrl + 4
        DC32     UsbStandardDeviceDescriptorStr
        DC32     UsbEp0Ctrl + 9
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock21

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock22 Using cfiCommon0
        CFI Function UsbGetDescriptorConfiguration
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbGetDescriptorConfiguration(Int8U)
UsbGetDescriptorConfiguration:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
        LDR      R0,??UsbGetDescriptorConfiguration_0  ;; UsbStandardConfigurationDescriptor
        MOVS     R2,R0
??UsbGetDescriptorConfiguration_1:
        LDRB     R0,[R2, #+0]
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptorConfiguration_2
        LDRB     R0,[R2, #+1]
        CMP      R0,#+2
        BNE      ??UsbGetDescriptorConfiguration_3
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x4  ;; UsbStandardDeviceDescriptorStr + 17
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BEQ      ??UsbGetDescriptorConfiguration_2
        LDRB     R0,[R2, #+5]
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R0,R1
        BEQ      ??UsbGetDescriptorConfiguration_2
??UsbGetDescriptorConfiguration_3:
        LDRB     R0,[R2, #+0]
        ADDS     R2,R2,R0
        B        ??UsbGetDescriptorConfiguration_1
??UsbGetDescriptorConfiguration_2:
        LDRB     R0,[R2, #+0]
        CMP      R0,#+0
        BNE      ??UsbGetDescriptorConfiguration_4
        MOVS     R0,#+1
        B        ??UsbGetDescriptorConfiguration_5
??UsbGetDescriptorConfiguration_4:
        LDRB     R3,[R2, #+2]
        LDRB     R0,[R2, #+3]
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+16
        LSRS     R0,R0,#+16
        ORRS     R3,R3,R0
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x8  ;; UsbEp0Ctrl + 4
        STR      R3,[R0, #+0]
        LDR      R0,??UsbGetDescriptorConfiguration_0+0xC  ;; UsbEp0Ctrl + 9
        MOVS     R3,#+0
        STRB     R3,[R0, #+0]
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x10  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        LDR      R3,??UsbGetDescriptorConfiguration_0+0x8  ;; UsbEp0Ctrl + 4
        LDR      R3,[R3, #+0]
        CMP      R0,R3
        BCS      ??UsbGetDescriptorConfiguration_6
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x8  ;; UsbEp0Ctrl + 4
        LDR      R3,??UsbGetDescriptorConfiguration_0+0x10  ;; UsbEp0SetupPacket + 6
        LDRH     R3,[R3, #+0]
        STR      R3,[R0, #+0]
        LDR      R0,??UsbGetDescriptorConfiguration_0+0xC  ;; UsbEp0Ctrl + 9
        MOVS     R3,#+1
        STRB     R3,[R0, #+0]
??UsbGetDescriptorConfiguration_6:
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x14  ;; UsbEp0Ctrl
        STR      R2,[R0, #+0]
        LDR      R0,??UsbGetDescriptorConfiguration_0+0x18  ;; UsbEp0Ctrl + 8
        MOVS     R3,#+3
        STRB     R3,[R0, #+0]
        MOVS     R0,#+0
??UsbGetDescriptorConfiguration_5:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbGetDescriptorConfiguration_0:
        DC32     UsbStandardConfigurationDescriptor
        DC32     UsbStandardDeviceDescriptorStr + 17
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 9
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock22

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock23 Using cfiCommon0
        CFI Function UsbGetDescriptorString
        THUMB
// static __??Code16?? __code __interwork UsbCommStatus_t UsbGetDescriptorString(Int8U)
UsbGetDescriptorString:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R1,R0
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,#+0
        BNE      ??UsbGetDescriptorString_0
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R4,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        LDRB     R4,[R4, #+0]
        STR      R4,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R4,#+0
        STRB     R4,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R4,??UsbGetDescriptorString_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R4,[R4, #+0]
        CMP      R0,R4
        BCC      ??UsbGetDescriptorString_2
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R4,??UsbGetDescriptorString_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R4,[R4, #+0]
        STR      R4,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R4,#+1
        STRB     R4,[R0, #+0]
??UsbGetDescriptorString_2:
        LDR      R0,??UsbGetDescriptorString_1+0x10  ;; UsbEp0Ctrl
        LDR      R4,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        STR      R4,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R4,#+3
        STRB     R4,[R0, #+0]
        MOVS     R0,#+0
        B        ??UsbGetDescriptorString_3
??UsbGetDescriptorString_0:
        MOVS     R0,#+0
        MOVS     R4,R0
        MOVS     R0,#+2
        MOVS     R3,R0
??UsbGetDescriptorString_4:
        LDR      R0,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        LDRB     R0,[R0, #+0]
        CMP      R3,R0
        BCS      ??UsbGetDescriptorString_5
        LDR      R0,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        LDRB     R0,[R0, R3]
        LDR      R5,??UsbGetDescriptorString_1+0x18  ;; UsbEp0SetupPacket + 4
        LDRB     R5,[R5, #+0]
        CMP      R0,R5
        BNE      ??UsbGetDescriptorString_6
        LDR      R0,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        ADDS     R0,R0,R3
        LDRB     R0,[R0, #+1]
        LDR      R5,??UsbGetDescriptorString_1+0x1C  ;; UsbEp0SetupPacket + 5
        LDRB     R5,[R5, #+0]
        CMP      R0,R5
        BEQ      ??UsbGetDescriptorString_5
??UsbGetDescriptorString_6:
        ADDS     R4,R4,#+1
        ADDS     R3,R3,#+2
        B        ??UsbGetDescriptorString_4
??UsbGetDescriptorString_5:
        LDR      R0,??UsbGetDescriptorString_1+0x4  ;; UsbLanguagesStr
        LDRB     R0,[R0, #+0]
        CMP      R3,R0
        BCC      ??UsbGetDescriptorString_7
        MOVS     R0,#+1
        B        ??UsbGetDescriptorString_3
??UsbGetDescriptorString_7:
        MOVS     R0,#+4
        MULS     R0,R4,R0
        LDR      R5,??UsbGetDescriptorString_1+0x20  ;; UsbString
        LDR      R0,[R5, R0]
        MOVS     R2,R0
??UsbGetDescriptorString_8:
        CMP      R2,#+0
        BEQ      ??UsbGetDescriptorString_9
        SUBS     R1,R1,#+1
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptorString_9
        ADDS     R2,R2,#+4
        B        ??UsbGetDescriptorString_8
??UsbGetDescriptorString_9:
        CMP      R2,#+0
        BNE      ??UsbGetDescriptorString_10
        MOVS     R0,#+1
        B        ??UsbGetDescriptorString_3
??UsbGetDescriptorString_10:
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R5,[R2, #+0]
        LDRB     R5,[R5, #+0]
        STR      R5,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R5,#+0
        STRB     R5,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R5,??UsbGetDescriptorString_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R5,[R5, #+0]
        CMP      R0,R5
        BCC      ??UsbGetDescriptorString_11
        LDR      R0,??UsbGetDescriptorString_1  ;; UsbEp0Ctrl + 4
        LDR      R5,??UsbGetDescriptorString_1+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R5,[R5, #+0]
        STR      R5,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x8  ;; UsbEp0Ctrl + 9
        MOVS     R5,#+1
        STRB     R5,[R0, #+0]
??UsbGetDescriptorString_11:
        LDR      R0,??UsbGetDescriptorString_1+0x10  ;; UsbEp0Ctrl
        LDR      R5,[R2, #+0]
        STR      R5,[R0, #+0]
        LDR      R0,??UsbGetDescriptorString_1+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R5,#+3
        STRB     R5,[R0, #+0]
        MOVS     R0,#+0
??UsbGetDescriptorString_3:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbGetDescriptorString_1:
        DC32     UsbEp0Ctrl + 4
        DC32     UsbLanguagesStr
        DC32     UsbEp0Ctrl + 9
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 8
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 5
        DC32     UsbString
        CFI EndBlock cfiBlock23

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock24 Using cfiCommon0
        CFI Function UsbGetDescriptor
        THUMB
// static __??Code16?? __code __interwork void UsbGetDescriptor(void)
UsbGetDescriptor:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbGetDescriptor_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+3
        BCC      ??UsbGetDescriptor_1
        LDR      R0,??UsbGetDescriptor_0+0x4  ;; UsbEp0SetupPacket + 3
        LDRB     R0,[R0, #+0]
        SUBS     R0,R0,#+1
        BEQ      ??UsbGetDescriptor_2
        SUBS     R0,R0,#+1
        BEQ      ??UsbGetDescriptor_3
        SUBS     R0,R0,#+1
        BEQ      ??UsbGetDescriptor_4
        B        ??UsbGetDescriptor_5
??UsbGetDescriptor_2:
        LDR      R0,??UsbGetDescriptor_0+0x8  ;; UsbEp0SetupPacket + 2
        LDRB     R0,[R0, #+0]
        BL       UsbGetDescriptorDevice
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptor_6
        LDR      R0,??UsbGetDescriptor_0+0xC  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetDescriptor_6
??UsbGetDescriptor_3:
        LDR      R0,??UsbGetDescriptor_0+0x8  ;; UsbEp0SetupPacket + 2
        LDRB     R0,[R0, #+0]
        BL       UsbGetDescriptorConfiguration
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptor_6
        LDR      R0,??UsbGetDescriptor_0+0xC  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetDescriptor_6
??UsbGetDescriptor_4:
        LDR      R0,??UsbGetDescriptor_0+0x8  ;; UsbEp0SetupPacket + 2
        LDRB     R0,[R0, #+0]
        BL       UsbGetDescriptorString
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptor_6
        LDR      R0,??UsbGetDescriptor_0+0xC  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetDescriptor_6
??UsbGetDescriptor_5:
        LDR      R0,??UsbGetDescriptor_0+0x10  ;; UsbUserFun + 148
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptor_7
        LDR      R0,??UsbGetDescriptor_0+0x14  ;; UsbEp0Ctrl
        LDR      R1,??UsbGetDescriptor_0+0x10  ;; UsbUserFun + 148
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BEQ      ??UsbGetDescriptor_6
??UsbGetDescriptor_7:
        LDR      R0,??UsbGetDescriptor_0+0xC  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbGetDescriptor_6
??UsbGetDescriptor_1:
        LDR      R0,??UsbGetDescriptor_0+0xC  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbGetDescriptor_6:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbGetDescriptor_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 3
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 8
        DC32     UsbUserFun + 148
        DC32     UsbEp0Ctrl
        CFI EndBlock cfiBlock24

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock25 Using cfiCommon0
        CFI Function UsbSetDescriptor
        THUMB
// static __??Code16?? __code __interwork void UsbSetDescriptor(void)
UsbSetDescriptor:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbSetDescriptor_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbSetDescriptor_1
        LDR      R0,??UsbSetDescriptor_0+0x4  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetDescriptor_2
??UsbSetDescriptor_1:
        LDR      R0,??UsbSetDescriptor_0+0x4  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbSetDescriptor_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbSetDescriptor_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock25

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock26 Using cfiCommon0
        CFI Function UsbGetConfiguration
        THUMB
// static __??Code16?? __code __interwork void UsbGetConfiguration(void)
UsbGetConfiguration:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbGetConfiguration_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbGetConfiguration_1
        LDR      R0,??UsbGetConfiguration_0+0x4  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbGetConfiguration_1
        LDR      R0,??UsbGetConfiguration_0+0x8  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbGetConfiguration_1
        LDR      R0,??UsbGetConfiguration_0+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??UsbGetConfiguration_1
        LDR      R0,??UsbGetConfiguration_0+0x10  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
        MOVS     R0,#+2
        BL       UsbCoreReq
        LDR      R1,??UsbGetConfiguration_0+0x14  ;; EpCtrlDataBuf
        STRB     R0,[R1, #+0]
        LDR      R0,??UsbGetConfiguration_0+0x18  ;; UsbEp0Ctrl
        LDR      R1,??UsbGetConfiguration_0+0x14  ;; EpCtrlDataBuf
        STR      R1,[R0, #+0]
        LDR      R0,??UsbGetConfiguration_0+0x1C  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbGetConfiguration_2
??UsbGetConfiguration_1:
        LDR      R0,??UsbGetConfiguration_0+0x1C  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbGetConfiguration_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbGetConfiguration_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0Ctrl + 4
        DC32     EpCtrlDataBuf
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock26

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock27 Using cfiCommon0
        CFI Function UsbSetConfiguration
        THUMB
// static __??Code16?? __code __interwork void UsbSetConfiguration(void)
UsbSetConfiguration:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbSetConfiguration_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+4
        BCC      ??UsbSetConfiguration_1
        LDR      R0,??UsbSetConfiguration_0+0x4  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetConfiguration_1
        LDR      R0,??UsbSetConfiguration_0+0x8  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetConfiguration_1
        LDR      R0,??UsbSetConfiguration_0+0xC  ;; UsbEp0SetupPacket + 2
        LDRB     R0,[R0, #+0]
        BL       UsbSetConfigurtonState
        CMP      R0,#+0
        BNE      ??UsbSetConfiguration_1
        LDR      R0,??UsbSetConfiguration_0+0x10  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetConfiguration_0+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetConfiguration_2
??UsbSetConfiguration_1:
        LDR      R0,??UsbSetConfiguration_0+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbSetConfiguration_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbSetConfiguration_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock27

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock28 Using cfiCommon0
        CFI Function UsbGetInterface
        THUMB
// static __??Code16?? __code __interwork void UsbGetInterface(void)
UsbGetInterface:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        LDR      R0,??UsbGetInterface_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BNE      ??UsbGetInterface_1
        LDR      R0,??UsbGetInterface_0+0x4  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        LDR      R1,??UsbGetInterface_0+0x8  ;; UsbDevCtrl + 8
        LDR      R1,[R1, #+0]
        CMP      R0,R1
        BNE      ??UsbGetInterface_1
        LDR      R0,??UsbGetInterface_0+0xC  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??UsbGetInterface_1
        LDR      R0,??UsbGetInterface_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbGetInterface_1
        LDR      R0,??UsbGetInterface_0+0x14  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
        LDR      R0,??UsbGetInterface_0+0x18  ;; UsbDevCtrl + 12
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbGetInterface_0+0x1C  ;; EpCtrlDataBuf
        STRB     R0,[R1, #+0]
        LDR      R0,??UsbGetInterface_0+0x20  ;; UsbEp0Ctrl
        LDR      R1,??UsbGetInterface_0+0x1C  ;; EpCtrlDataBuf
        STR      R1,[R0, #+0]
        LDR      R0,??UsbGetInterface_0+0x24  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbGetInterface_2
??UsbGetInterface_1:
        LDR      R0,??UsbGetInterface_0+0x8  ;; UsbDevCtrl + 8
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+0]
        LDR      R0,??UsbGetInterface_0+0x24  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbGetInterface_2:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbGetInterface_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbDevCtrl + 8
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 4
        DC32     UsbDevCtrl + 12
        DC32     EpCtrlDataBuf
        DC32     UsbEp0Ctrl
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock28

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock29 Using cfiCommon0
        CFI Function UsbSetInterface
        THUMB
// static __??Code16?? __code __interwork void UsbSetInterface(void)
UsbSetInterface:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        SUB      SP,SP,#+24
        CFI CFA R13+32
        LDR      R0,??UsbSetInterface_0  ;; UsbDevCtrl
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+29       ;; ZeroExtS R0,R0,#+29,#+29
        LSRS     R0,R0,#+29
        CMP      R0,#+5
        BNE      ??UsbSetInterface_1
        LDR      R0,??UsbSetInterface_0+0x4  ;; UsbEp0SetupPacket + 6
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbSetInterface_1
        LDR      R0,??UsbSetInterface_0+0x8  ;; UsbDevCtrl + 4
        LDR      R0,[R0, #+0]
        BL       UsbFindConfiguration
        MOVS     R4,R0
        MOV      R0,SP
        LDR      R1,??UsbSetInterface_0  ;; UsbDevCtrl
        MOVS     R2,#+24
        BL       __aeabi_memcpy4
        LDR      R0,??UsbSetInterface_0+0xC  ;; UsbEp0SetupPacket + 4
        LDRH     R0,[R0, #+0]
        STR      R0,[SP, #+8]
        LDR      R0,??UsbSetInterface_0+0x10  ;; UsbEp0SetupPacket + 2
        LDRH     R0,[R0, #+0]
        STR      R0,[SP, #+12]
        MOV      R0,SP
        LDRB     R0,[R0, #+20]
        MOVS     R1,#+253
        ANDS     R1,R1,R0
        MOV      R0,SP
        STRB     R1,[R0, #+20]
        MOV      R1,SP
        MOVS     R0,R4
        BL       UsbFindInterface
        CMP      R0,#+0
        BEQ      ??UsbSetInterface_2
        LDR      R0,??UsbSetInterface_0+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbSetInterface_3
??UsbSetInterface_2:
        MOV      R0,SP
        LDR      R1,??UsbSetInterface_0+0x18  ;; UsbUserFun + 152
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        LDR      R0,??UsbSetInterface_0  ;; UsbDevCtrl
        MOV      R1,SP
        MOVS     R2,#+24
        BL       __aeabi_memcpy4
        LDR      R0,??UsbSetInterface_0+0x1C  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        LDR      R0,??UsbSetInterface_0+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbSetInterface_3
??UsbSetInterface_1:
        LDR      R0,??UsbSetInterface_0+0x14  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
??UsbSetInterface_3:
        ADD      SP,SP,#+24
        CFI CFA R13+8
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbSetInterface_0:
        DC32     UsbDevCtrl
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbDevCtrl + 4
        DC32     UsbEp0SetupPacket + 4
        DC32     UsbEp0SetupPacket + 2
        DC32     UsbEp0Ctrl + 8
        DC32     UsbUserFun + 152
        DC32     UsbEp0Ctrl + 4
        CFI EndBlock cfiBlock29

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock30 Using cfiCommon0
        CFI Function UsbSynchFrame
        THUMB
// static __??Code16?? __code __interwork void UsbSynchFrame(void)
UsbSynchFrame:
        LDR      R0,??UsbSynchFrame_0  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        BX       LR               ;; return
        DATA
??UsbSynchFrame_0:
        DC32     UsbEp0Ctrl + 8
        CFI EndBlock cfiBlock30

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock31 Using cfiCommon0
        CFI Function UsbDevConnectCallback
        THUMB
// static __??Code16?? __code __interwork void UsbDevConnectCallback(Boolean)
UsbDevConnectCallback:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       UsbSetDevState
        LDR      R0,??UsbDevConnectCallback_0  ;; UsbUserFun + 160
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbDevConnectCallback_1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        LDR      R1,??UsbDevConnectCallback_0  ;; UsbUserFun + 160
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
??UsbDevConnectCallback_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbDevConnectCallback_0:
        DC32     UsbUserFun + 160
        CFI EndBlock cfiBlock31

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock32 Using cfiCommon0
        CFI Function UsbDevSuspendCallback
        THUMB
// static __??Code16?? __code __interwork void UsbDevSuspendCallback(Boolean)
UsbDevSuspendCallback:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       UsbSetDevSuspend
        LDR      R0,??UsbDevSuspendCallback_0  ;; UsbUserFun + 164
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbDevSuspendCallback_1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        LDR      R1,??UsbDevSuspendCallback_0  ;; UsbUserFun + 164
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
??UsbDevSuspendCallback_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbDevSuspendCallback_0:
        DC32     UsbUserFun + 164
        CFI EndBlock cfiBlock32

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock33 Using cfiCommon0
        CFI Function UsbDevResetCallback
        THUMB
// static __??Code16?? __code __interwork void UsbDevResetCallback(void)
UsbDevResetCallback:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R0,#+3
        BL       UsbSetDevState
        LDR      R0,??UsbDevResetCallback_0  ;; UsbUserFun + 156
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbDevResetCallback_1
        MOVS     R0,#+0
        LDR      R1,??UsbDevResetCallback_0  ;; UsbUserFun + 156
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
??UsbDevResetCallback_1:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??UsbDevResetCallback_0:
        DC32     UsbUserFun + 156
        CFI EndBlock cfiBlock33
//  142 
//  143 /*************************************************************************
//  144  * Function Name: EpCtrlSet_EA
//  145  * Parameters: volatile pInt32U pReg, Int32U Data
//  146  *
//  147  * Return: none
//  148  *
//  149  * Description: Set EP Addr
//  150  *
//  151  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock34 Using cfiCommon0
        CFI Function EpCtrlSet_EA
        THUMB
//  152 void EpCtrlSet_EA(volatile pInt32U pReg, Int32U Data)
//  153 {
EpCtrlSet_EA:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  154   Data &= 0xF;
        LSLS     R1,R1,#+28       ;; ZeroExtS R1,R1,#+28,#+28
        LSRS     R1,R1,#+28
//  155   Data |= *pReg & 0x0700 ;
        MOVS     R2,R1
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+224
        LSLS     R1,R1,#+3        ;; #+1792
        ANDS     R1,R1,R0
        ORRS     R1,R1,R2
//  156   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_EA_0  ;; 0x8080
        ORRS     R1,R1,R0
//  157   *pReg = Data;
        LDR      R0,[SP, #+4]
        STR      R1,[R0, #+0]
//  158 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        DATA
??EpCtrlSet_EA_0:
        DC32     0x8080
        CFI EndBlock cfiBlock34
//  159 
//  160 /*************************************************************************
//  161  * Function Name: EpCtrlGet_EA
//  162  * Parameters: volatile pInt32U pReg
//  163  *
//  164  * Return: Int16U
//  165  *
//  166  * Description: Get EP Addr
//  167  *
//  168  *************************************************************************/
//  169 inline
//  170 Int16U EpCtrlGet_EA(volatile pInt32U pReg)
//  171 {
//  172   return(*pReg & 0x0F);
//  173 }
//  174 
//  175 /*************************************************************************
//  176  * Function Name: EpCtrlSet_STAT_TX
//  177  * Parameters: volatile pInt32U pReg, EpState_t Data
//  178  *
//  179  * Return: none
//  180  *
//  181  * Description: Set EP IN State
//  182  *
//  183  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock35 Using cfiCommon0
        CFI Function EpCtrlSet_STAT_TX
        THUMB
//  184 void EpCtrlSet_STAT_TX (volatile pInt32U pReg, EpState_t Data)
//  185 {
EpCtrlSet_STAT_TX:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R1
//  186 Int32U State;
//  187   // The EP can change state between read and write
//  188   // operations from VALID to NAK and result of
//  189   // set operation will be invalid!
//  190   if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
        LDR      R0,[SP, #+0]
        BL       EpCtrlGet_STAT_TX
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R0,R4
        BEQ      ??EpCtrlSet_STAT_TX_0
//  191   {
//  192     State = Data;
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R5,R4
//  193     State <<= 4;
        LSLS     R5,R5,#+4
//  194     State ^= *pReg;
        MOVS     R1,R5
        LDR      R0,[SP, #+0]
        LDR      R5,[R0, #+0]
        EORS     R5,R5,R1
//  195     State |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_TX_1  ;; 0x8080
        ORRS     R5,R5,R0
//  196     State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_TX_1+0x4  ;; 0xffff8fbf
        ANDS     R5,R5,R0
//  197     *pReg = State;
        LDR      R0,[SP, #+0]
        STR      R5,[R0, #+0]
//  198   }
//  199   else
//  200   {
//  201     return;
//  202   }
//  203   if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
        LDR      R0,[SP, #+0]
        BL       EpCtrlGet_STAT_TX
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R0,R4
        BEQ      ??EpCtrlSet_STAT_TX_0
//  204   {
//  205     State = Data;
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R5,R4
//  206     State <<= 4;
        LSLS     R5,R5,#+4
//  207     State ^= *pReg;
        MOVS     R1,R5
        LDR      R0,[SP, #+0]
        LDR      R5,[R0, #+0]
        EORS     R5,R5,R1
//  208     State |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_TX_1  ;; 0x8080
        ORRS     R5,R5,R0
//  209     State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_TX_1+0x4  ;; 0xffff8fbf
        ANDS     R5,R5,R0
//  210     *pReg = State;
        LDR      R0,[SP, #+0]
        STR      R5,[R0, #+0]
??EpCtrlSet_STAT_TX_0:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??EpCtrlSet_STAT_TX_1:
        DC32     0x8080
        DC32     0xffff8fbf
        CFI EndBlock cfiBlock35
//  211   }
//  212 }
//  213 
//  214 /*************************************************************************
//  215  * Function Name: EpCtrlGet_STAT_TX
//  216  * Parameters: volatile pInt32U pReg
//  217  *
//  218  * Return: Int32U
//  219  *
//  220  * Description: Get EP IN Status
//  221  *
//  222  *************************************************************************/
//  223 inline
//  224 Int32U EpCtrlGet_STAT_TX (volatile pInt32U pReg)
//  225 {
//  226 Int32U Data = *pReg;
//  227   return((Data & (0x3UL << 4)) >> 4);
//  228 }
//  229 
//  230 /*************************************************************************
//  231  * Function Name: EpCtrlSet_DTOG_TX
//  232  * Parameters: volatile pInt32U pReg, Int32U Data
//  233  *
//  234  * Return: none
//  235  *
//  236  * Description: Set EP DTOG_TX bit (IN)
//  237  *
//  238  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock36 Using cfiCommon0
        CFI Function EpCtrlSet_DTOG_TX
        THUMB
//  239 void EpCtrlSet_DTOG_TX(volatile pInt32U pReg, Int32U Data)
//  240 {
EpCtrlSet_DTOG_TX:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  241   Data  = Data?(*pReg ^ (1UL<<6)):*pReg;
        CMP      R1,#+0
        BEQ      ??EpCtrlSet_DTOG_TX_0
        LDR      R0,[SP, #+0]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+64
        EORS     R1,R1,R0
        B        ??EpCtrlSet_DTOG_TX_1
??EpCtrlSet_DTOG_TX_0:
        LDR      R0,[SP, #+0]
        LDR      R1,[R0, #+0]
//  242   Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
??EpCtrlSet_DTOG_TX_1:
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_DTOG_TX_2  ;; 0xffff8fcf
        ANDS     R1,R1,R0
//  243   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_DTOG_TX_2+0x4  ;; 0x8080
        ORRS     R1,R1,R0
//  244   *pReg = Data;
        LDR      R0,[SP, #+0]
        STR      R1,[R0, #+0]
//  245 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??EpCtrlSet_DTOG_TX_2:
        DC32     0xffff8fcf
        DC32     0x8080
        CFI EndBlock cfiBlock36
//  246 
//  247 /*************************************************************************
//  248  * Function Name: EpCtrlToggle_DTOG_TX
//  249  * Parameters: volatile pInt32U pReg
//  250  *
//  251  * Return: none
//  252  *
//  253  * Description: Toggle EP DTOG_TX bit (IN)
//  254  *
//  255  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock37 Using cfiCommon0
        CFI Function EpCtrlToggle_DTOG_TX
        THUMB
//  256 void EpCtrlToggle_DTOG_TX(volatile pInt32U pReg)
//  257 {
EpCtrlToggle_DTOG_TX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  258 Int32U Data = *pReg | (1UL << 6);
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R2,#+64
        ORRS     R2,R2,R1
        MOVS     R0,R2
//  259   Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
        MOVS     R1,R0
        LDR      R0,??EpCtrlToggle_DTOG_TX_0  ;; 0xffff8fcf
        ANDS     R0,R0,R1
//  260   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R1,R0
        LDR      R0,??EpCtrlToggle_DTOG_TX_0+0x4  ;; 0x8080
        ORRS     R0,R0,R1
//  261   *pReg = Data;
        LDR      R1,[SP, #+4]
        STR      R0,[R1, #+0]
//  262 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        Nop      
        DATA
??EpCtrlToggle_DTOG_TX_0:
        DC32     0xffff8fcf
        DC32     0x8080
        CFI EndBlock cfiBlock37
//  263 
//  264 /*************************************************************************
//  265  * Function Name: EpCtrlGet_DTOG_TX
//  266  * Parameters: volatile pInt32U pReg
//  267  *
//  268  * Return: Int32U
//  269  *
//  270  * Description: Get EP DTOG_TX bit (IN)
//  271  *
//  272  *************************************************************************/
//  273 inline
//  274 Int32U EpCtrlGet_DTOG_TX(volatile pInt32U pReg)
//  275 {
//  276   return(((*pReg & (1UL<<6))?1:0));
//  277 }
//  278 
//  279 /*************************************************************************
//  280  * Function Name: EpCtrlClr_CTR_TX
//  281  * Parameters: volatile pInt32U pReg
//  282  *
//  283  * Return: none
//  284  *
//  285  * Description: Clear EP CTR_TX bit (IN)
//  286  *
//  287  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock38 Using cfiCommon0
        CFI Function EpCtrlClr_CTR_TX
        THUMB
//  288 void EpCtrlClr_CTR_TX(volatile pInt32U pReg)
//  289 {
EpCtrlClr_CTR_TX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  290 Int32U Data = *pReg;
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R0,R1
//  291   Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 7);
        MOVS     R1,R0
        LDR      R0,??EpCtrlClr_CTR_TX_0  ;; 0xffff8f0f
        ANDS     R0,R0,R1
//  292   Data |= 1UL <<15;   // Set RX_CTR
        MOVS     R1,R0
        MOVS     R0,#+128
        LSLS     R0,R0,#+8        ;; #+32768
        ORRS     R0,R0,R1
//  293   *pReg = Data;
        LDR      R1,[SP, #+4]
        STR      R0,[R1, #+0]
//  294 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        DATA
??EpCtrlClr_CTR_TX_0:
        DC32     0xffff8f0f
        CFI EndBlock cfiBlock38
//  295 
//  296 /*************************************************************************
//  297  * Function Name: EpCtrlGet_CTR_TX
//  298  * Parameters: volatile pInt32U pReg
//  299  *
//  300  * Return: UserFunc_t
//  301  *
//  302  * Description: Get EP CTR_TX bit (IN)
//  303  *
//  304  *************************************************************************/
//  305 inline
//  306 Int32U EpCtrlGet_CTR_TX(volatile pInt32U pReg)
//  307 {
//  308   return(((*pReg & (0x1UL << 7))?1:0));
//  309 }
//  310 
//  311 /*************************************************************************
//  312  * Function Name: EpCtrlSet_EP_KIND
//  313  * Parameters: volatile pInt32U pReg, Int32U Data
//  314  *
//  315  * Return: none
//  316  *
//  317  * Description: Set EP KIND bit
//  318  *
//  319  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock39 Using cfiCommon0
        CFI Function EpCtrlSet_EP_KIND
        THUMB
//  320 void EpCtrlSet_EP_KIND (volatile pInt32U pReg, Int32U Data)
//  321 {
EpCtrlSet_EP_KIND:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  322   Data  = Data ? (1UL << 8):0;
        CMP      R1,#+0
        BEQ      ??EpCtrlSet_EP_KIND_0
        MOVS     R1,#+128
        LSLS     R1,R1,#+1        ;; #+256
        B        ??EpCtrlSet_EP_KIND_1
??EpCtrlSet_EP_KIND_0:
        MOVS     R1,#+0
//  323   Data |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (1UL << 8));
??EpCtrlSet_EP_KIND_1:
        MOVS     R2,R1
        LDR      R0,[SP, #+0]
        LDR      R0,[R0, #+0]
        LDR      R1,??EpCtrlSet_EP_KIND_2  ;; 0xffff8e8f
        ANDS     R1,R1,R0
        ORRS     R1,R1,R2
//  324   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_EP_KIND_2+0x4  ;; 0x8080
        ORRS     R1,R1,R0
//  325   *pReg = Data;
        LDR      R0,[SP, #+0]
        STR      R1,[R0, #+0]
//  326 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??EpCtrlSet_EP_KIND_2:
        DC32     0xffff8e8f
        DC32     0x8080
        CFI EndBlock cfiBlock39
//  327 
//  328 /*************************************************************************
//  329  * Function Name: EpCtrlGet_EP_KIND
//  330  * Parameters: volatile pInt32U pReg
//  331  *
//  332  * Return: Int32U
//  333  *
//  334  * Description: Get EP KIND bit
//  335  *
//  336  *************************************************************************/
//  337 inline
//  338 Int32U EpCtrlGet_EP_KIND(volatile pInt32U pReg)
//  339 {
//  340   return(*pReg & (0x1UL << 8)?1:0);
//  341 }
//  342 
//  343 /*************************************************************************
//  344  * Function Name: EpCtrlSet_EP_TYPE
//  345  * Parameters: volatile pInt32U pReg, EpType_t Data
//  346  *
//  347  * Return: none
//  348  *
//  349  * Description: Set EP type
//  350  *
//  351  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock40 Using cfiCommon0
        CFI Function EpCtrlSet_EP_TYPE
        THUMB
//  352 void EpCtrlSet_EP_TYPE(volatile pInt32U pReg, EpType_t Data)
//  353 {
EpCtrlSet_EP_TYPE:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  354 Int32U Type;
//  355   Type = Data;
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R0,R1
//  356   Type <<= 9;
        LSLS     R0,R0,#+9
//  357   Type  |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (0x3UL << 9));
        MOVS     R3,R0
        LDR      R0,[SP, #+4]
        LDR      R2,[R0, #+0]
        LDR      R0,??EpCtrlSet_EP_TYPE_0  ;; 0xffff898f
        ANDS     R0,R0,R2
        ORRS     R0,R0,R3
//  358   Type  |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R2,R0
        LDR      R0,??EpCtrlSet_EP_TYPE_0+0x4  ;; 0x8080
        ORRS     R0,R0,R2
//  359   *pReg = Type;
        LDR      R2,[SP, #+4]
        STR      R0,[R2, #+0]
//  360 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        Nop      
        DATA
??EpCtrlSet_EP_TYPE_0:
        DC32     0xffff898f
        DC32     0x8080
        CFI EndBlock cfiBlock40
//  361 
//  362 /*************************************************************************
//  363  * Function Name: EpCtrlGet_EP_TYPE
//  364  * Parameters: volatile pInt32U pReg
//  365  *
//  366  * Return: Int32U
//  367  *
//  368  * Description: Get EP type
//  369  *
//  370  *************************************************************************/
//  371 inline
//  372 Int32U EpCtrlGet_EP_TYPE(volatile pInt32U pReg)
//  373 {
//  374 Int32U Data = *pReg;
//  375   Data &= (3UL<<9);
//  376   return(Data >> 9);
//  377 }
//  378 
//  379 /*************************************************************************
//  380  * Function Name: EpCtrlGet_SETUP
//  381  * Parameters: volatile pInt32U pReg
//  382  *
//  383  * Return: Int32U
//  384  *
//  385  * Description: Get EP SETUP bit
//  386  *
//  387  *************************************************************************/
//  388 inline
//  389 Int32U EpCtrlGet_SETUP(volatile pInt32U pReg)
//  390 {
//  391 Int32U Data = *pReg & (0x1UL << 11);
//  392   return(Data?1:0);
//  393 }
//  394 
//  395 /*************************************************************************
//  396  * Function Name: EpCtrlSet_STAT_RX
//  397  * Parameters: volatile pInt32U pReg, EpState_t Data
//  398  *
//  399  * Return: none
//  400  *
//  401  * Description: Set EP STAT_RX (OUT)
//  402  *
//  403  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock41 Using cfiCommon0
        CFI Function EpCtrlSet_STAT_RX
        THUMB
//  404 void EpCtrlSet_STAT_RX (volatile pInt32U pReg, EpState_t Data)
//  405 {
EpCtrlSet_STAT_RX:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R1
//  406 Int32U State;
//  407   // The EP can change state between read and write
//  408   // operations from VALID to NAK and result of
//  409   // set operation will be invalid!
//  410   if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
        LDR      R0,[SP, #+0]
        BL       EpCtrlGet_STAT_RX
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R0,R4
        BEQ      ??EpCtrlSet_STAT_RX_0
//  411   {
//  412     State = Data;
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R5,R4
//  413     State <<= 12;
        LSLS     R5,R5,#+12
//  414     State  ^= *pReg;
        MOVS     R1,R5
        LDR      R0,[SP, #+0]
        LDR      R5,[R0, #+0]
        EORS     R5,R5,R1
//  415     State  |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_RX_1  ;; 0x8080
        ORRS     R5,R5,R0
//  416     State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_RX_1+0x4  ;; 0xffffbf8f
        ANDS     R5,R5,R0
//  417     *pReg  = State;
        LDR      R0,[SP, #+0]
        STR      R5,[R0, #+0]
//  418   }
//  419   else
//  420   {
//  421     return;
//  422   }
//  423   if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
        LDR      R0,[SP, #+0]
        BL       EpCtrlGet_STAT_RX
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R0,R4
        BEQ      ??EpCtrlSet_STAT_RX_0
//  424   {
//  425     State = Data;
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R5,R4
//  426     State <<= 12;
        LSLS     R5,R5,#+12
//  427     State  ^= *pReg;
        MOVS     R1,R5
        LDR      R0,[SP, #+0]
        LDR      R5,[R0, #+0]
        EORS     R5,R5,R1
//  428     State  |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_RX_1  ;; 0x8080
        ORRS     R5,R5,R0
//  429     State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
        MOVS     R0,R5
        LDR      R5,??EpCtrlSet_STAT_RX_1+0x4  ;; 0xffffbf8f
        ANDS     R5,R5,R0
//  430     *pReg  = State;
        LDR      R0,[SP, #+0]
        STR      R5,[R0, #+0]
??EpCtrlSet_STAT_RX_0:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??EpCtrlSet_STAT_RX_1:
        DC32     0x8080
        DC32     0xffffbf8f
        CFI EndBlock cfiBlock41
//  431   }
//  432 }
//  433 
//  434 /*************************************************************************
//  435  * Function Name: EpCtrlGet_STAT_RX
//  436  * Parameters: volatile pInt32U pReg
//  437  *
//  438  * Return: Int32U
//  439  *
//  440  * Description: Get EP STAT_RX (OUT)
//  441  *
//  442  *************************************************************************/
//  443 inline
//  444 Int32U EpCtrlGet_STAT_RX(volatile pInt32U pReg)
//  445 {
//  446 Int32U Data = *pReg &( 0x3UL << 12);
//  447   return(Data >> 12);
//  448 }
//  449 
//  450 /*************************************************************************
//  451  * Function Name: EpCtrlSet_DTOG_RX
//  452  * Parameters: volatile pInt32U pReg, Int32U Data
//  453  *
//  454  * Return: none
//  455  *
//  456  * Description: Set DTOG_RX bit
//  457  *
//  458  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock42 Using cfiCommon0
        CFI Function EpCtrlSet_DTOG_RX
        THUMB
//  459 void EpCtrlSet_DTOG_RX(volatile pInt32U pReg, Int32U Data)
//  460 {
EpCtrlSet_DTOG_RX:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  461   Data  = Data?(*pReg ^ (1UL<<14)):*pReg;
        CMP      R1,#+0
        BEQ      ??EpCtrlSet_DTOG_RX_0
        LDR      R0,[SP, #+0]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+7        ;; #+16384
        EORS     R1,R1,R0
        B        ??EpCtrlSet_DTOG_RX_1
??EpCtrlSet_DTOG_RX_0:
        LDR      R0,[SP, #+0]
        LDR      R1,[R0, #+0]
//  462   Data &= ~0x3070;   // Clear the toggle bits without DTOG_RX (14)
??EpCtrlSet_DTOG_RX_1:
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_DTOG_RX_2  ;; 0xffffcf8f
        ANDS     R1,R1,R0
//  463   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R0,R1
        LDR      R1,??EpCtrlSet_DTOG_RX_2+0x4  ;; 0x8080
        ORRS     R1,R1,R0
//  464   *pReg = Data;
        LDR      R0,[SP, #+0]
        STR      R1,[R0, #+0]
//  465 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??EpCtrlSet_DTOG_RX_2:
        DC32     0xffffcf8f
        DC32     0x8080
        CFI EndBlock cfiBlock42
//  466 
//  467 /*************************************************************************
//  468  * Function Name: EpCtrlToggle_DTOG_RX
//  469  * Parameters: volatile Int16U pReg
//  470  *
//  471  * Return: none
//  472  *
//  473  * Description: Toggle DTOG_RX bit
//  474  *
//  475  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock43 Using cfiCommon0
        CFI Function EpCtrlToggle_DTOG_RX
        THUMB
//  476 void EpCtrlToggle_DTOG_RX(volatile pInt32U pReg)
//  477 {
EpCtrlToggle_DTOG_RX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  478 Int32U Data = *pReg | (1UL << 14);
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+7        ;; #+16384
        ORRS     R2,R2,R1
        MOVS     R0,R2
//  479   Data &= ~0x3070;   // Clear the toggle bits without DTOG_TX (14)
        MOVS     R1,R0
        LDR      R0,??EpCtrlToggle_DTOG_RX_0  ;; 0xffffcf8f
        ANDS     R0,R0,R1
//  480   Data |= USB_CTRL_CLEAR_ONLY_MASK;
        MOVS     R1,R0
        LDR      R0,??EpCtrlToggle_DTOG_RX_0+0x4  ;; 0x8080
        ORRS     R0,R0,R1
//  481   *pReg = Data;
        LDR      R1,[SP, #+4]
        STR      R0,[R1, #+0]
//  482 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        DATA
??EpCtrlToggle_DTOG_RX_0:
        DC32     0xffffcf8f
        DC32     0x8080
        CFI EndBlock cfiBlock43
//  483 
//  484 /*************************************************************************
//  485  * Function Name: EpCtrlGet_DTOG_RX
//  486  * Parameters: volatile pInt32U pReg
//  487  *
//  488  * Return: Int32U
//  489  *
//  490  * Description: Get EP DTOG_RX bit (OUT)
//  491  *
//  492  *************************************************************************/
//  493 inline
//  494 Int32U EpCtrlGet_DTOG_RX(volatile pInt32U pReg)
//  495 {
//  496   return(((*pReg & (1UL<<14))?1:0));
//  497 }
//  498 
//  499 /*************************************************************************
//  500  * Function Name: EpCtrlClr_CTR_RX
//  501  * Parameters: volatile pInt32U pReg
//  502  *
//  503  * Return: none
//  504  *
//  505  * Description: Clear EP CTR_RX bit (OUT)
//  506  *
//  507  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock44 Using cfiCommon0
        CFI Function EpCtrlClr_CTR_RX
        THUMB
//  508 void EpCtrlClr_CTR_RX(volatile pInt32U pReg)
//  509 {
EpCtrlClr_CTR_RX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
//  510 Int32U Data = *pReg;
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R0,R1
//  511   Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 15);
        MOVS     R1,R0
        LDR      R0,??EpCtrlClr_CTR_RX_0  ;; 0xffff0f8f
        ANDS     R0,R0,R1
//  512   Data |= 1UL << 7;   // Set TX_CTR
        MOVS     R1,R0
        MOVS     R0,#+128
        ORRS     R0,R0,R1
//  513   *pReg = Data;
        LDR      R1,[SP, #+4]
        STR      R0,[R1, #+0]
//  514 }
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        Nop      
        DATA
??EpCtrlClr_CTR_RX_0:
        DC32     0xffff0f8f
        CFI EndBlock cfiBlock44
//  515 
//  516 /*************************************************************************
//  517  * Function Name: EpCtrlGet_CTR_RX
//  518  * Parameters: volatile pInt32U pReg
//  519  *
//  520  * Return: Int32U
//  521  *
//  522  * Description: Get EP CTR_RX bit (OUT)
//  523  *
//  524  *************************************************************************/
//  525 inline
//  526 Int32U EpCtrlGet_CTR_RX(volatile pInt32U pReg)
//  527 {
//  528   return(*pReg & (0x1UL << 15)?1:0);
//  529 }
//  530 
//  531 /*************************************************************************
//  532  * Function Name: USB_Init
//  533  * Parameters: Int32U IntrSlotHi, Int32U IntrSlotLo,
//  534  *             CommUserFpnt_t UserCoreConfigure
//  535  *
//  536  * Return: none
//  537  *
//  538  * Description: Init USB
//  539  *
//  540  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock45 Using cfiCommon0
        CFI Function USB_Init
        THUMB
//  541 void USB_Init(Int32U IntrSlotHi, Int32U IntrSlotLo,
//  542               CommUserFpnt_t UserCoreConfigure)
//  543 {
USB_Init:
        PUSH     {R0,R1,R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+24
        MOVS     R4,R0
        MOVS     R5,R1
        MOVS     R6,R2
//  544 EIC_IRQInitTypeDef EIC_IRQInitStructure;
//  545 
//  546   // Init variables
//  547   UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
        LDR      R0,??USB_Init_0  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+2
        STRB     R1,[R0, #+0]
//  548   UsbCoreT9Fun = NULL;
        LDR      R0,??USB_Init_0+0x4  ;; UsbCoreT9Fun
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  549   UserCoreConfigure(NULL);
        MOVS     R0,#+0
        BL       __iar_via_R6
//  550   USB_UserFuncRegistering(UserCoreConfigure,UsbUserConfigure);
        MOVS     R1,#+38
        MOVS     R0,R6
        BL       USB_UserFuncRegistering
//  551   UsbSetDevState(UsbDevStatusUnknow);
        MOVS     R0,#+0
        BL       UsbSetDevState
//  552 
//  553   // Reset USB Engine
//  554   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_USB,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+2        ;; #+512
        BL       MRCC_PeripheralSWResetConfig
//  555   // Init USB Clock
//  556   MRCC_PeripheralClockConfig(MRCC_Peripheral_USB,ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+128
        LSLS     R0,R0,#+2        ;; #+512
        BL       MRCC_PeripheralClockConfig
//  557   // Release reset USB Engine
//  558   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_USB,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+2        ;; #+512
        BL       MRCC_PeripheralSWResetConfig
//  559 
//  560   // Enable 48MHz clock
//  561 #if USB_EXT_CLK > 0
//  562 GPIO_InitTypeDef  GPIO_InitStructure;
//  563   // Enable GPIO clocks
//  564   MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
//  565   // Release GPIO reset
//  566   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_GPIO,DISABLE);
//  567   // Assign the P0.19 for a clock input of the USB engine
//  568   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  569   GPIO_InitStructure.GPIO_Pin = 1UL<<19;
//  570   GPIO_Init(GPIO0,&GPIO_InitStructure);
//  571   MRCC_CKUSBConfig(MRCC_CKUSB_External);
//  572 #else
//  573   MRCC_CKUSBConfig(MRCC_CKUSB_Internal);
        LDR      R0,??USB_Init_0+0x8  ;; 0xffbfffff
        BL       MRCC_CKUSBConfig
//  574 #endif // USB_EXT_CLK > 0
//  575 
//  576   // Force USB Reset &  Disable USB interrupts
//  577   USB_CNTR = 1;
        LDR      R0,??USB_Init_0+0xC  ;; 0xffffa840
        MOVS     R1,#+1
        STRH     R1,[R0, #+0]
//  578 
//  579   // USB interrupt connect to VIC
//  580 #if USB_HIGH_PRIORITY_EVENT > 0
//  581   EIC_IRQInitStructure.EIC_IRQChannel = USB_HP_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+24
        STRB     R1,[R0, #+0]
//  582   EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrSlotHi;
        MOV      R0,SP
        MOVS     R1,R4
        STRB     R1,[R0, #+1]
//  583   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  584   EIC_IRQInit(&EIC_IRQInitStructure);
        MOV      R0,SP
        BL       EIC_IRQInit
//  585 #endif // USB_HIGH_PRIORITY_EVENT > 0
//  586 
//  587   EIC_IRQInitStructure.EIC_IRQChannel = USB_LP_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+23
        STRB     R1,[R0, #+0]
//  588   EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrSlotLo;
        MOV      R0,SP
        MOVS     R1,R5
        STRB     R1,[R0, #+1]
//  589   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  590   EIC_IRQInit(&EIC_IRQInitStructure);
        MOV      R0,SP
        BL       EIC_IRQInit
//  591 
//  592   // Disconnect device
//  593   USB_ConnectRes(FALSE);
        MOVS     R0,#+0
        BL       USB_ConnectRes
//  594   // Init controls endpoints
//  595   USB_HwReset();
        BL       USB_HwReset
//  596   // Clear spurious pending interrupt
//  597   USB_ISTR = 0;
        LDR      R0,??USB_Init_0+0x10  ;; 0xffffa844
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  598   // Enable Device interrupts
//  599   USB_CNTR = bmCTRM | bmRESETM | bmSUSPM | bmWKUPM |
//  600          (USB_SOF_EVENT   ? bmSOFM  : 0) |
//  601          (USB_ERROR_EVENT ? bmERRM  : 0) |
//  602          ( USB_DOVR_EVENT ? bmDOVRM : 0) ;
        LDR      R0,??USB_Init_0+0xC  ;; 0xffffa840
        MOVS     R1,#+156
        LSLS     R1,R1,#+8        ;; #+39936
        STRH     R1,[R0, #+0]
//  603 }
        POP      {R2-R6}
        POP      {R0}
        BX       R0               ;; return
        DATA
??USB_Init_0:
        DC32     UsbEp0Ctrl + 8
        DC32     UsbCoreT9Fun
        DC32     0xffbfffff
        DC32     0xffffa840
        DC32     0xffffa844
        CFI EndBlock cfiBlock45
        REQUIRE _A_USB_CNTR
        REQUIRE _A_USB_ISTR
//  604 
//  605 /*************************************************************************
//  606  * Function Name: USB_HwReset
//  607  * Parameters: none
//  608  *
//  609  * Return: none
//  610  *
//  611  * Description: Reset Usb engine
//  612  *
//  613  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock46 Using cfiCommon0
        CFI Function USB_HwReset
        THUMB
//  614 void USB_HwReset (void)
//  615 {
USB_HwReset:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
//  616 Int32U Count;
//  617 
//  618   for(Count = 0; Count < ENP_MAX_NUMB; Count++)
        MOVS     R0,#+0
        MOVS     R4,R0
??USB_HwReset_0:
        CMP      R4,#+32
        BCS      ??USB_HwReset_1
//  619   {
//  620     EpInfo[Count].pEpCtrl = NULL;
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HwReset_2  ;; EpInfo
        MOVS     R2,#+0
        STR      R2,[R1, R0]
//  621   }
        ADDS     R4,R4,#+1
        B        ??USB_HwReset_0
//  622   // Free entire USB packet memory
//  623   for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
??USB_HwReset_1:
        MOVS     R0,#+0
        MOVS     R4,R0
??USB_HwReset_3:
        CMP      R4,#+16
        BCS      ??USB_HwReset_4
//  624   {
//  625     PacketMemBuff[Count].Size = 0;
        MOVS     R0,#+12
        MULS     R0,R4,R0
        LDR      R1,??USB_HwReset_2+0x4  ;; PacketMemBuff
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STRH     R1,[R0, #+4]
//  626   }
        ADDS     R4,R4,#+1
        B        ??USB_HwReset_3
//  627 
//  628   // Resume delay counter
//  629   DlyCnt = 0;
??USB_HwReset_4:
        LDR      R0,??USB_HwReset_2+0x8  ;; DlyCnt
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  630 
//  631   // Filter disabled
//  632   CFG_USBFilterConfig(CFG_USBFilter_Disable);
        LDR      R0,??USB_HwReset_2+0xC  ;; 0xfffffdff
        BL       CFG_USBFilterConfig
//  633   // Clear USB Reset
//  634   USB_CNTR_bit.FRES   = 0;
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_HwReset_2+0x14  ;; 0xfffe
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        STRH     R1,[R0, #+0]
//  635   // Exit from suspend
//  636   USB_CNTR_bit.ESOFM  = 0;
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_HwReset_2+0x18  ;; 0xfeff
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        STRH     R1,[R0, #+0]
//  637   USB_CNTR_bit.RESUME = 0;
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_HwReset_2+0x1C  ;; 0xffef
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        STRH     R1,[R0, #+0]
//  638   USB_CNTR_bit.LPMODE = 0;
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_HwReset_2+0x20  ;; 0xfffb
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        STRH     R1,[R0, #+0]
//  639   USB_CNTR_bit.FSUSP  = 0;  // Must be 0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_HwReset_2+0x24  ;; 0xfff7
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x10  ;; 0xffffa840
        STRH     R1,[R0, #+0]
//  640   // USB_Configure
//  641   USB_Configure(FALSE);
        MOVS     R0,#+0
        BL       USB_Configure
//  642   // Set Buffer table address
//  643   USB_BTABLE = USB_OffsetOfDBT;
        LDR      R0,??USB_HwReset_2+0x28  ;; SFE(USB_PACKET_MEMORY)
        LDR      R1,??USB_HwReset_2+0x2C  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R1
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        LDR      R1,??USB_HwReset_2+0x30  ;; 0xfff8
        ANDS     R1,R1,R0
        LDR      R0,??USB_HwReset_2+0x34  ;; 0xffffa850
        STRH     R1,[R0, #+0]
//  644   // Init Packet memory table
//  645   pPacketMemUse = NULL;
        LDR      R0,??USB_HwReset_2+0x38  ;; pPacketMemUse
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  646   // Endpoint Init
//  647   USB_UserFuncRegistering((UserFunc_t)UsbCtrlEp,UsbEp0Out);
        MOVS     R1,#+0
        LDR      R0,??USB_HwReset_2+0x3C  ;; UsbCtrlEp
        BL       USB_UserFuncRegistering
//  648   USB_UserFuncRegistering((UserFunc_t)UsbCtrlEp,UsbEp0In);
        MOVS     R1,#+1
        LDR      R0,??USB_HwReset_2+0x3C  ;; UsbCtrlEp
        BL       USB_UserFuncRegistering
//  649   USB_RealizeEp(CTRL_ENP_IN,Ep0MaxSize,EP_CTRL,EP_SLOT0);
        MOVS     R3,#+0
        MOVS     R2,#+1
        MOVS     R1,#+8
        MOVS     R0,#+1
        BL       USB_RealizeEp
//  650   // Set address 0
//  651   USB_SetDefAdd();
        MOVS     R0,#+0
        BL       USB_SetAdd
//  652 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??USB_HwReset_2:
        DC32     EpInfo
        DC32     PacketMemBuff
        DC32     DlyCnt
        DC32     0xfffffdff
        DC32     0xffffa840
        DC32     0xfffe
        DC32     0xfeff
        DC32     0xffef
        DC32     0xfffb
        DC32     0xfff7
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     0xfff8
        DC32     0xffffa850
        DC32     pPacketMemUse
        DC32     UsbCtrlEp
        CFI EndBlock cfiBlock46
        REQUIRE _A_USB_CNTR
        REQUIRE USB_BTABLE
//  653 
//  654 /*************************************************************************
//  655  * Function Name: UsbMalloc
//  656  * Parameters: none
//  657  *
//  658  * Return: void *
//  659  *
//  660  * Description: Return address of free element of the PacketMemBuff massive
//  661  *
//  662  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock47 Using cfiCommon0
        CFI Function UsbMalloc
        THUMB
//  663 static
//  664 void * UsbMalloc(void)
//  665 {
UsbMalloc:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  666 Int32U Count;
//  667   for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
        MOVS     R0,#+0
        MOVS     R1,R0
??UsbMalloc_0:
        CMP      R1,#+16
        BCS      ??UsbMalloc_1
//  668   {
//  669     if (PacketMemBuff[Count].Size == 0)
        MOVS     R0,#+12
        MULS     R0,R1,R0
        LDR      R2,??UsbMalloc_2  ;; PacketMemBuff
        ADDS     R0,R2,R0
        LDRH     R0,[R0, #+4]
        CMP      R0,#+0
        BNE      ??UsbMalloc_3
//  670     {
//  671       return(&PacketMemBuff[Count]);
        MOVS     R0,#+12
        MULS     R1,R0,R1
        LDR      R0,??UsbMalloc_2  ;; PacketMemBuff
        ADDS     R0,R0,R1
        B        ??UsbMalloc_4
//  672     }
//  673   }
??UsbMalloc_3:
        ADDS     R1,R1,#+1
        B        ??UsbMalloc_0
//  674   return(NULL);
??UsbMalloc_1:
        MOVS     R0,#+0
??UsbMalloc_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??UsbMalloc_2:
        DC32     PacketMemBuff
        CFI EndBlock cfiBlock47
//  675 }
//  676 
//  677 /*************************************************************************
//  678  * Function Name: UsbFree
//  679  * Parameters: pPacketMemUse_t pPntr
//  680  *
//  681  * Return: none
//  682  *
//  683  * Description: Free element of the PacketMemBuff massive
//  684  *
//  685  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock48 Using cfiCommon0
        CFI Function UsbFree
        THUMB
//  686 inline static
//  687 void UsbFree(pPacketMemUse_t pPntr)
//  688 {
//  689   pPntr->Size = 0;
UsbFree:
        MOVS     R1,#+0
        STRH     R1,[R0, #+4]
//  690 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock48
//  691 
//  692 /*************************************************************************
//  693  * Function Name: USB_AllocateBuffer
//  694  * Parameters: pInt16U pOffset,Int32U PacketSize,
//  695  *             USB_Endpoint_t EndPoint
//  696  *
//  697  * Return: Boolean
//  698  *
//  699  * Description: Allocate a new buffer
//  700  *
//  701  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock49 Using cfiCommon0
        CFI Function USB_AllocateBuffer
        THUMB
//  702 static
//  703 Boolean USB_AllocateBuffer (pInt16U pOffset,pInt32U pPacketSize,
//  704                             USB_Endpoint_t EndPoint)
//  705 {
USB_AllocateBuffer:
        PUSH     {R0-R2,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+32
        MOVS     R4,R0
//  706 pPacketMemUse_t  pPacketMem = pPacketMemUse, pPacketMemNext, pPacketMemUseNew;
        LDR      R0,??USB_AllocateBuffer_0  ;; pPacketMemUse
        LDR      R0,[R0, #+0]
        MOVS     R7,R0
//  707 //Int32U MaxPacketSize = *pPacketSize+4;  // for CRC
//  708 Int32U MaxPacketSize = *pPacketSize;
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R6,R0
//  709   if(MaxPacketSize > 62)
        CMP      R6,#+63
        BCC      ??USB_AllocateBuffer_1
//  710   {
//  711     // coarse-granularity size alignment by 32
//  712     MaxPacketSize +=  (32-1);
        ADDS     R6,R6,#+31
//  713     MaxPacketSize &= ~(32-1);
        MOVS     R0,#+31
        BICS     R6,R6,R0
        B        ??USB_AllocateBuffer_2
//  714   }
//  715   else
//  716   {
//  717     // fine-granularity size alignment by 2
//  718     MaxPacketSize +=  (2-1);
??USB_AllocateBuffer_1:
        ADDS     R6,R6,#+1
//  719     MaxPacketSize &= ~(2-1);
        MOVS     R0,#+1
        BICS     R6,R6,R0
//  720   }
//  721   *pOffset = 0;
??USB_AllocateBuffer_2:
        MOVS     R0,#+0
        STRH     R0,[R4, #+0]
//  722   // Finding free memory block from the USB packet memory
//  723   while(pPacketMem != NULL)
??USB_AllocateBuffer_3:
        CMP      R7,#+0
        BEQ      ??USB_AllocateBuffer_4
//  724   {
//  725     *pOffset = pPacketMem->Start + pPacketMem->Size;
        LDRH     R0,[R7, #+2]
        LDRH     R1,[R7, #+4]
        ADDS     R0,R0,R1
        STRH     R0,[R4, #+0]
//  726     // Offset alignment by 4
//  727     *pOffset += (4-1);
        LDRH     R0,[R4, #+0]
        ADDS     R0,R0,#+3
        STRH     R0,[R4, #+0]
//  728     *pOffset &= ~(4-1);
        LDRH     R0,[R4, #+0]
        LDR      R1,??USB_AllocateBuffer_0+0x4  ;; 0xfffc
        ANDS     R1,R1,R0
        STRH     R1,[R4, #+0]
//  729     pPacketMemNext = pPacketMem->pNext;
        LDR      R0,[R7, #+8]
        STR      R0,[SP, #+0]
//  730     if((pPacketMem->pNext == NULL) ||
//  731        (pPacketMemNext->Start >= *pOffset + MaxPacketSize))
        LDR      R0,[R7, #+8]
        CMP      R0,#+0
        BEQ      ??USB_AllocateBuffer_4
        LDR      R0,[SP, #+0]
        LDRH     R0,[R0, #+2]
        LDRH     R1,[R4, #+0]
        ADDS     R1,R1,R6
        CMP      R0,R1
        BCS      ??USB_AllocateBuffer_4
//  732     {
//  733       break;
//  734     }
//  735     pPacketMem = pPacketMem->pNext;
        LDR      R7,[R7, #+8]
        B        ??USB_AllocateBuffer_3
//  736   }
//  737   // Is block find?
//  738   if((*pOffset + MaxPacketSize) >=
//  739      (((Int32U)__segment_size("USB_PACKET_MEMORY") -
//  740        sizeof(USB_BuffDeskTbl_t) * (MaxIndOfRealizeEp+1))&~7))
??USB_AllocateBuffer_4:
        LDRH     R0,[R4, #+0]
        ADDS     R0,R0,R6
        LDR      R1,??USB_AllocateBuffer_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_AllocateBuffer_0+0xC  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        CMP      R0,R1
        BCC      ??USB_AllocateBuffer_5
//  741   {
//  742     // Not enough space in the USB packet memory
//  743     return(FALSE);
        MOVS     R0,#+0
        B        ??USB_AllocateBuffer_6
//  744   }
//  745 
//  746   pPacketMemUseNew = UsbMalloc();
??USB_AllocateBuffer_5:
        BL       UsbMalloc
        MOVS     R5,R0
//  747   if(pPacketMemUseNew == NULL)
        CMP      R5,#+0
        BNE      ??USB_AllocateBuffer_7
//  748   {
//  749     // too many point are realized
//  750     return(FALSE);
        MOVS     R0,#+0
        B        ??USB_AllocateBuffer_6
//  751   }
//  752   if(pPacketMemUse == NULL)
??USB_AllocateBuffer_7:
        LDR      R0,??USB_AllocateBuffer_0  ;; pPacketMemUse
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??USB_AllocateBuffer_8
//  753   {
//  754     pPacketMemUse = pPacketMemUseNew;
        LDR      R0,??USB_AllocateBuffer_0  ;; pPacketMemUse
        STR      R5,[R0, #+0]
//  755     pPacketMemUse->pNext = NULL;
        LDR      R0,??USB_AllocateBuffer_0  ;; pPacketMemUse
        LDR      R0,[R0, #+0]
        MOVS     R1,#+0
        STR      R1,[R0, #+8]
        B        ??USB_AllocateBuffer_9
//  756   }
//  757   else
//  758   {
//  759     pPacketMemUseNew->pNext  = pPacketMem->pNext;
??USB_AllocateBuffer_8:
        LDR      R0,[R7, #+8]
        STR      R0,[R5, #+8]
//  760     pPacketMem->pNext        = pPacketMemUseNew;
        STR      R5,[R7, #+8]
//  761   }
//  762   pPacketMemUseNew->RpAddr = EndPoint;
??USB_AllocateBuffer_9:
        MOV      R0,SP
        LDRB     R0,[R0, #+8]
        STRB     R0,[R5, #+0]
//  763   pPacketMemUseNew->Start  = *pOffset;
        LDRH     R0,[R4, #+0]
        STRH     R0,[R5, #+2]
//  764   pPacketMemUseNew->Size   = MaxPacketSize;
        MOVS     R0,R6
        STRH     R0,[R5, #+4]
//  765   *pPacketSize = MaxPacketSize;
        LDR      R0,[SP, #+4]
        STR      R6,[R0, #+0]
//  766   return(TRUE);
        MOVS     R0,#+1
??USB_AllocateBuffer_6:
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??USB_AllocateBuffer_0:
        DC32     pPacketMemUse
        DC32     0xfffc
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     SFB(USB_PACKET_MEMORY)
        CFI EndBlock cfiBlock49
//  767 }
//  768 
//  769 /*************************************************************************
//  770  * Function Name: USB_ReleaseBuffer
//  771  * Parameters: USB_Endpoint_t EndPoint
//  772  *
//  773  * Return: none
//  774  *
//  775  * Description: Release the buffer/s
//  776  *
//  777  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock50 Using cfiCommon0
        CFI Function USB_ReleaseBuffer
        THUMB
//  778 static
//  779 void USB_ReleaseBuffer (USB_Endpoint_t EndPoint)
//  780 {
USB_ReleaseBuffer:
        PUSH     {R4-R6,LR}
        CFI R14 Frame(CFA, -4)
        CFI R6 Frame(CFA, -8)
        CFI R5 Frame(CFA, -12)
        CFI R4 Frame(CFA, -16)
        CFI CFA R13+16
        MOVS     R4,R0
//  781 pPacketMemUse_t  pPacketMem, pPacketMemPrev;
//  782   pPacketMem = pPacketMemUse;
        LDR      R0,??USB_ReleaseBuffer_0  ;; pPacketMemUse
        LDR      R0,[R0, #+0]
        MOVS     R5,R0
//  783   while(pPacketMem != NULL)
??USB_ReleaseBuffer_1:
        CMP      R5,#+0
        BEQ      ??USB_ReleaseBuffer_2
//  784   {
//  785     if(pPacketMem->RpAddr == EndPoint)
        LDRB     R0,[R5, #+0]
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R0,R4
        BNE      ??USB_ReleaseBuffer_3
//  786     {
//  787       pPacketMemPrev->pNext = pPacketMem->pNext;
        LDR      R0,[R5, #+8]
        STR      R0,[R6, #+8]
//  788       UsbFree(pPacketMem);
        MOVS     R0,R5
        BL       UsbFree
//  789       pPacketMem = pPacketMemPrev->pNext;
        LDR      R0,[R6, #+8]
        MOVS     R5,R0
        B        ??USB_ReleaseBuffer_4
//  790     }
//  791     else
//  792     {
//  793       pPacketMemPrev = pPacketMem;
??USB_ReleaseBuffer_3:
        MOVS     R6,R5
//  794       pPacketMem = pPacketMem->pNext;
        LDR      R5,[R5, #+8]
//  795     }
//  796     if (pPacketMem == NULL)
??USB_ReleaseBuffer_4:
        CMP      R5,#+0
        BNE      ??USB_ReleaseBuffer_1
//  797     {
//  798       break;
//  799     }
//  800   }
//  801 }
??USB_ReleaseBuffer_2:
        POP      {R4-R6}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??USB_ReleaseBuffer_0:
        DC32     pPacketMemUse
        CFI EndBlock cfiBlock50
//  802 
//  803 /*************************************************************************
//  804  * Function Name: USB_RealizeEp
//  805  * Parameters: USB_Endpoint_t EndPoint, Int32U MaxPacketSize,
//  806  *             EpType_t EpKind, EpSlot_t EpSlot,
//  807  *
//  808  * Return: USB_ErrorCodes_t
//  809  *
//  810  * Description: Enable or disable endpoint
//  811  *
//  812  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock51 Using cfiCommon0
        CFI Function USB_RealizeEp
        THUMB
//  813 USB_ErrorCodes_t USB_RealizeEp( USB_Endpoint_t EndPoint, Int32U MaxPacketSize,
//  814                                 EpType_t EpType, EpSlot_t EpSlot)
//  815 {
USB_RealizeEp:
        PUSH     {R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+20
        SUB      SP,SP,#+20
        CFI CFA R13+40
        MOVS     R7,R0
        MOVS     R4,R1
        MOVS     R5,R2
        MOVS     R6,R3
//  816 volatile pInt32U pEpCtrlBaseAddr;
//  817 Int16U Offset,Offset1;
//  818 Int32U MaxPacketSizeTmp;
//  819   if (MaxPacketSize)
        CMP      R4,#+0
        BNE      .+4
        B        ??USB_RealizeEp_0
//  820   {
//  821     // Allocate packet memory for EP buffer/s
//  822     // calculate actual size only for the OUT EPs
//  823     MaxPacketSizeTmp = MaxPacketSize;
        STR      R4,[SP, #+12]
//  824     if(!USB_AllocateBuffer(&Offset,&MaxPacketSizeTmp,EndPoint))
        MOVS     R2,R7
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        ADD      R1,SP,#+12
        ADD      R0,SP,#+4
        ADDS     R0,R0,#+2
        BL       USB_AllocateBuffer
        CMP      R0,#+0
        BNE      ??USB_RealizeEp_1
//  825     {
//  826       return(USB_MEMORY_FULL);
        MOVS     R0,#+4
        B        ??USB_RealizeEp_2
//  827     }
//  828     if((EpType == EP_ISO) || (EpType == EP_BULK_DOUB_BUF) || (EpType == EP_CTRL))
??USB_RealizeEp_1:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+2
        BEQ      ??USB_RealizeEp_3
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+4
        BEQ      ??USB_RealizeEp_3
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+1
        BNE      ??USB_RealizeEp_4
//  829     {
//  830       // Allocate packet second buffer
//  831       MaxPacketSizeTmp = MaxPacketSize;
??USB_RealizeEp_3:
        STR      R4,[SP, #+12]
//  832       if(!USB_AllocateBuffer(&Offset1,&MaxPacketSizeTmp,EndPoint))
        MOVS     R2,R7
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        ADD      R1,SP,#+12
        ADD      R0,SP,#+4
        BL       USB_AllocateBuffer
        CMP      R0,#+0
        BNE      ??USB_RealizeEp_4
//  833       {
//  834         // release buffer
//  835         USB_ReleaseBuffer(EndPoint);
        MOVS     R0,R7
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_ReleaseBuffer
//  836         return(USB_MEMORY_FULL);
        MOVS     R0,#+4
        B        ??USB_RealizeEp_2
//  837       }
//  838     }
//  839     pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
??USB_RealizeEp_4:
        LDR      R0,??USB_RealizeEp_5  ;; 0xffffa800
        STR      R0,[SP, #+0]
//  840     pEpCtrlBaseAddr += EpSlot;
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R0,#+4
        MULS     R0,R6,R0
        LDR      R1,[SP, #+0]
        ADDS     R0,R1,R0
        STR      R0,[SP, #+0]
//  841     // Set Ep Address
//  842     EpCtrlSet_EA(pEpCtrlBaseAddr,EndPoint >> 1);
        MOVS     R1,R7
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        LSRS     R1,R1,#+1
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EA
//  843 
//  844     if(EpType == EP_CTRL)
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+1
        BNE      ??USB_RealizeEp_6
//  845     {
//  846       EndPoint |= 1;
        MOVS     R0,R7
        MOVS     R7,#+1
        ORRS     R7,R7,R0
//  847     }
//  848   sec_pass: // only for control EP, because this type realize both directions
//  849     EpInfo[EndPoint].EpType  = EpType;
??USB_RealizeEp_6:
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_5+0x4  ;; EpInfo
        ADDS     R0,R1,R0
        STRB     R5,[R0, #+12]
//  850     EpInfo[EndPoint].pEpCtrl = pEpCtrlBaseAddr;
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_5+0x4  ;; EpInfo
        LDR      R2,[SP, #+0]
        STR      R2,[R1, R0]
//  851     EpInfo[EndPoint].EpSlot  = EpSlot;
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_5+0x4  ;; EpInfo
        ADDS     R0,R1,R0
        STRB     R6,[R0, #+4]
//  852     EpInfo[EndPoint].MaxSize = MaxPacketSize;
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_5+0x4  ;; EpInfo
        ADDS     R0,R1,R0
        STR      R4,[R0, #+8]
//  853     if(EndPoint & 1)
        LSLS     R0,R7,#+31
        BMI      .+4
        B        ??USB_RealizeEp_7
//  854     {
//  855       // IN EP
//  856       // Disable EP
//  857       EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  858       // Clear Tx toggle
//  859       EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_DTOG_TX
//  860       // Clear Correct Transfer for transmission flag
//  861       EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
        LDR      R0,[SP, #+0]
        BL       EpCtrlClr_CTR_TX
//  862       // Update EP description table
//  863       WriteEpDTB_AddrTx(EpSlot,Offset);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+6]
        STRH     R2,[R0, R1]
//  864       WriteEpDTB_CountTx(EpSlot,0);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R1,#+8
        MULS     R1,R6,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        MOVS     R1,#+0
        STRH     R1,[R0, R2]
//  865       // Set EP Kind & enable
//  866       switch(EpType)
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        MOVS     R0,R5
        CMP      R0,#+0
        BEQ      ??USB_RealizeEp_8
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_9
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_10
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_11
        B        ??USB_RealizeEp_12
//  867       {
//  868       case EP_BULK_SING_BUFF:
//  869         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
??USB_RealizeEp_8:
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  870         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  871         // Enable EP
//  872         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_RealizeEp_13
//  873         break;
//  874       case EP_CTRL:
//  875         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
??USB_RealizeEp_9:
        MOVS     R1,#+1
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  876         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  877         // Enable EP
//  878         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  879         // Realize OUT direction
//  880         EndPoint &= ~1;
        MOVS     R0,R7
        MOVS     R7,#+254
        ANDS     R7,R7,R0
//  881         Offset = Offset1;
        MOV      R0,SP
        MOV      R1,SP
        LDRH     R1,[R1, #+4]
        STRH     R1,[R0, #+6]
//  882         goto sec_pass;
        B        ??USB_RealizeEp_6
//  883       case EP_ISO:
//  884         // Disable receiving (only singe direction is possible)
//  885         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
??USB_RealizeEp_10:
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
//  886         WriteEpDTB_AddrRx(EpSlot,Offset1);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+4]
        STRH     R2,[R0, R1]
//  887         WriteEpDTB_CountRx(EpSlot,0);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+6
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R1,#+8
        MULS     R1,R6,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        MOVS     R1,#+0
        STRH     R1,[R0, R2]
//  888         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  889         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  890         // Enable EP
//  891         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_VALID);
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_RealizeEp_13
//  892         break;
//  893       case EP_INTERRUPT:
//  894         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
??USB_RealizeEp_11:
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  895         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  896         // Enable EP
//  897         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_RealizeEp_13
//  898         break;
//  899       default:  // EP_BULK_DOUB_BUF
//  900         // Disable receiving (only singe direction is possible)
//  901         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
??USB_RealizeEp_12:
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
//  902         // All buffers are empties
//  903         EpInfo[EndPoint].EpIntrFl = 1;
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_5+0x4  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+1
        STR      R1,[R0, #+16]
//  904         // Clear Tx Software toggle
//  905         WriteEpDTB_AddrRx(EpSlot,Offset1);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+4]
        STRH     R2,[R0, R1]
//  906         WriteEpDTB_CountRx(EpSlot,0);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+6
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R1,#+8
        MULS     R1,R6,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        MOVS     R1,#+0
        STRH     R1,[R0, R2]
//  907         EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_DTOG_RX
//  908         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  909         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,1);
        MOVS     R1,#+1
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  910         // Enable EP
//  911         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  912       }
//  913       // Clear Correct Transfer for transmission flag
//  914       EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
??USB_RealizeEp_13:
        LDR      R0,[SP, #+0]
        BL       EpCtrlClr_CTR_TX
        B        ??USB_RealizeEp_14
//  915     }
//  916     else
//  917     {
//  918       RxCount_t RxCount = {0};
??USB_RealizeEp_7:
        LDR      R0,??USB_RealizeEp_15  ;; `?<Constant {0}>`
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
//  919       // OUT EP
//  920       // Disable EP
//  921       EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
//  922       // Clear Rx toggle
//  923       EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_DTOG_RX
//  924       // Clear Correct Transfer for reception flag
//  925       EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
        LDR      R0,[SP, #+0]
        BL       EpCtrlClr_CTR_RX
//  926       // Update EP description table
//  927       RxCount.BlSizeField = (MaxPacketSize > 62);
        CMP      R4,#+63
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        MOV      R1,SP
        MOV      R2,SP
        LDRH     R2,[R2, #+8]
        LSLS     R2,R2,#+17       ;; ZeroExtS R2,R2,#+17,#+17
        LSRS     R2,R2,#+17
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSLS     R0,R0,#+15
        ORRS     R0,R0,R2
        STRH     R0,[R1, #+8]
//  928       RxCount.NubBlockField = (MaxPacketSize > 62) ? (MaxPacketSize>>5)-1:MaxPacketSizeTmp>>1;
        CMP      R4,#+63
        BCC      ??USB_RealizeEp_16
        MOVS     R1,R4
        LSRS     R1,R1,#+5
        SUBS     R1,R1,#+1
        B        ??USB_RealizeEp_17
??USB_RealizeEp_16:
        LDR      R0,[SP, #+12]
        MOVS     R1,R0
        LSRS     R1,R1,#+1
??USB_RealizeEp_17:
        MOV      R0,SP
        LDRH     R2,[R0, #+8]
        LDR      R0,??USB_RealizeEp_15+0x4  ;; 0x83ff
        ANDS     R0,R0,R2
        LSLS     R1,R1,#+10
        MOVS     R2,#+248
        LSLS     R2,R2,#+7        ;; #+31744
        ANDS     R2,R2,R1
        ORRS     R2,R2,R0
        MOV      R0,SP
        STRH     R2,[R0, #+8]
//  929       WriteEpDTB_AddrRx(EpSlot,Offset);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+6]
        STRH     R2,[R0, R1]
//  930       WriteEpDTB_CountRx(EpSlot,RxCount.Count);
        LDR      R0,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_5+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_5+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+6
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+8]
        STRH     R2,[R0, R1]
//  931 
//  932       // Set EP Kind & enable
//  933       switch(EpType)
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        MOVS     R0,R5
        CMP      R0,#+0
        BEQ      ??USB_RealizeEp_18
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_19
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_20
        SUBS     R0,R0,#+1
        BEQ      ??USB_RealizeEp_21
        B        ??USB_RealizeEp_22
//  934       {
//  935       case EP_BULK_SING_BUFF:
//  936         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
??USB_RealizeEp_18:
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  937         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  938         // Enable EP
//  939         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_RealizeEp_14
        DATA
??USB_RealizeEp_5:
        DC32     0xffffa800
        DC32     EpInfo
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        THUMB
//  940         break;
//  941       case EP_CTRL:
//  942         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
??USB_RealizeEp_19:
        MOVS     R1,#+1
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  943         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  944         // Enable EP
//  945         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_NAK);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_RealizeEp_14
//  946         break;
//  947       case EP_ISO:
//  948         WriteEpDTB_CountTx(EpSlot,RxCount.Count);
??USB_RealizeEp_20:
        LDR      R0,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_15+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+8]
        STRH     R2,[R0, R1]
//  949         // Disable transmitting (only singe direction is possible)
//  950         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  951         WriteEpDTB_AddrTx(EpSlot,Offset1);
        LDR      R0,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_15+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+4]
        STRH     R2,[R0, R1]
//  952         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        MOVS     R1,#+2
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  953         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  954         // Enable EP
//  955         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_RealizeEp_14
//  956         break;
//  957       case EP_INTERRUPT:
//  958         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
??USB_RealizeEp_21:
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  959         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  960         // Enable EP
//  961         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_RealizeEp_14
//  962         break;
//  963       default:  // EP_BULK_DOUB_BUF
//  964         // All buffers are empties
//  965         EpInfo[EndPoint].EpIntrFl = 0;
??USB_RealizeEp_22:
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_15+0x10  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STR      R1,[R0, #+16]
//  966         // Disable transmitting (only singe direction is possible)
//  967         EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  968         WriteEpDTB_CountTx(EpSlot,RxCount.Count);
        LDR      R0,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_15+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        ADDS     R1,R1,#+2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+8]
        STRH     R2,[R0, R1]
//  969         WriteEpDTB_AddrTx(EpSlot,Offset1);
        LDR      R0,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_RealizeEp_15+0xC  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_RealizeEp_15+0x8  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        ADDS     R1,R1,R2
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R2,#+8
        MULS     R2,R6,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        MOV      R2,SP
        LDRH     R2,[R2, #+4]
        STRH     R2,[R0, R1]
//  970         EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_DTOG_TX
//  971         EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_TYPE
//  972         EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,1);
        MOVS     R1,#+1
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_EP_KIND
//  973         // Enable EP
//  974         EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        MOVS     R1,#+3
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_RealizeEp_14
//  975       }
//  976     }
//  977   }
//  978   else
//  979   {
//  980     pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
??USB_RealizeEp_0:
        LDR      R0,??USB_RealizeEp_15+0x14  ;; 0xffffa800
        STR      R0,[SP, #+0]
//  981     pEpCtrlBaseAddr += EpSlot;
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        MOVS     R0,#+4
        MULS     R0,R6,R0
        LDR      R1,[SP, #+0]
        ADDS     R0,R1,R0
        STR      R0,[SP, #+0]
//  982     if(EndPoint & 1)
        LSLS     R0,R7,#+31
        BPL      ??USB_RealizeEp_23
//  983     {
//  984       // Disable IN EP
//  985       EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_TX
//  986       // Clear Correct Transfer for reception flag
//  987       EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
        LDR      R0,[SP, #+0]
        BL       EpCtrlClr_CTR_TX
        B        ??USB_RealizeEp_24
//  988     }
//  989     else
//  990     {
//  991       // Disable OUT EP
//  992       EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
??USB_RealizeEp_23:
        MOVS     R1,#+0
        LDR      R0,[SP, #+0]
        BL       EpCtrlSet_STAT_RX
//  993       // Clear Correct Transfer for reception flag
//  994       EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
        LDR      R0,[SP, #+0]
        BL       EpCtrlClr_CTR_RX
//  995     }
//  996     // release buffer
//  997     USB_ReleaseBuffer(EndPoint);
??USB_RealizeEp_24:
        MOVS     R0,R7
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       USB_ReleaseBuffer
//  998     EpInfo[EndPoint].pEpCtrl = NULL;
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        MOVS     R0,#+20
        MULS     R0,R7,R0
        LDR      R1,??USB_RealizeEp_15+0x10  ;; EpInfo
        MOVS     R2,#+0
        STR      R2,[R1, R0]
//  999   }
// 1000   return(USB_OK);
??USB_RealizeEp_14:
        MOVS     R0,#+0
??USB_RealizeEp_2:
        ADD      SP,SP,#+20
        CFI CFA R13+20
        POP      {R4-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??USB_RealizeEp_15:
        DC32     `?<Constant {0}>`
        DC32     0x83ff
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        DC32     0xffffa800
        CFI EndBlock cfiBlock51
        REQUIRE _A_USB_EP0R
// 1001 }
// 1002 
// 1003 /*************************************************************************
// 1004  * Function Name: USB_SetAdd
// 1005  * Parameters: Int32U DevAdd - device address between 0 - 127
// 1006  *
// 1007  * Return: none
// 1008  *
// 1009  * Description: Set device address
// 1010  *
// 1011  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock52 Using cfiCommon0
        CFI Function USB_SetAdd
        THUMB
// 1012 void USB_SetAdd(Int32U DevAdd)
// 1013 {
// 1014   USB_DADDR = DevAdd | 0x80;
USB_SetAdd:
        MOVS     R1,R0
        MOVS     R2,#+128
        ORRS     R2,R2,R1
        LDR      R1,??USB_SetAdd_0  ;; 0xffffa84c
        STRH     R2,[R1, #+0]
// 1015 }
        BX       LR               ;; return
        DATA
??USB_SetAdd_0:
        DC32     0xffffa84c
        CFI EndBlock cfiBlock52
        REQUIRE _A_USB_DADDR
// 1016 
// 1017 /*************************************************************************
// 1018  * Function Name: USB_ConnectRes
// 1019  * Parameters: Boolean Conn
// 1020  *
// 1021  * Return: none
// 1022  *
// 1023  * Description: Enable Pull-Up resistor
// 1024  *
// 1025  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock53 Using cfiCommon0
        CFI Function USB_ConnectRes
        THUMB
// 1026 void USB_ConnectRes (Boolean Conn)
// 1027 {
// 1028 }
USB_ConnectRes:
        BX       LR               ;; return
        CFI EndBlock cfiBlock53
// 1029 
// 1030 /*************************************************************************
// 1031  * Function Name: USB_Configure
// 1032  * Parameters: Boolean Configure
// 1033  *
// 1034  * Return: none
// 1035  *
// 1036  * Description: Configure device
// 1037  *  When Configure != 0 enable all Realize Ep
// 1038  *
// 1039  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock54 Using cfiCommon0
        CFI Function USB_Configure
        THUMB
// 1040 static
// 1041 void USB_Configure (Boolean Configure)
// 1042 {
// 1043 }
USB_Configure:
        BX       LR               ;; return
        CFI EndBlock cfiBlock54
// 1044 
// 1045 /*************************************************************************
// 1046  * Function Name: USB_Suspend
// 1047  * Parameters: void
// 1048  *
// 1049  * Return: none
// 1050  *
// 1051  * Description: Suspend the USB engine
// 1052  *
// 1053  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock55 Using cfiCommon0
        CFI Function USB_Suspend
        THUMB
// 1054 static
// 1055 void USB_Suspend(void)
// 1056 {
USB_Suspend:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1057   USB_CNTR_bit.FSUSP  = 1;
        LDR      R0,??USB_Suspend_0  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+8
        ORRS     R1,R1,R0
        LDR      R0,??USB_Suspend_0  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1058   USB_CNTR_bit.LPMODE = 1;
        LDR      R0,??USB_Suspend_0  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+4
        ORRS     R1,R1,R0
        LDR      R0,??USB_Suspend_0  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1059   CFG_USBFilterConfig(CFG_USBFilter_Enable);
        MOVS     R0,#+128
        LSLS     R0,R0,#+2        ;; #+512
        BL       CFG_USBFilterConfig
// 1060 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??USB_Suspend_0:
        DC32     0xffffa840
        CFI EndBlock cfiBlock55
        REQUIRE _A_USB_CNTR
// 1061 
// 1062 #if USB_REMOTE_WAKEUP != 0
// 1063 /*************************************************************************
// 1064  * Function Name: USB_WakeUp
// 1065  * Parameters: none
// 1066  *
// 1067  * Return: none
// 1068  *
// 1069  * Description: Wake up Usb
// 1070  *
// 1071  *************************************************************************/
// 1072 static
// 1073 void USB_WakeUp (void)
// 1074 {
// 1075   USB_Resume(USB_RESUME_SOFT_EVENT);
// 1076 }
// 1077 #endif // USB_REMOTE_WAKEUP != 0
// 1078 
// 1079 /*************************************************************************
// 1080  * Function Name: USB_Resume
// 1081  * Parameters: UsbResumeEvent_t UsbResumeEvent
// 1082  *
// 1083  * Return: none
// 1084  *
// 1085  * Description: USB Resume implement
// 1086  *
// 1087  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock56 Using cfiCommon0
        CFI Function USB_Resume
        THUMB
// 1088 static
// 1089 void USB_Resume (UsbResumeEvent_t UsbResumeEvent)
// 1090 {
USB_Resume:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
// 1091 Int32U LineStates;
// 1092   switch(UsbResumeEvent)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        CMP      R0,#+0
        BEQ      ??USB_Resume_0
        SUBS     R0,R0,#+1
        BEQ      ??USB_Resume_1
        SUBS     R0,R0,#+1
        BEQ      ??USB_Resume_2
        B        ??USB_Resume_3
// 1093   {
// 1094   case USB_RESUME_SOF_EVENT:
// 1095     if(DlyCnt)
??USB_Resume_0:
        LDR      R0,??USB_Resume_4  ;; DlyCnt
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??USB_Resume_3
// 1096     {
// 1097       // Waiting 8ms (not take more than 10ms) before release signal to host
// 1098       if(--DlyCnt == 0)
        LDR      R0,??USB_Resume_4  ;; DlyCnt
        LDR      R0,[R0, #+0]
        SUBS     R0,R0,#+1
        LDR      R1,??USB_Resume_4  ;; DlyCnt
        STR      R0,[R1, #+0]
        CMP      R0,#+0
        BNE      ??USB_Resume_3
// 1099       {
// 1100         USB_CNTR_bit.ESOFM  = 0;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0x8  ;; 0xfeff
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1101         USB_CNTR_bit.RESUME = 0;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0xC  ;; 0xffef
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1102         USB_CNTR_bit.FSUSP  = 0;  // Must be 0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0x10  ;; 0xfff7
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1103         if(UsbUserFun[UsbUserSuspend] != NULL)
        LDR      R0,??USB_Resume_4+0x14  ;; UsbUserFun + 164
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??USB_Resume_3
// 1104         {
// 1105           UsbUserFun[UsbUserSuspend]((void *)0);
        MOVS     R0,#+0
        LDR      R1,??USB_Resume_4+0x14  ;; UsbUserFun + 164
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        B        ??USB_Resume_3
// 1106         }
// 1107       }
// 1108     }
// 1109     break;
// 1110   case USB_RESUME_SOFT_EVENT: // resume by USER
// 1111     CFG_USBFilterConfig(CFG_USBFilter_Disable);
??USB_Resume_1:
        LDR      R0,??USB_Resume_4+0x18  ;; 0xfffffdff
        BL       CFG_USBFilterConfig
// 1112     USB_CNTR_bit.LPMODE = 0;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0x1C  ;; 0xfffb
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1113     DlyCnt = USB_RESUME_DLY;
        LDR      R0,??USB_Resume_4  ;; DlyCnt
        MOVS     R1,#+8
        STR      R1,[R0, #+0]
// 1114     USB_CNTR_bit.RESUME = 1;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+16
        ORRS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1115     USB_CNTR_bit.ESOFM  = 1;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+1        ;; #+256
        ORRS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
        B        ??USB_Resume_3
// 1116     break;
// 1117   case USB_RESUME_WAKE_UP_EVENT:  // resume by HOST
// 1118     LineStates = USB_FNR & 0xC000;
??USB_Resume_2:
        LDR      R0,??USB_Resume_4+0x20  ;; 0xffffa848
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+192
        LSLS     R1,R1,#+8        ;; #+49152
        ANDS     R1,R1,R0
        MOVS     R5,R1
// 1119     if((LineStates == 0x8000) && (LineStates == 0x3000))
        MOVS     R0,#+128
        LSLS     R0,R0,#+8        ;; #+32768
        CMP      R5,R0
        BNE      ??USB_Resume_5
        MOVS     R0,#+192
        LSLS     R0,R0,#+6        ;; #+12288
        CMP      R5,R0
        BNE      ??USB_Resume_5
// 1120     {
// 1121       // noise return to suspend again
// 1122       USB_Suspend();
        BL       USB_Suspend
        B        ??USB_Resume_3
// 1123     }
// 1124     else
// 1125     {
// 1126       // wake-up
// 1127       CFG_USBFilterConfig(CFG_USBFilter_Disable);
??USB_Resume_5:
        LDR      R0,??USB_Resume_4+0x18  ;; 0xfffffdff
        BL       CFG_USBFilterConfig
// 1128       USB_CNTR_bit.LPMODE = 0;
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0x1C  ;; 0xfffb
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1129       USB_CNTR_bit.FSUSP  = 0;  // Must be 0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LDR      R1,??USB_Resume_4+0x10  ;; 0xfff7
        ANDS     R1,R1,R0
        LDR      R0,??USB_Resume_4+0x4  ;; 0xffffa840
        STRH     R1,[R0, #+0]
// 1130     }
// 1131     break;
// 1132   }
// 1133 }
??USB_Resume_3:
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??USB_Resume_4:
        DC32     DlyCnt
        DC32     0xffffa840
        DC32     0xfeff
        DC32     0xffef
        DC32     0xfff7
        DC32     UsbUserFun + 164
        DC32     0xfffffdff
        DC32     0xfffb
        DC32     0xffffa848
        CFI EndBlock cfiBlock56
        REQUIRE _A_USB_CNTR
        REQUIRE _A_USB_FNR
// 1134 
// 1135 /*************************************************************************
// 1136  * Function Name: USB_GetDevStatus
// 1137  * Parameters: USB_DevStatusReqType_t Type
// 1138  *
// 1139  * Return: Boolean
// 1140  *
// 1141  * Description: Return USB device status
// 1142  *
// 1143  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock57 Using cfiCommon0
        CFI Function USB_GetDevStatus
        THUMB
// 1144 Boolean USB_GetDevStatus (USB_DevStatusReqType_t Type)
// 1145 {
USB_GetDevStatus:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
// 1146   switch (Type)
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R0,R1
        CMP      R0,#+0
        BEQ      ??USB_GetDevStatus_0
        SUBS     R0,R0,#+1
        BEQ      ??USB_GetDevStatus_1
        SUBS     R0,R0,#+1
        BEQ      ??USB_GetDevStatus_2
        B        ??USB_GetDevStatus_3
// 1147   {
// 1148   case USB_DevConnectStatus:
// 1149     return(TRUE);
??USB_GetDevStatus_0:
        MOVS     R0,#+1
        B        ??USB_GetDevStatus_4
// 1150   case USB_SuspendStatus:
// 1151     return(USB_CNTR_bit.FSUSP);
??USB_GetDevStatus_1:
        LDR      R0,??USB_GetDevStatus_5  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        MOVS     R2,#+8
        ANDS     R2,R2,R0
        MOVS     R0,R2
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+3
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        B        ??USB_GetDevStatus_4
// 1152   case USB_ResetStatus:
// 1153     return(USB_CNTR_bit.FRES);
??USB_GetDevStatus_2:
        LDR      R0,??USB_GetDevStatus_5  ;; 0xffffa840
        LDRH     R2,[R0, #+0]
        MOVS     R0,#+1
        ANDS     R0,R0,R2
        B        ??USB_GetDevStatus_4
// 1154   }
// 1155   return(FALSE);
??USB_GetDevStatus_3:
        MOVS     R0,#+0
??USB_GetDevStatus_4:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        DATA
??USB_GetDevStatus_5:
        DC32     0xffffa840
        CFI EndBlock cfiBlock57
        REQUIRE _A_USB_CNTR
// 1156 }
// 1157 
// 1158 /*************************************************************************
// 1159  * Function Name: USB_SetStallEP
// 1160  * Parameters: USB_Endpoint_t EndPoint, Boolean Stall
// 1161  *
// 1162  * Return: USB_ErrorCodes_t
// 1163  *
// 1164  * Description: The endpoint stall/unstall
// 1165  *
// 1166  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock58 Using cfiCommon0
        CFI Function USB_SetStallEP
        THUMB
// 1167 USB_ErrorCodes_t USB_SetStallEP (USB_Endpoint_t EndPoint, Boolean Stall)
// 1168 {
USB_SetStallEP:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
// 1169   if( EpInfo[EndPoint].pEpCtrl == NULL)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BNE      ??USB_SetStallEP_1
// 1170   {
// 1171     return(USB_EP_NOT_VALID);
        MOVS     R0,#+6
        B        ??USB_SetStallEP_2
// 1172   }
// 1173 
// 1174   if(Stall)
??USB_SetStallEP_1:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+0
        BEQ      ??USB_SetStallEP_3
// 1175   {
// 1176     if(EndPoint & 1)
        LSLS     R0,R4,#+31
        BPL      ??USB_SetStallEP_4
// 1177     {
// 1178       // IN EP
// 1179       EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_STALL);
        MOVS     R1,#+1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_SetStallEP_5
// 1180     }
// 1181     else
// 1182     {
// 1183       // OUT EP
// 1184       EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_STALL);
??USB_SetStallEP_4:
        MOVS     R1,#+1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_SetStallEP_5
// 1185     }
// 1186   }
// 1187   else if(EndPoint & 1)
??USB_SetStallEP_3:
        LSLS     R0,R4,#+31
        BPL      ??USB_SetStallEP_6
// 1188   {
// 1189     // IN EP
// 1190     // reset Data Toggle bit
// 1191     switch(EpInfo[EndPoint].EpType)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_SetStallEP_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+0
        BEQ      ??USB_SetStallEP_7
        SUBS     R0,R0,#+2
        CMP      R0,#+1
        BLS      ??USB_SetStallEP_7
        SUBS     R0,R0,#+2
        BNE      ??USB_SetStallEP_8
// 1192     {
// 1193     case EP_BULK_DOUB_BUF:
// 1194       EpInfo[EndPoint].EpIntrFl = 1;
??USB_SetStallEP_9:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_SetStallEP_0  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+1
        STR      R1,[R0, #+16]
// 1195       EpCtrlSet_DTOG_RX(EpInfo[EndPoint].pEpCtrl,0);
        MOVS     R1,#+0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_DTOG_RX
// 1196     case EP_INTERRUPT:
// 1197     case EP_ISO:
// 1198     case EP_BULK_SING_BUFF:
// 1199       EpCtrlSet_DTOG_TX(EpInfo[EndPoint].pEpCtrl,0);
??USB_SetStallEP_7:
        MOVS     R1,#+0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_DTOG_TX
// 1200     }
// 1201     EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
??USB_SetStallEP_8:
        MOVS     R1,#+2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_SetStallEP_5
// 1202   }
// 1203   else
// 1204   {
// 1205     // OUT EP
// 1206     // reset Data Toggle bit
// 1207     switch(EpInfo[EndPoint].EpType)
??USB_SetStallEP_6:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_SetStallEP_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+0
        BEQ      ??USB_SetStallEP_10
        SUBS     R0,R0,#+2
        CMP      R0,#+1
        BLS      ??USB_SetStallEP_10
        SUBS     R0,R0,#+2
        BNE      ??USB_SetStallEP_11
// 1208     {
// 1209     case EP_BULK_DOUB_BUF:
// 1210       EpInfo[EndPoint].EpIntrFl = 0;
??USB_SetStallEP_12:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_SetStallEP_0  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STR      R1,[R0, #+16]
// 1211       EpCtrlSet_DTOG_TX(EpInfo[EndPoint].pEpCtrl,0);
        MOVS     R1,#+0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_DTOG_TX
// 1212     case EP_INTERRUPT:
// 1213     case EP_ISO:
// 1214     case EP_BULK_SING_BUFF:
// 1215       EpCtrlSet_DTOG_RX(EpInfo[EndPoint].pEpCtrl,0);
??USB_SetStallEP_10:
        MOVS     R1,#+0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_DTOG_RX
// 1216     }
// 1217     EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
??USB_SetStallEP_11:
        MOVS     R1,#+2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_SetStallEP_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_RX
// 1218   }
// 1219   return(USB_OK);
??USB_SetStallEP_5:
        MOVS     R0,#+0
??USB_SetStallEP_2:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        DATA
??USB_SetStallEP_0:
        DC32     EpInfo
        CFI EndBlock cfiBlock58
// 1220 }
// 1221 
// 1222 /*************************************************************************
// 1223  * Function Name: USB_GetStallEP
// 1224  * Parameters: USB_Endpoint_t EndPoint, pBoolean pStall
// 1225  *
// 1226  * Return: USB_ErrorCodes_t
// 1227  *
// 1228  * Description: Get stall state of the endpoint
// 1229  *
// 1230  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock59 Using cfiCommon0
        CFI Function USB_GetStallEP
        THUMB
// 1231 static
// 1232 USB_ErrorCodes_t USB_GetStallEP (USB_Endpoint_t EndPoint, pBoolean pStall)
// 1233 {
USB_GetStallEP:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
// 1234   if(EpInfo[EndPoint].pEpCtrl == NULL)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_GetStallEP_0  ;; EpInfo
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BNE      ??USB_GetStallEP_1
// 1235   {
// 1236     return(USB_EP_NOT_VALID);
        MOVS     R0,#+6
        B        ??USB_GetStallEP_2
// 1237   }
// 1238   *pStall = (EndPoint & 1)?
// 1239             (EpCtrlGet_STAT_TX(EpInfo[EndPoint].pEpCtrl) == EP_STALL):  // IN EP
// 1240             (EpCtrlGet_STAT_RX(EpInfo[EndPoint].pEpCtrl) == EP_STALL);  // OUT EP
??USB_GetStallEP_1:
        LSLS     R0,R4,#+31
        BPL      ??USB_GetStallEP_3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_GetStallEP_0  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_STAT_TX
        CMP      R0,#+1
        BNE      ??USB_GetStallEP_4
        MOVS     R0,#+1
        B        ??USB_GetStallEP_5
??USB_GetStallEP_4:
        MOVS     R0,#+0
??USB_GetStallEP_5:
        B        ??USB_GetStallEP_6
??USB_GetStallEP_3:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_GetStallEP_0  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_STAT_RX
        CMP      R0,#+1
        BNE      ??USB_GetStallEP_7
        MOVS     R0,#+1
        B        ??USB_GetStallEP_8
??USB_GetStallEP_7:
        MOVS     R0,#+0
??USB_GetStallEP_8:
??USB_GetStallEP_6:
        STRB     R0,[R5, #+0]
// 1241   return (USB_OK);
        MOVS     R0,#+0
??USB_GetStallEP_2:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??USB_GetStallEP_0:
        DC32     EpInfo
        CFI EndBlock cfiBlock59
// 1242 }
// 1243 
// 1244 /*************************************************************************
// 1245  * Function Name: USB_EpValidate
// 1246  * Parameters: USB_Endpoint_t EndPoint, Boolean State
// 1247  *
// 1248  * Return: USB_ErrorCodes_t
// 1249  *
// 1250  * Description: Validate/Unvalidate EP buffer
// 1251  *
// 1252  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock60 Using cfiCommon0
        CFI Function USB_EpValidate
        THUMB
// 1253 USB_ErrorCodes_t USB_EpValidate( USB_Endpoint_t EndPoint, Boolean State)
// 1254 {
USB_EpValidate:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
// 1255   if( EpInfo[EndPoint].pEpCtrl == NULL)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BNE      ??USB_EpValidate_1
// 1256   {
// 1257     return(USB_EP_NOT_VALID);
        MOVS     R0,#+6
        B        ??USB_EpValidate_2
// 1258   }
// 1259 
// 1260   if(EndPoint & 1)
??USB_EpValidate_1:
        LSLS     R0,R4,#+31
        BPL      ??USB_EpValidate_3
// 1261   {
// 1262   // IN EP (Tx)
// 1263     if (State)
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+0
        BEQ      ??USB_EpValidate_4
// 1264     {
// 1265       // Validate buffer
// 1266        EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_EpValidate_5
// 1267     }
// 1268     else
// 1269     {
// 1270       // Unvalidate buffer
// 1271       EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
??USB_EpValidate_4:
        MOVS     R1,#+2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
// 1272       EpCtrlClr_CTR_TX(EpInfo[EndPoint].pEpCtrl);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_TX
        B        ??USB_EpValidate_5
// 1273     }
// 1274   }
// 1275   else
// 1276   {
// 1277   // OUT EP (Rx)
// 1278     if (State)
??USB_EpValidate_3:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+0
        BEQ      ??USB_EpValidate_6
// 1279     {
// 1280       // Validate buffer
// 1281       EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_RX
        B        ??USB_EpValidate_5
// 1282     }
// 1283     else
// 1284     {
// 1285       // Unvalidate buffer
// 1286       EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
??USB_EpValidate_6:
        MOVS     R1,#+2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_RX
// 1287       EpCtrlClr_CTR_RX(EpInfo[EndPoint].pEpCtrl);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpValidate_0  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_RX
// 1288     }
// 1289   }
// 1290   return(USB_OK);
??USB_EpValidate_5:
        MOVS     R0,#+0
??USB_EpValidate_2:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        DATA
??USB_EpValidate_0:
        DC32     EpInfo
        CFI EndBlock cfiBlock60
// 1291 }
// 1292 
// 1293 /*************************************************************************
// 1294  * Function Name: USB_EpWrite
// 1295  * Parameters: USB_Endpoint_t EndPoint, pInt8U pData, pInt32U pCount
// 1296  *
// 1297  * Return: USB_ErrorCodes_t
// 1298  *
// 1299  * Description: Endpoint Write (IN)
// 1300  *
// 1301  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock61 Using cfiCommon0
        CFI Function USB_EpWrite
        THUMB
// 1302 USB_ErrorCodes_t USB_EpWrite (USB_Endpoint_t EndPoint, pInt8U pData,
// 1303                               pInt32U pCount)
// 1304 {
USB_EpWrite:
        PUSH     {R2,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
        SUB      SP,SP,#+24
        CFI CFA R13+48
        MOVS     R4,R0
        MOVS     R5,R1
// 1305 __usb_epr_bits EpState;
// 1306 pInt32U pDst;
// 1307 Int32U  ActSize, Temp, Count, CurrSize, SizeHold;
// 1308 
// 1309   ActSize = *pCount;
        LDR      R0,[SP, #+24]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+12]
// 1310   *pCount = 0;
        LDR      R0,[SP, #+24]
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
// 1311   if(EpInfo[EndPoint].pEpCtrl == NULL)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BNE      ??USB_EpWrite_1
// 1312   {
// 1313     return(USB_EP_NOT_VALID);
        MOVS     R0,#+6
        BL       ??USB_EpWrite_2
// 1314   }
// 1315 
// 1316   EpState = *(__usb_epr_bits *)EpInfo[EndPoint].pEpCtrl;
??USB_EpWrite_1:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        LDR      R0,[R1, R0]
        LDRH     R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1317 
// 1318   if(EpState.STATTX == EP_DISABLED)
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BNE      ??USB_EpWrite_3
// 1319   {
// 1320     // Fatal error (require restart of the USB)
// 1321     return(USB_EP_FATAL_ERROR);
        MOVS     R0,#+10
        BL       ??USB_EpWrite_2
// 1322   }
// 1323 
// 1324   switch(EpInfo[EndPoint].EpType)
??USB_EpWrite_3:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+0
        BNE      .+4
        B        ??USB_EpWrite_4
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpWrite_5
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpWrite_6
        SUBS     R0,R0,#+1
        BEQ      ??USB_EpWrite_7
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpWrite_8
        BL       ??USB_EpWrite_9
// 1325   {
// 1326   case EP_INTERRUPT:
// 1327     if (EpState.STATTX == EP_STALL)
??USB_EpWrite_7:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpWrite_10
// 1328     {
// 1329       // EP stalled
// 1330       return(USB_EP_STALLED);
        MOVS     R0,#+8
        BL       ??USB_EpWrite_2
// 1331     }
// 1332     else if(EpState.STATTX == EP_VALID)
??USB_EpWrite_10:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpWrite_11
// 1333     {
// 1334       // Data sending in progress
// 1335       return(USB_OK);
        MOVS     R0,#+0
        BL       ??USB_EpWrite_2
// 1336     }
// 1337     // Get is smaller of user buffer and received data.
// 1338     SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
??USB_EpWrite_11:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+12]
        CMP      R0,R1
        BCS      ??USB_EpWrite_12
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+0]
        B        ??USB_EpWrite_13
??USB_EpWrite_12:
        LDR      R0,[SP, #+12]
        STR      R0,[SP, #+0]
// 1339     WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
??USB_EpWrite_13:
        LDR      R1,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+0]
        STRH     R1,[R0, R2]
// 1340     pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1341     for (Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_14:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_15
// 1342     {
// 1343       Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1344       if (++Count < SizeHold)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_16
// 1345       {
// 1346         Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1347       }
// 1348       *pDst++ = Temp;
??USB_EpWrite_16:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1349     }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_14
// 1350     // Validate buffer
// 1351     EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
??USB_EpWrite_15:
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        BL       ??USB_EpWrite_17
// 1352     break;
// 1353   case EP_CTRL:
// 1354     if(EpState.STATTX == EP_VALID)
??USB_EpWrite_5:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpWrite_18
// 1355     {
// 1356       // Data sending in progress
// 1357       return(USB_OK);
        MOVS     R0,#+0
        BL       ??USB_EpWrite_2
// 1358     }
// 1359     // Get smaller of the user buffer and the received data.
// 1360     SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
??USB_EpWrite_18:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+12]
        CMP      R0,R1
        BCS      ??USB_EpWrite_19
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+0]
        B        ??USB_EpWrite_20
??USB_EpWrite_19:
        LDR      R0,[SP, #+12]
        STR      R0,[SP, #+0]
// 1361     WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
??USB_EpWrite_20:
        LDR      R1,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+0]
        STRH     R1,[R0, R2]
// 1362     pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1363     for (Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_21:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_22
// 1364     {
// 1365       Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1366       if (++Count < SizeHold)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_23
// 1367       {
// 1368         Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1369       }
// 1370       *pDst++ = Temp;
??USB_EpWrite_23:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1371     }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_21
// 1372     // Validate buffer
// 1373     EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
??USB_EpWrite_22:
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpWrite_0  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_EpWrite_17
        Nop      
        DATA
??USB_EpWrite_0:
        DC32     EpInfo
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        THUMB
// 1374     break;
// 1375   case EP_ISO:
// 1376     if(EpState.DTOGTX)
??USB_EpWrite_6:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        LSLS     R0,R0,#+25
        BMI      .+4
        B        ??USB_EpWrite_24
// 1377     {
// 1378       // Tx Buffer
// 1379       pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1380       // Get smaller of the user buffer and the received data.
// 1381       SizeHold = MIN(EpInfo[EndPoint].MaxSize,SizeHold);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+0]
        CMP      R0,R1
        BCS      ??USB_EpWrite_26
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+0]
        B        ??USB_EpWrite_27
??USB_EpWrite_26:
        LDR      R0,[SP, #+0]
        STR      R0,[SP, #+0]
// 1382       for (Count = 0; Count < SizeHold; ++Count)
??USB_EpWrite_27:
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_28:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_29
// 1383       {
// 1384         Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1385         if (++Count < SizeHold)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_30
// 1386         {
// 1387           Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1388         }
// 1389         *pDst++ = Temp;
??USB_EpWrite_30:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1390       }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_28
// 1391       WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
??USB_EpWrite_29:
        LDR      R1,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+0]
        STRH     R1,[R0, R2]
        B        ??USB_EpWrite_17
// 1392     }
// 1393     else
// 1394     {
// 1395       // Rx Buffer
// 1396       pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
??USB_EpWrite_24:
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1397       // Get smaller of the user buffer and the received data.
// 1398       SizeHold = MIN(EpInfo[EndPoint].MaxSize,SizeHold);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+0]
        CMP      R0,R1
        BCS      ??USB_EpWrite_31
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+0]
        B        ??USB_EpWrite_32
??USB_EpWrite_31:
        LDR      R0,[SP, #+0]
        STR      R0,[SP, #+0]
// 1399       for (Count = 0; Count < SizeHold; ++Count)
??USB_EpWrite_32:
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_33:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_34
// 1400       {
// 1401         Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1402         if (++Count < SizeHold)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_35
// 1403         {
// 1404           Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1405         }
// 1406         *pDst++ = Temp;
??USB_EpWrite_35:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1407       }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_33
// 1408       WriteEpDTB_CountRx(EpInfo[EndPoint].EpSlot,SizeHold);
??USB_EpWrite_34:
        LDR      R1,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_25+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_25  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+0]
        STRH     R1,[R0, R2]
        B        ??USB_EpWrite_17
// 1409     }
// 1410     break;
// 1411   case EP_BULK_SING_BUFF:
// 1412     if (EpState.STATTX == EP_STALL)
??USB_EpWrite_4:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpWrite_36
// 1413     {
// 1414       // EP stalled
// 1415       return(USB_EP_STALLED);
        MOVS     R0,#+8
        B        ??USB_EpWrite_2
// 1416     }
// 1417     else if(EpState.STATTX == EP_VALID)
??USB_EpWrite_36:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpWrite_37
// 1418     {
// 1419       // Data sending in progress
// 1420       return(USB_OK);
        MOVS     R0,#+0
        B        ??USB_EpWrite_2
// 1421     }
// 1422     /// Get smaller of the user buffer and the received data.
// 1423     SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
??USB_EpWrite_37:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+12]
        CMP      R0,R1
        BCS      ??USB_EpWrite_38
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_25+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+0]
        B        ??USB_EpWrite_39
        DATA
??USB_EpWrite_25:
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        THUMB
??USB_EpWrite_38:
        LDR      R0,[SP, #+12]
        STR      R0,[SP, #+0]
// 1424     WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
??USB_EpWrite_39:
        LDR      R1,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+0]
        STRH     R1,[R0, R2]
// 1425 
// 1426     pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1427     for (Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_41:
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_42
// 1428     {
// 1429       Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1430       if (++Count < SizeHold)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+0]
        CMP      R7,R0
        BCS      ??USB_EpWrite_43
// 1431       {
// 1432         Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1433       }
// 1434       *pDst++ = Temp;
??USB_EpWrite_43:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1435     }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_41
// 1436     // Validate buffer
// 1437     EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
??USB_EpWrite_42:
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
        B        ??USB_EpWrite_17
// 1438     break;
// 1439   case EP_BULK_DOUB_BUF:
// 1440     // Double buffered
// 1441     if (EpState.STATTX == EP_STALL)
??USB_EpWrite_8:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpWrite_44
// 1442     {
// 1443       // EP stalled
// 1444       return(USB_EP_STALLED);
        MOVS     R0,#+8
        B        ??USB_EpWrite_2
// 1445     }
// 1446 
// 1447     SizeHold = 0;
??USB_EpWrite_44:
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
// 1448     do
// 1449     {
// 1450       // Check for a pending TX interrupt
// 1451       if(!EpCtrlGet_CTR_TX(EpInfo[EndPoint].pEpCtrl) &&
// 1452          !EpInfo[EndPoint].EpIntrFl &&
// 1453          (EpState.DTOGRX == EpCtrlGet_DTOG_TX(EpInfo[EndPoint].pEpCtrl)))
??USB_EpWrite_45:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_40+0x8  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_CTR_TX
        CMP      R0,#+0
        BNE      ??USB_EpWrite_46
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+16]
        CMP      R0,#+0
        BNE      ??USB_EpWrite_46
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        MOVS     R1,#+128
        LSLS     R1,R1,#+7        ;; #+16384
        ANDS     R1,R1,R0
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSRS     R1,R1,#+14
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        PUSH     {R1,LR}
        CFI CFA R13+56
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_40+0x8  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_DTOG_TX
        LDR      R1,[SP, #0]
        ADD      SP,SP,#+8
        CFI CFA R13+48
        CMP      R1,R0
        BNE      .+4
        B        ??USB_EpWrite_17
// 1454       {
// 1455         // All buffers are filled
// 1456         break;
// 1457       }
// 1458 
// 1459       // Get smaller of the rest of the user buffer and the received data.
// 1460       CurrSize = MIN(EpInfo[EndPoint].MaxSize,(ActSize-SizeHold));
??USB_EpWrite_46:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        LDR      R1,[SP, #+12]
        LDR      R2,[SP, #+0]
        SUBS     R1,R1,R2
        CMP      R0,R1
        BCS      ??USB_EpWrite_47
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+8]
        STR      R0,[SP, #+16]
        B        ??USB_EpWrite_48
??USB_EpWrite_47:
        LDR      R0,[SP, #+12]
        LDR      R1,[SP, #+0]
        SUBS     R0,R0,R1
        STR      R0,[SP, #+16]
// 1461 
// 1462       if(!EpState.DTOGRX)
??USB_EpWrite_48:
        MOV      R0,SP
        LDRH     R0,[R0, #+8]
        LSLS     R0,R0,#+17
        BPL      .+4
        B        ??USB_EpWrite_49
// 1463       {
// 1464         // Tx Buffer
// 1465         pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1466         WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,CurrSize);
        LDR      R1,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_40+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_40  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_40+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+16]
        STRH     R1,[R0, R2]
        B        ??USB_EpWrite_50
        Nop      
        DATA
??USB_EpWrite_40:
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        THUMB
// 1467       }
// 1468       else
// 1469       {
// 1470         // Rx Buffer
// 1471         pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
??USB_EpWrite_49:
        LDR      R0,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_51+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpWrite_51+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOVS     R6,R0
// 1472         WriteEpDTB_CountRx(EpInfo[EndPoint].EpSlot,CurrSize);
        LDR      R1,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpWrite_51+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpWrite_51  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDR      R1,[SP, #+16]
        STRH     R1,[R0, R2]
// 1473       }
// 1474 
// 1475       for (Count = 0; Count < CurrSize; ++Count)
??USB_EpWrite_50:
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpWrite_52:
        LDR      R0,[SP, #+16]
        CMP      R7,R0
        BCS      ??USB_EpWrite_53
// 1476       {
// 1477         Temp  = *pData++;
        LDRB     R0,[R5, #+0]
        STR      R0,[SP, #+4]
        ADDS     R5,R5,#+1
// 1478         if (++Count < CurrSize)
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+16]
        CMP      R7,R0
        BCS      ??USB_EpWrite_54
// 1479         {
// 1480           Temp |= (*pData++) << 8;
        LDR      R0,[SP, #+4]
        LDRB     R1,[R5, #+0]
        LSLS     R1,R1,#+8
        ORRS     R1,R1,R0
        STR      R1,[SP, #+4]
        ADDS     R5,R5,#+1
// 1481         }
// 1482         *pDst++ = Temp;
??USB_EpWrite_54:
        LDR      R0,[SP, #+4]
        STR      R0,[R6, #+0]
        ADDS     R6,R6,#+4
// 1483       }
        ADDS     R7,R7,#+1
        B        ??USB_EpWrite_52
// 1484 
// 1485       EpInfo[EndPoint].EpIntrFl = 0;
??USB_EpWrite_53:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_51+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+0
        STR      R1,[R0, #+16]
// 1486       // Clear for pending TX interrupt
// 1487       EpCtrlClr_CTR_TX(EpInfo[EndPoint].pEpCtrl);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_51+0x8  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_TX
// 1488       EpCtrlToggle_DTOG_RX(EpInfo[EndPoint].pEpCtrl);
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpWrite_51+0x8  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlToggle_DTOG_RX
// 1489       // Update RX toggle status
// 1490       EpState.DTOGRX ^= 1;
        MOV      R0,SP
        MOV      R1,SP
        LDRH     R1,[R1, #+8]
        MOVS     R2,#+128
        LSLS     R2,R2,#+7        ;; #+16384
        EORS     R2,R2,R1
        STRH     R2,[R0, #+8]
// 1491       EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
        MOVS     R1,#+3
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R2,??USB_EpWrite_51+0x8  ;; EpInfo
        LDR      R0,[R2, R0]
        BL       EpCtrlSet_STAT_TX
// 1492       SizeHold += CurrSize;
        LDR      R0,[SP, #+0]
        LDR      R1,[SP, #+16]
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1493     } while(SizeHold < ActSize);
        LDR      R0,[SP, #+0]
        LDR      R1,[SP, #+12]
        CMP      R0,R1
        BCS      .+4
        B        ??USB_EpWrite_45
        B        ??USB_EpWrite_17
// 1494     break;
// 1495   default:
// 1496     // Fatal error (require restart of the USB)
// 1497     return(USB_EP_FATAL_ERROR);
??USB_EpWrite_9:
        MOVS     R0,#+10
        B        ??USB_EpWrite_2
// 1498   }
// 1499 
// 1500   *pCount = SizeHold;
??USB_EpWrite_17:
        LDR      R0,[SP, #+24]
        LDR      R1,[SP, #+0]
        STR      R1,[R0, #+0]
// 1501   return(USB_OK);
        MOVS     R0,#+0
??USB_EpWrite_2:
        ADD      SP,SP,#+28
        CFI CFA R13+20
        POP      {R4-R7}
        POP      {R1}
        BX       R1               ;; return
        DATA
??USB_EpWrite_51:
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        CFI EndBlock cfiBlock61
// 1502 }
// 1503 
// 1504 /*************************************************************************
// 1505  * Function Name: USB_EpRead
// 1506  * Parameters: USB_Endpoint_t EndPoint, pInt8U pData, pInt32U pCount
// 1507  *
// 1508  * Return: USB_ErrorCodes_t
// 1509  *
// 1510  * Description: Endpoint Read (OUT)
// 1511  *
// 1512  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock62 Using cfiCommon0
        CFI Function USB_EpRead
        THUMB
// 1513 USB_ErrorCodes_t USB_EpRead (USB_Endpoint_t EndPoint, pInt8U pData,
// 1514                              pInt32U pCount)
// 1515 {
USB_EpRead:
        PUSH     {R2,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
        SUB      SP,SP,#+24
        CFI CFA R13+48
        MOVS     R4,R0
        MOVS     R6,R1
// 1516 volatile pInt32U pSrc;
// 1517 Int32U ActSize = *pCount, EpCount;
        LDR      R0,[SP, #+24]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+16]
// 1518 __usb_epr_bits EpState;
// 1519 Int32U Temp;
// 1520 Int32U CurrSize,SizeHold;
// 1521 Int32U Count;
// 1522 
// 1523   *pCount = 0;
        LDR      R0,[SP, #+24]
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
// 1524 
// 1525   if(EpInfo[EndPoint].pEpCtrl == NULL)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_0  ;; EpInfo
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BNE      ??USB_EpRead_1
// 1526   {
// 1527     return(USB_EP_NOT_VALID);
        MOVS     R0,#+6
        BL       ??USB_EpRead_2
// 1528   }
// 1529 
// 1530   EpState = *(__usb_epr_bits *)EpInfo[EndPoint].pEpCtrl;
??USB_EpRead_1:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_0  ;; EpInfo
        LDR      R0,[R1, R0]
        LDRH     R0,[R0, #+0]
        STR      R0,[SP, #+12]
// 1531   if(EpState.STATRX == EP_DISABLED)
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BNE      ??USB_EpRead_3
// 1532   {
// 1533     return(USB_EP_FATAL_ERROR);
        MOVS     R0,#+10
        BL       ??USB_EpRead_2
// 1534   }
// 1535 
// 1536   switch (EpInfo[EndPoint].EpType)
??USB_EpRead_3:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_0  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+0
        BNE      .+4
        B        ??USB_EpRead_4
        SUBS     R0,R0,#+1
        BEQ      ??USB_EpRead_5
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpRead_6
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpRead_7
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??USB_EpRead_8
        BL       ??USB_EpRead_9
// 1537   {
// 1538   case EP_CTRL:
// 1539     if(EpState.STATRX == EP_VALID)
??USB_EpRead_5:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpRead_10
// 1540     {
// 1541       // Data is not received yet.
// 1542       return(UB_EP_SETUP_UNDERRUN);
        MOVS     R0,#+7
        BL       ??USB_EpRead_2
// 1543     }
// 1544     // Get received bytes number
// 1545     EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
??USB_EpRead_10:
        LDR      R1,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1546     // Get address of the USB packet buffer for corresponding EP
// 1547     pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1548     // Get smaller of the user buffer and the received data.
// 1549     SizeHold = MIN(EpCount,ActSize);
        LDR      R0,[SP, #+4]
        LDR      R1,[SP, #+16]
        CMP      R0,R1
        BLS      ??USB_EpRead_11
        MOVS     R0,R1
??USB_EpRead_11:
        MOVS     R5,R0
// 1550 
// 1551     for(Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpRead_12:
        CMP      R7,R5
        BCS      ??USB_EpRead_13
// 1552     {
// 1553       Temp = *pSrc++;
        LDR      R0,[SP, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+4
        STR      R1,[SP, #+0]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1554       *pData++ = Temp;
        LDR      R0,[SP, #+8]
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1555       if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
        ADDS     R7,R7,#+1
        CMP      R5,R7
        BCC      ??USB_EpRead_14
// 1556       {
// 1557         *pData++ = Temp>>8;
        LDR      R0,[SP, #+8]
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1558       }
// 1559     }
??USB_EpRead_14:
        ADDS     R7,R7,#+1
        B        ??USB_EpRead_12
// 1560     *pCount = SizeHold;
??USB_EpRead_13:
        LDR      R0,[SP, #+24]
        STR      R5,[R0, #+0]
// 1561     if(EpCtrlGet_SETUP(EpInfo[EndPoint].pEpCtrl))
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_0  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_SETUP
        CMP      R0,#+0
        BNE      .+6
        BL       ??USB_EpRead_15
// 1562     {
// 1563       return(UB_EP_SETUP_OVERWRITE);
        MOVS     R0,#+9
        BL       ??USB_EpRead_2
// 1564     }
// 1565 
// 1566     break;
// 1567   case EP_ISO:
// 1568     if(EpState.DTOGRX)
??USB_EpRead_6:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        LSLS     R0,R0,#+17
        BMI      .+4
        B        ??USB_EpRead_16
// 1569     {
// 1570       EpCount = ReadEpDTB_CountTx(EpInfo[EndPoint].EpSlot) & 0x3FF;
        LDR      R1,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1571       pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_0+0x8  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_0+0x4  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_0  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
        B        ??USB_EpRead_17
        Nop      
        DATA
??USB_EpRead_0:
        DC32     EpInfo
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        THUMB
// 1572     }
// 1573     else
// 1574     {
// 1575       EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
??USB_EpRead_16:
        LDR      R1,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1576       pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1577     }
// 1578     // Get smaller of the user buffer and the received data.
// 1579     SizeHold = MIN(EpCount,ActSize);
??USB_EpRead_17:
        LDR      R0,[SP, #+4]
        LDR      R1,[SP, #+16]
        CMP      R0,R1
        BLS      ??USB_EpRead_19
        MOVS     R0,R1
??USB_EpRead_19:
        MOVS     R5,R0
// 1580 
// 1581     for(Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpRead_20:
        CMP      R7,R5
        BCC      .+4
        B        ??USB_EpRead_15
// 1582     {
// 1583       Temp = *pSrc++;
        LDR      R0,[SP, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+4
        STR      R1,[SP, #+0]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1584       *pData++ = Temp;
        LDR      R0,[SP, #+8]
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1585       if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
        ADDS     R7,R7,#+1
        CMP      R5,R7
        BCC      ??USB_EpRead_21
// 1586       {
// 1587         *pData++ = Temp>>8;
        LDR      R0,[SP, #+8]
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1588       }
// 1589     }
??USB_EpRead_21:
        ADDS     R7,R7,#+1
        B        ??USB_EpRead_20
// 1590     break;
// 1591   case EP_INTERRUPT:
// 1592     if (EpState.STATRX == EP_STALL)
??USB_EpRead_7:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpRead_22
// 1593     {
// 1594       return(USB_EP_STALLED);
        MOVS     R0,#+8
        B        ??USB_EpRead_2
// 1595     }
// 1596     else if (EpState.STATRX == EP_VALID)
??USB_EpRead_22:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpRead_23
// 1597     {
// 1598       return(UB_EP_SETUP_UNDERRUN);
        MOVS     R0,#+7
        B        ??USB_EpRead_2
// 1599     }
// 1600     EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
??USB_EpRead_23:
        LDR      R1,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1601     pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_18+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_18  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_18+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1602     // Get smaller of the user buffer and the received data.
// 1603     SizeHold = MIN(EpCount,ActSize);
        LDR      R0,[SP, #+4]
        LDR      R1,[SP, #+16]
        CMP      R0,R1
        BLS      ??USB_EpRead_24
        MOVS     R0,R1
??USB_EpRead_24:
        MOVS     R5,R0
// 1604 
// 1605     for(Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpRead_25:
        CMP      R7,R5
        BCC      .+4
        B        ??USB_EpRead_15
// 1606     {
// 1607       Temp = *pSrc++;
        LDR      R0,[SP, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+4
        STR      R1,[SP, #+0]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1608       *pData++ = Temp;
        LDR      R0,[SP, #+8]
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1609       if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
        ADDS     R7,R7,#+1
        CMP      R5,R7
        BCC      ??USB_EpRead_26
// 1610       {
// 1611         *pData++ = Temp>>8;
        LDR      R0,[SP, #+8]
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1612       }
// 1613     }
??USB_EpRead_26:
        ADDS     R7,R7,#+1
        B        ??USB_EpRead_25
// 1614     break;
// 1615   case EP_BULK_SING_BUFF:
// 1616     // Single buffer
// 1617     if (EpState.STATRX == EP_STALL)
??USB_EpRead_4:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpRead_27
// 1618     {
// 1619       return(USB_EP_STALLED);
        MOVS     R0,#+8
        B        ??USB_EpRead_2
// 1620     }
// 1621     else if (EpState.STATRX == EP_VALID)
??USB_EpRead_27:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+3
        BNE      ??USB_EpRead_28
// 1622     {
// 1623       return(UB_EP_SETUP_UNDERRUN);
        MOVS     R0,#+7
        B        ??USB_EpRead_2
        DATA
??USB_EpRead_18:
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        THUMB
// 1624     }
// 1625     EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
??USB_EpRead_28:
        LDR      R1,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1626     pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1627     // Get smaller of the user buffer and the received data.
// 1628     SizeHold = MIN(EpCount,ActSize);
        LDR      R0,[SP, #+4]
        LDR      R1,[SP, #+16]
        CMP      R0,R1
        BLS      ??USB_EpRead_30
        MOVS     R0,R1
??USB_EpRead_30:
        MOVS     R5,R0
// 1629 
// 1630     for(Count = 0; Count < SizeHold; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpRead_31:
        CMP      R7,R5
        BCC      .+4
        B        ??USB_EpRead_15
// 1631     {
// 1632       Temp = *pSrc++;
        LDR      R0,[SP, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+4
        STR      R1,[SP, #+0]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1633       *pData++ = Temp;
        LDR      R0,[SP, #+8]
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1634       if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
        ADDS     R7,R7,#+1
        CMP      R5,R7
        BCC      ??USB_EpRead_32
// 1635       {
// 1636         *pData++ = Temp>>8;
        LDR      R0,[SP, #+8]
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1637       }
// 1638     }
??USB_EpRead_32:
        ADDS     R7,R7,#+1
        B        ??USB_EpRead_31
// 1639     break;
// 1640   case EP_BULK_DOUB_BUF:
// 1641     if (EpState.STATRX == EP_STALL)
??USB_EpRead_8:
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        MOVS     R1,#+192
        LSLS     R1,R1,#+6        ;; #+12288
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSRS     R0,R0,#+12
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+1
        BNE      ??USB_EpRead_33
// 1642     {
// 1643       return(USB_EP_STALLED);
        MOVS     R0,#+8
        B        ??USB_EpRead_2
// 1644     }
// 1645     SizeHold = 0;
??USB_EpRead_33:
        MOVS     R0,#+0
        MOVS     R5,R0
// 1646     do
// 1647     {
// 1648       if(!EpInfo[EndPoint].EpIntrFl)
??USB_EpRead_34:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+16]
        CMP      R0,#+0
        BNE      .+4
        B        ??USB_EpRead_15
// 1649       {
// 1650         // Buffers are empties
// 1651         break;
// 1652       }
// 1653       if(!EpState.DTOGTX)
        MOV      R0,SP
        LDRH     R0,[R0, #+12]
        LSLS     R0,R0,#+25
        BMI      ??USB_EpRead_35
// 1654       {
// 1655         EpCount = ReadEpDTB_CountTx(EpInfo[EndPoint].EpSlot) & 0x3FF;
        LDR      R1,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+2
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1656         pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
        B        ??USB_EpRead_36
// 1657       }
// 1658       else
// 1659       {
// 1660         EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
??USB_EpRead_35:
        LDR      R1,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R0,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R0,R0,R2
        LSRS     R0,R0,#+1
        SUBS     R0,R0,#+24
        MOVS     R2,#+7
        BICS     R0,R0,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R2,#+20
        MULS     R2,R4,R2
        LDR      R3,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R3,R2
        LDRB     R2,[R2, #+4]
        MOVS     R3,#+8
        MULS     R2,R3,R2
        ADDS     R0,R0,R2
        ADDS     R0,R0,#+6
        LSLS     R0,R0,#+1
        MOVS     R2,#+3
        BICS     R0,R0,R2
        ADDS     R0,R1,R0
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+4]
        MOVS     R2,#+8
        MULS     R1,R2,R1
        MOVS     R2,#+1
        ANDS     R2,R2,R1
        LDRH     R0,[R0, R2]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R0,R0,#+22       ;; ZeroExtS R0,R0,#+22,#+22
        LSRS     R0,R0,#+22
        STR      R0,[SP, #+4]
// 1661         pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        LSLS     R1,R0,#+1
        MOVS     R0,#+3
        BICS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        MOV      R12,R0
        LDR      R0,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        LDR      R1,??USB_EpRead_29+0x4  ;; SFE(USB_PACKET_MEMORY)
        LDR      R2,??USB_EpRead_29  ;; SFB(USB_PACKET_MEMORY)
        SUBS     R1,R1,R2
        LSRS     R1,R1,#+1
        SUBS     R1,R1,#+24
        MOVS     R2,#+7
        BICS     R1,R1,R2
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R3,#+20
        MULS     R3,R4,R3
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R2,R2,R3
        LDRB     R3,[R2, #+4]
        MOVS     R2,#+8
        MULS     R3,R2,R3
        ADDS     R1,R1,R3
        ADDS     R1,R1,#+4
        LSLS     R1,R1,#+1
        MOVS     R2,#+3
        BICS     R1,R1,R2
        ADDS     R0,R0,R1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_29+0x8  ;; EpInfo
        ADDS     R1,R2,R1
        LDRB     R2,[R1, #+4]
        MOVS     R1,#+8
        MULS     R2,R1,R2
        MOVS     R1,#+1
        ANDS     R1,R1,R2
        LDRH     R0,[R0, R1]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R1,#+1
        ANDS     R1,R1,R0
        MOV      R0,R12
        ADDS     R0,R0,R1
        STR      R0,[SP, #+0]
// 1662       }
// 1663 
// 1664       // the buffer size must be enough bigger to get all available data from
// 1665       // the EP in other case remaining data will be lost
// 1666       if(EpCount > (ActSize-SizeHold))
??USB_EpRead_36:
        LDR      R0,[SP, #+16]
        SUBS     R0,R0,R5
        LDR      R1,[SP, #+4]
        CMP      R0,R1
        BCS      ??USB_EpRead_37
// 1667       {
// 1668         *pCount = SizeHold;
        LDR      R0,[SP, #+24]
        STR      R5,[R0, #+0]
// 1669         return (USB_BUF_OVERFLOW);
        MOVS     R0,#+5
        B        ??USB_EpRead_2
        Nop      
        DATA
??USB_EpRead_29:
        DC32     SFB(USB_PACKET_MEMORY)
        DC32     SFE(USB_PACKET_MEMORY)
        DC32     EpInfo
        THUMB
// 1670       }
// 1671       CurrSize = EpCount;
??USB_EpRead_37:
        LDR      R0,[SP, #+4]
        STR      R0,[SP, #+20]
// 1672       for(Count = 0; Count < CurrSize; ++Count)
        MOVS     R0,#+0
        MOVS     R7,R0
??USB_EpRead_38:
        LDR      R0,[SP, #+20]
        CMP      R7,R0
        BCS      ??USB_EpRead_39
// 1673       {
// 1674         Temp = *pSrc++;
        LDR      R0,[SP, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+4
        STR      R1,[SP, #+0]
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+8]
// 1675         *pData++ = Temp;
        LDR      R0,[SP, #+8]
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1676         if(++Count <= CurrSize) // fix overwrite problem with odd number of bytes
        ADDS     R7,R7,#+1
        LDR      R0,[SP, #+20]
        CMP      R0,R7
        BCC      ??USB_EpRead_40
// 1677         {
// 1678           *pData++ = Temp>>8;
        LDR      R0,[SP, #+8]
        LSRS     R0,R0,#+8
        STRB     R0,[R6, #+0]
        ADDS     R6,R6,#+1
// 1679         }
// 1680       }
??USB_EpRead_40:
        ADDS     R7,R7,#+1
        B        ??USB_EpRead_38
// 1681       EpCtrlToggle_DTOG_TX(EpInfo[EndPoint].pEpCtrl);
??USB_EpRead_39:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_41  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlToggle_DTOG_TX
// 1682       EpState.DTOGTX ^= 1;
        MOV      R0,SP
        MOV      R1,SP
        LDRH     R1,[R1, #+12]
        MOVS     R2,#+64
        EORS     R2,R2,R1
        STRH     R2,[R0, #+12]
// 1683       SizeHold += CurrSize;
        LDR      R0,[SP, #+20]
        ADDS     R5,R5,R0
// 1684       --EpInfo[EndPoint].EpIntrFl;
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_EpRead_41  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+16]
        SUBS     R0,R0,#+1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_EpRead_41  ;; EpInfo
        ADDS     R1,R2,R1
        STR      R0,[R1, #+16]
// 1685     } while(SizeHold < ActSize);
        LDR      R0,[SP, #+16]
        CMP      R5,R0
        BCS      .+4
        B        ??USB_EpRead_34
        B        ??USB_EpRead_15
// 1686     break;
// 1687   default:
// 1688     return(USB_EP_FATAL_ERROR);
??USB_EpRead_9:
        MOVS     R0,#+10
        B        ??USB_EpRead_2
// 1689   }
// 1690   *pCount = SizeHold;
??USB_EpRead_15:
        LDR      R0,[SP, #+24]
        STR      R5,[R0, #+0]
// 1691   return (USB_OK);
        MOVS     R0,#+0
??USB_EpRead_2:
        ADD      SP,SP,#+28
        CFI CFA R13+20
        POP      {R4-R7}
        POP      {R1}
        BX       R1               ;; return
        Nop      
        DATA
??USB_EpRead_41:
        DC32     EpInfo
        CFI EndBlock cfiBlock62
// 1692 }
// 1693 
// 1694 /*************************************************************************
// 1695  * Function Name: USB_EpLogToPhysAdd
// 1696  * Parameters: Int8U EpLogAdd
// 1697  *
// 1698  * Return: USB_Endpoint_t
// 1699  *
// 1700  * Description: Convert the logical to physical address
// 1701  *
// 1702  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock63 Using cfiCommon0
        CFI Function USB_EpLogToPhysAdd
        THUMB
// 1703 USB_Endpoint_t USB_EpLogToPhysAdd (Int8U EpLogAdd)
// 1704 {
USB_EpLogToPhysAdd:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
// 1705 USB_Endpoint_t Address = (USB_Endpoint_t)((EpLogAdd & 0x0F)<<1);
        LSLS     R2,R1,#+28       ;; ZeroExtS R2,R1,#+28,#+28
        LSRS     R2,R2,#+28
        LSLS     R2,R2,#+1
        MOVS     R0,R2
// 1706   if(EpLogAdd & 0x80)
        LSLS     R2,R1,#+24
        BPL      ??USB_EpLogToPhysAdd_0
// 1707   {
// 1708     ++Address;
        ADDS     R0,R0,#+1
// 1709   }
// 1710   return(Address);
??USB_EpLogToPhysAdd_0:
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock63
// 1711 }
// 1712 
// 1713 #if USB_SOF_EVENT > 0
// 1714 /*************************************************************************
// 1715  * Function Name: USB_GetFrameNumb
// 1716  * Parameters: none
// 1717  *
// 1718  * Return: Int32U
// 1719  *
// 1720  * Description: Return current value of SOF number
// 1721  *
// 1722  *************************************************************************/
// 1723 Int32U USB_GetFrameNumb (void)
// 1724 {
// 1725   return(USB_FNR_bit.FN);
// 1726 }
// 1727 #endif // USB_SOF_EVENT > 0
// 1728 
// 1729 #if USB_HIGH_PRIORITY_EVENT > 0
// 1730 /*************************************************************************
// 1731  * Function Name: USB_HIGH_ISR
// 1732  * Parameters: none
// 1733  *
// 1734  * Return: none
// 1735  *
// 1736  * Description: High priority USB interrupt subroutine
// 1737  *
// 1738  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock64 Using cfiCommon0
        CFI Function USB_HIGH_ISR
        THUMB
// 1739 void USB_HIGH_ISR (void)
// 1740 {
USB_HIGH_ISR:
        PUSH     {R0,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
// 1741 IntrStatus_t IntrStatus;
// 1742 Int32U EpPhAddr;
// 1743 Int32U Status;
// 1744   IntrStatus.Status  = USB_ISTR;
        LDR      R0,??USB_HIGH_ISR_0  ;; 0xffffa844
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R6,R0
// 1745   IntrStatus.Status &= USB_CNTR | 0x1F;
        MOVS     R1,R6
        LDR      R0,??USB_HIGH_ISR_0+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R6,#+31
        ORRS     R6,R6,R0
        ANDS     R6,R6,R1
// 1746   // Get masked interrupt flags
// 1747   if(IntrStatus.CTR)
        LSLS     R0,R6,#+16
        BMI      .+4
        B        ??USB_HIGH_ISR_1
// 1748   {
// 1749     // find corresponding EP
// 1750     pInt32U pReg = (pInt32U)&USB_EP0R;
        LDR      R0,??USB_HIGH_ISR_0+0x8  ;; 0xffffa800
        MOVS     R7,R0
// 1751     pReg += IntrStatus.EP_ID;
        LSLS     R0,R6,#+28       ;; ZeroExtS R0,R6,#+28,#+28
        LSRS     R0,R0,#+28
        MOVS     R1,#+4
        MULS     R0,R1,R0
        ADDS     R7,R7,R0
// 1752     EpPhAddr = ((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1);
        MOVS     R0,#+16
        ANDS     R0,R0,R6
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        LDR      R1,[R7, #+0]
        LSLS     R1,R1,#+28       ;; ZeroExtS R1,R1,#+28,#+28
        LSRS     R1,R1,#+28
        LSLS     R1,R1,#+1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        ADDS     R0,R1,R0
        MOVS     R4,R0
// 1753     if(EpInfo[EpPhAddr].EpType == EP_BULK_DOUB_BUF)
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+4
        BNE      ??USB_HIGH_ISR_2
// 1754     {
// 1755       EpInfo[EpPhAddr].EpIntrFl = 1;
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+1
        STR      R1,[R0, #+16]
// 1756     }
// 1757     if(IntrStatus.DIR == 1)
??USB_HIGH_ISR_2:
        MOVS     R0,#+16
        ANDS     R0,R0,R6
        LSRS     R0,R0,#+4
        CMP      R0,#+1
        BNE      ??USB_HIGH_ISR_3
// 1758     {
// 1759       // OUT EP
// 1760       EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_RX
// 1761       Status = EpCtrlGet_DTOG_TX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_DTOG_TX
        MOVS     R5,R0
// 1762       if(Status == EpCtrlGet_DTOG_RX(EpInfo[EpPhAddr].pEpCtrl))
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_DTOG_RX
        CMP      R5,R0
        BNE      ??USB_HIGH_ISR_4
// 1763       {
// 1764         // The both buffers are filled
// 1765         ++EpInfo[EpPhAddr].EpIntrFl;
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+16]
        ADDS     R0,R0,#+1
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R1,R2,R1
        STR      R0,[R1, #+16]
// 1766       }
// 1767       // Some times USB engine receive second packet
// 1768       // after resetting of interrupt flag and before testing of
// 1769       // toggle bits
// 1770       if(EpCtrlGet_CTR_RX(EpInfo[EpPhAddr].pEpCtrl))
??USB_HIGH_ISR_4:
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_CTR_RX
        CMP      R0,#+0
        BEQ      ??USB_HIGH_ISR_5
// 1771       {
// 1772         // Now both buffers are filled.
// 1773         EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_RX
// 1774         EpInfo[EpPhAddr].EpIntrFl = 2;
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R0,R1,R0
        MOVS     R1,#+2
        STR      R1,[R0, #+16]
// 1775       }
// 1776       if(UsbUserFun[EpPhAddr] != NULL)
??USB_HIGH_ISR_5:
        MOVS     R0,#+4
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0x10  ;; UsbUserFun
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BEQ      ??USB_HIGH_ISR_1
// 1777       {
// 1778         UsbUserFun[EpPhAddr]((void *)UsbDataOutPacket);
        MOVS     R0,#+1
        MOVS     R1,#+4
        MULS     R1,R4,R1
        LDR      R2,??USB_HIGH_ISR_0+0x10  ;; UsbUserFun
        LDR      R1,[R2, R1]
        BL       __iar_via_R1
        B        ??USB_HIGH_ISR_1
// 1779       }
// 1780     }
// 1781     else
// 1782     {
// 1783       // IN EP
// 1784       EpCtrlClr_CTR_TX(EpInfo[EpPhAddr].pEpCtrl);
??USB_HIGH_ISR_3:
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_TX
// 1785       Status = EpCtrlGet_DTOG_RX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_DTOG_RX
        MOVS     R5,R0
// 1786       if(Status == EpCtrlGet_DTOG_TX(EpInfo[EpPhAddr].pEpCtrl))
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_DTOG_TX
        CMP      R5,R0
        BNE      ??USB_HIGH_ISR_6
// 1787       {
// 1788         // The both buffers are drained
// 1789         ++EpInfo[EpPhAddr].EpIntrFl;
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+16]
        ADDS     R0,R0,#+1
        MOVS     R1,#+20
        MULS     R1,R4,R1
        LDR      R2,??USB_HIGH_ISR_0+0xC  ;; EpInfo
        ADDS     R1,R2,R1
        STR      R0,[R1, #+16]
// 1790       }
// 1791       if(UsbUserFun[EpPhAddr] != NULL)
??USB_HIGH_ISR_6:
        MOVS     R0,#+4
        MULS     R0,R4,R0
        LDR      R1,??USB_HIGH_ISR_0+0x10  ;; UsbUserFun
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BEQ      ??USB_HIGH_ISR_1
// 1792       {
// 1793         UsbUserFun[EpPhAddr]((void *)UsbDataInPacket);
        MOVS     R0,#+2
        MOVS     R1,#+4
        MULS     R1,R4,R1
        LDR      R2,??USB_HIGH_ISR_0+0x10  ;; UsbUserFun
        LDR      R1,[R2, R1]
        BL       __iar_via_R1
// 1794       }
// 1795     }
// 1796   }
// 1797 }
??USB_HIGH_ISR_1:
        POP      {R3-R7}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??USB_HIGH_ISR_0:
        DC32     0xffffa844
        DC32     0xffffa840
        DC32     0xffffa800
        DC32     EpInfo
        DC32     UsbUserFun
        CFI EndBlock cfiBlock64
        REQUIRE _A_USB_EP0R
        REQUIRE _A_USB_CNTR
        REQUIRE _A_USB_ISTR
// 1798 #endif // USB_HIGH_PRIORITY_EVENT > 0
// 1799 
// 1800 /*************************************************************************
// 1801  * Function Name: USB_ISR
// 1802  * Parameters: none
// 1803  *
// 1804  * Return: none
// 1805  *
// 1806  * Description: USB interrupt subroutine
// 1807  *
// 1808  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock65 Using cfiCommon0
        CFI Function USB_ISR
        THUMB
// 1809 void USB_ISR (void)
// 1810 {
USB_ISR:
        PUSH     {R0,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
// 1811 IntrStatus_t IntrStatus;
// 1812 Int32U EpPhAddr;
// 1813 Boolean SetupPacket;
// 1814 
// 1815   // Get masked interrupt flags
// 1816   IntrStatus.Status  = USB_ISTR;
        LDR      R0,??USB_ISR_0   ;; 0xffffa844
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R6,R0
// 1817   IntrStatus.Status &= USB_CNTR | 0x1F;
        MOVS     R1,R6
        LDR      R0,??USB_ISR_0+0x4  ;; 0xffffa840
        LDRH     R0,[R0, #+0]
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R6,#+31
        ORRS     R6,R6,R0
        ANDS     R6,R6,R1
// 1818 
// 1819 #if USB_DOVR_EVENT > 0
// 1820   // DMA over / underrun (for speed up retry process)
// 1821   if(IntrStatus.DOVR)
// 1822   {
// 1823     USB_ISTR = ~bmDOVR;
// 1824     if(UsbUserFun[UsbDovrEvent] != NULL)
// 1825     {
// 1826       UsbUserFun[UsbDovrEvent]((void *)Val);
// 1827     }
// 1828   }
// 1829 #endif
// 1830 
// 1831 #if USB_ERROR_EVENT > 0
// 1832   // USB engine error interrupt
// 1833   if(IntrStatus.ERR)
// 1834   {
// 1835     USB_ISTR = ~bmERRM;
// 1836     if(UsbUserFun[UsbErrorEvent] != NULL)
// 1837     {
// 1838       UsbUserFun[UsbErrorEvent](NULL);
// 1839     }
// 1840   }
// 1841 #endif
// 1842 
// 1843   // Device reset
// 1844   if(IntrStatus.RESET)
        LSLS     R0,R6,#+21
        BPL      ??USB_ISR_1
// 1845   {
// 1846     USB_ISTR = ~bmRESETM;
        LDR      R0,??USB_ISR_0   ;; 0xffffa844
        LDR      R1,??USB_ISR_0+0x8  ;; 0xfbff
        STRH     R1,[R0, #+0]
// 1847     USB_HwReset();
        BL       USB_HwReset
// 1848     UsbDevConnectCallback(TRUE);
        MOVS     R0,#+1
        BL       UsbDevConnectCallback
// 1849     UsbDevResetCallback();
        BL       UsbDevResetCallback
// 1850     UsbDevSuspendCallback(FALSE);
        MOVS     R0,#+0
        BL       UsbDevSuspendCallback
// 1851   }
// 1852 
// 1853 #if USB_SOF_EVENT > 0
// 1854   // Frame interrupt
// 1855   if(IntrStatus.SOF)
// 1856   {
// 1857     USB_ISTR = ~bmSOFM;
// 1858     if(UsbUserFun[UsbUserSofEvent] != NULL)
// 1859     {
// 1860       UsbUserFun[UsbUserSofEvent]((void *)NULL);
// 1861     }
// 1862   }
// 1863 #endif
// 1864 
// 1865   // Wake-up interrupt
// 1866   if(IntrStatus.WKUP)
??USB_ISR_1:
        LSLS     R0,R6,#+19
        BPL      ??USB_ISR_2
// 1867   {
// 1868     USB_ISTR = ~bmWKUPM;
        LDR      R0,??USB_ISR_0   ;; 0xffffa844
        LDR      R1,??USB_ISR_0+0xC  ;; 0xefff
        STRH     R1,[R0, #+0]
// 1869     USB_Resume(USB_RESUME_WAKE_UP_EVENT);
        MOVS     R0,#+2
        BL       USB_Resume
// 1870     UsbDevSuspendCallback(FALSE);
        MOVS     R0,#+0
        BL       UsbDevSuspendCallback
// 1871   }
// 1872 
// 1873   // Device suspend
// 1874   if(IntrStatus.SUSP)
??USB_ISR_2:
        LSLS     R0,R6,#+20
        BPL      ??USB_ISR_3
// 1875   {
// 1876     USB_Suspend();
        BL       USB_Suspend
// 1877     USB_ISTR = ~bmSUSPM;
        LDR      R0,??USB_ISR_0   ;; 0xffffa844
        LDR      R1,??USB_ISR_0+0x10  ;; 0xf7ff
        STRH     R1,[R0, #+0]
// 1878     UsbDevSuspendCallback(TRUE);
        MOVS     R0,#+1
        BL       UsbDevSuspendCallback
// 1879   }
// 1880 
// 1881   // Expected Frame interrupt
// 1882   if(IntrStatus.ESOF)
??USB_ISR_3:
        LSLS     R0,R6,#+23
        BPL      ??USB_ISR_4
// 1883   {
// 1884     USB_ISTR = ~bmESOFM;
        LDR      R0,??USB_ISR_0   ;; 0xffffa844
        LDR      R1,??USB_ISR_0+0x14  ;; 0xfeff
        STRH     R1,[R0, #+0]
// 1885     USB_Resume(USB_RESUME_SOF_EVENT);
        MOVS     R0,#+0
        BL       USB_Resume
// 1886   }
// 1887 
// 1888   // Low priority EP interrupt
// 1889   if(IntrStatus.CTR)
??USB_ISR_4:
        LSLS     R0,R6,#+16
        BPL      ??USB_ISR_5
// 1890   {
// 1891     // find corresponding EP
// 1892     pInt32U pReg = (pInt32U)&USB_EP0R;
        LDR      R0,??USB_ISR_0+0x18  ;; 0xffffa800
        MOVS     R7,R0
// 1893     pReg += IntrStatus.EP_ID;
        LSLS     R0,R6,#+28       ;; ZeroExtS R0,R6,#+28,#+28
        LSRS     R0,R0,#+28
        MOVS     R1,#+4
        MULS     R0,R1,R0
        ADDS     R7,R7,R0
// 1894     EpPhAddr = ((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1);
        MOVS     R0,#+16
        ANDS     R0,R0,R6
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        LDR      R1,[R7, #+0]
        LSLS     R1,R1,#+28       ;; ZeroExtS R1,R1,#+28,#+28
        LSRS     R1,R1,#+28
        LSLS     R1,R1,#+1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        ADDS     R0,R1,R0
        MOVS     R4,R0
// 1895     // The ISO and Double buffered endpoints have own interrupt (HIGH_INTR)
// 1896     if((EpInfo[EpPhAddr].EpType != EP_ISO) &&
// 1897        (EpInfo[EpPhAddr].EpType != EP_BULK_DOUB_BUF))
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+2
        BEQ      ??USB_ISR_5
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        ADDS     R0,R1,R0
        LDRB     R0,[R0, #+12]
        CMP      R0,#+4
        BEQ      ??USB_ISR_5
// 1898     {
// 1899       if(EpPhAddr == CTRL_ENP_OUT)
        CMP      R4,#+0
        BNE      ??USB_ISR_6
// 1900       {
// 1901         // determinate type of packet (only for control EP)
// 1902         SetupPacket = EpCtrlGet_SETUP(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlGet_SETUP
        MOVS     R5,R0
// 1903         EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_RX
// 1904         if(UsbUserFun[UsbEp0Out] != NULL)
        LDR      R0,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??USB_ISR_5
// 1905         {
// 1906           // call appropriate callback function
// 1907           if(SetupPacket)
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+0
        BEQ      ??USB_ISR_7
// 1908           {
// 1909             UsbUserFun[UsbEp0Out]((void *)UsbSetupPacket);
        MOVS     R0,#+0
        LDR      R1,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        B        ??USB_ISR_5
// 1910           }
// 1911           else
// 1912           {
// 1913             UsbUserFun[UsbEp0Out]((void *)UsbDataOutPacket);
??USB_ISR_7:
        MOVS     R0,#+1
        LDR      R1,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        B        ??USB_ISR_5
// 1914           }
// 1915         }
// 1916       }
// 1917       else if (EpPhAddr & 1)
??USB_ISR_6:
        MOVS     R0,R4
        LSLS     R0,R0,#+31
        BPL      ??USB_ISR_8
// 1918       {
// 1919         // OUT EP
// 1920         EpCtrlClr_CTR_TX(EpInfo[EpPhAddr].pEpCtrl);
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_TX
// 1921         if(UsbUserFun[EpPhAddr] != NULL)
        MOVS     R0,#+4
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BEQ      ??USB_ISR_5
// 1922         {
// 1923           UsbUserFun[EpPhAddr]((void *)UsbDataInPacket);
        MOVS     R0,#+2
        MOVS     R1,#+4
        MULS     R1,R4,R1
        LDR      R2,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R1,[R2, R1]
        BL       __iar_via_R1
        B        ??USB_ISR_5
// 1924         }
// 1925       }
// 1926       else
// 1927       {
// 1928         // IN EP
// 1929         EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
??USB_ISR_8:
        MOVS     R0,#+20
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x1C  ;; EpInfo
        LDR      R0,[R1, R0]
        BL       EpCtrlClr_CTR_RX
// 1930         if(UsbUserFun[EpPhAddr] != NULL)
        MOVS     R0,#+4
        MULS     R0,R4,R0
        LDR      R1,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R0,[R1, R0]
        CMP      R0,#+0
        BEQ      ??USB_ISR_5
// 1931         {
// 1932           UsbUserFun[EpPhAddr]((void *)UsbDataOutPacket);
        MOVS     R0,#+1
        MOVS     R1,#+4
        MULS     R1,R4,R1
        LDR      R2,??USB_ISR_0+0x20  ;; UsbUserFun
        LDR      R1,[R2, R1]
        BL       __iar_via_R1
// 1933         }
// 1934       }
// 1935     }
// 1936   }
// 1937 }
??USB_ISR_5:
        POP      {R3-R7}
        POP      {R0}
        BX       R0               ;; return
        DATA
??USB_ISR_0:
        DC32     0xffffa844
        DC32     0xffffa840
        DC32     0xfbff
        DC32     0xefff
        DC32     0xf7ff
        DC32     0xfeff
        DC32     0xffffa800
        DC32     EpInfo
        DC32     UsbUserFun
        CFI EndBlock cfiBlock65
        REQUIRE _A_USB_EP0R
        REQUIRE _A_USB_CNTR
        REQUIRE _A_USB_ISTR
// 1938 
// 1939 /*************************************************************************
// 1940  * Function Name: UsbCtrlEp
// 1941  * Parameters:  void * pArg
// 1942  *
// 1943  * Return: none
// 1944  *
// 1945  * Description: USB Ctrl EP Callback
// 1946  *
// 1947  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock66 Using cfiCommon0
        CFI Function UsbCtrlEp
        THUMB
// 1948 static inline
// 1949 void UsbCtrlEp (USB_PacketType_t Packet)
// 1950 {
UsbCtrlEp:
        PUSH     {R0,R1,R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+16
        MOVS     R4,R0
// 1951 Int32U CurrCount;
// 1952 
// 1953   if(Packet == UsbSetupPacket)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R4,#+0
        BEQ      .+4
        B        ??UsbCtrlEp_0
// 1954   {
// 1955     // Setup packet
// 1956     UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+2
        STRB     R1,[R0, #+0]
// 1957 
// 1958     UsbEp0Ctrl.pData   = EpCtrlDataBuf;
        LDR      R0,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,??UsbCtrlEp_1+0x8  ;; EpCtrlDataBuf
        STR      R1,[R0, #+0]
// 1959     UsbEp0Ctrl.Counter = Ep0MaxSize;
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        MOVS     R1,#+8
        STR      R1,[R0, #+0]
// 1960     USB_EpRead(CTRL_ENP_OUT,EpCtrlDataBuf,&UsbEp0Ctrl.Counter);
        LDR      R2,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R1,??UsbCtrlEp_1+0x8  ;; EpCtrlDataBuf
        MOVS     R0,#+0
        BL       USB_EpRead
// 1961 
// 1962     // Copy new setup packet int setup buffer
// 1963     memcpy((Int8U *)&UsbEp0SetupPacket,EpCtrlDataBuf,sizeof(UsbEp0SetupPacket));
        MOVS     R2,#+8
        LDR      R1,??UsbCtrlEp_1+0x8  ;; EpCtrlDataBuf
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        BL       memcpy
// 1964     if(UsbEp0Ctrl.Counter != sizeof(UsbSetupPacket_t))
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        CMP      R0,#+8
        BEQ      ??UsbCtrlEp_2
// 1965     {
// 1966       UsbEp0SetupPacket.mRequestType.Type = UsbTypeReserved;
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+96
        ORRS     R1,R1,R0
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_3
// 1967     }
// 1968     else
// 1969     {
// 1970       // validate OUT/IN EP
// 1971       USB_EpValidate(CTRL_ENP_OUT,TRUE);
??UsbCtrlEp_2:
        MOVS     R1,#+1
        MOVS     R0,#+0
        BL       USB_EpValidate
// 1972       USB_SetStallEP(CTRL_ENP_IN,FALSE);
        MOVS     R1,#+0
        MOVS     R0,#+1
        BL       USB_SetStallEP
// 1973     }
// 1974 
// 1975     switch(UsbEp0SetupPacket.mRequestType.Type)
??UsbCtrlEp_3:
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+96
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_4
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_5
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??UsbCtrlEp_6
        B        ??UsbCtrlEp_7
// 1976     {
// 1977     // Standard
// 1978     case UsbTypeStandart:
// 1979       // Decoding standard request
// 1980       switch (UsbEp0SetupPacket.bRequest)
??UsbCtrlEp_4:
        LDR      R0,??UsbCtrlEp_1+0x14  ;; UsbEp0SetupPacket + 1
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_8
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_9
        SUBS     R0,R0,#+2
        BEQ      ??UsbCtrlEp_10
        SUBS     R0,R0,#+2
        BEQ      ??UsbCtrlEp_11
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_12
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_13
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_14
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_15
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_16
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_17
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_18
        B        ??UsbCtrlEp_19
// 1981       {
// 1982       case GET_STATUS:
// 1983         UsbGetStatus();
??UsbCtrlEp_8:
        BL       UsbGetStatus
        B        ??UsbCtrlEp_0
// 1984         break;
// 1985       case CLEAR_FEATURE:
// 1986         UsbClearFeature();
??UsbCtrlEp_9:
        BL       UsbClearFeature
        B        ??UsbCtrlEp_0
// 1987         break;
// 1988       case SET_FEATURE:
// 1989         UsbSetFeature();
??UsbCtrlEp_10:
        BL       UsbSetFeature
        B        ??UsbCtrlEp_0
// 1990         break;
// 1991       case SET_ADDRESS:
// 1992         UsbSetAddress();
??UsbCtrlEp_11:
        BL       UsbSetAddress
        B        ??UsbCtrlEp_0
// 1993         break;
// 1994       case GET_DESCRIPTOR:
// 1995         if(UsbEp0SetupPacket.mRequestType.Recipient == UsbRecipientDevice)
??UsbCtrlEp_12:
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+27
        BNE      ??UsbCtrlEp_20
// 1996         {
// 1997           UsbGetDescriptor();
        BL       UsbGetDescriptor
        B        ??UsbCtrlEp_0
// 1998         }
// 1999         // Only get descriptor for device is standard request
// 2000         else if ((UsbEp0SetupPacket.mRequestType.Dir == UsbDevice2Host) &&
// 2001                  (UsbUserFun[UsbUserGetDescriptor] != NULL) &&
// 2002                  ((Int32U)UsbUserFun[UsbUserGetDescriptor](&UsbEp0Ctrl) == UsbPass))
??UsbCtrlEp_20:
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24
        BPL      ??UsbCtrlEp_21
        LDR      R0,??UsbCtrlEp_1+0x18  ;; UsbUserFun + 148
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_21
        LDR      R0,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,??UsbCtrlEp_1+0x18  ;; UsbUserFun + 148
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_21
// 2003         {
// 2004           if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        CMP      R0,R1
        BCC      ??UsbCtrlEp_23
// 2005           {
// 2006             UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        STR      R1,[R0, #+0]
// 2007             UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_24
// 2008           }
// 2009           else
// 2010           {
// 2011             UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
??UsbCtrlEp_23:
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2012           }
// 2013           UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
??UsbCtrlEp_24:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2014         }
// 2015         else
// 2016         {
// 2017           UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_21:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2018         }
// 2019         break;
// 2020       case SET_DESCRIPTOR:
// 2021         // Optional (only for configuration and string descriptors)
// 2022         UsbSetDescriptor();
??UsbCtrlEp_13:
        BL       UsbSetDescriptor
        B        ??UsbCtrlEp_0
// 2023         break;
// 2024       case GET_CONFIGURATION:
// 2025         UsbGetConfiguration();
??UsbCtrlEp_14:
        BL       UsbGetConfiguration
        B        ??UsbCtrlEp_0
// 2026         break;
// 2027       case SET_CONFIGURATION:
// 2028         UsbSetConfiguration();
??UsbCtrlEp_15:
        BL       UsbSetConfiguration
        B        ??UsbCtrlEp_0
// 2029         break;
// 2030       case GET_INTERFACE:
// 2031         UsbGetInterface();
??UsbCtrlEp_16:
        BL       UsbGetInterface
        B        ??UsbCtrlEp_0
// 2032         break;
// 2033       case SET_INTERFACE:
// 2034         UsbSetInterface();
??UsbCtrlEp_17:
        BL       UsbSetInterface
        B        ??UsbCtrlEp_0
// 2035         break;
// 2036       case SYNCH_FRAME:
// 2037         UsbSynchFrame();
??UsbCtrlEp_18:
        BL       UsbSynchFrame
        B        ??UsbCtrlEp_0
// 2038         break;
// 2039       default:
// 2040         UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_19:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2041       }
// 2042       break;
// 2043     // Class
// 2044     case UsbTypeClass:
// 2045       if(UsbUserFun[UsbUserClass] != NULL &&
// 2046          ((Int32U)UsbUserFun[UsbUserClass](&UsbEp0Ctrl) == UsbPass))
??UsbCtrlEp_5:
        LDR      R0,??UsbCtrlEp_22+0x8  ;; UsbUserFun + 140
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_25
        LDR      R0,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,??UsbCtrlEp_22+0x8  ;; UsbUserFun + 140
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_25
// 2047       {
// 2048         if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        CMP      R0,R1
        BCC      ??UsbCtrlEp_26
// 2049         {
// 2050           UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        STR      R1,[R0, #+0]
// 2051           UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_27
// 2052         }
// 2053         else
// 2054         {
// 2055           UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
??UsbCtrlEp_26:
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2056         }
// 2057         UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
??UsbCtrlEp_27:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2058       }
// 2059       else
// 2060       {
// 2061         UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_25:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2062       }
// 2063       break;
// 2064     // Vendor
// 2065     case UsbTypeVendor:
// 2066       if(UsbUserFun[UsbUserVendor] != NULL &&
// 2067         ((Int32U)UsbUserFun[UsbUserVendor](&UsbEp0Ctrl) == UsbPass))
??UsbCtrlEp_6:
        LDR      R0,??UsbCtrlEp_22+0xC  ;; UsbUserFun + 144
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_28
        LDR      R0,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,??UsbCtrlEp_22+0xC  ;; UsbUserFun + 144
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_28
// 2068       {
// 2069         if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        CMP      R0,R1
        BCC      ??UsbCtrlEp_29
// 2070         {
// 2071           UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R1,??UsbCtrlEp_22  ;; UsbEp0SetupPacket + 6
        LDRH     R1,[R1, #+0]
        STR      R1,[R0, #+0]
// 2072           UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_30
// 2073         }
// 2074         else
// 2075         {
// 2076           UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
??UsbCtrlEp_29:
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2077         }
// 2078         UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
??UsbCtrlEp_30:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2079       }
// 2080       else
// 2081       {
// 2082         UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_28:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_0
// 2083       }
// 2084       break;
// 2085     // Other
// 2086     default:
// 2087       UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_7:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2088     }
// 2089   }
// 2090 
// 2091   switch (UsbEp0Ctrl.EpStatus.Status)
??UsbCtrlEp_0:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        LDRB     R0,[R0, #+0]
        SUBS     R0,R0,#+3
        BEQ      ??UsbCtrlEp_31
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??UsbCtrlEp_32
        B        ??UsbCtrlEp_33
// 2092   {
// 2093   case UsbDataPhase:
// 2094     if (UsbEp0SetupPacket.mRequestType.Dir == UsbHost2Device)
??UsbCtrlEp_31:
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24
        BPL      .+4
        B        ??UsbCtrlEp_34
// 2095     {
// 2096       if (Packet == UsbDataOutPacket)
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R4,#+1
        BEQ      .+4
        B        ??UsbCtrlEp_35
// 2097       {
// 2098         // Data receiving
// 2099         CurrCount = Ep0MaxSize;
        MOVS     R0,#+8
        STR      R0,[SP, #+0]
// 2100         USB_EpRead(CTRL_ENP_OUT,UsbEp0Ctrl.pData,&CurrCount);
        MOV      R2,SP
        LDR      R0,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,[R0, #+0]
        MOVS     R0,#+0
        BL       USB_EpRead
// 2101         UsbEp0Ctrl.pData += CurrCount;
        LDR      R0,[SP, #+0]
        LDR      R1,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        LDR      R1,[R1, #+0]
        ADDS     R0,R1,R0
        LDR      R1,??UsbCtrlEp_1+0x4  ;; UsbEp0Ctrl
        STR      R0,[R1, #+0]
// 2102         UsbEp0Ctrl.Counter -= CurrCount;
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R1,[SP, #+0]
        SUBS     R0,R0,R1
        LDR      R1,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        STR      R0,[R1, #+0]
// 2103         // Receiving of data complete when all data was received or
// 2104         // when host send short packet
// 2105         if ((UsbEp0Ctrl.Counter == 0) ||
// 2106             (CurrCount < Ep0MaxSize))
        LDR      R0,??UsbCtrlEp_1+0xC  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_36
        LDR      R0,[SP, #+0]
        CMP      R0,#+8
        BCC      .+4
        B        ??UsbCtrlEp_37
// 2107         {
// 2108           // Find appropriate callback function depending of current request
// 2109           // after data was received
// 2110           switch (UsbEp0SetupPacket.mRequestType.Type)
??UsbCtrlEp_36:
        LDR      R0,??UsbCtrlEp_1+0x10  ;; UsbEp0SetupPacket
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+96
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        LSRS     R0,R0,#+5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_38
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_39
        SUBS     R0,R0,#+1
        BEQ      ??UsbCtrlEp_40
        B        ??UsbCtrlEp_41
// 2111           {
// 2112           case UsbTypeStandart:
// 2113             if ((UsbCoreT9Fun != NULL) &&
// 2114                 ((Int32U)UsbCoreT9Fun(NULL) == UsbPass))
??UsbCtrlEp_38:
        LDR      R0,??UsbCtrlEp_22+0x10  ;; UsbCoreT9Fun
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_42
        MOVS     R0,#+0
        LDR      R1,??UsbCtrlEp_22+0x10  ;; UsbCoreT9Fun
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_42
// 2115             {
// 2116               UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_43
// 2117             }
// 2118             else
// 2119             {
// 2120             // Stall EP if callback function isn't set, but host
// 2121             // send this type request or when callback function
// 2122             // return an error.
// 2123               UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_42:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2124             }
// 2125             UsbCoreT9Fun = NULL;
??UsbCtrlEp_43:
        LDR      R0,??UsbCtrlEp_22+0x10  ;; UsbCoreT9Fun
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        B        ??UsbCtrlEp_44
// 2126             break;
// 2127           case UsbTypeClass:
// 2128             if ((UsbUserFun[UsbClassEp0OutPacket] != NULL) &&
// 2129                 ((Int32U)UsbUserFun[UsbClassEp0OutPacket](NULL) == UsbPass))
??UsbCtrlEp_39:
        LDR      R0,??UsbCtrlEp_22+0x14  ;; UsbUserFun + 128
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_45
        MOVS     R0,#+0
        LDR      R1,??UsbCtrlEp_22+0x14  ;; UsbUserFun + 128
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_45
// 2130             {
// 2131               UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_44
// 2132             }
// 2133             else
// 2134             {
// 2135               UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_45:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_44
// 2136             }
// 2137             break;
// 2138           case UsbTypeVendor:
// 2139             if ((UsbUserFun[UsbVendorEp0OutPacket] != NULL) &&
// 2140                 ((Int32U)UsbUserFun[UsbVendorEp0OutPacket](NULL) == UsbPass))
??UsbCtrlEp_40:
        LDR      R0,??UsbCtrlEp_22+0x18  ;; UsbUserFun + 132
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_46
        MOVS     R0,#+0
        LDR      R1,??UsbCtrlEp_22+0x18  ;; UsbUserFun + 132
        LDR      R1,[R1, #+0]
        BL       __iar_via_R1
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_46
// 2141             {
// 2142               UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_44
// 2143             }
// 2144             else
// 2145             {
// 2146               UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_46:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_44
// 2147             }
// 2148             break;
// 2149           default:
// 2150             UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
??UsbCtrlEp_41:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
// 2151           }
// 2152           if(UsbEp0Ctrl.EpStatus.Status == UsbStatusPhase)
??UsbCtrlEp_44:
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        LDRB     R0,[R0, #+0]
        CMP      R0,#+4
        BEQ      .+4
        B        ??UsbCtrlEp_37
// 2153           {
// 2154             CurrCount = 0;
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
// 2155             USB_EpWrite(CTRL_ENP_IN,NULL,&CurrCount);
        MOV      R2,SP
        MOVS     R1,#+0
        MOVS     R0,#+1
        BL       USB_EpWrite
// 2156             UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_1  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_37
        DATA
??UsbCtrlEp_1:
        DC32     UsbEp0Ctrl + 8
        DC32     UsbEp0Ctrl
        DC32     EpCtrlDataBuf
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0SetupPacket
        DC32     UsbEp0SetupPacket + 1
        DC32     UsbUserFun + 148
        THUMB
// 2157           }
// 2158         }
// 2159       }
// 2160       else if (UsbEp0Ctrl.Counter == 0)
??UsbCtrlEp_35:
        LDR      R0,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_37
// 2161       {
// 2162         CurrCount = 0;
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
// 2163         USB_EpWrite(CTRL_ENP_IN,NULL,&CurrCount);
        MOV      R2,SP
        MOVS     R1,#+0
        MOVS     R0,#+1
        BL       USB_EpWrite
// 2164         UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_22+0x20  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_37
// 2165       }
// 2166     }
// 2167     else
// 2168     {
// 2169        // Data transmit
// 2170       if (Packet == UsbDataOutPacket)
??UsbCtrlEp_34:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R4,#+1
        BNE      ??UsbCtrlEp_47
// 2171       {
// 2172         USB_EpValidate(CTRL_ENP_OUT,TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+0
        BL       USB_EpValidate
// 2173         UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_22+0x20  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
// 2174         break;
        B        ??UsbCtrlEp_37
// 2175       }
// 2176       CurrCount = Ep0MaxSize;
??UsbCtrlEp_47:
        MOVS     R0,#+8
        STR      R0,[SP, #+0]
// 2177       if ((UsbEp0Ctrl.Counter == 0) && UsbEp0Ctrl.EpStatus.NoZeroLength)
        LDR      R0,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??UsbCtrlEp_48
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_48
// 2178       {
// 2179         UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        LDR      R0,??UsbCtrlEp_22+0x20  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+4
        STRB     R1,[R0, #+0]
// 2180         break;
        B        ??UsbCtrlEp_37
// 2181       }
// 2182       else if(UsbEp0Ctrl.Counter < Ep0MaxSize)
??UsbCtrlEp_48:
        LDR      R0,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        CMP      R0,#+8
        BCS      ??UsbCtrlEp_49
// 2183       {
// 2184         UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        LDR      R0,??UsbCtrlEp_22+0x4  ;; UsbEp0Ctrl + 9
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
// 2185         CurrCount = UsbEp0Ctrl.Counter;
        LDR      R0,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        STR      R0,[SP, #+0]
// 2186       }
// 2187       USB_EpWrite(CTRL_ENP_IN,UsbEp0Ctrl.pData,&CurrCount);
??UsbCtrlEp_49:
        MOV      R2,SP
        LDR      R0,??UsbCtrlEp_22+0x24  ;; UsbEp0Ctrl
        LDR      R1,[R0, #+0]
        MOVS     R0,#+1
        BL       USB_EpWrite
// 2188       UsbEp0Ctrl.Counter -= CurrCount;
        LDR      R0,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        LDR      R0,[R0, #+0]
        LDR      R1,[SP, #+0]
        SUBS     R0,R0,R1
        LDR      R1,??UsbCtrlEp_22+0x1C  ;; UsbEp0Ctrl + 4
        STR      R0,[R1, #+0]
// 2189       UsbEp0Ctrl.pData += CurrCount;
        LDR      R0,[SP, #+0]
        LDR      R1,??UsbCtrlEp_22+0x24  ;; UsbEp0Ctrl
        LDR      R1,[R1, #+0]
        ADDS     R0,R1,R0
        LDR      R1,??UsbCtrlEp_22+0x24  ;; UsbEp0Ctrl
        STR      R0,[R1, #+0]
        B        ??UsbCtrlEp_37
// 2190     }
// 2191     break;
// 2192   case UsbStatusPhase:
// 2193     if (UsbEp0SetupPacket.bRequest == SET_ADDRESS)
??UsbCtrlEp_32:
        LDR      R0,??UsbCtrlEp_22+0x28  ;; UsbEp0SetupPacket + 1
        LDRB     R0,[R0, #+0]
        CMP      R0,#+5
        BNE      ??UsbCtrlEp_50
// 2194     {
// 2195       // Device address is set after status phase of set address request
// 2196       UsbDevCtrl.DevAdd = UsbEp0SetupPacket.wValue.Lo;
        LDR      R0,??UsbCtrlEp_22+0x2C  ;; UsbDevCtrl + 16
        LDR      R1,??UsbCtrlEp_22+0x30  ;; UsbEp0SetupPacket + 2
        LDRB     R1,[R1, #+0]
        STR      R1,[R0, #+0]
// 2197       USB_SetAdd(UsbDevCtrl.DevAdd);
        LDR      R0,??UsbCtrlEp_22+0x2C  ;; UsbDevCtrl + 16
        LDR      R0,[R0, #+0]
        BL       USB_SetAdd
// 2198       if(UsbDevCtrl.DevAdd)
        LDR      R0,??UsbCtrlEp_22+0x2C  ;; UsbDevCtrl + 16
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??UsbCtrlEp_51
// 2199       {
// 2200         UsbSetDevState(UsbDevStatusAddress);
        MOVS     R0,#+4
        BL       UsbSetDevState
        B        ??UsbCtrlEp_50
// 2201       }
// 2202       else
// 2203       {
// 2204         // when address is 0 put device int configuration state
// 2205         UsbSetDevState(UsbDevStatusDefault);
??UsbCtrlEp_51:
        MOVS     R0,#+3
        BL       UsbSetDevState
// 2206       }
// 2207     }
// 2208     if (Packet == UsbDataOutPacket)
??UsbCtrlEp_50:
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R4,#+1
        BNE      ??UsbCtrlEp_52
// 2209     {
// 2210       USB_EpValidate(CTRL_ENP_OUT,TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+0
        BL       USB_EpValidate
// 2211     }
// 2212     UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
??UsbCtrlEp_52:
        LDR      R0,??UsbCtrlEp_22+0x20  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+2
        STRB     R1,[R0, #+0]
        B        ??UsbCtrlEp_37
// 2213     break;
// 2214   default:
// 2215     // Error
// 2216     USB_SetStallEP(CTRL_ENP_IN,TRUE);
??UsbCtrlEp_33:
        MOVS     R1,#+1
        MOVS     R0,#+1
        BL       USB_SetStallEP
// 2217     USB_SetStallEP(CTRL_ENP_OUT,TRUE);
        MOVS     R1,#+1
        MOVS     R0,#+0
        BL       USB_SetStallEP
// 2218     UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
        LDR      R0,??UsbCtrlEp_22+0x20  ;; UsbEp0Ctrl + 8
        MOVS     R1,#+2
        STRB     R1,[R0, #+0]
// 2219   }
// 2220 }
??UsbCtrlEp_37:
        POP      {R2-R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??UsbCtrlEp_22:
        DC32     UsbEp0SetupPacket + 6
        DC32     UsbEp0Ctrl + 9
        DC32     UsbUserFun + 140
        DC32     UsbUserFun + 144
        DC32     UsbCoreT9Fun
        DC32     UsbUserFun + 128
        DC32     UsbUserFun + 132
        DC32     UsbEp0Ctrl + 4
        DC32     UsbEp0Ctrl + 8
        DC32     UsbEp0Ctrl
        DC32     UsbEp0SetupPacket + 1
        DC32     UsbDevCtrl + 16
        DC32     UsbEp0SetupPacket + 2
        CFI EndBlock cfiBlock66

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock67 Using cfiCommon0
        CFI Function EpCtrlGet_STAT_TX
        THUMB
EpCtrlGet_STAT_TX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R0,R1
        MOVS     R1,#+48
        ANDS     R1,R1,R0
        MOVS     R0,R1
        LSRS     R0,R0,#+4
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock67

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock68 Using cfiCommon0
        CFI Function EpCtrlGet_DTOG_TX
        THUMB
EpCtrlGet_DTOG_TX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+64
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock68

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock69 Using cfiCommon0
        CFI Function EpCtrlGet_CTR_TX
        THUMB
EpCtrlGet_CTR_TX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock69

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock70 Using cfiCommon0
        CFI Function EpCtrlGet_SETUP
        THUMB
EpCtrlGet_SETUP:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R2,#+128
        LSLS     R2,R2,#+4        ;; #+2048
        ANDS     R2,R2,R1
        MOVS     R0,R2
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock70

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock71 Using cfiCommon0
        CFI Function EpCtrlGet_STAT_RX
        THUMB
EpCtrlGet_STAT_RX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R1,[SP, #+4]
        LDR      R1,[R1, #+0]
        MOVS     R2,#+192
        LSLS     R2,R2,#+6        ;; #+12288
        ANDS     R2,R2,R1
        MOVS     R0,R2
        LSRS     R0,R0,#+12
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock71

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock72 Using cfiCommon0
        CFI Function EpCtrlGet_DTOG_RX
        THUMB
EpCtrlGet_DTOG_RX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+7        ;; #+16384
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock72

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock73 Using cfiCommon0
        CFI Function EpCtrlGet_CTR_RX
        THUMB
EpCtrlGet_CTR_RX:
        PUSH     {R0}
        CFI CFA R13+4
        SUB      SP,SP,#+4
        CFI CFA R13+8
        LDR      R0,[SP, #+4]
        LDR      R0,[R0, #+0]
        MOVS     R1,#+128
        LSLS     R1,R1,#+8        ;; #+32768
        ANDS     R1,R1,R0
        SUBS     R0,R1,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        POP      {R2,R3}
        CFI CFA R13+0
        BX       LR               ;; return
        CFI EndBlock cfiBlock73

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock74 Using cfiCommon0
        CFI NoFunction
        THUMB
__iar_via_R1:
        BX       R1
        CFI EndBlock cfiBlock74

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock75 Using cfiCommon0
        CFI NoFunction
        THUMB
__iar_via_R6:
        BX       R6
        CFI EndBlock cfiBlock75

        SECTION `.rodata`:CONST:NOROOT(2)
`?<Constant {0}>`:
        DATA
        DC16 0
        DC8 0, 0

        END
// 2221 
// 
//  1 072 bytes in section .bss
//      4 bytes in section .rodata
// 14 702 bytes in section .text
//  1 024 bytes in section USB_PACKET_MEMORY
// 
// 14 484 bytes of CODE  memory (+ 218 bytes shared)
//      4 bytes of CONST memory
//  2 096 bytes of DATA  memory
//
//Errors: none
//Warnings: none
