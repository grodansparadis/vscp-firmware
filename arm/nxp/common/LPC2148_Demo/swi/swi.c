//
//  $Id: swi.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/swi/swi.c $
//

#include "FreeRTOS.h"

#include <stdio.h>

#include "swi.h"

//
//
//
#define LED2 GPIO_IO_P11

//
//
//
void swiInit (void)
{
}

int swiDispatchC (unsigned long r0, unsigned long r1 __attribute__ ((unused)), unsigned long r2 __attribute__ ((unused)), unsigned long swi)
{
  int state = 0;

  switch (swi)
  {
    case SWICALL_YIELDPROCESSOR :
    case SWICALL_A_LED2SET :
    case SWICALL_A_LED2ON :
    case SWICALL_A_LED2OFF :
    case SWICALL_A_LED2TOGGLE :
      printf ("Eeek!  This should have been handled in the asm code\n");
      break;

    case SWICALL_C_LED2SET :
      {
        state = GPIO0_FIOPIN & LED2;

        if (r0)
          GPIO0_FIOSET = LED2;			
        else
          GPIO0_FIOCLR = LED2;
      }
      break;

    case SWICALL_C_LED2ON :
      {
        state = GPIO0_FIOPIN & LED2;
        GPIO0_FIOCLR = LED2;			
      }
      break;

    case SWICALL_C_LED2OFF :
      {
        state = GPIO0_FIOPIN & LED2;
        GPIO0_FIOSET = LED2;			
      }
      break;

    case SWICALL_C_LED2TOGGLE :
      {
        if ((state = GPIO0_FIOPIN & LED2))
          GPIO0_FIOCLR = LED2;
        else
          GPIO0_FIOSET = LED2;			
      }
      break;

    default :
      printf ("Eeek! Unhandled SWI value\n");
      break;
  }

  return state;
}
