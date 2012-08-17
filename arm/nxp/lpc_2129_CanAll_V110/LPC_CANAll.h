/**************************************************************************
MODULE:    LPC_CANAll
CONTAINS:  Philips LPC2000 CAN interface driver.
           This driver shows how the CAN interface of the Philips LPC2000
           can be used to receive ALL messages on the CANbus.
           All incoming messages are copied into a receive queue.
           NOTE: This example does not implement any queue overrun handling
		       Compiled and Tested with Keil Tools www.keil.com
COPYRIGHT: Embedded Systems Academy, Inc. 2004.
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
           FOR KEIL SOFTWARE www.keil.com
VERSION:   1.00, Pf 14-JUL-04, First published release
---------------------------------------------------------------------------
HISTORY:   1.00, Pf 14-JUL-04, First published release

mt 14-May-05
modified Version for WinARM/LPC-P2129 - see LPC_CANAll.c
***************************************************************************/ 

/**************************************************************************
USER DEFINABLE PARAMETERS
***************************************************************************/ 

// Maximum number of CAN interfaces supported by this driver (1 to 4)
// So far values 3 and 4 were not tested
// The example code in "main" uses CAN ports 1 and 2 (MAX_CANPORTS 2)
#define MAX_CANPORTS 2

// Maximum number of messages kept in the receive queue
#define MAX_QUEUE 25


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
void CANAll_CANISR_Err (void) __attribute__ ((interrupt));
void CANAll_CANISR_Rx1 (void) __attribute__ ((interrupt));
void CANAll_CANISR_Rx2 (void) __attribute__ ((interrupt));
void CANAll_CANISR_Rx3 (void) __attribute__ ((interrupt));
void CANAll_CANISR_Rx4 (void) __attribute__ ((interrupt));

// Type definition to hold a CAN message
typedef struct
{
  unsigned long Frame; // Bits 16..19: DLC - Data Length Counter
                      // Bit 30: Set if this is a RTR message
                      // Bit 31: Set if this is a 29-bit ID message
  unsigned long MsgID; // CAN Message ID (11-bit or 29-bit)
  unsigned long DatA;  // CAN Message Data Bytes 0-3
  unsigned long DatB;  // CAN Message Data Bytes 4-7
} CANALL_MSG;


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/ 

/**************************************************************************
DOES:    Initializes one CAN interface of the LPC2000
GLOBALS: Resets all queue pointers, sets and enables CAN receive interrupt
RETURNS: One if initialization successful, else zero
***************************************************************************/ 
short CANAll_Init (
  unsigned short can_port,    // CAN interface to init (1, 2, 3 or 4)
  unsigned short can_isrvect, // Interrupt vector number to use for Rx ISR (0-15)
  unsigned long can_btr        // CAN BTR value used to set CAN baud rate
  );


/**************************************************************************
DOES:    Setup the interrupt service routine for CAN Status and Errors
NOTE:    The driver only implements an empty function for this interrupt,
         it must be filled by the user with application specific code
GLOBALS: Sets and enables the CAN Err interrupt service routine
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short CANAll_SetErrIRQ (
  unsigned short can_isrvect // Interrupt vector number to use for Err ISR (0-15)
  );


/**************************************************************************
DOES:    Adds a message to the three-buffer transmit queue of a selected
         CAN interface. The message is placed in the next available buffer.
GLOBALS: None
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short CANAll_PushMessage (
  unsigned short can_port, // CAN interface to use (1, 2, 3 or 4)
  CANALL_MSG *pTransmitBuf // Source pointer to a CAN message
  );


/**************************************************************************
DOES:    Poll the next received message from the receive queue
GLOBALS: Queue Out Pointer updated
RETURNS: One if operation successful, else zero
***************************************************************************/ 
short CANAll_PullMessage (
  unsigned short can_port, // CAN interface to use (1, 2, 3 or 4)
  CANALL_MSG *pReceiveBuf  // Destination pointer to a CAN message
  );


/*----------------------- END OF FILE ----------------------------------*/
