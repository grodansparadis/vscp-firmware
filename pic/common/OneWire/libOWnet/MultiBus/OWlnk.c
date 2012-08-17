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
BYTE owTouchReset(BYTE portnum);
BYTE owTouchBit(BYTE portnum, BYTE sendbit);
BYTE owTouchByte(BYTE portnum, BYTE sendbyte);
BYTE owWriteByte(BYTE portnum, BYTE sendbyte);
BYTE owReadByte(BYTE portnum);
BYTE owSpeed(BYTE portnum, BYTE new_speed);
BYTE owLevel(BYTE portnum, BYTE new_level);
BYTE owProgramPulse(BYTE portnum);
BYTE owWriteBytePower(BYTE portnum, BYTE sendbyte);
BYTE owReadBitPower(BYTE portnum, BYTE applyPowerResponse);
BYTE owHasPowerDelivery(BYTE portnum);
BYTE owHasOverDrive(BYTE portnum);
BYTE owHasProgramPulse(BYTE portnum);

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
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
BYTE owTouchReset(BYTE portnum)
{
  BYTE result = 0;
  
  owSetCurrentPort(portnum);

#ifdef OWNET_DPU_USED  
  if(owNetCurrentDefs.DPUPort != NULL) {  // Dynamic PullUp?
    // Disable it
    if(owNetCurrentDefs.DPUPolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DPUPort &= ~(1 << owNetCurrentDefs.DPUPin);
    } else {
      *owNetCurrentDefs.DPUPort |=  (1 << owNetCurrentDefs.DPUPin);
    }
  }
#endif

#ifdef OWNET_DRIVE_USED  
  if(owNetCurrentDefs.DrivePort == NULL) {
    // Use Sense for driving the net
    *owNetCurrentDefs.SenseTris &= ~(1 << owNetCurrentDefs.SensePin);
    tickDelay(H);
    *owNetCurrentDefs.SenseTris |=  (1 << owNetCurrentDefs.SensePin);
  } else {
    // Use Drive for driving the net
    if(owNetCurrentDefs.DrivePolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DrivePort |=  (1 << owNetCurrentDefs.DrivePin);
      tickDelay(H);
      *owNetCurrentDefs.DrivePort &= ~(1 << owNetCurrentDefs.DrivePin);
    } else {
      *owNetCurrentDefs.DrivePort &= ~(1 << owNetCurrentDefs.DrivePin);
      tickDelay(H);
      *owNetCurrentDefs.DrivePort |=  (1 << owNetCurrentDefs.DrivePin);
    }
  }
#else
  // Use Sense for driving the net
  *owNetCurrentDefs.SenseTris &= ~(1 << owNetCurrentDefs.SensePin);
  tickDelay(H);
  *owNetCurrentDefs.SenseTris |=  (1 << owNetCurrentDefs.SensePin);
#endif

#ifdef OWNET_DPU_USED  
  if(owNetCurrentDefs.DPUPort != NULL) {  // Dynamic PullUp?
    // Enable it
    if(owNetCurrentDefs.DPUPolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DPUPort |=  (1 << owNetCurrentDefs.DPUPin);
    } else {
      *owNetCurrentDefs.DPUPort &= ~(1 << owNetCurrentDefs.DPUPin);
    }
  }
#endif
  
  tickDelay(I);
  
  // Do we have a presencepulse?
  result = (*owNetCurrentDefs.SensePortI & (1 << owNetCurrentDefs.SensePin)) ? FALSE : TRUE;
  tickDelay(J);
  
  return(result);
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbit'    - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
BYTE owTouchBit(BYTE portnum, BYTE sendbit)
{
  BYTE result = 0;
  
  owSetCurrentPort(portnum);

// Drive the bus low
#ifdef OWNET_DPU_USED  
  if(owNetCurrentDefs.DPUPort != NULL) {  // Dynamic PullUp?
    // Disable it
    if(owNetCurrentDefs.DPUPolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DPUPort &= ~(1 << owNetCurrentDefs.DPUPin);
    } else {
      *owNetCurrentDefs.DPUPort |=  (1 << owNetCurrentDefs.DPUPin);
    }
  }
#endif

#ifdef OWNET_DRIVE_USED  
  if(owNetCurrentDefs.DrivePort == NULL) {
    // Use Sense for driving the net
    *owNetCurrentDefs.SenseTris &= ~(1 << owNetCurrentDefs.SensePin);
  } else {
    // Use Drive for driving the net
    if(owNetCurrentDefs.DrivePolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DrivePort |=  (1 << owNetCurrentDefs.DrivePin);
    } else {
      *owNetCurrentDefs.DrivePort &= ~(1 << owNetCurrentDefs.DrivePin);
    }
  }
#else
  // Use Sense for driving the net
  *owNetCurrentDefs.SenseTris &= ~(1 << owNetCurrentDefs.SensePin);
#endif

  if(sendbit & 0x01) {     // Only lsb   
    // Write a '1' bit
    tickDelay(A);
  } else {
    // Write a '0' bit
    tickDelay(C);
  }

// Drive the bus high again
#ifdef OWNET_DRIVE_USED  
  if(owNetCurrentDefs.DrivePort == NULL) {
    // Use Sense for driving the net
    *owNetCurrentDefs.SenseTris |=  (1 << owNetCurrentDefs.SensePin);
  } else {
    // Use Drive for driving the net
    if(owNetCurrentDefs.DrivePolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DrivePort &= ~(1 << owNetCurrentDefs.DrivePin);
    } else {
      *owNetCurrentDefs.DrivePort |=  (1 << owNetCurrentDefs.DrivePin);
    }
  }
#else
  // Use Sense for driving the net
  *owNetCurrentDefs.SenseTris |=  (1 << owNetCurrentDefs.SensePin);
#endif

#ifdef OWNET_DPU_USED  
  if(owNetCurrentDefs.DPUPort != NULL) {  // Dynamic PullUp?
    // Enable it
    if(owNetCurrentDefs.DPUPolarity == OWLEVEL_IDLE_LOW) {
      *owNetCurrentDefs.DPUPort |=  (1 << owNetCurrentDefs.DPUPin);
    } else {
      *owNetCurrentDefs.DPUPort &= ~(1 << owNetCurrentDefs.DPUPin);
    }
  }
#endif

  if(sendbit & 0x01) {     // Only lsb   
    tickDelay(E);
    // Read a bit
    result = (*owNetCurrentDefs.SensePortI & (1 << owNetCurrentDefs.SensePin)) ? 0x01 : 0x00;
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
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bytes read from sendbyte
//
BYTE owTouchByte(BYTE portnum, BYTE sendbyte)
{
  BYTE loop;
  BYTE result = 0;
  
  owSetCurrentPort(portnum);

  for (loop = 8; loop; loop--) {
    // shift the result to get it ready for the next bit
    result >>= 1;
    // Send the bit, get the read bit
    if (owTouchBit(owCurrentPortnum, sendbyte)) {
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
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
BYTE owWriteByte(BYTE portnum, BYTE sendbyte)
{
   owSetCurrentPort(portnum);
   
   return((owTouchByte(owCurrentPortnum, sendbyte) == sendbyte) ? TRUE : FALSE);
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
BYTE owReadByte(BYTE portnum)
{
   owSetCurrentPort(portnum);
   
   return(owTouchByte(owCurrentPortnum, 0xFF));
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
BYTE owSpeed(BYTE portnum, BYTE new_speed)
{
  owSetCurrentPort(portnum);

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
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_level'  - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//                MODE_PROGRAM    0x04
//
// Returns:  current 1-Wire Net level
//
BYTE owLevel(BYTE portnum, BYTE new_level)
{
  owSetCurrentPort(portnum);
  
  switch(new_level) {
#ifdef OWNET_SPU_USED  
  case MODE_STRONG5:
      if(owNetCurrentDefs.SPUPolarity == OWLEVEL_IDLE_LOW) {
        *owNetCurrentDefs.SPUPort |=  (1 << owNetCurrentDefs.SPUPin);
      } else {
        *owNetCurrentDefs.SPUPort &= ~(1 << owNetCurrentDefs.SPUPin);
      }
      owNetCurrent.CurrentLevel |= MODE_STRONG5;
      break;
#endif
#ifdef OWNET_PPU_USED  
  case MODE_PROGRAM:
      if(owNetCurrentDefs.PPUPolarity == OWLEVEL_IDLE_LOW) {
        *owNetCurrentDefs.PPUPort |=  (1 << owNetCurrentDefs.PPUPin);
      } else {
        *owNetCurrentDefs.PPUPort &= ~(1 << owNetCurrentDefs.PPUPin);
      }
      owNetCurrent.CurrentLevel |= MODE_PROGRAM;
      break;
#endif
  case MODE_NORMAL: // NOBREAK
  default:
#ifdef OWNET_SPU_USED  
      if(owNetCurrentDefs.SPUPolarity == OWLEVEL_IDLE_LOW) {
        *owNetCurrentDefs.SPUPort &= ~(1 << owNetCurrentDefs.SPUPin);
      } else {
        *owNetCurrentDefs.SPUPort |=  (1 << owNetCurrentDefs.SPUPin);
      }
#endif
#ifdef OWNET_PPU_USED  
      if(owNetCurrentDefs.PPUPolarity == OWLEVEL_IDLE_LOW) {
        *owNetCurrentDefs.PPUPort &= ~(1 << owNetCurrentDefs.PPUPin);
      } else {
        *owNetCurrentDefs.PPUPort |=  (1 << owNetCurrentDefs.PPUPin);
      }
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
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
BYTE owProgramPulse(BYTE portnum)
{
#ifdef OWNET_PPU_USED  
  owSetCurrentPort(portnum);

  if(!owHasProgramPulse(owCurrentPortnum)) {
    return(FALSE);
  }
  
  if(owNetCurrentDefs.PPUPolarity == OWLEVEL_IDLE_LOW) {
    *owNetCurrentDefs.PPUPort |=  (1 << owNetCurrentDefs.PPUPin);
    _Pragma(asmline DELAY (480000/CPUcycle))
    *owNetCurrentDefs.PPUPort &= ~(1 << owNetCurrentDefs.PPUPin);
  } else {
    *owNetCurrentDefs.PPUPort &= ~(1 << owNetCurrentDefs.PPUPin);
    _Pragma(asmline DELAY (480000/CPUcycle))
    *owNetCurrentDefs.PPUPort |=  (1 << owNetCurrentDefs.PPUPin);
  }
  
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
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same 
//
BYTE owWriteBytePower(BYTE portnum, BYTE sendbyte)
{
#ifdef OWNET_SPU_USED  
   owSetCurrentPort(portnum);

   // replace if platform has better implementation (faster response)
   if (!owHasPowerDelivery(owCurrentPortnum)) {
      return FALSE;
   }
   
   if(owTouchByte(owCurrentPortnum, sendbyte) != sendbyte) {
      return FALSE;
   }

   if(owLevel(owCurrentPortnum, MODE_STRONG5) != MODE_STRONG5) {
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
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery 
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
BYTE owReadBitPower(BYTE portnum, BYTE applyPowerResponse)
{
#ifdef OWNET_SPU_USED  
   owSetCurrentPort(portnum);
   
   // replace if platform has better implementation (faster response)
   if (!owHasPowerDelivery(owCurrentPortnum))
      return FALSE;

   if(owTouchBit(owCurrentPortnum, 0x01) != applyPowerResponse)
      return FALSE;

   if(owLevel(owCurrentPortnum, MODE_STRONG5) != MODE_STRONG5)
      return FALSE;

   return TRUE;
#else
   return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// This procedure indicates wether we can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of delivering power. 
//
BYTE owHasPowerDelivery(BYTE portnum)
{
#ifdef OWNET_SPU_USED  
  owSetCurrentPort(portnum);

  if(owNetCurrentDefs.SPUPort == NULL) {
    return(FALSE);
  }
  return(TRUE);
#else
  return(FALSE);
#endif
}

//--------------------------------------------------------------------------
// This procedure indicates wether we support OverDrive.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  always TRUE, because that is how it is. :-)
//
BYTE owHasOverDrive(BYTE portnum)
{
  owSetCurrentPort(portnum);

  return TRUE;
}
//--------------------------------------------------------------------------
// This procedure indicates wether we can generate a programpulse 
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available  
BYTE owHasProgramPulse(BYTE portnum)
{
#ifdef OWNET_PPU_USED  
  owSetCurrentPort(portnum);

  if(owNetCurrentDefs.PPUPort == NULL) {
    return FALSE;
  }
  return(TRUE);
#else
  return(FALSE);
#endif
}
