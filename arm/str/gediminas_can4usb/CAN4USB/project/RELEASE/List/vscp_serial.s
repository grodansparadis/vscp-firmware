///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:43 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\vscp\firmware\common\vscp_serial.c       /
//    Command line =  D:\development\vscp\firmware\common\vscp_serial.c -lC   /
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
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\v /
//                    scp_serial.s                                            /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME vscp_serial

        RTMODEL "__dlib_file_descriptor", "0"

        EXTERN __aeabi_memcpy
        EXTERN vs_sendBuffer
        EXTERN vs_sendSerialByte

        PUBWEAK memcpy
        PUBLIC vs_addSlidingWndFrame
        PUBLIC vs_bufferSerialByte
        PUBLIC vs_bufferSerialByteSubst
        PUBLIC vs_buffer_cnt
        PUBLIC vs_checksum
        PUBLIC vs_cntSlidingWndFrames
        PUBLIC vs_feed
        PUBLIC vs_findFlyingFrame
        PUBLIC vs_findFreeFlyingFrame
        PUBLIC vs_getFrame
        PUBLIC vs_init
        PUBLIC vs_initSlidingWndFrames
        PUBLIC vs_main_state
        PUBLIC vs_msgbuffer
        PUBLIC vs_removeSlidingWndFrame
        PUBLIC vs_sendAck
        PUBLIC vs_sendCommandFrame
        PUBLIC vs_sendErrorFrame
        PUBLIC vs_sendFrame
        PUBLIC vs_sendNack
        PUBLIC vs_sendReplyFrame
        PUBLIC vs_sendResponse
        PUBLIC vs_sub_state
        PUBLIC vs_timeoutSlidingWndFrame
        
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
        
// D:\development\vscp\firmware\common\vscp_serial.c
//    1  /**
//    2  * @brief           VSCP Serial Protocol
//    3  * @file            vscp_serial.c
//    4  * @author          Ake Hedman, D of Scandinavia, www.vscp.org
//    5  *
//    6  * @section description Description
//    7  **********************************
//    8  * This module contains the code that implements the
//    9  * VSCP serial protocol.
//   10  *********************************************************************/
//   11 
//   12 /* ******************************************************************************
//   13  * VSCP (Very Simple Control Protocol) 
//   14  * http://www.vscp.org
//   15  *
//   16  * Copyright (C) 1995-2008 Ake Hedman, 
//   17  * D of Scandinavia, <akhe@dofscandinavia.com>
//   18  *
//   19  * This software is provided 'as-is', without any express or implied
//   20  * warranty.  In no event will the authors be held liable for any damages
//   21  * arising from the use of this software.
//   22  *
//   23  * Permission is granted to anyone to use this software for any purpose,
//   24  * including commercial applications, and to alter it and redistribute it
//   25  * freely, subject to the following restrictions:
//   26  *
//   27  * 1. The origin of this software must not be misrepresented; you must not
//   28  *    claim that you wrote the original software. If you use this software
//   29  *    in a product, an acknowledgment in the product documentation would be
//   30  *    appreciated but is not required.
//   31  * 2. Altered source versions must be plainly marked as such, and must not be
//   32  *    misrepresented as being the original software.
//   33  * 3. This notice may not be removed or altered from any source distribution.
//   34  * 
//   35  *  This file is part of VSCP - Very Simple Control Protocol 	
//   36  *  http://www.vscp.org
//   37  *
//   38  * ******************************************************************************
//   39 */
//   40 
//   41 #include <stdio.h>
//   42 #include <string.h>

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
//   43 #include "vscp.h"
//   44 #include "vscp_projdefs.h"
//   45 #include "vscp_serial.h"
//   46 #include "crc.h"
//   47 
//   48 // Globals

        SECTION `.bss`:DATA:NOROOT(0)
//   49 uint8_t vs_main_state;     // state for the state machine
vs_main_state:
        DS8 1

        SECTION `.bss`:DATA:NOROOT(0)
//   50 uint8_t vs_sub_state;      // substate for the state machine
vs_sub_state:
        DS8 1

        SECTION `.bss`:DATA:NOROOT(2)
//   51 uint8_t vs_msgbuffer[ VSCP_SERIAL_BUFFER_SIZE ];  // Buffer for current event 
vs_msgbuffer:
        DS8 48

        SECTION `.bss`:DATA:NOROOT(0)
//   52 uint8_t vs_buffer_cnt;     // Character count for buffer
vs_buffer_cnt:
        DS8 1

        SECTION `.bss`:DATA:NOROOT(0)
//   53 uint8_t vs_checksum;       // Packet checksum
vs_checksum:
        DS8 1
//   54 
//   55 #ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 
//   56 
//   57 // Flying windows frames

        SECTION `.bss`:DATA:NOROOT(0)
//   58 uint8_t vs_cntSlidingWndFrames;   // Number of outstanding sliding window frames
vs_cntSlidingWndFrames:
        DS8 1
//   59 
//   60 // Array with info about outstanding flying window frames 
//   61 //vscp_serial_SlidingWndStruct slidingWndFrames[ VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ];   
//   62 
//   63 #endif
//   64 
//   65 
//   66 
//   67 ///////////////////////////////////////////////////////////////////////////////
//   68 // vs_sendchar_subst
//   69 //
//   70 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock1 Using cfiCommon0
        CFI Function vs_bufferSerialByte
        THUMB
