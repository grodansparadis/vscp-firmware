/*********************************************************************
 *
 *            CRC implementation, slow algorithm
 *
 *********************************************************************
 * FileName:        crc.c
 * Dependencies:    See INCLUDES section below
 * Processor:       Daytona
 * Compiler:		MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.10.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: Crc.c,v 1.2 2006/07/25 18:28:03 C12878 Exp $
 *
 ********************************************************************/

#include <stdlib.h>
#include <time.h>

#include <peripheral/generic.h>
#include "../include/crc.h"



// local storage

PRIVATE UINT32			CrcPoly;		// the current CRC generator polynomial
PRIVATE UINT32			CrcReg;			// the current value of the CRC, the shift register
PRIVATE UINT32			CrcMask;		// mask to retain the useful CRC result
PRIVATE UINT32			CrcMsb;			// mask to obtain the MSb-1, transport to the MSb.


// Crc 32 Slow implementation

/*********************************************************************
 * Function:        void Crc32Init(UINT32 polynomial, UINT32 polOrder, UINT32 seed)
 *
 * PreCondition:    polOrder <= 32
 * 
 * Input:           - polynomial: the generator polynomial to be used
 * 					- polOrder: the order of the generator polynomial
 * 					- seed: the initial seed into the generator polynomial
 * 
 * Output:          None
 * 
 * Side Effects:    None
 * 
 * Overview:        Initializes the software CRC generator.
 * 
 * Note:            None
 ********************************************************************/
PUBLIC void Crc32Init(UINT32 polynomial, UINT32 polOrder, UINT32 seed)
{
	CrcPoly=polynomial;
	CrcReg=seed;
	CrcMsb=1L<<(polOrder-1);			// mask to obtain the MSb-1, transport to the MSb.
	if(polOrder==32)
	{
		CrcMask=0xffffffff;
	}
	else
	{
		CrcMask=(1L<<polOrder)-1;		// mask to retain the useful CRC result
	}
	
}


/*********************************************************************
 * Function:        void Crc32Add(BYTE* pBuff, unsigned int bSize)
 *
 * PreCondition:    pBuff valid pointer
 * 
 * Input:           - pBuff: buffer to be calculated
 * 					- bSize: buffer size
 * 
 * Output:          None
 * 
 * Side Effects:    None
 * 
 * Overview:        Shifts bytes through the CRC shift register.
 * 
 * Note:            None
 ********************************************************************/
PUBLIC void Crc32Add(BYTE* pBuff, unsigned int bSize)
{
	while(bSize--)
	{
		int		ix;
		BYTE	b=*pBuff++;
		int		mask=0x80;
		
		for(ix=0; ix<8; ix++)
		{
			BOOL	fback=(CrcReg&CrcMsb)!=0;
			CrcReg<<=1;
			if((b&mask)!=0)
			{
				CrcReg|=1;
			}
			if(fback)
			{
				CrcReg^=CrcPoly;
			}
			mask>>=1;
		}
	}
}


/*********************************************************************
 * Function:        UINT32 Crc32Result(void)
 *
 * PreCondition:    None
 * 
 * Input:           None
 * 
 * Output:          the software CRC result.
 * 
 * Side Effects:    None
 * 
 * Overview:        Returns the current value of the shift register. This is the calculated CRC.
 * 
 * Note:            None
 ********************************************************************/
PUBLIC UINT32 Crc32Result(void)
{
	return CrcReg&CrcMask;
}

