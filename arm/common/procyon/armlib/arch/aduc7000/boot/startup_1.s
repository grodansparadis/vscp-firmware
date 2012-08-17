#/***********************************************************************/
#/*  This file is part of the uVision/ARM development tools             */
#/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                           */
#/***********************************************************************/
#/*                                                                     */
#/*  STARTUP.S:  Startup file ADI ADuC702x device series                */
#/*                                                                     */
#/***********************************************************************/


/* 
//*** <<< Use Configuration Wizard in Context Menu >>> *** 
*/


# *** Startup Code (executed after Reset) ***


# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs

        .equ    Mode_USR,       0x10
        .equ    Mode_FIQ,       0x11
        .equ    Mode_IRQ,       0x12
        .equ    Mode_SVC,       0x13
        .equ    Mode_ABT,       0x17
        .equ    Mode_UND,       0x1B
        .equ    Mode_SYS,       0x1F

        .equ    I_Bit,          0x80    /* when I bit is set, IRQ is disabled */
        .equ    F_Bit,          0x40    /* when F bit is set, FIQ is disabled */


/*
// <h> Stack Configuration
//   <o>  Top of Stack Address  <0x0-0xFFFFFFFF>
//   <h>  Stack Sizes (in Bytes)
//     <o1> Undefined Mode      <0x0-0xFFFFFFFF>
//     <o2> Supervisor Mode     <0x0-0xFFFFFFFF>
//     <o3> Abort Mode          <0x0-0xFFFFFFFF>
//     <o4> Fast Interrupt Mode <0x0-0xFFFFFFFF>
//     <o5> Interrupt Mode      <0x0-0xFFFFFFFF>
//     <o6> User/System Mode    <0x0-0xFFFFFFFF>
//   </h>
// </h>
*/
        .equ    Top_Stack,      0x00012000
        .equ    UND_Stack_Size, 0x00000004
        .equ    SVC_Stack_Size, 0x00000004
        .equ    ABT_Stack_Size, 0x00000004
        .equ    FIQ_Stack_Size, 0x00000080
        .equ    IRQ_Stack_Size, 0x00000080
        .equ    USR_Stack_Size, 0x00000400


# MMR definitions
        .equ    MMR_BASE,       0xFFFF0000     /* MMR Base Address */
        .equ    POWKEY1_OFFSET,     0x0404
        .equ    POWCON_OFFSET,      0x0408
        .equ    POWKEY2_OFFSET,     0x040C


/*
// <e> PLL Setup
//   <o1.0..2>  CD: PLL Multiplier Selection
//               <0-7>
//               <i> CD Value
//   <o1.3>     FINT: Fast Interrupt
//               <0-1>
//               <i> Switches to CD0 for FIQ
// </e>
*/
        .equ    PLL_SETUP,      1
        .equ    PLLCFG_Val,     0x00000000


/*
// <e> Pin Setup
*/
        .equ    GPIO_SETUP,     0
        .equ    GPIOBASE,       0xFFFFF400

/*
//   <h>  Port 0
//     <o.0..1>    P0.0  <0=> GPIO      <1=> CMPOUT    <2=> MS2     <3=> PLAI[7]
//     <o.4..5>    P0.1  <0=> GPIO      <1=> ---       <2=> XBEN0   <3=> ---
//     <o.8..9>    P0.2  <0=> GPIO      <1=> ---       <2=> XBEN1   <3=> ---
//     <o.12..13>  P0.3  <0=> GPIO      <1=> TRST      <2=> XA16    <3=> ADCBUSY
//     <o.16..17>  P0.4  <0=> GPIO/IRQ0 <1=> CONVSTART <2=> MS1     <3=> PLAO[1]
//     <o.20..21>  P0.5  <0=> GPIO/IRQ1 <1=> ADCBUSY   <2=> MS0     <3=> PLAO[2]
//     <o.24..25>  P0.6  <0=> GPIO      <1=> MRST      <2=> XAE     <3=> PLAO[3]
//     <o.28..29>  P0.7  <0=> GPIO      <1=> ECLK      <2=> SIN     <3=> PLAO[4]
//   </h>
*/
        .equ    GP0CON_Val,     0x00000000

