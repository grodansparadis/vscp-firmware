;******************************************************************************
;                                                                             *
;    Filename:     boot18.asm                                                 *
;    Date:         02 July 2004                                               *
;    File Version: 1.0a (first public release)                                *
;                                                                             *
;    Author:       Philip Pemberton                                           *
;    Email:        <philpem@dsl.pipex.com>                                    *
;    Web:          http://www.philpem.dsl.pipex.com/                          *
;                                                                             *
;   This program is a simple boot loader for the PIC18F252 and PIC18F452.     *
;   It resides in the Boot Block (0x0000 to 0x01FF) and enables the code in   *
;   the MCU to be reprogrammed as and when necessary.                         *
;   Upon startup, the bootloader will send a "H" over the RS232 link at       *
;   19200 Baud. The host must respond with an "i" within 200mS to activate    *
;   the loader, otherwise the bootloader will pass control to the             * 
;   application program. If no code has been loaded (i.e. the word at         *
;   progmem address 0x0200 is equal to 0x3FFF), the bootloader will keep      *
;   waiting for the entry signal ad infinitum.                                *
;                                                                             *
;   And now for the obligatory legalese.                                      *
;                                                                             *
;   Copyright (c) 2004, Philip Pemberton                                      *
;   All rights reserved.                                                      *
;                                                                             *
;   Redistribution and use in source and binary forms, with or without        *
;   modification, are permitted provided that the following conditions are    *
;   met:                                                                      *
;                                                                             *
;     * Redistributions of source code must retain the above copyright        *
;         notice, this list of conditions and the following disclaimer.       *
;     * Redistributions in binary form must reproduce the above copyright     *
;         notice, this list of conditions and the following disclaimer in     *
;         the documentation and/or other materials provided with the          *
;         distribution.                                                       *
;                                                                             *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
;   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
;   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      *
;   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
;   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
;   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
;   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
;   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
;   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
;                                                                             *
;   If you make use of this code, I would appreciate it if you sent me a      *
;   short email to tell me where you use it, what you use it for, etc.        *
;                                                                             *
;******************************************************************************
;                                                                             *
;    Files required:         P18F452.INC                                      *
;                                                                             *
;******************************************************************************

	LIST P=18F452		;directive to define processor
	#include <P18F452.INC>	;processor specific variable definitions

;******************************************************************************
; Constants

; Note: SPBRG=D'12', BRGH=1, Fosc=4MHz produces a baud rate of 19200
; SPBRG constant
BAUD_CONST		EQU		D'12'
; Comment the next line out for BRGH = 0
#define	BRGH_HIGH

; Write protect the boot block
#define	PROTECT_BOOTLOADER

; Bootloader version
VERSION_MAJOR	EQU		"1"		; version 1.0a
VERSION_MINOR	EQU		"0"
VERSION_FLAG	EQU		"a"

;;; Commands
CMD_CODEWRITE	EQU		0x00	; Program Memory Write
CMD_DATAWRITE	EQU		0x01	; Data Memory Write
CMD_IDENTIFY	EQU		0x02	; Device Identify/Bootloader Version
CMD_DATAREAD	EQU		0x03	; Data Memory Read
CMD_RESET		EQU		0x04	; Reset target device
LAST_COMMAND	EQU		0x04	; Last possible command value

;;; Error codes
ERR_NOERROR		EQU		0x00	; No error
ERR_BADPACKET	EQU		0x01	; Bad packet ID
ERR_BADCHECKSUM	EQU		0x02	; Bad checksum
ERR_VERIFYFAIL	EQU		0x03	; Verify failed

;******************************************************************************
; Configuration bits

	__CONFIG	_CONFIG1H, _OSCS_OFF_1H & _HS_OSC_1H
	__CONFIG	_CONFIG2L, _BOR_OFF_2L & _PWRT_ON_2L
	__CONFIG	_CONFIG2H, _WDT_OFF_2H
	__CONFIG	_CONFIG3H, _CCP2MX_OFF_3H
	__CONFIG	_CONFIG4L, _STVR_OFF_4L & _LVP_OFF_4L & _DEBUG_OFF_4L
	__CONFIG	_CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L 
	__CONFIG	_CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
	__CONFIG	_CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
#ifdef	PROTECT_BOOTLOADER		; Protect the bootloader from overwrites
	__CONFIG	_CONFIG6H, _WRTC_OFF_6H & _WRTB_ON_6H & _WRTD_OFF_6H
#else
	__CONFIG	_CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
#endif
	__CONFIG	_CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
	__CONFIG	_CONFIG7H, _EBTRB_OFF_7H

