; COPYRIGHT 2002 BY DAVID EK. PERMISSION IS GRANTED TO FREELY USE
; THIS SOFTWARE FOR NONCOMMERCIAL PURPOSES.
;
; NOTE: USE AT YOUR OWN RISK. THE AUTHOR ASSUMES NO LIABILITY FOR DAMAGE
; INCURRED FROM THE USE OF THIS SOFTWARE.
;
; THIS IS THE CODE FOR INSTALLMENT 3 OF THE PIC WX ARTICLES IN
; THE DIGITAL QRP HOMEBREWING COLUMN OF QRP QUARTERLY.
;
; PIC16F84 code for the NK0E weather station project. The purpose
; of this project is to provide a hardware interface which measures
; temperature, wind speed, wind direction, humidity, and pressure
; and communicates it to a PC via serial communications when
; requested. Note that the data collected and transmitted must be
; processed by the PC in order to obtain the actual measurement
; values (i.e. no calibration is done in the PIC code).
;
; The hardware circuit makes use of two major components: the 
; PIC16F84 itself and a MAX232 TTL-RS232 level converter.
; The MAX232 converts the TTL signals from the PIC to RS232 levels
; for the PC (and vice versa).
;
; The PIC16F84 pin assignments are as follows:
;
; RA1: serial out to PC
; RA2: clock out to SHT11 temp/humid sensor
; RA3: data in/out for SHT11 temp/humid sensor
; RB0: serial in from PC (generates interrupt)
;
; This software implements the following command set:
;
; Received  Reply        Description
;-----------------------------------
; 't'       12345 CR LF  Get the temperature raw data. Five digits
;           or e1 CR LF  are always returned, with leading zeroes
;           or e2 CR LF  if necessary, followed by a carriage return
;                        and line feed. See the SHT11 data sheet
;                        for instructions on computing the temperature.
;                        A return of e1 indicates that the SHT11 didn't
;                        acknowledge the request. A return of e2
;                        indicates that no data was returned by the SHT11
;                        within the wait period (255 ms).
; 'h'       12345 CR LF  Get the humidity raw data. Five digits
;           or e1 CR LF  are always returned, with leading zeroes
;           or e2 CR LF  if necessary, followed by a carriage return
;                        and line feed. See the SHT11 data sheet for
;                        instructions on computing the humidity.
;                        A return of e1 indicates that the SHT11 didn't
;                        acknowledge the request. A return of e2
;                        indicates that no data was returned by the SHT11
;                        within the wait period (255 ms).
; 'v'       version info Gets the version number of the firmware.
;                        returns an ASCII string followed by a 
;                        carriage return and line feed.
;
; Written using Microchip MPLAB v5.40
;
; Please direct any questions, comments, or suggestions to the
; author: David Ek
;         nk0e@earthlink.net
;
; History:
;
; Rev 1 (23 Jul 2002):
;    Creation. Returns dummy temperature data. This version used
;    for installment 2 of the PIC WX articles in QQ.
; Rev 2 (26 Sep 2002):
;    Added code for the Sensirion SHT11 temperature/humidity sensor
;    for installment 3 of the PIC WX articles in QQ.
;
;----------------------------------------------------------------

	list	p=16f84
	radix	dec
	__config _CP_OFF & _WDT_OFF & _XT_OSC

	include	"p16f84.inc"

; defines:

; These are used by the serial comm routines for timing. Note that
; slower speeds may not work well because the delays might be larger
; than 255, requiring the use of the prescalar.

_Clkspd		equ	1000000			;external clock / 4
_BaudRate	equ	9600			;desired baud rate
_Period		equ	(_Clkspd/_BaudRate)	;clock cycles / bit

_StartRxDelay	equ	(_Period/2 - 15)/3	;this is how long to
						;wait to get to the
						;middle of the start
						;bit. This is loops,
						;not clock cycles.

_BitRxDelay	equ	277 - _Period		;this is what to load
						;into TMR0 for correct
						;interval between bits
						;on RX

