//
//  $Id: main.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/main.c $
//

//
//  Standard includes
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//
// Scheduler includes
//
#include "FreeRTOS.h"
#include "task.h"

//
//  Demo application includes
//
#include "main.h"
#include "adc/adc.h"
#include "cpu/cpu.h"
#include "dac/dac.h"
#include "eints/eints.h"
#include "gps/gps.h"
#include "fiq/fiq.h"
#include "i2c/i2c.h"
#include "iap/iap.h"
#include "lcd/lcd.h"
#include "leds/leds.h"
#include "monitor/monitor.h"
#include "pwm/pwm.h"
#include "rtc/rtc.h"
#include "sensors/sensors.h"
#include "timer/timer.h"
#include "uart/uart.h"
#include "usbser/usbser.h"
#include "usbmass/usbmass.h"

//
//
//
#define BAUD_UART0 115200
#define BAUD_UART1 4800

#ifdef CFG_CONSOLE_UART1
#undef BAUD_UART1
#define BAUD_UART1 115200
#endif

#if defined CFG_CONSOLE_USB && defined CFG_USB_MSC
#error Cannot have USB console and MSC defined at the same time
#endif

//
//
//
xTaskHandle taskHandles [TASKHANDLE_LAST];

//
//
//
int main (void)
{
  cpuSetupHardware ();
  uartInit (0, BAUD_UART0, 64);
  uartInit (1, BAUD_UART1, 64);
#ifndef CFG_USB_MSC
  usbserInit ();
#else
  usbmassInit ();
#endif
  rtcInit ();
  timerInit ();
  adcInit ();
  dacInit ();
  pwmInit ();
  i2cInit ();
  eintsInit ();
  fiqInit ();
  iapInit ();
  lcdInit ();

  memset (taskHandles, 0, sizeof (taskHandles));

  xTaskCreate (vSensorsTask,  (const signed portCHAR * const) "Sensors",  512,                      NULL, (tskIDLE_PRIORITY + 2), &taskHandles [TASKHANDLE_SENSORS]);
#ifndef CFG_CONSOLE_UART1
  xTaskCreate (vGPSTask,      (const signed portCHAR * const) "GPS",      768,                      NULL, (tskIDLE_PRIORITY + 1), &taskHandles [TASKHANDLE_GPS]);
#endif
  xTaskCreate (vMonitorTask,  (const signed portCHAR * const) "Monitor",  1024,                     NULL, (tskIDLE_PRIORITY + 3), &taskHandles [TASKHANDLE_MONITOR]);
  xTaskCreate (vLEDFlashTask, (const signed portCHAR * const) "LEDx",     configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2), &taskHandles [TASKHANDLE_LED]);

  vTaskStartScheduler ();

  return 0;
}
