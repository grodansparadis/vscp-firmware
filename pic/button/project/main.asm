; ***********************************************
; * 8pincan.asm                                 *
; * Revision 1.0   September 2000               *
; * Developed by Rick Stoneking                 *
; * Developed using MPLAB V4.12 and MPASM V2.3  *
; *                                             *
; * This code demonstrates how a very low cost  *
; * CAN node can be implemented using a         *
; * Microchip PIC12C672 8 pin microcontroller   *
; * and a Microchip MCP2510 Stand Alone CAN     *
; * controller.                                 *
; *                                             *
; ***********************************************

; ***********************************************
; * Setup the MPASM assembler options           *
; ***********************************************

        LIST    p=12F675


; ***********************************************
; * Include the standard PIC12C672 include file *
; * and the custom MCP2510 support files        *
; ***********************************************

#include <p12f675.inc>                  
#include "mcp2510.inc"                  


; ***********************************************
; * Setup the PIC12C675 configuration Word      *
; ***********************************************

    __CONFIG   _CP_OFF & _WDT_OFF & _MCLRE_OFF & _INTRC_OSC_NOCLKOUT


; ***********************************************
; * Constants definitions                       *
; ***********************************************
TMR_COUNT   EQU     0xD9                ; Timer Reload value: 
                                        ; 0xD9 = 38 * 256 * 1us = 9.728ms

; ***********************************************
; * Variable definitions                        *
; ***********************************************
temp        EQU     0x20                    
temp1       EQU     0x21
byte_cnt    EQU     0x22
addr        EQU     0x23
tmp_data    EQU     0x24

; ***********************************************
; * PIC Initialization                          *
; ***********************************************

        org     0x00
        goto    start                   ; Jump around ISR vector
        
; ***********************************************
; * Interrupt service vector initialization     *
; ***********************************************
        org     0x04			
        goto    isr			            ; Point ISR vector to the ISR handler

; ***********************************************
; * Start of Main Code                          *
; ***********************************************
start
        bsf     STATUS,RP0              ; select bank1
        movlw   0x87                    ; Disable internal pullups
                                        ; Interrupt on negative going edge on GP2
                                        ; Prescaler = 1:256

        movwf   OPTION_REG              ; Load the OPTION register
        
        movlw   0x0B                    ; --001011
        movwf   TRISIO                  ; set all ports output except GP3/1/0
         
        bsf     INTCON,GPIE             ; enable GPIO Interrupt on change
        
;        movlw   0x04                    ; GP4&2 = DIO, GP0&1= ADC, Vref=VDD
;        movwf   ADCON1                  ; 

        movlw   0x04                    ; GPIE set - interrupt on pin change
                                        ; GIE cleared - global interrupts disabled
        bcf     STATUS,RP0              ; select bank0
 
                                        

; Initialize the A/D converter
                                    	
        movlw   0x40                    ; AN0 conversion clock =  Fosc/8 (TAD=2us)
        movwf   ADCON0                  ; Turn off A/D module

; Initialize Timer0

        movlw   TMR_COUNT               ; Initialize Timer0
        movwf   TMR0                    ; Timer0 interrupt every 9.728mS

; Set up initial conditions for the SPI 

        movlw   0x24                    ; CS high, INT high, data/clk low
        movwf   GPIO                    ; write to port         
       
        bsf     GPIO,cs_pin             ; set CS pin high
        bcf     GPIO,sck_pin            ; clear the sck pin
        bcf     GPIO,sdo_pin            ; clear the sdo pin
  
        call    mcp2510_init            ; initialize the mcp2510

; *******************************************
; * Main wait loop                          *
; *******************************************

wait                                    ; wait for interrupt to occur
        sleep                           ; sleep while not processing a message
        nop                             ; NOP executed when waking up from sleep
        nop                             ; NOP executed after ISR completes
        goto    wait                    ; go back to sleep and wait


; ***********************************************
; * MCP2510 Initialization                      *
; ***********************************************

