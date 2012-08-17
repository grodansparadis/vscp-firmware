//
//  $Id: usbser.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/usbser/usbser.h $
//

#ifndef _USBSER_H_
#define _USBSER_H_

#include "FreeRTOS.h"
#include "queue.h"

//
//
//
signed portBASE_TYPE usbserPutChar (signed portCHAR cOutChar, portTickType xBlockTime);
signed portBASE_TYPE usbserPutString (const signed portCHAR * const pcString, portTickType xBlockTime);
signed portBASE_TYPE usbserGetChar (signed portCHAR *pcRxedChar, portTickType xBlockTime);
void usbserInit (void);
void usbserGetRxQueue (xQueueHandle *qh);

#endif
