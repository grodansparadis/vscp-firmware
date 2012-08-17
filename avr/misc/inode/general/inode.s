	.file	"inode.c"
	.arch at90s2313
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__tmp_reg__ = 0
__zero_reg__ = 1
_PC_ = 2
	.text
.global	main
	.type	main,@function
main:
/* prologue: frame size=0 */
	ldi r28,lo8(__stack - 0)
	ldi r29,hi8(__stack - 0)
	out __SP_H__,r29
	out __SP_L__,r28
/* prologue end (size=4) */
	ldi r24,lo8(10)
	rcall test1
	ldi r24,lo8(20000)
	ldi r25,hi8(20000)
	rcall test2
	ldi r22,lo8(567890)
	ldi r23,hi8(567890)
	ldi r24,hlo8(567890)
	ldi r25,hhi8(567890)
	rcall test3
/* epilogue: frame size=0 */
__stop_progIi__:
	rjmp __stop_progIi__
/* epilogue end (size=1) */
/* function main size 15 (10) */
.Lfe1:
	.size	main,.Lfe1-main
.global	test1
	.type	test1,@function
test1:
/* prologue: frame size=2 */
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,2
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue end (size=10) */
	std Y+1,r24
	ldd r24,Y+1
	std Y+2,r24
	ldi r24,lo8(1)
	ldi r25,hi8(1)
/* epilogue: frame size=2 */
	adiw r28,2
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	ret
/* epilogue end (size=9) */
/* function test1 size 24 (5) */
.Lfe2:
	.size	test1,.Lfe2-test1
.global	test2
	.type	test2,@function
test2:
/* prologue: frame size=4 */
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,4
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue end (size=10) */
	std Y+1,r24
	std Y+2,r25
	ldd r24,Y+1
	ldd r25,Y+2
	std Y+3,r24
	std Y+4,r25
	ldd r24,Y+3
	ldd r25,Y+4
/* epilogue: frame size=4 */
	adiw r28,4
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	ret
/* epilogue end (size=9) */
/* function test2 size 27 (8) */
.Lfe3:
	.size	test2,.Lfe3-test2
.global	test3
	.type	test3,@function
test3:
/* prologue: frame size=8 */
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue end (size=10) */
	std Y+1,r22
	std Y+2,r23
	std Y+3,r24
	std Y+4,r25
	ldd r24,Y+1
	ldd r25,Y+2
	ldd r26,Y+3
	ldd r27,Y+4
	std Y+5,r24
	std Y+6,r25
	std Y+7,r26
	std Y+8,r27
	ldi r24,lo8(1)
	ldi r25,hi8(1)
/* epilogue: frame size=8 */
	adiw r28,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	ret
/* epilogue end (size=9) */
/* function test3 size 33 (14) */
.Lfe4:
	.size	test3,.Lfe4-test3
.global	_interrupt0_
	.type	_interrupt0_,@function
_interrupt0_:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue end (size=9) */
/* #APP */
	sei
/* #NOAPP */
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=7) */
/* function _interrupt0_ size 18 (2) */
.Lfe5:
	.size	_interrupt0_,.Lfe5-_interrupt0_
.global	_interrupt1_
	.type	_interrupt1_,@function
_interrupt1_:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue end (size=9) */
/* #APP */
	sei
/* #NOAPP */
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=7) */
/* function _interrupt1_ size 18 (2) */
.Lfe6:
	.size	_interrupt1_,.Lfe6-_interrupt1_
/* File inode.c: code  135 = 0x0087 (  41), prologues  52, epilogues  42 */
