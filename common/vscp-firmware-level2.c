/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	https://www.vscp.org
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2023 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * This file is part of VSCP - Very Simple Control Protocol 	
 * https://www.vscp.org
 *
 * ******************************************************************************
*/


#include <string.h>
#include <inttypes.h>
#include <vscp.h>
#include <vscp_class.h>      
#include <vscp_type.h>
#include <vscp-fifo.h>
#include <vscp-firmware-helper.h>

#include "vscp-firmware-level2.h"

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

/* Constants */

/* Globals */

uint8_t alarm_status;   /* VSCP Alarm Status */
uint8_t error_count;    /* Error counter */

/*!
  Page select is not normally used on a level II device
  but can be used to enable special write operations
  See: THIS_FIRMWARE_ENABLE_WRITE_PROTECTED_LOCATIONS
  in projdefs.h
*/

/* 
  User supplied data that will be provided to
  all callbacks.
*/ 
static const void *pUserData = NULL;

static uint16_t page_select;   

static uint8_t state;     // State machine state 
static uint8_t substate;  // State machine substate 


// Timer used for config restore 
static uint32_t config_timer;

///////////////////////////////////////////////////////////////////////////////
// vscp2_init
//

int vscp2_init(const void *pdata) 
{
  // Save user provided data pointer
  pUserData = pdata;

  // Strat up state
  state = VSCP2_STATE_STARTUP;

  // Init variables
  page_select = 0;
  alarm_status= 0;
  error_count = 0;

  vscpEvent ev;
  memset(&ev, 0, sizeof(vscpEvent));

  ev.head = VSCP_PRIORITY_NORMAL;
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  // GUID is zero => our own GUID will be used

  // Send new node online
  if (VSCP_ERROR_SUCCESS != vscp2_protocol_callback_send_event(pUserData, &ev)) {
    return VSCP_ERROR_ERROR;
  }

  state = VSCP2_STATE_UNCONNECTED;

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_do_work
//

int vscp2_do_work(vscpEvent *pev) 
{
  //int rv;
  uint8_t offset = 0;
  uint16_t vscp_class;

  // If there is an event we should check if we should react
  // on it
  if (NULL != pev) {

    vscp_class = pev->vscp_class;

    if (VSCP_CLASS2_LEVEL1_PROTOCOL == vscp_class) {
      offset = 16;        // Data is at offset 16
      vscp_class -= 512;
    }

    if (VSCP_CLASS1_PROTOCOL == vscp_class) {

      switch (pev->vscp_type) {
        
        case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
          vscp2_protocol_callback_enter_bootloader(pUserData);
          break;
        
        case VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO:
          vscp2_protocol_callback_report_dmatrix(pUserData);
          break;

        case VSCP_TYPE_PROTOCOL_GET_EMBEDDED_MDF:
          vscp2_protocol_callback_report_mdf(pUserData);
          break;

        case VSCP_TYPE_PROTOCOL_GET_EVENT_INTEREST:
          vscp2_protocol_callback_report_events_of_interest(pUserData);
          break;

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER
        case VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_RESPONSE:
          vscp2_protocol_callback_high_end_server_response(pUserData);
          break;  
#endif          
      }
    }
    // Level II
    else if (VSCP_CLASS2_PROTOCOL == pev->vscp_class) {
     
      switch (pev->vscp_type) {
        
        case VSCP2_TYPE_PROTOCOL_READ_REGISTER:
          vscp2_do_register_read(pev);
          break;          

        case VSCP2_TYPE_PROTOCOL_WRITE_REGISTER:
          vscp2_do_register_write(pev);
          break; 

      }
    } 

    //vscp_fwhlp_deleteEvent(&pev); 

  } // NULL event
  
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_do_register_read
//

int 
vscp2_do_register_read(vscpEvent* pev) 
{
  int rv;
  uint8_t regs[512];  
  uint16_t cnt = 1;
  uint32_t startreg;

  if (NULL == pev) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Must be at least GUID + start reg
  if (pev->sizeData < 20) {
    //vscp_fwhlp_deleteEvent(&pev);
    return VSCP_ERROR_PARAMETER;
  }  

  // Get start position
  startreg = construct_unsigned32(pev->pdata[16],
                                  pev->pdata[17],
                                  pev->pdata[18],
                                  pev->pdata[19]);

  // If size is given get it
  if (pev->sizeData >= 22) {
    cnt = construct_unsigned16(pev->pdata[20], pev->pdata[21]);
    if (cnt > (512-4)) {
     cnt = (512-4);
    }
  }

  // Read the registers
  for (uint32_t pos=startreg; pos < (startreg + cnt); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp2_read_reg(pos, &regs[pos-startreg]))) {
      //vscp_fwhlp_deleteEvent(&pev);
      return rv;
    }
  }

  // Original event is not needed anymore
  //vscp_fwhlp_deleteEvent(&pev);

  // Create new event
  // if (NULL == (pev = vscp_fwhlp_newEvent())) {
  //   return VSCP_ERROR_MEMORY;
  // }
  vscpEvent respev; // Response event

  // Construct reply event
  respev.head = VSCP_PRIORITY_NORMAL;
  memcpy(respev.GUID, vscp2_protocol_callback_get_guid(pUserData), 16);
  respev.vscp_class = VSCP_CLASS2_LEVEL1_PROTOCOL;
  respev.vscp_type = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  respev.timestamp = vscp2_protocol_callback_get_timestamp(pUserData);
  vscp2_protocol_callback_get_time(pUserData, pev);
  // Data
  respev.sizeData = cnt + 4;
  respev.pdata[0] = (uint8_t)((startreg >> 24) & 0xff);
  respev.pdata[1] = (uint8_t)((startreg >> 16) & 0xff);
  respev.pdata[2] = (uint8_t)((startreg >> 8) & 0xff);
  respev.pdata[3] = (uint8_t)(startreg & 0xff);
  
  // copy in values
  memcpy(respev.pdata + 4, regs, cnt);

  // Send event
  return vscp2_protocol_callback_send_event(pUserData, &respev);   
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_do_register_write
//

int 
vscp2_do_register_write(vscpEvent* pev)
{
  int rv;
  uint8_t regs[512];  
  uint16_t cnt = 1;
  uint32_t startpos;
  
  if (NULL == pev) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Must be at least GUID + start reg
  // if (pev->sizeData < 20) {
  //   vscp_fwhlp_deleteEvent(&pev);
  //   return VSCP_ERROR_PARAMETER;
  // }  

  // Get start position
  startpos = construct_unsigned32(pev->pdata[16],
                                  pev->pdata[17],
                                  pev->pdata[18],
                                  pev->pdata[19]);

  // Size bytes in frame minus GUID and startpos
  cnt = pev->sizeData - 16 - 4;

  for (uint32_t pos = startpos; pos < (startpos + cnt); pos++) {
    if (VSCP_ERROR_SUCCESS != (rv = vscp2_protocol_callback_write_user_reg(pUserData, pos, regs[pos]))) {
      //vscp_fwhlp_deleteEvent(&pev);
      return rv;
    }
  }

  // Original event is not needed anymore
  //vscp_fwhlp_deleteEvent(&pev);

  // Create new event
  // if (NULL == (pev = vscp_fwhlp_newEvent())) {
  //   return VSCP_ERROR_MEMORY;
  // }

  // Construct reply event
  vscpEvent respev;
  respev.head = VSCP_PRIORITY_NORMAL;
  memcpy(respev.GUID, vscp2_protocol_callback_get_guid(pUserData), 16);
  respev.vscp_class = VSCP_CLASS2_LEVEL1_PROTOCOL;
  respev.vscp_type = VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE;
  respev.timestamp = vscp2_protocol_callback_get_timestamp(pUserData);
  vscp2_protocol_callback_get_time(pUserData, pev);
  // Data
  respev.sizeData = cnt + 4;
  // respev.pdata = malloc(respev.sizeData);
  // if (NULL == respev.pdata) {
  //   vscp_fwhlp_deleteEvent(&pev);
  //   return VSCP_ERROR_MEMORY;
  // }

  respev.pdata[0] = (uint8_t)((startpos >> 24) & 0xff);
  respev.pdata[1] = (uint8_t)((startpos >> 16) & 0xff);
  respev.pdata[2] = (uint8_t)((startpos >> 8) & 0xff);
  respev.pdata[3] = (uint8_t)(startpos & 0xff);
  
  // copy in values
  memcpy(respev.pdata + 4, regs, cnt);

  // Send event
  return vscp2_protocol_callback_send_event(pUserData, &respev);
}


///////////////////////////////////////////////////////////////////////////////
// vscp2_read_reg
//

int vscp2_read_reg(uint32_t reg, uint8_t* pval)
{
  int rv;
  //int i;

  if (reg < VSCP2_STANDARD_REGISTER_START ) {
    // User register 
    return vscp2_protocol_callback_read_user_reg(pUserData, reg, pval);
  } 
  else {
    // standard register 
    if (VSCP2_STD_REG_ALARMSTATUS == reg) {
      // Writing/reading alarm resets it regardless of value
      *pval = alarm_status;          
      alarm_status = 0; 
    }
    else if (VSCP2_STD_REG_VSCP_MAJOR_VERSION == reg) {
      *pval = VSCP2_MAJOR_VERSION;
    }
    else if (VSCP2_STD_REG_VSCP_MINOR_VERSION == reg) {
      *pval = VSCP2_MINOR_VERSION;     
    }
    else if (VSCP2_STD_REG_NODE_ERROR_COUNTER == reg) {
      // Writing/reading error counter resets it regardless of value
      *pval = error_count;  
      error_count = 0;    
    }
    else if ((VSCP2_STD_REG_USERID0 >= reg) && (VSCP2_STD_REG_USERID4 <= reg)) {
      if (VSCP_ERROR_SUCCESS != (rv = vscp2_protocol_callback_get_user_id(pUserData, reg-VSCP2_STD_REG_USERID0, pval))) {
        return rv;
      }    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_ID0 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_ID0;  
    }
    else if (VSCP2_STD_REG_MANUFACTURER_ID1 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_ID1;    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_ID2 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_ID2;    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_ID3 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_ID3;    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_SUBID0 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_SUBID0;   
    }
    else if (VSCP2_STD_REG_MANUFACTURER_SUBID1 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_SUBID1;    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_SUBID2 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_SUBID2;    
    }
    else if (VSCP2_STD_REG_MANUFACTURER_SUBID3 == reg) {
      *pval = THIS_FIRMWARE_MANUFACTURER_SUBID3;    
    }
    else if (VSCP2_STD_REG_NICKNAME_ID == reg) {
      *pval = 0;    // Not used for level II
    }
    else if (VSCP2_STD_REG_PAGE_SELECT_MSB == reg) {
      *pval = (page_select >> 8) & 0xff;    
    }
    else if (VSCP2_STD_REG_PAGE_SELECT_MSB == reg) {
      *pval = page_select & 0xff;
    }
    else if (VSCP2_STD_REG_FIRMWARE_MAJOR_VERSION == reg) {
      *pval = THIS_FIRMWARE_MAJOR_VERSION;    
    }
    else if (VSCP2_STD_REG_FIRMWARE_MINOR_VERSION == reg) {
      *pval = THIS_FIRMWARE_MINOR_VERSION;    
    }
    else if (VSCP2_STD_REG_FIRMWARE_RELEASE_VERSION == reg) {
      *pval = THIS_FIRMWARE_RELEASE_VERSION;    
    }
    else if (VSCP2_STD_REG_BOOT_LOADER_ALGORITHM == reg) {
      *pval = THIS_FIRMWARE_BOOTLOADER_ALGORITHM;;    
    }
    else if (VSCP2_STD_REG_BUFFER_SIZE == reg) {
      *pval = 0;    // default = 0 => 512
    }
    else if (VSCP2_STD_REG_PAGES_USED == reg) {
      *pval = 0;    // Deprecated register value
    }
    else if (VSCP2_STD_REG_PAGES_USED == reg) {
      *pval = 0;    // Deprecated register value
    }
    else if (VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_FAMILY_CODE) >> 24 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE + 1) == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_FAMILY_CODE) >> 16 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE + 2) == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_FAMILY_CODE) >> 8 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_FAMILY_CODE + 3) == reg) {
      *pval = THIS_FIRMWARE_DEVICE_FAMILY_CODE & 0xff;
    }
    else if (VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_TYPE_CODE) >> 24 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE + 1) == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_TYPE_CODE) >> 16 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE + 2) == reg) {
      *pval = (THIS_FIRMWARE_DEVICE_TYPE_CODE) >> 8 & 0xff;
    }
    else if ((VSCP2_STD_REG_STANDARD_DEVICE_TYPE_CODE + 3) == reg) {
      *pval = THIS_FIRMWARE_DEVICE_TYPE_CODE & 0xff;
    }
    else if (VSCP2_STD_REG_DEFAULT_CONFIG_RESTORE == reg) {
      *pval = 0;  // always read as zero
    }
    else if (VSCP2_STD_REG_FIRMWARE_CODE == reg) {
      *pval = (THIS_FIRMWARE_CODE >> 8) & 0xff;
    }
    else if ((VSCP2_STD_REG_FIRMWARE_CODE + 1) == reg) {
      *pval = THIS_FIRMWARE_CODE & 0xff;
    }
    else if ((reg >= VSCP2_STD_REG_GUID) && (reg < (VSCP2_STD_REG_GUID + 16))) {
      *pval = vscp2_protocol_callback_get_guid(pUserData)[reg - VSCP2_STD_REG_GUID];
    }
    else if ((reg >= VSCP2_STD_REG_DEVICE_URL) && (reg < (VSCP2_STD_REG_DEVICE_URL + 16))) {
      char mdf_url[] = "THIS_FIRMWARE_MDF_URL";
    }
    else {
      // This standard register is not available
    }
  } // standard register

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_write_reg
//

