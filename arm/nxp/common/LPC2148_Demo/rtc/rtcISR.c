//
//  $Id: rtcISR.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/rtc/rtcISR.c $
//

#include <stdlib.h>

//
// Scheduler includes
//
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../uart/uart.h"
#include "../usbser/usbser.h"
#include "rtcISR.h"
#include "rtc.h"

//
//
//
static xQueueHandle consoleQueue = NULL;

//
//
//
void rtcISRInit (void)
{
#if defined CFG_CONSOLE_USB
  usbserGetRxQueue (&consoleQueue);
#elif defined CFG_CONSOLE_UART0
  uart0GetRxQueue (&consoleQueue);
#elif defined CFG_CONSOLE_UART1
  uart1GetRxQueue (&consoleQueue);
#else
#error "Eeek!  No console devices defined!"
#endif
}

//
//
//
static void rtcISR_Handler (void)
{
  portBASE_TYPE higherPriorityTaskWoken = pdFALSE;

  RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
  SCB_PCONP |= SCB_PCONP_PCRTC;

  if (RTC_ILR & RTC_ILR_RTCCIF)
  {
    U8 c = 0xff;

    if (consoleQueue)
      xQueueSendFromISR (consoleQueue, &c, &higherPriorityTaskWoken);

	  RTC_ILR = RTC_ILR_RTCCIF;
  }

  if (RTC_ILR & RTC_ILR_RTCALF)
  {
    U8 c = 0xfe;

    if (consoleQueue)
      xQueueSendFromISR (consoleQueue, &c, &higherPriorityTaskWoken);

	  RTC_ILR = RTC_ILR_RTCALF;
  }

  VIC_VectAddr = (unsigned portLONG) 0;

  RTC_CCR = (RTC_CCR_CLKEN | RTC_CCR_CLKSRC);
  SCB_PCONP &= ~SCB_PCONP_PCRTC;

  if (higherPriorityTaskWoken)
    portYIELD_FROM_ISR ();
}

void rtcISR (void) __attribute__ ((naked));
void rtcISR (void)
{
  portSAVE_CONTEXT ();
  rtcISR_Handler ();
  portRESTORE_CONTEXT ();
}
