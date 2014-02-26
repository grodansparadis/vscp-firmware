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
* SerialIO.c - ACS Serial I/O Software Implementation
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-04-13	First written.
*******************************************************************************                
*/
#include <string.h>
#include "SerialIO.h"

#define	GLOBALS_OWNER
#include "Globals.h"

#include "TimerRJ2.h"
#include "PhyRS485.h"
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_interface.h"
#include "DataFlashNvm.h"

static BYTE oneSecondTimer, CurrentContact;

static BYTE readExpansionBus0(BYTE address)
{
	BYTE	result;
	
	DISABLE_INTERRUPTS();
	XPD_DATA = address;
	XPD_OUTPUTS();
	ALE_HI(); ALE_LO();
	XPD_INPUTS();
	EXPRD0();
	result = XPD_DATA;
	NONE();
	ENABLE_INTERRUPTS();
	
	return result;
}

static void writeExpansionBus0(BYTE address, BYTE data)
{
	DISABLE_INTERRUPTS();
	XPD_DATA = address;
	XPD_OUTPUTS();
	ALE_HI(); ALE_LO();
	XPD_DATA = data;
	EXPWR0();
	NONE();
	XPD_INPUTS();	
	ENABLE_INTERRUPTS();
}

static BYTE readExpansionBus2(BYTE address)
{
	BYTE	result;
	
	DISABLE_INTERRUPTS();
	XPD_DATA = address;
	XPD_OUTPUTS();
	ALE_HI(); ALE_LO();
	XPD_INPUTS();
	EXPRD1();
	result = XPD_DATA;
	NONE();
	ENABLE_INTERRUPTS();
	
	return result;
}

static void writeExpansionBus2(BYTE address, BYTE data)
{
	DISABLE_INTERRUPTS();
	XPD_DATA = address;
	XPD_OUTPUTS();
	ALE_HI(); ALE_LO();
	XPD_DATA = data;
	EXPWR1();
	NONE();
	XPD_INPUTS();	
	ENABLE_INTERRUPTS();
}

static BYTE readOptoSwitches(void)
{
	BYTE	result;
	
	DISABLE_INTERRUPTS();
	OPTOSW();
	result = XPD_DATA;
	NONE();
	ENABLE_INTERRUPTS();
	
	return result;
}

static void writeRelaysLEDs(BYTE data)
{
	DISABLE_INTERRUPTS();
	XPD_DATA = data;
	XPD_OUTPUTS();
	RLYLED();
	NONE();
	XPD_INPUTS();
	ENABLE_INTERRUPTS();
}

BYTE ReadDipSwitch(void)
{
	BYTE	result;
	
	DISABLE_INTERRUPTS();
	DIPSW();
	result = XPD_DATA;
	NONE();
	ENABLE_INTERRUPTS();
	
	return result;
}

void SetLEDs(BYTE leds)
{
	DISABLE_INTERRUPTS();
	OutputContacts[3] &= ~(RED_LED | GREEN_LED);
	OutputContacts[3] |= leds;
	writeRelaysLEDs(OutputContacts[3]);
	ENABLE_INTERRUPTS();
}

