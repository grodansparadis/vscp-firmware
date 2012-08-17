
#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "messages.h"
#include "main.h"

#include "75x_lib.h"
#include "stdio.h"
#include "main.h"
#include "lists.h"
#include "string.h"
#include "leds.h"
#include "i2c_ee.h"

//USB includes
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"



/// Status codes /returned by status request)
#define CANAL_STATUS_NONE				0x00000000
#define CANAL_STATUS_ACTIVE			        0x10000000
#define CANAL_STATUS_PASSIVE				0x40000000
#define CANAL_STATUS_BUS_OFF				0x80000000
#define CANAL_STATUS_BUS_WARN				0x20000000
#define CANAL_STATUS_PHY_FAULT				0x08000000
#define CANAL_STATUS_PHY_H				0x04000000
#define CANAL_STATUS_PHY_L				0x02000000
#define CANAL_STATUS_SLEEPING				0x01000000
#define CANAL_STATUS_STOPPED				0x00800000
#define CANAL_STATUS_RECIVE_BUFFER_FULL		        0x00400000	// Drivers buffer
#define CANAL_STATUS_TRANSMIT_BUFFER_FULL	        0x00200000	// Drivers buffer

enum CAN_BAUD_RATE
{	
	CAN_BAUD_1000 = 0, 
        CAN_BAUD_800,
	CAN_BAUD_500, 
	CAN_BAUD_250, 
	CAN_BAUD_125,
	CAN_BAUD_100,
	CAN_BAUD_50,
	CAN_BAUD_20,
	CAN_BAUD_10,
        CAN_BAUD_MANUAL       
};


#define  CAN_MODE_NORMAL              0
#define  CAN_MODE_SILENT              1
#define  CAN_MODE_LOOPBACK            2
#define  CAN_MODE_SILENT_LOOPBACK     3


typedef struct
{
  u8   tseg1;
  u8   tseg2;
  u8   sjw;
  u16  brp;    
}CAN_TIMINGS;
  
extern void  USB2CAN_reset(void);
extern void  USB2CAN_open(u32 speed,u32 flags,CAN_TIMINGS *CanTimings);
extern void  USB2CAN_close(void);
extern void  USB2CAN_set_speed(u8 speed,CAN_TIMINGS *CanTimings);
extern u32  USB2CAN_get_status(void);
extern u32  USB2CAN_get_statistics( u8 num );

#endif