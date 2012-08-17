/*********************************************************************
 *
 *                  Test module for Microchip HTTP Server
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
 * 
 *
 ********************************************************************/
#include <p32xxxx.h>
#include <plib.h>
//#include <bsd_http_server\http.h>
//#include "ex16lcd.h"
//#include "pic32mx_cfg.def"		// config fuse settings
#include "eTCP.def"

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 10 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//

// * * * Hardware * * *

#define SYSCLK  (80000000)
#define PBCLK   (SYSCLK/8)

// I2C data rate
#define Fsck	100000              // I2C busspeed
#define BRG_VAL ((PBCLK/2/Fsck)-2)  // I2C channel config value

/********************************************************************
 * Oscillator Selection bits [FNOSC]
 *
 * FRC      - Fast RC oscillator  
 * FRCPLL   - Fast RC oscillator w/ PLL  
 * PRI      - Primary oscillator (XT, HS, EC)  
 * PRIPLL   - Primary oscillator (XT, HS, EC) w/ PLL  
 * SOSC     - Secondary oscillator  
 * LPRC     - Low power RC oscillator  
 * FRCDIV16 - Fast RC oscillator with divide by 16  
 * FRCDIV   - Fast RC oscillator with divide  
 ********************************************************************/
#pragma config FNOSC = PRIPLL

/********************************************************************
 * PLL Input Divider [FPLLIDIV]
 * The PLL need to have a value between 4 - 5 MHz.  Use this
 * setting to scale down your input frequency.
 * 
 * DIV_1    - Divide by 1  
 * DIV_2    - Divide by 2  
 * DIV_3    - Divide by 3  
 * DIV_4    - Divide by 4  
 * DIV_5    - Divide by 5  
 * DIV_6    - Divide by 6  
 * DIV_10   - Divide by 10  
 * DIV_12   - Divide by 12  
 ********************************************************************/
#pragma config FPLLIDIV = DIV_2

/********************************************************************
 * PLL Multipler [FPLLMUL]
 * 
 * MUL_15 - Multiply by 15  
 * MUL_16 - Multiply by 16  
 * MUL_17 - Multiply by 17  
 * MUL_18 - Multiply by 18  
 * MUL_19 - Multiply by 19  
 * MUL_20 - Multiply by 20  
 * MUL_21 - Multiply by 21  
 * MUL_24 - Multiply by 24  
 ********************************************************************/
#pragma config FPLLMUL = MUL_20

/********************************************************************
 * PLL Output Divider [FPLLODIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 * DIV_16 - Divide by 16  
 * DIV_32 - Divide by 32  
 * DIV_64 - Divide by 64  
 * DIV_256 - Divide by 256  
 ********************************************************************/
#pragma config FPLLODIV = DIV_1

/********************************************************************
 * Bootup Peripheral Bus Clock Divider [FPBDIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 ********************************************************************/
#pragma config FPBDIV = DIV_8

/********************************************************************
 * Primary Oscillator [POSCMOD]
 * 
 * EC - EC oscillator  
 * XT - XT oscillator  
 * HS - HS oscillator  
 * OFF - Disabled  
 ********************************************************************/
#pragma config POSCMOD = HS

/********************************************************************
 * Secondary Oscillator Enable [FSOSCEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FSOSCEN = OFF

/********************************************************************
 * Internal/External Switch Over [IESO]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config IESO = OFF

/********************************************************************
 * Clock Switching and Monitor Selection [FCKSM]
 * 
 * CSECME - Clock Switching Enabled, Clock Monitoring Enabled  
 * CSECMD - Clock Switching Enabled, Clock Monitoring Disabled  
 * CSDCMD - Clock Switching Disabled, Clock Monitoring Disabled  
 ********************************************************************/
#pragma config FCKSM = CSDCMD

/********************************************************************
 * Clock Output Enable [OSCIOFNC]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config OSCIOFNC = OFF

/********************************************************************
 * Watchdog Timer Enable [FWDTEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FWDTEN = OFF

/********************************************************************
 * Watchdog Timer Postscale Select [WDTPS]
 * 
 * PS1         - 1:1  
 * PS2         - 1:2  
 * PS4         - 1:4  
 * PS8         - 1:8  
 * PS16        - 1:16  
 * PS32        - 1:32  
 * PS64        - 1:64  
 * PS128       - 1:128  
 * PS256       - 1:256  
 * PS512       - 1:512  
 * PS1024      - 1:1024  
 * PS2048      - 1:2048  
 * PS4096      - 1:4096  
 * PS8192      - 1:8,192  
 * PS16384     - 1:16,384  
 * PS32768     - 1:32,768  
 * PS65536     - 1:65,536  
 * PS131072    - 1:131,072  
 * PS262144    - 1:262,144  
 * PS524288    - 1:524,288  
 * PS1048576   - 1:1,048,576  
 ********************************************************************/
#pragma config WDTPS = PS1048576