void TimerRJ2_Process(void)
{
	vscp_timer++;

	if (oneSecondTimer)
	{
		oneSecondTimer--;
	}
	
#if 0
	vscp_statuscnt++;
	if ((vscp_initledfunc == VSCP_LED_BLINK1) && (vscp_statuscnt > 5))
	{
		if (OutputContacts[3] & RED_LED)
		{
			OutputContacts[3] &= ~RED_LED;
		}
		else
		{
			OutputContacts[3] |= RED_LED;
		}
		vscp_statuscnt = 0;
	}
	else if (vscp_initledfunc == VSCP_LED_ON)
	{
		OutputContacts[3] |= RED_LED;
		vscp_statuscnt = 0;
	}
	else if (vscp_initledfunc == VSCP_LED_OFF)
	{
		OutputContacts[3] &= ~RED_LED;
		vscp_statuscnt = 0;
	}
#endif
	
	Contacts[0] = readExpansionBus0(0);
	ContactClosures[0] |= ((OlderContacts[0] | OldContacts[0]) ^ Contacts[0]) & Contacts[0];
	ContactOpenings[0] |= ((OlderContacts[0] & OldContacts[0]) ^ Contacts[0]) & ~Contacts[0];
	OlderContacts[0] = OldContacts[0];
	OldContacts[0] = Contacts[0];
	writeExpansionBus0(0, OutputContacts[0]);
	
	Contacts[1] = readExpansionBus0(1);
	ContactClosures[1] |= ((OlderContacts[1] | OldContacts[1]) ^ Contacts[1]) & Contacts[1];
	ContactOpenings[1] |= ((OlderContacts[1] & OldContacts[1]) ^ Contacts[1]) & ~Contacts[1];
	OlderContacts[1] = OldContacts[1];
	OldContacts[1] = Contacts[1];
	writeExpansionBus0(1, OutputContacts[1]);
	
	Contacts[2] = readExpansionBus0(2);
	ContactClosures[2] |= ((OlderContacts[2] | OldContacts[2]) ^ Contacts[2]) & Contacts[2];
	ContactOpenings[2] |= ((OlderContacts[2] & OldContacts[2]) ^ Contacts[2]) & ~Contacts[2];
	OlderContacts[2] = OldContacts[2];
	OldContacts[2] = Contacts[2];
	writeExpansionBus0(2, OutputContacts[2]);

	Contacts[3] = readOptoSwitches();
	ContactClosures[3] |= ((OlderContacts[3] | OldContacts[3]) ^ Contacts[3]) & Contacts[3];
	ContactOpenings[3] |= ((OlderContacts[3] & OldContacts[3]) ^ Contacts[3]) & ~Contacts[3];
	OlderContacts[3] = OldContacts[3];
	OldContacts[3] = Contacts[3];
	writeRelaysLEDs(OutputContacts[3]);

	Contacts[4] = readExpansionBus2(0);
	ContactClosures[4] |= ((OlderContacts[4] | OldContacts[4]) ^ Contacts[4]) & Contacts[4];
	ContactOpenings[4] |= ((OlderContacts[4] & OldContacts[4]) ^ Contacts[4]) & ~Contacts[4];
	OlderContacts[4] = OldContacts[4];
	OldContacts[4] = Contacts[4];
	writeExpansionBus2(0, OutputContacts[4]);
	
	Contacts[5] = readExpansionBus2(1);
	ContactClosures[5] |= ((OlderContacts[5] | OldContacts[5]) ^ Contacts[5]) & Contacts[5];
	ContactOpenings[5] |= ((OlderContacts[5] & OldContacts[5]) ^ Contacts[5]) & ~Contacts[5];
	OlderContacts[5] = OldContacts[5];
	OldContacts[5] = Contacts[5];
	writeExpansionBus2(1, OutputContacts[5]);
	
	Contacts[6] = readExpansionBus2(2);
	ContactClosures[6] |= ((OlderContacts[6] | OldContacts[6]) ^ Contacts[6]) & Contacts[6];
	ContactOpenings[6] |= ((OlderContacts[6] & OldContacts[6]) ^ Contacts[6]) & ~Contacts[6];
	OlderContacts[6] = OldContacts[6];
	OldContacts[6] = Contacts[6];
	writeExpansionBus2(2, OutputContacts[6]);
	
	if (vscp_sendFrameTimer)
	{
		vscp_sendFrameTimer--;
	}
}

