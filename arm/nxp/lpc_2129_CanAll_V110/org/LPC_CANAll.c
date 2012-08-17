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
VERSION:   1.10, Pf 05-JAN-05, Updates in accordance with errata sheets
---------------------------------------------------------------------------
HISTORY:   1.00, Pf 14-JUL-04, First published release
***************************************************************************/ 

#include <LPC21XX.H> // LPC21XX Peripheral Registers
#include "LPC_CANAll.h"


/**************************************************************************
GLOBAL VARIABLES
***************************************************************************/ 

// Receive Queue: one queue for each CAN port
CANALL_MSG mRxCAN[MAX_CANPORTS][MAX_QUEUE];

// Receive Queue IN pointer
short mRxIN[MAX_CANPORTS];

// Receive Queue OUT pointer
short mRxOUT[MAX_CANPORTS];

/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/ 

/**************************************************************************
Initialization of a CAN interface
as described in LPC_CANAll.h
***************************************************************************/ 
short CANAll_Init (
  unsigned short can_port,    // CAN interface to use
  unsigned short can_isrvect, // interrupt vector number to use for Rx ISR (0-15)
  unsigned int can_btr        // CAN BTR value used to set CAN baud rate
  )
{
unsigned int *pSFR; // pointer into SFR space
unsigned int *pSFR2; // pointer into SFR space
unsigned int offset; // offset added to pSFR

  // Double check can_isrvect value
  if (can_isrvect > 15)
  { // Illegal value for can_isrvect
    return 0;
  }

  // Double check can_port value
  if ((can_port < 1) || (can_port > MAX_CANPORTS))
  { // Illegal value for can_port
    return 0;
  }

  // Reset IN and OUT pointer
  mRxIN[can_port-1] = 0;
  mRxOUT[can_port-1] = 0;
  
  // Enable pins for selected CAN interface
  switch (can_port)
  {
  case 1:
    PINSEL1 |= 0x00040000L; // Set bit 18
    offset = 0x00000000L; // Use 1st set of CAN registers
    break;
#if (MAX_CANPORTS > 1)
  case 2:
    PINSEL1 |= 0x00014000L; // Set bits 14 and 16
    offset = 0x00001000L; // Use 2nd set of CAN registers
    break;
#endif
#if (MAX_CANPORTS > 2)
  case 3:
    PINSEL1 |= 0x00001800L; // Set bits 11 and 12
    offset = 0x00002000L; // Use 3rd set of CAN registers
    break;
#endif
#if (MAX_CANPORTS > 3)
  case 4:
    PINSEL0 |= 0x0F000000L; // Set bits 24 to 27
    offset = 0x00003000L; // Use 4th set of CAN registers
    break;
#endif
  default:
    return 0; // illegal value used
  }

  // Acceptance Filter Mode Register = filter off, receive all
  AFMR = 0x00000002L;

  pSFR = (unsigned int *) &C1MOD + offset; // Select Mode register
  *pSFR = 1; // Go into Reset mode

  pSFR = (unsigned int *) &C1IER + offset; // Select Interrupt Enable Register
  *pSFR = 0;// Disable All Interrupts

  pSFR = (unsigned int *) &C1GSR + offset; // Select Status Register
  *pSFR = 0; // Clear Status register

  pSFR = (unsigned int *) &C1BTR + offset; // Select BTR Register
  *pSFR = can_btr; // Set bit timing

  // Set and enable receive interrupt
  pSFR = (unsigned int *) &VICVectAddr0;
  pSFR += can_isrvect; // Set to desired interrupt vector
  
  pSFR2 = (unsigned int *) &VICVectCntl0;
  pSFR2 += can_isrvect; // Set to desired interrupt control

  switch (can_port)
  {
  case 1:
    // Set interrupt vector
    *pSFR = (unsigned long) CANAll_CANISR_Rx1; 
    // Use this Interrupt for CAN Rx1 Interrupt
    *pSFR2 = 0x20 | 26;
    // Enable CAN Rx1 Interrupt
    VICIntEnable = 0x04000000L;  
    break;
#if (MAX_CANPORTS > 1)
  case 2:
    // Set interrupt vector
    *pSFR = (unsigned long) CANAll_CANISR_Rx2; 
    // Use this Interrupt for CAN Rx2 Interrupt
    *pSFR2 = 0x20 | 27;
    // Enable CAN Rx2 Interrupt
    VICIntEnable = 0x08000000L;  
    break;
#endif
#if (MAX_CANPORTS > 2)
  case 3:
    // Set interrupt vector
    *pSFR = (unsigned long) CANAll_CANISR_Rx3; 
    // Use this Interrupt for CAN Rx3 Interrupt
    *pSFR2 = 0x20 | 28;
    // Enable CAN Rx3 Interrupt
    VICIntEnable = 0x10000000L;  
    break;
#endif
#if (MAX_CANPORTS > 3)
  case 4:
    // Set interrupt vector
    *pSFR = (unsigned long) CANAll_CANISR_Rx4; 
    // Use this Interrupt for CAN Rx4 Interrupt
    *pSFR2 = 0x20 | 29;
    // Enable CAN Rx4 Interrupt
    VICIntEnable = 0x20000000L;  
    break;
#endif
  default:
    return 0; // illegal value used
  }

  pSFR = (unsigned int *) &C1IER + offset; // Select Interrupt register
  *pSFR = 1; // Enable Receive Interrupt

  // Enter Normal Operating Mode
  pSFR = (unsigned int *) &C1MOD + offset; // Select Mode register
  *pSFR = 0; // Operating Mode 

  return 1;
}


