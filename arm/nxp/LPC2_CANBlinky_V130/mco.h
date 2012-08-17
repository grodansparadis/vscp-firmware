/**************************************************************************
MODULE:    MCO
CONTAINS:  MicroCANopen implementation
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2004.
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
           www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
           FOR KEIL SOFTWARE www.keil.com
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
VERSION:   1.21, Pf 02-FEB-04
---------------------------------------------------------------------------
HISTORY:   1.21, Pf 02-FEB-04, Release for Philips LPC2129
           1.20, Pf 19-AUG-03, Code changed to use process image
           1.10, Pf 27-MAY-03, Bug fixes in OD (hi byte was corrupted)
                 OD changed to indefinite length
				 Support of define controled MEMORY types
           1.01, Pf 17-DEC-02, Made Object Dictionary more readable
           1.00, Pf 07-OCT-02, First Published Version
***************************************************************************/ 

#ifndef _MCO_H
#define _MCO_H

/**************************************************************************
DEFINES: MEMORY TYPES USED
**************************************************************************/
// CONST Object Dictionary Data
#define MEM_CONST const
// Process data and frequently used variables
#define MEM_NEAR
// Seldomly used variables
#define MEM_FAR

/**************************************************************************
DEFINES: CONST ENTRIES IN OBJECT DICTIONARY
Modify these for your application
**************************************************************************/

#define OD_DEVICE_TYPE   0x000F0191L 
#define OD_VENDOR_ID     0x00455341L
#define OD_PRODUCT_CODE  0x00010002L
#define OD_REVISION      0x00010020L

// The following are optional and can also be left "undefined"
#define OD_SERIAL        0xFFFFFFFFL

/**************************************************************************
DEFINES: Definition of the process image
Modify these for your application
**************************************************************************/

// Define the size of the process image
#define PROCIMG_SIZE 16

// Define process variables: offsets into the process image 
// Digital Input 1
#define IN_digi_1 0x00
// Digital Input 2
#define IN_digi_2 0x01
// Digital Input 3
#define IN_digi_3 0x02
// Digital Input 4
#define IN_digi_4 0x03

// Analog Input 1
#define IN_ana_1 0x04
// Analog Input 2
#define IN_ana_2 0x06

// Digital Output 1
#define OUT_digi_1 0x08
// Digital Output 2
#define OUT_digi_2 0x09
// Digital Output 3
#define OUT_digi_3 0x0A
// Digital Output 4
#define OUT_digi_4 0x0B

// Analog Output 1
#define OUT_ana_1 0x0C
// Analog Output 2
#define OUT_ana_2 0x0E


/**************************************************************************
DEFINES: ENABLING/DISABLING CODE FUNCTIONALITY
**************************************************************************/

// If defined, both CAN interfaces 1 + 2 are used:
// transmit on 2, receive on 1
#define LPCDemoMode

// Maximum number of transmit PDOs (0 to 4)
#define NR_OF_TPDOS 2

// Maximum number of receive PDOs (0 to 4)
#define NR_OF_RPDOS 2

// If defined, all parameters passed to functions are checked for consistency. 
// On failures, the user function MCOUSER_FatalError is called.
#define CHECK_PARAMETERS


/**************************************************************************
END OF CUSTOMIZATION AREA - DON'T CHANGE ANYTHING BELOW
**************************************************************************/

#if (NR_OF_RPDOS == 0)
  #if (NR_OF_TPDOS == 0)
ERROR: At least one PDO must be defined!
  #endif
#endif

#if ((NR_OF_TPDOS > 4) || (NR_OF_RPDOS > 4))
ERROR: This configuration was never tested by ESAcademy!
#endif

/**************************************************************************
GLOBAL TYPE DEFINITIONS
**************************************************************************/

// Standard data types
#define BYTE  unsigned char
#define WORD  unsigned short
#define DWORD unsigned int

// Boolean expressions
#define BOOLEAN unsigned char
#define TRUE 0xFF
#define FALSE 0

// Data structure for a single CAN message 
typedef struct
{
  BYTE BUF[8];              // Data buffer 
  WORD ID;                  // Message Identifier 
  BYTE LEN;                 // Data length (0-8) 
} CAN_MSG;

