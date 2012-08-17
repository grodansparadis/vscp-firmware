///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:38 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\main.c     /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\main.c     /
//                    -lC D:\development\gediminas\CAN4USB\project\RELEASE\Li /
//                    st\ -lA D:\development\gediminas\CAN4USB\project\RELEAS /
//                    E\List\ -o D:\development\gediminas\CAN4USB\project\REL /
//                    EASE\Obj\ --no_cse --no_unroll --no_inline              /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\m /
//                    ain.s                                                   /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME main

        RTMODEL "__dlib_file_descriptor", "0"

        EXTERN CAN_EnterInitMode
        EXTERN CAN_Init
        EXTERN CAN_InvalidateAllMsgObj
        EXTERN CAN_LeaveInitMode
        EXTERN CAN_LeaveTestMode
        EXTERN CAN_ReceiveMessage
        EXTERN CAN_ReleaseTxMessage
        EXTERN CAN_SendMessage
        EXTERN CAN_SetBitrate
        EXTERN CAN_SetRxMsgObj
        EXTERN CAN_SetTxMsgObj
        EXTERN CAN_WaitEndOfTx
        EXTERN CFG_FLASHBurstConfig
        EXTERN EIC_DeInit
        EXTERN EIC_IRQCmd
        EXTERN EIC_IRQInit
        EXTERN GPIO_Init
        EXTERN GPIO_StructInit
        EXTERN GPIO_WriteBit
        EXTERN MRCC_CKSYSConfig
        EXTERN MRCC_CKTIMConfig
        EXTERN MRCC_DeInit
        EXTERN MRCC_GetClocksStatus
        EXTERN MRCC_HCLKConfig
        EXTERN MRCC_IOVoltageRangeConfig
        EXTERN MRCC_PCLKConfig
        EXTERN MRCC_PeripheralClockConfig
        EXTERN MRCC_PeripheralSWResetConfig
        EXTERN MRCC_WaitForOSC4MStartUp
        EXTERN TIM_ClearFlag
        EXTERN TIM_ClearITPendingBit
        EXTERN TIM_Cmd
        EXTERN TIM_ITConfig
        EXTERN TIM_Init
        EXTERN USB_ConnectRes
        EXTERN USB_Init
        EXTERN UsbCdcConfigure
        EXTERN UsbCdcInit
        EXTERN UsbCdcIsCdcConfigure
        EXTERN UsbCdcRead
        EXTERN UsbCdcWrite
        EXTERN __aeabi_idivmod
        EXTERN __aeabi_memcpy
        EXTERN __aeabi_uidivmod
        EXTERN delay_ms
        EXTERN getElapsedSysTICs
        EXTERN getSysTICs
        EXTERN initTimebase
        EXTERN vs_feed
        EXTERN vs_getFrame
        EXTERN vs_sendAck
        EXTERN vs_sendFrame
        EXTERN vs_sendNack

        PUBLIC CANAdapter_bOpen
        PUBLIC CANAdapter_bSendErrorFrames
        PUBLIC CANAdapter_modeOpen
        PUBLIC CANAdapter_stateChange
        PUBLIC CAN_Com_IRQ
        PUBLIC CAN_Com_Polling
        PUBLIC CAN_ISR
        PUBLIC Dly100us
        PUBLIC DlyCount
        PUBLIC IO_Init
        PUBLIC InitClock
        PUBLIC InitDlyTimer
        PUBLIC RxCanMsg
        PUBLIC Tim0Handler
        PUBLIC TxCanMsg
        PUBLIC UsbCdcWriteTimeout
        PUBWEAK __iar_enable_interrupt
        PUBLIC bBusOff
        PUBLIC bBusWarn
        PUBLIC bErrorPassive
        PUBLIC blink
        PUBLIC blink_state
        PUBLIC blink_state_time
        PUBLIC blink_type
        PUBLIC canstatus
        PUBLIC cntBusOff
        PUBLIC cntBusWarnings
        PUBLIC cntInFrames
        PUBLIC cntOutFrames
        PUBLIC cntReceiveOverruns
        PUBLIC cntReceivedData
        PUBLIC cntReceivedFrames
        PUBLIC cntSentData
        PUBLIC cntSentFrames
        PUBLIC cntTransmitOverruns
        PUBLIC init_CAN
        PUBLIC inputFrames
        PUBLIC lastAck
        PUBLIC ledOff
        PUBLIC ledOn
        PUBLIC main
        PUBWEAK memcpy
        PUBLIC outputFrames
        PUBLIC posInFetch
        PUBLIC posInPut
        PUBLIC posOutFetch
        PUBLIC posOutPut
        PUBLIC sendMessage
        PUBLIC serial_out_buf
        PUBLIC serial_out_pos
        PUBLIC setBlinkType
        PUBLIC swReceive
        PUBLIC swReceiveFrames
        PUBLIC vs_sendBuffer
        PUBLIC vs_sendSerialByte
        
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
        
        
        CFI Common cfiCommon1 Using cfiNames0
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
        CFI EndCommon cfiCommon1
        
// D:\development\gediminas\CAN4USB\project\app\main.c
//    1 /*************************************************************************
//    2  *
//    3  *    Used with ICCARM and AARM.
//    4  *
//    5  *    (c) Copyright IAR Systems 2006
//    6  *
//    7  *    File name   : main.c
//    8  *    Description : main module
//    9  *
//   10  *    History :
//   11  *    1. Date        : August 26, 2006
//   12  *       Author      : Stanimir Bonev
//   13  *       Description : Create
//   14  *
//   15  *  This example project shows how to use the IAR Embedded Workbench for ARM
//   16  * to develop code for the IAR STR750-SK evaluation boards. It implements USB
//   17  * CDC (Communication Device Class) device and install it like a Virtual COM
//   18  * port. The UART1 is used for physical implementation of the RS232 port.
//   19  *
//   20  * Jumpers:
//   21  *  PWR_SEL   - depend of power source
//   22  *  VRE       - Absence
//   23  *  BOOT0     - 0
//   24  *  BOOT1     - 0
//   25  *
//   26  *    $Revision: 18265 $
//   27  *
//   28  **************************************************************************/
//   29 #include "includes.h"

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
//   30 #include <inttypes.h>       // Set environment with setenv_vscp.bat in vscp folder
//   31 #include "vscp_compiler.h"
//   32 #include "vscp_projdefs.h"
//   33 #include "75x_conf.h"
//   34 #include "75x_lib.h"
//   35 #include "systime.h"
//   36 #include "delay.h"
//   37 #include "vscp_serial.h"    // Set environment with setenv_vscp.bat in vscp folder
//   38 #include "canusb.h"
//   39 
//   40 #define UART  UART_1
//   41 
//   42 // LED Defines
//   43 #define GREEN   0
//   44 #define RED     1
//   45 
//   46 // buffer for receive messages 

        SECTION `.bss`:DATA:NOROOT(2)
//   47 canmsg RxCanMsg;
RxCanMsg:
        DS8 20
//   48 
//   49 // Serial output buffer

        SECTION `.bss`:DATA:NOROOT(2)
//   50 uint32_t serial_out_pos;
serial_out_pos:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   51 uint8_t serial_out_buf[ SERIAL_OUT_BUFFE_SIZE ];
serial_out_buf:
        DS8 256
//   52 
//   53 // CAN output FIFO -- serial to CAN

        SECTION `.bss`:DATA:NOROOT(1)
//   54 uint16_t cntOutFrames;
cntOutFrames:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(1)
//   55 uint16_t posOutFetch;
posOutFetch:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(1)
//   56 uint16_t posOutPut;
posOutPut:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(2)
//   57 canmsg outputFrames[ MAX_OUT_FRAMES ];
outputFrames:
        DS8 320
//   58 
//   59 // CAN input FIFO -- CAN to serial

        SECTION `.bss`:DATA:NOROOT(1)
//   60 uint16_t cntInFrames;
cntInFrames:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(1)
//   61 uint16_t posInFetch;
posInFetch:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(1)
//   62 uint16_t posInPut;
posInPut:
        DS8 2

        SECTION `.bss`:DATA:NOROOT(2)
//   63 canmsg inputFrames[ MAX_IN_FRAMES ];
inputFrames:
        DS8 320
//   64 
//   65 // Sliding Windows on serial channel

        SECTION `.bss`:DATA:NOROOT(0)
//   66 uint8_t lastAck;  // Sequence # for last ACK'ed frame
lastAck:
        DS8 1
//   67 
//   68 // Statistics

        SECTION `.bss`:DATA:NOROOT(2)
//   69 uint32_t cntSentFrames;
cntSentFrames:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   70 uint32_t cntReceivedFrames;
cntReceivedFrames:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   71 uint32_t cntSentData;
cntSentData:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   72 uint32_t cntReceivedData;
cntReceivedData:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   73 uint32_t cntBusWarnings;
cntBusWarnings:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   74 uint32_t cntBusOff;
cntBusOff:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   75 uint32_t cntReceiveOverruns;  // Lost RX frames
cntReceiveOverruns:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   76 uint32_t cntTransmitOverruns; // Lost TX frames
cntTransmitOverruns:
        DS8 4
//   77 
//   78 // error states

        SECTION `.bss`:DATA:NOROOT(2)
//   79 volatile BOOL bBusOff;
bBusOff:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   80 volatile BOOL bBusWarn;
bBusWarn:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//   81 volatile BOOL bErrorPassive;
bErrorPassive:
        DS8 4
//   82 
//   83 // blink

        SECTION `.bss`:DATA:NOROOT(2)
//   84 uint32_t blink_state_time = 0;
blink_state_time:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(0)
//   85 uint8_t blink_state = 0;
blink_state:
        DS8 1

        SECTION `.bss`:DATA:NOROOT(0)
//   86 uint8_t blink_type = LED_OFF;
blink_type:
        DS8 1
//   87 
//   88 // last read CAN traceiver status

        SECTION `.bss`:DATA:NOROOT(0)
//   89 volatile uint8_t canstatus;
canstatus:
        DS8 1
//   90 
//   91 // Sliding window CAN receive

        SECTION `.bss`:DATA:NOROOT(2)
//   92 vs_swFrameStruct swReceiveFrames[ VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ];
swReceiveFrames:
        DS8 280

        SECTION `.bss`:DATA:NOROOT(2)
//   93 vs_swStruct swReceive;
swReceive:
        DS8 292
//   94 
//   95 // used message object numbers 
//   96 #define CAN_TX_MSGOBJ       0
//   97 #define CAN_RX_MSGOBJ0      1
//   98 
//   99 // CAN Adapter varaible

        SECTION `.bss`:DATA:NOROOT(2)
//  100 BOOL CANAdapter_bOpen = FALSE;              // No open
CANAdapter_bOpen:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(2)
//  101 BOOL CANAdapter_bSendErrorFrames = FALSE;   // No error frames as default
CANAdapter_bSendErrorFrames:
        DS8 4

        SECTION `.bss`:DATA:NOROOT(0)
//  102 uint8_t CANAdapter_modeOpen;
CANAdapter_modeOpen:
        DS8 1

        SECTION `.data`:DATA:NOROOT(0)
//  103 volatile uint8_t CANAdapter_stateChange = TRUE;
CANAdapter_stateChange:
        DATA
        DC8 1
//  104 
//  105 // array of pre-defined transmit messages 

        SECTION `.data`:DATA:NOROOT(2)
//  106 canmsg TxCanMsg[3] = {
TxCanMsg:
        DATA
        DC32 0, 291
        DC8 4, 1, 2, 4, 8, 0, 0, 0, 0, 0, 0, 0
        DC32 0, 1792
        DC8 4, 170, 85, 170, 85, 0, 0, 0, 0, 0, 0, 0
        DC32 1, 305419896
        DC8 8, 16, 17, 18, 19, 20, 21, 22, 23, 0, 0, 0
//  107   { CAN_STD_ID,      0x123, 4, { 0x01, 0x02, 0x04, 0x08 } },
//  108   { CAN_STD_ID,      0x700, 4, { 0xAA, 0x55, 0xAA, 0x55 } },
//  109   { CAN_EXT_ID, 0x12345678, 8, { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 } }
//  110 };
//  111 
//  112 // Prototypes
//  113 void InitClock ( void );
//  114 volatile uint32_t DlyCount;
//  115 void InitDlyTimer( uint32_t IntrPriority );
//  116 void Dly100us( void *arg );
//  117 void IO_Init( void );
//  118 void init_CAN( void );
//  119 void sendMessage( void );
//  120 void CAN_ISR( void );
//  121 void ledOn( uint8_t nled );
//  122 void LedOff( uint8_t nled );
//  123 void setBlinkType( uint8_t type  );
//  124 void blink( void );
//  125 void sendMessage( void );
//  126 Boolean UsbCdcWriteTimeout( uint8_t *Buffer, uint16_t count, uint32_t timeout );
//  127 
//  128 /*************************************************************************
//  129  * Function Name: InitClock
//  130  * Parameters: none
//  131  *
//  132  * Return: none
//  133  *
//  134  * Description: Init MCU clock
//  135  *
//  136  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function InitClock
        THUMB
