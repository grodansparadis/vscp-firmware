//
//  $Id: leds_reuse.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/leds/leds_reuse.c $
//

//
//  This code was from the original demo package.  Although it is no longer 
//  used, it shows an excellent example of how to reuse task code with 
//  local variables.  
//
//  Each time the task is create with xTaskCreate, it increment the task
//  number, which it uses to calculate a flash rate and which LED to
//  flash.
//
#include "FreeRTOS.h"
#include "task.h"

#include "../cpu/cpu.h"
#include "leds.h"

//
//
//
#define ledFLASH_RATE_BASE ((portTickType) 333)
static portBASE_TYPE uxFlashTaskNumber = 0;

portTASK_FUNCTION (vLEDFlashTask, pvParameters __attribute__ ((unused)))
{
  portTickType xFlashRate;
  portTickType xLastFlashTime;
  unsigned portBASE_TYPE uxLED;

  //
  // Calculate the LED and flash rate
  //
  portENTER_CRITICAL ();
  {
    uxLED = uxFlashTaskNumber;
    uxFlashTaskNumber++;
  }
  portEXIT_CRITICAL ();

  xFlashRate = ledFLASH_RATE_BASE + (ledFLASH_RATE_BASE * (portTickType) uxLED);
  xFlashRate /= portTICK_RATE_MS;

  //
  //  We will turn the LED on and off again in the delay period, so each delay is only half the total period
  //
  xFlashRate /= (portTickType) 2;

  //
  //  We need to initialise xLastFlashTime prior to the first call to vTaskDelayUntil()
  //
  xLastFlashTime = xTaskGetTickCount();

  for (;;)
  {
    vTaskDelayUntil (&xLastFlashTime, xFlashRate);
    cpuToggleLED (uxLED);
    vTaskDelayUntil (&xLastFlashTime, xFlashRate);
    cpuToggleLED (uxLED);
  }
}