_BitTxDelay	equ	285 - _Period		;this is what to load
						;into TMR0 for correct
						;interval between bits
						;on TX

_StopTxDelay	equ	272 - _Period		;this is what to load
						;into TMR0 for correct
						;interval betweeen last
						;bit and stop bit on TX

; defines for I/0 pins:

_SER_IN		equ	0			;Serial input line (PORTB)
_SER_OUT	equ	1			;Serial output line (PORTA)
_SHT11_SCK	equ	2			;clock line for SHT11 sensor
						;(PORTA)
_SHT11_DAT	equ	3			;data line for SHT11 sensor
						;(PORTA)

; macros for the bin-to-ascii conversion routines:

loadw	macro	arg1
	movlw	high(arg1)
	movwf	hi
	movlw	low(arg1)
	movwf	lo
	endm
dodigit	macro	arg1
	movlw	high(arg1)
	movwf	shi
	movlw	low(arg1)
	movwf	slo
	call	dosub
	endm

; memory locations:

	cblock 0x0C

	BitCount	;number of bits left to send or
			;receive, not including start & stop
			;bits

	RXChar		;received character while being received
	RXBuff		;most recently received character

	TXChar		;character to transmit

	SerialReg	;status register:
			;bit 0: on if character has been
			;       received
			;bit 1: on if busy with RX/TX
			;bit 2: on if sending, off if receiving
			;bit 3: on if next bit to send is stop bit

	WSave		;copy of W register

	SSave		;copy of the Status register

	SHT11Byte	;a byte of data sent to or returned by the SHT11

	counter		;generic counter register

	hi		;hi byte for number to be converted to ascii
	lo		;lo byte for number to be converted to ascii
	shi		;hi byte for subtractor for conversion to ascii
	slo		;lo byte for subtractor for conversion to ascii
	digit		;ascii digit converted from binary. Also used as
			;input to SendErrorCode

	MSDelay		;register for WaitMS timing

	endc

	org	0x00
	goto	Main

	org	0x04

;--------------------------------------------------------------------
;-----Serial Communication Routines----------------------------------
;--------------------------------------------------------------------
;
; The serial comm routines generate 1 start bit, 8 data bits, 1 stop bit, 
; no parity. The baud rate is determined by the delay programmed
; into the onboard timer. The sending and receiving is interrupt driven,
; meaning other tasks can be carried on while the characters are being
; sent and received.
;
;-----Main Interrupt Routine-----------------------------------------
;
; for timing: 4 cycles from interrupt time to get here.

; Save the W and STATUS registers:

Int
	movwf	WSave
	swapf	STATUS,W	;use swapf to prevent any
	movwf	SSave		;status flags from being changed

; cycles so far since interrupt occurred: 7

; Okay, I'm doing something goofy here. Instead of actually checking
; the overflow bits themselves, I'm using the enable bits to determine
; which interrupt occurred. I can do this because there should only
; ever be one type of interrupt active at a time. The problem with
; checking the overflow bits is that the T0IF bit can get set even
; if that interrupt is disabled.

; Check first for a timer overflow interrupt. The overflow bit gets
; set even if the interrupt is disabled.

	btfsc	INTCON,T0IE
	goto	DoBit		;we're in the middle of sending or
				;receiving

; 10 cycles executed on entry to DoBit

; If not a timer overflow interrupt, check for external interrupt:

	btfsc	INTCON,INTE	;RB0 is our receive line and it
	goto	StartRX		;generates an interrupt on a high-
				;to-low transition

; 12 cycles executed on entry to StartRX

; Else, must be something we don't care about:

	;do nothing for now

; Restore the W and STATUS registers:

Restore	swapf	SSave,W
	movwf	STATUS
	swapf	WSave,F
	swapf	WSave,W

	retfie

;------end Main Interrupt Routine------------------------------------


;------Subroutine SerSetup-------------------------------------------

SerSetup

