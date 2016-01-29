#include <stdio.h>
#include <string.h>
#include <delays.h>
#include <maths.h>
#include "ownet.h"
#include "pic.h"

// General ROMcommands
#define OW_READROM         (0x33)
#define OW_MATCHROM        (0x55)
#define OW_SKIPROM         (0xCC)
#define OW_SEARCHROM       (0xF0)
#define OW_ALARMSEARCH     (0xEC)
#define OW_ODSKIPROM       (0x3C)
#define OW_ODMATCHROM      (0x69)

// Family 0x10 commands
#define OW_CONVERTT        (0x44)
#define OW_WRITESCRATCHPAD (0x4E)
#define OW_READSCRATCHPAD  (0xBE)
#define OW_COPYSCRATCHPAD  (0x48)
#define OW_RECALLE2        (0xB8)
#define OW_READPOWERSUPPLY (0xB4)

// Family 0x10 ScratchPad definitions
#define SCRATCHPAD_SIZE            (9) // ScratchPad size including CRC
static BYTE ScratchPad[SCRATCHPAD_SIZE];
#define SCRATCHPAD_TEMPERATURELSB  (0) // Measured Temperature Least Significant Byte
#define SCRATCHPAD_TEMPERATUREMSB  (1) // Measured Temperature Most  Significant Byte
#define SCRATCHPAD_ALARMLOW        (2) // Temperature Alarm Low
#define SCRATCHPAD_ALARMHIGH       (3) // Temperature Alarm High
#define SCRATCHPAD_RESERVED0       (4) // Reserved
#define SCRATCHPAD_RESERVED1       (5) // Reserved
#define SCRATCHPAD_COUNTREMAIN     (6) // Count Remain
#define SCRATCHPAD_COUNTPERDEGREE  (7) // Count per Degree
#define SCRATCHPAD_CRC             (8) // Cyclic Redundancy Check

// Serial port definitions
#define    SERIALRATE 38400
const int  BITTIME_OUT=APROCFREQ/SERIALRATE/4;
const BYTE SERIALPORT_OUT=&PORTD;
const BYTE SERIALBIT_OUT=7;
#pragma    callfunction pSerialOut

//----------------------------------------------------------------------
//  Family 0x10 Demo
//
void main(void)
{
  BYTE  portnum;
  BYTE  i;
  int   temp;
  int   sign;

  //----------------------------------------
  ADCON1      = 0x07;     // PortB digital

  bTRD6       = 1;        // RD6 = Serial Input
  bTRD7       = 0;        // RD7 = Serial Output
  bLD7        = 1;

  printf("\nFamily 0x10 Demo\n%");

////////////////////////////////////////////////////////////
// Initialise the OneWire networks (buses)
  for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
    // attempt to acquire the 1-Wire Nets
    if (!owAcquire(portnum, NULL)) {
      printf("Acquire failed for OneWire net %d.\n%", portnum);
      while(1)
        ;
    }
  }
  
