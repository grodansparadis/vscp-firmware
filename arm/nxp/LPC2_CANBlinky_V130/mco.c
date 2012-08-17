/**************************************************************************
MODULE:    MCO
CONTAINS:  Minimal MicroCANopen implementation
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
           1.20, Pf 19-AUG-03, Code changed to use process image
                 Bug fix for incorrect inhibit time handling
           1.10, Pf 27-MAY-03, Bug fixes in OD (hi byte was corrupted)
                 OD changed to indefinite length
				 Support of define controled MEMORY types
           1.01, Pf 17-DEC-02, Made Object Dictionary more readable
           1.00, Pf 07-OCT-02, First Published Version
***************************************************************************/ 

#include "mco.h"
#include "mcohw.h"
#include <string.h>


/**************************************************************************
GLOBAL VARIABLES
***************************************************************************/ 

// This structure holds all node specific configuration
MCO_CONFIG MEM_FAR gMCOConfig;

#if NR_OF_TPDOS > 0
// This structure holds all the TPDO configuration data for up to 4 TPDOs
TPDO_CONFIG MEM_FAR gTPDOConfig[NR_OF_TPDOS];

// This is the next TPDO to be checked in MCO_ProcessStack
BYTE MEM_FAR gTPDONr = NR_OF_TPDOS;
#endif

#if NR_OF_RPDOS > 0
// This structure holds all the RPDO configuration data for up to 4 RPDOs
RPDO_CONFIG MEM_FAR gRPDOConfig[NR_OF_RPDOS];
#endif

// This structure holds the current receive message
CAN_MSG MEM_FAR gRxCAN;

// This structure holds the CAN message for SDO responses or aborts
CAN_MSG MEM_FAR gTxSDO;

// Process image from user_xxxx.c
extern BYTE MEM_NEAR gProcImg[];

// Table with SDO Responses for read requests to OD - defined in user_xxx.c
extern BYTE MEM_CONST SDOResponseTable[];

// 1ms timer
extern WORD gTimCnt;

/**************************************************************************
LOCAL FUNCTIONS
***************************************************************************/ 

// SDO Abort Messages
#define SDO_ABORT_UNSUPPORTED  0x06010000UL
#define SDO_ABORT_NOT_EXISTS   0x06020000UL
#define SDO_ABORT_READONLY     0x06010002UL
#define SDO_ABORT_TYPEMISMATCH 0x06070010UL


/**************************************************************************
DOES:    Search the SDO Response table for a specifc index and subindex.
RETURNS: 255 if not found, otherwise the number of the record found
         (staring at zero)
**************************************************************************/
BYTE MCO_Search_OD (
  WORD index,   // Index of OD entry searched
  BYTE subindex // Subindex of OD entry searched 
  )
{
BYTE i;
BYTE i_hi, hi;
BYTE i_lo, lo;
BYTE *p;
BYTE *r;

  i = 0;
  i_hi = (BYTE) (index >> 8);
  i_lo = (BYTE) index;
  r = (BYTE *) &(SDOResponseTable[0]);
  while (i < 255)
  {
    p = r;
    r += 8; // Set r to next record in table
    p++; // Skip command byte
	lo = *p;
	p++;
	hi = *p;
    if ((lo == 0xFF) && (hi == 0xFF))
    { // if index in table is 0xFFFF, this is end of table
	  return 255;
	}
    if (lo == i_lo)
    { 
      if (hi == i_hi)
      { 
        p++;
        if (*p == subindex)
        { // Entry found
          return i;
        }
      }
    }
    i++;
  } // while search loop
  return 255;
}


/**************************************************************************
DOES: Generates an SDO Abort Response
**************************************************************************/
void MCO_Send_SDO_Abort
  (
  DWORD ErrorCode   // 4 byte SDO abort error code
  )
{
BYTE i;

  gTxSDO.BUF[0] = 0x80;
  for (i=0;i<4;i++)
  {
    gTxSDO.BUF[4+i] = ErrorCode;
    ErrorCode >>= 8;
  }
  
  if (!MCOHW_PushMessage(&gTxSDO))
  {
    MCOUSER_FatalError(0x8801);
  }
}


