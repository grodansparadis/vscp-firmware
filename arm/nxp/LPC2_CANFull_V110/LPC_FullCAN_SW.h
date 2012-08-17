/**************************************************************************
MODULE:    LPC_FullCAN_SW
CONTAINS:  Philips LPC2000 CAN interface driver.
           The first revisions of the Philips LPC2000 with CAN interface
           do NOT support the FullCAN mode in hardware. This driver 
           implements the FullCAN mode in software.
		       Compiled and Tested with Keil Tools www.keil.com
COPYRIGHT: Embedded Systems Academy, Inc. 2004.
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
           FOR KEIL SOFTWARE www.keil.com
VERSION:   1.01, Pf 19-APR-04
---------------------------------------------------------------------------
HISTORY:   1.01, Pf 18-APR-04, Added Timer and LED output
HISTORY:   1.00, Pf 16-APR-04, First published release
***************************************************************************/ 

/**************************************************************************
USER DEFINABLE PARAMETERS
***************************************************************************/ 

// Maximum number of CAN interfaces supported by this driver (1 to 4)
// So far values 3 and 4 were not tested
// The example code in "main" uses CAN ports 1 and 2 (MAX_CANPORTS 2)
#define MAX_CANPORTS 2

// Maximum number of total FullCAN Filters for ALL CAN interfaces
#define MAX_FILTERS 20


/**************************************************************************
DO NOT CHANGE ANYTHING BELOW
***************************************************************************/ 

// Define CAN SFR address bases 
#define CAN_REG_BASE                    (0xE0000000)
#define ACCEPTANCE_FILTER_RAM_BASE      (CAN_REG_BASE + 0x00038000)
#define ACCEPTANCE_FILTER_REGISTER_BASE (CAN_REG_BASE + 0x0003C000)
#define CENTRAL_CAN_REGISTER_BASE       (CAN_REG_BASE + 0x00040000)              

// Common CAN bit rates
#define   CANBitrate125k_12MHz          0x001C001D
#define   CANBitrate250k_12MHz          0x001C000E

// CAN Interrupt Service Routines
void FullCAN_CANISR_Err (void) __attribute__ ((interrupt));
void FullCAN_CANISR_Rx1 (void) __attribute__ ((interrupt));
void FullCAN_CANISR_Rx2 (void) __attribute__ ((interrupt));
void FullCAN_CANISR_Rx3 (void) __attribute__ ((interrupt));
void FullCAN_CANISR_Rx4 (void) __attribute__ ((interrupt));

// Type definition to hold a FullCAN message
// Compatible to FullCAN Mode Stored Messages in LPC User Manual
typedef struct
{
  unsigned int Dat1; // Bits  0..10: CAN Message ID
                     // Bits 13..15: CAN interface number (1..4)
                     // Bits 16..19: DLC - Data Length Counter
                     // Bits 24..25: Semaphore bits
  unsigned int DatA; // CAN Message Data Bytes 0-3
  unsigned int DatB; // CAN Message Data Bytes 4-7
} FULLCAN_MSG;


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/ 

/**************************************************************************
DOES:    Initializes one CAN interface of the LPC2000
GLOBALS: Resets all FullCAN filters, sets and enables CAN receive interrupt
RETURNS: One if initialization successful, else zero
***************************************************************************/ 
short FullCAN_Init (
  unsigned short can_port,    // CAN interface to init (1, 2, 3 or 4)
  unsigned short can_isrvect, // Interrupt vector number to use for Rx ISR (0-15)
  unsigned int can_btr        // CAN BTR value used to set CAN baud rate
  );


/**************************************************************************
DOES:    Setup a FullCAN filter
GLOBALS: Adds the specified CAN identifier to the list of CAN messages
         received by this device
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short FullCAN_SetFilter 
  (
  unsigned short can_port, // CAN interface to init (1, 2, 3 or 4)
  unsigned int CANID       // 11-bit CAN message identifier
  );


/**************************************************************************
DOES:    Setup the interrupt service routine for CAN Status and Errors
NOTE:    The driver only implements an empty function for this interrupt,
         it must be filled by the user with application specific code
GLOBALS: Sets and enables the CAN Err interrupt service routine
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short FullCAN_SetErrIRQ (
  unsigned short can_isrvect // Interrupt vector number to use for Err ISR (0-15)
  );


/**************************************************************************
DOES:    Adds a message to the three-buffer transmit queue of a selected
         CAN interface. The message is placed in the next available buffer.
GLOBALS: None
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short FullCAN_PushMessage (
  unsigned short can_port,  // CAN interface to use (1, 2, 3 or 4)
  FULLCAN_MSG *pTransmitBuf // Source pointer to a CAN message
  );


/**************************************************************************
DOES:    Poll the Full CAN message storage area  for the next available
         CAN message received.
GLOBALS: If a CAN message is found, the matching semaphore bits are cleared
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short FullCAN_PullMessage (
  unsigned short can_port, // CAN interface to use (1, 2, 3 or 4)
  FULLCAN_MSG *pReceiveBuf // Destination pointer to a CAN message
  );


/*----------------------- END OF FILE ----------------------------------*/
