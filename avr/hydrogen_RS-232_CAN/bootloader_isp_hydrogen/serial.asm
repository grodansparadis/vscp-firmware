;///////////////////////////////////////////////////////////////////////////////
;// sendchar
;//
;// Transmitt character in R16
;//

;#include "defines.h"

sendchar:

        OUT     UDR0,R16

wait_until_sent:

        SBIS    UCSR0A,TXC0		
        RJMP    wait_until_sent

        SBI     UCSR0A,TXC0		; clr TXCflag vy writing a one (yes!) 

        RET     

;///////////////////////////////////////////////////////////////////////////////
;// getchar
;//
;// Returns character in R16
;//

getchar:

        IN      R16,UCSR0A			; Wait for data to arrive
		ANDI	R16,(1<<RXC0)
        BREQ    getchar

        IN      R16,UDR0			; Get data byte

        RET     