/*
//   <h>  Port 1
//     <o.0..1>    P1.0  <0=> GPIO      <1=> SIN       <2=> I2C0SCL <3=> PLAI[0]
//     <o.4..5>    P1.1  <0=> GPIO      <1=> SOUT      <2=> I2C0SDA <3=> PLAI[1]
//     <o.8..9>    P1.2  <0=> GPIO      <1=> RTS       <2=> I2C1SCL <3=> PLAI[2]
//     <o.12..13>  P1.3  <0=> GPIO      <1=> CTS       <2=> I2C1SDA <3=> PLAI[3]
//     <o.16..17>  P1.4  <0=> GPIO/IRQ2 <1=> RI        <2=> SPICLK  <3=> PLAI[4]
//     <o.20..21>  P1.5  <0=> GPIO/IRQ3 <1=> DCD       <2=> SPIMISO <3=> PLAI[5]
//     <o.24..25>  P1.6  <0=> GPIO      <1=> DSR       <2=> SPIMOSI <3=> PLAI[6]
//     <o.28..29>  P1.7  <0=> GPIO      <1=> DTR       <2=> SPICSL  <3=> PLAO[0]
//   </h>
*/
        .equ    GP1CON_Val,     0x00000000

/*
//   <h>  Port 2
//     <o.0..1>    P2.0  <0=> GPIO      <1=> CONVSTART <2=> SOUT    <3=> PLAO[5]
//     <o.4..5>    P2.1  <0=> GPIO      <1=> ---       <2=> XWS     <3=> PLAO[6]
//     <o.8..9>    P2.2  <0=> GPIO      <1=> ---       <2=> XRS     <3=> PLAO[7]
//     <o.12..13>  P2.3  <0=> GPIO      <1=> ---       <2=> XAE     <3=> ---
//     <o.16..17>  P2.4  <0=> GPIO      <1=> ---       <2=> MS0     <3=> ---
//     <o.20..21>  P2.5  <0=> GPIO      <1=> ---       <2=> MS1     <3=> ---
//     <o.24..25>  P2.6  <0=> GPIO      <1=> ---       <2=> MS2     <3=> ---
//     <o.28..29>  P2.7  <0=> GPIO      <1=> ---       <2=> MS3     <3=> ---
//   </h>
*/
        .equ    GP2CON_Val,     0x00000000

/*
//   <h>  Port 3
//     <o.0..1>    P3.0  <0=> GPIO      <1=> PWM0H     <2=> XAD0    <3=> PLAI[8]
//     <o.4..5>    P3.1  <0=> GPIO      <1=> PWM0L     <2=> XAD1    <3=> PLAI[9]
//     <o.8..9>    P3.2  <0=> GPIO      <1=> PWM1H     <2=> XAD2    <3=> PLAI[10]
//     <o.12..13>  P3.3  <0=> GPIO      <1=> PWM1L     <2=> XAD3    <3=> PLAI[11]
//     <o.16..17>  P3.4  <0=> GPIO      <1=> PWM2H     <2=> XAD4    <3=> PLAI[12]
//     <o.20..21>  P3.5  <0=> GPIO      <1=> PWM2L     <2=> XAD5    <3=> PLAI[13]
//     <o.24..25>  P3.6  <0=> GPIO      <1=> PWMTRIP   <2=> XAD6    <3=> PLAI[14]
//     <o.28..29>  P3.7  <0=> GPIO      <1=> PWMSYNC   <2=> XAD7    <3=> PLAI[15]
//   </h>
*/
        .equ    GP3CON_Val,     0x00000000

/*
//   <h>  Port 4
//     <o.0..1>    P4.0  <0=> GPIO      <1=> ---       <2=> XAD8    <3=> PLAO[8]
//     <o.4..5>    P4.1  <0=> GPIO      <1=> ---       <2=> XAD9    <3=> PLAO[9]
//     <o.8..9>    P4.2  <0=> GPIO      <1=> ---       <2=> XAD10   <3=> PLAO[10]
//     <o.12..13>  P4.3  <0=> GPIO      <1=> ---       <2=> XAD11   <3=> PLAO[11]
//     <o.16..17>  P4.4  <0=> GPIO      <1=> ---       <2=> XAD12   <3=> PLAO[12]
//     <o.20..21>  P4.5  <0=> GPIO      <1=> ---       <2=> XAD13   <3=> PLAO[13]
//     <o.24..25>  P4.6  <0=> GPIO      <1=> ---       <2=> XAD14   <3=> PLAO[14]
//     <o.28..29>  P4.7  <0=> GPIO      <1=> ---       <2=> XAD15   <3=> PLAO[15]
//   </h>
*/
        .equ    GP4CON_Val,     0x00000000