/**************************************************************************
DOES: Handle an incoimg SDO request.
**************************************************************************/
BYTE MCO_Handle_SDO_Request 
  (               // Returns 1 if SDO Access success
                  // Returns 0 if SDO Abort generated
  BYTE *pData     // Pointer to 8 data bytes with SDO data
  )
{
BYTE cmd;         // Command byte of SDO request
WORD index;       // Index of SDO request
BYTE subindex;    // Subindex of SDO request
BYTE found;       // Search result of Search_OD
unsigned int *src, *dst; // Source and destination pointer for copying
#ifdef PROCIMG_IN_OD
BYTE len;
OD_PROCESS_DATA_ENTRY MEM_CONST *pOD; // Pointer to an entry in gODProcTable
#endif 

  // Init variables
  cmd = *pData & 0xE0; // Upper 3-bits are the command
  index = pData[2]; // Get hi byte of index
  index = (index << 8) + pData[1]; // Add lo byte of index
  subindex = pData[3];

  // Copy Multiplexor into response
  gTxSDO.BUF[1] = pData[1]; // index lo
  gTxSDO.BUF[2] = pData[2]; // index hi
  gTxSDO.BUF[3] = pData[3]; // subindex

  if ((cmd == 0x40) || (cmd == 0x20)) 
  { // It is a read or write command

    // Search const table 
    found = MCO_Search_OD(index,subindex);
    if (found < 255)
    { // OD entry found
      if (cmd == 0x40)
      { // Read command
        // Copy from response table to TxSDO buffer
        dst = (unsigned int *) &(gTxSDO.BUF[0]);
        src = (unsigned int *) &(SDOResponseTable[(found*8)]);
        *dst = *src;
        dst++;
        src++;
        *dst = *src;

        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      // Write command
      MCO_Send_SDO_Abort(SDO_ABORT_READONLY);
      return 0;
    }
    if ((index == 0x1001) && (subindex == 0x00))
    {
      if (cmd == 0x40)
      { // Read command
        gTxSDO.BUF[0] = 0x4F; // Expedited, 1-byte of data
        gTxSDO.BUF[4] = gMCOConfig.error_register;
        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      // Write command
      MCO_Send_SDO_Abort(SDO_ABORT_READONLY);
      return 0;
    }
    MCO_Send_SDO_Abort(SDO_ABORT_NOT_EXISTS);
    return 0;
  }
  if (cmd != 0x80)
  { // Ignore "abort received" - all others produce error
    MCO_Send_SDO_Abort(SDO_ABORT_UNSUPPORTED);
    return 0;
  }
  return 1;
}


#if NR_OF_TPDOS > 0
/**************************************************************************
DOES: Called when going into the operational mode.
      Prepares all TPDOs for operational.
**************************************************************************/
void MCO_Prepare_TPDOs 
  (
    void
  )
{
BYTE i;
BYTE x;

  i = 0;
  while (i < NR_OF_TPDOS)
  { // Prepare all TPDOs for transmission
    if (gTPDOConfig[i].CAN.ID != 0)
    { // This TPDO is used
      // Copy current process data
      for (x=0;x<gTPDOConfig[i].CAN.LEN;x++) 
      {
        gTPDOConfig[i].CAN.BUF[x] = gProcImg[gTPDOConfig[i].offset+x];
      }
      // Reset event timer for immediate transmission
      gTPDOConfig[i].event_timestamp = MCOHW_GetTime() - 2;
    }
  i++;
  }
  gTPDONr = NR_OF_TPDOS; // Ensure that MCO_ProcessStack starts with TPDO1
}


/**************************************************************************
DOES: Called when a TPDO needs to be transmitted
**************************************************************************/
void MCO_TransmitPDO 
  (
  BYTE PDONr
  )
{
  gTPDOConfig[PDONr].event_timestamp = MCOHW_GetTime() + gTPDOConfig[PDONr].event_time; 
  if (!MCOHW_PushMessage(&gTPDOConfig[PDONr].CAN))
  {
    MCOUSER_FatalError(0x8801);
  }
}
#endif // NR_OF_TPDOS > 0


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/ 

void MCO_Init 
  (
  WORD Baudrate,
  BYTE Node_ID,
  WORD Heartbeat
  )
{
BYTE i;

  // Init the global variables
  gMCOConfig.Node_ID = Node_ID;
  gMCOConfig.error_code = 0;
  gMCOConfig.Baudrate = Baudrate;
  gMCOConfig.heartbeat_time = Heartbeat;
  gMCOConfig.heartbeat_msg.ID = 0x700+Node_ID;
  gMCOConfig.heartbeat_msg.LEN = 1;
  gMCOConfig.heartbeat_msg.BUF[0] = 0; // Current NMT state of this node = bootup
  gMCOConfig.error_register = 0;
 
  // Init SDO Response/Abort message
  gTxSDO.ID = 0x580+gMCOConfig.Node_ID;
  gTxSDO.LEN = 8;
   
#if NR_OF_TPDOS > 0
  i = 0;
  while (i < NR_OF_TPDOS)
  { // Init TPDOs
     gTPDOConfig[i].CAN.ID = 0;
     i++;
  }
#endif
#if NR_OF_RPDOS > 0
  i = 0;
  while (i < NR_OF_RPDOS)
  { // Init RPDOs
     gRPDOConfig[i].CANID = 0;
     i++;
  }
#endif

  // Init the CAN interface
  if (!MCOHW_Init(Baudrate))
  {
    MCOUSER_FatalError(0x8802);
  }
  if (!MCOHW_SetCANFilter(0)) // For NMT master message 
  {
    MCOUSER_FatalError(0x8803);
  }
  if (!MCOHW_SetCANFilter(0x600+Node_ID)) // For SDO Requests
  {
    MCOUSER_FatalError(0x8803);
  }

  // Signal to MCO_ProcessStack: we just initialized
  gTPDONr = 0xFF;
}  

#if NR_OF_RPDOS > 0
/**************************************************************************
DOES: This function initializes a receive PDO. Once initialized, the 
      MicroCANopen stack automatically updates the data at offset.
NOTE: For data consistency, the application should not read the data
      while function MCO_ProcessStack executes.
**************************************************************************/
void MCO_InitRPDO
  (
  BYTE PDO_NR,       // RPDO number (1-4)
  WORD CAN_ID,       // CAN identifier to be used (set to 0 to use default)
  BYTE len,          // Number of data bytes in RPDO
  BYTE offset        // Offset to data location in process image
  )
{

#ifdef CHECK_PARAMETERS
  if ( ((PDO_NR < 1) || (PDO_NR > NR_OF_RPDOS)) ||  // Check PDO range 
       ((gMCOConfig.Node_ID < 1) || (gMCOConfig.Node_ID > 127)) )   
                                                    // Check Node ID range 1-127
  {
    MCOUSER_FatalError(0x8804);
  }
  if (offset >= PROCIMG_SIZE)   
  { // Size of process image exceeded 
    MCOUSER_FatalError(0x8904);
  }
#endif
  PDO_NR--;
  gRPDOConfig[PDO_NR].len = len;
  gRPDOConfig[PDO_NR].offset = offset;
  if (CAN_ID == 0)
  {
    gRPDOConfig[PDO_NR].CANID = 0x200 + (0x100 * ((WORD)(PDO_NR))) + gMCOConfig.Node_ID;
  }
  else
  {
    gRPDOConfig[PDO_NR].CANID = CAN_ID;
  }
  if (!MCOHW_SetCANFilter(gRPDOConfig[PDO_NR].CANID))
  {
    MCOUSER_FatalError(0x8805);
  }
}
#endif // NR_OF_RPDOS > 0


#if NR_OF_TPDOS > 0
/**************************************************************************
DOES: This function initializes a transmit PDO. Once initialized, the 
      MicroCANopen stack automatically handles transmitting the PDO.
      The application can directly change the data at any time.
NOTE: For data consistency, the application should not write to the data
      while function MCO_ProcessStack executes.
**************************************************************************/
void MCO_InitTPDO
  (
  BYTE PDO_NR,        // TPDO number (1-4)
  WORD CAN_ID,        // CAN identifier to be used (set to 0 to use default)
  WORD event_time,    // Transmitted every event_tim ms 
  WORD inhibit_time,  // Inhibit time in ms for change-of-state transmit
                      // (set to 0 if ONLY event_tim should be used)
  BYTE len,           // Number of data bytes in TPDO
  BYTE offset         // Offset to data location in process image
  )
{

#ifdef CHECK_PARAMETERS
  if ( ((PDO_NR < 1) || (PDO_NR > NR_OF_TPDOS)) ||   // Check PDO range
       ((gMCOConfig.Node_ID < 1) || (gMCOConfig.Node_ID > 127)) || // Check Node ID
       ((len < 1) || (len > 8)) ||                   // Check len range 1-8
       ((event_time == 0) && (inhibit_time == 0)) )  // One of these needs to be set
  {
    MCOUSER_FatalError(0x8806);
  }
  if (offset >= PROCIMG_SIZE)   
  { // Size of process image exceeded 
    MCOUSER_FatalError(0x8906);
  }
#endif
  PDO_NR--;
  if (CAN_ID == 0)
  {
    gTPDOConfig[PDO_NR].CAN.ID = 0x180 + (0x100 * ((WORD)(PDO_NR))) + gMCOConfig.Node_ID;
  }
  else
  {
    gTPDOConfig[PDO_NR].CAN.ID = CAN_ID;
  }
  gTPDOConfig[PDO_NR].CAN.LEN = len;
  gTPDOConfig[PDO_NR].offset = offset;
  gTPDOConfig[PDO_NR].event_time = event_time;
}
#endif // NR_OF_TPDOS > 0


/**************************************************************************
DOES: This function implements the main MicroCANopen protocol stack. 
      It must be called frequently to ensure proper operation of the
      communication stack. 
      Typically it is called from the while(1) loop in main.
**************************************************************************/
BYTE MCO_ProcessStack
  ( // Returns 0 if nothing was done
    // Returns 1 if a CAN message was received or sent
  void
  )
{
BYTE i;
BYTE x;
BYTE ret_val = 0;

  // Check if this is right after boot-up
  if (gTPDONr == 0xFF) // Was set by MCO_Init
  {
    // Init heartbeat time
    gMCOConfig.heartbeat_timestamp = MCOHW_GetTime() + gMCOConfig.heartbeat_time;
    // Send Boot-up message  
    if (!MCOHW_PushMessage(&gMCOConfig.heartbeat_msg))
    {
      MCOUSER_FatalError(0x8801);
    }
    gMCOConfig.heartbeat_msg.BUF[0] = 0x05; // going into operational state
#if NR_OF_TPDOS > 0
    MCO_Prepare_TPDOs();
#endif
    gTPDONr = NR_OF_TPDOS; // Retun value to default
    return 1;
  }
 
  // Work on next incoming messages
  if (MCOHW_PullMessage(&gRxCAN))
  { // Message received

gRxCAN.ID++;
MCOHW_PushMessage(&gRxCAN);
gRxCAN.ID--;
  
    // Check if it is NMT master message
    if (gRxCAN.ID == 0)
    { // NMT Master Message
      if ((gRxCAN.BUF[1] == gMCOConfig.Node_ID) || (gRxCAN.BUF[1] == 0))
      { // NMT message is for this node or all nodes
        switch (gRxCAN.BUF[0])
        {
        case 1: // Start node
          gMCOConfig.heartbeat_msg.BUF[0] = 5;
#if NR_OF_TPDOS > 0          
          MCO_Prepare_TPDOs();
#endif
          break;
        case 2: // Stop node
          gMCOConfig.heartbeat_msg.BUF[0] = 4;
          break;
        case 128:
          gMCOConfig.heartbeat_msg.BUF[0] = 127;
          break;
        case 129:
          // Node Reset
          MCOUSER_ResetApplication();
          break;
        case 130:
          // Reset Communication
          MCOUSER_ResetCommunication();
          break;
        default:
          break;
        } // switch
        return 1;
      } // NMT message addressed to this node
    } // NMT master message received
    
    // Check if it is SDO request message
    if (gMCOConfig.heartbeat_msg.BUF[0] != 4)
    { // Node is NOT stopped
      if (gRxCAN.ID == gMCOConfig.Node_ID+0x600)
      { // SDO Request
        i = MCO_Handle_SDO_Request(&gRxCAN.BUF[0]); // Return value not used in this version
        return 1;
      }
    }

#if NR_OF_RPDOS > 0
    // Check if it is RPDO message
    if (gMCOConfig.heartbeat_msg.BUF[0] == 5)
    { // Node is in operational
      i = 0;
      while (i < NR_OF_RPDOS)
      {
        if (gRxCAN.ID == gRPDOConfig[i].CANID)
        { // RPDO match
          for (x=0;x<gRPDOConfig[i].len;x++) 
          {
            gProcImg[gRPDOConfig[i].offset+x] = gRxCAN.BUF[x];
          }
          i = NR_OF_RPDOS;
          ret_val = 1;
        }
        i++;
      } // for all RPDOs
    } // Node is operational
#endif // NR_OF_RPDOS > 0
  } // Message received

#if NR_OF_TPDOS > 0
  // Check Next TPDO for transmit
  if (gMCOConfig.heartbeat_msg.BUF[0] == 0x05)
  { // Node is in operational
    gTPDONr++;
    if (gTPDONr >= NR_OF_TPDOS)
    {
      gTPDONr = 0;
    }
    if (gTPDOConfig[gTPDONr].CAN.ID != 0)
    { // TPDO 'gTPDONr' is used
      if ((gTPDOConfig[gTPDONr].event_time != 0) && 
          (MCOHW_IsTimeExpired(gTPDOConfig[gTPDONr].event_timestamp)) )
      { // TPDO 'i' uses event timer and event timer is expired
        // Get application data
        for (x=0;x<gTPDOConfig[gTPDONr].CAN.LEN;x++) 
        {
          gTPDOConfig[gTPDONr].CAN.BUF[x] = gProcImg[gTPDOConfig[gTPDONr].offset+x];
        }
        MCO_TransmitPDO(gTPDONr);
        return 1;
      }
    } // PDO active (CAN_ID != 0)  
  } // if node is operational
#endif // NR_OF_TPDOS > 0
  
  // Produce Heartbeat
  if (gMCOConfig.heartbeat_time != 0)
  {
    if (MCOHW_IsTimeExpired(gMCOConfig.heartbeat_timestamp))
    {
      if (!MCOHW_PushMessage(&gMCOConfig.heartbeat_msg))
      {
        MCOUSER_FatalError(0x8801);
      }
      gMCOConfig.heartbeat_timestamp = MCOHW_GetTime() + gMCOConfig.heartbeat_time;
      ret_val = 1;
    }
  }
  return ret_val;
}

