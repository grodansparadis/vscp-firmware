***************************************************************************
; 
; File Name             :'DS1820.asm"
; Title                 :
; Date                  :
; Version               :
; Support telephone     :765 287 1987  David B. VanHorn
; Support fax           :765 287 1989
; Support Email         :dvanhorn@cedar.net
; Target MCU            :AT90S8515
;
; DESCRIPTION
;
; DEFINITIONS
;
;       TEMP_BUF        Temp sensor bytes end up in here
;
; 
;***************************************************************************;
;       M O D I F I C A T I O N   H I S T O R Y 
;
;
;       rev.      date    who   why
;       ----    --------  ---   ------------------------------------------
;       0.01    98.09.28  dvh   Creation  A basic handler for the DS1820.
;
;******************************************************************************
;
;Uses a 9 byte buffer at TEMP_BUF, and a byte flag "TEMP_FLAG"
;
;Ram Data:
;Byte   0       Temp LSB        First byte transmitted
;       1       Temp MSB	
;       2       TH/User 1
;       3       TH/User 2
;       4       Res
;       5       Res
;       6       Count remain
;       7       Count per Deg C.
;       8       CRC             Last byte transmitted
;

;
;Gets all 9 bytes of temp
;
;Flag:  $FF Conversion started
;       $01 Conversion Done
;       $00 Not in use
;
Start_BAT_TEMP:
        lds     TEMP,TEMP_FLAG  ;Already converting?
        and     TEMP,TEMP       ;
        brne    Start_Bat_Temp_Done;Yes, just exit

        rcall   Reset_1820      ;Reset the chip
        rcall   Skip_Rom        ;Skip rom ID
        rcall   Convert_T       ;Start temp conversion
        ldi     TEMP,$FF        ;
        sts     TEMP_FLAG,TEMP  ;Flag conversion started
Start_Bat_Temp_Done:
        ret

Poll_Bat_Temp:
        lds     TEMP,TEMP_FLAG  ;
        inc     TEMP            ;
        brne    Poll_Bat_Temp_Done
        rcall   Poll_Temp       ;Poll till not busy
        ;Sets flag to $01 if temp convert is done
Poll_Bat_Temp_Done:
        ret

Get_Bat_Temp:
        lds     TEMP,TEMP_FLAG  ;
        dec     TEMP            ;
        brne    Get_Bat_Temp_Done

        rcall   Reset_1820      ;Reset the chip again
        rcall   Skip_Rom        ;Skip rom ID
        rcall   Read_Scratchpad ;read the data into TEMP_BUF
        rcall   Reset_1820      ;Reset it again
        ldi     TEMP,$00        ;
        sts     TEMP_FLAG,TEMP  ;Signal done
Get_Bat_Temp_Done:
        ret                     ;

;
;Gets 8 bytes from rom, device ID, serno and CRC
GET_SERNO:
        rcall   Reset_1820      ;Reset the chip
        rcall   Read_Rom        ;Get rom data into Temp_Buf
        rcall   Reset_1820      ;Reset it again
        ret

;
;This routine verifies the presence of a Dallas 1820 chip
;It can't be fooled by hung hi or hung lo lines.
;
RESET_1820:
        push    TEMP            ;Save it
        cbi     PORTA,7         ;Go tri-state
        sbi     DDRA,7          ;Make this pin drive low

;
;
;This should be a 1Ms Delay
;
        ldi     TEMP,$0C        ;
RST_A:  ldi     TEMP2,220       ;Calculated for 8 mhz xtal
        mov     LOOP,TEMP2
RST_B:  dec     LOOP            ;1 FF*2+
        brne    RST_B           ;1 0F*3
        dec     TEMP            ;1
        brne    RST_A           ;1

        ;At this point, at least 1mS has passed, maybe up to 2ms.
        cli                     ;***Shut off ints
        sbi     PORTA,7         ;Drive high
        cbi     DDRA,7          ;then go O/C again

        ;At this point, the I/O goes high, for a min of 15 and max of 60uS

        ldi     TEMP,5          ;5*3=15!
RST_BB: dec     TEMP            ;
        rcall   Three_uS        ;
        brne    RST_BB          ;

        in      TEMP,PINA       ;Check that the temp sensor
        andi    TEMP,$80        ;output actually went high
        breq    RST1820_BAD     ;

        ;Then goes low for a min of 60 and a max of 240uS
        ;Sampling must then occur not sooner 61uS or later than 75

RST1820_C:
        ldi     TEMP,20         ;20*3=60
RST_C:  dec     temp            ;
        rcall   Three_uS        ;
        brne    RST_C           ;

        ;Make sure that the device is present by sensing a low state 
        ;at this point in the transaction
        in      TEMP,PINA       ;
        andi    TEMP,$80        ;
        brne    RST1820_BAD     ;If it's high at this point, there's no sensor

        SEI     ;***Restore ints!

        ldi     TEMP2,10        ;
        mov     LOOP,TEMP2      ;
RST_D:  dec     LOOP            ;
        rcall   Three_uS        ;
        breq    RST1820_BAD     ;If it never goes high, it's still cat-food

        ldi     TEMP,7          ;Wait before checking
RST_DA: dec     TEMP            ;
        rcall   Three_uS        ;
        brne    RST_DA          ;
        in      TEMP,PINA       ;
        andi    TEMP,$80        ;
        breq    RST_D           ;

RST1820_OK:
        ldi     TEMP,$00        ;
        sts     TEMP_STAT,TEMP  ;Signal that the sensor is OK	
        rjmp    RST_1820_DONE

RST1820_BAD:
        sei                     ;JIC
        ;rcall  Bad_PING        ;
        ldi     TEMP,$FF        ;
        sts     TEMP_STAT,TEMP  ;Show what part of the conversation we are objecting to