;******************************************************************************
; Variable definitions

		CBLOCK	0x080			; Main GPR RAM
		ENDC

		CBLOCK	0x000			; Access RAM
			PacketType, LoadAddress:3		; Packet type and load address
			DataLength						; Buffer length
			DataBuffer:64					; 96-byte data buffer
			DataChecksum					; Data buffer checksum
			d1, d2, d3						; Delay counters
			Loop							; Loop counter
			ByteCounter, SegmentCounter		; Byte and segment counters
		ENDC

;******************************************************************************
; Reset vector

		ORG	0x0000
		BRA		Main						; Jump to start of main code

;******************************************************************************
; Interrupt vectors

		ORG	0x0008							; High priority
		BRA		HighIntReloc

		ORG	0x0018							; Low priority
		BRA		LowIntReloc

;******************************************************************************
; Pass control onto the user program
PassControl:
		CLRF	RCSTA						; Disable USART
		CLRF	TXSTA
		SETF	TRISC
		BRA		ResetReloc	; Now the PIC is in more or less the same shape
							; it was in on POR, so jump to the user code

;******************************************************************************
; Start of main program

Main:
; RS232 setup
		BCF		TRISC,		TX				; Set up the RS232 pins
		BSF		TRISC,		RX
		MOVLW	BAUD_CONST					; Set up the baud rate generator
		MOVWF	SPBRG
#ifdef	BRGH_HIGH
		MOVLW	B'00100100'					; Set up and enable the transmitter
#else
		MOVLW	B'00100000'
#endif
		MOVWF	TXSTA
		MOVLW	B'10010000'					; Set up and enable the receiver
		MOVWF	RCSTA
		BCF		INTCON,			GIE			; Interrupts off

WaitForHost:
; Ask the host to respond
		MOVLW	'H'							; Send a "H" over the RS232 bus
		RCALL	RS232_Send
		RCALL	Delay						; Wait half a second
		BTFSS	PIR1,			RCIF		; Anything in the buffer?
		BRA		PassControl					; No, pass control to user code

		RCALL	RS232_Receive				; Read the byte from the host
		XORLW	'i'							; Was it an "i"?
		BNZ		PassControl					; No, pass control to user code

		MOVLW	'K'							; Yes, acknowledge it
		RCALL	RS232_Send

; Welcome to Bootloader Land
BootloaderLoop:
		RCALL	RS232_Receive				; Get the Packet Type
		MOVWF	PacketType
		RCALL	RS232_Receive				; Get the Load Address MSB
		MOVWF	LoadAddress+0
		RCALL	RS232_Receive
		MOVWF	LoadAddress+1
		RCALL	RS232_Receive				; Get the Load Address LSB
		MOVWF	LoadAddress+2
		RCALL	RS232_Receive				; Get the data length
		MOVWF	DataLength

		MOVF	DataLength,		W			; Any data?
		BZ		NoData						; No, don't read the data or CRC

		CLRF	DataChecksum				; Clear the checksum accumulator
		LFSR	FSR0,			DataBuffer	; Set up the FSR
		MOVFF	DataLength,		Loop		; Get the data
ReadDataLoop:
		RCALL	RS232_Receive				; Get a byte
		MOVWF	POSTINC0					; Save and inc FSR
		ADDWF	DataChecksum,	F			; Update the checksum
		DECFSZ	Loop,			F			; Decrement loop counter
		BRA		ReadDataLoop				; if >0, keep looping

		RCALL	RS232_Receive				; Get the checksum
		ADDWF	DataChecksum,	F
		BZ		ChecksumCorrect				; 2s complement + sum = 0

		MOVLW	ERR_BADCHECKSUM				; Bad checksum
		RCALL	RS232_Send
		BRA		BootloaderLoop				; Bomb out

ChecksumCorrect:
NoData:
; Check the packet type byte
;		MOVF	PacketType,		W
;		XORLW	CMD_RESET					; Reset Target?
;		BZ		0x0000						; 0x0000 is the reset vector
;		MOVF	PacketType,		W
;		XORLW	CMD_CODEWRITE				; Code Memory Write?
;		BZ		CodeMemPacket
;		MOVF	PacketType,		W
;		XORLW	CMD_DATAWRITE				; Data Memory Write?
;		BZ		DataMemPacket
;		MOVF	PacketType,		W
;		XORLW	CMD_IDENTIFY				; Request Device ID and Device ID
;		BZ		IDRequest
;		MOVF	PacketType,		W
;		XORLW	CMD_DATAREAD				; Data Memory Read?
;		BZ		DataMemReadPacket
;		MOVLW	ERR_BADPACKET				; Bad packet type
;		RCALL	RS232_Send
;		BRA		BootloaderLoop				; Bomb out

		MOVF	PacketType,		W			; Check the command value
		SUBLW	LAST_COMMAND
		BNC		InvalidCommand
		CLRF	PCLATH						; Prepare for jump table
		CLRF	PCLATU
		RLNCF	PacketType,		W			; Each instr. = 2 bytes
		ADDWF	PCL,			F			; Jump
		BRA		CodeMemPacket				; 0x00	Code memory write
		BRA		DataMemPacket				; 0x01	Data memory write
		BRA		IDRequest					; 0x02	Identify
		BRA		DataMemReadPacket			; 0x03	Data memory read
		BRA		0x0000						; 0x04	Reset

