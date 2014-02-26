/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2013-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* vscp_interface.c - ACS Serial I/O VSCP Interface Software Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-24-13	First written.
*******************************************************************************                
*/

#include <string.h>
#include "SerialIO.h"
#include "PhyRS485.h"
#include "TimerRJ2.h"
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "Globals.h"
#include "vscp_interface.h"
#include "DataFlashNvm.h"

/* application registers */
#define	APP_REG_CONTACTS		0
#define	APP_REG_REPORT_ON		(APP_REG_CONTACTS + CONTACT_MEMORY_ALLOCATION)
#define	APP_REG_REPORT_OFF		(APP_REG_CONTACTS + APP_REG_REPORT_ON + CONTACT_MEMORY_ALLOCATION)
#define	APP_REG_DECISION_MATRIX	64

// --------------------------- External Functions -----------------------------
//
// All functions below should be implemented by the application
//
// --------------------------- External Functions -----------------------------

// RS-485 Frame
//	Sent using byte stuffing: 
//
//	SYNC = 0x7E	- beginning/end of frame
//	CE	= 0x7D	- control escape
//
//	When sending, if a SYNC or CE value appear within the frame, a CE followed by the value XOR 0x20 is substituted.
//	When receiving, if a SYNC or CE VALUE appears within the frame it is substituted by the following value XOR 0x20.
//
//	SYNC
//	Address
//		0 = server
//		1-127 = device address
//	Operation
//		0 = NOP
//		1 = Level 1 event to/from device
//		2 = Level 2 event to/from device
//		3 = Poll for event
//		4 = no events to send
//		249 = Sent ACK
//		250 = Sent NACK
//		251 = ACK
//		252 = NACK
//		253 = error
//		254 = Command Reply
//		255 = Command
//	Flags
//		B5 = bit 9 of Class
//		B4-B0 = number of Data Bytes
//	Class
//		Lower 8 bits of Class
//	Type
//		Type
//	Data
//		0 to 15 data bytes
//	CRC
//		2 byte CRC-16
//	SYNC
//

/*
    Get a VSCP frame frame
    @param pvscpclass Pointer to variable that will get VSCP class.
    @param pvscptype Pointer to variable which will get VSCP type.
    @param pNodeId Pointer to variable which will get nodeid.
    @param pPriority Pointer to variable which will get priority (0-7).
    @param pSize Pointer to variable that will get datasize.
    @param pData pinter to array that will get event data.
    @return TRUE on success.
 */
int8_t getVSCPFrame(uint16_t *pvscpclass,
			        uint8_t *pvscptype,
			        uint8_t *pNodeId,
			        uint8_t *pPriority,
			        uint8_t *pSize,
			        uint8_t *pData)
{
	int8_t	result = FALSE;
	uint8_t	flags, ndx;

	if (PHY_RS485_IsReceivedFrameAvailable(pNodeId))
	{
		*pPriority = 0;
		flags = PHY_RS485_ReceiveFrameData();
		*pvscpclass = (uint16_t)PHY_RS485_ReceiveFrameData() | ((uint16_t)flags & 0x0010) << 4;
		*pvscptype = PHY_RS485_ReceiveFrameData();
		if ((flags &= 0x0f) > 8) flags = 8;
		*pSize = flags;
		 for (ndx = 0; ndx < flags; ndx++)
		 {
			 pData[ndx] = PHY_RS485_ReceiveFrameData();
		 }
		 
		 result = TRUE;
	}
	
	return result;
}

/*
    Send a VSCP frame 
    @param vscpclass VSCP class for event.
    @param vscptype VSCP type for event.
        @param nodeid Nodeid for originating node.
        @param priority Priotity for event.
    @param size Size of data portion.
    @param pData Pointer to event data.
    @return TRUE on success.
 */
int8_t sendVSCPFrame(uint16_t vscpclass,
			        uint8_t vscptype,
			        uint8_t nodeid,
			        uint8_t priority,
			        uint8_t size,
			        uint8_t *pData)
{
	int8_t	result = FALSE;
	uint8_t ndx;

	if ((size &= 0x0f) > 8) size = 8;
	vscp_sendFrameTimer = 50;	/* only hang in here for up to one second */
	do
	{	
		if (PHY_RS485_SendRoomForFrame(12 + size))
		{
			PHY_RS485_InitSendFrame(nodeid);
			PHY_RS485_SendFrameData((vscpclass & 0x0100) >> 4 | size);
			PHY_RS485_SendFrameData((uint8_t)vscpclass);
			PHY_RS485_SendFrameData(vscptype);
			for (ndx = 0; ndx < size; ndx++)
			{
				PHY_RS485_SendFrameData(pData[ndx]);
			}
			PHY_RS485_FinishSendFrame();
		
			result = TRUE;
			break;
		}
		
	} while (PHY_RS485_IsSending() && vscp_sendFrameTimer);
	
	return result;
}