/********************************************************************
 * Boot Flash Protect Enable [BWP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config BWP = OFF

/********************************************************************
 * Code Protect Enable [CP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config CP = OFF

/********************************************************************
 * ICE/ICD Comm Channel Select [ICESEL]
 * 
 * ICS_PGx1 - ICE pins are shared with PGC1, PGD1  
 * ICS_PGx2 - ICE pins are shared with PGC2, PGD2  
 ********************************************************************/
#pragma config ICESEL = ICS_PGx2


// The code has been created to allow for communication to the 
// ENC28J60 by either SPI 1 or SPI 2.  The user needs to define 
// which SPI port that will use.
// SPI 1
#define ETHERNET_USE_SPI_1
// SPI 2
//#define ETHERNET_USE_SPI_2

// SPI Open Config setting (see spi.h)
#define ETHERNET_SPI_OPEN_CFG_1		( PRI_PRESCAL_4_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON | SPI_MODE8_ON  )
#define ETHERNET_SPI_OPEN_CFG_2		( SPI_ENABLE )

// SPI baud rate.  Make sure that the baud rate is within ENC28J60 spec. (recommended < 20MHz)) and SPI.
#define ETHERNET_IP_SPI_BRG      	( 10 )

// The device is configured with a default IP and MAC address.
// If the project uses a DHCP client, than the IP address will
// be assigned by the DCHP server.  
#define DEFAULT_IP_ADDR				"192.168.1.201"
#define DEFAULT_IP_GATEWAY			"192.168.1.255"
#define DEFAULT_IP_MASK				"255.255.255.0"
#define DEFAULT_MAC_ADDR			"00-04-a3-00-00-02"


// The clock frequency that your device is running at.  Please
// refer to the configuration bit settings to figure the frequency out
#define CLOCK_FREQ					80000000

// For a B1 errata for the ENC28J60, you will need to provide 
// a transmitt collision timeout.  It is recommmended that it be
// somewhere between .5 and .20 times the TCICKS_PER_SECOND.
//  For more informaiton about this errata please refer to the
// ENC28J60 errata documentation
#define TX_COLLSION_TIMEOUT			((WORD)(TICKS_PER_SECOND * .15))


// Chip Select (Pin 5)

// Direction
#define mSetupEthernetCSDirection()     {   mPORTESetBits( BIT_2 ); mPORTESetPinsDigitalOut( BIT_2 );  }

// CS on, active low
#define mSetEthernetCSOn()              {   mPORTEClearBits( BIT_2 ); }

// CS off, active low
#define mSetEthernetCSOff()             {   mPORTESetBits( BIT_2 );   }



// Reset (Pin 6)
// Direction
#define mSetupEthernetResetDirection()  {   mPORTESetBits( BIT_3 ); mPORTESetPinsDigitalOut( BIT_3 );  }

// Reset on, active low
#define mSetEthernetResetOn()           {   mPORTEClearBits( BIT_3 ); }

// Reset off, active low
#define mSetEthernetResetOff()          {   mPORTESetBits( BIT_3 );   }




// Set up the direction for Wake on LAN
#define mSetupEthernetWOLDirection()    {   mPORTBSetPinsDigitalIn( BIT_15 );  }




// INT (pin 28)

// Set up the direction for ethernet interrupt
#define mSetupEthernetINTDirection()    {   mPORTASetPinsDigitalIn( BIT_14 );  }




///////////////////////////////////////////////////////
int main()
{
	//IP_ADDR	curr_ip;
	//IP_ADDR ip;
	unsigned char rv;
	
	// Turn on the interrupts
	INTEnableSystemMultiVectoredInt();

    SYSTEMConfigPerformance(80000000);	
    //Ex16LCDInit(36000000);
    //Ex16LCDWriteLine(1, "PIC32 BSD HTTP");
    
    mSetupEthernetCSDirection();
    mSetupEthernetResetDirection();
    
    OpenSPI1(( PRI_PRESCAL_4_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON | SPI_MODE8_ON  ), SPI_ENABLE);
    mSetEthernetCSOn();
    putcSPI1( 0x1d );
    rv = getcSPI1();
    putcSPI1( 0x00 );
    rv = getcSPI1();
    mSetEthernetCSOff();
    
    //MSTimerInit(40000000);
    
/*    
    MACInit();

    // Initialize the TCP/IP
	TCPIPSetDefaultAddr();
	InitStackMgr();
	TickInit();

	// Initialize the FAT16 library.
	if ( !FSInit() )
	{
		// If failed to initialize the FAT16, set an error LED
		// Primary reasons for failure would be no SD card detected
		// Or badly formatted SD card.
		return -1;
	}
	
	if(!HTTPInit())
		return -1;
*/	
		
	//DHCPInit();
/*	
	curr_ip.Val = 0;
	
	while(1)
	{
		StackMgrProcess();
		HTTPServer();	
		DHCPTask();
		
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
*/	
    return 0;

}
