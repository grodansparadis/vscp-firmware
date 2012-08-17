;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Sensirion SHT11 sensor interface                                  ;;
;;                                                                   ;;
;; Copyleft  (L) P Pemberton                                         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
;; PIC setup and toplevel includes
	LIST    R=DEC
    INCLUDE sxdefs.inc
 	LIST
 	PROCESSOR       16C56   ; &quot;SX18AB&quot;
 	LIST
 	DATA    _FUSE           ;configuration bits (TURBO, SYNC, OPTIONX, etc.)
 	DATA    _FUSEX          ; (PINS, CARRYX, BOR40, BANKS, PAGES)
 	LIST
    INCLUDE sxregs.inc
    
;-----------------------------  Basic defines  --------------------------------
 
;For instruction destination argument
W       EQU     0
F       EQU     1
 
Indir   EQU     00h             ;Used for indirects thru FSR
RTCC    EQU     01h             ;Real Time Clock/Counter
PC      EQU     02h             ;Program Counter Low
FSR     EQU     04h             ;File Select Register (index register)
PORTA   EQU     05h             ;I/O Port A
PORTB   EQU     06h             ;I/O Port B
PORTC   EQU     07h             ;I/O Port C
 
Status  EQU     03h             ;Status register:
#DEFINE CF      Status,0        ; Bit 0 = Carry Flag
#DEFINE DCF     Status,1        ; Bit 1 = Digit Carry Flag
#DEFINE ZF      Status,2        ; Bit 2 = Zero Flag
#DEFINE PDF     Status,3        ; Bit 3 = Power Down Flag
#DEFINE TOF     Status,4        ; Bit 4 = Time Out Flag
#DEFINE Pa0     Status,5        ; Bit 5 = Program Page select bit LSB
#DEFINE Pa1     Status,6        ; Bit 6 = Program Page select bit
#DEFINE Pa2     Status,7        ; Bit 7 = Program Page select bit MSB
 
;Option EQU     81h     ;Option register
#DEFINE PS0     Option,0        ; Bit 0 = Prescaler divider
#DEFINE PS1     Option,1        ; Bit 1 =   Divide of 2 thru 256 for RTCC
#DEFINE PS2     Option,2        ; Bit 2 =   Divide of 1 thru 128 for WDT
#DEFINE PSA     Option,3        ; Bit 3 = Prescaler assign: 0 = RTCC, 1 = WDT
#DEFINE RTE_ES  Option,4        ; Bit 4 = RTCC clock edge: 0 = low to high
#DEFINE RTS     Option,5        ; Bit 5 = RTCC clock source: 0 = internal
#DEFINE RTE_IE  Option,6        ; Bit 6 = RTCC Interrupt Enable: 0 = enabled
#DEFINE RTW     Option,7        ; Bit 7 = W-reg mapped to RTCC = 0
	EXPAND                          ; Expand must be ON for ICD    

#DEFINE BANK0   BCF     STATUS, PA0
#DEFINE BANK1   BSF     STATUS, PA0

; SX18AB
DEVICE  EQU     PINS18+OSC4MHZ+PAGES2+BANKS2
RAMBase EQU     07h             ;Start of RAM
 
SEROUT  MACRO   __byte
		MOVLW   __byte
;       CALL    SER_SEND
        ENDM
 
CHKWZ   MACRO   __outz, __outnz
        IORLW   .0
        MOVLW   __outz
        BTFSS   STATUS,         Z
        MOVLW   __outnz
;       CALL    SER_SEND
        ENDM
 
; Pindefs
#DEFINE SHT_SDA PORTB,  7
#DEFINE SHT_SCL PORTB,  6
 
; Variables
		CBLOCK  RAMBase
         TEMPERATURE:2
         HUMIDITY:2
         CHKSUM
         CHAHI, CHALO
     	ENDC
       
; Reset vector
		ORG     0
		GOTO    START
; Interrupt vector
		ORG     4
		GOTO    START
                    
; Start of code
ORG     5

START:
;       CALL    SER_SETUP               ; Set up the serial port
 
        CALL    SHT_BUSINIT             ; Init the SHT bus
        MOVLW   .5                      ; Read Humidity
        CALL    SHT_SEND
        CALL    SHT_WAITACK             ; Wait for an ACK
        CHKWZ   &quot;A&quot;,&quot;!&quot;
        IORLW   .0
        MOVLW   &quot;A&quot;
        BTFSS   STATUS,         Z
        MOVLW   &quot;!&quot;
;       CALL    SER_SEND
 		CALL    SHT_WAIT                ; Wait for the data
        CHKWZ   &quot;W&quot;,&quot;!&quot;
        IORLW   .0
        MOVLW   &quot;W&quot;
        BTFSS   STATUS,         Z
        MOVLW   &quot;!&quot;
 ;       CALL    SER_SEND
 
        CALL    SHT_RECEIVE             ; Get byte 1
        MOVWF   HUMIDITY+0              ; Store it
        CALL    SHT_ACK                 ; Wait for an ACK
 
        CALL    SHT_RECEIVE             ; Get byte 2
        MOVWF   HUMIDITY+1              ; Store it
        CALL    SHT_ACK                 ; Wait for an ACK
 
        CALL    SHT_RECEIVE
        MOVWF   CHKSUM
        CALL    SHT_NACK
 
        MOVF    HUMIDITY+0,     W       ; Send humidity to host
        CALL    HEXOUT
		MOVF    HUMIDITY+1,     W
        CALL    HEXOUT
        MOVF    CHKSUM,         W
        CALL    HEXOUT

        SEROUT  &quot; &quot;
        MOVLW   &quot; &quot;
;       CALL    SER_SEND
 
        GOTO    $                       ; Crash!

;; Subroutines
 
HEXOUT:
  		MOVWF   CHALO
        SWAPF   CHALO,          W
        ANDLW   0x0F
        MOVWF   CHAHI
        MOVLW   0x0F
        ANDWF   CHALO
 
        MOVF    CHAHI,          W
        CALL    HEXTAB
;       CALL    SER_SEND
 
        MOVF    CHALO,          W
        CALL    HEXTAB
;       CALL    SER_SEND

 		RETURN
                
HEXTAB:
		ADDWF   PC,     F
		DT &quot;0123456789ABCDEF&quot;
 
;; Final post-code includes
#DEFINE SER_BAUD .19200
 

INCLUDE "sht11.inc";

                             
        END
 