; set up the option register for internal counting, WDT disabled,
; no prescaler.

	clrf	TMR0
	bsf	STATUS,RP0
	clrwdt			;set bits in OPTION_REG to
	movlw	b'10001000'	;enable internal clock counting,
	movwf	OPTION_REG	;disable watchdog timer.
	bcf	STATUS,RP0	;switch to bank 0

; set the output line to idle (high)

	bsf	PORTA,_SER_OUT	;set the output line to idle
				;(high) state

	movlw	b'00000001'
	movwf	PORTB

; enable the external interrupt via RB0

	movlw	b'10010000'	;set bits in INTCON to enable
	movwf	INTCON		;external interrupt

; initialize the SerialReg:

	clrf	SerialReg

	return

;------end SetSetup--------------------------------------------------

;------Subroutine StartRX--------------------------------------------
;
; This subroutine is called by the main interrupt routine when an
; external interrupt on RB0 occurs. This means we're receiving the
; start bit for a character. We want to enable the external TMR0
; interrupt and prepare to receive the character.

StartRX

; wait halfway through the bit to see if it's real:

	bcf	INTCON,INTF	;clear the interrupt
	movlw	_StartRxDelay
	movwf	BitCount	;this is the 15th instruction since
				;the interrupt. Note--we're using
				;BitCount for this loop purely for
				;convenience. Usually it's used to
				;actually count the bits we TX/RX.

RXWait	decfsz	BitCount,F	;this loop takes 3 times the initial
	goto	RXWait		;value of BitCount clock cycles
	
; now we should be at the middle of the start bit. Is the input still
; low? If not, goto Restore and ignore this interrupt.

	btfsc	PORTB,_SER_IN
	goto	Restore

; if we get to here it must really be the start bit. Load TMR0,
; disable the external interrupt, and enable the TMR0 interrupt. 

; load up the appropriate delay to get us to the middle of the
; first bit:

	movlw	_BitRxDelay
	movwf	TMR0		;4 cycles from read of PORTB
	movlw	b'00100000'
	movwf	INTCON

; set the SerialReg to indicate that the routines are busy getting
; a character:

	movlw	b'00000010'
	movwf	SerialReg

; initialize BitCount:

	movlw	8
	movwf	BitCount

; okay, now we return.

	goto	Restore

;------end StartRX---------------------------------------------------


;------DoBit---------------------------------------------------------
;
; sends or receives the next bit. Bits are sent/received from least
; to most significant bit.

DoBit

; clear the TMR0 overflow interrupt flag:

	bcf	INTCON,T0IF

; Are we receiving?

	btfsc	SerialReg,2
	goto	Sending

; check to see if we're receiving the stop bit:

	movf	BitCount,F
	btfsc	STATUS,Z
	goto	GetStopBit

; if we get to here, we're in the middle of receiving. Get the next
; bit: (16 cycles to get to the next instruction from the start of
; the interrupt).

	rrf	PORTB,W		;rrf PORTB into W. This sets
				;the carry bit if RB0 was high.
	rrf	RXChar,F	;doing a rrf on RXChar brings
				;in the carry bit to the MSB.

; Decrement the bit counter.

	decf	BitCount,F

; reload TMR0 for the next interrupt, and
; go to the end of the interrupt routine.

	movlw	_BitRxDelay	
	movwf	TMR0		;21 cycles from start of interrupt
	goto	Restore

; if we get to here it's because we need to check for the stop bit.

GetStopBit
	btfss	PORTB,_SER_IN	;is the RX line low? If so, it's not
	goto	Done		;the stop bit. Otherwise, set the
	movlw	b'00000001'	;SerialReg to show a character has
	movwf	SerialReg	;been received
	movf	RXChar,W	;copy the received character to RXBuff
	movwf	RXBuff
	goto	Done
		
; We got here because we're sending.
; check to see if we're finished sending the stop bit:

Sending
	btfsc	SerialReg,3
	goto	Done

; check to see if we need to send the stop bit:

	movf	BitCount,F
	btfsc	STATUS,Z	;18th cycle
	goto	SendStopBit

