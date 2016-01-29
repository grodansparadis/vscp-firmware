/*
* THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*/
#include <pic18.h>
#include <string.h>
#include "delay_ds.h"
#include "ds1820.h"

//Macro for setting port Low and High
#define OW_LOW {DSPORT &= ~DS_BIT;TRISC &= ~DS_BIT;}
#define OW_HIGH {TRISC |= DS_BIT;}
    
   
/* **************************************************************** */
void convert(char *temp_buff)
{
    char i;

	ow_init();
	ow_out_byte(SKIP_ROM);
	ow_out_byte(CONVERT);

	DelayMs_ds(254);
	DelayMs_ds(100);

	ow_init();
	ow_out_byte(SKIP_ROM);
	ow_out_byte(READSCRATCH);

	for (i=0; i<9; i++)
   	{
  		temp_buff[i]=ow_in_byte();	//Read data and put in buffer
	}
}

/* **************************************************************** */
// The following are standard 1-Wire routines.
void ow_init(void)
{
   	
   	OW_LOW;

  	Delayus_ds(255);	//Drive Low for 500uS
   	Delayus_ds(255);
	
   	OW_HIGH;
	
	//TO DO need to detect Presence Pulse here
	
 	Delayus_ds(255);
	Delayus_ds(255);

}

/* **************************************************************** */
char ow_in_byte(void)
{
	char i, in_char;
	char temp;    
    
    di();   //Disable interrupt
    
    for (i=0; i<8; i++)
    {
        OW_LOW;
        Delayus_ds(2);
	    OW_HIGH;
        Delayus_ds(3);
        temp = DSPORT & DS_BIT;
    
        if (temp)
        {
            in_char=(in_char>>1) | 0x80;	// LSB first
        }
        else
        {
            in_char = in_char >> 1;
        }
        Delayus_ds(80);
    }

	ei();   //Enable interrupt
	
    return(in_char);
}

/* **************************************************************** */
void ow_out_byte(unsigned char out)
{
   unsigned char i;
    
    di();   //Disable interrupt
    
    for(i=0; i<8; i++)
    {
        if (out & 0x01)	
        { //Write a 1
            OW_LOW;
	        Delayus_ds(2);
            OW_HIGH;
   	        Delayus_ds(80);
        }
        else
        {	//Write a 0
            OW_LOW;
            Delayus_ds(80);
            OW_HIGH;
        }
        out = out >> 1;
    }
	
	ei();   //Enable interrupt
}