mcp2510_init                                    
        movlw   CAN_WRITE               ; write command
        bcf     GPIO,cs_pin             ; lower CS to enable MCP2510
        call    spi_send                ; send comand
        movlw   CANCTRL                 ; select CANCTRL register address
        call    spi_send                ; and send it
        movlw   REQOP_CONFIG            ; Request Config Mode
        call    spi_send                ; send data
        bsf     GPIO,cs_pin             ; raise CS to terminate operation
        bcf     GPIO,sck_pin            ; set clock and
        bcf     GPIO,sdo_pin            ; data pins low

        movlw   0x71                    ; number of addresses to be written
        movwf   byte_cnt                ; load into byte counter 
        movlw   CAN_WRITE               ; write command
        bcf     GPIO,cs_pin             ; enable MCP2510
        call    spi_send                ; send command
        movlw   0x00                    ; start writting at address 0x00
        call    spi_send                ; send address
        movlw   0x01
        movwf   addr
seq_wr                                  ; sequential write loop
        movlw   HIGH reg_init_tbl       ; get high byte of reg_int_tbl address
        movwf   PCLATH                  ; load into high byte of PC counter
        movfw   addr                    ; write into jump table pointer (addr) 
        decf    addr, 1                 ;
        movf    addr, W                 ;
        call    reg_init_tbl            ; fetch byte to be written
        call    spi_send                ; send it to MCP2510
        incf    addr,1                  ; increment the jump table pointer
        incf    addr,1                  ; twice to point to the next byte
        decfsz  byte_cnt,1              ; decrement the byte counter and test for zero
        goto    seq_wr                  ; not done so repeat
        bsf     GPIO,cs_pin             ; raise CS to terminate operation

        movlw   CAN_WRITE               ; write command
        bcf     GPIO,cs_pin             ; enable MCP2510
        call    spi_send                
        movlw   CANCTRL                 ; write to CANCTRL register
        call    spi_send                
        movlw   REQOP_NORMAL            ; Normal Mode
        call    spi_send
        bsf     GPIO,cs_pin             ; terminate operation
        
        movlw   0x00                    ; clear byte_cnt variable
        movwf   byte_cnt  

        bsf     INTCON,GIE              ; Enable Interrupts
        return


       
; *******************************************************************
; * Interrupt Service Routine                                       * 
; * The ISR determines whether a TMR0 interrupt or an external INT  *
; * pin interrupt occurs and then proceeds accordingly              *
; *******************************************************************
isr                                     
        bcf     STATUS,RP1              ; select bank 0/1

        btfss   INTCON,T0IE             ; Timer0 interrupt?
        goto    intpin                  ; No, so jump to external interrupt pin ISR

        movlw   TMR_COUNT               ; reload
        movwf   TMR0                    ; Timer0        

        bcf     ADCON0,CHS0             ; select ADC channel 0
        call    adc_cnv                 ; go do the conversion

        bcf     GPIO,cs_pin             ; enable MCP2510

        movlw   CAN_WRITE               ; send write command to MCP2510
        call    spi_send                ;

        movlw   TXB0D0                  ; set write address to TXB0D0
        call    spi_send                ; 

;        movfw   ADRES                   ; write ADC conversion result
		movlw	0x00
        call    spi_send                ;
        bsf     GPIO,cs_pin             ; terminate SPI operation


        bcf     GPIO,cs_pin             ; enable MCP2510
        movlw   CAN_RTS_TXB0            ; Send RTS command for TXB0
        call    spi_send                
        bsf     GPIO,cs_pin             ; terminate operation

        bcf     INTCON, T0IF            ; clear TMR0 interrupt flag
        return                          ; exit isr

intpin                                  ; Message received interrupt
                            
       movlw    CAN_READ                
       bcf      GPIO,cs_pin             ; lower CS line
       call     spi_send                ; send read command to MCP2510

                                        ; Check for RXB1IF flag by reading
       movlw    CANINTF                 ; the interrupt flag register (CANINTF)
       call     spi_send
       call     spi_rx                  ; read the data from the MCP2510
       bsf      GPIO,cs_pin             ; terminate SPI read

       movwf    tmp_data                ; save CANINTF value
       
       btfsc    tmp_data,1              ; test CANINTF for RX1IF
       call     msg_rcvd                ; if RX1IF set go process message
                                         
       btfss    tmp_data,5              ; test CANINTF for ERRIF 
       call     can_err                 ; if ERRIF set go process CAN error

       movlw    B'11011101'             ; mask off RXB1IF and ERRIF bits
       andwf    tmp_data,1              ; of CANINTF
       btfsc    STATUS,Z                ; if any bit set process invalid interrupt

       call     sys_err                 ; Not an error interrupt so initiate an invalid interrupt
                                        ; occured message.

       bcf      INTCON,GPIF             ; reset interrupt flag
       retfie                           ; return to main routine
        

