//
//  $Id: usbinit.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/usb/usbinit.c $
//

#include "FreeRTOS.h"
#include "usbapi.h"


//
//  Data storage area for standard requests
//
static U8 abStdReqData [8];


//
//  USB reset handler
//
static void usbHandleReset(U8 bDevStatus __attribute__ ((unused)))
{
}

//
//
//
BOOL usbRegisterHandlers (void)
{
  usbHardwareInit ();
  usbHardwareRegisterDevIntHandler (usbHandleReset);
  usbHardwareRegisterEPIntHandler (0x00, usbHandleControlTransfer);
  usbHardwareRegisterEPIntHandler (0x80, usbHandleControlTransfer);
  usbRegisterRequestHandler (REQTYPE_TYPE_STANDARD, usbHandleStandardRequest, abStdReqData);

  return TRUE;
}
