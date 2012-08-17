//
//  $Id: uartISR.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/uart/uartISR.h $
//

#ifndef _UARTISR_H_
#define _UARTISR_H_

#include "FreeRTOS.h"
#include "queue.h"

//
//
//
void uartISRCreateQueues (portCHAR pxPort, unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRX0Queue, xQueueHandle *pxTX0Queue, portLONG volatile **pplTHREEmptyFlag);
void uartISR0 (void);
void uartISR1 (void);
signed portBASE_TYPE uart0PostSpecialFromISR (U8 special);
signed portBASE_TYPE uart1PostSpecialFromISR (U8 special);

#endif
