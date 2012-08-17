//
//  $Id: sensors.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/sensors/sensors.c $
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "../main.h"
#include "../adc/adc.h"
#include "../dac/dac.h"
#include "../leds/leds.h"
#include "sensors.h"

//
//
//
static sensorData_t sensorData;
static xSemaphoreHandle semaphore;

//
//  Return 1 if got a copy, 0 if not.
//
int sensorsCopyData (sensorData_t *dst)
{
  if (xSemaphoreTake (semaphore, 100 / portTICK_RATE_MS) == pdTRUE)
  {
    memcpy (dst, &sensorData, sizeof (sensorData));
    xSemaphoreGive (semaphore);
    return 1;
  }

  memset (dst, 0, sizeof (sensorData_t));
  return 0;
}

//
//
//
portTASK_FUNCTION (vSensorsTask, pvParameters __attribute__ ((unused)))
{
  portTickType xTickCount;
  int adcValue;
  int adcLastValue = -1;
  int adcChanged = -1;
  int dacValue = 0;
  int dacDirection = -16;

  memset (&sensorData, 0, sizeof (sensorData));

  vSemaphoreCreateBinary (semaphore);

  xTickCount = xTaskGetTickCount ();

  for (;;)
  {
    vTaskDelayUntil (&xTickCount, 100 / portTICK_RATE_MS);

    //
    //  Adjust the DAC value so we output a slow sine wave
    //
    if ((dacValue <= 0) || (dacValue >= (1024 - dacDirection)))
      dacDirection = 0 - dacDirection;

    dacSet (dacValue);
    dacValue += dacDirection;

    //
    //  Read the current ADC value, keep only top 2 bits.  If it changes,
    //  tell the LED task to change the blink rate.
    //
    if ((adcValue = (adcRead0_3 () >> 8)) != adcLastValue)
    {
      xQueueSend (xLEDQueue, &adcValue, (portTickType) 0);
      adcLastValue = adcValue;
      adcChanged++;
    }

    //
    //  Update the sensors data
    //
    if (xSemaphoreTake (semaphore, portMAX_DELAY) == pdTRUE)
    {
      sensorData.sensorCount++;
      sensorData.adcChanges = adcChanged;
      sensorData.dacValue = dacValue;
      sensorData.dacDirection = dacDirection;

      xSemaphoreGive (semaphore);
    }
  }
}
