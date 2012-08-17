//
//  $Id: leds.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/leds/leds.c $
//

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../cpu/cpu.h"
#include "leds.h"

//
//
//
typedef struct
{
  int timeOn;
  int timeOff;
}
ledDutyCycles_t;

static ledDutyCycles_t ledDutyCycles [] = 
{
  { (200 / portTICK_RATE_MS), (800 / portTICK_RATE_MS) },
  { (400 / portTICK_RATE_MS), (600 / portTICK_RATE_MS) },
  { (600 / portTICK_RATE_MS), (400 / portTICK_RATE_MS) },
  { (800 / portTICK_RATE_MS), (200 / portTICK_RATE_MS) },
};

xQueueHandle xLEDQueue;

//
//
//
portTASK_FUNCTION (vLEDFlashTask, pvParameters __attribute__ ((unused)))
{
  portTickType ledTimeOn = 1;
  portTickType ledTimeOff = 1;
  portTickType lastTickTime;
  int dutyCycle = 0;

  //
  //  Create the queue, turn on LED and die if we can't
  //
  if ((xLEDQueue = xQueueCreate (1, sizeof (dutyCycle))) == 0)
  {
    GPIO0_FIOSET = GPIO_IO_P10;

    while (1)
      vTaskDelay (100);
  }

  //
  //  Send ourselves a message to init the flash time
  //
  xQueueSend (xLEDQueue, &dutyCycle, (portTickType) 0);

  //
  //  We need to initialise lastTickTime prior to the first call to vTaskDelayUntil()
  //
  lastTickTime = xTaskGetTickCount ();

  for (;;)
  {
    vTaskDelayUntil (&lastTickTime, ledTimeOn);
    GPIO0_FIOSET = GPIO_IO_P10;
    vTaskDelayUntil (&lastTickTime, ledTimeOff);
    GPIO0_FIOCLR = GPIO_IO_P10;

    if (xQueueReceive (xLEDQueue, &dutyCycle, (portTickType) 0))
    {
      dutyCycle %= arrsizeof (ledDutyCycles);

      ledTimeOn  = ledDutyCycles [dutyCycle].timeOn;
      ledTimeOff = ledDutyCycles [dutyCycle].timeOff;
    }
  }
}
