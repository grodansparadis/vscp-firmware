/********************************************************************
*   Delayfunctions, compatible with Microchip MCC18 delayroutines   *
*                                                                   *
*   WARNING: A few of these functions are less accurate when        *
*            QuickCall is disabled.                                 *
********************************************************************/

#ifdef _Delay10TCYx
/********************************************************************
*   Function Name: Delay10TCYx                                      *
*   Return Value:  void                                             *
*   Parameters:    unit:                                            *
*   Description:   This routine delays for multiples of 10          *
*                  instruction cycles that is specified in unit.    *
*   Special Note:  This routine provides multiples of 10 Tcy.       *
*                  A value of 1 to 255 provides 10 to 2550   Tcy    *
*                  delay.  A value of 0 will provide a delay of     *
*                  2560 Tcy.                                        *
********************************************************************/
#if (!defined(QUICKCALL)) || (QUICKCALL == 0)
	#warning With QuickCall disabled, Delay10TCYx is less accurate
#endif
#pragma asm
module "_Delay10TCYx"
		
Delay10TCYx::
	if _QUICKCALL==1
		movlw	1
	else
		movlw	3
	endif
	movf 	PLUSW2,W
	dcfsnz	WREG,1
	MRET	0
D10x
	nop
	bra		$+2
	bra		$+2
	bra		$+2
D10_1
	decfsz	WREG,F
	bra		D10x
	MRET	0

endmodule
#pragma asmend
#endif		/* _Delay10TCYx */


#ifdef _Delay100TCYx
/********************************************************************
*      Function Name:  Delay100TCYx                                 *
*      Return Value:   void                                         *
*      Parameters:     unit:                                        *
*      Description:    This routine delays for multiples of 100     *
*                      instruction cycles that is specified in unit.*
*      Special Note:   This routine provides multiples of 100 Tcy.  *
*                      A value of 1 to 255 provides 100 to 25500 Tcy*
*                      delay.  A value of 0 will provide a delay of *
*                      25600 Tcy.                                   *
********************************************************************/
#pragma asm
module "_Delay100TCYx"
	
Delay100TCYx::
	if _QUICKCALL==1
		movlw	1
	else
		movlw	3
	endif
	movf	PLUSW2,W
	movwf	ACC+0
	if _QUICKCALL==1
		movlw	0x1B
	else
		movlw	0x18
	endif
	bra		$+2
	bra		D100_1
D100x
	movlw	0x20
D100_1
	movwf	ACC+1
	decfsz	ACC+1,F
	bra		$-2
	decfsz	ACC+0,F
	bra		D100x
	bra		$+2
	MRET	0
endmodule
#pragma asmend
#endif		/* _Delay100TCYx */


#ifdef _Delay1KTCYx
/********************************************************************
*   Function Name: Delay1KTCYx                                      *
*   Return Value:  void                                             *
*   Parameters:    unit:                                            *
*   Description:   This routine delays for multiples of 1000        *
*                  instruction cycles that is specified in unit.    *
*   Special Note:  This routine provides multiples of 1000 Tcy      *
*                  A value of 1 to 255 to provides 1000 to          *
*                  255000 Tcy delay.  A value of 0 will provide     *
*                  a delay of 256000 Tcy.                           *
********************************************************************/
#pragma asm
	module "_Delay1KTCYx"
		
Delay1KTCYx::
	if _QUICKCALL==1
		movlw	1
	else
		movlw	3
	endif
	movf	PLUSW2,W
	movwf	ACC+0
	if _QUICKCALL==1
		movlw	0x48
	else
		movlw	0x45
	endif
	bra     D1K_1
D1Kx
	movlw   0x4C
D1K_1
	movwf   ACC+1
	decfsz  ACC+1,F
	bra     $-2
	clrf    ACC+1
	decfsz  ACC+1,F
	bra     $-2
	decfsz  ACC+0,F
	bra     D1Kx
	nop
	MRET	0
endmodule
#pragma asmend
#endif		/* _Delay1KTCYx */


#ifdef _Delay10KTCYx
/********************************************************************
*   Function Name: Delay10KTCYx                                     *
*   Return Value:  void                                             *
*   Parameters:    unit:                                            *
*   Description:   This routine delays for multiples of 10000       *
*                  instruction cycles that is specified in unit.    *
*   Special Note:  This routine provides multiples of 10000 Tcy     *
*                  A value of 1 to 255 to provides 10000 to         *
*                  2550000 Tcy delay.  A value of 0 will            *
*                  provide a delay of 2560000 Tcy.                  *
********************************************************************/
#pragma asm
	module "_Delay10KTCYx"
		
Delay10KTCYx::
	if _QUICKCALL==1
		movlw	1
	else
		movlw	3
	endif
	movf	PLUSW2,W
	movwf	ACC+0
	if _QUICKCALL==1
		movlw	0xEF
	else
		movlw	0xEC
	endif
	bra     D10K_1
D10Kx
	movlw   0xF3
D10K_1 
	movwf   ACC+1
	decfsz  ACC+1,F
	goto    $-2
	movlw   0x0C
	movwf   ACC2+0
D10Kl
	clrf    ACC+1
	decfsz  ACC+1,F
	goto    $-2
	decfsz  ACC2+0,F
	goto    D10Kl
	decfsz  ACC+0,F
	goto    D10Kx
	MRET	0
endmodule
#pragma asmend
#endif		/* _Delay10KTCYx */
