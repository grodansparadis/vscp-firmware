/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	https://www.vscp.org
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2022 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
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
#include "vscp-firmware-level2.h"

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

/* Macros */

/* This macro construct a signed integer from two unsigned chars in a safe way */
#define construct_signed16(msb, lsb) ((int16_t)((((uint16_t)msb) << 8) + \
                                                (uint16_t)lsb))

/* This macro construct a unsigned integer from two unsigned chars in a safe way */
#define construct_unsigned16(msb, lsb) ((uint16_t)((((uint16_t)msb) << 8) + \
                                                   (uint16_t)lsb))

/* This macro construct a signed long from four unsigned chars in a safe way */
#define construct_signed32(b0, b1, b2, b3) ((int32_t)((((uint32_t)b0) << 24) + \
                                                      (((uint32_t)b0) << 16) + \
                                                      (((uint32_t)b0) << 8) +  \
                                                      (uint32_t)b0))

/* This macro construct a unsigned long from four unsigned chars in a safe way */
#define construct_unsigned32(b0, b1, b2, b3) ((uint32_t)((((uint32_t)b0) << 24) + \
                                                         (((uint32_t)b0) << 16) + \
                                                         (((uint32_t)b0) << 8) +  \
                                                         (uint32_t)b0))

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
uint16_t page_select;   

uint8_t node_state;     /* State machine state */
uint8_t node_substate;  /* State machine substate */

/* GUID for this node */
extern uint8_t device_guid[16];

/* Device version */
extern uint8_t device_version[4];

/* Input fifo containing events to handle*/
extern vscp_fifo_t fifoEventsIn;


/**
 * \fn vscp2_init
 */

int vscp2_init(const void* pdata) 
{
  // Send new node on-License
  return VSCP_ERROR_SUCCESS;
}


/**
 * \fn vscp2_sendEvent
 */

int vscp2_send_event(const void* pdata, vscpEvent *ev) 
{
  return VSCP_ERROR_SUCCESS;
}


/** 
 * \fn vscp2_sendEvent
 */

int vscp2_send_eventex(const void* pdata, vscpEventEx *ex) 
{
  return VSCP_ERROR_SUCCESS;
}


/**
 * \fn vscp2_getEvent
 */

int vscp2_get_event(const void* pdata, vscpEvent** pev) 
{
  return VSCP_ERROR_SUCCESS;
}


/**
 * \fn vscp2_readRegister
 */

int vscp2_read_reg(const void* pdata, uint32_t reg, uint8_t* pval)
{
  int rv;
  int i;

  if (reg < VSCP2_STANDARD_REGISTER_START ) {
    /* User register */
    vscp2_callback_read_reg(pdata, reg, pval);
  } 
  else {
    /* standard register */
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
      *pval = error_count;;          
      error_count = 0;    
    }
    else if ((VSCP2_STD_REG_USERID0 >= reg) && (VSCP2_STD_REG_USERID4 <= reg)) {
      if (VSCP_ERROR_SUCCESS != (rv = vscp2_callback_get_user_id(pdata, reg-VSCP2_STD_REG_USERID0, pval))) {
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
      *pval = device_guid[reg - VSCP2_STD_REG_GUID];
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

/**
 * \fn vscp2_writeRegister
 */

int vscp2_write_reg(const void* pdata, uint32_t reg, uint8_t val)
{
  if (reg < VSCP2_STANDARD_REGISTER_START ) {
    /* User register */
    vscp2_callback_read_reg(pdata, reg, &val);
  } 
  else {
    if (reg == VSCP2_STD_REG_ALARMSTATUS) {
      // Writing/reading alarm resets it regardless of value
      alarm_status = 0;
    }
    else if (reg == VSCP2_STD_REG_NODE_ERROR_COUNTER) {
      // Writing/reading error counter resets it regardless of value
      alarm_status = 0;
    }
    else if ( ( reg >= VSCP2_STD_REG_USERID0 ) && ( reg <= VSCP2_STD_REG_USERID4 ) )  {
      return vscp2_callback_write_user_id(pdata, reg-VSCP2_STD_REG_USERID0, val);
    }
#ifdef THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS
    /* Write manufacturer id configuration information */
    else if ((reg >= VSCP2_STD_REG_MANUFACTURER_ID0) && (reg <= VSCP2_STD_REG_MANUFACTURER_SUBID3)) {
    if ((0xff != ((page_select >> 8) & 0xff)) || (0xff != (page_select & 0xff))) {
      return VSCP_ERROR_ERROR;
    } 
    else {
      /* Write */
      return vscp2_write_manufacturer_id(pdata, reg-VSCP2_STD_REG_MANUFACTURER_ID0, val);     
    }        
    else if ((reg >= (VSCP2_STD_REG_GUID0)) && (reg <= VSCP2_STD_REG_GUID15)) {
      /* page must be 0xffff for writes to be possible */
      if ((0xff != ((page_select >> 8) & 0xff)) || (0xff != (page_select & 0xff))) {
        /* return complement to indicate error */
        return VSCP_ERROR_ERROR;
      } 
      else {
        return vscp2_callback_write_guid(pdata, reg-VSCP2_STD_REG_GUID0, data);
      }
    }
#endif
  }

  return VSCP_ERROR_SUCCESS;
}








/**
 * \fn vscp2_sendHighEndServerProbe
 */

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER

int vscp2_send_highend_server_probe(const void *pdata)
{
  // outEvent.head = (VSCP_PRIORITY_HIGH << 5);
  // outEvent.sizeData = 5;
  // outEvent.vscp_class = VSCP_CLASS1_PROTOCOL;
  // outEvent.vscp_type = VSCP_TYPE_PROTOCOL_HIGH_END_SERVER_PROBE;
  // /* GUID is filled in by send routine */
  // outEvent.data[0] = 0; /* TCP interface */
  // outEvent.data[1] = vscp2_getIPsddress( 0 );
  // outEvent.data[2] = vscp2_getIPsddress( 1 );
  // outEvent.data[3] = vscp2_getIPsddress( 2 );
  // outEvent.data[4] = vscp2_getIPsddress( 3 );

  // return vscp2_sendEvent(pdata, &outEvent);
}

#endif



