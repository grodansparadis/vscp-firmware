//---------------------------------------------------------------------------
// Copyright (C) 2001 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
/*********************************************************************
** This port of the OneWire libraries for wizC and fedC is created by:
** (C) 2006 HMCS / Marcel van Lieshout
**
** wizC/fedC are (C) Forest Electronic Developments.
*********************************************************************/
//
//  OWlnk.c - Link Layer functions required by general 1-Wire drive
//              implimentation.  Fill in the platform specific code.
//
//  Version: 3.00
//

#include <delays.h>
#include "ownet.h"

// exportable link-level functions
BYTE SowTouchReset(void);
BYTE SowTouchBit(BYTE sendbit);
BYTE SowTouchByte(BYTE sendbyte);
BYTE SowWriteByte(BYTE sendbyte);
BYTE SowReadByte(void);
BYTE SowSpeed(BYTE new_speed);
BYTE SowLevel(BYTE new_level);
BYTE SowProgramPulse(void);
BYTE SowWriteBytePower(BYTE sendbyte);
BYTE SowReadBitPower(BYTE applyPowerResponse);
BYTE SowHasPowerDelivery(void);
BYTE SowHasOverDrive(void);
BYTE SowHasProgramPulse(void);

#define tickDelay(t)              \
  if(owNetCurrent.CurrentLevel & MODE_OVERDRIVE) { \
    _Pragma(asmline DELAY t##od)  \
  } else {                        \
    _Pragma(asmline DELAY t##ns)  \
  }

#define CPUcycle      (1000000000/(APROCFREQ/4))
// Values in nanoseconds/cycletime = #CPUcycles
// Normal speed
#define Ans   (6000/CPUcycle)
#define Bns  (64000/CPUcycle)
#define Cns  (60000/CPUcycle)
#define Dns  (10000/CPUcycle)
#define Ens   (9000/CPUcycle)
#define Fns  (55000/CPUcycle)
#define Gns      (0/CPUcycle)
#define Hns (480000/CPUcycle)
#define Ins  (70000/CPUcycle)
#define Jns (410000/CPUcycle)
// Overdrive Speed
#define Aod   (1500/CPUcycle)
#define Bod   (7500/CPUcycle)
#define Cod   (7500/CPUcycle)
#define Dod   (2500/CPUcycle)
#define Eod    (750/CPUcycle)
#define Fod   (7000/CPUcycle)
#define God   (2500/CPUcycle)
#define Hod  (70000/CPUcycle)
#define Iod   (8500/CPUcycle)
#define Jod  (40000/CPUcycle)


//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
BYTE SowTouchReset(void)
{
  BYTE result = 0;
  
#ifdef OWNET_DPU_PORT
  // Disable it
  #if OWNET_DPU_POLARITY == OWLEVEL_IDLE_LOW
    OWNET_DPU_PORT &= ~(1 << OWNET_DPU_PIN);
  #else
    OWNET_DPU_PORT |=  (1 << OWNET_DPU_PIN);
  #endif
#endif

#ifdef OWNET_DRIVE_PORT  
  // Use Drive for driving the net
  #if OWNET_DRIVE_POLARITY == OWLEVEL_IDLE_LOW
      OWNET_DRIVE_PORT |=  (1 << OWNET_DRIVE_PIN);
      tickDelay(H);
      OWNET_DRIVE_PORT &= ~(1 << OWNET_DRIVE_PIN);
  #else
      OWNET_DRIVE_PORT &= ~(1 << OWNET_DRIVE_PIN);
      tickDelay(H);
      OWNET_DRIVE_PORT |=  (1 << OWNET_DRIVE_PIN);
  #endif
  }
#else
  // Use Sense for driving the net
  OWNET_SENSE_TRIS &= ~(1 << OWNET_SENSE_PIN);
  tickDelay(H);
  OWNET_SENSE_TRIS |=  (1 << OWNET_SENSE_PIN);
#endif

#ifdef OWNET_DPU_PORT  
  // Enable it
  #if OWNET_DPU_POLARITY == OWLEVEL_IDLE_LOW
      OWNET_DPU_PORT |=  (1 << OWNET_DPU_PIN);
  #else
      OWNET_DPU_PORT &= ~(1 << OWNET_DPU_PIN);
  #endif
#endif
  
  tickDelay(I);
  
  // Do we have a presencepulse?
  result = (OWNET_SENSE_PORTI & (1 << OWNET_SENSE_PIN)) ? FALSE : TRUE;
  tickDelay(J);
  
  return(result);
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
BYTE SowTouchBit(BYTE sendbit)
{
  BYTE result = 0;
  
// Drive the bus low
#ifdef OWNET_DPU_PORT
  // Disable it
  #if OWNET_DPU_POLARITY == OWLEVEL_IDLE_LOW
    OWNET_DPU_PORT &= ~(1 << OWNET_DPU_PIN);
  #else
    OWNET_DPU_PORT |=  (1 << OWNET_DPU_PIN);
  #endif
#endif

#ifdef OWNET_DRIVE_PORT  
  // Use Drive for driving the net
  #if OWNET_DRIVE_POLARITY == OWLEVEL_IDLE_LOW
      OWNET_DRIVE_PORT |=  (1 << OWNET_DRIVE_PIN);
  #else
      OWNET_DRIVE_PORT &= ~(1 << OWNET_DRIVE_PIN);
  #endif
  }
#else
  // Use Sense for driving the net
  OWNET_SENSE_TRIS &= ~(1 << OWNET_SENSE_PIN);
#endif

  if(sendbit & 0x01) {     // Only lsb   
    // Write a '1' bit
    tickDelay(A);
  } else {
    // Write a '0' bit
    tickDelay(C);
  }

// Drive the bus high again
#ifdef OWNET_DRIVE_PORT  
  // Use Drive for driving the net
  #if OWNET_DRIVE_POLARITY == OWLEVEL_IDLE_LOW
      OWNET_DRIVE_PORT &= ~(1 << OWNET_DRIVE_PIN);
  #else
      OWNET_DRIVE_PORT |=  (1 << OWNET_DRIVE_PIN);
  #endif
  }
#else
  // Use Sense for driving the net
  OWNET_SENSE_TRIS |=  (1 << OWNET_SENSE_PIN);
#endif

#ifdef OWNET_DPU_PORT  
  // Enable it
  #if OWNET_DPU_POLARITY == OWLEVEL_IDLE_LOW
      OWNET_DPU_PORT |=  (1 << OWNET_DPU_PIN);
  #else
      OWNET_DPU_PORT &= ~(1 << OWNET_DPU_PIN);
  #endif
#endif


  if(sendbit & 0x01) {     // Only lsb   
    tickDelay(E);
    // Read a bit
    result = (OWNET_SENSE_PORTI & (1 << OWNET_SENSE_PIN)) ? 0x01 : 0x00;
    tickDelay(F);
  } else {
    tickDelay(D);
  }
  
  return(result);
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bytes read from sendbyte
//
BYTE SowTouchByte(BYTE sendbyte)
{
  BYTE loop;
  BYTE result = 0;
  
  for (loop = 8; loop; loop--) {
    // shift the result to get it ready for the next bit
    result >>= 1;
    // Send the bit, get the read bit
    if (SowTouchBit(sendbyte)) {
      result |= 0x80;
    }
     
    // shift the data byte for the next bit
    sendbyte >>= 1;

  }
  return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
BYTE SowWriteByte(BYTE sendbyte)
{
   return((SowTouchByte(sendbyte) == sendbyte) ? TRUE : FALSE);
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// Returns:  8 bytes read from 1-Wire Net
//
BYTE SowReadByte(void)
{
   return(SowTouchByte(0xFF));
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
BYTE SowSpeed(BYTE new_speed)
{
  if(new_speed == MODE_OVERDRIVE) {
    owNetCurrent.CurrentLevel |= MODE_OVERDRIVE;
  } else {
    owNetCurrent.CurrentLevel &= ~MODE_OVERDRIVE;
  }
  
  return(new_speed);
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for NewLevel are
// as follows:
//
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//
// Returns:  current 1-Wire Net level
//
BYTE SowLevel(BYTE new_level)
{
  switch(new_level) {
#ifdef OWNET_SPU_PORT  
  case MODE_STRONG5:
      #if OWNET_SPU_POLARITY == OWLEVEL_IDLE_LOW
        OWNET_SPU_PORT |=  (1 << OWNET_SPU_PIN);
      #else
        OWNET_SPU_PORT &= ~(1 << OWNET_SPU_PIN);
      #endif
      owNetCurrent.CurrentLevel |= MODE_STRONG5;
      break;
#endif
#ifdef OWNET_PPU_PORT  
  case MODE_PROGRAM:
      #if OWNET_PPU_POLARITY == OWLEVEL_IDLE_LOW
        OWNET_PPU_PORT |=  (1 << OWNET_PPU_PIN);
      #else
        OWNET_PPU_PORT &= ~(1 << OWNET_PPU_PIN);
      #endif
      owNetCurrent.CurrentLevel |= MODE_PROGRAM;
      break;
#endif
  case MODE_NORMAL: // NOBREAK
  default:
#ifdef OWNET_SPU_PORT  
      #if OWNET_SPU_POLARITY == OWLEVEL_IDLE_LOW
        OWNET_SPU_PORT &= ~(1 << OWNET_SPU_PIN);
      #else
        OWNET_SPU_PORT |=  (1 << OWNET_SPU_PIN);
      #endif
#endif
#ifdef OWNET_PPU_PORT  
      #if OWNET_PPU_POLARITY == OWLEVEL_IDLE_LOW
        OWNET_PPU_PORT &= ~(1 << OWNET_PPU_PIN);
      #else
        OWNET_PPU_PORT |=  (1 << OWNET_PPU_PIN);
      #endif
#endif
      owNetCurrent.CurrentLevel &= ~(MODE_STRONG5 | MODE_PROGRAM);
      break;
  }
      
  return(owNetCurrent.CurrentLevel & new_level);
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
BYTE SowProgramPulse(void)
{
#ifdef OWNET_PPU_PORT
  #if OWNET_PPU_POLARITY == OWLEVEL_IDLE_LOW
    OWNET_PPU_PORT |=  (1 << OWNET_PPU_PIN);
    _Pragma(asmline DELAY (480000/CPUcycle))
    OWNET_PPU_PORT &= ~(1 << OWNET_PPU_PIN);
  #else
    OWNET_PPU_PORT &= ~(1 << OWNET_PPU_PIN);
    _Pragma(asmline DELAY (480000/CPUcycle))
    OWNET_PPU_PORT |=  (1 << OWNET_PPU_PIN);
  #endif
  
  return(TRUE);
#else
  return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).  
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same 
//
BYTE SowWriteBytePower(BYTE sendbyte)
{
#ifdef OWNET_SPU_PORT  
   if(SowTouchByte(sendbyte) != sendbyte) {
      return FALSE;
   }

   if(SowLevel(MODE_STRONG5) != MODE_STRONG5) {
      return FALSE;
   }
   
   return TRUE;
#else
   return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and verify that the
// response matches the 'applyPowerResponse' bit and apply power delivery
// to the 1-Wire net.  Note that some implementations may apply the power
// first and then turn it off if the response is incorrect.
//
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery 
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
BYTE SowReadBitPower(BYTE applyPowerResponse)
{
#ifdef OWNET_SPU_PORT  
   if(SowTouchBit(0x01) != applyPowerResponse)
      return FALSE;

   if(SowLevel(MODE_STRONG5) != MODE_STRONG5)
      return FALSE;

   return TRUE;
#else
   return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// This procedure indicates wether we can deliver power.
//
// Returns:  TRUE  if adapter is capable of delivering power. 
//
BYTE owHasPowerDelivery(void)
{
#ifdef OWNET_SPU_PORT  
  return(TRUE);
#else
  return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// This procedure indicates wether we support OverDrive.
//
// Returns:  always TRUE, because that is how it is. :-)
//
BYTE owHasOverDrive(void)
{
  return TRUE;
}
//--------------------------------------------------------------------------
// This procedure indicates wether we can generate a programpulse 
// on the 1-Wire Net for programming EPROM iButtons.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available  
BYTE owHasProgramPulse(void)
{
#ifdef OWNET_PPU_PORT  
  return(TRUE);
#else
  return(FALSE);
#endif
}