InvalidCommand:
		MOVLW	ERR_BADPACKET				; Bad packet type
		RCALL	RS232_Send
		BRA		BootloaderLoop				; Bomb out

;******************************************************************************
; Code Memory Write handler
CodeMemPacket:
		MOVF	LoadAddress+2,	W			; Load address LSB
		ANDLW	D'63'						; IF LoadAddr & 63 != 0...
		BNZ		DontEraseCodeBlk			; Don't erase codemem block
											; Else erase the code memory block
		MOVFF	LoadAddress+0,	TBLPTRU		; Address upper byte
		MOVFF	LoadAddress+1,	TBLPTRH		; Address high byte
		MOVFF	LoadAddress+2,	TBLPTRL		; Address low byte
		BSF		EECON1,			EEPGD		; Point to FLASH program memory
		BCF		EECON1,			CFGS		; PROGMEM, not CFGMEM
		BSF		EECON1,			WREN		; Enable writes
		BSF		EECON1,			FREE		; Enable row-erase operations
		MOVLW	0x55						; First byte of write sequence
		MOVWF	EECON2
		MOVLW	0xAA						; Second byte of write sequence
		MOVWF	EECON2
		BSF		EECON1,			WR			; Initiate the write operation
DontEraseCodeBlk:
		LFSR	FSR0, DataBuffer			; Set up FSR0
		MOVFF	LoadAddress+0,	TBLPTRU		; Set up the table pointer address
		MOVFF	LoadAddress+1,	TBLPTRH
		MOVFF	LoadAddress+2,	TBLPTRL
		TBLRD*-								; Dummy decrement

		MOVLW	D'2'						; 2 segments to load
		MOVWF	SegmentCounter
SegmentLoop:
		MOVLW	D'8'						; 8 bytes to load
		MOVWF	ByteCounter
WriteHregLoop:
		MOVFF	POSTINC0,		TABLAT		; Copy one byte to the table latch
		TBLWT+*								; Write to Hreg with preincrement
		DECFSZ	ByteCounter,	F			; Decrement byte counter
		BRA		WriteHregLoop				; Keep looping if >0

		BCF		EECON1,			CFGS		; PROGMEM, not CFGMEM
		BSF		EECON1,			EEPGD		; Write to program memory

		BSF		EECON1,			WREN		; Enable FLASH writes
		MOVLW	0x55						; First byte of write sequence
		MOVWF	EECON2
		MOVLW	0xAA						; Second byte of write sequence
		MOVWF	EECON2
		BSF		EECON1,			WR			; Initiate the write operation
											; CPU halts until write completes
		BCF		EECON1,			WREN		; Disable memory writes

		DECFSZ	SegmentCounter,	F			; Decrement segment counter
		BRA		SegmentLoop					; Keep looping if >0

; Now verify the data that was written
		LFSR	FSR0,			DataBuffer	; Set up FSR0
		MOVFF	LoadAddress+0,	TBLPTRU		; Set up the table pointer address
		MOVFF	LoadAddress+1,	TBLPTRH
		MOVFF	LoadAddress+2,	TBLPTRL
		MOVLW	D'16'						; 16 bytes to verify
		MOVWF	ByteCounter
VerifyCodeLoop:
		TBLRD*+								; Read a byte from progmem
		MOVF	POSTINC0,		W			; Compare the two bytes
		XORWF	TABLAT,			W
		BZ		CodeByteIsCorrect			; If equal, keep looping

		MOVLW	ERR_VERIFYFAIL				; Else bomb out
		RCALL	RS232_Send
		BRA		BootloaderLoop

CodeByteIsCorrect:
		DECFSZ	ByteCounter,	F			; Decrement byte counter
		BRA		VerifyCodeLoop				; If >0, keep looping

		MOVLW	ERR_NOERROR					; Verified OK, exit with no error
		RCALL	RS232_Send
		BRA		BootloaderLoop

;******************************************************************************
; Data Memory Write handler
DataMemPacket:
		LFSR	FSR0,			DataBuffer	; Set up FSR0
		MOVFF	LoadAddress+2,	EEADR		; Set up the EEPROM address
		MOVFF	DataLength,		Loop		; Set up the loop counter
DataMemLoop:
		BCF		PIR2,			EEIF		; Clear the "write complete" flag

		MOVFF	INDF0,			EEDATA		; Load EEDATA
		BCF		EECON1,			EEPGD		; Point to data EEPROM
		BCF		EECON1,			CFGS		; Access progmem or data EEPROM
		BSF		EECON1,			WREN		; Enable writes

		MOVLW	0x55						; First byte of write sequence
		MOVWF	EECON2
		MOVLW	0xAA						; Second byte of write sequence
		MOVWF	EECON2
		BSF		EECON1,			WR			; Initiate the write operation
