#include <stdio.h>
#include <string.h>
#include <delays.h>
#include "ownet.h"
#include "pic.h"

// Serial port definitions
#define    SERIALRATE 38400
const int  BITTIME_OUT=APROCFREQ/SERIALRATE/4;
const BYTE SERIALPORT_OUT=&PORTD;
const BYTE SERIALBIT_OUT=7;
#pragma    callfunction pSerialOut

// Local prototypes
static void FindFamily(BYTE portnum, BYTE family);

//----------------------------------------------------------------------
//  Main SearchFamily Test
//
void main(void)
{
  BYTE portnum;

  //----------------------------------------
  ADCON1      = 0x07;     // PortB digital

  bTRD6       = 1;        // RD6 = Serial Input
  bTRD7       = 0;        // RD7 = Serial Output
  bLD7        = 1;

  printf("\nEnumeration (FamilySearch) test\n%");

  // Initialise the OneWire nets
  for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
    // attempt to acquire the 1-Wire Nets
    if (!owAcquire(portnum,NULL)) {
      printf("Acquire failed for OneWire net %d.\n%", portnum);
    }
  }
   
  while(1) {
    // Search the nets
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Find the device(s) for family 0x01
      FindFamily(portnum, 0x01);
      // Find the device(s) for family 0x10
      FindFamily(portnum, 0x10);
    }   
     
    Wait(10);  // Just a convenient way to set a breakpoint
  }
}

void FindFamily(BYTE portnum, BYTE family)
{
  printf("\nEnumeration of FamilyCode 0x%02X for net %d.\n%",
         family, portnum);
  owFamilySearchSetup(portnum, family);
  while(owNext(portnum, TRUE, FALSE)) {
    if(owNetCurrent.SerialNum[0] != family) {
      break;
    }
      printf("Device found: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n%",
             owNetCurrent.SerialNum[7],
             owNetCurrent.SerialNum[6],
             owNetCurrent.SerialNum[5],
             owNetCurrent.SerialNum[4],
             owNetCurrent.SerialNum[3],
             owNetCurrent.SerialNum[2],
             owNetCurrent.SerialNum[1],
             owNetCurrent.SerialNum[0]);
  }
}