// This structure holds all node specific configuration
typedef struct
{
  CAN_MSG heartbeat_msg;    // Heartbeat message contents
  BYTE Node_ID;             // Current Node ID (1-126)
  BYTE error_code;          // Bits: 0=RxQueue 1=TxQueue 3=CAN
  WORD Baudrate;            // Current Baud rate in kbit
  WORD heartbeat_time;      // Heartbeat time in ms
  WORD heartbeat_timestamp; // Timestamp of last heartbeat
  BYTE error_register;      // Error regiter for OD entry [1001,00]
} MCO_CONFIG;

// This structure holds all the TPDO configuration data for one TPDO
typedef struct 
{
  CAN_MSG CAN;              // Current/last CAN message to be transmitted
  WORD event_time;          // Event timer in ms (0 for COS only operation)
  WORD event_timestamp;     // If event timer is used, this is the 
                            // timestamp for the next transmission
  BYTE offset;              // Offest to application data in process image
} TPDO_CONFIG;

// This structure holds all the RPDO configuration data for one RPDO
typedef struct 
{
  WORD CANID;               // Message Identifier 
  BYTE len;                 // Data length (0-8) 
  BYTE offset;              // Pointer to destination of data 
} RPDO_CONFIG;


/**************************************************************************
GLOBAL FUNCTIONS
**************************************************************************/

/**************************************************************************
DOES: This function initializes the CANopen protocol stack.
      It must be called from within MCOUSER_ResetApplication.
**************************************************************************/
void MCO_Init 
  (
  WORD Baudrate,  // CAN baudrate in kbit(1000,800,500,250,125,50,25 or 10)
  BYTE Node_ID,   // CANopen node ID (1-126)
  WORD Heartbeat  // Heartbeat time in ms (0 for none)
  );


/**************************************************************************
DOES: This function initializes a transmit PDO. Once initialized, the 
      MicroCANopen stack automatically handles transmitting the PDO.
      The application can directly change the data at any time.
NOTE: For data consistency, the application should not write to the data
      while function MCO_ProcessStack executes.
**************************************************************************/
void MCO_InitTPDO
  (
  BYTE PDO_NR,       // TPDO number (1-4)
  WORD CAN_ID,       // CAN identifier to be used (set to 0 to use default)
  WORD event_tim,    // Transmitted every event_tim ms 
                     // (set to 0 if ONLY inhibit_tim should be used)
  WORD inhibit_tim,  // Inhibit time in ms for change-of-state transmit
                     // (set to 0 if ONLY event_tim should be used)
  BYTE len,          // Number of data bytes in TPDO
  BYTE offset        // Offset to data location in process image
  );


/**************************************************************************
DOES: This function initializes a receive PDO. Once initialized, the 
      MicroCANopen stack automatically updates the data at offset.
NOTE: For data consistency, the application should not read the data
      while function MCO_ProcessStack executes.
**************************************************************************/
void MCO_InitRPDO
  (
  BYTE PDO_NR,       // RPDO number (1-4)
  WORD CAN_ID,       // CAN identifier to be used (set to 0 to use default)
  BYTE len,          // Number of data bytes in RPDO
  BYTE offset        // Offset to data location in process image
  );


/**************************************************************************
DOES: This function implements the main MicroCANopen protocol stack. 
      It must be called frequently to ensure proper operation of the
      communication stack. 
      Typically it is called from the while(1) loop in main.
**************************************************************************/
BYTE MCO_ProcessStack
  ( // Returns 0 if nothing needed to be done
    // Returns 1 if a CAN message was received or sent
  void
  );


/**************************************************************************
USER CALL-BACK FUNCTIONS
These must be implemented by the application.
**************************************************************************/

/**************************************************************************
DOES: This function resets the application. It is called from within the
      CANopen protocol stack, if a NMT master message was received that
      demanded "Reset Application".
**************************************************************************/
void MCOUSER_ResetApplication
  (
  void
  );


/**************************************************************************
DOES: This function both resets and initializes both the CAN interface
      and the CANopen protocol stack. It is called from within the
      CANopen protocol stack, if a NMT master message was received that
      demanded "Reset Communication".
      This function should call MCO_Init and MCO_InitTPDO/MCO_InitRPDO.
**************************************************************************/
void MCOUSER_ResetCommunication
  (
  void
  );


/**************************************************************************
DOES: This function is called if a fatal error occurred. 
      Error codes of mcohwxxx.c are in the range of 0x8000 to 0x87FF.
      Error codes of mco.c are in the range of 0x8800 to 0x8FFF. 
      All other error codes may be used by the application.
**************************************************************************/
void MCOUSER_FatalError
  (
  WORD ErrCode // To debug, search source code for the ErrCode encountered
  );

#endif
