
	; HI-TECH C PRO for the PIC32 MCU Family V9.60
	; Copyright (C) 1984-2008 HI-TECH Software
	;Licensed for evaluation purposes only.
	;This licence will expire on Wed, 29 Oct 2008

	; Auto-generated runtime startup code for prelink stage.

	;
	; Compiler options:
	;
	; -ohitec-test.elf -mhitec-test.map --runtime=default --summary=default \
	; --interrupts= --output=default main.p1 --chip=32MX360F512L -P \
	; --opt=default -g --asmlist --errformat=Error   [%n] %f; %l.%c %s \
	; --msgformat=Advisory[%n] %s --warnformat=Warning [%n] %f; %l.%c %s
	;


	processor	32MX360F512L
	psect	init,class=KSEG0_BOOT,isa=mips32r2,reloc=4
	psect	powerup,class=KSEG1_BOOT,isa=mips32r2,reloc=4
	psect	text32,class=KSEG0_PFM,isa=mips32r2,reloc=4
	psect	text16,class=KSEG0_PFM,isa=mips16e,reloc=4
	psect	bss,class=KSEG0_DRM,reloc=4,isa=none
	psect	data,class=KSEG0_DRM,reloc=4,isa=none
	psect	idata,class=KSEG0_PFM,reloc=4,isa=none
	psect	strings,class=KSEG0_PFM,reloc=4,isa=none
	psect	const,class=KSEG0_PFM,reloc=4,isa=none
	psect	constfar,class=KSEG0_PFM,reloc=4,isa=none
	psect	config,class=KSEG1_CONFIG,reloc=4,isa=none
	psect	nvram,class=KSEG0_DRM,reloc=4,isa=none
	psect	nvbit,class=KSEG0_DRM,bit,reloc=4,isa=none
	psect	rbit,class=KSEG0_DRM,bit,reloc=4,isa=none
	psect	bootex,class=KSEG1_BOOT,isa=mips32r2,reloc=4
	psect	reset,class=KSEG1_BOOT,isa=mips32r2,reloc=4

	psect	vectors,class=KSEG0_BOOT,reloc=0x1000,isa=mips32r2
	global	startup,_main,_main_isa,___nmi_handler
	global	__Lconst,__Hconst,__Lvectors

	psect reset
; reset vector
	li at, startup
	jr at
	nop

	psect	init
startup:
opt asmopt_off
	di
	mfc0	t0, $12	;status register
	ext	at, t0, 19, 1	;status<NMI>
; jump to NMI handler if at != $0
	beq at, zero, ___skip_nmi
	nop
	li at, ___nmi_handler
	jalr at
	nop
	___skip_nmi:
; init heap and stack
	psect	heapstack,class=KSEG0_DRM,reloc=4,isa=none
	global __Lheapstack, __Hheapstack
	global __Lstack, __Hstack
	global __Lheap, __Hheap
	__Lstack equ __Lheapstack
	__Hstack equ __Hheapstack
	__Lheap equ __Lheapstack
	__Hheap equ __Hheapstack
; no room for heap and stack !
	psect init
	global	__Hstack
	li	sp,	__Hstack
; no bss to clear
; no rbit to clear
; no data to initialise
; init gp with base of sfr 
	lui gp, 0xBF88
	psect init
;
; initialise CP0 registers
;
; clear Count register
	mtc0	zero, $9
	;set Compare register
	addiu	at, zero, -1
	mtc0	at, $11
; clear all Status bits but SR, NMI
	li	at, 0x180000
	mfc0	t0, $12
	and	at, at, t0
	mtc0	at, $12
; initialise Cause register DC=0, IV=1, IP1=0, IP0=0
	li	at, (1<<23)
	mtc0	at, $13
; init global pointer to middle of KSEG0_DRM
	global __gptr
	__gptr equ 0x80004000
	li	gp, __gptr
	wrpgpr gp, gp
; init frame pointer to middle of const psect
	global __cptr
	__cptr equ __Lconst+0x0
	li	fp, __cptr
	wrpgpr fp, fp
;
; Invoke main
;
	li at, _main+_main_isa
	jalr	at
	nop
__done:
	b	__done
	nop
opt asmopt_on
	end	startup
