//
//  $Id: uart.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/uart/uart.h $
//

#ifndef _UART_H_
#define _UART_H_

#include "FreeRTOS.h"
#include "queue.h"

typedef void *xComPortHandle;

xComPortHandle uartInit (portCHAR pxPort, unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength);
void uartPutString (portCHAR pxPort, const signed portCHAR * const pcString, portTickType xBlockTime);
signed portBASE_TYPE uartGetChar (portCHAR pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime);
signed portBASE_TYPE uartPutChar (portCHAR pxPort, signed portCHAR cOutChar, portTickType xBlockTime);
void uartClose (portCHAR xPort);
void uart0GetRxQueue (xQueueHandle *qh);
void uart1GetRxQueue (xQueueHandle *qh);

#endif
