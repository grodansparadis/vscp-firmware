/*********************************************************************
 *
 *       Emulating Data EEPROM for PIC32 microcontrollers
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
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
 *$Id: main.c 274 2008-12-28 20:25:01Z akhe $
 *
 **********************************************************************/
/*
 * This application is intended to design to show the usage and working of 
 * Data EEPROM emulation library. Here is a bunch of data writing and reading  
 * from the emulated pages using the DEE APIs. Starter Kit is used as a hardware  
 * to demonstrate the usage. The Red LED will glow in case of any data error and 
 * Orange LED in the case of read/write error and Green LED upon success.
 */
#include <dee_emulation/dee_emulation_pic32.h>
#include <plib.h>

#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2

#define RED_LED	   (BIT_0) // data mismatch
#define ORANGE_LED (BIT_1) // read/write error
#define GREEN_LED  (BIT_2) // success

int main(void)
{
    int value;
	int i;
    int DEEdata=0;
    unsigned short DEEaddr=0;
	unsigned char writeArray[5]={15,16,17,18,19};
	unsigned char readArray[5];

	mPORTDClearBits(RED_LED);
    mPORTDClearBits(ORANGE_LED);
	mPORTDClearBits(GREEN_LED);
	mPORTDSetPinsDigitalOut(RED_LED);
    mPORTDSetPinsDigitalOut(ORANGE_LED);
	mPORTDSetPinsDigitalOut(GREEN_LED);

    if(DataEEInit()) // Call initialization function before any Dee related functions. 
    {
        mPORTDSetBits(ORANGE_LED); // Error occured
        return (0);
    }

    for(i=0; i<5; i++)
    {
       if(DataEEWrite(DEEdata,DEEaddr))
       {
          mPORTDSetBits(ORANGE_LED); // Error occured
          return 0;
       }
       DEEdata += 0x6BCA1A;
       DEEaddr++;
    }
	
    DEEaddr=0;
    DEEdata= 0;

    for(i=0; i<5; i++)
    {
       if(DataEERead(&value, DEEaddr))
       {
         mPORTDSetBits(ORANGE_LED); // Error occured
         return (0);
       }
       if(value != DEEdata)
       {
		  mPORTDSetBits(RED_LED); // Error occured
          return(0);// should never hit here
       }
       DEEaddr++;
       DEEdata += 0x6BCA1A;
    }
	
   // Test for Read/Write array

	if(DataEEWriteArray(writeArray,25,5))
	{
	   mPORTDSetBits(RED_LED); // Error occured
       return (0); // should never hit here
	}
    if(DataEEReadArray(readArray,25,5))
	{
	   mPORTDSetBits(RED_LED); // Error occured
       return (0); // should never hit here
    }

	mPORTDSetBits(GREEN_LED); //Success condition
	
	while(1);
    return(0);
}

