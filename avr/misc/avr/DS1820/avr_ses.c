/***********************************************************************************
Project:          Dallas 1-wire program
Filename:         avr_ses.c    

Processor:        AVR family
Compiler:         IAR Version 1.50C
  
Author:               SH
Company:         ATMEL Norway     
***********************************************************************************/
#include "avr_lnk.h"



// local function prototypes
SMALLINT owAcquire(int,char *);
void     owRelease(int);

//---------------------------------------------------------------------------
// Attempt to acquire a 1-Wire net
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'port_zstr'  - zero terminated port name.
//
// Returns: TRUE - success, port opened
//
SMALLINT owAcquire(int portnum, char *port_zstr)
{
	port_zstr = 0;
	owLevel(portnum, MODE_STRONG5);// drive bus high.

	usDelay(500); // give time for line to settle
	owLevel(portnum, MODE_NORMAL);

   // checks to make sure the line is idling high.
   return (ow_PinStatus()==1?TRUE:FALSE);
}

//---------------------------------------------------------------------------
// Release the previously acquired a 1-Wire net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
void owRelease(int portnum)
{
   portnum = 0;
}