//  137 void InitClock ( void )
//  138 {
InitClock:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  139   // Clock
//  140   // MRCC system reset(for debug purpose)
//  141   MRCC_DeInit();
        BL       MRCC_DeInit
//  142   
//  143   // Wait for OSC4M start-up
//  144   MRCC_WaitForOSC4MStartUp();
        BL       MRCC_WaitForOSC4MStartUp
//  145   
//  146   // Set HCLK = 64
//  147   MRCC_HCLKConfig( MRCC_CKSYS_Div1 ); // Original div 1  60 MHZ 30
        MOVS     R0,#+0
        BL       MRCC_HCLKConfig
//  148   
//  149   // Set CKTIM = 16 (TB clock etc)
//  150   MRCC_CKTIMConfig( MRCC_HCLK_Div4 ); // Original div 1  60 MHZ 15
        MOVS     R0,#+2
        BL       MRCC_CKTIMConfig
//  151   
//  152   // Set PCLK = 8
//  153   MRCC_PCLKConfig( MRCC_CKTIM_Div2 ); // Original div 2  30 MHz 7.5
        MOVS     R0,#+4
        BL       MRCC_PCLKConfig
//  154   
//  155   // Set CKSYS
//  156   MRCC_CKSYSConfig( MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_16 ); // Original 15
        MOVS     R1,#+0
        MOVS     R0,#+3
        BL       MRCC_CKSYSConfig
//  157   
//  158   // GPIO pins optimized for 3V3 operation
//  159   MRCC_IOVoltageRangeConfig( MRCC_IOVoltageRange_3V3 );
        MOVS     R0,#+128
        LSLS     R0,R0,#+9        ;; #+65536
        BL       MRCC_IOVoltageRangeConfig
//  160 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock1
//  161 

        SECTION `.bss`:DATA:NOROOT(2)
//  162 volatile uint32_t DlyCount;
DlyCount:
        DS8 4
//  163 /*************************************************************************
//  164  * Function Name: Tim0Handler
//  165  * Parameters: none
//  166  *
//  167  * Return: none
//  168  *
//  169  * Description: Timer 0 interrupt handler
//  170  *
//  171  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function Tim0Handler
        THUMB