; *******************************************************************
; * CAN Error routine                                               * 
; * This routine reads the value of the MCP2510 Error flag (EFLG)   *
; * register, writes it to byte 0 of TXB1, and then transmits the   *
; * TXB1 message                                                    * 
; *******************************************************************
can_err
                                        
        movlw   CAN_READ                ; SPI Read operation
        bcf     GPIO,cs_pin             ; enable MCP2510
        call    spi_send                ; 
        movlw   EFLG                    ; EFLG register to be read
        call    spi_send                ;
        call    spi_rx                  ; read the data
        bsf     GPIO,cs_pin             ; terminate SPI operation
        movwf   tmp_data                ; save the value of EFLG register

        movlw   CAN_WRITE               ; now write to MCP2510
        bcf     GPIO,cs_pin             ;
        call    spi_send                ;
        movlw   TXB1D0                  ; write to data byte 0 of TXB1
        call    spi_send                ; 
        movfw   tmp_data                ; write EFLG register contents
        call    spi_send                ;
        bsf     GPIO,cs_pin             ; terminate SPI operation

        movlw   CAN_RTS_TXB1            ; send request to send 
        bcf     GPIO,cs_pin             ; for transmit buffer 1
        call    spi_send
        bsf     GPIO,cs_pin
                                        ; exit isr and re-enable interrupts
        retfie


; *******************************************************************
; * System Error Handler Routine                                    * 
; * This routines transmits the TXB2 message to indicate that a     *
; * unidintifiable system error has occured.                        *
; *******************************************************************
sys_err
        movlw   CAN_RTS_TXB2            ; send request to send
        bcf     GPIO,cs_pin             ; for transmit buffer 2
        call    spi_send                ; when a system error occurs
        bsf     GPIO,cs_pin

        retfie

      
; *******************************************************************
; * CAN Msg Received Routine                                        * 
; * This routine is called when a message has been received into    *
; * TXB0 of the MCP2510.  This routine reads the filter bits to     *
; * determine the type of message received and then initiates the   *
; * appropriate response.                                           *
; *******************************************************************
msg_rcvd
        movlw   CAN_READ                ; SPI read command
        bcf     GPIO,cs_pin             ; enable MCP2510
        call    spi_send                
 
        movlw   RXB0CTRL                ; Read buffer 0 control register
        call    spi_send
        call    spi_rx
        bsf     GPIO,cs_pin             ; terminate function

        andlw   B'00000111'             ; mask off all but the FILHIT bits
        movwf   temp                    ; store value in temp

        movlw   0x01                    ;
        subwf   temp,1
        btfsc   STATUS,Z                ; filter 1 match?
        goto    filter1
        
        movlw   0x02
        subwf   temp,1
        btfsc   STATUS,Z                ; filter 2 match
        goto    filter2

        movlw   0x03
        subwf   temp,1
        btfsc   STATUS,Z                ; filter 3 match
        goto    filter3

        movlw   0x04
        subwf   temp,1
        btfsc   STATUS,Z                ; filter 4 match
        goto    filter4

filter1
        call    wrt_txb0sidh            ; load the transmit buffer SIDH register
        
        bsf     ADCON0,CHS0             ; select ADC channel 1
        call    adc_cnv                 ; go do the conversion

        bcf     GPIO,cs_pin             ; enable MCP2510
        movlw   CAN_WRITE               ; send write command to MCP2510
        call    spi_send                ;
        movlw   TXB0D0                  ; set write address to TXB0D0
        call    spi_send                ;
;        movfw   ADRES                   ; write ADC conversion result
		movlw	0x00
        call    spi_send                ; 
        bsf     GPIO,cs_pin             ; terminate SPI operation

        goto    filter_done

filter2
        call    wrt_txb0sidh            ; load the transmit buffer SIDH register

        bcf     GPIO,cs_pin             ; enable MCP2510
        movlw   CAN_READ                ; send read command to MCP2510
        call    spi_send                ;
        movlw   TXRTSCTRL               ; set read address to TXRTSCTRL
        call    spi_send                ;
        call    spi_rx                  ; read data
        bsf     GPIO,cs_pin

        bcf     GPIO,cs_pin
        movlw   CAN_WRITE               ; write TXTRTSCTRL value
        call    spi_send                ; to data byte zero of 
        movlw   TXB0D0                  ; transmit buffer zero
        call    spi_send                ;
        bsf     GPIO,cs_pin             ; terminate SPI operation

        goto    filter_done

