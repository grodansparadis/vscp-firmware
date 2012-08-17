/***************************************************************************
   Copyright © Intel Corporation, March 18th 1998.  All rights reserved.
   Copyright ARM Limited 1998 - 2000.  All rights reserved.
****************************************************************************

  led.c

	$Id: led.c,v 1.1 2005/12/19 11:45:57 akhe Exp $

   This file contains all the drivers etc. to interface with the LEDs on
   a ARM target. The aim of these routines is to provide a standard
   mechanism to access the LED(s) hiding any target-specific code.

****************************************************************************/

#include	"uhal.h" 
 

void		*uHALiv_LedHomes[] = uHAL_LED_OFFSETS ;
unsigned int	 uHALiv_LedMasks[] = uHAL_LED_MASKS ;


/* Get the state of the specified LED */
int uHALr_ReadLED(unsigned int number)
{
unsigned int	*address ;
unsigned int	 mask ;

    if (number == 0 || number > uHAL_NUM_OF_LEDS)
	return -1;
	
    address = (unsigned int *)uHALiv_LedHomes[number] ;
    mask = uHALiv_LedMasks[number] ;

    /* If bit is set, return TRUE if uHAL_LED_ON has a bit set too.
     * For clear bit, return FALSE if uHAL_LED_OFF is also clear.
     */
    if (*address & mask)
	return ((uHAL_LED_ON)  ? TRUE : FALSE);
    else
	return ((uHAL_LED_OFF) ? TRUE : FALSE);
}

/* Turn on the specified LED */
void uHALr_SetLED(unsigned int number)
{
    uHALr_WriteLED(number, TRUE) ;
}


/* Turn on the specified LED */
void uHALr_ResetLED(unsigned int number)
{
    uHALr_WriteLED(number, FALSE) ;
}


/* Report to system how many LEDs this host has */
unsigned int uHALr_CountLEDs(void)
{
    return (uHAL_NUM_OF_LEDS) ;
}


/* Routine to set all LEDs to OFF - handy on reset/power-up */
unsigned int uHALr_InitLEDs(void)
{
int	i ;

    for (i = 1; i <= uHAL_NUM_OF_LEDS; i++)
	uHALr_WriteLED(i, FALSE) ;

    return (uHAL_NUM_OF_LEDS) ;
}

// End of file - led.c

