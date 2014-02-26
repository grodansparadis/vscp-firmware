/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2013-2014CS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
*                                                                                              
*       This Information is Proprietary to Ackerman Computer Sciences, Sarasota,               
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another               
* program without the express written consent of ACS. This Information or any                  
* portion thereof remains the property of ACS. The Information contained herein                
* is believed to be accurate and ACS assumes no responsibility or liability for                
* its use in any way and conveys no license or title under any patent or                       
* copyright and makes no representation or warranty that this Information is                   
* free from patent or copyright infringement.                                                  
*------------------------------------------------------------------------------                
* PhyRS485.c - ACS Serial I/O RS-485 Physical Layer Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*   6-07-13	First written.
*******************************************************************************                
*/                                                                                             

/////////////////////////////////////////////////////////////////////////////
// Physical Layer RS-485 Frame
/////////////////////////////////////////////////////////////////////////////
//	Sent using byte stuffing: 
//
//	SYNC = 0x7E	- beginning/end of frame
//	CE	= 0x7D	- control escape
//
//	When sending, if a SYNC or CE value appear within the frame, a CE followed 
//	by the value XOR 0x20 is substituted. When receiving, if a SYNC or CE value
//	appears within the frame it is substituted by the following value XOR 0x20.
//
//	SYNC
//	Destination Address
//		0 = server
//		1-127 = device address
//	Source Address
//		0 = server
//		1-127 = device address
//	Operation
//		0 = NOP - no data follows, just CRC
//		1 = poll for packet - no data follows, just CRC
//		2 = no packets to send - no data follows, just CRC
//		3 = upper level packet
//
//	Upper level packet
//
//	CRC
//		2 byte CRC-16
//	SYNC
/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "SerialIO.h"
#include "PhyRS485.h"
#include "Uart.h"
#include "crc.h"

/////////////////////////////////////////////////////////////////////////////
// local definitions
/////////////////////////////////////////////////////////////////////////////
#define	SYNC			0x7e	// sync
#define	CE				0x7d	// control escape
#define	TRANSP_MODIFIER	0x20	// transparency modifier

/* frameReceived() states */
#define	OUTSIDE_FRAME	0
#define	BEGIN_FRAME		1
#define	LINK_ESCAPE		2
#define	INSIDE_FRAME	3

#define	MAX_FRAME_BUFFER		15

/* phy frame offsets */
#define	FRAME_DESTINATION_ADDRESS_OFFSET	0
#define	FRAME_SOURCE_ADDRESS_OFFSET			1
#define	FRAME_OPERATION_OFFSET				2
#define	FRAME_PACKET_OFFSET					3

/* phy frame operation codes */
#define	OPERATION_NOP			0
#define	OPERATION_POLL			1
#define	OPERATION_NO_DATA		2
#define	OPERATION_DATA			3

/* PHY_RS485_Process() states */
#define	PHY_IDLE								0
#define	PHY_DATA_FRAME_RECEIVED					1
#define	PHY_RECEIVED_DATA_FRAME_HANDLED			2
#define	PHY_SENDING								3

/////////////////////////////////////////////////////////////////////////////
// local variables
/////////////////////////////////////////////////////////////////////////////
static volatile BYTE phyProcessState;

static BYTE	sendFrameCount;
static BYTE sendFrameQueued;
static crc	sendFrameCrc;

static volatile BYTE receiveFrameState, receiveCount, receivedDataNdx, receiveCrcErrors, receiveFrameErrors;
static BYTE	receiveFrameBuffer[MAX_FRAME_BUFFER];

/////////////////////////////////////////////////////////////////////////////
// local functions
/////////////////////////////////////////////////////////////////////////////
//
static void stuffSendData(BYTE data)
{
	sendFrameCrc = crcAccumulate(sendFrameCrc, data);
	if (data == SYNC || data == CE)
	{
		UART_QueueSendByte(CE);
		data ^= TRANSP_MODIFIER;
	}

	UART_QueueSendByte(data);
}

static void stuffSendDataNoCrc(BYTE data)
{
	if (data == SYNC || data == CE)
	{
		UART_QueueSendByte(CE);
		data ^= TRANSP_MODIFIER;
	}

	UART_QueueSendByte(data);
}