filter3
        call    wrt_txb0sidh            ; load the transmit buffer SIDH register
        
        movlw   CAN_READ                ; Read contents of receive buffer zero
        bcf     GPIO,cs_pin             ; byte zero to get value to write to 
        call    spi_send                ; GP output pin of MCP2510
        movlw   RXB1D0                  ; 
        call    spi_send
        call    spi_rx
        bsf     GPIO,cs_pin
        movwf   tmp_data                ; store value in tmp_data

        movlw   CAN_BIT_MODIFY          ; use bit modify command to 
        bcf     GPIO,cs_pin             ; set/reset the B0BFS bit of BFPCTRL register
        call    spi_send
        movlw   BFPCTRL
        call    spi_send
        movlw   B0BFS
        call    spi_send

        movlw   0xFF                    ; assume that B0BFS is to be set
        btfss   tmp_data,0              ; test the value received in message and if it is 0
        movlw   0x00                    ; load w register to reset bit in BFPCTRL register

        call    spi_send
        bsf     GPIO,cs_pin

        goto    filter_done

filter4
        call    wrt_txb0sidh            ; load the transmit buffer SIDH register

        movlw   CAN_READ                ; Read contents of receive buffer zero       
        bcf     GPIO,cs_pin             ; byte zero to get value to write to         
        call    spi_send                ; GP output pin of MCP2510                   
        movlw   RXB1D0                  ;         
        call    spi_send                                                             
        call    spi_rx                                                               
        bsf     GPIO,cs_pin                                                          
        movwf   tmp_data                ; store value in tmp_data                    
                                                                                     
        movlw   CAN_BIT_MODIFY          ; use bit modify command to                  
        bcf     GPIO,cs_pin             ; set/reset the B0BFS bit of BFPCTRL register
        call    spi_send
        movlw   BFPCTRL
        call    spi_send
        movlw   B1BFS
        call    spi_send

        movlw   0xFF                    ; assume that B1BFS is to be set
        btfss   tmp_data,0              ; test the value received in message and if it is 0
        movlw   0x00                    ; load w register to reset bit in BFPCTRL register

        call    spi_send
        bsf     GPIO,cs_pin

filter_done
        movlw   CAN_RTS_TXB0            ; last step is to send the 
        bcf     GPIO,cs_pin             ; request to send command for 
        call    spi_send                ; transmit buffer zero
        bsf     GPIO,cs_pin
        
        return

; *******************************************************************
; * write TXB0SIDH                                                  * 
; * This routine reads the SIDH register from the received message  *
; * and then sets the SID3 bit and writes the new value to the TX   *
; * buffer.                                                         * 
; *******************************************************************
wrt_txb0sidh
        movlw   CAN_READ                ; SPI read command
        bcf     GPIO,cs_pin             ; enable MCP2510
        call    spi_send
        movlw   RXB0SIDH                ; Read received SIDH register 
        call    spi_send
        call    spi_rx
        bsf     GPIO,cs_pin             ; terminate function

        movwf   tmp_data                    ; store SIDH value in data

        bcf     GPIO,cs_pin
        movlw   CAN_WRITE
        call    spi_send
        movlw   TXB0SIDH                ; write to the SIDH register
        call    spi_send                ; 
        movfw   tmp_data                    ; retreive SIDH value of received message
        bsf     W,0                     ; set bit SID3 high
        call    spi_send                ; 
        bsf     GPIO,cs_pin
        return

        
        
; *******************************************************************
; * Anadlog to Digital Conversion Routine                           * 
; * This routine initiates the A/D conversion.  The ADC channel     *
; * select bits (CHS1:0) have to be set prior to this routine being *
; * called.  The routine waits for the conversion to complete       *
; * before returning to the calling function.                       *
; *******************************************************************
adc_cnv
        bsf     ADCON0,GO