; if we get to here, we're in the middle of sending. Send the next
; bit: (16 cycles to get to the next instruction from the start of
; the interrupt).

	rrf	TXChar,F	;doing rrf on TXChar puts the
	btfss	STATUS,C	;least significant bit in the
	goto	SendZero	;carry flag.
	nop
	bsf	PORTA,_SER_OUT	;if carry is set, send a one.
	goto	EndDoBit	;PORTA,1 is set on the 24th cycle

SendZero
	bcf	PORTA,_SER_OUT	;otherwise, send a zero. (24th cycle)
	nop			;nop's are for taking the same time
	nop			;to get to reloading TMR0 as for when
				;a one is sent.

; Decrement the bit counter.

EndDoBit
	decf	BitCount,F

; reload TMR0 for the next interrupt, and
; go to the end of the interrupt routine.

	movlw	_BitTxDelay	
	movwf	TMR0		;29th cycle
	goto	Restore


; Here we need to send the stop bit, turn off the TMR0 interrupt,
; turn on the external interrupt, and set the SerStatus register
; flags appropriately.

SendStopBit
	nop
	nop
	nop
	bsf	PORTA,_SER_OUT	;no. Send the stop bit. (24th cycle)
	bsf	SerialReg,3	;set the "sending stop bit" flag

; reload TMR0 for the next interrupt, and
; go to the end of the interrupt routine.

	movlw	_StopTxDelay
	movwf	TMR0		;27th cycle
	goto	Restore

; we're completely done sending or receiving. Clean up.

Done	movlw	b'00010000'	;set bits in INTCON to enable
	movwf	INTCON		;external interrupt
	movlw	b'00000001'
	andwf	SerialReg,F	;clear the busy bits in SerialReg
	goto	Restore

;------end DoBit-----------------------------------------------------

;------Subroutine SendChar-------------------------------------------
;
; This is not called by the interrupt handler. Rather, it activates 
; the interrupts needed to send it. Put the character to be sent in
; the TXChar file register before calling this subroutine.
;

SendChar
	
; send the start bit:

	bcf	PORTA,_SER_OUT

; set the SerStatus to indicate that the routines are busy sending
; a character:

	movlw	b'00000110'
	movwf	SerialReg

; load up TMR0 so it overflows at the right time.

	nop			;for timing
	movlw	_BitTxDelay
	movwf	TMR0		;5th cycle after write to PORTA

; clear the external interrupt flag, disable the external interrupt,
; and enable the TMR0 interrupt.

	movlw	b'10100000'
	movwf	INTCON

; set the BitCount for the eight bits to send:

	movlw	8
	movwf	BitCount

	return

;------end SendChar--------------------------------------------------

;------begin GetAChar------------------------------------------------

GetAChar
	call	Idle
	btfss	SerialReg,0	;wait for a character to be received
	goto	GetAChar
	bcf	SerialReg,0
	return

;------end GetAChar--------------------------------------------------

;------begin SendAChar-----------------------------------------------

SendAChar
	call	SendChar

WaitToFinish
	call	Idle
	btfsc	SerialReg,1	;wait for the character to be sent
	goto	WaitToFinish
	return

;------end SendAChar-------------------------------------------------

;------Subroutine WaitMS---------------------------------------------
;
; WaitMS is an approximate millisecond delay. It assumes a 4 MHz
; oscillator, meaning instructions are executed at a rate of 1 MHz.
; I got the timing info (number of cycles per instruction) from the
; Microchip PIC16F84 data sheet.

; the call to this subroutine takes 2 cycles to execute.

WaitMS
	movlw	248		;1 cycle
	movwf	MSDelay		;1 cycle
	nop			;1 cycle--these nops are added to 
	nop			;1 cycle  make the total number of
	nop			;1 cycle  instructions executed in
				;         the routine to be 1000.
				;the nop instruction simply does 
				;nothing except take time to execute.

