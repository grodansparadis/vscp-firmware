; ***********************************************
; * spi.asm                                     *
; * Revision 1.0   May 2000                     *
; * Developed by Rick Stoneking                 *
; * Developed using MPLAB V4.12 and MPASM V2.3  *
; *                                             *
; * This code implements SPI communications for *
; * the Microchip PIC12C672 8 pin micro         *
; * using a three wire interface                *
; *                                             *
; ***********************************************
 
; ***********************************************
; * Define I/O pin assignments                  *
; ***********************************************
cs_pin          EQU     5
sdi_pin         EQU     4
sdo_pin         EQU     4
int_pin         EQU     3
sck_pin         EQU     2

; ***********************************************
; * Variable definitions                        *
; ***********************************************


; ***********************************************
; * SPI_SEND                                    *
; * This routine sends a single byte out.  The  *
; * byte to be transmitted must be loaded into  *
; * the w register prior to calling this        *
; * routine.                                    *
; * The routine is designed to allow            *
; * sending multiple bytes by using consecutive *
; * calls, therefore it is necessary for the    *
; * calling routine to handle the CS line       *
; * before calling the routine for the first    *
; * time, and after sending the last byte.      *
; ***********************************************
spi_send                                ; w reg must have data byte to be sent in it
        movwf   temp                    ; save data byte
        bsf     STATUS,RP0              ; select bank1
        movlw   0x08                    ; set GP4 to output
        movwf   TRISIO
        bcf     STATUS,RP0              ; select bank0
        movlw   8
        movwf   temp1                   ; bit counter

write_loop        
        rlf     temp,1                  ; move bit into carry flag
        bcf     GPIO,sck_pin            ; set sck low        
        btfsc   STATUS,C                ; test and if clear skip next instruction
        goto    send_hi 
        bcf     GPIO,sdo_pin
        goto    wr_clock
send_hi
        bsf     GPIO,sdo_pin
wr_clock        
        bsf     GPIO,sck_pin            ; set sck high
        decfsz  temp1,1         
        goto    write_loop
        bcf     GPIO,sdo_pin            ; exit routine with SDO low
        retlw   0
    
; ***********************************************
; * SPI_RX                                      *
; * This routine receives a single byte.   The  *
; * received byte is in the w register upon     *
; * returning to the calling function.          *
; * The routine is designed to allow receiving  *
; * multiple bytes by using consecutive         *
; * calls, therefore it is necessary for the    *
; * calling routine to handle the CS line       *
; * before calling the routine for the first    *
; * time, and after receiving the last byte.    *
; ***********************************************
spi_rx
        bsf     STATUS,RP0              ; select bank1
        movlw   0x18                    ; set GP4 to input
        movwf   TRISIO
        bcf     STATUS,RP0              ; select bank0
        movlw   0
        movwf   temp
        movlw   8                       ; set bit count
        movwf   temp1
rd_loop        
        bcf     GPIO,sck_pin
        rlf     temp,1                  ; rotate is done here so that we do
                                        ;  not rotate after bit counter = 0
        btfsc   GPIO,sdi_pin            ; check input pin
        goto    bit_high                ; 
        bcf     temp,0
        goto    shift
bit_high
        bsf     temp,0
shift   
        bsf     GPIO,sck_pin            ; raise clock
        decfsz  temp1,1
        goto    rd_loop
        retlw   temp        


        