adc_busy
        btfsc   ADCON0,GO_DONE          ; wait for ADC to complete
        goto    adc_busy
        
        movlw   CAN_WRITE               ; SPI write command
        bcf     GPIO,cs_pin             ; lower CS line
        call    spi_send                ; send write command to MCP2510
        movlw   TXB0D0                  ; data being written to data byte zero of buff 0
        call    spi_send                ;
;        movf    ADRES,0                 ; Move ADC value to W register 
		movlw	0
        call    spi_send                ; send to MCP2510
        bsf     GPIO,cs_pin             ; terminate SPI command
        return


; **************************************************
; * Include the custom three wire SPI support file *
; **************************************************

#include "spi.inc"                      ; SPI routines  


; *******************************************************************
; * MCP2510 register initialization table                           * 
; * Store at the end of ROM memory                                  *
; * Note that all addresses are initialized to simplify the         *
; * initialization code.                                            * 
; *******************************************************************
 
        org     0x038e                  ; Initialization table address
reg_init_tbl
        addwf   PCL, 1                  ; Register  Addr
                                        ; --------- ----
        retlw   0xff                    ; RXF0SIDH  0x00
        retlw   0xff                    ; RXF0SIDL  0x01
        retlw   0xff                    ; RXF0EID8  0x02
        retlw   0xff                    ; RXF0EID0  0x03
        retlw   0xff                    ; RXF1SIDH  0x04
        retlw   0xff                    ; RXF1SIDL  0x05
        retlw   0xff                    ; RXF1EID8  0x06
        retlw   0xff                    ; RXF1EID0  0x07
        retlw   0x7e                    ; RXF2SIDH  0x08    Filter 2 matches 0x3f0
        retlw   0x00                    ; RXF2SIDL  0x09
        retlw   0xff                    ; RXF2EID8  0x0A
        retlw   0xff                    ; RXF2EID0  0x0B
        retlw   0x3c                    ; BFPCTRL   0x0C    BFP pins as digital outputs, initial state hi
        retlw   0x00                    ; TXRTSCTRL 0x0D    TXRTS pins as digital inputs
        retlw   0x80                    ; CANSTAT   0x0E
        retlw   0x80                    ; CANCTRL   0x0F
        
        retlw   0x7e                    ; RXF3SIDH  0x10    Filter 3 matches 0x3f1
        retlw   0x20                    ; RXF3SIDL  0x11
        retlw   0xff                    ; RXF3EID8  0x12
        retlw   0xff                    ; RXF3EID0  0x13
        retlw   0x7e                    ; RXF4SIDH  0x14    Filter 4 matches 0x3f2
        retlw   0x40                    ; RXF4SIDL  0x15
        retlw   0xff                    ; RXF4EID8  0x16
        retlw   0xff                    ; RXF4EID0  0x17
        retlw   0x7e                    ; RXF5SIDH  0x18    Filter 5 matches 0x3f3
        retlw   0x50                    ; RXF5SIDL  0x19
        retlw   0xff                    ; RXF5EID8  0x1A
        retlw   0xff                    ; RXF5EID0  0x1B
        retlw   0x00                    ; TEC       0x1C
        retlw   0x00                    ; REC       0x1D
        retlw   0x80                    ; CANSTAT   0x1E
        retlw   0x80                    ; CANCTRL   0x1F
        
        retlw   0xff                    ; RXM0SIDH  0x20    Enable all mask bits so that no msg's
        retlw   0xff                    ; RXM0SIDL  0x21    are received into RXB0 
        retlw   0xff                    ; RXM0EID8  0x22
        retlw   0xff                    ; RXM0EID0  0x23
        retlw   0x7e                    ; RXM1SIDH  0x24    Set RXM1 to match msg ID's of 0x3f0 to 0x3ff
        retlw   0x00                    ; RXM1SIDL  0x25
        retlw   0x00                    ; RXM1EID8  0x26
        retlw   0x00                    ; RXM1EID0  0x27
        retlw   0x02                    ; CNF3      0x28    PHSEG2 = 3TQ
        retlw   0x90                    ; CNF2      0x29    PHSEG1 = 3TQ, PRSEG = 1TQ 
        retlw   0x03                    ; CNF1      0x2A    SJW = 1TQ, BRP set to 4
        retlw   0x22                    ; CANINTE   0x2B    MERRIE and RX1IE enabled
        retlw   0x00                    ; CANINTF   0x2C
        retlw   0x00                    ; EFLG      0x2D
        retlw   0x80                    ; CANSTAT   0x2E
        retlw   0x80                    ; CANCTRL   0x2F
        
        retlw   0x03                    ; TXB0CTRL  0x30    Highest priority
        retlw   0x7e                    ; TXB0SIDH  0x31
        retlw   0x00                    ; TXB0SIDL  0x32
        retlw   0x00                    ; TXB0EID8  0x33
        retlw   0x00                    ; TXB0EID0  0x34
        retlw   0x01                    ; TXB0DLC   0x35
        retlw   0x00                    ; TXB0DB0   0x36
        retlw   0x00                    ; TXB0DB1   0x37
        retlw   0x00                    ; TXB0DB2   0x38
        retlw   0x00                    ; TXB0DB3   0x39
        retlw   0x00                    ; TXB0DB4   0x3A
        retlw   0x00                    ; TXB0DB5   0x3B
        retlw   0x00                    ; TXB0DB6   0x3C
        retlw   0x00                    ; TXB0DB7   0x3D
        retlw   0x80                    ; CANSTAT   0x3E
        retlw   0x80                    ; CANCTRL   0x3F
           
        retlw   0x03                    ; TXB1CTRL  0x40    Highest priority
        retlw   0x7e                    ; TXB1SIDH  0x41
        retlw   0xe0                    ; TXB1SIDL  0x42
        retlw   0x00                    ; TXB1EID8  0x43
        retlw   0x00                    ; TXB1EID0  0x44
        retlw   0x01                    ; TXB1DLC   0x45
        retlw   0x00                    ; TXB1DB0   0x46
        retlw   0x00                    ; TXB1DB1   0x47
        retlw   0x00                    ; TXB1DB2   0x48
        retlw   0x00                    ; TXB1DB3   0x49
        retlw   0x00                    ; TXB1DB4   0x4A
        retlw   0x00                    ; TXB1DB5   0x4B
        retlw   0x00                    ; TXB1DB6   0x4C
        retlw   0x00                    ; TXB1DB7   0x4D
        retlw   0x80                    ; CANSTAT   0x4E
        retlw   0x80                    ; CANCTRL   0x4F
        
        retlw   0x03                    ; TXB2CTRL  0x50
        retlw   0x7e                    ; TXB2SIDH  0x51
        retlw   0xe0                    ; TXB2SIDL  0x52
        retlw   0x00                    ; TXB2EID8  0x53
        retlw   0x00                    ; TXB2EID0  0x54
        retlw   0x00                    ; TXB2DLC   0x55
        retlw   0x00                    ; TXB2DB0   0x56
        retlw   0x00                    ; TXB2DB1   0x57
        retlw   0x00                    ; TXB2DB2   0x58
        retlw   0x00                    ; TXB2DB3   0x59
        retlw   0x00                    ; TXB2DB4   0x5A
        retlw   0x00                    ; TXB2DB5   0x5B
        retlw   0x00                    ; TXB2DB6   0x5C
        retlw   0x00                    ; TXB2DB7   0x5D
        retlw   0x80                    ; CANSTAT   0x5E
        retlw   0x80                    ; CANCTRL   0x5F
        
        retlw   0x20                    ; RXB0CTRL  0x60    Receive only Standard Id's that match Masks/Filters
        retlw   0x00                    ; RXB0SIDH  0x61
        retlw   0x00                    ; RXB0SIDL  0x62
        retlw   0x00                    ; RXB0EID8  0x63
        retlw   0x00                    ; RXB0EID0  0x64
        retlw   0x00                    ; RXB0DLC   0x65
        retlw   0x00                    ; RXB0DB0   0x66
        retlw   0x00                    ; RXB0DB1   0x67
        retlw   0x00                    ; RXB0DB2   0x68
        retlw   0x00                    ; RXB0DB3   0x69
        retlw   0x00                    ; RXB0DB4   0x6A
        retlw   0x00                    ; RXB0DB5   0x6B
        retlw   0x00                    ; RXB0DB6   0x6C
        retlw   0x00                    ; RXB0DB7   0x6D
        retlw   0x80                    ; CANSTAT   0x6E
        retlw   0x80                    ; CANCTRL   0x6F
        
        retlw   0x20                    ; RXB1CTRL  0x70    Receive only Standard Id's that match Masks/Filters
        END                             
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