; The loop below takes four cycles for every time through except the
; last, when it takes five (including the time needed to execute the
; return). So, the total number of instructions executed in getting
; to and returning from this subroutine is:
;
;        2 to get here
;    +   2 to set the MSDelay value
;    +   3 for the nops
;   + 247*4 for the first 247 times through the loop
;    +   5 for the last time through the loop and to return
;   --------
;    = 1000

RepeatWaitMS
	nop			;1 cycle
	decfsz	MSDelay,F	;1 cycle if not zero, 2 if zero
	goto	RepeatWaitMS	;2 cycles
	return			;2 cycles

;------end WaitMS----------------------------------------------------

;------begin SHT11TXRX-----------------------------------------------
;
; Sends a byte command to the SHT11 temp/humidity sensor and retrieves
; a two-byte response. Sends the response back to the PC as an ASCII
; string representation of the number.
;
; Put the byte to send in SHT11Byte before calling this routine. 

SHT11TXRX

	;make _SHT11_DAT an output

	bsf	STATUS,RP0	;switch to bank 1
	bcf	TRISA,_SHT11_DAT	;make Port A data line an output
	bcf	STATUS,RP0	;switch back to bank 0

	;send the Transmission Start sequence:

	bsf	PORTA,_SHT11_DAT	;set the data line high
	bsf	PORTA,_SHT11_SCK	;take the clock line high
	bcf	PORTA,_SHT11_DAT	;take the data line low
	bcf	PORTA,_SHT11_SCK	;take the clock line low
	bsf	PORTA,_SHT11_SCK	;take the clock line high again
	bsf	PORTA,_SHT11_DAT	;set the data line high again
	
	;load up the counter to loop through the eight bits to send:

	movlw	8
	movwf	counter

SHT11SendBitLoop
	bcf	PORTA,_SHT11_SCK	;take the clock line low

	btfss	SHT11Byte,7	;is the next bit to send a one?
	goto	SHT11SendZero	;nope. Go send a zero.
	bsf	PORTA,_SHT11_DAT	;if it's a one, send it.
	goto	SHT11SendBit

SHT11SendZero
	bcf	PORTA,_SHT11_DAT	;set the data line to zero

SHT11SendBit
	bsf	PORTA,_SHT11_SCK	;take the clock line high to send

	rlf	SHT11Byte,F	;move the next bit into MSB

	decfsz	counter,F	;dec the counter and check for zero.
	goto	SHT11SendBitLoop	;if not zero, more bits to send

	bcf	PORTA,_SHT11_SCK	;take the clock line low

	;no more bits to send. Set the data line to be an input and
	;wait for the ack from the SHT11:

	bsf	STATUS,RP0	;switch to bank 1
	bsf	TRISA,_SHT11_DAT	;make Port A data line an input
	bcf	STATUS,RP0	;switch back to bank 0

	; now look for an ack (SHT11 pulls data line low--should
	; happen on the next rise of the SCK line). If it doesn't
	; happen, return an 'e' and quit.

	bsf	PORTA,_SHT11_SCK

SHT11WaitAck
	btfss	PORTA,_SHT11_DAT
	goto	SHT11GotAck

	;if we don't get an ack, quit, send an 'e1' for error and return.

	movlw	'1'
	movwf	digit
	call	SendErrorCode
	goto	SHT11TXRXDone

	; we got an Ack. Get ready for the data to be returned. take
	; the clock line low, and then wait for the data line to be
	; pulled low again.

SHT11GotAck
	bcf	PORTA,_SHT11_SCK

	; now wait for the data. It takes approximately 210 ms for
	; the temperature measurement, or 55 ms for the humidity
	; measurement, so we'll wait up to 255 ms before giving up.

	movlw	255
	movwf	counter

SHT11WaitData
	btfss	PORTA,_SHT11_DAT
	goto	SHT11DataReady
	call	WaitMS
	decfsz	counter,F
	goto	SHT11WaitData

	;if we don't get the data, quit, send an 'e2' error and return.

	movlw	'2'
	movwf	digit
	call	SendErrorCode
	goto	SHT11TXRXDone

