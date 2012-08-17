/**************************************************************************
MODULE:    MCOHW_LPC2
CONTAINS:  Preliminary, limited hardware driver implementation for 
           Philips LPC2000 derivatives with CAN interface.
		       Compiled and Tested with Keil Tools www.keil.com
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2004.
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
		       www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   Users that have purchased a license for PCANopenMagic
           (www.esacademy.com/software/pcanopenmagic)
           may use this code in commercial projects.
           Otherwise only educational use is acceptable.
VERSION:   1.30, Pf 05-JAN-05, Changes required by errata sheet implemented
---------------------------------------------------------------------------
HISTORY:   1.22, Pf 20-APR-04, FullCAN SW mode implemented
           1.21, Pf 09-FEB-04, Timer and second CAN interface added
           1.20, Pf 12-OCT-03, First Implementation for Philips LPC222
           1.10, Pf 27-MAY-03, Cleaned file to eliminate compiler warnings
                 Support of MEMORY types
           1.00, Ck 07-OCT-02, First Published Version
***************************************************************************/ 

#include <LPC21XX.H> // LPC21XX Peripheral Registers
#include "mcohw.h"

// Process data communicated via CAN
extern BYTE gProcImg[PROCIMG_SIZE];

// Define CAN SFR address bases 
#define CAN_REG_BASE                    (0xE0000000)
#define ACCEPTANCE_FILTER_RAM_BASE      (CAN_REG_BASE + 0x00038000)
#define ACCEPTANCE_FILTER_REGISTER_BASE (CAN_REG_BASE + 0x0003C000)
#define CENTRAL_CAN_REGISTER_BASE       (CAN_REG_BASE + 0x00040000)              
#define CAN_1_BASE                      (CAN_REG_BASE + 0x00044000)              
#define CAN_2_BASE                      (CAN_REG_BASE + 0x00048000)              

// Common CAN bit rates
#define   CANBitrate125k_12MHz          0x001C001D
#define   CANBitrate250k_12MHz          0x001C000E

// Maximum number of FullCAN Filters
#define MAX_FILTERS 20

// Timer Interrupt
void MCOHW_TimerISR (void) __attribute__ ((interrupt)); 

// CAN Interrupt
void MCOHW_CANISR_Err (void) __attribute__ ((interrupt));
void MCOHW_CANISR_Tx1 (void) __attribute__ ((interrupt));
void MCOHW_CANISR_Tx2 (void) __attribute__ ((interrupt));
void MCOHW_CANISR_Rx1 (void) __attribute__ ((interrupt));
void MCOHW_CANISR_Rx2 (void) __attribute__ ((interrupt));

void MCOHW_DefaultISR (void) __attribute__ ((interrupt)); 


// Global timer/conter variable, incremented every millisecond
WORD volatile gTimCnt = 0;

// Counts number of filters (CAN message objects) used
WORD volatile gCANFilter = 0;

// Type definition to hold a FullCAN message
typedef struct
{
  DWORD Dat1;
  DWORD DatA;
  DWORD DatB;
} FULLCAN_MSGFIELD;

// FullCAN Message List
FULLCAN_MSGFIELD volatile gFullCANList[MAX_FILTERS];

// Lookup table for receive filters
WORD gFilterList[20];							


// Pull the next CAN message from receive buffer
BYTE MCOHW_PullMessage (CAN_MSG MEM_FAR *pReceiveBuf)
{
unsigned int obj;
unsigned int candata1, *candataA, *candataB;

  obj = 0;
  candataA = (unsigned int *) &(pReceiveBuf->BUF[0]);
  candataB = (unsigned int *) &(pReceiveBuf->BUF[4]);
  
  while (obj < gCANFilter)
  {
	  // if its currently updated, we come back next time and do not wait here
    candata1 = gFullCANList[obj].Dat1;

    // semaphore set to 11?
    if ((candata1 & 0x03000000L) == 0x03000000L)
    { // Object Updated since last access
      candata1 = gFullCANList[obj].Dat1;
      gFullCANList[obj].Dat1 &= 0xFCFFFFFF; // clear Semaphore
      *candataA = gFullCANList[obj].DatA;
      *candataB = gFullCANList[obj].DatB;

      pReceiveBuf->ID = (WORD) candata1 & 0x07FF;
      pReceiveBuf->LEN = (BYTE) (candata1 >> 16) & 0x0F;

      candata1 = gFullCANList[obj].Dat1; // Re-read for semaphore
      if ((candata1 & 0x03000000L) == 0)
      { // Only return it, if not updated while reading
        return 1;
      }
    }
    obj ++; // Next message object buffer
  }
  return 0; // Return False, no msg rcvd 
}

