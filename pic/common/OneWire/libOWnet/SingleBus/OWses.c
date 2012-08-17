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
BYTE SowAcquire(BYTE *port_zstr);
void SowRelease(void);
void SowSetCurrentPort(void);

// Global to speedup some things
OWNETWORKVARS  owNetCurrent;

//---------------------------------------------------------------------------
// Attempt to acquire a 1-Wire net
//
// 'port_zstr'  - not used
//
// Returns: TRUE - success, port opened
//
BYTE SowAcquire(BYTE *port_zstr)
{
#if !defined(OWNET_SENSE_PORTI) || !defined(OWNET_SENSE_PORTO)
     return(FALSE);
#endif
   
   // Init the hardware
// Sense
   OWNET_SENSE_TRIS  |=  (1 << OWNET_SENSE_PIN);

// Drive
#ifdef OWNET_DRIVE_PORT  
   #if OWNET_DRIVE_POLARITY == OWLEVEL_IDLE_LOW
     OWNET_DRIVE_PORT &= ~(1 << OWNET_DRIVE_PIN);
   #else
     OWNET_DRIVE_PORT |=  (1 << OWNET_DRIVE_PIN);
   #endif
   OWNET_DRIVE_TRIS &=  ~(1 << OWNET_DRIVE_PIN);
#else
   OWNET_SENSE_PORTO  &=  ~(1 << OWNET_SENSE_PIN);
#endif
  
// DPU
#ifdef OWNET_DPU_PORT
   #if OWNET_DPU_POLARITY == OWLEVEL_IDLE_LOW
     OWNET_DPU_PORT &= ~(1 << OWNET_DPU_PIN);
   #else
     OWNET_DPU_PORT |=  (1 << OWNET_DPU_PIN);
   #endif
   OWNET_DPU_TRIS &= ~(1 << OWNET_DPU_PIN);
#endif
   
// SPU
#ifdef OWNET_SPU_PORT
   #if OWNET_SPU_POLARITY == OWLEVEL_IDLE_LOW)
     OWNET_SPU_PORT &= ~(1 << OWNET_SPU_PIN);
   #else 
     OWNET_SPU_PORT |=  (1 << OWNET_SPU_PIN);
   #endif
   OWNET_SPU_TRIS &= ~(1 << OWNET_SPU_PIN);
#endif
   
// PPU
#ifdef OWNET_PPU_PORT  
   #if OWNET_PPU_POLARITY == OWLEVEL_IDLE_LOW
     OWNET_PPU_PORT &= ~(1 << OWNET_PPU_PIN);
   #else
     OWNET_PPU_PORT |=  (1 << OWNET_PPU_PIN);
   #endif
   OWNET_PPU_TRIS &= ~(1 << OWNET_PPU_PIN);
#endif
   
   owNetCurrent.CurrentLevel = MODE_NORMAL;
   
   return TRUE;
}

//---------------------------------------------------------------------------
// Release the previously acquired a 1-Wire net.
//
void owRelease(void)
{   
  // Nothing to do here
}