SHT11DataReady

	; get the most sig byte:

	call	SHT11GetByte
	movf	SHT11Byte,W
	movwf	hi
	call	SendAck		;acknowledge the byte

	; get the least sig byte:

	call	SHT11GetByte
	movf	SHT11Byte,W
	movwf	lo
	call	SendAck		;acknowledge the byte

	call	SHT11GetByte	; gets the checksum (not needed)

	; Send the data as ASCII:
	call	SendAsciiNum

	; Send the terminating CR and LF:
	call	SendCRLF

SHT11TXRXDone
	return

;------end SHT11TXRX--------------------------------------------------

;------begin SHT11GetByte---------------------------------------------
;
; Gets a byte of data from the SHT11. Assumes that the data
; is ready to be sent by the SHT11. Also assumes that _SHT11_DAT has
; been set to input. Also assumes that _SHT11_SCK has been set to low.
; Returns the byte in SHT11Byte.

SHT11GetByte
	; clear SHT11Byte:

	clrf	SHT11Byte

	; set counter to get eight bits

	movlw	8
	movwf	counter

SHT11GetByteLoop
	bsf	PORTA,_SHT11_SCK	;set the clock high to get the next bit
	btfss	PORTA,_SHT11_DAT	;is the next bit a one?
	goto	SHT11GetZeroBit	;no--it's a zero
	bsf	SHT11Byte,0	;if it's a one, set the LSB in SHT11Byte
	goto	SHT11GotBit

SHT11GetZeroBit
	bcf	SHT11Byte,0	;set the LSB to zero in SHT11Byte

SHT11GotBit
	bcf	PORTA,_SHT11_SCK	;set the clock line low again.
	decfsz	counter,F
	goto	SHT11GetNextBit
	goto	SHT11GetByteDone

SHT11GetNextBit
	rlf	SHT11Byte,F	;move the bits over to get the next bit
	goto	SHT11GetByteLoop

SHT11GetByteDone

	return

;------end SHT11GetByte-----------------------------------------------

;------begin SendAck-------------------------------------------------
;
; send the ack. Set the data line as an output:

SendAck
	bsf	STATUS,RP0	;switch to bank 1
	bcf	TRISA,_SHT11_DAT	;make Port A data line an output
	bcf	STATUS,RP0	;switch back to bank 0

	; now send the ack. Take the data line low.

	bcf	PORTA,_SHT11_DAT
	bsf	PORTA,_SHT11_SCK
	bcf	PORTA,_SHT11_SCK

	; now make the data line an input again.

	bsf	STATUS,RP0	;switch to bank 1
	bsf	TRISA,_SHT11_DAT	;make Port A data line an input
	bcf	STATUS,RP0	;switch back to bank 0

	return

;------end SendAck---------------------------------------------------

;------begin SendErrorCode-------------------------------------------
;
; send error code back to PC. Error code is 'e' plus a digit. Load
; ASCII value of digit into 'digit' register before calling.

SendErrorCode
	movlw	'e'
	movwf	TXChar
	call	SendAChar
	movf	digit,W
	movwf	TXChar
	call	SendAChar
	call	SendCRLF
	return
	
;------end SendErrorCode---------------------------------------------

;------begin TellTemperature-----------------------------------------

;This subroutine is called when the 't' command is received. Calls
;SHT11TXRX.

TellTemperature

	movlw	3
	movwf	SHT11Byte
	call	SHT11TXRX

	goto	MainLoop

;------end TellTemperature-------------------------------------------

;------begin TellHumidity--------------------------------------------

;This subroutine is called when the 'h' command is received. Calls
;SHT11TXRX.

TellHumidity

	movlw	5
	movwf	SHT11Byte
	call	SHT11TXRX

	goto	MainLoop

;------end TellHumidity----------------------------------------------

;------begin SendCRLF------------------------------------------------
;
; Send the terminating CR and LF:

