/**************************************************************************
MODULE:    BlinkCAN - Main
CONTAINS:  CAN Example program for MCB2100 board from 
           Keil Software
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2004.
           All rights reserved. See www.microcanopen.com for more...
           This software was written in accordance to the guidelines at
           www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
           FOR KEIL SOFTWARE www.keil.com
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
		       BASED ON BlinkyIRQ PROVIDED BY KEIL SOFTWARE
VERSION:   1.22, Pf 18-MAR-04
---------------------------------------------------------------------------
HISTORY:   1.23, Pf 22-MAR-04, Release for Philips LPC2129
***************************************************************************/ 
                  
#include <LPC21xx.H> // LPC21xx definitions
#include "mco.h"
#include "mcohw.h"

// Timer Ticks
extern WORD gTimCnt;

// Process data communicated via CAN
extern BYTE gProcImg[PROCIMG_SIZE];

// Execute an A to D Conversion
DWORD getAD 
  (
    void
  ) 
{    
  DWORD val;

  ADCR |= 0x01000000; // Start A/D Conversion

  do 
  {
    val = ADDR; // Read A/D Data Register
  } 
  while ((val & 0x80000000) == 0); // Wait for end of A/D Conversion

  ADCR &= ~0x01000000; // Stop A/D Conversion
  val = (val >> 6) & 0x03FF; // Extract AIN0 Value

  return val;
}

// Dynamic wait
// While waiting update process data and call CANopen Stack
void wait 
  (
  void
  )  
{
  DWORD ana;
  WORD comp;

  // Receive analog data from process image
  ana = gProcImg[OUT_ana_1+1];
  ana <<= 8;
  ana += gProcImg[OUT_ana_1];
  comp = MCOHW_GetTime() + 10 + (ana >> 2);

  while (!MCOHW_IsTimeExpired(comp)) // Variable wait   
  {
    MCO_ProcessStack();

    // Get analog input from ADC
    ana = getAD();
    gProcImg[IN_ana_1]   = (BYTE) ana;
    gProcImg[IN_ana_1+1] = (BYTE) (ana >> 8);

    // Echo analog output received
    gProcImg[IN_ana_2]   = gProcImg[OUT_ana_1];
    gProcImg[IN_ana_2+1] = gProcImg[OUT_ana_1+1];

    // Transmit timer values
    gProcImg[IN_digi_1] = (BYTE) comp;
    gProcImg[IN_digi_2] = (BYTE) (comp >> 8);
    gProcImg[IN_digi_3] = (BYTE) gTimCnt;
    gProcImg[IN_digi_4] = (BYTE) (gTimCnt >> 8);

    // Calculate next compare value
    ana = gProcImg[OUT_ana_1+1];
    ana <<= 8;
    ana += gProcImg[OUT_ana_1];
  }
}


int main 
  (
  void
  ) 
{
DWORD j; // LED var

  // No divider: peripheral clock = processor clock
  VPBDIV = 1; 
    
  // Init Vector Interrupt Controller
  VICIntEnClr = 0xFFFFFFFF; // Disable all Ints
  VICIntSelect = 0x00000000;


  IODIR1 = 0x00FF0000; // P1.16..23 defined as Outputs
  ADCR   = 0x00270401; // Setup A/D: 10-bit AIN0 @ 3MHz 

  // Initialisation of MicroCANopen
  MCOUSER_ResetCommunication();

  while (1)  
  {                                             // Loop forever 
    for (j = 0x010000; j < 0x800000; j <<= 1) { // Blink LED 0,1,2,3,4,5,6
      IOSET1 = j;                               // Turn on LED
      wait ();                                  // call wait function
      IOCLR1 = j;                               // Turn off LED
    }
    for (j = 0x800000; j > 0x010000; j >>=1 ) { // Blink LED 7,6,5,4,3,2,1
      IOSET1 = j;                               // Turn on LED
      wait ();                                  // call wait function
      IOCLR1 = j;                               // Turn off LED
    }
  }
}