/*!
        The following methods must be defined
        in the application and should return firmware version
        information
 */
uint8_t vscp_getMajorVersion(void)
{
	return FIRMWARE_VERSION_MAJOR;
}

uint8_t vscp_getMinorVersion(void)
{
	return FIRMWARE_VERSION_MINOR;
}

uint8_t vscp_getSubMinorVersion(void)
{
	return FIRMWARE_VERSION_SUBMINOR;
}

/*! 
        Get GUID from permament storage
 */
uint8_t vscp_getGUID(uint8_t idx)
{
	return *NVM_ReadBytes(NVM_OFFSET(NvmGUID[idx]));
}

void vscp_setGUID(uint8_t idx, uint8_t data)
{
	NVM_WriteBytes(NVM_OFFSET(NvmGUID[idx]), &data, 1);
}

/*!
        User ID 0 idx=0
        User ID 1 idx=1
        User ID 2 idx=2
        User ID 3 idx=3
 */
uint8_t vscp_getUserID(uint8_t idx)
{
	return *NVM_ReadBytes(NVM_OFFSET(NvmUserID[idx]));
}

void vscp_setUserID(uint8_t idx, uint8_t data)
{
	NVM_WriteBytes(NVM_OFFSET(NvmUserID[idx]), &data, 1);
}

/*!
        Handle manufacturer id.
	
        Not that both main and sub id are fetched here
                Manufacturer device ID byte 0 - idx=0
                Manufacturer device ID byte 1 - idx=1
                Manufacturer device ID byte 2 - idx=2
                Manufacturer device ID byte 3 - idx=3
                Manufacturer device sub ID byte 0 - idx=4
                Manufacturer device sub ID byte 1 - idx=5
                Manufacturer device sub ID byte 2 - idx=6
                Manufacturer device sub ID byte 3 - idx=7
 */
uint8_t vscp_getManufacturerId(uint8_t idx)
{
	return *NVM_ReadBytes(NVM_OFFSET(NvmManufacturerID[idx]));
}

void vscp_setManufacturerId(uint8_t idx, uint8_t data)
{
	NVM_WriteBytes(NVM_OFFSET(NvmManufacturerID[idx]), &data, 1);
}

/*!
        Get bootloader algorithm from permanent storage
 */
uint8_t vscp_getBootLoaderAlgorithm(void)
{
	return VSCP_BOOTLOADER_NONE;
}

/*! 
        Get buffer size
 */
uint8_t vscp_getBufferSize(void)
{
	return 8;
}

/*! 
        Get number of register pages used by app.
 */
uint8_t vscp_getRegisterPagesUsed(void)
{
	return MAX_APP_REG_PAGES;
}

/*!
        Get URL from device from permanent storage
        index 0-32
 */
uint8_t vscp_getMDF_URL(uint8_t idx)
{
	if (idx <= strlen(MDF_URL))
	{
		return MDF_URL[idx];
	}
	else
	{
		return 0;
	}
}

/*!
        Fetch nickname from permanent storage
        @return read nickname.
 */
uint8_t vscp_readNicknamePermanent(void)
{
	return ReadDipSwitch();
}

/*! 
        Write nickname to permanent storage
        @param nickname to write
 */
void vscp_writeNicknamePermanent(uint8_t nickname)
{
}

/*! 
        Fetch segment CRC from permanent storage
 */
uint8_t vscp_getSegmentCRC(void)
{
	return *(NVM_ReadBytes(NVM_OFFSET(NvmSegmentCrc)));
}

/*! 
        Write segment CRC to permanent storage
 */
void vscp_setSegmentCRC(uint8_t crc)
{
	NVM_WriteBytes(NVM_OFFSET(NvmSegmentCrc), &crc, 1);
}

/*! 
        Write control byte permanent storage
 */
void vscp_setControlByte(uint8_t ctrl)
{
	NVM_WriteBytes(NVM_OFFSET(NvmControlByte), &ctrl, 1);
}

/*!
        Fetch control byte from permanent storage
 */
uint8_t vscp_getControlByte(void)
{
	return *(NVM_ReadBytes(NVM_OFFSET(NvmControlByte)));
}