SendCRLF
	movlw	13
	movwf	TXChar
	call	SendAChar
	movlw	10
	movwf	TXChar
	call	SendAChar
	return

;------end SendCRLF--------------------------------------------------

;------begin SendAsciiNum--------------------------------------------
;
; load lo, hi with 16 bit unsigned num to send

SendAsciiNum
	dodigit	10000
	movf	digit,W
	movwf	TXChar
	call	SendAChar
	dodigit	1000
	movf	digit,W
	movwf	TXChar
	call	SendAChar
	dodigit	100
	movf	digit,W
	movwf	TXChar
	call	SendAChar
	dodigit	10
	movf	digit,W
	movwf	TXChar
	call	SendAChar
	movf	lo,w		; ls byte is already correct
	addlw	'0'		; convert to ascii
	movwf	TXChar
	call	SendAChar
	return			; done

; "dosub" is called by the "dodigit" macro defined above.
; Subtract the number in shi/slo from hi/lo until the result
; is negative, incrementing the ascii equivelent each time.

dosub	movlw	'0'-1
	movwf	digit
moresub	incf	digit,F		; increment ASCII character
	movf	slo,w		; subtract current power of 10
	subwf	lo,f
	movf	shi,w
	btfss	STATUS,C
	addlw	1
	subwf	hi,f
	btfsc	STATUS,C	; any carry?
	goto	moresub		; no, keep subtracting

	movf	slo,w		; reverse the last subtraction
	addwf	lo,f
	movf	shi,w
	btfsc	STATUS,C
	addlw	1
	addwf	hi,f
	return

;------end SendAsciiNum----------------------------------------------

;------begin ReportVersion-------------------------------------------

; send a string with the version in it. The string comes from EEPROM
; memory and is null-terminated. The null terminator is not sent. The
; protocol dictates that the string sent is terminated by a CR, which
; is sent. This subroutine is called when the 'v' command is received.

ReportVersion
	bcf	STATUS,RP0
	clrf	EEADR		;the string we want starts at the 
				;beginning of EEPROM memory.

GetNextVersionChar
	bsf	STATUS,RP0
	bsf	EECON1,RD
	bcf	STATUS,RP0
	movf	EEDATA,W
	btfsc	STATUS,Z	;if the character in W is null, don't
                                ;send any more.
	goto	MainLoop

	movwf	TXChar
	call	SendAChar
	incf	EEADR,F
	goto	GetNextVersionChar

;------end ReportVersion---------------------------------------------

;------begin Idle----------------------------------------------------
;
; Idle should be called whenever the chip is waiting for something
; to happen (waiting for a character to be sent or received, for
; example). Here it's not doing anything.

Idle
	return

;------end Idle------------------------------------------------------

;------Main Program--------------------------------------------------

Main

; set up the ports as inputs and outputs as needed.

	bsf	STATUS,RP0	;switch to bank 1
	movlw	0xFF
	movwf	TRISB		;make Port B input
	movlw	0x00
	movwf	TRISA		;make Port A output
	bcf	STATUS,RP0

	clrf	PORTA

	call	SerSetup	;set up serial comm routines & int.

; this main program simply waits for characters to be received, then
; calls the handler for the command indicated by the received character.

MainLoop
	call	GetAChar	;wait for a character

	movf	RXBuff,W	;move the rx char into W
	sublw	't'		;compare with 't' character
	btfsc	STATUS,Z
	goto	TellTemperature	;if t, report the temperature

	movf	RXBuff,W	;move the rx char into W
	sublw	'h'		;compare with 'h' character
	btfsc	STATUS,Z
	goto	TellHumidity	;if h, report the humidity

	movf	RXBuff,W	;move the rx char into W
	sublw	'v'		;compare with 'v' character
	btfsc	STATUS,Z
	goto	ReportVersion	;if v, report the version number

	goto	MainLoop

;------Version EEPROM------------------------------------------------

	org	0x2100
	de	"WxPIC v0.3b (c) 2002 by NK0E",0x0D,0x0A,0x00	; Version 0.3b
	end
	
