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
VERSION:   1.10, Pf 05-JAN-05, Latest errata sheet suggestions implemented
---------------------------------------------------------------------------
HISTORY:   1.01, Pf 18-APR-04, Added Timer and LED output
HISTORY:   1.00, Pf 16-APR-04, First published release
***************************************************************************/ 

#include <LPC21XX.H> // LPC21XX Peripheral Registers
#include "LPC_FullCAN_SW.h"


/**************************************************************************
GLOBAL VARIABLES
***************************************************************************/ 

// Counts number of filters (CAN message objects) used so far
short volatile gCANFilter = 0;

// FullCAN Message List
FULLCAN_MSG volatile gFullCANList[MAX_FILTERS];


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/ 

/**************************************************************************
Initialization of a CAN interface
as described in LPC_FullCAN_SW.h
***************************************************************************/ 
short FullCAN_Init (
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

  // Reset and disable all message filters
  gCANFilter = 0;
  // Acceptance Filter Mode Register = off !
  AFMR = 0x00000001L;

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
    *pSFR = (unsigned long) FullCAN_CANISR_Rx1; 
    // Use this Interrupt for CAN Rx1 Interrupt
    *pSFR2 = 0x20 | 26;
    // Enable CAN Rx1 Interrupt
    VICIntEnable = 0x04000000L;  
    break;
#if (MAX_CANPORTS > 1)
  case 2:
    // Set interrupt vector
    *pSFR = (unsigned long) FullCAN_CANISR_Rx2; 
    // Use this Interrupt for CAN Rx2 Interrupt
    *pSFR2 = 0x20 | 27;
    // Enable CAN Rx2 Interrupt
    VICIntEnable = 0x08000000L;  
    break;
#endif
#if (MAX_CANPORTS > 2)
  case 3:
    // Set interrupt vector
    *pSFR = (unsigned long) FullCAN_CANISR_Rx3; 
    // Use this Interrupt for CAN Rx3 Interrupt
    *pSFR2 = 0x20 | 28;
    // Enable CAN Rx3 Interrupt
    VICIntEnable = 0x10000000L;  
    break;
#endif
#if (MAX_CANPORTS > 3)
  case 4:
    // Set interrupt vector
    *pSFR = (unsigned long) FullCAN_CANISR_Rx4; 
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
Setting a CAN receive filter
as described in LPC_FullCAN_SW.h
***************************************************************************/ 
short FullCAN_SetFilter (
  unsigned short can_port, // CAN interface number
  unsigned int CANID // 11-bit CAN ID
  )
{
unsigned int p, n;
unsigned int buf0, buf1;
unsigned int ID_lower, ID_upper;
unsigned int candata;
unsigned int *pAddr;

  // Double check can_port value
  if ((can_port < 1) || (can_port > MAX_CANPORTS))
  { // Illegal value for can_port
    return 0;
  }

  // Acceptance Filter Mode Register = off !
  AFMR = 0x00000001L;

  if (gCANFilter == 0)
  { // First call, init entry zero
    gFullCANList[0].Dat1 = 0x000037FFL; // CAN 1, disabled and unused
  }
  if (gCANFilter >= MAX_FILTERS)
  {
    return 0;
  }

  CANID &= 0x000007FFL; // Mask out 11-bit ID
  CANID |= (can_port << 13); // Put can_port info in bits 13-15

  // Filters must be sorted by interface, then by priority
  // new filter is sorted into array
  p = 0;
  while (p < gCANFilter) // loop through all existing filters 
  {
    if ((gFullCANList[p].Dat1 & 0x0000FFFFL) > CANID)
    {
      break;
    }
    p++;
  }

  // insert new filter here
  buf0 = gFullCANList[p].Dat1; // save current entry
  gFullCANList[p].Dat1 = CANID; // insert the new entry

  // move all remaining entries one row up
  gCANFilter++;
  while (p < gCANFilter)
  {
    p++;
    buf1 = gFullCANList[p].Dat1;
    gFullCANList[p].Dat1 = buf0;
    buf0 = buf1;
  }

  // Now work on Acceptance Filter Configuration     
  // Set CAN filter for 11-bit standard identifiers
  p = 0;

  // Set pointer for Standard Frame Individual
  // Standard Frame Explicit
  SFF_sa = p;

  pAddr = (unsigned int *) ACCEPTANCE_FILTER_RAM_BASE;
  for (n = 0; n < ((gCANFilter+1)/2); n++)
  {
    ID_lower = gFullCANList[n * 2].Dat1 & 0x0000FFFFL;
    ID_upper = gFullCANList[n * 2 + 1].Dat1 & 0x0000FFFFL;
    candata = (ID_lower << 16) + ID_upper;
    *pAddr = candata;
    p += 4;
    pAddr++;
  }

  // p is still pointing to ENDofTable;
  // last entry must contain unused filters
  *pAddr = 0xF7FFF7FF;
  p += 4;
  
  // Set pointer for Standard Frame Groups
  // Standard Frame Group Start Address Register
  SFF_GRP_sa = p;

  // Set pointer for Extended Frame Individual
  // Extended Frame Start Address Register
  EFF_sa = p;

  // Set pointer for Extended Frame Groups
  // Extended Frame Group Start Address Register
  EFF_GRP_sa = p;

  // Set ENDofTable 
  // End of AF Tables Register
  ENDofTable = p;

  // Acceptance Filter Mode Register, start using filter
  AFMR = 0;
  
  return 1;
}


/**************************************************************************
Installing the CAN Err ISR
as described in LPC_FullCAN_SW.h
***************************************************************************/ 
short FullCAN_SetErrIRQ (
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
  *pSFR = (unsigned long) FullCAN_CANISR_Err; 
  
  pSFR = (unsigned int *) &VICVectCntl0;
  pSFR += can_isrvect; // Set to desired interrupt control
  *pSFR = 0x20 | 19;

  // Enable Interrupt source
  VICIntEnable = 0x00080000L;  

  return 1;
}


/**************************************************************************
Transmitting a CAN message on a selected CAN interface
as described in LPC_FullCAN_SW.h
***************************************************************************/ 
short FullCAN_PushMessage (
  unsigned short can_port,
  FULLCAN_MSG *pTransmitBuf
  )
{
unsigned int *pAddr;  
unsigned int *pCandata;
unsigned int offset;

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

  // Write DLC
  pAddr = (unsigned int *) &C1TFI1 + offset;
  *pAddr = pTransmitBuf->Dat1 & 0x000F0000L;  
  
  // Write CAN ID
  pAddr++;
  *pAddr = pTransmitBuf->Dat1 & 0x000007FFL;
 
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
as described in LPC_FullCAN_SW.h
***************************************************************************/ 
short FullCAN_PullMessage (
  unsigned short can_port,
  FULLCAN_MSG *pReceiveBuf
  )
{
unsigned short obj; // loop counter for message objects/filters
unsigned int *pSrc; // Source pointer
unsigned int *pDst; // Destination pointer
unsigned int match; // Match value for can_port and semaphore

  // Double check can_port value
  if ((can_port < 1) || (can_port > MAX_CANPORTS))
  { // Illegal value for can_port
    return 0;
  }

  // Initialize pointers
  pSrc = (unsigned int *) &(gFullCANList[0].Dat1);
  pDst = (unsigned int *) &(pReceiveBuf->Dat1);

  // Prepare match value for CAN interface
  match = can_port << 13;
  match |= 0x03000000L; // Semaphore bits are 11b
  
  obj = 0; 
  while (obj < gCANFilter)
  {
    // match can_port and semaphore set to 11?
    if ((*pSrc & 0x0300E000L) == match)
    { // Object Updated since last access
      *pSrc &= 0xFCFFFFFFL; // clear Semaphore

      *pDst = *pSrc; // Copy Dat1

      pSrc++; // set to gFullCANList[obj].DatA
      pDst++; // set to pReceiveBuf->DatA

      *pDst = *pSrc; // Copy DatA

      pSrc++; // set to gFullCANList[obj].DatB
      pDst++; // set to pReceiveBuf->DatB

      *pDst = *pSrc; // Copy DatB

      pSrc -= 2; // set back to gFullCANList[obj].Dat1
      pDst -= 2; // set back to &(pReceiveBuf->Dat1)

      if ((*pSrc & 0x03000000L) == 0)
      { // Only return it, if not updated while reading
        return 1;
      }
    }
    obj ++; // Next message object buffer
    pSrc += 3; // set to gFullCANList[obj].Dat1
  }
  return 0; // Return False, no msg rcvd 
}


/**************************************************************************
PRIVATE FUNCTIONS
***************************************************************************/ 

/**************************************************************************
DOES:    Interrupt Service Routine for CAN Errors
GLOBALS: none
RETURNS: nothing
***************************************************************************/ 
void FullCAN_CANISR_Err (
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
GLOBALS: Copies the received message into the gFullCANList[] array
         Handles semaphore bits as described in LPC user manual
RETURNS: nothing
***************************************************************************/ 
void FullCAN_CANISR_Rx1 (
  void
  ) 
{
unsigned int buf;
unsigned int *pDest;

  if (!(C1RFS & 0xC0000400L))
  { // 11-bit ID, no RTR, matched a filter

    // initialize destination pointer
    // filter number is in lower 10 bits of C1RFS
    pDest = (unsigned int *) &(gFullCANList[(C1RFS & 0x000003FFL)].Dat1);
    
    // calculate contents for first entry into FullCAN list
    buf = C1RFS & 0xC00F0000L; // mask FF, RTR and DLC
    buf |= 0x01002000L; // set semaphore to 01b and CAN port to 1
    buf |= C1RID & 0x000007FFL; // get CAN message ID

    // now copy entire message to FullCAN list
    *pDest = buf; 
    pDest++; // set to gFullCANList[(C1RFS & 0x000003FFL)].DatA
    *pDest = C1RDA; 
    pDest++; // set to gFullCANList[(C1RFS & 0x000003FFL)].DatB
    *pDest = C1RDB; 

    // now set the sempahore to complete
    buf |= 0x03000000L; // set semaphore to 11b
    pDest -= 2; // set to gFullCANList[(C1RFS & 0x000003FFL)].Dat1
    *pDest = buf; 
  }

  C1CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}


#if (MAX_CANPORTS > 1)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 2
GLOBALS: Copies the received message into the gFullCANList[] array
         Handles semaphore bits as described in LPC user manual
RETURNS: nothing
***************************************************************************/ 
void FullCAN_CANISR_Rx2 (
  void
  ) 
{
unsigned int buf;
unsigned int *pDest;

  if (!(C2RFS & 0xC0000400L))
  { // 11-bit ID, no RTR, matched a filter

    // initialize destination pointer
    // filter number is in lower 10 bits of C2RFS
    pDest = (unsigned int *) &(gFullCANList[(C2RFS & 0x000003FFL)].Dat1);
    
    // calculate contents for first entry into FullCAN list
    buf = C2RFS & 0xC00F0000L; // mask FF, RTR and DLC
    buf |= 0x01004000L; // set semaphore to 01b and CAN port to 2
    buf |= C2RID & 0x000007FFL; // get CAN message ID

    // now copy entire message to FullCAN list
    *pDest = buf; 
    pDest++; // set to gFullCANList[(C2RFS & 0x000003FFL)].DatA
    *pDest = C2RDA; 
    pDest++; // set to gFullCANList[(C2RFS & 0x000003FFL)].DatB
    *pDest = C2RDB; 

    // now set the sempahore to complete
    buf |= 0x03000000L; // set semaphore to 11b
    pDest -= 2; // set to gFullCANList[(C2RFS & 0x000003FFL)].Dat1
    *pDest = buf; 
  }

  C2CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 1


#if (MAX_CANPORTS > 2)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 3
GLOBALS: Copies the received message into the gFullCANList[] array
         Handles semaphore bits as described in LPC user manual
RETURNS: nothing
***************************************************************************/ 
void FullCAN_CANISR_Rx3 (
  void
  ) 
{
unsigned int buf;
unsigned int *pDest;

  if (!(C3RFS & 0xC0000400L))
  { // 11-bit ID, no RTR, matched a filter

    // initialize destination pointer
    // filter number is in lower 10 bits of C3RFS
    pDest = (unsigned int *) &(gFullCANList[(C3RFS & 0x000003FFL)].Dat1);
    
    // calculate contents for first entry into FullCAN list
    buf = C3RFS & 0xC00F0000L; // mask FF, RTR and DLC
    buf |= 0x01006000L; // set semaphore to 01b and CAN port to 3
    buf |= C3RID & 0x000007FFL; // get CAN message ID

    // now copy entire message to FullCAN list
    *pDest = buf; 
    pDest++; // set to gFullCANList[(C3RFS & 0x000003FFL)].DatA
    *pDest = C3RDA; 
    pDest++; // set to gFullCANList[(C3RFS & 0x000003FFL)].DatB
    *pDest = C3RDB; 

    // now set the sempahore to complete
    buf |= 0x03000000L; // set semaphore to 11b
    pDest -= 2; // set to gFullCANList[(C3RFS & 0x000003FFL)].Dat1
    *pDest = buf; 
  }

  C3CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 2


#if (MAX_CANPORTS > 3)
/**************************************************************************
DOES:    Interrupt Service Routine for CAN receive on CAN interface 4
GLOBALS: Copies the received message into the gFullCANList[] array
         Handles semaphore bits as described in LPC user manual
RETURNS: nothing
***************************************************************************/ 
void FullCAN_CANISR_Rx4 (
  void
  ) 
{
unsigned int buf;
unsigned int *pDest;

  if (!(C4RFS & 0xC0000400L))
  { // 11-bit ID, no RTR, matched a filter

    // initialize destination pointer
    // filter number is in lower 10 bits of C4RFS
    pDest = (unsigned int *) &(gFullCANList[(C4RFS & 0x000003FFL)].Dat1);
    
    // calculate contents for first entry into FullCAN list
    buf = C4RFS & 0xC00F0000L; // mask FF, RTR and DLC
    buf |= 0x01008000L; // set semaphore to 01b and CAN port to 4
    buf |= C2RID & 0x000007FFL; // get CAN message ID

    // now copy entire message to FullCAN list
    *pDest = buf; 
    pDest++; // set to gFullCANList[(C4RFS & 0x000003FFL)].DatA
    *pDest = C4RDA; 
    pDest++; // set to gFullCANList[(C4RFS & 0x000003FFL)].DatB
    *pDest = C4RDB; 

    // now set the sempahore to complete
    buf |= 0x03000000L; // set semaphore to 11b
    pDest -= 2; // set to gFullCANList[(C4RFS & 0x000003FFL)].Dat1
    *pDest = buf; 
  }

  C4CMR = 0x04; // release receive buffer
  VICVectAddr = 0xFFFFFFFFL; // acknowledge Interrupt
}
#endif // MAX_CANPORTS > 4


/*----------------------- END OF FILE ----------------------------------*/