/*! 
        Get page select bytes
                idx=0 - byte 0 MSB
                idx=1 - byte 1 LSB
 */
//uint8_t vscp_getPageSelect(uint8_t idx)
//{
//}

/*!
        Set page select registers
        @param idx 0 for LSB, 1 for MSB
        @param data Byte to set of page select registers
 */
//void vscp_setPageSelect(uint8_t idx, uint8_t data)
//{
//}

/*!
        Read application register (lower part)
        @param reg Register to read (<0x80)
        @return Register content or 0x00 for non valid register
 */
uint8_t vscp_readAppReg(uint8_t reg)
{
	uint8_t regOffset = reg >> 3;
	uint8_t regBit = (1 << (reg & 0x07));
	
	if (reg <= APP_REG_LAST_CONTACT)
	{
		switch (vscp_page_select)
		{
		case APP_REG_PAGE_CONTACTS:
		default:
			return (Contacts[regOffset] & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_CLOSURES:
			return (ContactClosures[regOffset] & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_OPENINGS:
			return (ContactOpenings[regOffset] & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_REPORT_ON:
			return (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOn[regOffset])) & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_REPORT_OFF:
			return (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[regOffset])) & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_REPORT_CLOSURES:
			return (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactClosures[regOffset])) & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_REPORT_OPENINGS:
			return (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactOpenings[regOffset])) & regBit) ? 1 : 0;
			
		case APP_REG_PAGE_REPORT_SUBZONES:
			return *(NVM_ReadBytes(NVM_OFFSET(NvmReportSubzones[reg])));
		}
	}
	else if (reg == APP_REG_ZONE)
	{
		return *(NVM_ReadBytes(NVM_OFFSET(NvmZone)));
	}
	else if (reg == APP_REG_SUBZONE)
	{
		return *(NVM_ReadBytes(NVM_OFFSET(NvmSubzone)));
	}
	else if (reg >= APP_REG_DECISION_MATRIX)
	{
		reg -= APP_REG_DECISION_MATRIX;
		regOffset = NVM_OFFSET(NvmDecisionMatrix[reg / sizeof (struct _dmrow)]);
		return *(NVM_ReadBytes(regOffset + (reg % sizeof (struct _dmrow))));
	}
	else
	{
		return 0;
	}
}

/*!
        Write application register (lower part)
        @param reg Register to read (<0x80)
        @param value Value to write to register.
        @return Register content or 0xff for non valid register
 */
uint8_t vscp_writeAppReg(uint8_t reg, uint8_t value)
{
	uint8_t regOffset = reg >> 3;
	uint8_t regBit = (1 << (reg & 0x07));
	uint8_t	preRead = 0;
	
	if (reg <= APP_REG_LAST_CONTACT)
	{
		switch (vscp_page_select)
		{
		case APP_REG_PAGE_CONTACTS:
			if (value)
			{
				DISABLE_TIMERRJ2_INTERRUPT();
				OutputContacts[regOffset] |= regBit;
				ENABLE_TIMERRJ2_INTERRUPT();

				if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOn[regOffset]))) & regBit)
				{
					vscp_sendInformationEvent(reg, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);
				}
			}
			else
			{
				DISABLE_TIMERRJ2_INTERRUPT();
				OutputContacts[regOffset] &= ~regBit;
				ENABLE_TIMERRJ2_INTERRUPT();

				if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[regOffset]))) & regBit)
				{
					vscp_sendInformationEvent(reg, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF);
				}
			}
			return (OutputContacts[regOffset] & regBit) ? 1 : 0;
		
		case APP_REG_PAGE_CLOSURES:
			DISABLE_TIMERRJ2_INTERRUPT();
			if (value)
			{
				ContactClosures[regOffset] |= regBit;
			}
			else
			{
				ContactClosures[regOffset] &= ~regBit;
			}
			ENABLE_TIMERRJ2_INTERRUPT();
			break;
		
		case APP_REG_PAGE_OPENINGS:
			DISABLE_TIMERRJ2_INTERRUPT();
			if (value)
			{
				ContactOpenings[regOffset] |= regBit;
			}
			else
			{
				ContactOpenings[regOffset] &= ~regBit;
			}
			ENABLE_TIMERRJ2_INTERRUPT();
			break;
		
		case APP_REG_PAGE_REPORT_ON:
			preRead = *NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOn[regOffset]));
			if (value)
			{
				preRead |= regBit;
			}
			else
			{
				preRead &= ~regBit;
			}
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactsOn[regOffset]), &preRead, 1);
			break;

		case APP_REG_PAGE_REPORT_OFF:
			preRead = *NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[regOffset]));
			if (value)
			{
				preRead |= regBit;
			}
			else
			{
				preRead &= ~regBit;
			}
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactsOff[regOffset]), &preRead, 1);
			break;
		
		case APP_REG_PAGE_REPORT_CLOSURES:
			preRead = *NVM_ReadBytes(NVM_OFFSET(NvmReportContactClosures[regOffset]));
			if (value)
			{
				preRead |= regBit;
			}
			else
			{
				preRead &= ~regBit;
			}
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactClosures[regOffset]), &preRead, 1);
			break;
		
		case APP_REG_PAGE_REPORT_OPENINGS:
			preRead = *NVM_ReadBytes(NVM_OFFSET(NvmReportContactOpenings[regOffset]));
			if (value)
			{
				preRead |= regBit;
			}
			else
			{
				preRead &= ~regBit;
			}
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactOpenings[regOffset]), &preRead, 1);
			break;
			
		case APP_REG_PAGE_REPORT_SUBZONES:
			NVM_WriteBytes(NVM_OFFSET(NvmReportSubzones[reg]), &value, 1);
			break;
			
		default:
			break;
		}
		return vscp_readAppReg(reg);
	}
	else if (reg == APP_REG_ZONE)
	{
		NVM_WriteBytes(NVM_OFFSET(NvmZone), &value, 1);
		return vscp_readAppReg(reg);
	}
	else if (reg == APP_REG_SUBZONE)
	{
		NVM_WriteBytes(NVM_OFFSET(NvmSubzone), &value, 1);
		return vscp_readAppReg(reg);
	}
	else if (reg >= APP_REG_DECISION_MATRIX)
	{
		reg -= APP_REG_DECISION_MATRIX;
		regOffset = NVM_OFFSET(NvmDecisionMatrix[reg / sizeof (struct _dmrow)]);
		NVM_WriteBytes(regOffset + (reg % sizeof (struct _dmrow)), &value, 1);
		return vscp_readAppReg(reg + APP_REG_DECISION_MATRIX);
	}
	else
	{
		return 0;
	}
}

