//
//  $Id: eintsISR.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/eints/eintsISR.c $
//

#include "FreeRTOS.h"
#include "task.h"

#include "eintsISR.h"

//
//
//
static void eintsISR_EINT0_Handler (void)
{
  SCB_EXTINT |= SCB_EXTINT_EINT0;

  GPIO0_FIOSET = GPIO_IO_P11;

	VIC_VectAddr = (unsigned portLONG) 0;
}

void eintsISR_EINT0 (void) __attribute__ ((naked));
void eintsISR_EINT0 (void)
{
  portSAVE_CONTEXT ();
  eintsISR_EINT0_Handler ();
  portRESTORE_CONTEXT ();
}

//
//
//
static void eintsISR_EINT2_Handler (void)
{
  SCB_EXTINT |= SCB_EXTINT_EINT1;

  GPIO0_FIOCLR = GPIO_IO_P11;

	VIC_VectAddr = (unsigned portLONG) 0;
}

void eintsISR_EINT2 (void) __attribute__ ((naked));
void eintsISR_EINT2 (void)
{
  portSAVE_CONTEXT ();
  eintsISR_EINT2_Handler ();
  portRESTORE_CONTEXT ();
}

