/**************************************************************************
MODULE:    Main for LPC_CANAll
CONTAINS:  CAN Example using the LPC_CANAll Driver
           Tested with MCB2100 board from Keil Software
DOES:      
COPYRIGHT: Embedded Systems Academy, Inc. 2004.
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
           FOR KEIL SOFTWARE www.keil.com
VERSION:   1.10, Pf 05-JAN-05, Updates in accordance with errata sheets
---------------------------------------------------------------------------
HISTORY:   1.00, Pf 14-JUL-04, First published release
***************************************************************************/ 
                  
#include <LPC21xx.H> // LPC21xx definitions
#include "LPC_CANAll.h"


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
CANALL_MSG MsgBuf; // Buffers one CAN message
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
  CANAll_Init(1,0,CANBitrate125k_12MHz); 
  
  // CAN interface 2, use IRQVec1, at 125kbit
  CANAll_Init(2,1,CANBitrate125k_12MHz); 

  // Set CAN Err ISR to IRQVec2
  CANAll_SetErrIRQ(2);
  
  // Initialize Timer Interrupt
  T0MR0 = 5999; // 100 microseconds = 6.000-1 counts
  T0MCR = 3; // Interrupt and Reset on MR0
  T0TCR = 1;  // Timer0 Enable

  // Set Timer0 ISR to IRQVec3
  VICVectAddr3 = (unsigned long) Timer0ISR; // set interrupt vector
  VICVectCntl3 = 0x20 | 4;  // use it for Timer 0 Interrupt
  VICIntEnable = 0x00000010;  // enable Timer0 Interrupt

  // DEBUG HELP:
  // Wait for 10 milliseconds, to allow debug hardware to catch up
  SlowDown(100);

  // Initialize MsgBuf
  MsgBuf.Frame = 0x80080000L; // 29-bit, no RTR, DLC is 8 bytes
  MsgBuf.MsgID = 0x00012345L; // CAN ID
  MsgBuf.DatA = 0x00000000L; // all zeros
  MsgBuf.DatB = 0x00000000L; // all zeros
  // Transmit initial message on CAN 2
  CANAll_PushMessage(2,&MsgBuf);

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
    if (CANAll_PullMessage(1,&MsgBuf))
    { // Message was received
      if (MsgBuf.MsgID == 0x00012345L)
      { // 12345h received, transmit 54321h
        MsgBuf.MsgID = 0x00054321L; 
        MsgBuf.DatA++;;
        CANAll_PushMessage(1,&MsgBuf);
      }
    } // Message on CAN 1 received

    // Check if message received on CAN 2
    if (CANAll_PullMessage(2,&MsgBuf))
    { // Message was received
      if (MsgBuf.MsgID == 0x00054321L)
      { // 54321h received, transmit 102h
        MsgBuf.MsgID = 0x00012345L;
        MsgBuf.DatB++;
        CANAll_PushMessage(2,&MsgBuf);
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