// Push the next transmit message into transmit queue
BYTE MCOHW_PushMessage (CAN_MSG MEM_FAR *pTransmitBuf)
{
DWORD *pAddr;  
DWORD status;
DWORD candata;
DWORD *pCandata;
  
  if (pTransmitBuf->ID == 0)
  { // This may not happen
    pTransmitBuf->ID = 1;
    pTransmitBuf->LEN = 8;
  }
  
  // Wait loop, if transmit buffer is busy, wait a while
  candata = 10000; // loop counter for waiting
  status = C2SR; // CANSR
  while (!(status & 0x00000004L) && (candata > 0))
  {
    status = C2SR; // refresh CANSR 
    candata--;
  }

  status = C2SR; // CANSR

  if (!(status & 0x00000004L))
  { // Transmit Channel is not available
    return 0; // No channel available
  }

  candata = pTransmitBuf->LEN;
  candata <<= 16;

  pAddr = (DWORD *) &C2TFI1;
  *pAddr = candata;  
  
  pAddr++;
  *pAddr = pTransmitBuf->ID;
 
  pCandata = (DWORD *) &(pTransmitBuf->BUF[0]);
  pAddr++;
  *pAddr = *pCandata;

  pCandata++;
  pAddr++;
  *pAddr = *pCandata;
  
  C2CMR = 0x30; // Self Transmission Request  Buf 1

  return 1;
}


/**************************************************************************
DOES: Reads and returns the value of the current 1 millisecond system
      timer tick.
**************************************************************************/
WORD MCOHW_GetTime (void)
{
  return gTimCnt;
}


// Checks if a TimeStamp expired
BYTE MCOHW_IsTimeExpired(WORD timestamp)
{
WORD time_now;

  time_now = gTimCnt;
  if (time_now > timestamp)
  {
    if ((time_now - timestamp) < 0x8000)
      return 1;
    else
      return 0;
  }
  else
  {
    if ((timestamp - time_now) > 0x8000)
      return 1;
    else
      return 0;
  }
}

// Timer ISR
void MCOHW_TimerISR 
  (
  void
  ) 
{
  gTimCnt++;
  T0IR = 1; // Clear interrupt flag
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
}

// CDefault ISR
void MCOHW_DefaultISR 
  (
  void
  ) 
{
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
  while (1)
  {
  // DEBUG: WE SHOULD NEVER GET HERE
  }
}
                                                        
// CAN ISR
void MCOHW_CANISR_Err 
  (
  void
  ) 
{
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
  while (1)
  {
  // DEBUG: WE SHOULD NEVER GET HERE
  }
}

void MCOHW_CANISR_Tx1 
  (
  void
  ) 
{
DWORD dummy;

  dummy = C1ICR; // Read from interrupt register to acknowledge interrupt
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
}

void MCOHW_CANISR_Tx2 
  (
  void
  ) 
{
DWORD dummy;

  dummy = C2ICR; // Read from interrupt register to acknowledge interrupt
  gProcImg[IN_ana_1+1]++; // Change process data to show action
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
}