////////////////////////////////////////////////////////////
// Let's set the alarmtemps of all devices on all buses.
// To demonstrate this, we write a low alarmtemp of 19
// and a high alarmtemp of 21 to all devices.
// Finally we copy the alarmtemps to non-volatile EEPROM.
// Remark: Although all buses can have their devices
// update their EEPROM at the same time, we do it here
// bus by bus (just for demo-purposes).
  for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
    // Set alarmtemps
    printf("\nStart - Set AlarmTemperatures for OneWire net %d.\n%", portnum);
      // Reset the devices
      owTouchReset(portnum);
      // Address all devices on this net
      owWriteByte(portnum, OW_SKIPROM);
      // Use the WriteScratchpad command to set AlarmTemps
      owWriteByte(portnum, OW_WRITESCRATCHPAD);
      owWriteByte(portnum, 19);  // Valid RoomTemperature
      owWriteByte(portnum, 21);  //   19 <= Temp < 21
    printf("End   - Set AlarmTemperatures for OneWire net %d.\n%", portnum);
    // Set alarmtemps done
    
    // Copy the temps to EEPROM
    printf("\nStart - Write AlarmTemperatures to EEPROM for OneWire net %d.\n%", portnum);
      // Reset the devices
      owTouchReset(portnum);
      // Address all devices on this net
      owWriteByte(portnum, OW_SKIPROM);
      // send the CopyScratchpad command to copy the AlarmTemps to EEPROM
      // If the bus supports Strong PullUp, we use it (because
      // there might be parasite powered devices on this bus).
      if(owHasPowerDelivery(portnum)) {
        printf("This bus has a Strong PullUp\n%");
        owWriteBytePower(portnum, OW_COPYSCRATCHPAD);
        // Eeprom update takes 10 milliseconds max. 11ms is safer
        Wait(11);
        // turn off the strong pull-up
        owLevel(portnum, MODE_NORMAL);
      } else {
        printf("This bus has NO Strong PullUp\n%");
        owWriteByte(portnum, OW_COPYSCRATCHPAD);
        // Eeprom update takes 10 milliseconds max. 11ms is safer
        Wait(11);
      }
    printf("End   - Write AlarmTemperatures to EEPROM for OneWire net %d.\n%", portnum);
    // Copy the temps to EEPROM done
  }
  
  while(1) {
////////////////////////////////////////////////////////////
    // We do a temp conversion on all DS18S20's on all nets simultanously
    printf("\nStart - TemperatureConversions\n%");
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Reset the devices
      owTouchReset(portnum);
      // Address all devices on this net
      owWriteByte(portnum, OW_SKIPROM);
      // Send the convert command and start power delivery if available.
      // Note that we use a different way here to determine
      // Strong PullUp capability.
      if(owWriteBytePower(portnum, OW_CONVERTT)) {
        printf("We are using a Strong PullUp on net %d.\n%", portnum);
      } else {
        printf("No Strong PullUp on net %d, trying without SPU.\n%", portnum);
        owWriteByte(portnum, OW_CONVERTT);
      }
    }
    // Coversion takes 750 milliseconds max. 751ms is safer
    Wait(751);
    // turn off the 1-Wire Net strong pull-up
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      owLevel(portnum, MODE_NORMAL);
    }
    printf("End - TemperatureConversions\n%");

////////////////////////////////////////////////////////////
    // We will now demonstrate an alarmsearch
    printf("\nStart - Find devices in alarmstate\n%");
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Find the devices with temp < 19 or temp >= 21
      printf("\nAlarm Enumeration for net %d.\n%", portnum);
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
    printf("End   - Find devices in alarmstate\n%");

////////////////////////////////////////////////////////////
    // We will now demonstrate a normal search. For
    // every device found, it's temperature is retrieved.
    printf("\nStart - Find devices and get their temperatures.\n%");
    for(portnum = 0; portnum < MAX_PORTNUM; portnum++) {
      // Start the search for net portnum
      printf("\nEnumeration for net %d.\n%", portnum);
      if(!owFirst(portnum, TRUE, FALSE)) {
        printf("No devices on this net.\n%");
      } else {
        do {
          printf("Device found: 0x%02X%02X%02X%02X%02X%02X%02X%02X%",
                 owNetCurrent.SerialNum[7],
                 owNetCurrent.SerialNum[6],
                 owNetCurrent.SerialNum[5],
                 owNetCurrent.SerialNum[4],
                 owNetCurrent.SerialNum[3],
                 owNetCurrent.SerialNum[2],
                 owNetCurrent.SerialNum[1],
                 owNetCurrent.SerialNum[0]);
          // Init the crc
          setcrc8(portnum, 0);
          // Read the device's memory
          owWriteByte(portnum, OW_READSCRATCHPAD);
          for(i = 0; i < SCRATCHPAD_SIZE; i++) {
            ScratchPad[i] = owReadByte(portnum);
            docrc8(portnum, ScratchPad[i]);
          }
          // Check the CRC
          if(owNetCurrent.utilcrc8 != 0) {
            printf(", crc is NOT OK.\n%");          
          } else {
            // We don't want to use float's
            temp = ((int)ScratchPad[SCRATCHPAD_TEMPERATUREMSB] << 8) |
                    (int)ScratchPad[SCRATCHPAD_TEMPERATURELSB];
            if(temp < 0) {
              temp = -temp;
              sign = '-';
            } else if(temp == 0) {
              sign = ' ';
            } else {
              sign = '+';
            }
            printf(", Temperature is %c%d.%d degrees.\n%",
                   sign,
                   (temp >> 1),
                   (temp & 0x01) ? 5 : 0);
          }
        } while(owNext(portnum, TRUE, FALSE));
      }
    }   
    printf("End   - Find devices and get their temperatures.\n%");
     
////////////////////////////////////////////////////////////
    Wait(1);  // Just a convenient way to set a breakpoint
  }
}