//   71 void vs_bufferSerialByte( uint8_t b )
//   72 {
vs_bufferSerialByte:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
//   73   if ( vs_buffer_cnt >= VSCP_SERIAL_BUFFER_SIZE ) return;
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        LDRB     R1,[R1, #+0]
        CMP      R1,#+46
        BCS      ??vs_bufferSerialByte_0
//   74   vs_msgbuffer[ vs_buffer_cnt++ ] = b;
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        LDRB     R1,[R1, #+0]
        LDR      R2,??DataTable53  ;; vs_msgbuffer
        STRB     R0,[R2, R1]
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        LDRB     R1,[R1, #+0]
        ADDS     R1,R1,#+1
        LDR      R2,??DataTable50  ;; vs_buffer_cnt
        STRB     R1,[R2, #+0]
//   75 }
??vs_bufferSerialByte_0:
        POP      {R3}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock1
//   76 
//   77 
//   78 ///////////////////////////////////////////////////////////////////////////////
//   79 // vs_sendchar_subst
//   80 //
//   81 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock2 Using cfiCommon0
        CFI Function vs_bufferSerialByteSubst
        THUMB
//   82 BOOL vs_bufferSerialByteSubst( uint8_t *pcsum, uint8_t b )
//   83 {
vs_bufferSerialByteSubst:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
//   84   if ( DLE == b ) {
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+16
        BNE      ??vs_bufferSerialByteSubst_0
//   85     vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//   86     vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//   87     *pcsum ^= DLE; // Only one is used for checksum calculations
        LDRB     R0,[R4, #+0]
        MOVS     R1,#+16
        EORS     R1,R1,R0
        STRB     R1,[R4, #+0]
        B        ??vs_bufferSerialByteSubst_1
//   88   }
//   89   else {
//   90     vs_bufferSerialByte( b );	
??vs_bufferSerialByteSubst_0:
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       vs_bufferSerialByte
//   91     *pcsum ^= b;
        LDRB     R0,[R4, #+0]
        EORS     R0,R0,R5
        STRB     R0,[R4, #+0]
//   92   }
//   93   
//   94   return TRUE;
??vs_bufferSerialByteSubst_1:
        MOVS     R0,#+1
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock2
//   95 }
//   96 
//   97 
//   98 ///////////////////////////////////////////////////////////////////////////////
//   99 // vscp_serial_init
//  100 //
//  101 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock3 Using cfiCommon0
        CFI Function vs_init
        THUMB
//  102 void vs_init( void ) 
//  103 {
//  104    vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_NONE;
vs_init:
        LDR      R0,??DataTable51  ;; vs_main_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  105    vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE; 
        LDR      R0,??DataTable52  ;; vs_sub_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  106    vs_buffer_cnt = 0;
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  107    vs_checksum = 0;
        LDR      R0,??DataTable45  ;; vs_checksum
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  108 }
        BX       LR               ;; return
        CFI EndBlock cfiBlock3
//  109 
//  110 ///////////////////////////////////////////////////////////////////////////////
//  111 // vscp_serial_feed
//  112 //
//  113 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock4 Using cfiCommon0
        CFI Function vs_feed
        THUMB
//  114 BOOL vs_feed( uint8_t b )
//  115 {
vs_feed:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  116   BOOL rv = FALSE;
        MOVS     R0,#+0
        MOVS     R5,R0
//  117   
//  118   switch ( b ) {
        LSLS     R4,R4,#+24       ;; ZeroExtS R4,R4,#+24,#+24
        LSRS     R4,R4,#+24
        MOVS     R0,R4
        SUBS     R0,R0,#+1
        BEQ      ??vs_feed_0
        SUBS     R0,R0,#+1
        BEQ      ??vs_feed_1
        SUBS     R0,R0,#+14
        BNE      ??vs_feed_2
//  119   
//  120     case DLE: // escape
//  121       if ( VSCP_SERIAL_PROTOCOL_STATE_NONE == vs_main_state ) {
??vs_feed_3:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_feed_4
//  122         vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE;  
        LDR      R0,??DataTable52  ;; vs_sub_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??vs_feed_5
//  123       }
//  124       else if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {
??vs_feed_4:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_5
//  125         
//  126         if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_6
//  127           // Second DLE => it's a DLE character
//  128           vs_checksum ^= DLE;
        LDR      R0,??DataTable45  ;; vs_checksum
        LDRB     R0,[R0, #+0]
        MOVS     R1,#+16
        EORS     R1,R1,R0
        LDR      R0,??DataTable45  ;; vs_checksum
        STRB     R1,[R0, #+0]
//  129           vs_msgbuffer[ vs_buffer_cnt++ ] = DLE;
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        LDR      R1,??DataTable53  ;; vs_msgbuffer
        MOVS     R2,#+16
        STRB     R2,[R1, R0]
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        STRB     R0,[R1, #+0]
//  130           vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE; 
        LDR      R0,??DataTable52  ;; vs_sub_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
        B        ??vs_feed_5
//  131         }
//  132         else {
//  133           vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE;
??vs_feed_6:
        LDR      R0,??DataTable52  ;; vs_sub_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??vs_feed_5
//  134         }
//  135         
//  136       }
//  137       break;
//  138      
//  139     case STX: // start
//  140       if ( VSCP_SERIAL_PROTOCOL_STATE_NONE == vs_main_state ) {
??vs_feed_0:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_feed_7
//  141         
//  142         if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_5
//  143           // Here if waiting for packet and escape has been received.
//  144           // Start to receive a package    
//  145           vs_init();
        BL       vs_init
//  146           vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_PACKET; 
        LDR      R0,??DataTable51  ;; vs_main_state
        MOVS     R1,#+1
        STRB     R1,[R0, #+0]
        B        ??vs_feed_5
//  147         }
//  148         
//  149       }
//  150       else if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {
??vs_feed_7:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_5
//  151         
//  152         if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_8
//  153           // DLE STX in package - Start all over
//  154           vs_init();
        BL       vs_init
        B        ??vs_feed_5
//  155         }
//  156         else if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE == vs_sub_state ) {
??vs_feed_8:
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_feed_5
//  157           // Just a stray STX
//  158           vs_checksum ^= b;
        LDR      R0,??DataTable45  ;; vs_checksum
        LDRB     R0,[R0, #+0]
        EORS     R0,R0,R4
        LDR      R1,??DataTable45  ;; vs_checksum
        STRB     R0,[R1, #+0]
//  159           vs_msgbuffer[ vs_buffer_cnt++ ] = b; 
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        LDR      R1,??DataTable53  ;; vs_msgbuffer
        STRB     R4,[R1, R0]
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        STRB     R0,[R1, #+0]
        B        ??vs_feed_5
//  160         }
//  161         
//  162       }
//  163       break;
//  164       
//  165     case ETX: // End
//  166       if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {
??vs_feed_1:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_5
//  167         
//  168         if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_9
//  169           
//  170           // DLE ETX in package - We have received a packet
//  171           // Check the packet checksum and return OK
//  172           
//  173         if ( 0 == vs_checksum ) {
        LDR      R0,??DataTable45  ;; vs_checksum
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_feed_10
//  174             rv = TRUE;
        MOVS     R0,#+1
        MOVS     R5,R0
//  175             // Ack the frame
//  176             if (  ( VSCP_SERIAL_OPCODE_ACK != vs_msgbuffer[ 0 ] ) && 
//  177                 ( VSCP_SERIAL_OPCODE_SENT_ACK != vs_msgbuffer[ 0 ] ) ) {
//  178 							//vs_sendAck( vs_msgbuffer[ 2 ], vs_msgbuffer[ 3 ] );
//  179             }
//  180           }
//  181           else {
//  182             // Nack the frame
//  183             if (  ( VSCP_SERIAL_OPCODE_NACK != vs_msgbuffer[ 0 ] ) && 
//  184                 ( VSCP_SERIAL_OPCODE_SENT_NACK != vs_msgbuffer[ 0 ] ) ) {
//  185 							//vs_sendNack( vs_msgbuffer[ 2 ], vs_msgbuffer[ 3 ] );
//  186             }
//  187           }
//  188         
//  189           vs_init();
??vs_feed_10:
        BL       vs_init
        B        ??vs_feed_5
//  190           
//  191         }
//  192         else if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE == vs_sub_state ) {
??vs_feed_9:
        LDR      R0,??DataTable52  ;; vs_sub_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_feed_5
//  193           // Just a stray ETX
//  194           vs_checksum ^= b;
        LDR      R0,??DataTable45  ;; vs_checksum
        LDRB     R0,[R0, #+0]
        EORS     R0,R0,R4
        LDR      R1,??DataTable45  ;; vs_checksum
        STRB     R0,[R1, #+0]
//  195           vs_msgbuffer[ vs_buffer_cnt++ ] = b; 
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        LDR      R1,??DataTable53  ;; vs_msgbuffer
        STRB     R4,[R1, R0]
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        STRB     R0,[R1, #+0]
        B        ??vs_feed_5
//  196         }
//  197         
//  198       }
//  199       break;
//  200       
//  201     default:
//  202       // Receiving packet
//  203       if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {
??vs_feed_2:
        LDR      R0,??DataTable51  ;; vs_main_state
        LDRB     R0,[R0, #+0]
        CMP      R0,#+1
        BNE      ??vs_feed_5
//  204         vs_checksum ^= b;
        LDR      R0,??DataTable45  ;; vs_checksum
        LDRB     R0,[R0, #+0]
        EORS     R0,R0,R4
        LDR      R1,??DataTable45  ;; vs_checksum
        STRB     R0,[R1, #+0]
//  205         vs_msgbuffer[ vs_buffer_cnt++ ] = b;
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        LDR      R1,??DataTable53  ;; vs_msgbuffer
        STRB     R4,[R1, R0]
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        LDR      R1,??DataTable50  ;; vs_buffer_cnt
        STRB     R0,[R1, #+0]
//  206       }
//  207       break;
//  208   }
//  209   
//  210   // Check for buffer overflow
//  211   if ( vs_buffer_cnt > VSCP_SERIAL_FRAME_SIZE ) {
??vs_feed_5:
        LDR      R0,??DataTable50  ;; vs_buffer_cnt
        LDRB     R0,[R0, #+0]
        CMP      R0,#+24
        BCC      ??vs_feed_11
//  212     // Start all over
//  213     vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_NONE;
        LDR      R0,??DataTable51  ;; vs_main_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  214     vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE;
        LDR      R0,??DataTable52  ;; vs_sub_state
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  215   }
//  216   
//  217   return rv;
??vs_feed_11:
        MOVS     R0,R5
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock4
//  218 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable45:
        DC32     vs_checksum

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable50:
        DC32     vs_buffer_cnt

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable51:
        DC32     vs_main_state

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable52:
        DC32     vs_sub_state
//  219              
//  220 
//  221 ///////////////////////////////////////////////////////////////////////////////
//  222 // vs_getFrame
//  223 // 
//  224              

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock5 Using cfiCommon0
        CFI Function vs_getFrame
        THUMB
//  225 BOOL vs_getFrame( vs_frame *pFrame )
//  226 {     
vs_getFrame:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R4,R0
//  227   // Check pointer
//  228   if ( NULL == pFrame ) return FALSE; 
        CMP      R4,#+0
        BNE      ??vs_getFrame_0
        MOVS     R0,#+0
        B        ??vs_getFrame_1
//  229   
//  230   pFrame->opcode = vs_msgbuffer[ 0 ];
??vs_getFrame_0:
        LDR      R0,??DataTable53  ;; vs_msgbuffer
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+0]
//  231   pFrame->flags = vs_msgbuffer[ 1 ];
        LDR      R0,??vs_getFrame_2  ;; vs_msgbuffer + 1
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+1]
//  232   pFrame->channel = vs_msgbuffer[ 2 ];
        LDR      R0,??vs_getFrame_2+0x4  ;; vs_msgbuffer + 2
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+2]
//  233   pFrame->seqnumber = vs_msgbuffer[ 3 ];
        LDR      R0,??vs_getFrame_2+0x8  ;; vs_msgbuffer + 3
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+3]
//  234   pFrame->vscpclass = vs_msgbuffer[ 4 ];
        LDR      R0,??vs_getFrame_2+0xC  ;; vs_msgbuffer + 4
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+4]
//  235   pFrame->vscptype = vs_msgbuffer [ 5 ];
        LDR      R0,??vs_getFrame_2+0x10  ;; vs_msgbuffer + 5
        LDRB     R0,[R0, #+0]
        STRB     R0,[R4, #+5]
//  236   memcpy( pFrame->data, ( vs_msgbuffer + 6 ), 16 );
        MOVS     R2,#+16
        LDR      R1,??vs_getFrame_2+0x14  ;; vs_msgbuffer + 6
        MOVS     R0,R4
        ADDS     R0,R0,#+6
        BL       memcpy
//  237 
//  238   return TRUE;
        MOVS     R0,#+1
??vs_getFrame_1:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        DATA
??vs_getFrame_2:
        DC32     vs_msgbuffer + 1
        DC32     vs_msgbuffer + 2
        DC32     vs_msgbuffer + 3
        DC32     vs_msgbuffer + 4
        DC32     vs_msgbuffer + 5
        DC32     vs_msgbuffer + 6
        CFI EndBlock cfiBlock5
//  239 }

        SECTION `.text`:CODE:NOROOT(2)
        DATA
??DataTable53:
        DC32     vs_msgbuffer
//  240              
//  241 
//  242 ///////////////////////////////////////////////////////////////////////////////
//  243 // vs_sendResponse
//  244 //
//  245 // Sent inline so it can be used inside frame feed routine
//  246 //
//  247 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock6 Using cfiCommon0
        CFI Function vs_sendResponse
        THUMB
//  248 BOOL vs_sendResponse( uint8_t opcode, uint8_t channel, uint8_t seqnumber )
//  249 {
vs_sendResponse:
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
//  250   uint8_t checksum = 0;
        MOVS     R0,#+0
        MOVS     R7,R0
//  251  
//  252   // Start of frame
//  253 	if ( !vs_sendSerialByte( DLE ) ) {
        MOVS     R0,#+16
        BL       vs_sendSerialByte
        CMP      R0,#+0
        BNE      ??vs_sendResponse_0
//  254 		return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendResponse_1
//  255 	}
//  256 	if ( !vs_sendSerialByte( STX ) ) {
??vs_sendResponse_0:
        MOVS     R0,#+1
        BL       vs_sendSerialByte
        CMP      R0,#+0
        BNE      ??vs_sendResponse_2
//  257 		return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendResponse_1
//  258 	}
//  259   
//  260   // opcode Code
//  261 	if ( !vs_sendSerialByte( opcode ) ) {
??vs_sendResponse_2:
        MOVS     R0,R4
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       vs_sendSerialByte
        CMP      R0,#+0
        BNE      ??vs_sendResponse_3
//  262 		return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendResponse_1
//  263 	}
//  264 	checksum ^= opcode;
??vs_sendResponse_3:
        EORS     R7,R7,R4
//  265 
//  266 	// Flags
//  267 	if ( !vs_sendSerialByte( 0 ) ) {
        MOVS     R0,#+0
        BL       vs_sendSerialByte
        CMP      R0,#+0
        BNE      ??vs_sendResponse_4
//  268 		return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendResponse_1
//  269 	}
//  270 	checksum ^= 0;
//  271   
//  272   // Channel
//  273 	if ( DLE == channel ) {
??vs_sendResponse_4:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R5,#+16
        BNE      ??vs_sendResponse_5
//  274     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
//  275     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
//  276     checksum ^= DLE; // Only one is used for checksum calculations
        MOVS     R0,R7
        MOVS     R7,#+16
        EORS     R7,R7,R0
        B        ??vs_sendResponse_6
//  277   }
//  278   else {
//  279     vs_sendSerialByte( channel );	
??vs_sendResponse_5:
        MOVS     R0,R5
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       vs_sendSerialByte
//  280     checksum ^= channel;
        EORS     R7,R7,R5
//  281   }
//  282   
//  283   // Counter
//  284 	if ( DLE == seqnumber ) {
??vs_sendResponse_6:
        LSLS     R6,R6,#+24       ;; ZeroExtS R6,R6,#+24,#+24
        LSRS     R6,R6,#+24
        CMP      R6,#+16
        BNE      ??vs_sendResponse_7
//  285     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
//  286     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
//  287     checksum ^= DLE; // Only one is used for checksum calculations
        MOVS     R0,R7
        MOVS     R7,#+16
        EORS     R7,R7,R0
        B        ??vs_sendResponse_8
//  288   }
//  289   else {
//  290     vs_sendSerialByte( seqnumber );	
??vs_sendResponse_7:
        MOVS     R0,R6
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       vs_sendSerialByte
//  291     checksum ^= seqnumber;
        EORS     R7,R7,R6
//  292   }
//  293   
//  294   // Checksum
//  295 	if ( DLE == checksum ) {
??vs_sendResponse_8:
        LSLS     R7,R7,#+24       ;; ZeroExtS R7,R7,#+24,#+24
        LSRS     R7,R7,#+24
        CMP      R7,#+16
        BNE      ??vs_sendResponse_9
//  296     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
//  297     vs_sendSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_sendSerialByte
        B        ??vs_sendResponse_10
//  298   }
//  299   else {
//  300     vs_sendSerialByte( checksum );	
??vs_sendResponse_9:
        MOVS     R0,R7
        LSLS     R0,R0,#+24       ;; ZeroExtS R0,R0,#+24,#+24
        LSRS     R0,R0,#+24
        BL       vs_sendSerialByte
//  301   }
//  302   
//  303   // End of frame
//  304 	if ( !vs_sendSerialByte( DLE ) ) {
??vs_sendResponse_10:
        MOVS     R0,#+16
        BL       vs_sendSerialByte
        CMP      R0,#+0
        BNE      ??vs_sendResponse_11
//  305 		return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendResponse_1
//  306 	}
//  307 	if ( !vs_sendSerialByte( ETX ) ) {
??vs_sendResponse_11:
        MOVS     R0,#+2
        BL       vs_sendSerialByte
        SUBS     R0,R0,#+1
        SBCS     R0,R0,R0
        MVNS     R0,R0
        LSRS     R0,R0,#+31
//  308 		return FALSE;
??vs_sendResponse_1:
        POP      {R3-R7}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock6
//  309 	}
//  310   
//  311   return TRUE; 		
//  312 }
//  313 
//  314 ///////////////////////////////////////////////////////////////////////////////
//  315 // vs_sendAck
//  316 //
//  317 // Sent inline so it can be used inside frame feed routine
//  318 //
//  319 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock7 Using cfiCommon0
        CFI Function vs_sendAck
        THUMB
//  320 BOOL vs_sendAck( uint8_t channel, uint8_t seqnumber )
//  321 {
vs_sendAck:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
//  322 	return vs_sendResponse( VSCP_SERIAL_OPCODE_ACK,
//  323 														channel,
//  324 														seqnumber );
        MOVS     R2,R5
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        MOVS     R1,R4
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R0,#+251
        BL       vs_sendResponse
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock7
//  325 }
//  326              
//  327 ///////////////////////////////////////////////////////////////////////////////
//  328 // vs_sendNack
//  329 //             
//  330 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock8 Using cfiCommon0
        CFI Function vs_sendNack
        THUMB
//  331 BOOL vs_sendNack( uint8_t channel, uint8_t seqnumber )
//  332 {
vs_sendNack:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
        MOVS     R5,R1
//  333 	return vs_sendResponse( VSCP_SERIAL_OPCODE_NACK,
//  334 														channel,
//  335 														seqnumber );
        MOVS     R2,R5
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        MOVS     R1,R4
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOVS     R0,#+252
        BL       vs_sendResponse
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock8
//  336 }
//  337        
//  338 ///////////////////////////////////////////////////////////////////////////////
//  339 // vs_sendFrame
//  340 //             
//  341 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock9 Using cfiCommon0
        CFI Function vs_sendFrame
        THUMB
//  342 BOOL vs_sendFrame( vs_frame *pFrame )
//  343 {
vs_sendFrame:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R4,R0
//  344   int i;
//  345   uint8_t checksum = 0;
        MOV      R0,SP
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  346   
//  347   vs_buffer_cnt = 0;
        LDR      R0,??vs_sendFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  348       
//  349   // Check pointer
//  350   if ( NULL == pFrame ) return FALSE; 
        CMP      R4,#+0
        BNE      ??vs_sendFrame_1
        MOVS     R0,#+0
        B        ??vs_sendFrame_2
//  351   
//  352   // Start of frame
//  353   vs_bufferSerialByte( DLE );
??vs_sendFrame_1:
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  354   vs_bufferSerialByte( STX );
        MOVS     R0,#+1
        BL       vs_bufferSerialByte
//  355        
//  356   // Operation     
//  357   vs_bufferSerialByteSubst( &checksum, pFrame->opcode );    
        LDRB     R1,[R4, #+0]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  358        
//  359   // Flags     
//  360   vs_bufferSerialByteSubst( &checksum, pFrame->flags ); 
        LDRB     R1,[R4, #+1]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  361   
//  362   // Channel
//  363   vs_bufferSerialByteSubst( &checksum, pFrame->channel );
        LDRB     R1,[R4, #+2]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  364   
//  365   // Counter
//  366   vs_bufferSerialByteSubst( &checksum, pFrame->seqnumber );
        LDRB     R1,[R4, #+3]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  367   
//  368   // VSCP Class
//  369   vs_bufferSerialByteSubst( &checksum, pFrame->vscpclass );
        LDRB     R1,[R4, #+4]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  370   
//  371   //  VSCP type
//  372   vs_bufferSerialByteSubst( &checksum, pFrame->vscptype );
        LDRB     R1,[R4, #+5]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  373 
//  374   // data
//  375   for ( i=0; i < ( pFrame->flags & 0x1f ); i++ ) {
        MOVS     R0,#+0
        MOVS     R5,R0
??vs_sendFrame_3:
        LDRB     R0,[R4, #+1]
        LSLS     R0,R0,#+27       ;; ZeroExtS R0,R0,#+27,#+27
        LSRS     R0,R0,#+27
        CMP      R5,R0
        BGE      ??vs_sendFrame_4
//  376 		vs_bufferSerialByteSubst( &checksum, pFrame->data[ i ] );
        ADDS     R0,R4,R5
        LDRB     R1,[R0, #+6]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  377   }
        ADDS     R5,R5,#+1
        B        ??vs_sendFrame_3
//  378 
//  379   //  Checksum - Note: checksum byte(s) sent before checksum calcuulated so OK
//  380   vs_bufferSerialByteSubst( &checksum, checksum );
??vs_sendFrame_4:
        MOV      R0,SP
        LDRB     R1,[R0, #+0]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  381   
//  382   // End of frame
//  383   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  384   vs_bufferSerialByte( ETX );     
        MOVS     R0,#+2
        BL       vs_bufferSerialByte
//  385   
//  386   // Send the buffer content
//  387   if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        LDR      R0,??vs_sendFrame_0  ;; vs_buffer_cnt
        LDRB     R1,[R0, #+0]
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LDR      R0,??vs_sendFrame_0+0x4  ;; vs_msgbuffer
        BL       vs_sendBuffer
        CMP      R0,#+0
        BNE      ??vs_sendFrame_5
//  388     vs_buffer_cnt = 0;
        LDR      R0,??vs_sendFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  389     return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendFrame_2
//  390   }
//  391 
//  392   vs_buffer_cnt = 0;
??vs_sendFrame_5:
        LDR      R0,??vs_sendFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  393   
//  394   return TRUE;
        MOVS     R0,#+1
??vs_sendFrame_2:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        DATA
??vs_sendFrame_0:
        DC32     vs_buffer_cnt
        DC32     vs_msgbuffer
        CFI EndBlock cfiBlock9
//  395 }
//  396 
//  397 
//  398 ///////////////////////////////////////////////////////////////////////////////
//  399 // vs_sendCommandFrame
//  400 //             
//  401 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock10 Using cfiCommon0
        CFI Function vs_sendCommandFrame
        THUMB
//  402 BOOL vs_sendCommandFrame( uint8_t channel, 
//  403                             uint8_t seqnumber, 
//  404 														uint16_t command,
//  405                             uint8_t count,  
//  406                             uint8_t *pData  )
//  407 {
vs_sendCommandFrame:
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
        LDR      R6,[SP, #+32]
//  408   int i;
//  409   uint8_t checksum = 0;
        MOV      R0,SP
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  410   
//  411   vs_buffer_cnt = 0;
        LDR      R0,??vs_sendCommandFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  412   
//  413   // Check pointer - Must be at least command byte
//  414   if ( NULL == pData ) return FALSE;
        CMP      R6,#+0
        BNE      ??vs_sendCommandFrame_1
        MOVS     R0,#+0
        B        ??vs_sendCommandFrame_2
//  415   
//  416   count &= 0x1f;  // Max 16 bytes
??vs_sendCommandFrame_1:
        LSLS     R5,R5,#+27       ;; ZeroExtS R5,R5,#+27,#+27
        LSRS     R5,R5,#+27
//  417   
//  418   // Start of frame
//  419   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  420   vs_bufferSerialByte( STX );
        MOVS     R0,#+1
        BL       vs_bufferSerialByte
//  421   
//  422   // ACK Code
//  423   vs_bufferSerialByteSubst( &checksum, VSCP_SERIAL_OPCODE_COMMAND );
        MOVS     R1,#+255
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  424   
//  425   // Flags: Data byte count
//  426   vs_bufferSerialByteSubst( &checksum, count );
        MOVS     R1,R5
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  427   
//  428   // Channel
//  429   vs_bufferSerialByteSubst( &checksum, channel );
        MOV      R0,SP
        LDRB     R1,[R0, #+4]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  430 
//  431 	// Counter
//  432   vs_bufferSerialByteSubst( &checksum, seqnumber );
        MOV      R0,SP
        LDRB     R1,[R0, #+8]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  433 
//  434 	// Command
//  435 	vs_bufferSerialByteSubst( &checksum, ( ( command >> 8 ) & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSRS     R1,R1,#+8
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  436 	vs_bufferSerialByteSubst( &checksum, ( command & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  437   
//  438   for ( i = 0; i<count; i++ ) {
        MOVS     R0,#+0
        MOVS     R7,R0
??vs_sendCommandFrame_3:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R7,R5
        BGE      ??vs_sendCommandFrame_4
//  439     vs_bufferSerialByteSubst( &checksum, pData[ i ] );
        LDRB     R1,[R6, R7]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  440   }
        ADDS     R7,R7,#+1
        B        ??vs_sendCommandFrame_3
//  441   
//  442   // Checksum
//  443   vs_bufferSerialByteSubst( &checksum, checksum );
??vs_sendCommandFrame_4:
        MOV      R0,SP
        LDRB     R1,[R0, #+0]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  444   
//  445   // End of frame
//  446   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  447   vs_bufferSerialByte( ETX );
        MOVS     R0,#+2
        BL       vs_bufferSerialByte
//  448   
//  449   // Send the buffer content
//  450 	if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        LDR      R0,??vs_sendCommandFrame_0  ;; vs_buffer_cnt
        LDRB     R1,[R0, #+0]
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LDR      R0,??vs_sendCommandFrame_0+0x4  ;; vs_msgbuffer
        BL       vs_sendBuffer
        CMP      R0,#+0
        BNE      ??vs_sendCommandFrame_5
//  451 	  vs_buffer_cnt = 0;
        LDR      R0,??vs_sendCommandFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  452 	  return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendCommandFrame_2
//  453   }
//  454 
//  455   vs_buffer_cnt = 0;
??vs_sendCommandFrame_5:
        LDR      R0,??vs_sendCommandFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  456     
//  457   return TRUE;  
        MOVS     R0,#+1
??vs_sendCommandFrame_2:
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        DATA
??vs_sendCommandFrame_0:
        DC32     vs_buffer_cnt
        DC32     vs_msgbuffer
        CFI EndBlock cfiBlock10
//  458 }
//  459 
//  460 
//  461 ///////////////////////////////////////////////////////////////////////////////
//  462 // vs_sendErrorFrame
//  463 //             
//  464 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock11 Using cfiCommon0
        CFI Function vs_sendErrorFrame
        THUMB
//  465 BOOL vs_sendErrorFrame( uint8_t channel, 
//  466                           uint8_t seqnumber, 
//  467 													uint16_t errorcode,
//  468                           uint8_t count,  
//  469                           uint8_t *pData  )
//  470 {
vs_sendErrorFrame:
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
        LDR      R6,[SP, #+32]
//  471   int i;
//  472   uint8_t checksum = 0;
        MOV      R0,SP
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  473   
//  474   vs_buffer_cnt = 0;
        LDR      R0,??vs_sendErrorFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  475   
//  476   // Check pointer - must at least be error byte
//  477   if ( NULL == pData ) return FALSE;
        CMP      R6,#+0
        BNE      ??vs_sendErrorFrame_1
        MOVS     R0,#+0
        B        ??vs_sendErrorFrame_2
//  478   
//  479   count &= 0x1f;  // Max 16 bytes
??vs_sendErrorFrame_1:
        LSLS     R5,R5,#+27       ;; ZeroExtS R5,R5,#+27,#+27
        LSRS     R5,R5,#+27
//  480   
//  481   // Start of frame
//  482   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  483   vs_bufferSerialByte( STX );
        MOVS     R0,#+1
        BL       vs_bufferSerialByte
//  484   
//  485   // ACK Code
//  486   vs_bufferSerialByteSubst( &checksum, VSCP_SERIAL_OPCODE_ERROR );
        MOVS     R1,#+253
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  487   
//  488   // Flags: Data byte count
//  489   vs_bufferSerialByteSubst( &checksum, count );
        MOVS     R1,R5
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  490   
//  491   // Channel
//  492   vs_bufferSerialByteSubst( &checksum, channel );
        MOV      R0,SP
        LDRB     R1,[R0, #+4]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  493   
//  494   // Counter
//  495   vs_bufferSerialByteSubst( &checksum, seqnumber );
        MOV      R0,SP
        LDRB     R1,[R0, #+8]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  496 
//  497 	// Errorcode
//  498 	vs_bufferSerialByteSubst( &checksum, ( ( errorcode >> 8 ) & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSRS     R1,R1,#+8
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  499 	vs_bufferSerialByteSubst( &checksum, ( errorcode & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  500   
//  501   for ( i = 0; i<count; i++ ) {
        MOVS     R0,#+0
        MOVS     R7,R0
??vs_sendErrorFrame_3:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R7,R5
        BGE      ??vs_sendErrorFrame_4
//  502     vs_bufferSerialByteSubst( &checksum, pData[ i ] );
        LDRB     R1,[R6, R7]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  503   }
        ADDS     R7,R7,#+1
        B        ??vs_sendErrorFrame_3
//  504   
//  505   // Checksum
//  506   vs_bufferSerialByteSubst( &checksum, checksum );
??vs_sendErrorFrame_4:
        MOV      R0,SP
        LDRB     R1,[R0, #+0]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  507   
//  508   // End of frame
//  509   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  510   vs_bufferSerialByte( ETX );
        MOVS     R0,#+2
        BL       vs_bufferSerialByte
//  511   
//  512   // Send the buffer content
//  513   if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        LDR      R0,??vs_sendErrorFrame_0  ;; vs_buffer_cnt
        LDRB     R1,[R0, #+0]
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LDR      R0,??vs_sendErrorFrame_0+0x4  ;; vs_msgbuffer
        BL       vs_sendBuffer
        CMP      R0,#+0
        BNE      ??vs_sendErrorFrame_5
//  514 	  vs_buffer_cnt = 0;
        LDR      R0,??vs_sendErrorFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  515 	  return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendErrorFrame_2
//  516   }
//  517 
//  518   vs_buffer_cnt = 0;
??vs_sendErrorFrame_5:
        LDR      R0,??vs_sendErrorFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  519     
//  520   return TRUE;  
        MOVS     R0,#+1
??vs_sendErrorFrame_2:
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        DATA
??vs_sendErrorFrame_0:
        DC32     vs_buffer_cnt
        DC32     vs_msgbuffer
        CFI EndBlock cfiBlock11
//  521 }
//  522 
//  523 ///////////////////////////////////////////////////////////////////////////////
//  524 // vs_sendReplyFrame
//  525 
//  526 //             
//  527 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock12 Using cfiCommon0
        CFI Function vs_sendReplyFrame
        THUMB
//  528 BOOL vs_sendReplyFrame( uint8_t channel, 
//  529                           uint8_t seqnumber, 
//  530 													uint16_t replycode,
//  531                           uint8_t count,  
//  532                           uint8_t *pData  )
//  533 {
vs_sendReplyFrame:
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
        LDR      R6,[SP, #+32]
//  534   int i;
//  535   uint8_t checksum = 0;
        MOV      R0,SP
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  536   
//  537   vs_buffer_cnt = 0;
        LDR      R0,??vs_sendReplyFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  538   
//  539   // Check pointer - must at least be error byte
//  540   if ( NULL == pData ) return FALSE;
        CMP      R6,#+0
        BNE      ??vs_sendReplyFrame_1
        MOVS     R0,#+0
        B        ??vs_sendReplyFrame_2
//  541   
//  542   count &= 0x1f;  // Max 16 bytes
??vs_sendReplyFrame_1:
        LSLS     R5,R5,#+27       ;; ZeroExtS R5,R5,#+27,#+27
        LSRS     R5,R5,#+27
//  543   
//  544   // Start of frame
//  545   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  546   vs_bufferSerialByte( STX );
        MOVS     R0,#+1
        BL       vs_bufferSerialByte
//  547   
//  548   // ACK Code
//  549   vs_bufferSerialByteSubst( &checksum, VSCP_SERIAL_OPCODE_ERROR );
        MOVS     R1,#+253
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  550   
//  551   // Flags: Data byte count
//  552   vs_bufferSerialByteSubst( &checksum, count );
        MOVS     R1,R5
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  553   
//  554   // Channel
//  555   vs_bufferSerialByteSubst( &checksum, channel );
        MOV      R0,SP
        LDRB     R1,[R0, #+4]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  556   
//  557   // Counter
//  558   vs_bufferSerialByteSubst( &checksum, seqnumber );
        MOV      R0,SP
        LDRB     R1,[R0, #+8]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  559 
//  560 	// Replycode
//  561 	vs_bufferSerialByteSubst( &checksum, ( ( replycode >> 8 ) & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LSRS     R1,R1,#+8
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  562 	vs_bufferSerialByteSubst( &checksum, ( replycode & 0xff ) );
        MOVS     R1,R4
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  563   
//  564   for ( i = 0; i<count; i++ ) {
        MOVS     R0,#+0
        MOVS     R7,R0
??vs_sendReplyFrame_3:
        LSLS     R5,R5,#+24       ;; ZeroExtS R5,R5,#+24,#+24
        LSRS     R5,R5,#+24
        CMP      R7,R5
        BGE      ??vs_sendReplyFrame_4
//  565     vs_bufferSerialByteSubst( &checksum, pData[ i ] );
        LDRB     R1,[R6, R7]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  566   }
        ADDS     R7,R7,#+1
        B        ??vs_sendReplyFrame_3
//  567   
//  568   // Checksum
//  569   vs_bufferSerialByteSubst( &checksum, checksum );
??vs_sendReplyFrame_4:
        MOV      R0,SP
        LDRB     R1,[R0, #+0]
        MOV      R0,SP
        BL       vs_bufferSerialByteSubst
//  570   
//  571   // End of frame
//  572   vs_bufferSerialByte( DLE );
        MOVS     R0,#+16
        BL       vs_bufferSerialByte
//  573   vs_bufferSerialByte( ETX );
        MOVS     R0,#+2
        BL       vs_bufferSerialByte
//  574   
//  575   // Send the buffer content
//  576   if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        LDR      R0,??vs_sendReplyFrame_0  ;; vs_buffer_cnt
        LDRB     R1,[R0, #+0]
        LSLS     R1,R1,#+16       ;; ZeroExtS R1,R1,#+16,#+16
        LSRS     R1,R1,#+16
        LDR      R0,??vs_sendReplyFrame_0+0x4  ;; vs_msgbuffer
        BL       vs_sendBuffer
        CMP      R0,#+0
        BNE      ??vs_sendReplyFrame_5
//  577 	  vs_buffer_cnt = 0;
        LDR      R0,??vs_sendReplyFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  578 	  return FALSE;
        MOVS     R0,#+0
        B        ??vs_sendReplyFrame_2
//  579   }
//  580 
//  581   vs_buffer_cnt = 0;
??vs_sendReplyFrame_5:
        LDR      R0,??vs_sendReplyFrame_0  ;; vs_buffer_cnt
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  582     
//  583   return TRUE;  
        MOVS     R0,#+1
??vs_sendReplyFrame_2:
        POP      {R1-R7}
        POP      {R1}
        BX       R1               ;; return
        DATA
??vs_sendReplyFrame_0:
        DC32     vs_buffer_cnt
        DC32     vs_msgbuffer
        CFI EndBlock cfiBlock12
//  584 }
//  585 
//  586 ///////////////////////////////////////////////////////////////////////////////
//  587 ///////////////////////////////////////////////////////////////////////////////
//  588 ///////////////////////////////////////////////////////////////////////////////
//  589 
//  590 
//  591 
//  592 #ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 
//  593 
//  594 
//  595 ///////////////////////////////////////////////////////////////////////////////
//  596 // vs_initSlidingWndFrames
//  597 //
//  598 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock13 Using cfiCommon0
        CFI Function vs_initSlidingWndFrames
        THUMB
//  599 void vs_initSlidingWndFrames( vs_swStruct *pSlide, 
//  600 								uint32_t timeout,
//  601 								uint8_t resend ) 
//  602 {
vs_initSlidingWndFrames:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
//  603   int i;
//  604   
//  605   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        MOVS     R4,#+0
        MOVS     R3,R4
??vs_initSlidingWndFrames_0:
        CMP      R3,#+10
        BGE      ??vs_initSlidingWndFrames_1
//  606     pSlide->Frames[ i ].timestamp = 0;
        MOVS     R4,#+28
        MULS     R4,R3,R4
        ADDS     R4,R0,R4
        MOVS     R5,#+0
        STR      R5,[R4, #+12]
//  607     pSlide->Frames[ i ].send_count = 0;
        MOVS     R4,#+28
        MULS     R4,R3,R4
        ADDS     R4,R0,R4
        ADDS     R4,R4,#+38
        MOVS     R5,#+0
        STRB     R5,[R4, #+0]
//  608   }
        ADDS     R3,R3,#+1
        B        ??vs_initSlidingWndFrames_0
//  609 
//  610   pSlide->cntOutstanding = 0;
??vs_initSlidingWndFrames_1:
        MOVS     R4,#+0
        STRB     R4,[R0, #+0]
//  611   pSlide->maxresend = resend;
        STRB     R2,[R0, #+8]
//  612   pSlide->maxtime = timeout;
        STR      R1,[R0, #+4]
//  613 }
        POP      {R3-R5}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock13
//  614 
//  615 
//  616 
//  617 ///////////////////////////////////////////////////////////////////////////////
//  618 // vs_findFlyingFrame
//  619 //
//  620 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock14 Using cfiCommon0
        CFI Function vs_findFlyingFrame
        THUMB
//  621 vs_swFrameStruct *
//  622 vs_findFlyingFrame( vs_swStruct *pSlide, 
//  623                         uint8_t channel, 
//  624                         uint8_t seqnumber ) 
//  625 {
vs_findFlyingFrame:
        PUSH     {R4,LR}
        CFI R14 Frame(CFA, -4)
        CFI R4 Frame(CFA, -8)
        CFI CFA R13+8
        MOVS     R3,R0
//  626   int i;
//  627   
//  628   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        MOVS     R0,#+0
        MOVS     R4,R0
??vs_findFlyingFrame_0:
        CMP      R4,#+10
        BGE      ??vs_findFlyingFrame_1
//  629     if ( ( channel == pSlide->Frames[ i ].frame.channel ) &&
//  630         ( seqnumber == pSlide->Frames[ i ].frame.seqnumber ) ) {
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        LDRB     R0,[R0, #+18]
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,R0
        BNE      ??vs_findFlyingFrame_2
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        LDRB     R0,[R0, #+19]
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,R0
        BNE      ??vs_findFlyingFrame_2
//  631       return &( pSlide->Frames[ i ] );      
        MOVS     R0,#+28
        MULS     R4,R0,R4
        ADDS     R0,R3,R4
        ADDS     R0,R0,#+12
        B        ??vs_findFlyingFrame_3
//  632     }
//  633   }
??vs_findFlyingFrame_2:
        ADDS     R4,R4,#+1
        B        ??vs_findFlyingFrame_0
//  634   
//  635   return NULL;
??vs_findFlyingFrame_1:
        MOVS     R0,#+0
??vs_findFlyingFrame_3:
        POP      {R4}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock14
//  636 }
//  637 
//  638 ///////////////////////////////////////////////////////////////////////////////
//  639 // vs_findFreeFlyingFrame
//  640 //
//  641 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock15 Using cfiCommon0
        CFI Function vs_findFreeFlyingFrame
        THUMB
//  642 vs_swFrameStruct *
//  643 vs_findFreeFlyingFrame( vs_swStruct *pSlide ) 
//  644 {
vs_findFreeFlyingFrame:
        PUSH     {R0,LR}
        CFI R14 Frame(CFA, -4)
        CFI CFA R13+8
        MOVS     R1,R0
//  645   int i;
//  646   
//  647   if ( pSlide->cntOutstanding >= VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ) {
        LDRB     R0,[R1, #+0]
        CMP      R0,#+10
        BCC      ??vs_findFreeFlyingFrame_0
//  648 		return NULL;
        MOVS     R0,#+0
        B        ??vs_findFreeFlyingFrame_1
//  649   }
//  650 
//  651   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
??vs_findFreeFlyingFrame_0:
        MOVS     R0,#+0
        MOVS     R2,R0
??vs_findFreeFlyingFrame_2:
        CMP      R2,#+10
        BGE      ??vs_findFreeFlyingFrame_3
//  652     if ( ( 0 == pSlide->Frames[ i ].timestamp ) &&
//  653         ( 0 == pSlide->Frames[ i ].send_count ) ) {
        MOVS     R0,#+28
        MULS     R0,R2,R0
        ADDS     R0,R1,R0
        LDR      R0,[R0, #+12]
        CMP      R0,#+0
        BNE      ??vs_findFreeFlyingFrame_4
        MOVS     R0,#+28
        MULS     R0,R2,R0
        ADDS     R0,R1,R0
        ADDS     R0,R0,#+38
        LDRB     R0,[R0, #+0]
        CMP      R0,#+0
        BNE      ??vs_findFreeFlyingFrame_4
//  654       return &( pSlide->Frames[ i ] );      
        MOVS     R0,#+28
        MULS     R2,R0,R2
        ADDS     R0,R1,R2
        ADDS     R0,R0,#+12
        B        ??vs_findFreeFlyingFrame_1
//  655     }
//  656   }
??vs_findFreeFlyingFrame_4:
        ADDS     R2,R2,#+1
        B        ??vs_findFreeFlyingFrame_2
//  657   
//  658   return NULL;
??vs_findFreeFlyingFrame_3:
        MOVS     R0,#+0
??vs_findFreeFlyingFrame_1:
        POP      {R3}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock15
//  659 }
//  660 
//  661 ///////////////////////////////////////////////////////////////////////////////
//  662 // vs_addSlidingWndFrame
//  663 //
//  664 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock16 Using cfiCommon0
        CFI Function vs_addSlidingWndFrame
        THUMB
//  665 BOOL vs_addSlidingWndFrame( vs_swStruct *pSlide,
//  666                                 vs_frame *pFrame,
//  667                                 uint32_t now ) 
//  668 {
vs_addSlidingWndFrame:
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
//  669   int i;
//  670   
//  671   // Check pointer
//  672   if ( NULL == pSlide ) return FALSE;
        CMP      R4,#+0
        BNE      ??vs_addSlidingWndFrame_0
        MOVS     R0,#+0
        B        ??vs_addSlidingWndFrame_1
//  673   
//  674   // Check if there is room for another frame
//  675   if ( pSlide->cntOutstanding >= VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ) return FALSE;
??vs_addSlidingWndFrame_0:
        LDRB     R0,[R4, #+0]
        CMP      R0,#+10
        BCC      ??vs_addSlidingWndFrame_2
        MOVS     R0,#+0
        B        ??vs_addSlidingWndFrame_1
//  676   
//  677   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
??vs_addSlidingWndFrame_2:
        MOVS     R0,#+0
        MOVS     R7,R0
??vs_addSlidingWndFrame_3:
        CMP      R7,#+10
        BGE      ??vs_addSlidingWndFrame_4
//  678     if ( 0 == pSlide->Frames[ i ].timestamp   ) {
        MOVS     R0,#+28
        MULS     R0,R7,R0
        ADDS     R0,R4,R0
        LDR      R0,[R0, #+12]
        CMP      R0,#+0
        BNE      ??vs_addSlidingWndFrame_5
//  679       memcpy( &pSlide->Frames[ i ].frame,
//  680                   pFrame,
//  681                   sizeof( vs_frame  ) );
        MOVS     R2,#+22
        MOVS     R1,R5
        MOVS     R0,#+28
        MULS     R0,R7,R0
        ADDS     R0,R4,R0
        ADDS     R0,R0,#+16
        BL       memcpy
//  682       pSlide->Frames[ i ].timestamp = now;
        MOVS     R0,#+28
        MULS     R0,R7,R0
        ADDS     R0,R4,R0
        STR      R6,[R0, #+12]
//  683       pSlide->Frames[ i ].send_count = 0;
        MOVS     R0,#+28
        MULS     R0,R7,R0
        ADDS     R0,R4,R0
        ADDS     R0,R0,#+38
        MOVS     R1,#+0
        STRB     R1,[R0, #+0]
//  684       pSlide->cntOutstanding++; // Another frame added
        LDRB     R0,[R4, #+0]
        ADDS     R0,R0,#+1
        STRB     R0,[R4, #+0]
//  685       return TRUE;
        MOVS     R0,#+1
        B        ??vs_addSlidingWndFrame_1
//  686     }
//  687   }  
??vs_addSlidingWndFrame_5:
        ADDS     R7,R7,#+1
        B        ??vs_addSlidingWndFrame_3
//  688   
//  689   // No room for a new frame found - strange!!!
//  690   return FALSE;
??vs_addSlidingWndFrame_4:
        MOVS     R0,#+0
??vs_addSlidingWndFrame_1:
        POP      {R3-R7}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock16
//  691 }
//  692 
//  693 
//  694 ///////////////////////////////////////////////////////////////////////////////
//  695 // vs_removeSlidingWndFrame
//  696 //
//  697 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock17 Using cfiCommon0
        CFI Function vs_removeSlidingWndFrame
        THUMB
//  698 BOOL vs_removeSlidingWndFrame( vs_swStruct *pSlide, 
//  699                                   uint8_t channel, 
//  700                                   uint8_t seqnumber ) 
//  701 {
vs_removeSlidingWndFrame:
        PUSH     {R0,R4,R5,LR}
        CFI R14 Frame(CFA, -4)
        CFI R5 Frame(CFA, -8)
        CFI R4 Frame(CFA, -12)
        CFI CFA R13+16
        MOVS     R3,R0
//  702   int i;
//  703   
//  704   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        MOVS     R0,#+0
        MOVS     R4,R0
??vs_removeSlidingWndFrame_0:
        CMP      R4,#+10
        BGE      ??vs_removeSlidingWndFrame_1
//  705     if ( ( 0 != pSlide->Frames[ i ].timestamp ) &&
//  706 					( channel == pSlide->Frames[ i ].frame.channel ) &&
//  707 					( seqnumber == pSlide->Frames[ i ].frame.seqnumber ) ) {
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        LDR      R0,[R0, #+12]
        CMP      R0,#+0
        BEQ      ??vs_removeSlidingWndFrame_2
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        LDRB     R0,[R0, #+18]
        LSLS     R1,R1,#+24       ;; ZeroExtS R1,R1,#+24,#+24
        LSRS     R1,R1,#+24
        CMP      R1,R0
        BNE      ??vs_removeSlidingWndFrame_2
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        LDRB     R0,[R0, #+19]
        LSLS     R2,R2,#+24       ;; ZeroExtS R2,R2,#+24,#+24
        LSRS     R2,R2,#+24
        CMP      R2,R0
        BNE      ??vs_removeSlidingWndFrame_2
//  708       pSlide->Frames[ i ].timestamp = 0;
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        MOVS     R5,#+0
        STR      R5,[R0, #+12]
//  709       pSlide->Frames[ i ].send_count = 0;
        MOVS     R0,#+28
        MULS     R0,R4,R0
        ADDS     R0,R3,R0
        ADDS     R0,R0,#+38
        MOVS     R5,#+0
        STRB     R5,[R0, #+0]
//  710       pSlide->cntOutstanding--; // Another frame removed
        LDRB     R0,[R3, #+0]
        SUBS     R0,R0,#+1
        STRB     R0,[R3, #+0]
//  711       return TRUE;      
        MOVS     R0,#+1
        B        ??vs_removeSlidingWndFrame_3
//  712     }
//  713   }  
??vs_removeSlidingWndFrame_2:
        ADDS     R4,R4,#+1
        B        ??vs_removeSlidingWndFrame_0
//  714   
//  715   return FALSE;
??vs_removeSlidingWndFrame_1:
        MOVS     R0,#+0
??vs_removeSlidingWndFrame_3:
        POP      {R3-R5}
        POP      {R1}
        BX       R1               ;; return
        CFI EndBlock cfiBlock17
//  716 }
//  717 
//  718 
//  719 ///////////////////////////////////////////////////////////////////////////////
//  720 // vs_timeoutSlidingWndFrame
//  721 //
//  722 

        SECTION `.text`:CODE:NOROOT(2)
        CFI Block cfiBlock18 Using cfiCommon0
        CFI Function vs_timeoutSlidingWndFrame
        THUMB
//  723 void vs_timeoutSlidingWndFrame( vs_swStruct *pSlide, 
//  724                                   uint32_t now ) 
//  725 {
vs_timeoutSlidingWndFrame:
        PUSH     {R0,R4-R7,LR}
        CFI R14 Frame(CFA, -4)
        CFI R7 Frame(CFA, -8)
        CFI R6 Frame(CFA, -12)
        CFI R5 Frame(CFA, -16)
        CFI R4 Frame(CFA, -20)
        CFI CFA R13+24
        MOVS     R4,R0
        MOVS     R5,R1
//  726   int i;
//  727   uint32_t diff;
//  728   
//  729   for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        MOVS     R0,#+0
        MOVS     R6,R0
??vs_timeoutSlidingWndFrame_0:
        CMP      R6,#+10
        BGE      ??vs_timeoutSlidingWndFrame_1
//  730 
//  731 	// Skip unused positions
//  732 	if ( 0 == pSlide->Frames[ i ].timestamp ) continue;
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        LDR      R0,[R0, #+12]
        CMP      R0,#+0
        BEQ      ??vs_timeoutSlidingWndFrame_2
//  733 
//  734     diff = now - pSlide->Frames[ i ].timestamp;
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        LDR      R0,[R0, #+12]
        SUBS     R0,R5,R0
        MOVS     R7,R0
//  735     if ( diff > pSlide->maxtime  ) {
        LDR      R0,[R4, #+4]
        CMP      R0,R7
        BCS      ??vs_timeoutSlidingWndFrame_2
//  736       if ( pSlide->Frames[ i ].send_count > pSlide->maxresend ) {
        LDRB     R0,[R4, #+8]
        MOVS     R1,#+28
        MULS     R1,R6,R1
        ADDS     R1,R4,R1
        ADDS     R1,R1,#+38
        LDRB     R1,[R1, #+0]
        CMP      R0,R1
        BCS      ??vs_timeoutSlidingWndFrame_3
//  737         // Abort sending of this frame
//  738         vs_removeSlidingWndFrame( pSlide, 
//  739                                     pSlide->Frames[ i ].frame.channel,
//  740                                     pSlide->Frames[ i ].frame.seqnumber );
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        LDRB     R2,[R0, #+19]
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        LDRB     R1,[R0, #+18]
        MOVS     R0,R4
        BL       vs_removeSlidingWndFrame
        B        ??vs_timeoutSlidingWndFrame_2
//  741       }
//  742       else {
//  743         // Retransmit the frame
//  744         if ( vs_sendFrame( &pSlide->Frames[ i ].frame ) ) {
??vs_timeoutSlidingWndFrame_3:
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        ADDS     R0,R0,#+16
        BL       vs_sendFrame
        CMP      R0,#+0
        BEQ      ??vs_timeoutSlidingWndFrame_2
//  745           pSlide->Frames[ i ].timestamp = now;
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        STR      R5,[R0, #+12]
//  746           pSlide->Frames[ i ].send_count++;
        MOVS     R0,#+28
        MULS     R0,R6,R0
        ADDS     R0,R4,R0
        ADDS     R0,R0,#+38
        LDRB     R0,[R0, #+0]
        ADDS     R0,R0,#+1
        MOVS     R1,#+28
        MULS     R1,R6,R1
        ADDS     R1,R4,R1
        ADDS     R1,R1,#+38
        STRB     R0,[R1, #+0]
//  747         }
//  748       }
//  749     }
//  750   }  
??vs_timeoutSlidingWndFrame_2:
        ADDS     R6,R6,#+1
        B        ??vs_timeoutSlidingWndFrame_0
//  751 }
??vs_timeoutSlidingWndFrame_1:
        POP      {R3-R7}
        POP      {R0}
        BX       R0               ;; return
        CFI EndBlock cfiBlock18

        END
//  752 
//  753 
//  754 #endif  // Sliding winow
// 
//    53 bytes in section .bss
// 2 218 bytes in section .text
// 
// 2 192 bytes of CODE memory (+ 26 bytes shared)
//    53 bytes of DATA memory
//
//Errors: none
//Warnings: none
