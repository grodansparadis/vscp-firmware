#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#include "75x_lib.h"
#include "usb_lib.h"

#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

//#define USB_STRING_DESCRIPTOR_TYPE              0x03
//#define VIRTUAL_COM_PORT_SIZ_STRING_SERIAL        18

typedef struct
{
  u8  Virtual_Com_Port_StringSerial[VIRTUAL_COM_PORT_SIZ_STRING_SERIAL];
  u8  fver_maj;
  u8  fver_min;
  u8  hver_maj;
  u8  hver_min;
}CONF;

extern CONF conf;

#endif
