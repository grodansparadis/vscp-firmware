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
* DataFlashNvm.c - ACS Serial I/O Data Flash Implementation
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

#include "inttypes.h"
#include "SerialIO.h"
#include "Globals.h"
#include "DataFlashNvm.h"
#include "crc.h"
#include "Flash_API_R8C_M12A.h"

#define	BLOCK_A_BASE	((uint8_t *)0x03000)
#define	BLOCK_B_BASE	((uint8_t *)0x03400)
#define	BLOCK_SIZE		1024

#define	BLOCK_A_SELECT	0
#define	BLOCK_B_SELECT	1

static volatile unsigned char selectedBlock;

static unsigned char *getReadBlockBase(void)
{
	return (unsigned char *)((selectedBlock == BLOCK_B_SELECT) ? BLOCK_B_BASE : BLOCK_A_BASE);
}

static unsigned char *getWriteBlockBase(void)
{
	return (unsigned char *)((selectedBlock == BLOCK_B_SELECT) ? BLOCK_A_BASE : BLOCK_B_BASE);
}

uint8_t *NVM_ReadBytes(uint16_t nvmOffset)
{
	return (unsigned char *)(getReadBlockBase() + nvmOffset);
}

void NVM_WriteBytes(unsigned short nvmOffset, unsigned char *data, unsigned short size)
{
	unsigned char *blockReadBase = getReadBlockBase();
	unsigned char *blockWriteBase = getWriteBlockBase();
	unsigned short blockOffset;
	unsigned char field;
	
	blockOffset = 0; 
	while (blockOffset < NVM_OFFSET(NvmCrc))
	{
		if (blockOffset < nvmOffset)
		{
			FlashWrite(blockWriteBase + blockOffset, blockReadBase + blockOffset, nvmOffset - blockOffset);
			blockOffset += (nvmOffset - blockOffset);
		}
		else if (blockOffset == nvmOffset)
		{
			FlashWrite(blockWriteBase + nvmOffset, data, size);
			blockOffset += size;
		}
		else
		{
			FlashWrite(blockWriteBase + blockOffset, blockReadBase + blockOffset, NVM_OFFSET(NvmCrc) - blockOffset);
			blockOffset += NVM_OFFSET(NvmCrc) - blockOffset;
		}
	}
	
	blockOffset = crcMedium(blockWriteBase, NVM_OFFSET(NvmCrc));
	field = blockOffset >> 8;
	FlashWrite(blockWriteBase + NVM_OFFSET(NvmCrc), &field, 1);
	field = blockOffset & 0x00ff;
	FlashWrite(blockWriteBase + NVM_OFFSET(NvmCrc) + 1, &field, 1);
	
	if (selectedBlock == BLOCK_A_SELECT)
	{
		selectedBlock = BLOCK_B_SELECT;
		FlashErase(BLOCK_A);
	}
	else
	{
		selectedBlock = BLOCK_A_SELECT;
		FlashErase(BLOCK_B);
	}
}

void NVM_Init(void)
{
	selectedBlock = BLOCK_A_SELECT;
	
	/* determine which block is intact */
	if (crcMedium(BLOCK_A_BASE, sizeof (struct NvmVariables)) == 0)
	{
		/* A is intact */
		selectedBlock = BLOCK_A_SELECT;
	}
	else if (crcMedium(BLOCK_B_BASE, sizeof (struct NvmVariables)) == 0)
	{
		/* B is intact */
		selectedBlock = BLOCK_B_SELECT;
	}
	else
	{
		FlashErase(BLOCK_A);
		FlashErase(BLOCK_B);
		NVM_Default();
	}
}

void NVM_Default(void)
{
	uint8_t ndx = 0;
	uint8_t	zeroData = 0;
	
	const struct _dmrow noDecision = {0,0,0,0,0,0,0,0};
	
	/* init the GUID */
	NVM_WriteBytes(NVM_OFFSET(NvmGUID), (uint8_t *)&GUID, sizeof (GUID));
		
	/* init the reporting */
	for (ndx = 0; ndx < MAX_CONTACTS; ndx++)
	{
		if (ndx < CONTACT_MEMORY_ALLOCATION)
		{
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactsOn[ndx]), &zeroData, 1);
			NVM_WriteBytes(NVM_OFFSET(NvmReportContactsOff[ndx]), &zeroData, 1);
		}
		
		NVM_WriteBytes(NVM_OFFSET(NvmReportSubzones[ndx]), &zeroData, 1);
	}
	
	/* init zone and subzone */
	NVM_WriteBytes(NVM_OFFSET(NvmZone), &zeroData, 1);
	NVM_WriteBytes(NVM_OFFSET(NvmSubzone), &zeroData, 1);
	
	/* init the decision matrix */
	for (ndx = 0; ndx < MAX_DECISION_ROWS; ndx++)
	{
		NVM_WriteBytes(NVM_OFFSET(NvmDecisionMatrix[ndx]), (uint8_t *)&noDecision, sizeof (struct _dmrow));
	}
}

