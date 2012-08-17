/*********************************************************************
 *
 *                  Test module for Microchip HTTP Server
 *
 *********************************************************************
 * FileName:        main.c
 * Processor:       PIC32
 * Complier:        MPLAB C32
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
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
 * Author               Date      Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Abdul Rafiq			8/20/06		Demo to exersize minimal dhcp server 
 *
 ********************************************************************/
#include <plib.h>
#include "bsd_dhcp_client\dhcp.h"

//Config Fuses for 72Mhz Core and 36Mhz Peripheral
#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF, WDTPS = PS512
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2, DEBUG = OFF, IESO = OFF, FCKSM = CSDCMD, FSOSCEN = OFF


void SetupDefault_IP_MAC()
{
	// Initialize default IP and MAC address
	IP_StrToDWord( DEFAULT_IP_ADDR, 	&SysIPAddr.Val );	
	IP_StrToDWord( DEFAULT_IP_MASK, 	&SysIPMask.Val );	
	IP_StrToDWord( DEFAULT_IP_GATEWAY, 	&SysIPGateway.Val );	
	MAC_StrToBin ( DEFAULT_MAC_ADDR,	SysMACAddr.v );  
  
}

void DHCPServerTask(void);

///////////////////////////////////////////////////////
int main()
{
    SYSTEMConfigWaitStatesAndPB(72000000);	
	// Turn on the interrupts
	INTEnableSystemMultiVectoredInt();
	
	/* enable cacheability for KSEG0 */
	CheKseg0CacheOn();
	mBMXDisableDRMWaitState();

	/* configure the cache for prefetch and 2 wait-state operation */
	mCheConfigure(CHE_CONF_WS2 | CHE_CONF_PF_C);
	
    // Initialize the TCP/IP
	SetupDefault_IP_MAC();
	MSTimerInit(36000000);
	InitStackMgr();
	TickInit();
	DDPCON = 0;
	mPORTASetPinsDigitalOut(BIT_2);	//LED Blink on Explorer 16
	int LEDCntr = 0;
	
	while(1)
	{
		if( (LEDCntr++ % 3000) == 0 )	//Blink LED
			mPORTAToggleBits(BIT_2);

		StackMgrProcess();
		DHCPServerTask();		
	}
	
    return 0;
}
