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
* DataFlashNvm.h - ACS Serial I/O Data Flash Interface
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

#ifndef	_DATAFLASHNVM_H_
#define	_DATAFLASHNVM_H_

#include <stddef.h>
#include "SerialIO.h"
#include "Globals.h"
#include "vscp_firmware.h"
#include "vscp_interface.h"

#pragma pack

struct NvmVariables
{
	unsigned char	NvmSegmentCrc;
	unsigned char	NvmControlByte;
	unsigned char	NvmGUID[16];
	unsigned char	NvmUserID[4];
	unsigned char	NvmManufacturerID[8];
	unsigned char	NvmZone;
	unsigned char	NvmSubzone;
	struct _dmrow	NvmDecisionMatrix[MAX_DECISION_ROWS];
	unsigned char	NvmReportContactsOn[CONTACT_MEMORY_ALLOCATION];
	unsigned char	NvmReportContactsOff[CONTACT_MEMORY_ALLOCATION];
	unsigned char	NvmReportContactClosures[CONTACT_MEMORY_ALLOCATION];
	unsigned char	NvmReportContactOpenings[CONTACT_MEMORY_ALLOCATION];
	unsigned char	NvmReportSubzones[MAX_CONTACTS];
	unsigned short	NvmCrc;
};

#pragma packoption

#define	NVM_OFFSET(id)	(offsetof(struct NvmVariables, id))

unsigned char *NVM_ReadBytes(uint16_t nvmOffset);
void NVM_WriteBytes(unsigned short nvmOffset, unsigned char *data, unsigned short size);
void NVM_Init(void);
void NVM_Default(void);

#endif	/* _DATAFLASHNVM_H_ */
