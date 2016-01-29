//---------------------------------------------------------------------------
// Copyright (C) 1999 Dallas Semiconductor Corporation, All Rights Reserved.
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
//  OWses.c - Acquire and release a Session Todo for general 1-Wire Net
//              library
//
//  Version: 2.00
//

#include "ownet.h"

// local function prototypes
BYTE owAcquire(BYTE portnum, BYTE *port_zstr);
void owRelease(BYTE portnum);
void owSetCurrentPort(BYTE portnum);

#if _SERIES == 18
  #define SETINPUT(p)  (&PORT##p)
  #define SETOUTPUT(p) ((&PORT##p-&PORTA)+&LATA)
  #define SETTRIS(p)   ((&PORT##p-&PORTA)+&TRISA)
#else
  #define SETINPUT(p)  (&PORT##p)
  #define SETOUTPUT(p) (&PORT##p)
  #define SETTRIS(p)   ((&PORT##p-&PORTA)+&TRISA)
#endif

// OneWire networks definitions
const OWNETWORKDEFS owNetDefs[] =
{
  #ifdef OWNET0_SENSE_PORT
    {
      #ifndef OWNET0_SENSE_POLARITY
        #define OWNET0_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET0_SENSE_PORT), SETOUTPUT(OWNET0_SENSE_PORT), SETTRIS(OWNET0_SENSE_PORT), OWNET0_SENSE_PIN, OWNET0_SENSE_POLARITY,
      #ifdef OWNET0_DRIVE_PORT
        #ifndef OWNET0_DRIVE_POLARITY
          #define OWNET0_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
        #endif
        SETOUTPUT(OWNET0_DRIVE_PORT), SETTRIS(OWNET0_DRIVE_PORT), OWNET0_DRIVE_PIN, OWNET0_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET0_DPU_PORT
        #ifndef OWNET0_DPU_POLARITY
	      #define OWNET0_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET0_DPU_PORT), SETTRIS(OWNET0_DPU_PORT), OWNET0_DPU_PIN, OWNET0_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET0_SPU_PORT
        #ifndef OWNET0_SPU_POLARITY
	      #define OWNET0_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET0_SPU_PORT), SETTRIS(OWNET0_SPU_PORT), OWNET0_SPU_PIN, OWNET0_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET0_PPU_PORT
        #ifndef OWNET0_PPU_POLARITY
	      #define OWNET0_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET0_PPU_PORT), SETTRIS(OWNET0_PPU_PORT), OWNET0_PPU_PIN, OWNET0_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 0
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif
      
  #ifdef OWNET1_SENSE_PORT
    },
    {
      #ifndef OWNET1_SENSE_POLARITY
        #define OWNET1_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET1_SENSE_PORT), SETOUTPUT(OWNET1_SENSE_PORT), SETTRIS(OWNET1_SENSE_PORT), OWNET1_SENSE_PIN, OWNET1_SENSE_POLARITY,
      #ifdef OWNET1_DRIVE_PORT
        #ifndef OWNET1_DRIVE_POLARITY
	      #define OWNET1_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET1_DRIVE_PORT), SETTRIS(OWNET1_DRIVE_PORT), OWNET1_DRIVE_PIN, OWNET1_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET1_DPU_PORT
        #ifndef OWNET1_DPU_POLARITY
	      #define OWNET1_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET1_DPU_PORT), SETTRIS(OWNET1_DPU_PORT), OWNET1_DPU_PIN, OWNET1_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET1_SPU_PORT
        #ifndef OWNET1_SPU_POLARITY
	      #define OWNET1_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET1_SPU_PORT), SETTRIS(OWNET1_SPU_PORT), OWNET1_SPU_PIN, OWNET1_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET1_PPU_PORT
        #ifndef OWNET1_PPU_POLARITY
	      #define OWNET1_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET1_PPU_PORT), SETTRIS(OWNET1_PPU_PORT), OWNET1_PPU_PIN, OWNET1_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 1
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif
    
  #ifdef OWNET2_SENSE_PORT
    },
    {
      #ifndef OWNET2_SENSE_POLARITY
        #define OWNET2_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET2_SENSE_PORT), SETOUTPUT(OWNET2_SENSE_PORT), SETTRIS(OWNET2_SENSE_PORT), OWNET2_SENSE_PIN, OWNET2_SENSE_POLARITY,
      #ifdef OWNET2_DRIVE_PORT
        #ifndef OWNET2_DRIVE_POLARITY
	      #define OWNET2_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET2_DRIVE_PORT), SETTRIS(OWNET2_DRIVE_PORT), OWNET2_DRIVE_PIN, OWNET2_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET2_DPU_PORT
        #ifndef OWNET2_DPU_POLARITY
	      #define OWNET2_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET2_DPU_PORT), SETTRIS(OWNET2_DPU_PORT), OWNET2_DPU_PIN, OWNET2_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET2_SPU_PORT
        #ifndef OWNET2_SPU_POLARITY
	      #define OWNET2_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET2_SPU_PORT), SETTRIS(OWNET2_SPU_PORT), OWNET2_SPU_PIN, OWNET2_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET2_PPU_PORT
        #ifndef OWNET2_PPU_POLARITY
	      #define OWNET2_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET2_PPU_PORT), SETTRIS(OWNET2_PPU_PORT), OWNET2_PPU_PIN, OWNET2_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 2
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif

  #ifdef OWNET3_SENSE_PORT
    },
    {
      #ifndef OWNET3_SENSE_POLARITY
        #define OWNET3_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET3_SENSE_PORT), SETOUTPUT(OWNET3_SENSE_PORT), SETTRIS(OWNET3_SENSE_PORT), OWNET3_SENSE_PIN, OWNET3_SENSE_POLARITY,
      #ifdef OWNET3_DRIVE_PORT
        #ifndef OWNET3_DRIVE_POLARITY
	      #define OWNET3_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET3_DRIVE_PORT), SETTRIS(OWNET3_DRIVE_PORT), OWNET3_DRIVE_PIN, OWNET3_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET3_DPU_PORT
        #ifndef OWNET3_DPU_POLARITY
	      #define OWNET3_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET3_DPU_PORT), SETTRIS(OWNET3_DPU_PORT), OWNET3_DPU_PIN, OWNET3_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET3_SPU_PORT
        #ifndef OWNET3_SPU_POLARITY
	      #define OWNET3_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET3_SPU_PORT), SETTRIS(OWNET3_SPU_PORT), OWNET3_SPU_PIN, OWNET3_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET3_PPU_PORT
        #ifndef OWNET3_PPU_POLARITY
	      #define OWNET3_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET3_PPU_PORT), SETTRIS(OWNET3_PPU_PORT), OWNET3_PPU_PIN, OWNET3_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 3
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif

  #ifdef OWNET4_SENSE_PORT
    },
    {
      #ifndef OWNET4_SENSE_POLARITY
        #define OWNET4_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET4_SENSE_PORT), SETOUTPUT(OWNET4_SENSE_PORT), SETTRIS(OWNET4_SENSE_PORT), OWNET4_SENSE_PIN, OWNET4_SENSE_POLARITY,
      #ifdef OWNET4_DRIVE_PORT
        #ifndef OWNET4_DRIVE_POLARITY
	      #define OWNET4_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET4_DRIVE_PORT), SETTRIS(OWNET4_DRIVE_PORT), OWNET4_DRIVE_PIN, OWNET4_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET4_DPU_PORT
        #ifndef OWNET4_DPU_POLARITY
	      #define OWNET4_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET4_DPU_PORT), SETTRIS(OWNET4_DPU_PORT), OWNET4_DPU_PIN, OWNET4_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET4_SPU_PORT
        #ifndef OWNET4_SPU_POLARITY
	      #define OWNET4_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET4_SPU_PORT), SETTRIS(OWNET4_SPU_PORT), OWNET4_SPU_PIN, OWNET4_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET4_PPU_PORT
        #ifndef OWNET4_PPU_POLARITY
	      #define OWNET4_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET4_PPU_PORT), SETTRIS(OWNET4_PPU_PORT), OWNET4_PPU_PIN, OWNET4_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 4
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif

  #ifdef OWNET5_SENSE_PORT
    },
    {
      #ifndef OWNET5_SENSE_POLARITY
        #define OWNET5_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET5_SENSE_PORT), SETOUTPUT(OWNET5_SENSE_PORT), SETTRIS(OWNET5_SENSE_PORT), OWNET5_SENSE_PIN, OWNET5_SENSE_POLARITY,
      #ifdef OWNET5_DRIVE_PORT
        #ifndef OWNET5_DRIVE_POLARITY
	      #define OWNET5_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET5_DRIVE_PORT), SETTRIS(OWNET5_DRIVE_PORT), OWNET5_DRIVE_PIN, OWNET5_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET5_DPU_PORT
        #ifndef OWNET5_DPU_POLARITY
	      #define OWNET5_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET5_DPU_PORT), SETTRIS(OWNET5_DPU_PORT), OWNET5_DPU_PIN, OWNET5_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET5_SPU_PORT
        #ifndef OWNET5_SPU_POLARITY
	      #define OWNET5_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET5_SPU_PORT), SETTRIS(OWNET5_SPU_PORT), OWNET5_SPU_PIN, OWNET5_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET5_PPU_PORT
        #ifndef OWNET5_PPU_POLARITY
	      #define OWNET5_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET5_PPU_PORT), SETTRIS(OWNET5_PPU_PORT), OWNET5_PPU_PIN, OWNET5_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 5
    },
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif

  #ifdef OWNET6_SENSE_PORT
    },
    {
      #ifndef OWNET6_SENSE_POLARITY
        #define OWNET6_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET6_SENSE_PORT), SETOUTPUT(OWNET6_SENSE_PORT), SETTRIS(OWNET6_SENSE_PORT), OWNET6_SENSE_PIN, OWNET6_SENSE_POLARITY,
      #ifdef OWNET6_DRIVE_PORT
        #ifndef OWNET6_DRIVE_POLARITY
	      #define OWNET6_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET6_DRIVE_PORT), SETTRIS(OWNET6_DRIVE_PORT), OWNET6_DRIVE_PIN, OWNET6_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET6_DPU_PORT
        #ifndef OWNET6_DPU_POLARITY
	      #define OWNET6_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET6_DPU_PORT), SETTRIS(OWNET6_DPU_PORT), OWNET6_DPU_PIN, OWNET6_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET6_SPU_PORT
        #ifndef OWNET6_SPU_POLARITY
	      #define OWNET6_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET6_SPU_PORT), SETTRIS(OWNET6_SPU_PORT), OWNET6_SPU_PIN, OWNET6_SPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET6_PPU_PORT
        #ifndef OWNET6_PPU_POLARITY
	      #define OWNET6_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET6_PPU_PORT), SETTRIS(OWNET6_PPU_PORT), OWNET6_PPU_PIN, OWNET6_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 6
    },
    {
      NULL, NULL, NULL, 0, 0, 
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
  #endif

  #ifdef OWNET7_SENSE_PORT
    },
    {
      #ifndef OWNET7_SENSE_POLARITY
        #define OWNET7_SENSE_POLARITY     OWLEVEL_IDLE_LOW
      #endif
      SETINPUT(OWNET7_SENSE_PORT), SETOUTPUT(OWNET7_SENSE_PORT), SETTRIS(OWNET7_SENSE_PORT), OWNET7_SENSE_PIN, OWNET7_SENSE_POLARITY,
      #ifdef OWNET7_DRIVE_PORT
        #ifndef OWNET7_DRIVE_POLARITY
	      #define OWNET7_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET7_DRIVE_PORT), SETTRIS(OWNET7_DRIVE_PORT), OWNET7_DRIVE_PIN, OWNET7_DRIVE_POLARITY,
      #else
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET7_DPU_PORT
        #ifndef OWNET7_DPU_POLARITY
	      #define OWNET7_DPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET7_DPU_PORT), SETTRIS(OWNET7_DPU_PORT), OWNET7_DPU_PIN, OWNET7_DPU_POLARITY,
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET7_SPU_PORT
        #ifndef OWNET7_SPU_POLARITY
	      #define OWNET7_SPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET7_SPU_PORT), SETTRIS(OWNET7_SPU_PORT), OWNET7_SPU_PIN, OWNET7_SPU_POLARITY
      #else 
        NULL, NULL, 0, 0,
      #endif
      #ifdef OWNET7_PPU_PORT
        #ifndef OWNET7_PPU_POLARITY
	      #define OWNET7_PPU_POLARITY     OWLEVEL_IDLE_LOW
	    #endif
        SETOUTPUT(OWNET7_PPU_PORT), SETTRIS(OWNET7_PPU_PORT), OWNET7_PPU_PIN, OWNET7_PPU_POLARITY
      #else 
        NULL, NULL, 0, 0
      #endif
  #elif MAX_PORTNUM > 7
    },
    {
      NULL, NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0,
      NULL, NULL, 0, 0
    #endif
  }
};