/*
// </e>
*/


/*
// <e> External Memory Interface
*/
        .equ    XM_SETUP,       0
        .equ    XMBASE,         0xFFFFF000

/*
//   <e.0>  Enable Memory Region 0
//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
//     <o1.11>     Enable Dynamic Addressing
//     <o1.15>     Byte Enabled Write Strobe
//     <o1.10>     Disable extra Address Latch Hold Cycle
//     <o1.8>      Disable extra Write Address Hold Cycle
//     <o1.9>      Disable Read Bus Turn Cycle
//     <o1.12..14> Address Wait States <0-7>
//                 <i> Number of Wait States added for AE
//     <o1.0..3>   Read Wait States <0-15>
//                 <i> Number of Wait States added for RS
//     <o1.4..7>   Write Wait States <0-15>
//                 <i> Number of Wait States added for WS
//   </e>
*/
        .equ    XM0CON_Val,     0x00000000
        .equ    XM0PAR_Val,     0x000070FF

/*
//   <e.0>  Enable Memory Region 1
//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
//     <o1.11>     Enable Dynamic Addressing
//     <o1.15>     Byte Enabled Write Strobe
//     <o1.10>     Disable extra Address Latch Hold Cycle
//     <o1.8>      Disable extra Write Address Hold Cycle
//     <o1.9>      Disable Read Bus Turn Cycle
//     <o1.12..14> Address Wait States <0-7>
//                 <i> Number of Wait States added for AE
//     <o1.0..3>   Read Wait States <0-15>
//                 <i> Number of Wait States added for RS
//     <o1.4..7>   Write Wait States <0-15>
//                 <i> Number of Wait States added for WS
//   </e>
*/
        .equ    XM1CON_Val,     0x00000000
        .equ    XM1PAR_Val,     0x000070FF

/*
//   <e.0>  Enable Memory Region 2
//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
//     <o1.11>     Enable Dynamic Addressing
//     <o1.15>     Byte Enabled Write Strobe
//     <o1.10>     Disable extra Address Latch Hold Cycle
//     <o1.8>      Disable extra Write Address Hold Cycle
//     <o1.9>      Disable Read Bus Turn Cycle
//     <o1.12..14> Address Wait States <0-7>
//                 <i> Number of Wait States added for AE
//     <o1.0..3>   Read Wait States <0-15>
//                 <i> Number of Wait States added for RS
//     <o1.4..7>   Write Wait States <0-15>
//                 <i> Number of Wait States added for WS
//   </e>
*/
        .equ    XM2CON_Val,     0x00000000
        .equ    XM2PAR_Val,     0x000070FF

/*
//   <e.0>  Enable Memory Region 3
//     <o.1>       Data Bus Width  <0=> 8-bit  <1=> 16-bit
//     <o1.11>     Enable Dynamic Addressing
//     <o1.15>     Byte Enabled Write Strobe
//     <o1.10>     Disable extra Address Latch Hold Cycle
//     <o1.8>      Disable extra Write Address Hold Cycle
//     <o1.9>      Disable Read Bus Turn Cycle
//     <o1.12..14> Address Wait States <0-7>
//                 <i> Number of Wait States added for AE
//     <o1.0..3>   Read Wait States <0-15>
//                 <i> Number of Wait States added for RS
//     <o1.4..7>   Write Wait States <0-15>
//                 <i> Number of Wait States added for WS
//   </e>
*/
        .equ    XM3CON_Val,     0x00000000
        .equ    XM3PAR_Val,     0x000070FF

/*
//   <e.0>         Memory Muxed Mode
*/
        .equ    XMCFG_Val,      0x00000001
/*
//   </e>
*/

/*
// </e>
*/


