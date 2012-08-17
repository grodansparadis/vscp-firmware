
write_page:

        RCALL   WAIT_SPMEN	; Wait for SPMEN flag cleared        
        MOVW    R30,R16    	; move adress to z pointer (R31=ZH R30=ZL)
        MOV	 	R17,R20
        RJMP	SPMSTUFF

fill_temp_buffer:

        RCALL   WAIT_SPMEN  ; Wait for SPMEN flag cleared
  
        MOVW    R30,R16     ; move adress to z pointer (R31=ZH R30=ZL)     	     
        MOVW    R0,R20		; move data to reg 0 and 1 
 
        LDI     R17,(1<<SPMEN)

SPMSTUFF:
 
        OUT     SPMCR,R17   ; argument 2 decides function
        SPM              	; Store program memory
        .DW     0xFFFF     	; For future part compatibility, instruction alignment
        NOP
 
WAIT_SPMEN:
        
        RET

read_program_memory:

        RCALL   WAIT_SPMEN
   
        MOVW    R30,R16    	; R31=ZH R30=ZL, move adress to z pointer
        SBRC    R20,0     	; read lockbits? (second argument=0x09)
                            ; if so, place second argument in SPMEN register

        OUT     SPMCR,R20   ; argument 2 decides function
       
        LPM     R16,Z+              
        LPM     R17,Z+

        RET

write_lock_bits:

        RCALL   WAIT_SPMEN  ; Wait for SPMEN flag cleared

        MOV     R0,R16   
        LDI     R17,( ( 1 << BLBSET ) | ( 1 << SPMEN ) )
		RJMP 	SPMSTUFF
	
