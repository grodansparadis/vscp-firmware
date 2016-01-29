#include <stdio.h>
#include <string.h>
#include <delays.h>
#include "ownet.h"
#include "pic.h"

#define OW_SKIPROM         0xCC
#define OW_CONVERTT        0x44
#define OW_WRITESCRATCHPAD 0x4E

// Serial port definitions
#define    SERIALRATE      38400
const int  BITTIME_OUT    = APROCFREQ/SERIALRATE/4;
const BYTE SERIALPORT_OUT = &PORTD;
const BYTE SERIALBIT_OUT  = 7;
#pragma    callfunction pSerialOut

//----------------------------------------------------------------------
//  Main AlarmSearch Test
//
void main(void)
{
  BYTE portnum;

  //----------------------------------------
  ADCON1      = 0x07;     // PortB digital

  bTRD6       = 1;        // RD6 = Serial Input
  bTRD7       = 0;        // RD7 = Serial Output
  bLD7        = 1;

  printf("\nEnumeration (AlarmSearch) test\n%");

// Initialise the OneWire nets and the temperature devices
  for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
    // attempt to acquire the 1-Wire Nets
    if (!owAcquire(portnum,NULL)) {
      printf("Acquire failed for OneWire net %d.\n%", portnum);
    } else {
      // Reset the devices
      owTouchReset(portnum);
      // Address all devices on this net
      owWriteByte(portnum, OW_SKIPROM);
      // send the WriteScratchpad command to set AlarmTemps
      owWriteByte(portnum, OW_WRITESCRATCHPAD);
      owWriteByte(portnum, 19);  // Valid RoomTemperature
      owWriteByte(portnum, 21);  //   19 <= Temp < 21
    }
  }
  
  while(1) {
    // Do a temp conversion on all DS18S20's on all nets simultanously
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Reset the devices
      owTouchReset(portnum);
      // Address all devices on this net
      owWriteByte(portnum, OW_SKIPROM);
      // send the convert command and start power delivery
      if(!owWriteBytePower(portnum, OW_CONVERTT)) {
        printf("\nNo Strong PullUp on net %d. Trying without SPU.\n%", portnum);
        owWriteByte(portnum, OW_CONVERTT);
      }
    }

    // Conversion takes 750 milliseconds max. Safer is 751ms
    Wait(751);
    
    // turn off the 1-Wire Net strong pull-up
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      owLevel(portnum, MODE_NORMAL);
    }
    
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Find the device(s) in AlarmState
      printf("\nEnumeration for net %d.\n%", portnum);
      if(!owFirst(portnum, TRUE, TRUE)) {
        printf("No devices in AlarmState found on this net.\n%");
      } else {
        do {
          printf("Device in AlarmState found: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n%",
                 owNetCurrent.SerialNum[7],
                 owNetCurrent.SerialNum[6],
                 owNetCurrent.SerialNum[5],
                 owNetCurrent.SerialNum[4],
                 owNetCurrent.SerialNum[3],
                 owNetCurrent.SerialNum[2],
                 owNetCurrent.SerialNum[1],
                 owNetCurrent.SerialNum[0]);
        } while(owNext(portnum, TRUE, TRUE));
      }
    }   
     
    Wait(10);  // Just a convenient way to set a breakpoint
  }
}
