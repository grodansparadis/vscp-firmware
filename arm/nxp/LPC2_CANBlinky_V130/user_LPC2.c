/**************************************************************************
MODULE:    USER
CONTAINS:  MicroCANopen Object Dictionary and Process Image implementation
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2004.
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
           www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   THIS VERSION CREATED FOR FREE DISTRIBUTION
           FOR KEIL SOFTWARE www.keil.com
		       FOR PHILIPS SEMICONDUCTORS www.philipsmcu.com
VERSION:   1.21, Pf 02-FEB-04
---------------------------------------------------------------------------
HISTORY:   1.21, Pf 02-FEB-04, Release for Philips LPC2129
           1.20, Pf 19-AUG-03, First Published Version
           Functionality in this module was located in module MCO in
		     previous releases.
***************************************************************************/ 

#include "mco.h"
#include "mcohw.h"
#include "string.h"

#if (NR_OF_RPDOS != 2)
  #if (NR_OF_TPDOS != 2)
ERROR: This example is for 2 TPDOs and 2 RPDOs only
  #endif
#endif

// Global timer/conter variable, incremented every millisecond
extern WORD volatile gTimCnt;

/**************************************************************************
MACROS FOR OBJECT DICTIONARY ENTRIES
***************************************************************************/ 

#define GETBYTE(val,pos) ((val >> pos) & 0xFF)
#define GETBYTES16(val) GETBYTE(val, 0), GETBYTE(val, 8)
#define GETBYTES32(val) GETBYTE(val, 0), GETBYTE(val, 8), GETBYTE(val,16), GETBYTE(val,24)
#define SDOREPLY(index,sub,len,val) 0x43 | ((4-len)<<2), GETBYTES16(index), sub, GETBYTES32(val)
#define SDOREPLY4(index,sub,len,d1,d2,d3,d4) 0x43 | ((4-len)<<2), GETBYTES16(index), sub, d1, d2, d3, d4

#define ODENTRY(index,sub,len,offset) {index, sub, len, offset}

/**************************************************************************
GLOBAL VARIABLES
***************************************************************************/ 

// This structure holds all node specific configuration
BYTE volatile gProcImg[PROCIMG_SIZE];

// Table with SDO Responses for read requests to OD
BYTE MEM_CONST SDOResponseTable[] = {
// Each Row has 8 Bytes:
// Command Specifier for SDO Response (1 byte)
//   bits 2+3 contain: '4' – {number of data bytes}
// Object Dictionary Index (2 bytes, low first)
// Object Dictionary Subindex (1 byte)
// Data (4 bytes, lowest bytes first)

// [1000h,00]: Device Type
SDOREPLY(0x1000, 0x00, 4, OD_DEVICE_TYPE),

#ifdef OD_SERIAL
// [1018h,00]: Identity Object, Number of Entries = 4
SDOREPLY(0x1018, 0x00, 1, 0x00000004L),
#else
// [1018h,00]: Identity Object, Number of Entries = 3
SDOREPLY(0x1018, 0x00, 1, 0x00000003L),
#endif

// [1018h,01]: Identity Object, Vendor ID
SDOREPLY(0x1018, 0x01, 4, OD_VENDOR_ID),

// [1018h,02]: Identity Object, Product Code
SDOREPLY(0x1018, 0x02, 4, OD_PRODUCT_CODE),

// [1018h,03]: Identity Object, Revision
SDOREPLY(0x1018, 0x03, 4, OD_REVISION),

#ifdef OD_SERIAL
// [1018h,04]: Identity Object, Serial
SDOREPLY(0x1018, 0x04, 4, OD_SERIAL),
#endif

// [2018h,00]: MicroCANopen Identity Object, Number of Entries = 3
SDOREPLY(0x2018, 0x00, 1, 0x00000003L),

// [2018h,01]: MicroCANopen Identity Object, Vendor ID = 01455341, ESA Inc.
SDOREPLY(0x2018, 0x01, 4, 0x01455341L),

// [2018h,02]: MicroCANopen Identity Object, Product Code = "MCOP"
SDOREPLY4(0x2018, 0x02, 4, 'P', 'O', 'C', 'M'),

// [2018h,03]: MicroCANopen Identity Object, Revision = 1.20
SDOREPLY(0x2018, 0x03, 4, 0x00010020L),

// End-of-table marker
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


/**************************************************************************
GLOBAL FUNCTIONS
***************************************************************************/ 

/**************************************************************************
DOES:    Call-back function for occurance of a fatal error. 
         Stops operation and displays blnking error pattern on LED
**************************************************************************/
void MCOUSER_FatalError (WORD ErrCode)
{
  while(1)
  {
  }
}


/**************************************************************************
DOES:    Call-back function for reset application.
         Starts the watchdog and waits until watchdog causes a reset.
**************************************************************************/
void MCOUSER_ResetApplication (void)
{
}


/**************************************************************************
DOES:    Call-back function for reset communication.
         Re-initializes the process image and the entire MicroCANopen
         communication.
**************************************************************************/
void MCOUSER_ResetCommunication (void)
{
BYTE i;
WORD delay;

  // Initialize Process Variables
  for (i = 0; i < PROCIMG_SIZE; i++)
  {
    gProcImg[i] = 0;
  }

  // 125kbit, Node 7, 2s heartbeat
  MCO_Init(125,0x07,2000); 
  

  // DEBUG: Delay 10ms for all external HW to catch up
  delay = gTimCnt + 10;
  while (!MCOHW_IsTimeExpired(delay))
  {
  }

  MCO_InitRPDO(1,0,4,OUT_digi_1); 
  // RPDO1, default ID(0x200+nodeID), 4 bytes

#ifdef LPCDemoMode
  MCO_InitRPDO(2,0x287,4,OUT_ana_1); 
  // RPDO2, default ID, 4 bytes
#else
  MCO_InitRPDO(2,0,4,OUT_ana_1); 
  // RPDO2, default ID, 4 bytes
#endif

  MCO_InitTPDO(1,0,300,0,4,IN_digi_1);    
  // TPDO1, default ID (0x180+nodeID), 300ms event, 0ms inhibit, 4 bytes
  
  MCO_InitTPDO(2,0,200,0,4,IN_ana_1); 
  // TPDO2, default ID (0x280+nodeID), 200ms event, 0ms inhibit, 4 bytes
}

/**************************************************************************
END-OF-FILE 
***************************************************************************/ 

