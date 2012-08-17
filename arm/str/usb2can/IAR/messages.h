
/*
Copyright (C) 2008 Gediminas Simanskis , edevices, www.edevices.lt
*/

#ifndef __MESSAGES_H
#define __MESSAGES_H

#include "75x_lib.h"
#include "usb_lib.h"

#include "configuration.h"

// Byte stuffing start and end characters
#define USB_DLE					0x10
#define USB_STX					0x02
#define USB_ETX					0x03

//////// command list

#define USB2CAN_RESET               1
#define USB2CAN_OPEN                2
#define USB2CAN_CLOSE               3
#define USB2CAN_SET_SPEED           4
#define USB2CAN_SET_MASK_FILETR     5
#define USB2CAN_GET_STATUS          6
#define USB2CAN_GET_STATISTICS      7
#define USB2CAN_GET_SERIAL          8
#define USB2CAN_GET_SOFTW_VER       9
#define USB2CAN_GET_HARDW_VER      10
#define USB2CAN_RESET_TIMESTAMP    11

#define USB2CAN_CMD_SUCCESS         0
#define USB2CAN_CMD_ERROR         255 

//USB2CAN Frame types
#define DATA_FRAME          		0	
#define DATA_FRAME_ACK                  1
#define DATA_FRAME_NACK                 2
#define DATA_FRAME_ERROR                3

/// Status message codes ( in received message )
#define CANAL_STATUSMSG_OK		0x00	// Normal condition. 
#define CANAL_STATUSMSG_OVERRUN		0x01	// Overrun occured when sending data to CAN bus. 
#define CANAL_STATUSMSG_BUSLIGHT	0x02	// Error counter has reached 96. 
#define CANAL_STATUSMSG_BUSHEAVY	0x03	// Error counter has reached 128. 
#define CANAL_STATUSMSG_BUSOFF		0x04	// Device is in BUSOFF. CANAL_STATUS_OK is 
																				// sent when returning to operational mode.
#define CANAL_STATUSMSG_STUFF		0x20	// Stuff Error.
#define CANAL_STATUSMSG_FORM		0x21	// Form Error.
#define CANAL_STATUSMSG_ACK		0x23	// Ack Error.
#define CANAL_STATUSMSG_BIT1		0x24	// Bit1 Error.
#define CANAL_STATUSMSG_BIT0		0x25	// Bit0 Error.
#define CANAL_STATUSMSG_CRC		0x27	// CRC Error.

//extern void  sendUSBdataToCANlist(void);
extern bool  USBdataCANmsg(canmsg *CanMsg);
extern void  CANtoUSBdata( canmsg *msg);
extern u8    GetUSBdataCount(void);
extern void  ClearUSBdataBuffer(void);

extern u8  GetUSBcmdCount(void);
extern void ClearUSBcmdBuffer(void);

extern void GetUSBcmd(void);
extern void ReturnUSBcmd(void);

extern bool SendErrorMessage( u8 err );

#endif