/**************************************************************************
Installing the CAN Err ISR
as described in LPC_CANAll.h
***************************************************************************/ 
short CANAll_SetErrIRQ (
  unsigned short can_isrvect // interrupt vector number to use for Err ISR (0-15)
  )
{
unsigned int *pSFR; // pointer into SFR space

  // Double check can_isrvect value
  if (can_isrvect > 15)
  { // Illegal value for can_isrvect
    return 0;
  }

  // Set and enable err interrupt
  pSFR = (unsigned int *) &VICVectAddr0;
  pSFR += can_isrvect; // Set to desired interrupt vector
  // Set interrupt vector
  *pSFR = (unsigned long) CANAll_CANISR_Err; 
  
  pSFR = (unsigned int *) &VICVectCntl0;
  pSFR += can_isrvect; // Set to desired interrupt control
  *pSFR = 0x20 | 19;

  // Enable Interrupt source
  VICIntEnable = 0x00080000L;  

  return 1;
}


/**************************************************************************
Transmitting a CAN message on a selected CAN interface
as described in LPC_CANAll.h
***************************************************************************/ 
short CANAll_PushMessage (
  unsigned short can_port,
  CANALL_MSG *pTransmitBuf
  )
{
unsigned int *pAddr;  
unsigned int *pCandata;
unsigned int offset;
  
  // Double check can_port value
  if ((can_port < 1) || (can_port > MAX_CANPORTS))
  { // Illegal value for can_port
    return 0;
  }

  switch (can_port)
  {
  case 1:
    offset = 0x00000000L; // Use 1st set of CAN registers
    break;
#if (MAX_CANPORTS > 1)
  case 2:
    offset = 0x00001000L; // Use 2nd set of CAN registers
    break;
#endif
#if (MAX_CANPORTS > 2)
  case 3:
    offset = 0x00002000L; // Use 3rd set of CAN registers
    break;
#endif
#if (MAX_CANPORTS > 3)
  case 4:
    offset = 0x00003000L; // Use 4th set of CAN registers
    break;
#endif
  default:
    return 0; // illegal value used
  }

  pAddr = (unsigned int *) &C1SR + offset; // CANSR
  if (!(*pAddr & 0x00000004L))
  { // Transmit Channel is not available
    return 0; // No channel available
  }

  // Write DLC, RTR and FF
  pAddr = (unsigned int *) &C1TFI1 + offset;
  *pAddr = pTransmitBuf->Frame & 0xC00F0000L;  
  
  // Write CAN ID
  pAddr++;
  *pAddr = pTransmitBuf->MsgID;
 
  // Write first 4 data bytes 
  pCandata = (unsigned int *) &(pTransmitBuf->DatA);
  pAddr++;
  *pAddr = *pCandata;

  // Write second 4 data bytes 
  pCandata++;
  pAddr++;
  *pAddr = *pCandata;
  
  // Write self transmission request
  pAddr = (unsigned int *) &C1CMR + offset;
  *pAddr = 0x30; // Self Transmission Request Buf 1

  return 1;
}


