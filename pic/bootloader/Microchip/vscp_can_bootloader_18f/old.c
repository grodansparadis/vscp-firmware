	movlw	0x30					; Program memory < 0x300000
	cpfslt	WREG2
	bra		DataRegJp1
	
	btfss	CAN_PG_BIT			
	bra		PMEraseWrite
	bra		PMRead
	
DataRegJp1	
	movf	WREG2,W					; Config memory = 0x300000
	xorlw	0x30
	bz		CFGDecode
	
	btfsc	CAN_PG_BIT			
	bra		PMRead
	
DataRegJp2
	movf	WREG2,W					; EEPROM data = 0xF00000
	xorlw	0xF0
	bz		EEDecode
	
	bra		CANMain	




; *********************************************************
EEReadOrWrite
	btfsc	CAN_PG_BIT
	bra		EERead

	btfsc	MODE_WRITE_LOCK			; Stop if write locked
	bra		CANMain	
	
	movf	POSTINC0, W				; Load data
	movwf	EEDATA
	
	rcall	UpdateChksum			; Adjust the checksum

	rcall	WriteEEWait
	bra		EEReadOrWriteJp1

EERead
	clrf	EECON1 

	bsf		EECON1, RD				; Read the data
	movff	EEDATA, POSTINC0
	
EEReadOrWriteJp1
	infsnz	EEADR, F				; Adjust EEDATA pointer
	incf	EEADRH, F

	decfsz	WREG1, F
	bra		EEReadOrWrite			; Not finished then repeat

	btfsc	CAN_PG_BIT
	bra		CANSendResponce
; *********************************************************


	movf	_bootSpcCmd, W			; NOP Command
	bz		CANSendAck

	xorlw	CMD_RESET				; RESET Command
	btfsc	STATUS, Z
	reset
	
	movf	_bootSpcCmd, W			; RESET_CHKSM Command
	xorlw	CMD_RST_CHKSM
	bnz		SpecialCmdJp1
	clrf	_bootChksmH				; Reset chksum
	clrf	_bootChksmL

SpecialCmdJp1
	movf	_bootSpcCmd, W			; RUN_CHKSM Command
	xorlw	CMD_CHK_RUN
	bnz		SpecialCmdJp2

	movf	_bootChkL, W			; Add the control byte
	addwf	_bootChksmL, F
	bnz		SpecialCmdJp2
	movf	_bootChkH, W
	addwfc	_bootChksmH, F
	bnz		SpecialCmdJp2
	
	setf	EEADR					; Point to last location of EEDATA
	setf	EEADRH	
	clrf	EEDATA					; and clear the data
	movlw	b'00000100'				; Setup for EEData
	rcall	StartWrite
	
SpecialCmdJp2	
	bra		CANSendAck				; or send an acknowledge


	btfss	_bootSpcCmd, 0
	bra		SpecialCmdJp1
	
	bcf		_bootSpcCmd, 0
	
	clrf	_bootChksmH				; Reset chksum
	clrf	_bootChksmL

SpecialCmdJp1
	btfss	_bootSpcCmd, 1
	bra		SpecialCmdJp2

	bcf		_bootSpcCmd, 1
	
	movf	_bootChkL, W			; Add the control byte
	addwf	_bootChksmL, F
	bnz		SpecialCmdJp2
	movf	_bootChkH, W
	addwfc	_bootChksmH, F
	bnz		SpecialCmdJp2
	
	setf	EEADR					; Point to last location of EEDATA
	setf	EEADRH	
	clrf	EEDATA					; and clear the data
	movlw	b'00000100'				; Setup for EEData
	rcall	StartWrite

SpecialCmdJp2
	btfsc	_bootSpcCmd, 7
	reset
	
	bra		CANSendAck				; or send an acknowledge
	
	
	

#if 0
; *****************************************************************************
_WRITEWAIT		CODE	0x0A
; *****************************************************************************
WriteEEWait:
	movlw	b'00000100'				; Setup for EEData
	rcall	StartWrite		
	btfsc	EECON1, WR				; Write and wait
	bra		$ - 2
	return
; *****************************************************************************
#endif
