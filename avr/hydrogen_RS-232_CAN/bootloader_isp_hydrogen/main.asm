;/**********************************************************************************
; File: main.c
;
; Copyright (C) 1996-1998 Atmel Corporation
; Changes Copyright (C) Andy Hutchinson
; and
; Changes Copyright (C) 2002 Ake Hedman, akhe@eurosource.se, 
; http://www.eurosource.se
;
; Description: 
;
;	This Program allows an AVR with bootloader capabilities to 
;   Read/write its own Flash/EEprom. To enter Programming mode   
;   PD4 is checked, it this pin is pulled low, programming mode  
;   is entered. If not, normal execution is done from $0000 
;   "reset" vector in Application area.
;
;   The PD4 pin should be pulled HIGH by an external Pull-up 
;   resistor.
;
; Other info:  
;
;	Code must be modified to use this specific code with devices
;   other than ATmega161. Following change should be made:
;
;      -Specify device in define list (e.g. _ATMEGA163)
;                 
;   When using this code with (future) parts other than ATmega32,
;   ATmega161 and ATmega163, it will be necessary to change more 
;   than the device name. The changes that should be made is:
;
;     - Device name
;     - Device signature and memory settings (defines.h)
;     - Device code
;     - Linker file
;
;   In general the linker file should always be verifyed to match
;   the used part's boot address and size. Note that memory size
;   is specified in bytes in the linker file.
;
;	intval          	R26-R27
;	data            	R16-R17
;	ldata           	R4
;	address         	R24-R25
;	val             	R16
;	funcptr         	R30-R31
;

	.include "m161def.inc"
	.CSEG

main:
 
        CLR     R30
        CLR     R31
  
        LDI     R16,3		; Baudrate: 3-115200 23-19200) 7.3728 MHz
        OUT     UBRR0,R16
 
        LDI     R16,24		; Enable recieve and transmit UART1
        OUT     UCSR0B,R16
	
;       PORTD pin 4 low to enter programming mode			
				
		CBI     DDRD,4		; PORTD.4 - Input
        SBI     PORTD,4		; Pull up enabled
 
        SBIC    PIND,4		; Boot mode?
        RJMP    runmode

;*************************************************************
;                        enter bootmode
;*************************************************************

bootmode:

        rcall	getchar

        CPI     R16,'a'			; autoincrement X?
        BRNE    bootmode2

;
; *** autoincrement * * *
;

        LDI     R16,'Y'			; OK
        RJMP    next_command

bootmode2:
 
        CPI     R16,'A'			; write address? 	
        BRNE    bootmode3

;
; *** Write Address * * *
;

        rcall 	getchar			; read address 8 MSB
        MOV     R25,R16

        rcall   getchar			; read address 8 LSB
        MOV     R24,R16

        LSL     R24				; *2
        ROL     R25
        RJMP    sendcr			; ACK

bootmode3:
 
        CPI     R16,'c'			; Write program memory, low byte?
        BRNE    bootmode4

;
; *** Write Program Memory, low byte * * *
;

        rcall   getchar
        MOV     R4,R16
        RJMP    sendcr			; ACK

bootmode4:

        CPI     R16,'C' 		; Write program memory, high byte?
        BRNE    bootmode5

;
; *** Write Program Memory, high byte * * *
;

        rcall   getchar
        MOV     R21,R16
        MOV     R20,R4
 
        MOVW    R16,R24
        rcall   fill_temp_buffer  
        ADIW    R24,2			; Next data pair
        RJMP    sendcr			; ACK

bootmode5:

        CPI     R16,'e'			; Chip erase
        BRNE    bootmode6
;
; *** Chip Erase * * *
;

        CLR     R24
        CLR     R25

bootmode_write_page:

        CPI     R25,LOW(60)
        BRCS    PC+2
        RJMP    sendcr			; ACK

;  Perform page erase

        LDI     R20,3
        Rcall   pre_write_page
        SUBI    R24,0x80
        SBCI    R25,0xff
        RJMP    bootmode_write_page

        
bootmode6:
 
        CPI     R16,'l'			; write lockbits?
        BRNE    bootmode7

;
; *** Write lockbits * * *
;

        rcall   getchar
        rcall   write_lock_bits
        RJMP    sendcr			; ACK

bootmode7:

        CPI     R16,'m'			; write page?
        BRNE    bootmode8

;
; *** Write page * * *
;

        LDI     R20,5
        Rcall   pre_write_page
        RJMP    sendcr			; ACK

bootmode8:
 
        CPI     R16,'P'			; Enter programming mode? 
        BRNE    PC + 2			
        RJMP    sendcr
        CPI     R16,'L'
        BRNE    bootmode9

;
; *** Enter Programming Mode * * *
;

bootmode_programming_mode:

        RJMP    sendcr			; ACK

bootmode9:
 
        CPI     R16,'p'			; Return programmer type?
        BRNE    bootmode10
 
;
; *** Return Programmer Type * * *
;

        LDI     R16,'S'			; Serial device
        RJMP    next_command

bootmode10:
 
        CPI     R16,'R'			; Read program memory 
        BRNE    bootmode11

        CLR  	R20
        MOVW    R16,R24
        rcall   read_program_memory
        ADIW    R24,LWRD(2)
        MOV     R26,R16

        MOV     R16,R17			; send MSB  
        rcall   sendchar		

        MOV     R16,R26			; send LSB
			
; 		SPM uses Z pointer but the pointer is only 16bit

        RJMP    next_command

bootmode11:

        CPI     R16,'D'			; Write Data Memory?
        BRNE    bootmode12

;
; *** Write Data Memory * * *
;

        OUT     EEARL,R24		; LSB
        OUT     EEARH,R25		; MSB
        ADIW    R24,1			; next address

        rcall   getchar
        OUT     EEDR,R16		; Write data
        SBI     EECR,EEMWE
        SBI     EECR,EEWE

wait_eeprom:
 
        SBIC    EECR,EEWE
        RJMP    wait_eeprom

        RJMP    sendcr			; ACK

bootmode12:
 
        CPI     R16,'d'			; Read data memory?
        BRNE    bootmode13

;
; *** Read Data Memory * * *
;

        OUT     EEARL,R24		; LSB
        OUT     EEARH,R25		; MSB
        ADIW    R24,1			; next address;
        SBI     EECR,EERE
        IN      R16,EEDR
        RJMP    next_command

bootmode13:

        CPI     R16,'F'			; read fuse bits?
        BRNE    bootmode14

;
; *** Read Fuse Bits * * *
;

        CLR     R16
        RJMP    read_fuse_bits1

bootmode14:

        CPI     R16,'r'			; read lock bits?
        BRNE    bootmode15


;
; *** Read Lock Bits * * *
;

        LDI     R16,1
        RJMP    read_fuse_bits1

bootmode15:

        CPI     R16,'N'			; read high fuse bits?
        BRNE    bootmode16

;
; *** Read High Fuse Bits * * *
;
 
        LDI     R16,3

read_fuse_bits1:

        LDI     R20,9        

read_fuse_bits2:

        LDI     R17,0
        rcall   read_program_memory
        RJMP    next_command

bootmode16:

        CPI     R16,'t'			; Return programmer type? 
        BRNE    bootmode17

;
; *** Return programmer type * * *
;

        LDI     R16,0x60		; 0x60 - ATmega161, 0x66 - Atmega163
        rcall   sendchar
        CLR     R16
        RJMP    next_command

bootmode17:

        CPI     R16,'x'			; set LED
        BREQ    bootmode18
        CPI     R16,'y'			; clear LED
        BREQ    bootmode18
        CPI     R16,'T'			; selct Device Type
        BRNE    bootmode19

;
; *** Read High Fuse Bits * * *
;

bootmode18:

        rcall    getchar		; get character 

sendcr:

        LDI     R16,13			; ACK
        RJMP    next_command

bootmode19:

        CPI     R16,'S'			; Return software identifier?
        BRNE    bootmode20

;
; *** Return software identifier * * *
;

		LDI		R30,LOW(ID<<1)
		LDI		R31,HIGH(ID<<1)
		RJMP	PC+2

IDLP:

		RCALL   sendchar
		LPM		R16,Z+
		TST		R16
		BRNE    IDLP
		RJMP	bootmode


ID:		.DB "AVRBOOT",0


bootmode20:
 
        CPI     R16,'V'			; Return Software Version?
        BRNE    bootmode21
 
;
; *** Return software version * * *
;

        LDI     R16,'1'
        rcall   sendchar
        LDI     R16,'0'
        RJMP    next_command

bootmode21:

        CPI     R16,'s'			; Return Signature Bytes?
        BRNE    bootmode22

;
; *** Return signature bytes * * *
;

        LDI     R16,0x01		; Use 0x02 for ATmega163
        rcall   sendchar
        
		LDI     R16,0x94		; Use 0x94 for ATmega163
        rcall   sendchar

        LDI     R16,0x1E		; Use 0x1E for ATmega163
        RJMP	next_command

bootmode22:

        CPI     R16,0x1b		; ESC
        BRNE    PC+2
        RJMP    bootmode
 
        LDI     R16,'?'

next_command:
        
		rcall   sendchar
		RJMP    bootmode

;*************************************************************
;                        enter runmode
;*************************************************************

runmode:
                   
        IJMP 			; Jump to Reset vector 0x0000 in Application Section   

pre_write_page:

        MOVW    R16,R24
        JMP     write_page


; intval	R26,R27
; data		R16,R17
; ldata		R4
; address	R24,R25
; val		R16
; funcptr	R30,R31