// Globals to speedup some things
BYTE           owCurrentPortnum = 0xFF;
OWNETWORKVARS  owNetVars[MAX_PORTNUM];
OWNETWORKVARS  owNetCurrent;
OWNETWORKDEFS  owNetCurrentDefs;

//---------------------------------------------------------------------------
// Attempt to acquire a 1-Wire net
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// 'port_zstr'  - not used
//
// Returns: TRUE - success, port opened
//
BYTE owAcquire(BYTE portnum, BYTE *port_zstr)
{
   owSetCurrentPort(portnum);

   if(owNetCurrentDefs.SensePort == NULL) {
     return(FALSE);
   }
   
   // Init the hardware
// Sense
   *owNetCurrentDefs.SenseTris  |=  (1 << owNetCurrentDefs.SensePin);

// Drive
#ifdef OWNET_DRIVE_USED  
   if(owNetCurrentDefs.DrivePort == NULL) {
     *owNetCurrentDefs.SensePortO &= ~(1 << owNetCurrentDefs.SensePin);
   } else {
     if(owNetCurrentDefs.DrivePolarity == OWLEVEL_IDLE_LOW) {
       *owNetCurrentDefs.DrivePort &= ~(1 << owNetCurrentDefs.DrivePin);
     } else {
       *owNetCurrentDefs.DrivePort |=  (1 << owNetCurrentDefs.DrivePin);
     }
     *owNetCurrentDefs.DriveTris &=  ~(1 << owNetCurrentDefs.DrivePin);
   }
#else
   *owNetCurrentDefs.SensePortO &= ~(1 << owNetCurrentDefs.SensePin);
#endif
  
// DPU
#ifdef OWNET_DPU_USED  
   if(owNetCurrentDefs.DPUPort != NULL) {
     if(owNetCurrentDefs.DPUPolarity == OWLEVEL_IDLE_LOW) {
       *owNetCurrentDefs.DPUPort &= ~(1 << owNetCurrentDefs.DPUPin);
     } else {
       *owNetCurrentDefs.DPUPort |=  (1 << owNetCurrentDefs.DPUPin);
     }
     *owNetCurrentDefs.DPUTris &= ~(1 << owNetCurrentDefs.DPUPin);
   }
#endif
   
// SPU
#ifdef OWNET_SPU_USED  
   if(owNetCurrentDefs.SPUPort != NULL) {
     if(owNetCurrentDefs.SPUPolarity == OWLEVEL_IDLE_LOW) {
       *owNetCurrentDefs.SPUPort &= ~(1 << owNetCurrentDefs.SPUPin);
     } else {
       *owNetCurrentDefs.SPUPort |=  (1 << owNetCurrentDefs.SPUPin);
     }
     *owNetCurrentDefs.SPUTris &= ~(1 << owNetCurrentDefs.SPUPin);
   }
#endif
   
// PPU
#ifdef OWNET_PPU_USED  
   if(owNetCurrentDefs.PPUPort != NULL) {
     if(owNetCurrentDefs.PPUPolarity == OWLEVEL_IDLE_LOW) {
       *owNetCurrentDefs.PPUPort &= ~(1 << owNetCurrentDefs.PPUPin);
     } else {
       *owNetCurrentDefs.PPUPort |=  (1 << owNetCurrentDefs.PPUPin);
     }
     *owNetCurrentDefs.PPUTris &= ~(1 << owNetCurrentDefs.PPUPin);
   }
#endif
   
   owNetCurrent.CurrentLevel = MODE_NORMAL;
   
   return TRUE;
}

//---------------------------------------------------------------------------
// Release the previously acquired a 1-Wire net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
void owRelease(BYTE portnum)
{   
  // Nothing to do here
}

//---------------------------------------------------------------------------
// Set the current 1-Wire net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
void owSetCurrentPort(BYTE portnum)
{  
   if(portnum != owCurrentPortnum) {
     if(owCurrentPortnum < MAX_PORTNUM) {
       owNetVars[owCurrentPortnum] = owNetCurrent;
     }
     
     owCurrentPortnum = portnum;
     owNetCurrentDefs = owNetDefs[owCurrentPortnum];
     owNetCurrent = owNetVars[owCurrentPortnum];
   }
}

