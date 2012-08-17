@===========================================@
@                                           @
@   ADuC702X Startup Code V0.2              @
@                                           @
@===========================================@
@                                           @
@   The purpose of this code is to setup    @
@   the Interrupt Vectors, the stack        @
@   pointer in the different modes,         @
@   to zero the RAM and copy variables      @
@   from ROM to RAM and to brach to         @
@   C code at reset.                        @
@                                           @
@===========================================@

@   Define required MMR Base value and offsets

.equ    MMR_BASE_0,         0xFFFF0000
.equ    POWKEY1_OFFSET,     0x0404
.equ    POWCON_OFFSET,      0x0408
.equ    POWKEY2_OFFSET,     0x040C

/* 
//*** <<< Use Configuration Wizard in Context Menu >>> *** 
//  
*/

# *** Startup Code (executed after Reset) ***

# Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs 
 
        .equ    Mode_USR,   0x10 
        .equ    Mode_FIQ,   0x11 
        .equ    Mode_IRQ,   0x12 
        .equ    Mode_SVC,   0x13 
        .equ    Mode_ABT,   0x17 
        .equ    Mode_UNDEF, 0x1B 
        .equ    Mode_SYS,   0x1F 
 
        .equ    I_Bit, 0x80        /* when I bit is set, IRQ is disabled */ 
        .equ    F_Bit, 0x40        /* when F bit is set, FIQ is disabled */ 
 
 
# System Memory definitions 
 
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
        .equ    Top_Stack,      0x12000
        .equ    UNDEF_Stack_Size,   4 
        .equ    SVC_Stack_Size,     4 
        .equ    ABT_Stack_Size,     4 
        .equ    FIQ_Stack_Size,    128 
        .equ    IRQ_Stack_Size,    128 
        .equ    USR_Stack_Size,    1024 
 

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
// <q> Enable Stdlib I/O 
// <i> enable this option when your code contains printf calls 
*/
		.equ    En_StdIO, 0

		
       

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

Vectors:        LDR     PC, Reset_Address
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                            /* Reserved Vector */
                LDR     PC, IRQ_Addr
                LDR     PC, FIQ_Addr

Reset_Address:  .word   Reset_Handler
Undef_Addr:     .word   ADI_UNDEF_Interrupt_Setup 
SWI_Addr:       .word   ADI_SWI_Interrupt_Setup 
PAbt_Addr:      .word   ADI_PABORT_Interrupt_Setup 
DAbt_Addr:      .word   ADI_DABORT_Interrupt_Setup 
IRQ_Addr:       .word   ADI_IRQ_Interrupt_Setup 
FIQ_Addr:       .word   ADI_FIQ_Interrupt_Setup 

ADI_UNDEF_Interrupt_Setup:
ADI_SWI_Interrupt_Setup:
ADI_PABORT_Interrupt_Setup:
ADI_DABORT_Interrupt_Setup:
ADI_IRQ_Interrupt_Setup:
ADI_FIQ_Interrupt_Setup:

# Reset Handler

Reset_Handler:  

.if PLL_SETUP
                LDR     R0, =MMR_BASE_0
                MOV     R1, #0x01         
                STR     R1, [R0,#POWKEY1_OFFSET]          
                MOV     R1, #PLLCFG_Val      
                STR     R1, [R0,#POWCON_OFFSET]    
                MOV     R1, #0xF4
                STR     R1, [R0,#POWKEY2_OFFSET]
.endif

# Setup Stack for each mode 
                LDR     R0, =Top_Stack 
 
#  Enter Undefined Instruction Mode and set its Stack Pointer 
                MSR     CPSR_c, #Mode_UNDEF|I_Bit|F_Bit 
                MOV     SP, R0 
                SUB     R0, R0, #UNDEF_Stack_Size 
 
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
 
#  Setup a default Stack Limit (when compiled with -mapcs-stack-check) 
                mov     R0,#USR_Stack_Size
                SUB     SL, SP,R0         /* USER STACK SIZE SL = R10 */ 
 
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
 

.if En_StdIO
            
# Enter the standard system startup code required for stdlib I/O  
              B       _start	
			      
.endif
 
# Enter the C code 
           	 


Jump_To_Main:   
                ADR     LR, __Return_from_Main           
                LDR     R0, =main
                BX      R0

__Return_from_Main:
                B       __Return_from_Main	 


        .size   _startup, . - _startup 
        .endfunc 

        .end