/*
// <q> Enable Stdlib I/O 
// <i> enable this option when your code contains printf calls 
*/
        .equ    En_StdIO,       0


# Starupt Code must be linked first at Address at which it expects to run.

        .text
        .arm

        .global _startup
        .func   _startup
_startup:


# Exception Vectors
#  Mapped to Address 0.
#  Absolute addressing mode must be used.
#  Dummy Handlers are implemented as infinite loops which can be modified.

Vectors:        LDR     PC, Reset_Addr         
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                            /* Reserved Vector */
                LDR     PC, IRQ_Addr
                LDR     PC, FIQ_Addr

Reset_Addr:     .word   Reset_Handler
Undef_Addr:     .word   ADI_UNDEF_Interrupt_Setup 
SWI_Addr:       .word   ADI_SWI_Interrupt_Setup 
PAbt_Addr:      .word   ADI_PABORT_Interrupt_Setup 
DAbt_Addr:      .word   ADI_DABORT_Interrupt_Setup 
IRQ_Addr:       .word   ADI_IRQ_Interrupt_Setup 
FIQ_Addr:       .word   ADI_FIQ_Interrupt_Setup 

# Reset Handler

Reset_Handler:  


.if PLL_SETUP
                LDR     R0, =MMR_BASE
                MOV     R1, #0x01         
                STR     R1, [R0,#POWKEY1_OFFSET]          
                MOV     R1, #PLLCFG_Val      
                STR     R1, [R0,#POWCON_OFFSET]    
                MOV     R1, #0xF4
                STR     R1, [R0,#POWKEY2_OFFSET]
.endif


# Setup Pins
.if GPIO_SETUP

                ADR     R10, GPIO_CFG          /* Pointer to GPIO CFG */
                LDMIA   R10, {R0-R5}           /* Load GPIO Configuration */
                STMIA   R0, {R1-R5}            /* Store GPxCON */
                B       GPIO_END

GPIO_CFG:       .word   GPIOBASE
                .word   GP0CON_Val
                .word   GP1CON_Val
                .word   GP2CON_Val
                .word   GP3CON_Val
                .word   GP4CON_Val
GPIO_END:

.endif  /* GPIO_SETUP */


# Setup External Memory Interface
.if XM_SETUP

                ADR     R10, XM_CFG            /* Pointer to XM CFG */
                LDMIA   R10, {R0-R9}           /* Load XM Configuration */
                STR     R1, [R0],#0x10         /* Store XMCFG */
                STMIA   R0, {R2-R9}            /* Store XMxCON & XMxPAR */
                B       XM_END

XM_CFG:         .word   XMBASE
                .word   XMCFG_Val
                .word   XM0CON_Val
                .word   XM1CON_Val
                .word   XM2CON_Val
                .word   XM3CON_Val
                .word   XM0PAR_Val
                .word   XM1PAR_Val
                .word   XM2PAR_Val
                .word   XM3PAR_Val
XM_END:

.endif  /* XM_SETUP */


# Setup Stack for each mode

                LDR     R0, =Top_Stack

#  Enter Undefined Instruction Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

#  Enter Abort Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

#  Enter FIQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

#  Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

#  Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

#  Enter User Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_USR
                MOV     SP, R0

#  Setup a default Stack Limit (when compiled with "-mapcs-stack-check")
                SUB     SL, SP, #USR_Stack_Size


# Relocate .data section (Copy from ROM to RAM)
                LDR     R1, =_etext
                LDR     R2, =_data
                LDR     R3, =_edata
LoopRel:        CMP     R2, R3
                LDRLO   R0, [R1], #4
                STRLO   R0, [R2], #4
                BLO     LoopRel


# Clear .bss section (Zero init)
                MOV     R0, #0
                LDR     R1, =__bss_start__
                LDR     R2, =__bss_end__
LoopZI:         CMP     R1, R2
                STRLO   R0, [R1], #4
                BLO     LoopZI


# Enter the C code

.if En_StdIO
            
# Enter the standard system startup code required for stdlib I/O  
                B       _start

.else

                LDR     LR, =__Return_from_main
                LDR     R0, =main
                BX      R0

__Return_from_main:
                B       __Return_from_main

.endif


        .size   _startup, . - _startup
        .endfunc


        .end