/**************************************************************************
Receiving a CAN message
as described in LPC_CANAll.h
***************************************************************************/ 
short CANAll_PullMessage (
  unsigned short can_port,
  CANALL_MSG *pReceiveBuf
  )
{
unsigned int *pSrc; // Source pointer
unsigned int *pDst; // Destination pointer

  // Double check can_port value
  if ((can_port < 1) || (can_port > MAX_CANPORTS))
  { // Illegal value for can_port
    return 0;
  }
  can_port--; // Adjust to 0 - MAX_CANPORTS-1

  // Check if something is in RxQueue
  if ((mRxIN[can_port]) == (mRxOUT[can_port]))
  { // Queue is empty
    return 0;
  }
  // Pointers are different, so something is in queue

  // Initialize pointers
  pSrc = (unsigned int *) &(mRxCAN[can_port][mRxOUT[can_port]]);
  pDst = (unsigned int *) &(pReceiveBuf->Frame);
  *pDst = *pSrc; // Copy Frame

  pSrc++;
  pDst++;
  *pDst = *pSrc; // Copy MsgID

  pSrc++;
  pDst++;
  *pDst = *pSrc; // Copy DatA

  pSrc++;
  pDst++;
  *pDst = *pSrc; // Copy DatB

  // Adjust OUT pointer
  mRxOUT[can_port]++;
  if (mRxOUT[can_port] >= MAX_QUEUE)
  {
    mRxOUT[can_port] = 0;
  }

  return 1;
}


/**************************************************************************
PRIVATE FUNCTIONS
***************************************************************************/ 

/**************************************************************************
DOES:    Interrupt Service Routine for CAN Errors
GLOBALS: none
RETURNS: nothing
***************************************************************************/ 
void CANAll_CANISR_Err (
  void
  ) 
{
  // DEBUG VERSION: do not return from Errors
  // Replace this with application specific error handling
  while (1)
  { 
  }
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}


/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 1
GLOBALS: Copies the received message into the gRxCAN[0][] array
RETURNS: nothing
***************************************************************************/ 
void CANAll_CANISR_Rx1 (
  void
  ) 
{
unsigned int *pDest;

  // initialize destination pointer
  pDest = (unsigned int *) &(mRxCAN[0][mRxIN[0]]);
  *pDest = C1RFS;  // Frame

  pDest++;
  *pDest = C1RID; // ID

  pDest++;
  *pDest = C1RDA; // Data A

  pDest++;
  *pDest = C1RDB; // Data B

  // Adjust IN pointer
  mRxIN[0]++;
  if (mRxIN[0] >= MAX_QUEUE)
  {
    mRxIN[0] = 0;
  }

  C1CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}


#if (MAX_CANPORTS > 1)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 2
GLOBALS: Copies the received message into the gRxCAN[1][] array
RETURNS: nothing
***************************************************************************/ 
void CANAll_CANISR_Rx2 (
  void
  ) 
{
unsigned int *pDest;

  // initialize destination pointer
  pDest = (unsigned int *) &(mRxCAN[1][mRxIN[1]]);
  *pDest = C2RFS;  // Frame

  pDest++;
  *pDest = C2RID; // ID

  pDest++;
  *pDest = C2RDA; // Data A

  pDest++;
  *pDest = C2RDB; // Data B

  // Adjust IN pointer
  mRxIN[1]++;
  if (mRxIN[1] >= MAX_QUEUE)
  {
    mRxIN[1] = 0;
  }

  C2CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 1


#if (MAX_CANPORTS > 2)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 3
GLOBALS: Copies the received message into the gRxCAN[2][] array
RETURNS: nothing
***************************************************************************/ 
void CANAll_CANISR_Rx3 (
  void
  ) 
{
unsigned int *pDest;

  // initialize destination pointer
  pDest = (unsigned int *) &(mRxCAN[2][mRxIN[2]]);
  *pDest = C3RFS;  // Frame

  pDest++;
  *pDest = C3RID; // ID

  pDest++;
  *pDest = C3RDA; // Data A

  pDest++;
  *pDest = C3RDB; // Data B

  // Adjust IN pointer
  mRxIN[2]++;
  if (mRxIN[2] >= MAX_QUEUE)
  {
    mRxIN[2] = 0;
  }

  C3CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 2


#if (MAX_CANPORTS > 3)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 4
GLOBALS: Copies the received message into the gRxCAN[3][] array
RETURNS: nothing
***************************************************************************/ 
void CANAll_CANISR_Rx4 (
  void
  ) 
{
unsigned int *pDest;

  // initialize destination pointer
  pDest = (unsigned int *) &(mRxCAN[3][mRxIN[3]]);
  *pDest = C4RFS;  // Frame

  pDest++;
  *pDest = C4RID; // ID

  pDest++;
  *pDest = C4RDA; // Data A

  pDest++;
  *pDest = C4RDB; // Data B

  // Adjust IN pointer
  mRxIN[3]++;
  if (mRxIN[3] >= MAX_QUEUE)
  {
    mRxIN[3] = 0;
  }

  C4CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 4


/*----------------------- END OF FILE ----------------------------------*/