void main(void)
{
	unsigned char 	contactOffset, contactMask;
	
	// setup the internal oscillator
	exckcr = 0x00;	// use internal oscillator
	prcr = 0x01;	// unprotect
	ococr = 0x03;	// switch to high speed clock
	sckcr = 0x40;
	phisel = 0x00;	// system clock divider
	ckstpr = 0x80;
	
#ifdef CLOCK_18_432MHz
	
	frv1 = fr18s0;  //@ 0x67  default = 0xCF (!!!! ONLY ON THIS PARTICULAR MICRO !!!!!)
	frv2 = fr18s1;  //@ 0x68  default = 0x5A
	
#endif

	prcr = 0x00;	// protect
	
	/* now initialize all other I/O pins */
	Macro_Init_OtherIO();

	PHY_RS485_Init();
	
	TimerRJ2_Init();
	
	Contacts[0] = readExpansionBus0(0); OldContacts[0] = OlderContacts[0] = Contacts[0];
	Contacts[1] = readExpansionBus0(1); OldContacts[1] = OlderContacts[1] = Contacts[1];
	Contacts[2] = readExpansionBus0(2); OldContacts[2] = OlderContacts[2] = Contacts[2];
	Contacts[3] = readOptoSwitches();   OldContacts[3] = OlderContacts[3] = Contacts[3];
	Contacts[4] = readExpansionBus2(0); OldContacts[4] = OlderContacts[4] = Contacts[4];
	Contacts[5] = readExpansionBus2(1); OldContacts[5] = OlderContacts[5] = Contacts[5];
	Contacts[6] = readExpansionBus2(2); OldContacts[6] = OlderContacts[6] = Contacts[6];
	
	ENABLE_INTERRUPTS();
	
	NVM_Init();
	
	if (!vscp_check_pstorage())
	{
		NVM_Default();
	}
	
	vscp_init();
	
	while (TRUE)
	{
		PHY_RS485_Process();
		
		vscp_imsg.flags = 0;
		vscp_getEvent();
		
		if (oneSecondTimer == 0)
		{
			oneSecondTimer = 50;
			
			vscp_doOneSecondWork();
		}
		
		switch (vscp_node_state)
		{
		case VSCP_STATE_STARTUP:
//			if (vscp_nickname == VSCP_ADDRESS_FREE)
//			{
//				vscp_node_state = VSCP_STATE_INIT;
//			}
//			else
//			{
				vscp_node_state = VSCP_STATE_ACTIVE;
				vscp_setSegmentCRC(0x40);
				vscp_goActiveState();
//			}
			break;
		
//		case VSCP_STATE_INIT:
//			vscp_handleProbeState();
//			break;
			
//		case VSCP_STATE_PREACTIVE:
//			vscp_goActiveState();
//			break;
			
		case VSCP_STATE_ACTIVE:
			if (vscp_imsg.flags & VSCP_VALID_MSG)
			{
				vscp_handleProtocolEvent();
				vscp_doDecisionMatrix();
			}
			break;
			
		case VSCP_STATE_ERROR:
			vscp_error();
			break;
			
		default:
			vscp_node_state = VSCP_STATE_STARTUP;
			break;
		}

		/* application */
		if (vscp_node_state == VSCP_STATE_ACTIVE)
		{
			contactOffset = (CurrentContact >> 3);
			contactMask = (1 << (CurrentContact & 0x07));
			
			if (ContactClosures[contactOffset] & contactMask)
			{
				if (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactClosures[contactOffset])) & contactMask)
				{
					vscp_sendInformationEvent(CurrentContact, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_CLOSED);

					DISABLE_TIMERRJ2_INTERRUPT();
					ContactClosures[contactOffset] &= ~contactMask;
					ENABLE_TIMERRJ2_INTERRUPT();
				}
			}
			
			else if (ContactOpenings[contactOffset] & contactMask)
			{
				if (*NVM_ReadBytes(NVM_OFFSET(NvmReportContactOpenings[contactOffset])) & contactMask)
				{
					vscp_sendInformationEvent(CurrentContact, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OPENED);

					DISABLE_TIMERRJ2_INTERRUPT();
					ContactOpenings[contactOffset] &= ~contactMask;
					ENABLE_TIMERRJ2_INTERRUPT();
				}
			}
			
			CurrentContact++;
			if (CurrentContact >= MAX_CONTACTS)
			{
				CurrentContact = 0;
			}
		}
	}
}