RST_1820_DONE:
        pop     TEMP            ;
        ret
;

;
;Gets serno, Family code, CRC (Only one device on bus!)
;*** Working!
Read_Rom:
        push    TEMP
        ldi     TEMP,$33
        mov     TEMP2,TEMP
        rcall   Write_Byte

        rcall   read_byte               ;Get Family code
        sts     (SERNO_BUF+0),TEMP2
        rcall   read_byte               ;Get Serno lsb
        sts     (SERNO_BUF+1),TEMP2
        rcall   read_byte               ;Get Serno
        sts     (SERNO_BUF+2),TEMP2
        rcall   read_byte               ;Get Serno
        sts     (SERNO_BUF+3),TEMP2
        rcall   read_byte               ;Get Serno
        sts     (SERNO_BUF+4),TEMP2
        rcall   read_byte               ;Get Serno
        sts     (SERNO_BUF+5),TEMP2
        rcall   read_byte               ;Get Serno MSB
        sts     (SERNO_BUF+6),TEMP2
        rcall   read_byte               ;Get CRC
        sts     (SERNO_BUF+7),TEMP2
        pop     TEMP
        ret
;
;Address a single device (takes a 64 bit ROM number)
;
Match_Rom:
        push    TEMP

        pop     TEMP
        ret
;
;Read rom W/O serno, single device only
;
Skip_Rom:
        ldi     TEMP,$CC
        mov     TEMP2,TEMP
        rcall   Write_Byte
        ret

;
;Scan for devices
;
Search_Rom:
        push    TEMP
        ;Not implemented
        pop     TEMP
        ret
;
;Same as search rom, but they only respond if the temp
;has been exceeded (internal alarms)
;
Alarm_Search:
        push    TEMP
        ;Not implemented
        pop     TEMP
        ret

;Starts the conversion process
;May take up to .5 sec
;Polling the device returns a zero until it is ready
;
Convert_T:
        ;Send $44 as protocol
        ldi     TEMP,$44
        mov     TEMP2,TEMP
        rcall   Write_Byte
        ret


Poll_Temp:
        clr     TEMP2
        rcall   READ_BIT        ;
        and     TEMP2,TEMP2     ;Is there a one?
        breq    POLL_TEMP_Done  ;No, leave it alone
        ldi     TEMP,$01        ;
        mov     TEMP2,TEMP      ;
        sts     TEMP_FLAG,TEMP2 ;Flag conversion done
Poll_Temp_Done:
        ret


Read_Scratchpad:
        ;Send $BE as protocol
        ldi     TEMP,$BE
        mov     TEMP2,TEMP
        rcall   Write_Byte

        rcall   read_byte
        sts     (TEMP_BUF+0),TEMP2      ;Always pull in all the data!
        rcall   read_byte
        sts     (TEMP_BUF+1),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+2),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+3),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+4),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+5),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+6),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+7),TEMP2
        rcall   read_byte
        sts     (TEMP_BUF+8),TEMP2

        ret

;
;Assumes byte in TEMP2
;
Write_Byte:
        push    LOOP
        ldi     TEMP,8
        mov     LOOP,TEMP

WB_A:   ror     TEMP2   ;LSB to carry
        brcs    W_one

W_Zero: rcall   Write_0_Bit
        rjmp    W_Done

W_One:  rcall   Write_1_Bit
        rjmp    W_Done

W_Done:	
        dec     LOOP
        brne    WB_A

        pop     LOOP
        ret
;
;Get a byte from the temp sensor
;
Read_Byte:
        push    LOOP            ;
        ldi     TEMP2,8         ;
        mov     LOOP,TEMP2
R_B_A:  rcall   Read_Bit        ;
        dec     Loop            ;
        brne    R_B_A           ;
        pop     loop            ;
        ret

;
;Get a single bit, as fast as possible
;1/15/98 updated for true O/C operation
;
Read_BIT:
        cbi     PORTA,7         ;Tri-state
        sbi     DDRA,7          ;Make it low
        rcall   Three_uS        ;Wait
        sbi     PORTA,7         ;Set Output High
        cbi     DDRA,7          ;Back to O/C

        ldi     TEMP,4          ;15uS timewaster
RB_A:   dec     TEMP            ;
        rcall   Three_uS        ;
        brne    RB_A            ;

        in      TEMP,PINA       ;Read the bit
        rol     TEMP            ;Put it in the Carry
        ror     TEMP2           ;Store it in TEMP2(Register)


RB_B:
        rcall   Three_uS	
        in      TEMP,PINA       ;
        andi    TEMP,$80        ;
        breq    RB_B            ;Wait till it goes high
        ret

WRITE_0_BIT:
        cbi     PORTA,7         ;Tri-state
        sbi     DDRA,7          ;Force low
        rcall   Seventy_uS      ;
        sbi     PORTA,7         ;Force high
        cbi     DDRA,7          ;O/C high
        rcall   Three_uS        ;
        ret

WRITE_1_BIT:
        cbi     PORTA,7         ;
        sbi     DDRA,7          ;
        rcall   Three_uS        ;
        sbi     PORTA,7         ;
        cbi     DDRA,7          ;
        rcall   Seventy_uS      ;
        RET                     ;


;
;Hand-tweaked time delays, Check using PortB,7
;
;
;
Seventy_uS:
        push    LOOP            ;
        ldi     TEMP2,16        ;16 call and returns takes 70uS (Go figure)
        mov     LOOP,TEMP2      ;

WR_1_L: dec     loop            ;
        rcall   Three_uS        ;
        brne    WR_1_L          ;
        pop     Loop            ;
        ret

Three_US:
        nop                     ;24 nops @ 8 Mhz, + call and return overhead,
        nop                     ;=3uS
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        nop                     ;
        ret
	
