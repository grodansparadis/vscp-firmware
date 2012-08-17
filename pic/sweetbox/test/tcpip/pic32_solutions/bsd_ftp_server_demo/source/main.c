/*********************************************************************
 *
 *                  Test module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        main.c
 * Processor:       PIC24
 * Complier:        MPLAB C30
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
 * Abdul Rafiq			11/9/2005  Original        (Rev 1.0)
 *
 ********************************************************************/

#include "bsd_ftp_server\ftp.h"
#include "bsd_dhcp_client\dhcp.h"
#include <plib.h>
#include "ex16lcd.h"
#include "pic32mx_cfg.def"

#define VERSION_STR		"1.0"


// You may need to change this definitions depending on your board.
#define ERROR_LED			LATAbits.LATA0
#define ERROR_LED_TRIS		TRISAbits.TRISA0

#define INPROCESS_LED		LATAbits.LATA6
#define INPROCESS_LED_TRIS	TRISAbits.TRISA6
	
//LATA1 and LATA7 are used by tcp app

///////////////////////////////////////////////////////
int main()
{
	IP_ADDR	curr_ip;
	IP_ADDR ip;
	
	// Turn on the interrupts
	INTEnableSystemMultiVectoredInt();

    SYSTEMConfigWaitStatesAndPB(72000000);	
    Ex16LCDInit(36000000);
    Ex16LCDWriteLine(1, "PIC32 BSD FTP");

	TCPIPSetDefaultAddr();
	InitStackMgr();
	TickInit();
	

	// First of all initialize the FAT16 library.
	if ( !FSInit() )
	{
		// If failed to initialize the FAT16, set an error LED
		// Primary reasons for failure would be no SD card detected
		// Or badly formatted SD card.
		ERROR_LED = 1;
		return -1;
	}


	FTPInit();
	DHCPInit();

	curr_ip.Val = 0;

	while(1)
	{

		StackMgrProcess();	
		DHCPTask();
		FTPServer();


		if(curr_ip.Val != (ip.Val = TCPIPGetIPAddr()))
		{
	        BYTE  ip_addr[17];
	        BYTE  number[4];
	        
	        itoa(ip.v[0],number);
	        strcpy(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(ip.v[1],number);
	        strcat(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(ip.v[2],number);
	        strcat(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(ip.v[3],number);
	        strcat(ip_addr, number);
    		
    		Ex16LCDWriteLine(2, ip_addr);
    		
    		curr_ip.Val = ip.Val;
			
		}			
	}
}