int 
vscp2_write_reg(uint32_t reg, uint8_t val)
{
  if (reg < VSCP2_STANDARD_REGISTER_START ) {
    // User register 
    return vscp2_protocol_callback_write_user_reg(pUserData, reg, val);
  } 
  else if ( ( reg >= VSCP2_STD_REG_USERID0 ) && ( reg <= VSCP2_STD_REG_USERID4 ) )  {
    return vscp2_protocol_callback_write_user_id(pUserData, reg-VSCP2_STD_REG_USERID0, val);
  }
#ifdef THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS
  /* Write manufacturer id configuration information */
  else if ((reg >= VSCP2_STD_REG_MANUFACTURER_ID0) && (reg <= VSCP2_STD_REG_MANUFACTURER_SUBID3)) {
    /* page register must be 0xffff for writes to be possible */
    if ((0xff != ((page_select >> 8) & 0xff)) || (0xff != (page_select & 0xff))) {
      return VSCP_ERROR_ERROR;
    } 
    else {
      /* Write */
      return vscp2_protocol_callback_write_manufacturer_id(pUserData, reg-VSCP2_STD_REG_MANUFACTURER_ID0, val);     
    }
  }        
  else if ((reg >= (VSCP2_STD_REG_GUID)) && (reg <= (VSCP2_STD_REG_GUID + 15))) {
    /* page register must be 0xffff for writes to be possible */
    if ((0xff != ((page_select >> 8) & 0xff)) || (0xff != (page_select & 0xff))) {
      return VSCP_ERROR_ERROR;
    } 
    else {
      return vscp2_protocol_callback_write_guid(pUserData, reg-VSCP2_STD_REG_GUID, val);
    }
  }
#endif
  else if (reg == VSCP2_STD_REG_DEFAULT_CONFIG_RESTORE)  {      
    uint32_t timer;
    if (VSCP_ERROR_SUCCESS != vscp2_protocol_callback_get_ms(pUserData, &timer)) {
      return VSCP_ERROR_ERROR;
    }

    if (0x55 == val) {        
      config_timer = timer;           
    }
    else if (0xaa == val) {
      if ((timer - config_timer) < 1000) {
        config_timer = 0;
        return vscp2_protocol_callback_restore_defaults(pUserData);
      }
      else {
        return VSCP_ERROR_ERROR; 
      }
    }  
  }

  // Register does not exist or is not writeable
  return VSCP_ERROR_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_send_heartbeat
//

int
vscp2_send_heartbeat(void)
{
  vscpEvent ev;
  // Create new event
  // if (NULL == (pev = vscp_fwhlp_newEvent())) {
  //   return VSCP_ERROR_MEMORY;
  // }

  // Construct reply event
  ev.head = VSCP_PRIORITY_NORMAL;
  memcpy(ev.GUID, vscp2_protocol_callback_get_guid(pUserData), 16);
  ev.vscp_class = VSCP_CLASS2_INFORMATION;
  ev.vscp_type = VSCP2_TYPE_INFORMATION_HEART_BEAT;
  ev.timestamp = vscp2_protocol_callback_get_timestamp(pUserData);
  vscp2_protocol_callback_get_time(pUserData, &ev);

  // Data
  ev.sizeData = 0;
  
  // Send event
  return vscp2_protocol_callback_send_event(pUserData, &ev);
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_send_caps
//

int
vscp2_send_caps(void)
{
  uint8_t guid[16] = {0}; // THIS_FIRMWARE_GUID;    TODO
  vscpEvent ev;
  // Create new event
  // if (NULL == (pev = vscp_fwhlp_newEvent())) {
  //   return VSCP_ERROR_MEMORY;
  // }

  // Construct reply event
  ev.head = VSCP_PRIORITY_NORMAL;
  memcpy(ev.GUID, vscp2_protocol_callback_get_guid(pUserData), 16);
  ev.vscp_class = VSCP_CLASS2_PROTOCOL;
  ev.vscp_type = VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS;
  ev.timestamp = vscp2_protocol_callback_get_timestamp(pUserData);
  vscp2_protocol_callback_get_time(pUserData, &ev);
  
  // Data
  ev.sizeData = 104;
  memset(ev.pdata, 0, ev.sizeData);

  // Capabilities
  ev.pdata[0] = 0x40;   // Node have a standard decision matrix.
  ev.pdata[1] = 0x00;
  ev.pdata[2] = 0x00;
  ev.pdata[3] = 0x00;
  ev.pdata[4] = 0x00;
  ev.pdata[5] = 0x00;
  ev.pdata[6] = 0x80;   // Have VSCP TCP server with VCSP link interface.
  ev.pdata[7] = 0x38;   // IP4 support, SLL support
                       // Accepts two or more simultaneous connections on TCP/IP interface.

  // GUID
  memcpy(ev.pdata + 8, guid, 16);

  // Get ipv6/ipv4 address (Always 16-byte or NULL)
  uint8_t ipaddr[16];
  
  if (VSCP_ERROR_SUCCESS == vscp2_protocol_callback_get_ip_addr(pUserData, ipaddr) ) {
    // IP address
    memcpy((ev.pdata + 24), ipaddr, 16);
  }

  // Device name
  strncpy((char *)(ev.pdata + 40), THIS_FIRMWARE_DEVICE_NAME, 63);
  
  
  // Send event
  return vscp2_protocol_callback_send_event(pUserData, &ev);
}

///////////////////////////////////////////////////////////////////////////////
// vscp2_send_high_end_server_probe
//

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER

int vscp2_send_high_end_server_probe(void)
{
  vscpEvent ev;
  // Create new event
  // if (NULL == (pev = vscp_fwhlp_newEvent())) {
  //   return VSCP_ERROR_MEMORY;
  // }


  // Construct reply event
  ev.head = VSCP_PRIORITY_NORMAL;
  memcpy(&ev.GUID, vscp2_protocol_callback_get_guid(pUserData), 16);
  ev.vscp_class = VSCP_CLASS2_PROTOCOL;
  ev.vscp_type = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE;
  ev.timestamp = vscp2_protocol_callback_get_timestamp(pUserData);
  vscp2_protocol_callback_get_time(pUserData, &ev);
  
  // Data
  ev.sizeData = 0;

  // Send event
  return vscp2_protocol_callback_send_event(pUserData, &ev);
}



#endif



