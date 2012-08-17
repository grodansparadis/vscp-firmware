NAME delay

#include <io8535.h>

;
; 3.6964 MHZ
; ==========
; 9600 baud,  384 cycles, b = 60
; 19200 baud, 192 cycles, b = 28
; 57600 baud, 64 cycles, b = 7
; 115200 baud, 32 cycles, b = 2   
;

#define	  b	7	 

#define	SoftTxD	4		        ;Transmit pin is PC4

#define	Txbyte	R16			;Data to be transmitted
#define	temp	R17			;temporary storage register
#define	bitcnt  R18			;bit counter

#define sb    1                         ; stopbits


PUBLIC putchar
PUBLIC DelayMs

RSEG CODE
    
;***************************************************************************
;* DelayMs
;* =======
;* (akhe@users.sourceforge.net)
;* 
;* *** Note: Uses R16 and R17
;* Delay for approx. one ms on a machine with a clock freq. of 3.6864 MHz
;* 11+5(n-1)+(n-1)subdelay
;*
;*

DelayMs:          ldi     R16,20            ; 1 - 
DelayBitMs_Loop:  rcall   _delay            ; 3
                  dec	  R16               ; 1
                  brne	  DelayBitMs_Loop   ; 1/2 ( false/true)
                  ret                       ; 4

; SubDelay = 1 + ((R17 *3) - 1) + 4 this is (ldi + loop + ret)
_delay:           ldi     R17,60        ; 1         
_delayloop:	  dec	  R17           ; 1
		  brne	  _delayloop    ; 1/2 (false/true)
		
		  ret                   ; 4 cycles
		
;***************************************************************************
;* UART_delay
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "b":
;*
;*	3·b + 7 cycles (including rcall and ret)
;*
;* Number of words	:4 including return
;* Low registers used	:None
;* High registers used	:1 (temp)
;* Pointers used	:None
;*
;***************************************************************************
; Some b values: 	(See also table in Appnote documentation)
;
; 1 MHz crystal:
;   9600 bps - b=14
;  19200 bps - b=5
;  28800 bps - b=2
;
; 2 MHz crystal:
;  19200 bps - b=14
;  28800 bps - b=8
;  57600 bps - b=2

; 4 MHz crystal:
;  19200 bps - b=31
;  28800 bps - b=19
;  57600 bps - b=8
; 115200 bps - b=2

UART_delay:	  ldi	R17,b
UART_delay_loop:  dec	temp
		  brne	UART_delay_loop

		  ret		

;***************************************************************************
;* "putchar"
;*
;* This subroutine transmits the byte stored in the "Txbyte" register
;* The number of stop bits used is set with the sb constant
;*
;* Number of words	:14 including return
;* Number of cycles	:Depens on bit rate
;* Low registers used	:None
;* High registers used	:2 (bitcnt,Txbyte)
;* Pointers used	:None
;* 
;*
		
putchar:	//cli
                ldi	R18,9+sb	;1+8+sb (sb is # of stop bits)
		com	Txbyte		;Invert everything
		sec			;Start bit

putchar0:	brcc	putchar1	;If carry set
		cbi	PORTC,SoftTxD	;    send a '0'
		rjmp	putchar2	;else	

putchar1:	sbi	PORTC,SoftTxD	;    send a '1'
		nop

putchar2:	rcall UART_delay	;One bit delay
		rcall UART_delay

		lsr	Txbyte		;Get next bit
		dec	bitcnt		;If not all bit sent
		brne	putchar0	;   send next
					;else
					
		//sei						
		ret			;   return

END
