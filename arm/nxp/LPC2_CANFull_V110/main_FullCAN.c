/**************************************************************************
MODULE:    Main for LPC_FullCAN_SW
CONTAINS:  CAN Example using the LPC_FullCAN_SW Driver
           Tested with MCB2100 board from Keil Software
DOES:      Sends CAN message with ID 0x101 from CAN 1
COPYRIGHT: Embedded Systems Academy, Inc. 2004.
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
           FOR KEIL SOFTWARE www.keil.com
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
VERSION:   1.10, Pf 05-JAN-05, Latest errata sheet suggestions implemented
---------------------------------------------------------------------------
HISTORY:   1.01, Pf 18-APR-04, Added Timer and LED output
HISTORY:   1.00, Pf 16-APR-04, First published release
***************************************************************************/ 
                  
#include <LPC21xx.H> // LPC21xx definitions
#include "LPC_FullCAN_SW.h"


// This value defines the number of timer ticks (of 100 microcseconds)
// that the program waits before sending the next CAN message. Here: 100ms
#define SLOWDOWN 1000


// Interrupt Service Routines
void DefaultISR (void) __attribute__ ((interrupt)); 
void Timer0ISR (void) __attribute__ ((interrupt)); 


// Global Timer Tick
unsigned int volatile gTimerTick;


/**************************************************************************
DOES:    Default Service Routine for interrupts
GLOBALS: none
RETURNS: never
***************************************************************************/ 
void DefaultISR (
  void
  ) 
{
  while (1)
  { // We should never get here, if we do it is an error
  }
}


/**************************************************************************
DOES:    Timer 0 Interrupt Service Routine
GLOBALS: Increment timer tick
RETURNS: never
***************************************************************************/ 
void Timer0ISR (
  void
  ) 
{
  gTimerTick++;
  T0IR = 1; // Clear interrupt flag
  VICVectAddr = 0xFFFFFFFF; // Acknowledge Interrupt
}


/**************************************************************************
DOES:    Checks if a timestamp expired
GLOBALS: none
RETURNS: one if it expired, else zero
***************************************************************************/ 
short IsTimeExpired (
  unsigned int timestamp
  )
{
unsigned int time_now;

  time_now = gTimerTick;
  if (time_now > timestamp)
  {
    if ((time_now - timestamp) < 0x80000000L)
      return 1;
    else
      return 0;
  }
  else
  {
    if ((timestamp - time_now) > 0x80000000L)
      return 1;
    else
      return 0;
  }
}


/**************************************************************************
DOES:    Waits until a timeout expires. Actual time waited is in the range
         from [0 to 100us*(delay-1)] to [0 to 100us*delay]
GLOBALS: none
RETURNS: nothing
***************************************************************************/ 
void SlowDown (
  unsigned int delay // Number of 100us timeouts used for waiting
  )
{
  delay += gTimerTick;
  while (!IsTimeExpired(delay))
  {
  }
}


/**************************************************************************
DOES:    Main routine of demo program:
         1.) Initialization
         2.) Send a single CAN message
         3.) Wait for CAN message,
             send response to received messages
GLOBALS: none
RETURNS: never
***************************************************************************/ 
int main 
  (
  void
  ) 
{
FULLCAN_MSG MsgBuf; // Buffers one CAN message
unsigned int LEDvalue = 0x10000000L; // Used for LED output

  // No divider: peripheral clock = processor clock
  VPBDIV = 1; 
    
  // Init LED ports
  IODIR1 = 0x00FF0000; // P1.16..23 defined as Outputs

  // Init Vector Interrupt Controller
  VICIntEnClr = 0xFFFFFFFFL; // Disable all Ints
  VICIntSelect = 0x00000000L;

  // Install Default IRQVec
  VICDefVectAddr = (unsigned long) DefaultISR; // set interrupt vector

  // Initialisation of CAN interfaces
  // CAN interface 1, use IRQVec0, at 125kbit
  FullCAN_Init(1,0,CANBitrate125k_12MHz); 
  
  // CAN interface 2, use IRQVec1, at 125kbit
  FullCAN_Init(2,1,CANBitrate125k_12MHz); 

  // Set CAN Err ISR to IRQVec2
  FullCAN_SetErrIRQ(2);
  
  // Initialize Timer Interrupt
  T0MR0 = 5999; // 100 microseconds = 6.000-1 counts
  T0MCR = 3; // Interrupt and Reset on MR0
  T0TCR = 1;  // Timer0 Enable

  // Set Timer0 ISR to IRQVec3
  VICVectAddr3 = (unsigned long) Timer0ISR; // set interrupt vector
  VICVectCntl3 = 0x20 | 4;  // use it for Timer 0 Interrupt
  VICIntEnable = 0x00000010;  // enable Timer0 Interrupt

  // Receive message with ID 101h on CAN 2
  FullCAN_SetFilter(2,0x101); 

  // Receive message with ID 201h on CAN 2
  FullCAN_SetFilter(2,0x201); 

  // Receive message with ID 102h on CAN 1
  FullCAN_SetFilter(1,0x102); 

  // Receive message with ID 202h on CAN 1
  FullCAN_SetFilter(1,0x202); 

  // DEBUG HELP:
  // Wait for 10 milliseconds, to allow debug hardware to catch up
  SlowDown(100);

  // Initialize MsgBuf
  MsgBuf.Dat1 = 0x00080102L; // 8 bytes, ID 0x102
  MsgBuf.DatA = 0x00000000L; // all zeros
  MsgBuf.DatB = 0x00000000L; // all zeros
  // Transmit initial message on CAN 2
  FullCAN_PushMessage(2,&MsgBuf);

  while (1)  
  {// Loop forever 

    // Handle LEDs
    LEDvalue <<= 1;
    if (LEDvalue > 0x00800000L)
    {
      LEDvalue = 0x00010000L;
    }
    IOSET1 = LEDvalue;

    // Check if message received on CAN 1
    if (FullCAN_PullMessage(1,&MsgBuf))
    { // Message was received
      if ((MsgBuf.Dat1 & 0x07FF) == 0x0102)
      { // 102h received, transmit 201h
        MsgBuf.Dat1 += 0xFF; // New ID is 0x201
        MsgBuf.DatA += 2;
        FullCAN_PushMessage(1,&MsgBuf);
      }
      else if ((MsgBuf.Dat1 & 0x07FF) == 0x0202)
      { // 202h received, transmit 101h
        MsgBuf.Dat1 -= 0x101; // New ID is 0x101
        MsgBuf.DatA -= 1;
        FullCAN_PushMessage(1,&MsgBuf);
      }
    } // Message on CAN 1 received

    // Check if message received on CAN 2
    if (FullCAN_PullMessage(2,&MsgBuf))
    { // Message was received
      if ((MsgBuf.Dat1 & 0x07FF) == 0x0101)
      { // 101h received, transmit 102h
        MsgBuf.Dat1 += 1; // New ID is 0x102
        MsgBuf.DatB += 2;
        FullCAN_PushMessage(2,&MsgBuf);
      }
      else if ((MsgBuf.Dat1 & 0x07FF) == 0x0201)
      { // 201h received, transmit 202h
        MsgBuf.Dat1 += 1; // New ID is 0x202
        MsgBuf.DatB -= 1;
        FullCAN_PushMessage(2,&MsgBuf);
      }
    } // Message on CAN 2 received

#if (SLOWDOWN > 0)
    SlowDown(SLOWDOWN);
#endif
    // LEDs off    
    IOCLR1 = LEDvalue;

  } // while(1)
}

/*----------------------- END OF FILE ----------------------------------*/