DataMemProgWait:
		BTFSS	PIR2,			EEIF		; Wait for the write to complete
		BRA		DataMemProgWait

		BCF		EECON1,			WREN		; Disable writes

		BSF		EECON1,			RD			; Read from data EEPROM
		MOVF	POSTINC0,		W			; Compare and increment pointer
		XORWF	EEDATA,			W
		BZ		DataMemVerifyGood			; Verify OK

		MOVLW	ERR_VERIFYFAIL				; Verify failure
		RCALL	RS232_Send
		BRA		BootloaderLoop				; Bomb out

DataMemVerifyGood:
		INCF	EEADR,			F			; Increment EEPROM address
		DECFSZ	Loop,			F			; Decrement loopvar
		BRA		DataMemLoop					; Keep looping if >0

		MOVLW	ERR_NOERROR					; Operation Successful
		RCALL	RS232_Send
		BRA		BootloaderLoop

;******************************************************************************
; "Request Device ID and Bootloader Version" handler
IDRequest:
		MOVLW	ERR_NOERROR					; Operation Successful
		RCALL	RS232_Send

; Send the device ID
		MOVLW	0x3F						; Device ID byte 1 base address
		MOVWF	TBLPTRU
		SETF	TBLPTRH						; TBLPTRH = 0xFF
		MOVLW	0xFE
		MOVWF	TBLPTRL
		TBLRD*+								; Read DevID byte 1 and inc ptr
		MOVF	TABLAT,			W
		RCALL	RS232_Send
		TBLRD*								; Read DevID byte 2
		MOVF	TABLAT,			W
		RCALL	RS232_Send

; Send the bootloader version ID
		MOVLW	VERSION_MAJOR
		RCALL	RS232_Send
		MOVLW	VERSION_MINOR
		RCALL	RS232_Send
		MOVLW	VERSION_FLAG
		RCALL	RS232_Send
		
		BRA		BootloaderLoop				; Execute another command

;******************************************************************************
; Data Memory Read handler
DataMemReadPacket:
		MOVLW	ERR_NOERROR					; No error
		RCALL	RS232_Send

		CLRF	DataChecksum				; Clear the checksum accumulator

		MOVFF	LoadAddress+2,	EEADR		; Set up the EEPROM address
		MOVFF	DataLength,		Loop		; Set up the loop counter
DataMemReadLoop:
		BCF		EECON1,			EEPGD		; Point to data EEPROM
		BCF		EECON1,			CFGS		; Access progmem or data EEPROM
		BSF		EECON1,			RD			; Initiate the read operation

		MOVF	EEDATA,			W
		ADDWF	DataChecksum,	F			; Update the checksum
		RCALL	RS232_Send					; Send EEDATA over the RS232 link

		INCF	EEADR,			F			; Increment EEPROM address
		DECFSZ	Loop,			F			; Decrement loopvar
		BRA		DataMemReadLoop				; Keep looping if >0

		NEGF	DataChecksum,	W			; 2's complement (inversion)
		CALL	RS232_Send					; Send the checksum

		BRA		BootloaderLoop				; Execute another command

;******************************************************************************
;******************************************************************************
; Subroutines
;******************************************************************************
;******************************************************************************

;******************************************************************************
; Send a byte over the RS232 link
RS232_Send:
		BTFSS	PIR1,			TXIF		; Wait for xmit buffer to empty
		BRA		RS232_Send
		MOVWF	TXREG						; Send the byte
		RETURN

;******************************************************************************
; Read a byte from the RS232 link
RS232_Receive:
		BTFSS	PIR1,			RCIF		; Wait for a byte to arrive
		BRA		RS232_Receive
		MOVF	RCREG,			W			; Receive the byte
		RETURN

;******************************************************************************
; half second delay
Delay
			;499994 cycles
	movlw	0x03
	movwf	d1
	movlw	0x18
	movwf	d2
	movlw	0x02
	movwf	d3
Delay_0
	decfsz	d1, f
	bra		$+4
	decfsz	d2, f
	bra		$+4
	decfsz	d3, f
	bra		Delay_0

			;2 cycles
	bra		$+2

			;4 cycles (including call)
	return

;******************************************************************************
; Relocation points
		ORG		0x0200
ResetReloc:
;		BRA		$		; Stop the loader going off the rails if there's no
						; user program loaded
		ORG		0x0208
HighIntReloc:
		ORG		0x0218
LowIntReloc:

;******************************************************************************
; Data EEPROM
;		ORG	0xf00000

;		DE	"data eeprom shit"

;******************************************************************************
;End of program

		END