//  172 void Tim0Handler (void)
//  173 {
Tim0Handler:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  174   if ( DlyCount ) {
        LDR      R0,??DataTable9  ;; DlyCount
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??Tim0Handler_0
//  175     --DlyCount;
        LDR      R0,??DataTable9  ;; DlyCount
        LDR      R0,[R0, #+0]
        SUBS     R0,R0,#+1
        LDR      R1,??DataTable9  ;; DlyCount
        STR      R0,[R1, #+0]
//  176   }
//  177   
//  178   // Clear the Update pending Bit
//  179   TIM_ClearITPendingBit( TIM0, TIM_IT_Update );
??Tim0Handler_0:
        MOVS     R1,#+1
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_ClearITPendingBit
//  180 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock2
//  181 
//  182 /*************************************************************************
//  183  * Function Name: InitDlyTimer
//  184  * Parameters: Int32U IntrPriority
//  185  *
//  186  * Return: none
//  187  *
//  188  * Description: Init Delay Timer (TIM 0)
//  189  *
//  190  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function InitDlyTimer
        THUMB
//  191 void InitDlyTimer( uint32_t IntrPriority )
//  192 {
InitDlyTimer:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        SUB      SP,SP,#+64
        CFI CFA R13+72
        MOVS     R4,R0
//  193   MRCC_ClocksTypeDef MRCC_Clocks;
//  194   TIM_InitTypeDef TIM_InitStructure;
//  195   EIC_IRQInitTypeDef EIC_IRQInitStructure;
//  196 
//  197   // Enable TIM0 clocks
//  198   MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0, ENABLE);
        MOVS     R1,#+1
        MOVS     R0,#+4
        BL       MRCC_PeripheralClockConfig
//  199   
//  200   // Release TIM0 reset
//  201   MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,DISABLE);
        MOVS     R1,#+0
        MOVS     R0,#+4
        BL       MRCC_PeripheralSWResetConfig
//  202 
//  203   // Timer 0
//  204   // TIM Configuration in Output Compare Timing Mode period 100us
//  205   // Calculate counter value
//  206   MRCC_GetClocksStatus(&MRCC_Clocks);
        ADD      R0,SP,#+40
        BL       MRCC_GetClocksStatus
//  207 
//  208   TIM_InitStructure.TIM_Period = ( MRCC_Clocks.CKTIM_Frequency / 10000 );
        LDR      R0,[SP, #+56]
        LDR      R1,??InitDlyTimer_0  ;; 0x2710
        BL       __aeabi_uidivmod
        ADD      R1,SP,#+4
        STRH     R0,[R1, #+10]
//  209   TIM_InitStructure.TIM_Prescaler = 0;
        ADD      R0,SP,#+4
        MOVS     R1,#+0
        STRH     R1,[R0, #+2]
//  210   TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
        MOV      R0,SP
        MOVS     R1,#+1
        STRH     R1,[R0, #+4]
//  211   TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
        ADD      R0,SP,#+4
        MOVS     R1,#+1
        STRH     R1,[R0, #+4]
//  212   TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        ADD      R0,SP,#+4
        MOVS     R1,#+0
        STRH     R1,[R0, #+8]
//  213   TIM_InitStructure.TIM_Channel = TIM_Channel_2;
        ADD      R0,SP,#+4
        MOVS     R1,#+2
        STRH     R1,[R0, #+12]
//  214   TIM_Init( TIM0, &TIM_InitStructure );
        ADD      R1,SP,#+4
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_Init
//  215 
//  216   // Enable and configure the priority of the TIM0 Output compare IRQ Channel
//  217   EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+12
        STRB     R1,[R0, #+0]
//  218   EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
        MOV      R0,SP
        MOVS     R1,R4
        STRB     R1,[R0, #+1]
//  219   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  220   EIC_IRQInit( &EIC_IRQInitStructure );
        MOV      R0,SP
        BL       EIC_IRQInit
//  221 }
        ADD      SP,SP,#+64
        CFI CFA R13+8
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??InitDlyTimer_0:
        DC32     0x2710
        CFI EndBlock cfiBlock3
//  222 
//  223 /*************************************************************************
//  224  * Function Name: Dly100us
//  225  * Parameters: void *arg
//  226  *
//  227  * Return: none
//  228  *
//  229  * Description: Delay 100us * arg
//  230  *
//  231  *************************************************************************/

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function Dly100us
        THUMB
//  232 void Dly100us( void *arg )
//  233 {
Dly100us:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  234   DlyCount = (uint32_t)arg;
        LDR      R0,??DataTable9  ;; DlyCount
        STR      R4,[R0, #+0]
//  235 
//  236   // Clear TIM0 flags
//  237   TIM_ClearFlag( TIM0, TIM_FLAG_Update );
        MOVS     R1,#+1
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_ClearFlag
//  238 
//  239   // Enable TIM0 Update interrupt
//  240   TIM_ITConfig( TIM0, TIM_IT_Update, ENABLE );
        MOVS     R2,#+1
        MOVS     R1,#+1
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_ITConfig
//  241 
//  242   // Enable TIM0 counter
//  243   TIM_Cmd( TIM0, ENABLE );
        MOVS     R1,#+1
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_Cmd
//  244   
//  245   while ( DlyCount > 0 );
??Dly100us_0:
        LDR      R0,??DataTable9  ;; DlyCount
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??Dly100us_0
//  246 
//  247   // Disable TIM0 Update interrupt
//  248   TIM_ITConfig( TIM0, TIM_IT_Update, DISABLE );
        MOVS     R2,#+0
        MOVS     R1,#+1
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_ITConfig
//  249 
//  250   // Disable TIM0 counter
//  251   TIM_Cmd( TIM0, DISABLE );
        MOVS     R1,#+0
        LDR      R0,??DataTable11  ;; 0xffff8c00
        BL       TIM_Cmd
//  252 }
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock4

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable9:
        DC32     DlyCount

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable11:
        DC32     0xffff8c00
//  253 
//  254 
//  255 
//  256 ///////////////////////////////////////////////////////////////////////////////
//  257 // IO_Init
//  258 //
//  259 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function IO_Init
        THUMB
//  260 void IO_Init( void )
//  261 {
IO_Init:
        PUSH     {LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+4
        SUB      SP,SP,#+28
        CFI CFA R13+32
//  262   GPIO_InitTypeDef  GPIO_InitStructure;
//  263   GPIO_InitTypeDef  CAN_RX;
//  264   GPIO_InitTypeDef  CAN_TX;
//  265   
//  266   GPIO_StructInit( &GPIO_InitStructure );
        MOV      R0,SP
        BL       GPIO_StructInit
//  267   GPIO_StructInit( &CAN_TX );
        ADD      R0,SP,#+8
        BL       GPIO_StructInit
//  268   GPIO_StructInit( &CAN_RX );
        ADD      R0,SP,#+16
        BL       GPIO_StructInit
//  269 
//  270   // Enable GPIO clocks + CAN clocks
//  271   MRCC_PeripheralClockConfig( MRCC_Peripheral_GPIO | MRCC_Peripheral_CAN, 
//  272                                 ENABLE );
        MOVS     R1,#+1
        LDR      R0,??IO_Init_0   ;; 0x1010000
        BL       MRCC_PeripheralClockConfig
//  273   
//  274   // Release GPIO reset
//  275   MRCC_PeripheralSWResetConfig( MRCC_Peripheral_GPIO, DISABLE );
        MOVS     R1,#+0
        MOVS     R0,#+128
        LSLS     R0,R0,#+17       ;; #+16777216
        BL       MRCC_PeripheralSWResetConfig
//  276 
//  277   // IO configure 
//  278   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        MOV      R0,SP
        MOVS     R1,#+6
        STRB     R1,[R0, #+4]
//  279   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
        MOVS     R0,#+6
        STR      R0,[SP, #+0]
//  280   GPIO_Init( GPIO0, &GPIO_InitStructure );
        MOV      R1,SP
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_Init
//  281   
//  282   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        MOV      R0,SP
        MOVS     R1,#+6
        STRB     R1,[R0, #+4]
//  283   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
        MOVS     R0,#+1
        STR      R0,[SP, #+0]
//  284   GPIO_Init( GPIO1, &GPIO_InitStructure );
        MOV      R1,SP
        LDR      R0,??IO_Init_0+0x4  ;; 0xffffe440
        BL       GPIO_Init
//  285   
//  286   // CAN TX
//  287   CAN_TX.GPIO_Mode = GPIO_Mode_AF_PP;
        ADD      R0,SP,#+8
        MOVS     R1,#+8
        STRB     R1,[R0, #+4]
//  288   CAN_TX.GPIO_Pin = GPIO_Pin_15;
        MOVS     R0,#+128
        LSLS     R0,R0,#+8        ;; #+32768
        STR      R0,[SP, #+8]
//  289   GPIO_Init( GPIO0, &CAN_TX );
        ADD      R1,SP,#+8
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_Init
//  290 
//  291   // CAN RX
//  292   CAN_RX.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        ADD      R0,SP,#+16
        MOVS     R1,#+2
        STRB     R1,[R0, #+4]
//  293   CAN_RX.GPIO_Pin = GPIO_Pin_14;
        MOVS     R0,#+128
        LSLS     R0,R0,#+7        ;; #+16384
        STR      R0,[SP, #+16]
//  294   GPIO_Init( GPIO0, &CAN_RX );
        ADD      R1,SP,#+16
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_Init
//  295   
//  296   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );  // Turn LED off (Red)
        MOVS     R2,#+0
        MOVS     R1,#+2
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
//  297   GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );    // Turn LED on (Green)
        MOVS     R2,#+1
        MOVS     R1,#+4
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
//  298   GPIO_WriteBit( GPIO1, GPIO_Pin_0, Bit_RESET );  // USB reset  
        MOVS     R2,#+0
        MOVS     R1,#+1
        LDR      R0,??IO_Init_0+0x4  ;; 0xffffe440
        BL       GPIO_WriteBit
//  299 }
        ADD      SP,SP,#+28
        CFI CFA R13+4
        POP      {R0}
        BX       R0               ;; return
        DATA
??IO_Init_0:
        DC32     0x1010000
        DC32     0xffffe440
        CFI EndBlock cfiBlock5
//  300 
//  301 ///////////////////////////////////////////////////////////////////////////////
//  302 // ledOn
//  303 //
//  304 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function ledOn
        THUMB
//  305 void ledOn( uint8_t nled )
//  306 {
ledOn:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  307   switch ( nled ) {
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        CMP      R0,#+0
        BEQ      ??ledOn_0
        SUBS     R0,R0,#+1
        BNE      ??ledOn_1
//  308     case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_SET );  
??ledOn_2:
        MOVS     R2,#+1
        MOVS     R1,#+2
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
        B        ??ledOn_1
//  309       break;
//  310     case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );  
??ledOn_0:
        MOVS     R2,#+1
        MOVS     R1,#+4
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
//  311       break;
//  312   }
//  313 }
??ledOn_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock6
//  314 
//  315 ///////////////////////////////////////////////////////////////////////////////
//  316 // ledOff
//  317 //
//  318 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function ledOff
        THUMB
//  319 void ledOff( uint8_t nled )
//  320 {
ledOff:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  321   switch ( nled ) {
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        CMP      R0,#+0
        BEQ      ??ledOff_0
        SUBS     R0,R0,#+1
        BNE      ??ledOff_1
//  322     case RED:   GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );  
??ledOff_2:
        MOVS     R2,#+0
        MOVS     R1,#+2
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
        B        ??ledOff_1
//  323       break;
//  324     case GREEN: GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_RESET );  
??ledOff_0:
        MOVS     R2,#+0
        MOVS     R1,#+4
        LDR      R0,??DataTable20  ;; 0xffffe400
        BL       GPIO_WriteBit
//  325       break;
//  326   }
//  327 }
??ledOff_1:
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock7

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable20:
        DC32     0xffffe400
//  328 
//  329 ///////////////////////////////////////////////////////////////////////////////
//  330 // setBlinkType
//  331 //
//  332 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function setBlinkType
        THUMB
//  333 void setBlinkType( uint8_t type  )
//  334 {
//  335   blink_state_time = 0;
setBlinkType:
        LDR      R1,??DataTable21  ;; blink_state_time
        MOVS     R2,#+0
        STR      R2,[R1, #+0]
//  336   blink_type = type;
        LDR      R1,??DataTable22  ;; blink_type
        STRB     R0,[R1, #+0]
//  337   CANAdapter_stateChange = TRUE;
        LDR      R1,??DataTable23  ;; CANAdapter_stateChange
        MOVS     R2,#+1
        STRB     R2,[R1, #+0]
//  338 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock8

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable21:
        DC32     blink_state_time

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable22:
        DC32     blink_type

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable23:
        DC32     CANAdapter_stateChange
//  339 
//  340 ///////////////////////////////////////////////////////////////////////////////
//  341 // ledOff
//  342 //
//  343 // This routine is called from the 1 ms interrupt and blink
//  344 // lamp in the selected patters
//  345 //
//  346 
//  347 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function blink
        THUMB
//  348 void blink( void ) 
//  349 {
blink:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  350   blink_state_time++;
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??blink_0     ;; blink_state_time
        STR      R0,[R1, #+0]
//  351   
//  352   switch( blink_type ) {
        LDR      R0,??blink_0+0x4  ;; blink_type
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??blink_1
        SUBS     R0,R0,#+1
        BEQ      ??blink_2
        SUBS     R0,R0,#+1
        BEQ      ??blink_3
        SUBS     R0,R0,#+1
        BEQ      ??blink_4
        SUBS     R0,R0,#+1
        BEQ      ??blink_5
        SUBS     R0,R0,#+1
        BEQ      ??blink_6
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??blink_7
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??blink_8
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??blink_9
        B        ??blink_10
//  353   
//  354     case LED_OFF:
//  355       ledOff( RED );
??blink_1:
        MOVS     R0,#+1
        BL       ledOff
//  356       ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
        B        ??blink_10
//  357       break;
//  358     
//  359     case LED_RED_STEADY: 
//  360       ledOn( RED );
??blink_2:
        MOVS     R0,#+1
        BL       ledOn
//  361       ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
        B        ??blink_10
//  362       break;
//  363     
//  364     case LED_GREEN_STEADY: 
//  365       ledOn( GREEN );
??blink_3:
        MOVS     R0,#+0
        BL       ledOn
//  366       ledOff( RED );
        MOVS     R0,#+1
        BL       ledOff
        B        ??blink_10
//  367       break;
//  368     
//  369     case LED_RED_BLINK_FAST:
//  370       ledOff( GREEN );
??blink_4:
        MOVS     R0,#+0
        BL       ledOff
//  371       if ( 0 == blink_state ) {
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_11
//  372         if (  blink_state_time > 500 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  373           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  374           ledOff( RED );
        MOVS     R0,#+1
        BL       ledOff
//  375           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  376         }
//  377       }
//  378       else {
//  379         if (  blink_state_time > 500 ) {
??blink_11:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  380           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  381           ledOn( RED );
        MOVS     R0,#+1
        BL       ledOn
//  382           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  383         }  
//  384       }
//  385       break;
//  386     
//  387     case LED_RED_BLINK_SLOW: 
//  388       ledOff( GREEN );
??blink_5:
        MOVS     R0,#+0
        BL       ledOff
//  389       if ( 0 == blink_state ) {
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_12
//  390         if (  blink_state_time > 1000 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  391           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  392           ledOff( RED );
        MOVS     R0,#+1
        BL       ledOff
//  393           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  394         }
//  395       }
//  396       else {
//  397         if (  blink_state_time > 1000 ) {
??blink_12:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  398           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  399           ledOn( RED );
        MOVS     R0,#+1
        BL       ledOn
//  400           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  401         }  
//  402       }
//  403       break;
//  404     
//  405     case LED_GREEN_BLINK_FAST: 
//  406       ledOff( RED );
??blink_6:
        MOVS     R0,#+1
        BL       ledOff
//  407       if ( 0 == blink_state ) {
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_13
//  408         if (  blink_state_time > 500 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  409           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  410           ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
//  411           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  412         }
//  413       }
//  414       else {
//  415         if (  blink_state_time > 500 ) {
??blink_13:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCS      .+4
        B        ??blink_10
//  416           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  417           ledOn( GREEN );
        MOVS     R0,#+0
        BL       ledOn
//  418           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  419         }  
//  420       }
//  421       break;
//  422     
//  423     case LED_GREEN_BLINK_SLOW: 
//  424       ledOff( RED );
??blink_7:
        MOVS     R0,#+1
        BL       ledOff
//  425       if ( 0 == blink_state ) {
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_14
//  426         if (  blink_state_time > 1000 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCC      ??blink_10
//  427           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  428           ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
//  429           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  430         }
//  431       }
//  432       else {
//  433         if (  blink_state_time > 1000 ) {
??blink_14:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCC      ??blink_10
//  434           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  435           ledOn( GREEN );
        MOVS     R0,#+0
        BL       ledOn
//  436           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  437         }  
//  438       }
//  439       break;
//  440     
//  441     case LED_RED_GREEN_BLINK_FAST:
//  442       if ( 0 == blink_state ) {
??blink_9:
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_15
//  443         if (  blink_state_time > 500 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCC      ??blink_10
//  444           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  445           ledOff( RED );
        MOVS     R0,#+1
        BL       ledOff
//  446           ledOn( GREEN );
        MOVS     R0,#+0
        BL       ledOn
//  447           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  448         }
//  449       }
//  450       else {
//  451         if (  blink_state_time > 500 ) {
??blink_15:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        MOVS     R1,#+255
        ADDS     R1,R1,#+246      ;; #+501
        CMP      R0,R1
        BCC      ??blink_10
//  452           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  453           ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
//  454           ledOn( RED );
        MOVS     R0,#+1
        BL       ledOn
//  455           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  456         }  
//  457       }
//  458       break;
//  459       
//  460     case LED_RED_GREEN_BLINK_SLOW:
//  461       if ( 0 == blink_state ) {
??blink_8:
        LDR      R0,??blink_0+0x8  ;; blink_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??blink_16
//  462         if (  blink_state_time > 1000 ) {
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCC      ??blink_10
//  463           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  464           ledOff( RED );
        MOVS     R0,#+1
        BL       ledOff
//  465           ledOn( GREEN );
        MOVS     R0,#+0
        BL       ledOn
//  466           blink_state = 1;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??blink_10
//  467         }
//  468       }
//  469       else {
//  470         if (  blink_state_time > 1000 ) {
??blink_16:
        LDR      R0,??blink_0     ;; blink_state_time
        LDR      R0,[R0, #+0]
        LDR      R1,??blink_0+0xC  ;; 0x3e9
        CMP      R0,R1
        BCC      ??blink_10
//  471           blink_state_time = 0;
        LDR      R0,??blink_0     ;; blink_state_time
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  472           ledOff( GREEN );
        MOVS     R0,#+0
        BL       ledOff
//  473           ledOn( RED );
        MOVS     R0,#+1
        BL       ledOn
//  474           blink_state = 0;
        LDR      R0,??blink_0+0x8  ;; blink_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  475         }  
//  476       }
//  477       break;
//  478       
//  479   }
//  480 }
??blink_10:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??blink_0:
        DC32     blink_state_time
        DC32     blink_type
        DC32     blink_state
        DC32     0x3e9
        CFI EndBlock cfiBlock9
//  481 
//  482 ///////////////////////////////////////////////////////////////////////////////
//  483 // CAN_Com_Polling
//  484 //
//  485 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function CAN_Com_Polling
        THUMB
//  486 void CAN_Com_Polling( void )
//  487 {
CAN_Com_Polling:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//  488 /*  
//  489   CAN_InitTypeDef CAN_Struct;
//  490   
//  491   CAN_EnterInitMode( CAN_CR_CCE ); 
//  492   
//  493   // initialize the CAN at a standard bitrate, interrupts disabled 
//  494   CAN_Struct.CAN_ConfigParameters=0x0;
//  495   CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
//  496   CAN_Init( &CAN_Struct );
//  497  
//  498   // configure the message objects  
//  499   CAN_InvalidateAllMsgObj();
//  500   CAN_SetTxMsgObj( CAN_TX_STD_MSGOBJ, CAN_STD_ID );
//  501   CAN_SetRxMsgObj( CAN_RX_MSGOBJ0, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
//  502   CAN_LeaveInitMode( );
//  503 */
//  504   // Send the pre-defined answer
//  505   CAN_SendMessage( CAN_TX_MSGOBJ, &TxCanMsg[ 0 ] );
        LDR      R1,??CAN_Com_Polling_0  ;; TxCanMsg
        MOVS     R0,#+0
        BL       CAN_SendMessage
//  506 
//  507   // wait until end of transmission 
//  508   CAN_WaitEndOfTx();
        BL       CAN_WaitEndOfTx
//  509   
//  510   CAN_ReleaseTxMessage( CAN_TX_MSGOBJ );
        MOVS     R0,#+0
        BL       CAN_ReleaseTxMessage
//  511 
//  512 /* 
//  513   // wait for reception of a data frame 
//  514   while ( !CAN_ReceiveMessage( CAN_RX_MSGOBJ, FALSE, &RxCanMsg ) )
//  515   {
//  516     // Add Timer
//  517   }
//  518 
//  519   // Test Received Msg 
//  520   if ( ( RxCanMsg.IdType == CAN_STD_ID )&&
//  521       ( RxCanMsg.Id == 0x321 ) && 
//  522       ( RxCanMsg.Dlc == 4 ) &&
//  523       ( RxCanMsg.Data[0] == 0xAA ) &&
//  524       ( RxCanMsg.Data[1] == 0x55 ) &&
//  525       ( RxCanMsg.Data[2] == 0xAA ) && 
//  526       ( RxCanMsg.Data[3] == 0x55 ) ) {
//  527     // Received Msg OK
//  528     LED_ON( GREEN );
//  529   } 
//  530   else {
//  531     // Received Msg KO
//  532     LED_OFF( RED );
//  533   }
//  534 */
//  535 
//  536   // release the message objects 
//  537   //CAN_ReleaseTxMessage( CAN_TX_STD_MSGOBJ );
//  538   //CAN_ReleaseRxMessage( CAN_RX_MSGOBJ0 );
//  539   
//  540   delay_ms( 10 );
        MOVS     R0,#+10
        BL       delay_ms
//  541 }
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        DATA
??CAN_Com_Polling_0:
        DC32     TxCanMsg
        CFI EndBlock cfiBlock10
//  542 
//  543 ///////////////////////////////////////////////////////////////////////////////
//  544 // CAN_Com_IRQ
//  545 //
//  546 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function CAN_Com_IRQ
        THUMB
//  547 void CAN_Com_IRQ( void )
//  548 {
CAN_Com_IRQ:
        PUSH     {LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+4
        SUB      SP,SP,#+20
        CFI CFA R13+24
//  549   EIC_IRQInitTypeDef  EIC_IRQInitStructure;
//  550   CAN_InitTypeDef     CAN_Struct;
//  551   
//  552   // initialize the interrupt controller 
//  553   EIC_DeInit();
        BL       EIC_DeInit
//  554     
//  555   EIC_IRQInitStructure.EIC_IRQChannel=CAN_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+22
        STRB     R1,[R0, #+0]
//  556   EIC_IRQInitStructure.EIC_IRQChannelPriority=1;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+1]
//  557   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  558   EIC_IRQInit(&EIC_IRQInitStructure);
        MOV      R0,SP
        BL       EIC_IRQInit
//  559 	
//  560   // initialize the CAN at a standard bitrate, interrupts enabled 
//  561   CAN_Struct.CAN_ConfigParameters=CAN_CR_IE;
        MOV      R0,SP
        MOVS     R1,#+2
        STRB     R1,[R0, #+8]
//  562   CAN_Struct.CAN_Bitrate=CAN_BITRATE_500K;
        MOVS     R0,#+6
        STR      R0,[SP, #+12]
//  563   CAN_Init(&CAN_Struct);
        ADD      R0,SP,#+8
        BL       CAN_Init
//  564 
//  565   // configure the message objects 
//  566   CAN_EnterInitMode( CAN_CR_CCE );
        MOVS     R0,#+64
        BL       CAN_EnterInitMode
//  567   CAN_InvalidateAllMsgObj();
        BL       CAN_InvalidateAllMsgObj
//  568   CAN_SetTxMsgObj( CAN_TX_MSGOBJ, CAN_EXT_ID );
        MOVS     R1,#+1
        MOVS     R0,#+0
        BL       CAN_SetTxMsgObj
//  569   CAN_SetRxMsgObj( CAN_RX_MSGOBJ0, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
        SUB      SP,SP,#+8
        CFI CFA R13+32
        MOVS     R0,#+1
        STR      R0,[SP, #+0]
        LDR      R3,??CAN_Com_IRQ_0  ;; 0x1fffffff
        MOVS     R2,#+0
        MOVS     R1,#+1
        MOVS     R0,#+1
        BL       CAN_SetRxMsgObj
        ADD      SP,SP,#+8
        CFI CFA R13+24
//  570   CAN_LeaveInitMode();
        BL       CAN_LeaveInitMode
//  571   
//  572   // enable global interrupt 
//  573   EIC_IRQCmd( ENABLE );
        MOVS     R0,#+1
        BL       EIC_IRQCmd
//  574 
//  575   // Send the pre-defined frame 
//  576   CAN_SendMessage( CAN_TX_MSGOBJ, &TxCanMsg[2] );
        LDR      R1,??CAN_Com_IRQ_0+0x4  ;; TxCanMsg + 40
        MOVS     R0,#+0
        BL       CAN_SendMessage
//  577 
//  578   // wait until end of transmission 
//  579   CAN_WaitEndOfTx();
        BL       CAN_WaitEndOfTx
//  580   
//  581   // reception and release are done in the interrupt handler 
//  582 
//  583   // delay to add when the reception occurs 
//  584   Int32U dly = 0xffff;
        LDR      R0,??CAN_Com_IRQ_0+0x8  ;; 0xffff
        STR      R0,[SP, #+4]
//  585   Dly100us( &dly );
        ADD      R0,SP,#+4
        BL       Dly100us
//  586 
//  587   // Test Received Msg 
//  588   if ( ( RxCanMsg.IdType == CAN_EXT_ID ) && 
//  589       ( RxCanMsg.Id == 0x12345678 ) &&
//  590       ( RxCanMsg.Dlc == 8 ) &&
//  591       ( RxCanMsg.Data[0] == 0x10 ) &&
//  592       ( RxCanMsg.Data[1] == 0x11 ) &&
//  593       ( RxCanMsg.Data[2] == 0x12 ) && 
//  594       ( RxCanMsg.Data[3] == 0x13 ) &&
//  595       ( RxCanMsg.Data[4] == 0x14 ) &&
//  596       ( RxCanMsg.Data[5] == 0x15 ) &&
//  597       ( RxCanMsg.Data[6] == 0x16 ) &&
//  598       ( RxCanMsg.Data[7]==0x17 ) ) {
//  599     // Received Msg OK
//  600   } else {
//  601     // Received Msg KO
//  602 
//  603   }
//  604 
//  605   // disable interrupts globally 
//  606   EIC_IRQCmd( DISABLE );
        MOVS     R0,#+0
        BL       EIC_IRQCmd
//  607 }
        ADD      SP,SP,#+20
        CFI CFA R13+4
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??CAN_Com_IRQ_0:
        DC32     0x1fffffff
        DC32     TxCanMsg + 40
        DC32     0xffff
        CFI EndBlock cfiBlock11
//  608 
//  609 ///////////////////////////////////////////////////////////////////////////////
//  610 // init_CAN
//  611 //
//  612 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function init_CAN
        THUMB
//  613 void init_CAN( void )
//  614 {
init_CAN:
        PUSH     {R0-R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+24
//  615   int i;
//  616   
//  617   EIC_IRQInitTypeDef  EIC_IRQInitStructure;
//  618   CAN_InitTypeDef     CAN_Struct;
//  619   
//  620   // Init output fifo
//  621   cntOutFrames = 0;
        LDR      R0,??init_CAN_0  ;; cntOutFrames
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  622   posOutFetch = 0;
        LDR      R0,??init_CAN_0+0x4  ;; posOutFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  623   posOutPut = 0;
        LDR      R0,??init_CAN_0+0x8  ;; posOutPut
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  624   
//  625   // Init input fifo
//  626   cntInFrames = 0;
        LDR      R0,??init_CAN_0+0xC  ;; cntInFrames
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  627   posInFetch = 0;
        LDR      R0,??init_CAN_0+0x10  ;; posInFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  628   posInPut = 0;
        LDR      R0,??init_CAN_0+0x14  ;; posInPut
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  629   
//  630   // Last ACK'ed frame is none
//  631   lastAck = 0;
        LDR      R0,??init_CAN_0+0x18  ;; lastAck
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  632   
//  633   // initialize the interrupt controller 
//  634   EIC_IRQInitStructure.EIC_IRQChannel = CAN_IRQChannel;
        MOV      R0,SP
        MOVS     R1,#+22
        STRB     R1,[R0, #+0]
//  635   EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+1]
//  636   EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+2]
//  637   EIC_IRQInit( &EIC_IRQInitStructure );
        MOV      R0,SP
        BL       EIC_IRQInit
//  638   
//  639   CAN_EnterInitMode( CAN_CR_CCE );
        MOVS     R0,#+64
        BL       CAN_EnterInitMode
//  640    
//  641   // initialize the CAN at a standard bitrate, interrupts enabled 
//  642   CAN_Struct.CAN_ConfigParameters = CAN_CR_IE | CAN_CR_EIE | CAN_CR_SIE;
        MOV      R0,SP
        MOVS     R1,#+14
        STRB     R1,[R0, #+4]
//  643   CAN_Struct.CAN_Bitrate = CAN_BITRATE_500K;
        MOVS     R0,#+6
        STR      R0,[SP, #+8]
//  644   CAN_Init( &CAN_Struct );
        ADD      R0,SP,#+4
        BL       CAN_Init
//  645   
//  646   // configure the message objects 
//  647   CAN_InvalidateAllMsgObj();
        BL       CAN_InvalidateAllMsgObj
//  648   
//  649   // Configure the standard id transmit object
//  650   CAN_SetTxMsgObj( CAN_TX_MSGOBJ, CAN_STD_ID );
        MOVS     R1,#+0
        MOVS     R0,#+0
        BL       CAN_SetTxMsgObj
//  651   
//  652   // Configure the extended id transmit object
//  653   CAN_SetTxMsgObj( CAN_TX_MSGOBJ + 1, CAN_EXT_ID );
        MOVS     R1,#+1
        MOVS     R0,#+1
        BL       CAN_SetTxMsgObj
//  654   
//  655   // Configure receive objects
//  656   for ( i=2; i<15; i++ ) {
        MOVS     R0,#+2
        MOVS     R4,R0
??init_CAN_1:
        CMP      R4,#+15
        BGE      ??init_CAN_2
//  657     CAN_SetRxMsgObj( i, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
        SUB      SP,SP,#+8
        CFI CFA R13+32
        MOVS     R0,#+1
        STR      R0,[SP, #+0]
        LDR      R3,??init_CAN_0+0x1C  ;; 0x1fffffff
        MOVS     R2,#+0
        MOVS     R1,#+1
        MOVS     R0,R4
        BL       CAN_SetRxMsgObj
        ADD      SP,SP,#+8
        CFI CFA R13+24
//  658   }
        ADDS     R4,R4,#+1
        B        ??init_CAN_1
//  659   for ( i=16; i<32; i++ ) {
??init_CAN_2:
        MOVS     R0,#+16
        MOVS     R4,R0
??init_CAN_3:
        CMP      R4,#+32
        BGE      ??init_CAN_4
//  660     CAN_SetRxMsgObj( i, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
        SUB      SP,SP,#+8
        CFI CFA R13+32
        MOVS     R0,#+1
        STR      R0,[SP, #+0]
        LDR      R3,??init_CAN_0+0x20  ;; 0x7ff
        MOVS     R2,#+0
        MOVS     R1,#+0
        MOVS     R0,R4
        BL       CAN_SetRxMsgObj
        ADD      SP,SP,#+8
        CFI CFA R13+24
//  661   }
        ADDS     R4,R4,#+1
        B        ??init_CAN_3
//  662   
//  663   CAN_LeaveInitMode();
??init_CAN_4:
        BL       CAN_LeaveInitMode
//  664   
//  665   // enable global interrupt 
//  666   EIC_IRQCmd( ENABLE );
        MOVS     R0,#+1
        BL       EIC_IRQCmd
//  667 }
        POP      {R0-R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??init_CAN_0:
        DC32     cntOutFrames
        DC32     posOutFetch
        DC32     posOutPut
        DC32     cntInFrames
        DC32     posInFetch
        DC32     posInPut
        DC32     lastAck
        DC32     0x1fffffff
        DC32     0x7ff
        CFI EndBlock cfiBlock12
//  668 
//  669 ///////////////////////////////////////////////////////////////////////////////
//  670 // sendMessage
//  671 //
//  672 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function sendMessage
        THUMB
//  673 void sendMessage( void )
//  674 {
sendMessage:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        SUB      SP,SP,#+24
        CFI CFA R13+32
//  675   canmsg msg;
//  676   vs_frame *pFrame;
//  677   
//  678   if ( cntOutFrames ) {           // If anything to send
        LDR      R0,??sendMessage_0  ;; cntOutFrames
        LDRH     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??sendMessage_1
//  679         
//  680     pFrame = (vs_frame *)&outputFrames[ posOutFetch ];
        LDR      R0,??sendMessage_0+0x4  ;; posOutFetch
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+20
        MULS     R0,R1,R0
        LDR      R1,??sendMessage_0+0x8  ;; outputFrames
        ADDS     R0,R1,R0
        MOVS     R4,R0
//  681       
//  682     // Send the message
//  683     if ( !( pFrame->data[0] & 0x80 ) ) {
        LDRB     R0,[R4, #+6]
        LSLS     R0,R0,#+24
        BMI      ??sendMessage_2
//  684       // Standard id
//  685       msg.IdType = 0;
        MOVS     R0,#+0
        STR      R0,[SP, #+0]
//  686       if ( pFrame->data[0] & 0x40 ) {
        LDRB     R0,[R4, #+6]
        LSLS     R0,R0,#+25
        BPL      ??sendMessage_3
//  687         msg.IdType |= 0x00;   // Remote frame
        LDR      R0,[SP, #+0]
        STR      R0,[SP, #+0]
//  688       }
//  689       
//  690       msg.Id = pFrame->data[3] + 
//  691                 ( pFrame->data[2] << 8 ) +
//  692                 ( pFrame->data[1] << 16 ) +
//  693                 ( ( pFrame->data[0] & 0x0f ) << 24 );
??sendMessage_3:
        LDRB     R0,[R4, #+9]
        LDRB     R1,[R4, #+8]
        LSLS     R1,R1,#+8
        ADDS     R0,R0,R1
        LDRB     R1,[R4, #+7]
        LSLS     R1,R1,#+16
        ADDS     R0,R0,R1
        LDRB     R1,[R4, #+6]
        LSLS     R1,R1,#+28       ;; ZeroExtS R1,R1,#+28,#+28
        LSRS     R1,R1,#+28
        LSLS     R1,R1,#+24
        ADDS     R0,R0,R1
        STR      R0,[SP, #+4]
//  694       msg.Dlc = ( pFrame->flags & 0x1f ) - 8;
        LDRB     R0,[R4, #+1]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        SUBS     R0,R0,#+8
        MOV      R1,SP
        STRB     R0,[R1, #+8]
//  695       memcpy( msg.Data, ( pFrame->data + 4 ), msg.Dlc );
        MOV      R0,SP
        LDRB     R2,[R0, #+8]
        MOVS     R1,R4
        ADDS     R1,R1,#+10
        MOV      R0,SP
        ADDS     R0,R0,#+9
        BL       memcpy
//  696       CAN_SendMessage( CAN_TX_MSGOBJ, &msg );
        MOV      R1,SP
        MOVS     R0,#+0
        BL       CAN_SendMessage
        B        ??sendMessage_4
//  697     }
//  698     else {
//  699       // Extended id
//  700       msg.IdType = CAN_EXT_ID;
??sendMessage_2:
        MOVS     R0,#+1
        STR      R0,[SP, #+0]
//  701       if ( pFrame->data[0] & 0x40 ) {
        LDRB     R0,[R4, #+6]
        LSLS     R0,R0,#+25
        BPL      ??sendMessage_5
//  702         msg.IdType |= 0x00;   // Remote frame
        LDR      R0,[SP, #+0]
        STR      R0,[SP, #+0]
//  703       }
//  704       
//  705       msg.Id = pFrame->data[ 3 ] + 
//  706                 ( pFrame->data[ 2 ] << 8 ) +
//  707                 ( pFrame->data[ 1 ] << 16 ) +
//  708                 ( ( pFrame->data[ 0 ] & 0x0f ) << 24 );
??sendMessage_5:
        LDRB     R0,[R4, #+9]
        LDRB     R1,[R4, #+8]
        LSLS     R1,R1,#+8
        ADDS     R0,R0,R1
        LDRB     R1,[R4, #+7]
        LSLS     R1,R1,#+16
        ADDS     R0,R0,R1
        LDRB     R1,[R4, #+6]
        LSLS     R1,R1,#+28       ;; ZeroExtS R1,R1,#+28,#+28
        LSRS     R1,R1,#+28
        LSLS     R1,R1,#+24
        ADDS     R0,R0,R1
        STR      R0,[SP, #+4]
//  709       msg.Dlc = ( pFrame->flags & 0x1f ) - 4;
        LDRB     R0,[R4, #+1]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        SUBS     R0,R0,#+4
        MOV      R1,SP
        STRB     R0,[R1, #+8]
//  710       memcpy( msg.Data, ( pFrame->data + 8 ), msg.Dlc );
        MOV      R0,SP
        LDRB     R2,[R0, #+8]
        MOVS     R1,R4
        ADDS     R1,R1,#+14
        MOV      R0,SP
        ADDS     R0,R0,#+9
        BL       memcpy
//  711       CAN_SendMessage( CAN_TX_MSGOBJ + 1, &msg );
        MOV      R1,SP
        MOVS     R0,#+1
        BL       CAN_SendMessage
//  712     }
//  713     
//  714     posOutFetch++;
??sendMessage_4:
        LDR      R0,??sendMessage_0+0x4  ;; posOutFetch
        LDRH     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??sendMessage_0+0x4  ;; posOutFetch
        STRH     R0,[R1, #+0]
//  715     if ( posOutFetch >= MAX_OUT_FRAMES ) posOutFetch = 0;
        LDR      R0,??sendMessage_0+0x4  ;; posOutFetch
        LDRH     R0,[R0, #+0]
        CMP      R0,#+16
        BCC      ??sendMessage_6
        LDR      R0,??sendMessage_0+0x4  ;; posOutFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  716     cntOutFrames--;
??sendMessage_6:
        LDR      R0,??sendMessage_0  ;; cntOutFrames
        LDRH     R0,[R0, #+0]
        SUBS     R0,R0,#+1
        LDR      R1,??sendMessage_0  ;; cntOutFrames
        STRH     R0,[R1, #+0]
//  717         
//  718     // Statistics
//  719     cntSentFrames++;
        LDR      R0,??sendMessage_0+0xC  ;; cntSentFrames
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??sendMessage_0+0xC  ;; cntSentFrames
        STR      R0,[R1, #+0]
//  720     cntSentData += outputFrames[ posOutFetch ].Dlc;
        LDR      R0,??sendMessage_0+0x10  ;; cntSentData
        LDR      R0,[R0, #+0]
        LDR      R1,??sendMessage_0+0x4  ;; posOutFetch
        LDRH     R1,[R1, #+0]
        MOVS     R2,#+20
        MULS     R1,R2,R1
        LDR      R2,??sendMessage_0+0x8  ;; outputFrames
        ADDS     R1,R2,R1
        LDRB     R1,[R1, #+8]
        ADDS     R0,R0,R1
        LDR      R1,??sendMessage_0+0x10  ;; cntSentData
        STR      R0,[R1, #+0]
//  721   }
//  722 }
??sendMessage_1:
        ADD      SP,SP,#+24
        CFI CFA R13+8
        POP      {R4}
        POP      {R0}
        BX       R0               ;; return
        Nop      
        DATA
??sendMessage_0:
        DC32     cntOutFrames
        DC32     posOutFetch
        DC32     outputFrames
        DC32     cntSentFrames
        DC32     cntSentData
        CFI EndBlock cfiBlock13
//  723 
//  724 ///////////////////////////////////////////////////////////////////////////////
//  725 // CAN_ISR
//  726 //
//  727 // CAN interrupt service routine
//  728 //
//  729 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function CAN_ISR
        THUMB
//  730 void CAN_ISR( void )
//  731 {
CAN_ISR:
        PUSH     {R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+12
        SUB      SP,SP,#+20
        CFI CFA R13+32
//  732   canmsg RxCanMsg;
//  733   uint8_t msgobj;
//  734   uint16_t idr;
//  735   
//  736   if ( 0x8000 == ( idr = CAN->IDR ) ) {
        LDR      R0,??CAN_ISR_0   ;; 0xffffc410
        LDRH     R0,[R0, #+0]
        MOVS     R5,R0
        MOVS     R1,#+128
        LSLS     R1,R1,#+8        ;; #+32768
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R0,R1
        BNE      ??CAN_ISR_1
//  737     
//  738     // We have a status interrupt
//  739     // bit 7 - Bus off
//  740     // bit 6 - Warning
//  741     // bit 5 - Error Passive
//  742     // bit 4 - RXOK
//  743     // bit 3 - TXOK
//  744     // bit 2-0 - Last Error Code.
//  745     canstatus = CAN->SR;
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDR      R1,??CAN_ISR_0+0x8  ;; 0xffffc404
        LDRH     R1,[R1, #+0]
        STRB     R1,[R0, #+0]
//  746     
//  747     // May need visual indication
//  748     CANAdapter_stateChange = TRUE;
        LDR      R0,??CAN_ISR_0+0xC  ;; CANAdapter_stateChange
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
//  749     
//  750     // Check for bus off
//  751     if ( bBusOff ) {
        LDR      R0,??CAN_ISR_0+0x10  ;; bBusOff
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??CAN_ISR_2
//  752       if ( !( canstatus & CAN_SR_BOFF ) ) {
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24
        BMI      ??CAN_ISR_3
//  753         // Going away from bus off
//  754         bBusOff = FALSE;  
        LDR      R0,??CAN_ISR_0+0x10  ;; bBusOff
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        B        ??CAN_ISR_3
//  755       }
//  756     }
//  757     else {
//  758       if ( canstatus & CAN_SR_BOFF ) {
??CAN_ISR_2:
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+24
        BPL      ??CAN_ISR_3
//  759         // bus off
//  760         bBusOff = TRUE;
        LDR      R0,??CAN_ISR_0+0x10  ;; bBusOff
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  761       }  
//  762     }
//  763     
//  764     // Check for bus warning
//  765     if ( bBusWarn ) {
??CAN_ISR_3:
        LDR      R0,??CAN_ISR_0+0x14  ;; bBusWarn
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??CAN_ISR_4
//  766       if ( !( canstatus & CAN_SR_EWARN ) ) {
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+25
        BMI      ??CAN_ISR_5
//  767         // Going away from bus warning
//  768         bBusWarn = FALSE;  
        LDR      R0,??CAN_ISR_0+0x14  ;; bBusWarn
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        B        ??CAN_ISR_5
//  769       }
//  770     }
//  771     else {
//  772       if ( canstatus & CAN_SR_EWARN ) {
??CAN_ISR_4:
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+25
        BPL      ??CAN_ISR_5
//  773         // bus warning
//  774         bBusWarn = TRUE;
        LDR      R0,??CAN_ISR_0+0x14  ;; bBusWarn
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
//  775       }  
//  776     }
//  777     
//  778     // Check for error passive
//  779     if ( bErrorPassive ) {
??CAN_ISR_5:
        LDR      R0,??CAN_ISR_0+0x18  ;; bErrorPassive
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??CAN_ISR_6
//  780       if ( !( canstatus & CAN_SR_EPASS ) ) {
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+26
        BMI      ??CAN_ISR_7
//  781         // Going away from error passive
//  782         bErrorPassive = FALSE;  
        LDR      R0,??CAN_ISR_0+0x18  ;; bErrorPassive
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
        B        ??CAN_ISR_7
//  783       }
//  784     }
//  785     else {
//  786       if ( canstatus & CAN_SR_EPASS ) {
??CAN_ISR_6:
        LDR      R0,??CAN_ISR_0+0x4  ;; canstatus
        LDRB     R0,[R0, #+0]
        LSLS     R0,R0,#+26
        BPL      ??CAN_ISR_7
//  787         // error passive
//  788         bErrorPassive = TRUE;  
        LDR      R0,??CAN_ISR_0+0x18  ;; bErrorPassive
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
        B        ??CAN_ISR_7
//  789       }  
//  790     }
//  791     
//  792   }
//  793   else if ( msgobj = ( idr & 0x1f ) ) {
??CAN_ISR_1:
        MOVS     R0,R5
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        MOVS     R4,R0
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        CMP      R0,#+0
        BEQ      ??CAN_ISR_7
//  794     
//  795     if ( msgobj < 3 ) {
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        CMP      R4,#+3
        BCS      ??CAN_ISR_8
//  796       
//  797       // We have transmitted a message, send next if one is availabe    
//  798       
//  799       // Clear interrupt pending bit
//  800       CAN->sMsgObj[1].CMR = CAN_CMR_CLRINTPND;
        LDR      R0,??CAN_ISR_0+0x1C  ;; 0xffffc484
        MOVS     R1,#+8
        STRH     R1,[R0, #+0]
//  801       CAN->sMsgObj[1].CRR = msgobj;
        LDR      R0,??CAN_ISR_0+0x20  ;; 0xffffc480
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        STRH     R4,[R0, #+0]
//  802       
//  803       sendMessage();
        BL       sendMessage
        B        ??CAN_ISR_7
//  804       
//  805     }
//  806     else {
//  807       
//  808       // We have received a message - get it
//  809       CAN_ReceiveMessage( msgobj-1, TRUE, &RxCanMsg );
??CAN_ISR_8:
        MOV      R2,SP
        MOVS     R1,#+1
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        SUBS     R0,R4,#+1
        BL       CAN_ReceiveMessage
//  810       
//  811       if ( CANAdapter_bOpen ) { // Receive only if open
        LDR      R0,??CAN_ISR_0+0x24  ;; CANAdapter_bOpen
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??CAN_ISR_7
//  812         // Statistics
//  813         cntReceivedFrames++;
        LDR      R0,??CAN_ISR_0+0x28  ;; cntReceivedFrames
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??CAN_ISR_0+0x28  ;; cntReceivedFrames
        STR      R0,[R1, #+0]
//  814         cntReceivedData += RxCanMsg.Dlc;
        LDR      R0,??CAN_ISR_0+0x2C  ;; cntReceivedData
        LDR      R0,[R0, #+0]
        MOV      R1,SP
        LDRB     R1,[R1, #+8]
        ADDS     R0,R0,R1
        LDR      R1,??CAN_ISR_0+0x2C  ;; cntReceivedData
        STR      R0,[R1, #+0]
//  815       
//  816         if ( cntInFrames <= MAX_IN_FRAMES ) {
        LDR      R0,??CAN_ISR_0+0x30  ;; cntInFrames
        LDRH     R0,[R0, #+0]
        CMP      R0,#+17
        BCS      ??CAN_ISR_9
//  817           memcpy( &inputFrames[ posInPut ], &RxCanMsg, sizeof( RxCanMsg ) );
        MOVS     R2,#+20
        MOV      R1,SP
        LDR      R0,??CAN_ISR_0+0x34  ;; posInPut
        LDRH     R0,[R0, #+0]
        MOVS     R3,#+20
        MULS     R0,R3,R0
        LDR      R3,??CAN_ISR_0+0x38  ;; inputFrames
        ADDS     R0,R3,R0
        BL       memcpy
//  818           cntInFrames++;
        LDR      R0,??CAN_ISR_0+0x30  ;; cntInFrames
        LDRH     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??CAN_ISR_0+0x30  ;; cntInFrames
        STRH     R0,[R1, #+0]
//  819           posInPut++;
        LDR      R0,??CAN_ISR_0+0x34  ;; posInPut
        LDRH     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??CAN_ISR_0+0x34  ;; posInPut
        STRH     R0,[R1, #+0]
//  820           if ( posInPut >= MAX_IN_FRAMES ) posInPut = 0;
        LDR      R0,??CAN_ISR_0+0x34  ;; posInPut
        LDRH     R0,[R0, #+0]
        CMP      R0,#+16
        BCC      ??CAN_ISR_7
        LDR      R0,??CAN_ISR_0+0x34  ;; posInPut
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
        B        ??CAN_ISR_7
//  821         }
//  822         else {
//  823           cntReceiveOverruns++;
??CAN_ISR_9:
        LDR      R0,??CAN_ISR_0+0x3C  ;; cntReceiveOverruns
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??CAN_ISR_0+0x3C  ;; cntReceiveOverruns
        STR      R0,[R1, #+0]
//  824         }
//  825         
//  826       }
//  827       
//  828     }
//  829        
//  830   }
//  831 }
??CAN_ISR_7:
        ADD      SP,SP,#+20
        CFI CFA R13+12
        POP      {R4,R5}
        POP      {R0}
        BX       R0               ;; return
        DATA
??CAN_ISR_0:
        DC32     0xffffc410
        DC32     canstatus
        DC32     0xffffc404
        DC32     CANAdapter_stateChange
        DC32     bBusOff
        DC32     bBusWarn
        DC32     bErrorPassive
        DC32     0xffffc484
        DC32     0xffffc480
        DC32     CANAdapter_bOpen
        DC32     cntReceivedFrames
        DC32     cntReceivedData
        DC32     cntInFrames
        DC32     posInPut
        DC32     inputFrames
        DC32     cntReceiveOverruns
        CFI EndBlock cfiBlock14
//  832 
//  833 
//  834 
//  835 /*************************************************************************
//  836  * Function Name: main
//  837  * Parameters: none
//  838  *
//  839  * Return: none
//  840  *
//  841  * Description:
//  842  *
//  843  *************************************************************************/
//  844 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function main
        THUMB
//  845 int main( void )
//  846 {
main:
        PUSH     {R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+20
        LDR      R0,??main_0      ;; 0xfffffbcc
        ADD      SP,SP,R0
        CFI CFA R13+1096
//  847   uint16_t i;
//  848   uint8_t Buffer[ 1024 ];
//  849   uint32_t readCnt; // Incoming data count
//  850   uint32_t readPos; // Serial incoming data pointer
//  851   Boolean CdcConfigureStateHold;
//  852   
//  853   // Initialize output fifo 
//  854   cntOutFrames = 0;
        LDR      R0,??main_0+0x4  ;; cntOutFrames
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  855   posOutFetch = 0;
        LDR      R0,??main_0+0x8  ;; posOutFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  856   posOutPut = 0;
        LDR      R0,??main_0+0xC  ;; posOutPut
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  857   
//  858   // Initialize input fifo 
//  859   cntInFrames = 0;
        LDR      R0,??main_0+0x10  ;; cntInFrames
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  860   posInFetch = 0;
        LDR      R0,??main_0+0x14  ;; posInFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  861   posInPut = 0;
        LDR      R0,??main_0+0x18  ;; posInPut
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
//  862   
//  863   // Initialize statistics
//  864   cntSentFrames = 0;
        LDR      R0,??main_0+0x1C  ;; cntSentFrames
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  865   cntReceivedFrames = 0;
        LDR      R0,??main_0+0x20  ;; cntReceivedFrames
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  866   cntSentData = 0;
        LDR      R0,??main_0+0x24  ;; cntSentData
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  867   cntReceivedData = 0;
        LDR      R0,??main_0+0x28  ;; cntReceivedData
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  868   cntBusWarnings = 0;
        LDR      R0,??main_0+0x2C  ;; cntBusWarnings
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  869   cntBusOff = 0;
        LDR      R0,??main_0+0x30  ;; cntBusOff
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  870   cntReceiveOverruns = 0;
        LDR      R0,??main_0+0x34  ;; cntReceiveOverruns
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  871   cntTransmitOverruns = 0;
        LDR      R0,??main_0+0x38  ;; cntTransmitOverruns
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  872   
//  873   // Error states
//  874   bBusOff = FALSE;
        LDR      R0,??main_0+0x3C  ;; bBusOff
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  875   bBusWarn = FALSE;
        LDR      R0,??main_0+0x40  ;; bBusWarn
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  876   bErrorPassive = FALSE;
        LDR      R0,??main_0+0x44  ;; bErrorPassive
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
//  877   
//  878   // Sliding window init for USB receive
//  879   swReceive.cntOutstanding = 0;
        LDR      R0,??main_0+0x48  ;; swReceive
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  880   swReceive.maxtime = 500;
        LDR      R0,??main_0+0x4C  ;; swReceive + 4
        MOVS     R1,#+250
        LSLS     R1,R1,#+1        ;; #+500
        STR      R1,[R0, #+0]
//  881   swReceive.maxresend = 3;
        LDR      R0,??main_0+0x50  ;; swReceive + 8
        MOVS     R1,#+3
        STRB     R1,[R0, #+0]
//  882   
//  883   // Application
//  884   vs_frame serialFrame;       // VSCP Serial frame
//  885   canmsg msg;                 // CAN frame
//  886   uint16_t com_channel = 0;   // Communication channel (Always zero)
        MOV      R0,SP
        MOVS     R1,#+0
        STRH     R1,[R0, #+2]
//  887   uint8_t seqnumber = 0;      // Packet sequence number
        MOVS     R0,#+0
        MOVS     R7,R0
//  888   
//  889   // Set Burst mode
//  890   CFG_FLASHBurstConfig( CFG_FLASHBurst_Enable );
        MOVS     R0,#+128
        LSLS     R0,R0,#+1        ;; #+256
        BL       CFG_FLASHBurstConfig
//  891   
//  892   // MCU clock init
//  893   InitClock();
        BL       InitClock
//  894   
//  895   // EIC init
//  896   EIC_DeInit();
        BL       EIC_DeInit
//  897   
//  898   // Delay timer init
//  899   InitDlyTimer( 1 );
        MOVS     R0,#+1
        BL       InitDlyTimer
//  900   
//  901   // Init 1 ms timebase
//  902   initTimebase();
        BL       initTimebase
//  903   
//  904   // Init UART 0/1
//  905   //UartInit( UART, 3 );
//  906 
//  907   // GPIO ports init
//  908   IO_Init();
        BL       IO_Init
//  909   
//  910   // Initialize USB
//  911   // Init USB
//  912   USB_Init( 5, 4, UsbCdcConfigure );
        LDR      R2,??main_0+0x54  ;; UsbCdcConfigure
        MOVS     R1,#+4
        MOVS     R0,#+5
        BL       USB_Init
//  913   
//  914   // Init CDC
//  915   UsbCdcInit( 2 );
        MOVS     R0,#+2
        BL       UsbCdcInit
//  916   
//  917   // Init CAN interface
//  918   init_CAN();
        BL       init_CAN
//  919   
//  920   // Soft connection enable
//  921   USB_ConnectRes( TRUE );
        MOVS     R0,#+1
        BL       USB_ConnectRes
//  922 
//  923   // Enable the Interrupt controller to manage IRQ channel
//  924   EIC_IRQCmd( ENABLE );
        MOVS     R0,#+1
        BL       EIC_IRQCmd
//  925   __enable_interrupt();
        BL       __iar_enable_interrupt
//  926 
//  927   CdcConfigureStateHold = !UsbCdcIsCdcConfigure();
        BL       UsbCdcIsCdcConfigure
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        LSRS     R0,R0,#+31
        MOV      R1,SP
        STRB     R0,[R1, #+0]
//  928 
//  929   setBlinkType( LED_RED_BLINK_FAST );
        MOVS     R0,#+3
        BL       setBlinkType
//  930   
//  931   
//  932   while( 1 ) {
//  933     
//  934     if ( UsbCdcIsCdcConfigure() ) {
??main_1:
        BL       UsbCdcIsCdcConfigure
        CMP      R0,#+0
        BNE      .+4
        B        ??main_2
//  935       
//  936       if ( CANAdapter_stateChange ) {
        LDR      R0,??main_0+0x58  ;; CANAdapter_stateChange
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??main_3
//  937         if ( CANAdapter_bOpen  ) {
        LDR      R0,??main_0+0x5C  ;; CANAdapter_bOpen
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??main_4
//  938           if ( bBusOff ) {
        LDR      R0,??main_0+0x3C  ;; bBusOff
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??main_5
//  939             setBlinkType( LED_RED_BLINK_FAST ); 
        MOVS     R0,#+3
        BL       setBlinkType
        B        ??main_6
//  940           }
//  941           else if ( bBusWarn ) {
??main_5:
        LDR      R0,??main_0+0x40  ;; bBusWarn
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??main_7
//  942             setBlinkType( LED_RED_GREEN_BLINK_FAST );
        MOVS     R0,#+8
        BL       setBlinkType
        B        ??main_6
//  943           }
//  944           else {
//  945             setBlinkType( LED_GREEN_BLINK_FAST );
??main_7:
        MOVS     R0,#+5
        BL       setBlinkType
        B        ??main_6
//  946           }
//  947         }
//  948         else {
//  949           setBlinkType( LED_GREEN_STEADY );
??main_4:
        MOVS     R0,#+2
        BL       setBlinkType
//  950         }
//  951         
//  952         CANAdapter_stateChange = FALSE;
??main_6:
        LDR      R0,??main_0+0x58  ;; CANAdapter_stateChange
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  953         
//  954       }
//  955       
//  956       
//  957       if ( CdcConfigureStateHold == FALSE ) {
??main_3:
        MOV      R0,SP
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??main_8
//  958         CdcConfigureStateHold = TRUE;
        MOV      R0,SP
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
//  959       }
//  960       
//  961       // Data from USB
//  962       if ( readCnt = UsbCdcRead( Buffer, sizeof( Buffer ) ) ) {
??main_8:
        MOVS     R1,#+128
        LSLS     R1,R1,#+3        ;; #+1024
        ADD      R0,SP,#+48
        BL       UsbCdcRead
        MOVS     R5,R0
        CMP      R0,#+0
        BNE      .+4
        B        ??main_9
//  963         
//  964         readPos = 0;
        MOVS     R0,#+0
        MOVS     R6,R0
//  965         do {
//  966        
//  967           if ( vs_feed( Buffer[ readPos ] ) ) {
??main_10:
        ADD      R0,SP,#+48
        LDRB     R0,[R0, R6]
        BL       vs_feed
        CMP      R0,#+0
        BNE      .+4
        B        ??main_11
//  968             
//  969             // We have a frame
//  970             if ( vs_getFrame( &serialFrame ) ) {
        ADD      R0,SP,#+4
        BL       vs_getFrame
        CMP      R0,#+0
        BNE      .+4
        B        ??main_11
//  971             
//  972               switch ( serialFrame.opcode ) {
        MOV      R0,SP
        LDRB     R0,[R0, #+4]
        CMP      R0,#+0
        BEQ      ??main_12
        SUBS     R0,R0,#+1
        BEQ      ??main_13
        SUBS     R0,R0,#+1
        BEQ      ??main_14
        SUBS     R0,R0,#+1
        BEQ      ??main_15
        SUBS     R0,R0,#+2
        BEQ      ??main_16
        SUBS     R0,R0,#+250
        BEQ      ??main_17
        B        ??main_11
//  973             
//  974                 case VSCP_SERIAL_OPCODE_NOOP:
//  975                   
//  976                   // We don't do anything other then ACK the frame
//  977                   vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
??main_12:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
//  978                   break;
//  979               
//  980                 case VSCP_SERIAL_OPCODE_LEVEL1_EVENT:
//  981                   // We don't accept LEVEL I events
//  982                   vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
??main_13:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendNack
        B        ??main_11
//  983                   break;
//  984                   
//  985                 case VSCP_SERIAL_OPCODE_CAN_FRAME:
//  986                   
//  987                   if ( CANAdapter_bOpen ) { // Send only if open
??main_16:
        LDR      R0,??main_0+0x5C  ;; CANAdapter_bOpen
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BEQ      ??main_18
//  988                     // Place message in outqueue if there is room for it
//  989                     if ( cntOutFrames > MAX_OUT_FRAMES ) {
        LDR      R0,??main_0+0x4  ;; cntOutFrames
        LDRH     R0,[R0, #+0]
        CMP      R0,#+17
        BCC      ??main_19
//  990                       // No room for frame - TX overrun
//  991                       cntTransmitOverruns++;
        LDR      R0,??main_0+0x38  ;; cntTransmitOverruns
        LDR      R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??main_0+0x38  ;; cntTransmitOverruns
        STR      R0,[R1, #+0]
        B        ??main_11
//  992                     }
//  993                     else {
//  994                     
//  995                       // A CAN frame has it's id in the four first
//  996                       // bytes of the data area and timestamp in the
//  997                       // last four databytes. This means that
//  998                       // the data count must be at least eight bytes
//  999                      // for a fram to be a valid CAN frame.
// 1000                       // id + data + timestamp
// 1001                       if ( ( serialFrame.flags & 0x1f ) >= 8 ) {
??main_19:
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+1]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        CMP      R0,#+8
        BGE      .+4
        B        ??main_11
// 1002                          
// 1003                         // Place frame in outqueue
// 1004                         lastAck = serialFrame.seqnumber;
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+3]
        LDR      R1,??main_20     ;; lastAck
        STRB     R0,[R1, #+0]
// 1005                         memcpy( &outputFrames[ posOutPut++ ], 
// 1006                                   &serialFrame, 
// 1007                                   sizeof( serialFrame ) );
        LDR      R0,??main_0+0xC  ;; posOutPut
        LDRH     R0,[R0, #+0]
        MOVS     R1,R0
        ADDS     R1,R1,#+1
        LDR      R2,??main_0+0xC  ;; posOutPut
        STRH     R1,[R2, #+0]
        MOVS     R2,#+22
        ADD      R1,SP,#+4
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        MOVS     R3,#+20
        MULS     R0,R3,R0
        LDR      R3,??main_20+0x4  ;; outputFrames
        ADDS     R0,R3,R0
        BL       memcpy
// 1008                         cntOutFrames++;
        LDR      R0,??main_0+0x4  ;; cntOutFrames
        LDRH     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??main_0+0x4  ;; cntOutFrames
        STRH     R0,[R1, #+0]
// 1009                         posOutPut %= MAX_OUT_FRAMES;
        LDR      R0,??main_0+0xC  ;; posOutPut
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+16
        BL       __aeabi_idivmod
        LDR      R0,??main_0+0xC  ;; posOutPut
        STRH     R1,[R0, #+0]
// 1010                            
// 1011                         // If the CAN output queue was empty we need to send
// 1012                         // the event here
// 1013                         if ( 0 == ( cntOutFrames - 1 ) ) {
        LDR      R0,??main_0+0x4  ;; cntOutFrames
        LDRH     R0,[R0, #+0]
        SUBS     R0,R0,#+1
        CMP      R0,#+0
        BEQ      .+4
        B        ??main_11
// 1014                           sendMessage();
        BL       sendMessage
        B        ??main_11
// 1015                         }
// 1016                       
// 1017                       }
// 1018                     
// 1019                     }
// 1020                   }
// 1021                   else {
// 1022                     // Not open
// 1023                     vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
??main_18:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendNack
        B        ??main_11
// 1024                   }
// 1025                   break;
// 1026               
// 1027                 case VSCP_SERIAL_OPCODE_LEVEL2_EVENT:
// 1028                   // We don't accept LEVEL II events
// 1029                   vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
??main_14:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendNack
        B        ??main_11
// 1030                   break;
// 1031               
// 1032                 case VSCP_SERIAL_OPCODE_POLL_EVENT:
// 1033                   // We don't accept poll events
// 1034                   vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
??main_15:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendNack
        B        ??main_11
// 1035                   break;
// 1036               
// 1037                 case VSCP_SERIAL_OPCODE_NO_EVENT:
// 1038                   // We don't do anything
// 1039                   break;
// 1040               
// 1041                 case VSCP_SERIAL_OPCODE_ACK:
// 1042                   // ACK received - remove the frame for the active frames
// 1043                   break;
// 1044               
// 1045                 case VSCP_SERIAL_OPCODE_NACK: 
// 1046                   // NACK received
// 1047                   break;
// 1048               
// 1049                 case VSCP_SERIAL_OPCODE_ERROR:
// 1050                   // We don't do anything
// 1051                   break;
// 1052                   
// 1053                 case VSCP_SERIAL_OPCODE_REPLY:
// 1054                   // We don't do anything
// 1055                   break;    
// 1056               
// 1057                 case VSCP_SERIAL_OPCODE_COMMAND:
// 1058                   switch ( serialFrame.vscptype ) {
??main_17:
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+5]
        CMP      R0,#+0
        BEQ      ??main_21
        SUBS     R0,R0,#+1
        BEQ      ??main_22
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??main_23
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??main_24
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??main_25
        SUBS     R0,R0,#+1
        BNE      .+4
        B        ??main_26
        SUBS     R0,R0,#+1
        CMP      R0,#+7
        BHI      .+4
        B        ??main_11
        B        ??main_27
// 1059                     
// 1060                     case CANUSB_COMMAND_NOOP:
// 1061                       // OK
// 1062                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
??main_21:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1063                       break;
// 1064                      
// 1065                     case CANUSB_COMMAND_SET_BITRATE:
// 1066                       
// 1067                       CAN_EnterInitMode( CAN_CR_CCE | CAN_CR_IE | CAN_CR_EIE | CAN_CR_SIE );
??main_22:
        MOVS     R0,#+78
        BL       CAN_EnterInitMode
// 1068                       
// 1069                       if ( 0 == serialFrame.data[ 0 ] ) {
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+6]
        CMP      R0,#+0
        BNE      ??main_28
// 1070                                       
// 1071                         // Standard baudrate
// 1072                         switch ( serialFrame.data[ 1 ] ) {
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+7]
        CMP      R0,#+0
        BEQ      ??main_29
        SUBS     R0,R0,#+1
        BEQ      ??main_30
        SUBS     R0,R0,#+1
        BEQ      ??main_31
        SUBS     R0,R0,#+1
        BEQ      ??main_32
        SUBS     R0,R0,#+1
        BEQ      ??main_33
        SUBS     R0,R0,#+1
        BEQ      ??main_34
        SUBS     R0,R0,#+1
        BEQ      ??main_35
        SUBS     R0,R0,#+1
        BEQ      ??main_36
        SUBS     R0,R0,#+1
        BEQ      ??main_37
        B        ??main_38
// 1073                         
// 1074                           case 0x00:      
// 1075                             CAN_SetBitrate( CAN_BITRATE_10K );
??main_29:
        MOVS     R0,#+0
        BL       CAN_SetBitrate
        B        ??main_38
// 1076                             break;
// 1077                             
// 1078                           case 0x01:      
// 1079                             CAN_SetBitrate( CAN_BITRATE_20K );
??main_30:
        MOVS     R0,#+1
        BL       CAN_SetBitrate
        B        ??main_38
// 1080                             break;                            
// 1081                             
// 1082                           case 0x02:      
// 1083                             CAN_SetBitrate( CAN_BITRATE_50K );
??main_31:
        MOVS     R0,#+2
        BL       CAN_SetBitrate
        B        ??main_38
// 1084                             break;                            
// 1085                             
// 1086                           case 0x03:      
// 1087                             CAN_SetBitrate( CAN_BITRATE_100K );
??main_32:
        MOVS     R0,#+3
        BL       CAN_SetBitrate
        B        ??main_38
// 1088                             break;                            
// 1089                             
// 1090                           case 0x04:      
// 1091                             CAN_SetBitrate( CAN_BITRATE_125K );
??main_33:
        MOVS     R0,#+4
        BL       CAN_SetBitrate
        B        ??main_38
// 1092                             break;                            
// 1093                           
// 1094                           case 0x05:      
// 1095                             CAN_SetBitrate( CAN_BITRATE_250K );
??main_34:
        MOVS     R0,#+5
        BL       CAN_SetBitrate
        B        ??main_38
// 1096                             break;                            
// 1097                             
// 1098                           case 0x06:      
// 1099                             CAN_SetBitrate( CAN_BITRATE_500K );
??main_35:
        MOVS     R0,#+6
        BL       CAN_SetBitrate
        B        ??main_38
// 1100                             break;                            
// 1101                             
// 1102                           case 0x07:      
// 1103                             CAN_SetBitrate( CAN_BITRATE_800K );
??main_36:
        MOVS     R0,#+7
        BL       CAN_SetBitrate
        B        ??main_38
// 1104                             break;                            
// 1105                             
// 1106                           case 0x08:      
// 1107                             CAN_SetBitrate( CAN_BITRATE_1M );
??main_37:
        MOVS     R0,#+8
        BL       CAN_SetBitrate
        B        ??main_38
        DATA
??main_0:
        DC32     0xfffffbcc
        DC32     cntOutFrames
        DC32     posOutFetch
        DC32     posOutPut
        DC32     cntInFrames
        DC32     posInFetch
        DC32     posInPut
        DC32     cntSentFrames
        DC32     cntReceivedFrames
        DC32     cntSentData
        DC32     cntReceivedData
        DC32     cntBusWarnings
        DC32     cntBusOff
        DC32     cntReceiveOverruns
        DC32     cntTransmitOverruns
        DC32     bBusOff
        DC32     bBusWarn
        DC32     bErrorPassive
        DC32     swReceive
        DC32     swReceive + 4
        DC32     swReceive + 8
        DC32     UsbCdcConfigure
        DC32     CANAdapter_stateChange
        DC32     CANAdapter_bOpen
        THUMB
// 1108                             break;                            
// 1109                         }
// 1110                         
// 1111                       }
// 1112                       else {
// 1113                         // Custom bitrate settings
// 1114                         CAN->BTR = 256 * serialFrame.data[ 1 ] + serialFrame.data[ 2 ];    
??main_28:
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+7]
        MOVS     R1,#+128
        LSLS     R1,R1,#+1        ;; #+256
        MULS     R0,R1,R0
        ADD      R1,SP,#+4
        LDRB     R1,[R1, #+8]
        ADDS     R0,R0,R1
        LDR      R1,??main_20+0x8  ;; 0xffffc40c
        STRH     R0,[R1, #+0]
// 1115                         CAN->BRPR = serialFrame.data[ 3 ];
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+9]
        LDR      R1,??main_20+0xC  ;; 0xffffc418
        STRH     R0,[R1, #+0]
// 1116                       }
// 1117                       
// 1118                       CAN_LeaveInitMode();
??main_38:
        BL       CAN_LeaveInitMode
// 1119                       CAN_LeaveTestMode();
        BL       CAN_LeaveTestMode
// 1120                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1121                       break;
// 1122                     
// 1123                     case CANUSB_COMMAND_SET_FILTER:
// 1124                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
??main_23:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1125                       break;
// 1126                     
// 1127                     case CANUSB_COMMAND_SET_MASK:
// 1128                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
??main_24:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1129                       break;
// 1130                     
// 1131                     case CANUSB_COMMAND_OPEN:
// 1132                       setBlinkType( LED_GREEN_BLINK_FAST  );
??main_25:
        MOVS     R0,#+5
        BL       setBlinkType
// 1133                       CANAdapter_bOpen = TRUE;
        LDR      R0,??main_20+0x10  ;; CANAdapter_bOpen
        MOVS     R1,#+1
        STR      R1,[R0, #+0]
// 1134                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1135                       break;
// 1136                     
// 1137                     case CANUSB_COMMAND_CLOSE:
// 1138                       setBlinkType( LED_GREEN_STEADY );
??main_26:
        MOVS     R0,#+2
        BL       setBlinkType
// 1139                       CANAdapter_bOpen = FALSE;
        LDR      R0,??main_20+0x10  ;; CANAdapter_bOpen
        MOVS     R1,#+0
        STR      R1,[R0, #+0]
// 1140                       vs_sendAck( serialFrame.channel, serialFrame.seqnumber );
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendAck
        B        ??main_11
// 1141                       break;
// 1142                     
// 1143                     case CANUSB_COMMAND_GET_STATUS:
// 1144                       
// 1145                       break;
// 1146                     
// 1147                     case CANUSB_COMMAND_GET_VERSION:
// 1148                       
// 1149                       break;
// 1150                     
// 1151                     case CANUSB_COMMAND_GET_SERIALNO:
// 1152                       
// 1153                       break;
// 1154                     
// 1155                     case CANUSB_COMMAND_FIND_BITRATE:
// 1156                       
// 1157                       break;
// 1158                     
// 1159                     case CANUSB_COMMAND_GET_STAT_FRAME:
// 1160                       
// 1161                       break;
// 1162                     
// 1163                     case CANUSB_COMMAND_GET_STAT_DATA:
// 1164                       
// 1165                       break;
// 1166                     
// 1167                     case CANUSB_COMMAND_GET_STAT_ERROR:
// 1168                       
// 1169                       break;
// 1170                       
// 1171                     case CANUSB_COMMAND_CLR_STAT:
// 1172                       
// 1173                       break;                      
// 1174 
// 1175                     default:
// 1176                       // Unknown command
// 1177                       vs_sendNack( serialFrame.channel, serialFrame.seqnumber );
??main_27:
        ADD      R0,SP,#+4
        LDRB     R1,[R0, #+3]
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+2]
        BL       vs_sendNack
// 1178                       break;
// 1179                   }
// 1180                   break;
// 1181                   
// 1182               } // frame type
// 1183               
// 1184             } // get frame
// 1185             
// 1186           } // new frame
// 1187           
// 1188           // Next Character.
// 1189           readPos++;
??main_11:
        ADDS     R6,R6,#+1
// 1190           
// 1191         } while( --readCnt );
        SUBS     R5,R5,#+1
        CMP      R5,#+0
        BEQ      .+4
        B        ??main_10
// 1192         
// 1193       } // Data available
// 1194 
// 1195       // Send frames to USB host if any pending
// 1196       if ( CANAdapter_bOpen && ( cntInFrames > 0 ) ) { // Only if open
??main_9:
        LDR      R0,??main_20+0x10  ;; CANAdapter_bOpen
        LDR      R0,[R0, #+0]
        CMP      R0,#+0
        BNE      .+4
        B        ??main_1
        LDR      R0,??main_20+0x14  ;; cntInFrames
        LDRH     R0,[R0, #+0]
        CMP      R0,#+1
        BCS      .+4
        B        ??main_1
// 1197         
// 1198         memcpy( &msg, &inputFrames[ posInFetch ], sizeof( canmsg )  );
        MOVS     R2,#+20
        LDR      R0,??main_20+0x18  ;; posInFetch
        LDRH     R0,[R0, #+0]
        MOVS     R1,#+20
        MULS     R0,R1,R0
        LDR      R1,??main_20+0x1C  ;; inputFrames
        ADDS     R1,R1,R0
        ADD      R0,SP,#+28
        BL       memcpy
// 1199         
// 1200         serialFrame.channel = com_channel;
        ADD      R0,SP,#+4
        MOV      R1,SP
        LDRH     R1,[R1, #+2]
        STRB     R1,[R0, #+2]
// 1201 	serialFrame.seqnumber = seqnumber++;
        ADD      R0,SP,#+4
        STRB     R7,[R0, #+3]
        ADDS     R7,R7,#+1
// 1202 	serialFrame.flags = 4 + 4 + msg.Dlc; // id + timestamp + dlc
        ADD      R0,SP,#+28
        LDRB     R0,[R0, #+8]
        ADDS     R0,R0,#+8
        ADD      R1,SP,#+4
        STRB     R0,[R1, #+1]
// 1203 	serialFrame.data[ 3 ] = (uint8_t)(( msg.Id >> 0 ) & 0xff);
        ADD      R0,SP,#+4
        LDR      R1,[SP, #+32]
        STRB     R1,[R0, #+9]
// 1204 	serialFrame.data[ 2 ] = (uint8_t)(( msg.Id >> 9 ) & 0xff);
        LDR      R0,[SP, #+32]
        LSRS     R0,R0,#+9
        ADD      R1,SP,#+4
        STRB     R0,[R1, #+8]
// 1205 	serialFrame.data[ 1 ] = (uint8_t)(( msg.Id >> 16 ) & 0xff);
        LDR      R0,[SP, #+32]
        LSRS     R0,R0,#+16
        ADD      R1,SP,#+4
        STRB     R0,[R1, #+7]
// 1206 	serialFrame.data[ 0 ] = (uint8_t)(( msg.Id >> 24 ) & 0xff);
        LDR      R0,[SP, #+32]
        LSRS     R0,R0,#+24
        ADD      R1,SP,#+4
        STRB     R0,[R1, #+6]
// 1207         if ( CAN_EXT_ID ==  msg.IdType ) {
        LDR      R0,[SP, #+28]
        CMP      R0,#+1
        BNE      ??main_39
// 1208 	  serialFrame.data[ 0 ] |= VSCP_SERIAL_CAN_DATA0_EXTENDED;
        ADD      R0,SP,#+4
        LDRB     R0,[R0, #+6]
        MOVS     R1,#+128
        ORRS     R1,R1,R0
        ADD      R0,SP,#+4
        STRB     R1,[R0, #+6]
// 1209         }
// 1210      
// 1211         
// 1212         // Data
// 1213 	for ( i=0; i<msg.Dlc; i++ ) {
??main_39:
        MOVS     R0,#+0
        MOVS     R4,R0
??main_40:
        ADD      R0,SP,#+28
        LDRB     R0,[R0, #+8]
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        LSLS     R0,R0,#+16       ;; ZeroExtS R0,R0,#+16,#+16
        LSRS     R0,R0,#+16
        CMP      R4,R0
        BCS      ??main_41
// 1214 	  serialFrame.data[ 4 + i ] = msg.Data[ i ];	
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        ADD      R0,SP,#+4
        ADDS     R0,R0,R4
        LSLS     R4,R4,#+16       ;; ZeroExtS R4,R4,#+16,#+16
        LSRS     R4,R4,#+16
        ADD      R1,SP,#+28
        ADDS     R1,R1,R4
        LDRB     R1,[R1, #+9]
        STRB     R1,[R0, #+10]
// 1215 	}
        ADDS     R4,R4,#+1
        B        ??main_40
// 1216         
// 1217         // Timestamp  
// 1218         serialFrame.data[ 7 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 0 ) & 0xff);
??main_41:
        BL       getSysTICs
        ADD      R1,SP,#+28
        LDRB     R1,[R1, #+8]
        ADD      R2,SP,#+4
        ADDS     R1,R2,R1
        STRB     R0,[R1, #+13]
// 1219 	serialFrame.data[ 6 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 9 ) & 0xff);
        BL       getSysTICs
        ADD      R1,SP,#+28
        LDRB     R1,[R1, #+8]
        ADD      R2,SP,#+4
        ADDS     R1,R2,R1
        LSRS     R0,R0,#+9
        STRB     R0,[R1, #+12]
// 1220 	serialFrame.data[ 5 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 16 ) & 0xff);
        BL       getSysTICs
        ADD      R1,SP,#+28
        LDRB     R1,[R1, #+8]
        ADD      R2,SP,#+4
        ADDS     R1,R2,R1
        LSRS     R0,R0,#+16
        STRB     R0,[R1, #+11]
// 1221 	serialFrame.data[ 4 + msg.Dlc ] = (uint8_t)(( getSysTICs() >> 24 ) & 0xff);
        BL       getSysTICs
        ADD      R1,SP,#+28
        LDRB     R1,[R1, #+8]
        ADD      R2,SP,#+4
        ADDS     R1,R2,R1
        LSRS     R0,R0,#+24
        STRB     R0,[R1, #+10]
// 1222         
// 1223 	serialFrame.opcode = VSCP_SERIAL_OPCODE_CAN_FRAME;
        MOV      R0,SP
        MOVS     R1,#+5
        STRB     R1,[R0, #+4]
// 1224 	serialFrame.vscpclass = 0;
        ADD      R0,SP,#+4
        MOVS     R1,#+0
        STRB     R1,[R0, #+4]
// 1225 	serialFrame.vscptype = 0;
        ADD      R0,SP,#+4
        MOVS     R1,#+0
        STRB     R1,[R0, #+5]
// 1226 	
// 1227 	vs_sendFrame( &serialFrame );
        ADD      R0,SP,#+4
        BL       vs_sendFrame
// 1228         
// 1229         posInFetch++;
        LDR      R0,??main_20+0x18  ;; posInFetch
        LDRH     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??main_20+0x18  ;; posInFetch
        STRH     R0,[R1, #+0]
// 1230         if ( posInFetch >= MAX_IN_FRAMES ) posInFetch = 0;
        LDR      R0,??main_20+0x18  ;; posInFetch
        LDRH     R0,[R0, #+0]
        CMP      R0,#+16
        BCC      ??main_42
        LDR      R0,??main_20+0x18  ;; posInFetch
        MOVS     R1,#+0
        STRH     R1,[R0, #+0]
// 1231         cntInFrames--;
??main_42:
        LDR      R0,??main_20+0x14  ;; cntInFrames
        LDRH     R0,[R0, #+0]
        SUBS     R0,R0,#+1
        LDR      R1,??main_20+0x14  ;; cntInFrames
        STRH     R0,[R1, #+0]
        B        ??main_1
// 1232         
// 1233       }
// 1234       
// 1235 
// 1236       // Data from UART
// 1237       //readCnt = UartRead( UART, Buffer, sizeof( Buffer ) - 1 );
// 1238       //if ( readCnt ) {
// 1239       //  while(!UsbCdcWrite( Buffer, readCnt ) );
// 1240       //}
// 1241       //else {
// 1242       //  strcpy( Buffer, "Hello World\n" );
// 1243       //  UsbCdcWriteTimeout( Buffer, 100 );
// 1244       //}
// 1245     } // USB configured
// 1246     else {
// 1247       if ( TRUE == CdcConfigureStateHold ) {
??main_2:
        MOV      R0,SP
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BEQ      .+4
        B        ??main_1
// 1248         CdcConfigureStateHold = FALSE;
        MOV      R0,SP
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??main_1
        DATA
??main_20:
        DC32     lastAck
        DC32     outputFrames
        DC32     0xffffc40c
        DC32     0xffffc418
        DC32     CANAdapter_bOpen
        DC32     cntInFrames
        DC32     posInFetch
        DC32     inputFrames
        CFI EndBlock cfiBlock15
// 1249       }
// 1250     }
// 1251   } // Main loop
// 1252 }
// 1253 
// 1254 ///////////////////////////////////////////////////////////////////////////////
// 1255 // UsbCdcWriteTimeout
// 1256 //
// 1257 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function UsbCdcWriteTimeout
        THUMB
// 1258 Boolean UsbCdcWriteTimeout( uint8_t *Buffer, uint16_t count, uint32_t timeout ) 
// 1259 {
UsbCdcWriteTimeout:
        PUSH     {R0,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
        MOVS     R4,R0
        MOVS     R5,R1
        MOVS     R6,R2
// 1260   uint32_t tics = getSysTICs();
        BL       getSysTICs
        MOVS     R7,R0
// 1261   
// 1262   while ( getElapsedSysTICs( tics ) < timeout ) {
??UsbCdcWriteTimeout_0:
        MOVS     R0,R7
        BL       getElapsedSysTICs
        CMP      R0,R6
        BCS      ??UsbCdcWriteTimeout_1
// 1263     if ( UsbCdcWrite( Buffer, count ) ) return TRUE;   
        LSLS     R5,R5,#+16       ;; ZeroExtS R5,R5,#+16,#+16
        LSRS     R5,R5,#+16
        MOVS     R1,R5
        MOVS     R0,R4
        BL       UsbCdcWrite
        CMP      R0,#+0
        BEQ      ??UsbCdcWriteTimeout_0
        MOVS     R0,#+1
        B        ??UsbCdcWriteTimeout_2
// 1264   }
// 1265   
// 1266   return FALSE;
??UsbCdcWriteTimeout_1:
        MOVS     R0,#+0
??UsbCdcWriteTimeout_2:
        POP      {R3-R7}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock16
// 1267 }
// 1268 
// 1269 
// 1270 ///////////////////////////////////////////////////////////////////////////////
// 1271 // vs_sendBuffer
// 1272 //
// 1273 // This routine is required by the VSCP serial protocol. 
// 1274 //
// 1275 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function vs_sendBuffer
        THUMB
// 1276 BOOL vs_sendBuffer( uint8_t *pbuf, uint16_t count  )
// 1277 {
vs_sendBuffer:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
// 1278   // UsbCdcWrite( pbuf, count );
// 1279   //while ( !UsbCdcWrite( pbuf, count ) );
// 1280   if ( !UsbCdcWriteTimeout(  pbuf, count, 500 ) ) {
        MOVS     R2,#+250
        LSLS     R2,R2,#+1        ;; #+500
        MOVS     R1,R5
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        MOVS     R0,R4
        BL       UsbCdcWriteTimeout
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
// 1281     return FALSE;  
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock17
// 1282   }
// 1283     
// 1284   return TRUE;
// 1285 }
// 1286 
// 1287 ///////////////////////////////////////////////////////////////////////////////
// 1288 // vs_sendSerialByte
// 1289 //
// 1290 // This routine is required by the VSCP serial protocol. 
// 1291 //
// 1292 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function vs_sendSerialByte
        THUMB
// 1293 vs_sendSerialByte( uint8_t b )
// 1294 {
vs_sendSerialByte:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
// 1295   if ( !UsbCdcWriteTimeout( &b, 1, 500 ) ) {
        MOVS     R2,#+250
        LSLS     R2,R2,#+1        ;; #+500
        MOVS     R1,#+1
        MOV      R0,SP
        BL       UsbCdcWriteTimeout
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
// 1296     return FALSE;  
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock18
// 1297   }
// 1298     
// 1299   return TRUE;
// 1300 }

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock19 Using cfiCommon1
        CFI NoFunction
        ARM
__iar_enable_interrupt:
        MRS      R12,CPSR
        BIC      R12,R12,#0xC0
        MSR      CPSR_c,R12
        BX       LR
        CFI EndBlock cfiBlock19

        END
// 
// 1 569 bytes in section .bss
//    61 bytes in section .data
// 3 754 bytes in section .text
// 
// 3 712 bytes of CODE memory (+ 42 bytes shared)
// 1 630 bytes of DATA memory
//
//Errors: none
//Warnings: 4