/////////////////////////////////////////////////////////////////////////////
//
static BOOLEAN frameReceived(void)
{
	BOOLEAN	frameReceived = FALSE;
	BYTE	c;
	
	if (UART_IsReceivedDataAvailable())
	{
		c = UART_ReceiveByte();
		
		switch (receiveFrameState)
		{
		case OUTSIDE_FRAME:
			if (c == SYNC)
			{
				receiveFrameState = BEGIN_FRAME;
				receiveCount = 0;
			}
			break;
			
		case BEGIN_FRAME:
			switch (c)
			{
			case SYNC:		// continue
				break;
				
			case CE:		// stuffed byte
				receiveFrameState = LINK_ESCAPE;
				break;
				
			default:		// first byte of frame
				if (receiveCount < MAX_FRAME_BUFFER)
				{
					receiveFrameBuffer[receiveCount++] = c;
					receiveFrameState = INSIDE_FRAME;
				}
				break;
			}
			break;
			
		case LINK_ESCAPE:
			switch (c)
			{
			case SYNC:		// new frame
				receiveFrameState = BEGIN_FRAME;
				receiveCount = 0;
				receiveFrameErrors++;
				break;
				
			case CE:		// undefined state
				break;
				
			default:		// stuffed byte - reverse transparency
				c ^= TRANSP_MODIFIER;
				if (receiveCount < MAX_FRAME_BUFFER)
				{
					receiveFrameBuffer[receiveCount++] = c;
					receiveFrameState = INSIDE_FRAME;
				}
				break;
			}
			break;
			
		case INSIDE_FRAME:
			switch (c)
			{
			case SYNC:		// new frame
				if (receiveCount > 0)
				{
					receiveFrameState = OUTSIDE_FRAME;
					frameReceived = (crcMedium(receiveFrameBuffer, receiveCount) == 0) ? TRUE : FALSE;
					if (!frameReceived)
					{
						SetLEDs(RED_LED);
						receiveCrcErrors++;
					}
					receiveCount -= 2;
				}
				else
				{
					receiveFrameState = BEGIN_FRAME;
					receiveCount = 0;
				}
				break;
				
			case CE:		// stuffed byte
				receiveFrameState = LINK_ESCAPE;
				break;

			default:		// next byte of frame
				if (receiveCount < MAX_FRAME_BUFFER)
				{
					receiveFrameBuffer[receiveCount++] = c;
					receiveFrameState = INSIDE_FRAME;
				}
				break;
			}
			break;
				
		default:	// error in frame
			receiveFrameState = OUTSIDE_FRAME;
			receiveCount = 0;
			break;
		}
	}
	
	return frameReceived;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOLEAN PHY_RS485_IsReceivedFrameAvailable(BYTE *nodeId)
{
	BOOLEAN	result = FALSE;
	
	if (phyProcessState == PHY_DATA_FRAME_RECEIVED)
	{
		if (nodeId != NULL)
		{
			*nodeId = receiveFrameBuffer[FRAME_SOURCE_ADDRESS_OFFSET];
		}
		result = TRUE;
	}
	
	return result;
}

BYTE PHY_RS485_ReceiveFrameData(void)
{
	BYTE	data = 0;
	
	if (phyProcessState == PHY_DATA_FRAME_RECEIVED)
	{
		data = receiveFrameBuffer[receivedDataNdx++];
		if (receivedDataNdx >= receiveCount)
		{
			phyProcessState = PHY_RECEIVED_DATA_FRAME_HANDLED;
		}
	}
	
	return data;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOLEAN PHY_RS485_SendRoomForFrame(BYTE count)
{
	return UART_SendQueueRoomFor(count);
}

void PHY_RS485_InitSendFrame(BYTE destinationAddress)
{
	UART_QueueSendByte(SYNC);
	UART_QueueSendByte(SYNC);
	sendFrameCrc = crcInit();
	stuffSendData(destinationAddress);
	stuffSendData(ReadDipSwitch());
	stuffSendData(OPERATION_DATA);
}

void PHY_RS485_SendFrameData(BYTE data)
{
	stuffSendData(data);
}

void PHY_RS485_FinishSendFrame(void)
{
	sendFrameCrc = crcFinal(sendFrameCrc);
	stuffSendDataNoCrc((BYTE)(sendFrameCrc >> 8));
	stuffSendDataNoCrc((BYTE)(sendFrameCrc & 0x00ff));
	UART_QueueSendByte(SYNC);
	sendFrameQueued = 1;
}

BOOLEAN PHY_RS485_IsSending(void)
{
	PHY_RS485_Process();
	
	return (phyProcessState == PHY_SENDING);
}

/////////////////////////////////////////////////////////////////////////////
//
void PHY_RS485_Init(void)
{
	UART_Init(TRUE, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
void PHY_RS485_Process(void)
{
	switch (phyProcessState)
	{
	case PHY_IDLE:
		if (frameReceived())
		{
			if (receiveFrameBuffer[FRAME_DESTINATION_ADDRESS_OFFSET] == ReadDipSwitch())
			{
				SetLEDs(GREEN_LED);
				switch (receiveFrameBuffer[FRAME_OPERATION_OFFSET])
				{
				case OPERATION_POLL:
					if (sendFrameQueued)
					{
						UART_SendQueue();
					}
					else
					{
						UART_QueueSendByte(SYNC);
						sendFrameCrc = crcInit();
						stuffSendData(receiveFrameBuffer[FRAME_SOURCE_ADDRESS_OFFSET]);
						stuffSendData(ReadDipSwitch());
						stuffSendData(OPERATION_NO_DATA);
						PHY_RS485_FinishSendFrame();
						UART_SendQueue();
					}
					sendFrameQueued = 0;
					phyProcessState = PHY_SENDING;
					break;
					
				case OPERATION_DATA:
					receivedDataNdx = FRAME_PACKET_OFFSET;
					phyProcessState = PHY_DATA_FRAME_RECEIVED;
					break;
					
				default:
					break;
				}
			}
		}
		else
		{
			SetLEDs(0);
		}
		break;
		
	case PHY_DATA_FRAME_RECEIVED:
		break;
		
	case PHY_RECEIVED_DATA_FRAME_HANDLED:
		if (sendFrameQueued)
		{
			UART_SendQueue();
			sendFrameQueued = 0;
			phyProcessState = PHY_SENDING;
		}
		else
		{
			phyProcessState = PHY_IDLE;
		}
		break;
		
	case PHY_SENDING:
		if (UART_IsSendDataCompleted())
		{
			phyProcessState = PHY_IDLE;
		}
		break;
		
	default:
		phyProcessState = PHY_IDLE;
		break;
	}	
}