/*!
        Get DM matrix info
        The output message data structure should be filled with
        the following data by this routine.
        byte 0 - Number of DM rows. 0 if none.
        byte 1 - offset in register space.
        byte 2 - start page MSB
        byte 3 - start page LSB
        byte 4 - End page MSB
        byte 5 - End page LSB
        byte 6 - Level II size of DM row (Just for Level II nodes).
 */
void vscp_getMatrixInfo(char *pData)
{
	memset(pData, 0, 7);
	pData[0] = MAX_DECISION_ROWS;
	pData[1] = APP_REG_DECISION_MATRIX;
}

/*!
        Get embedded MDF info
        If available this routine sends an embedded MDF file
        in several events. See specification CLASS1.PROTOCOL
        Type=35/36
 */
void vscp_getEmbeddedMdfInfo(void)
{
	BYTE	urlIndex = 0;
	BYTE	msgIndex;
	
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF_RESPONSE;

#if 1

		vscp_omsg.data[0] = 0;
		vscp_omsg.data[1] = 0;
		vscp_omsg.data[2] = 0;
		vscp_omsg.flags = VSCP_VALID_MSG + 3;
		vscp_sendEvent();

#else

	do 
	{
		vscp_omsg.data[0] = 0;
		vscp_omsg.data[1] = urlIndex;
		
		for (msgIndex = 2; msgIndex < 8 && urlIndex < strlen(MDF_URL); msgIndex++)
		{
			vscp_omsg.data[msgIndex] = MDF_URL[urlIndex++];
		}
		
		vscp_omsg.flags = VSCP_VALID_MSG + msgIndex;
		
		vscp_sendEvent();
		
	} while (urlIndex < strlen(MDF_URL));
	
#endif
	
}

/*!
        Go bootloader mode
        This routine force the system into bootloader mode according
        to the selected protocol.
 */
void vscp_goBootloaderMode(void)
{
}

/*!
        Get Zone for device
        Just return zero if not used.
 */
uint8_t vscp_getZone(void)
{
	return *(NVM_ReadBytes(NVM_OFFSET(NvmZone)));
}

/*!
        Get Subzone for device
        Just return zero if not used.
    
 */
uint8_t vscp_getSubzone(void)
{
	return *(NVM_ReadBytes(NVM_OFFSET(NvmSubzone)));
}

