@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ Startup code
@
@ Entry point: _start
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.text
	.code 32
	.align

@ ARM has 8 exception vectors

	.global	_start
_start:
	ldr		pc, =ResetHandler		@ reset vector
	ldr		pc, =ResetHandler		@ undefined instruction
	ldr		pc, =ResetHandler		@ software interrupt
	ldr		pc, =ResetHandler		@ prefetch abort
	ldr		pc, =ResetHandler		@ data abort
	ldr		pc, =ResetHandler		@ reserved
	ldr		pc, [pc, #-0xFF0]		@ IRQ. load vector from VICVectAddr
	ldr		pc, =ResetHandler		@ FIQ

	.pool							@ The ldr instructions load the jump
									@ addresses indirectly from a neareast pool.
									@ Because only the first 0x40 bytes of RAM
									@ are remapped to address 0, we have to put
									@ it here.

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ create a variable to hold a C function pointer to be called on IRQ

	.global	irqVector0
irqVector0:
	.word	0						@ this should be set to a C function

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ IRQHandler0
@ This handler wraps a C function to be called upon IRQ.
@ This handler is put into VICVectAddr0 register.

	.global	IRQHandler0
IRQHandler0:
	stmfd	sp!, {r0-r3,r12,lr}		@ store regs that are used here and in C

	mov		r0, #irqVector0
	ldr		r0, [r0]
	ldr		lr, =IRQHandler0_resume
	mov		pc, r0					@ call C function
IRQHandler0_resume:

	ldr		r1, =0xFFFFF030			@ VICVectAddr
	str		r0, [r1]				@ update priority hardware (required!)

	ldmfd	sp!, {r0-r3,r12,lr}
	subs	pc, lr, #4				@ the way IRQ handler should return

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ IRQHandler1
@ An example without any C function wrapping.
@
@	.global	IRQHandler1
@IRQHandler1:
@	stmfd	sp!, {r0-r3,r12,lr}		@ store regs that are used here and in C
@
@	ldr		r1, =0xE0008000			@ T1IR
@	mov		r0, #0xFF				@ all flags
@	str		r0, [r1]				@ clear timer 1 interrupt flags
@
@	ldr		r1, =0xFFFFF030			@ VICVectAddr
@	str		r0, [r1]				@ update priority hardware
@
@	ldmfd	sp!, {r0-r3,r12,lr}
@	subs	pc, lr, #4				@ the way IRQ handler should return

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ __disable_interrupts
@ Disables the interrupts

	.global	__disable_interrupts
__disable_interrupts:
	mrs		r0, CPSR
	orr		r0, r0, #0xC0			@ disable IRQ/FIQ interrupts
	msr		CPSR_fsxc, r0
	mov		pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ __enable_interrupts
@ Enables the interrupts

	.global	__enable_interrupts
__enable_interrupts:
	mrs		r0, CPSR
	bic		r0, r0, #0x80			@ enable IRQ interrupts
	msr		CPSR_fsxc, r0
	mov		pc, lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@ The first things that need to be done

ResetHandler:

@
@ Initialize stack pointers of IRQ and supervisor mode to end of RAM
@

	ldr		r2, =__sp - 0x100
	mrs		r0, cpsr				@ Original PSR value
	bic		r0, r0, #0x1F			@ Clear the mode bits
	orr		r0, r0, #0x12			@ Set IRQ mode bits
	msr		cpsr_c, r0				@ Change the mode
	mov		sp, r2

	sub		r2, r2, #0x100
	bic		r0, r0, #0x1F			@ Clear the mode bits
	orr		r0, r0, #0x13			@ Set Supervisor mode bits
	msr		cpsr_c, r0		    	@ Change the mode
	mov		sp, r2

@
@ Initialize interrupts
@

	ldr		r0, =0xE01FC040			@ MEMMAP
	mov		r1, #2					@ remap interrupt vectors to SRAM
	str		r1, [r0]

	@ldr		r0, =IRQHandler		@ set default IRQ handler
	@ldr		r1, =0xFFFFF034		@ VICDefVectAddr
	@str		r0, [r1]

	ldr		r0, =IRQHandler0		@ set first IRQ handler
	ldr		r1, =0xFFFFF100			@ VICVectAddr0
	str		r0, [r1]

@
@ Clear BSS (non-initialized variables)
@

	mov		r0, #0
	mov		r1, #__data_end
	mov		r2, #__bss_end
ClearBSSLoop:
	cmps	r1, r2
	beq		ClearBSSLoopDone		@ check if we're done yet
	str		r0, [r1], #4			@ fill zero's
	b		ClearBSSLoop	
ClearBSSLoopDone:

@
@ Jump to user code
@

	b		main					@ assume we're not going to return

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	.end

