/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : cdc_cmd.h
 *    Description : Communication Device Class commands definitions
 *
 *    History :
 *    1. Date        : June 30, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
 **************************************************************************/
#include "arm_comm.h"
#include "usb_9.h"

#ifndef __CDC_CMD_H
#define __CDC_CMD_H

// CD Class commands
typedef enum
{
// Common
  SEND_ENCAPSULATED_COMMAND = 0,
  GET_ENCAPSULATED_RESPONSE,
  SET_COMM_FEATURE,
  GET_COMM_FEATURE,
  CLEAR_COMM_FEATURE,
// Direct Line Control Model
  SET_AUX_LINE_STATE = 0x10,
  SET_HOOK_STATE,
  PULSE_SETUP,
  SEND_PULSE,
  SET_PULSE_TIME,
  RING_AUX_JACK,
// Abstract Control Model
  SET_LINE_CODING = 0x20,
  GET_LINE_CODING,
  SET_CONTROL_LINE_STATE,
  SEND_BREAK,
// Telephone Control Model
  SET_RINGER_PARMS = 0x30,
  GET_RINGER_PARMS,
  SET_OPERATION_PARMS,
  GET_OPERATION_PARMS,
  SET_LINE_PARMS,
  GET_LINE_PARMS,
  DIAL_DIGITS,
  // Multi-Channel Model
  SET_UNIT_PARAMETER,
  GET_UNIT_PARAMETER,
  CLEAR_UNIT_PARAMETER,
  // CAPI Control Model
  GET_PROFILE,
// Ethernet Networking Control Model
  SET_ETHERNET_MULTICAST_FILTERS = 0x40,
  SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER,
  GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER,
  SET_ETHERNET_PACKET_FILTER,
  GET_ETHERNET_STATISTIC,
// ATM Networking Control Model
  SET_ATM_DATA_FORMAT = 0x50,
  GET_ATM_DEVICE_STATISTICS,
  SET_ATM_DEFAULT_VC,
  GET_ATM_VC_STATISTICS,
} CD_ClassCommands_t;

// CD Class Notification Codes
typedef enum
{
  NETWORK_CONNECTION = 0,
  RESPONSE_AVAILABLE,
  AUX_JACK_HOOK_STATE = 8,
  RING_DETECT,
  SERIAL_STATE = 0x20,
  CALL_STATE_CHANGE = 0x28,
  LINE_STATE_CHANGE,
  CONNECTION_SPEED_CHANGE
} CD_ClassNotification_t;

#endif //__CDC_CMD_H
