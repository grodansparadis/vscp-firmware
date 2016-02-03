/********************************************************************
*   Delayfunctions, compatible with Microchip MCC18 delayroutines   *
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
	#error Delay10TCYx is not implemented for this line of PIC's
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
	#error Delay100TCYx is not implemented for this line of PIC's
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
	#error Delay1KTCYx is not implemented for this line of PIC's
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
	#error Delay10KTCYx is not implemented for this line of PIC's
#endif		/* _Delay10KTCYx */