void MCOHW_CANISR_Rx1 
  (
  void
  ) 
{
DWORD buf;
DWORD *pDest;

  if (!(C1RFS & 0xC0000400L))
  { // 11-bit ID, no RTR, matched a filter

    // initialize destination pointer
    // filter number is in lower 10 bits of C1RFS
    pDest = (DWORD *) &(gFullCANList[(C1RFS & 0x000003FFL)].Dat1);
    
    // calculate contents for first entry into FullCAN list
    buf = C1RFS & 0xC00F0000L; // mask FF, RTR and DLC
    buf |= 0x01000000L; // set semaphore to 01b
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
  VICVectAddr = 0xFFFFFFFF; // acknowledge Interrupt
}

void MCOHW_CANISR_Rx2 
  (
  void
  ) 
{
  C2CMR = 0x04; // Release receive buffer
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
}


// Init CAN Interface and Timer
BYTE MCOHW_Init 
  (
  WORD BaudRate
  )
{
  if (BaudRate != 125) 
  { // This implementation only supports 125kbit
      return 0;
  }

  // Enable Pins for CAN port 1 and 2
  // PINSEL0 |= (DWORD) 0x00000000; 
  PINSEL1 |= (DWORD) 0x00054000; 
  
  C1MOD = 1; // Enter Reset Mode
  C1GSR = 0; // Clear status register
  C1BTR = CANBitrate125k_12MHz; // Set bit timing
  
  AFMR = 0x00000001; // Disable acceptance filter

  // Disable All Interrupts
  C1IER = 0;

  // Enter Normal Operating Mode
  C1MOD = 0; // Operating Mode 

  // Init CAN interface 2
  C2MOD = 1; // Enter Reset Mode
  C2GSR = 0; // Clear status register
  C2BTR = CANBitrate125k_12MHz; // Set bit timing
  
  // Disable All Interrupt
  C2IER = 0;

  // Enter Normal Operating Mode
  C2MOD = 0; // Operating Mode 
  
  // Init Interrupts
  VICDefVectAddr = (unsigned long) MCOHW_DefaultISR;

  // Initialize Timer Interrupt
  T0MR0 = 59999; // 1mSec = 60.000-1 counts
  T0MCR = 3; // Interrupt and Reset on MR0
  T0TCR = 1;  // Timer0 Enable

  VICVectAddr0 = (unsigned long) MCOHW_TimerISR; // set interrupt vector
  VICVectCntl0 = 0x20 | 4;  // use it for Timer 0 Interrupt
  VICIntEnable = 0x00000010;  // Enable Timer0 Interrupt

  VICVectAddr1 = (unsigned long) MCOHW_CANISR_Rx1; // set interrupt vector
  VICVectCntl1 = 0x20 | 26;  // use it for CAN Rx1 Interrupt
  VICIntEnable = 0x04000000;  // Enable CAN Rx1 Interrupt
  
  C1IER = 0x0001; // Enable CAN 1 RX interrupt

  return 1;
}


BYTE MCOHW_SetCANFilter 
  (
  WORD CANID
  )
{
int p, n;
int buf0, buf1;
int ID_lower, ID_upper;
DWORD candata;
DWORD *pAddr;

  if (gCANFilter == 0)
  { // First call, init entry zero
    gFilterList[0] = 0x17FF; // Disabled and unused
  }
  if (gCANFilter >= MAX_FILTERS)
  {
    return 0;
  }

  // Filters must be sorted by priority

  // new filter is sorted into array
  p = 0;
  while (p < gCANFilter) // loop through all existing filters 
  {
    if (gFilterList[p] > CANID)
    {
      break;
    }
    p++;
  }
  // insert new filter here
  buf0 = gFilterList[p]; // save current entry
  gFilterList[p] = CANID; // insert the new entry
  // move all remaining entries one row up
  gCANFilter++;
  while (p < gCANFilter)
  {
    p++;
    buf1 = gFilterList[p];
    gFilterList[p] = buf0;
    buf0 = buf1;
  }

  // Now work on Acceptance Filter Configuration     
  // Acceptance Filter Mode Register = off !
  AFMR = 0x00000001;
  
  // Set CAN filter for 11-bit standard identifiers
  p = 0;

  // Set pointer for Standard Frame Individual
  // Standard Frame Explicit
  SFF_sa = p;

  pAddr = (DWORD *) ACCEPTANCE_FILTER_RAM_BASE;
  for (n = 0; n < ((gCANFilter+1)/2); n++)
  {
    ID_lower = gFilterList[n * 2];
    ID_upper = gFilterList[n * 2 + 1];
    // 0x20002000 indicates CAN interface 1
    candata = 0x20002000 + (ID_lower << 16) + ID_upper;
    *pAddr = candata;
    p += 4;
    pAddr++;
  }

  // p is still ENDofTable;
  // last entry must be unused filters
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
  AFMR = 0x00000000;
  
  return 1;
}


/*----------------------- END OF FILE ----------------------------------*/