/* process decision matrix */
void vscp_doDecisionMatrix(void)
{
	uint8_t 	rowNdx, contactOffset, contactBit;
	uint16_t	class_filter, class_mask;
	struct _dmrow *decision;

	/* Q - CLASS1_PROTOCOL event ? */
	if (vscp_imsg.class == VSCP_CLASS1_PROTOCOL) 
	{
		/* yes - ignore it */
		return;
	}
	
	/* process all decision matrix rows */
	for (rowNdx = 0; rowNdx < MAX_DECISION_ROWS; rowNdx++)
	{
		/* get pointer to decision matrix row */
		decision = (struct _dmrow *)NVM_ReadBytes(NVM_OFFSET(NvmDecisionMatrix[rowNdx]));
		
		/* Q - is this row enabled ? */
		if (decision->flags & VSCP_DM_FLAG_ENABLED)
		{
			/* yes - Q - check originating address enabled and does it match ? */
			if (	(decision->flags & VSCP_DM_FLAG_CHECK_OADDR)
				&&	(vscp_imsg.oaddr != decision->oaddr))
			{
				/* no */
				continue;
			}
			
			/* Q - check zone match enabled ? */
			if (decision->flags & VSCP_DM_FLAG_CHECK_ZONE)
			{
				/* yes - Q - does the zone match ? */
				if (	(vscp_imsg.data[1] != 255)
					&&	(vscp_imsg.data[1] != *NVM_ReadBytes(NVM_OFFSET(NvmZone))))
				{
					/* no */
					continue;
				}
			}
			
			class_filter = ((decision->flags & VSCP_DM_FLAG_CLASS_FILTER) << 8) | decision->class_filter;
			class_mask = ((decision->flags & VSCP_DM_FLAG_CLASS_MASK) << 8) | decision->class_mask;
			if (	!((vscp_imsg.class ^ class_filter) & class_mask)
				&&	!((vscp_imsg.type ^ decision->type_filter) & decision->type_mask))
			{
				if (decision->action_param <= APP_REG_LAST_CONTACT)
				{
					if (decision->flags & VSCP_DM_FLAG_CHECK_SUBZONE)
					{
						if (vscp_imsg.data[2] != 255)
						{
							if (*NVM_ReadBytes(NVM_OFFSET(NvmReportSubzones[decision->action_param])) == 0)
							{
								if (vscp_imsg.data[2] != *NVM_ReadBytes(NVM_OFFSET(NvmSubzone)))
								{
									continue;
								}
							}
							else
							{
								if (vscp_imsg.data[2] != *NVM_ReadBytes(NVM_OFFSET(NvmReportSubzones[decision->action_param])))
								{
									continue;
								}
							}
						}
					}
						
					contactOffset = decision->action_param >> 3;
					contactBit = (1 << (decision->action_param & 0x07));
					switch (decision->action)
					{
					case ACTION_OFF:
						OutputContacts[contactOffset] &= ~(contactBit);
						if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[contactOffset]))) & contactBit)
						{
							vscp_sendInformationEvent(decision->action_param, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF);
						}
						break;
				
					case ACTION_ON:
						OutputContacts[contactOffset] |= contactBit;
						if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[contactOffset]))) & contactBit)
						{
							vscp_sendInformationEvent(decision->action_param, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);
						}
						break;
				
					case ACTION_TOGGLE:
						OutputContacts[contactOffset] ^= contactBit;
						// fall into ACTION_STATUS
										
					case ACTION_STATUS:
						if (OutputContacts[contactOffset] & contactBit)
						{
							if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[contactOffset]))) & contactBit)
							{
								vscp_sendInformationEvent(decision->action_param, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);
							}
						}
						else
						{
							if (*(NVM_ReadBytes(NVM_OFFSET(NvmReportContactsOff[contactOffset]))) & contactBit)
							{
								vscp_sendInformationEvent(decision->action_param, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF);
							}
						}
						break;
				
					case ACTION_NOP:
					default:
						break;
					}
				}
			}
		}
	}
}

void vscp_sendInformationEvent(BYTE index, BYTE eventClass, BYTE eventType)
{
	BYTE	subzone = 0;
	
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = eventClass;
	vscp_omsg.type = eventType;
	vscp_omsg.data[0] = index;
	vscp_omsg.data[1] = *NVM_ReadBytes(NVM_OFFSET(NvmZone));
	subzone = *(NVM_ReadBytes(NVM_OFFSET(NvmReportSubzones[index])));
	vscp_omsg.data[2] = (subzone > 0) ? subzone : *NVM_ReadBytes(NVM_OFFSET(NvmSubzone));
	vscp_sendEvent();
}

