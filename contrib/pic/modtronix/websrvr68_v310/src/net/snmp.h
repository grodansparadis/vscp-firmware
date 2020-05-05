/**
 * @brief           SNMP Module for Modtronix TCP/IP Stack
 * @file            snmp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, udp.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_snmp
 *
 * @section description Description
 **********************************
 * This module contains the SNMP code.
 *
 *
 * @section snmp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- SNMP Configuration --------------------
 //*********************************************************************
 //This is the file that contains SNMP BIB file.  File name must contain all upper case
 //letter and must match with what was included in File System Image.
 #define SNMP_BIB_FILE_NAME      "SNMP.BIB"

 //This is the maximum length for community string. Application must ensure that this
 //length is observed. SNMP module does not check for length overflow.
 #define SNMP_COMMUNITY_MAX_LEN  (8ul)
 #define NOTIFY_COMMUNITY_LEN    (SNMP_COMMUNITY_MAX_LEN)

 //Change this to match your OID string length.
 #define OID_MAX_LEN             (15ul)

 #define SNMP_AGENT_PORT         (161ul)
 #define SNMP_NMS_PORT           (162ul)
 #define SNMP_AGENT_NOTIFY_PORT  (0xfffeul)
 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     1/9/03  Original        (Rev 1.0)
 ********************************************************************/

#ifndef SNMP_H
#define SNMP_H

#include "net\ip.h"

/*
 * This is the file that contains SNMP BIB file.
 * File name must contain all upper case letter and must match
 * with what was included in File System Image.
 */
#if !defined(SNMP_BIB_FILE_NAME)    //To change this default value, define it in projdefs.h
#define SNMP_BIB_FILE_NAME      "SNMP.BIB"
#endif

/*
 * This is the maximum length for community string.
 * Application must ensure that this length is observed.
 * SNMP module does not check for length overflow.
 */
#if !defined(SNMP_COMMUNITY_MAX_LEN)    //To change this default value, define it in projdefs.h
#define SNMP_COMMUNITY_MAX_LEN  (8ul)
#endif
#if !defined(NOTIFY_COMMUNITY_LEN)      //To change this default value, define it in projdefs.h
#define NOTIFY_COMMUNITY_LEN    (SNMP_COMMUNITY_MAX_LEN)
#endif

/*
 * Change this to match your OID string length.
 */ 
#if !defined(OID_MAX_LEN)               //To change this default value, define it in projdefs.h
#define OID_MAX_LEN             (15ul)
#endif

#if !defined(SNMP_AGENT_PORT)           //To change this default value, define it in projdefs.h
#define SNMP_AGENT_PORT         (161ul)
#endif
#if !defined(SNMP_NMS_PORT)             //To change this default value, define it in projdefs.h
#define SNMP_NMS_PORT           (162ul)
#endif
#if !defined(SNMP_AGENT_NOTIFY_PORT)    //To change this default value, define it in projdefs.h
#define SNMP_AGENT_NOTIFY_PORT  (0xfffeul)    
#endif

#define SNMP_START_OF_VAR       (0ul)
#define SNMP_END_OF_VAR         (0xfful)
#define SNMP_INDEX_INVALID      (0xfful)


/**
 * Initialize SNMP module internals. This function is called only once
 * during lifetime of the application.
 *
 * Side Effects:    One UDP socket will be used.
 *
 * @preCondition    At least one UDP socket must be available.
 *                  UDPInit() is already called.
 *
 * @return          SNMP agent module is initialized.
 */
void SNMPInit(void);


/**
 * Handle incoming SNMP requests as well as any
 * outgoing SNMP responses and timeout conditions
 *
 * @preCondition    SNMPInit is already called.
 *
 * @return          TRUE if SNMP module has finished with a state<br>
 *                  FALSE if a state has not been finished.
 */
BOOL SNMPTask(void);


/**
 * This is the SNMP OID variable id.
 * This id is assigned via MIB file.  Only dynamic and AgentID
 * variables can contian ID.  MIB2BIB utility enforces this
 * rules when BIB was generated.
 */
typedef BYTE SNMP_ID;
typedef BYTE SNMP_INDEX;

typedef union _SNMP_VAL
{
    DWORD dword;
    WORD  word;
    BYTE  byte;
    BYTE  v[sizeof(DWORD)];
} SNMP_VAL;

/**
 * This is a callback function called by SNMP module. SNMP user must implement
 * this function in user application and provide appropriate data when called.
 *
 * @param var       Variable id whose value is to be returned
 * @param index     Index of variable that should be transferred
 * @param ref       Variable reference used to transfer multi-byte data
 *                  It is always SNMP_START_OF_VAR when very first byte is requested.
 *                  Otherwise, use this as a reference to keep track of multi-byte transfers.
 * @param val       Pointer to up to 4 byte buffer.
 *                  - If var data type is BYTE, transfer data in val->byte
 *                  - If var data type is WORD, transfer data in val->word
 *                  - If var data type is DWORD, transfer data in val->dword
 *                  - If var data type is IP_ADDRESS, transfer data in val->v[] or val->dword
 *                  - If var data type is COUNTER32, TIME_TICKS or GAUGE32, transfer data in val->dword
 *                  - If var data type is ASCII_STRING or OCTET_STRING transfer data in val->byte
 *                    using multi-byte transfer mechanism.
 *
 * @return          TRUE if a value exists for given variable at given index<br>
 *                  FALSE otherwise.
 */
#ifdef THIS_IS_SNMP_SERVER
    extern BOOL SNMPGetVar(SNMP_ID var, SNMP_INDEX index,
                           BYTE *ref, SNMP_VAL* val);
#endif


/**
 * This is a callback function called by SNMP module. SNMP user must implement
 * this function in user application and provide appropriate data when
 * called.  This function will only be called for OID variable of type sequence.
 *
 * @param var   Variable id whose value is to be returned
 * @param idnex Next Index of variable that should be transferred
 *
 * @return      TRUE if a next index value exists for given variable at given
 *              index and index parameter contains next valid index.<br>
 *              FALSE otherwise.
 */
#ifdef THIS_IS_SNMP_SERVER
    extern BOOL SNMPGetNextIndex(SNMP_ID var, SNMP_INDEX *index);
#endif


/**
 * This is a callback function called by module.
 * User application must implement this function.
 *
 * This function will be called for only variables
 * that are defined as ASCII_STRING and OCTET_STRING
 * (i.e. data length greater than 4 bytes)
 *
 * @param var   Variable id whose value is to be set
 * @param len   Length value that is to be validated.
 *
 * @return      TRUE if given var can be set to given len<br>
 *              FALSE if otherwise.
 */
#ifdef THIS_IS_SNMP_SERVER
    extern BOOL SNMPIsValidSetLen(SNMP_ID var, BYTE len);
#endif


/**
 * This is a callback function called by module.
 * User application must implement this function.
 *
 * This function may get called more than once
 * depending on number of bytes in a specific
 * set request for given variable.
 *
 * @param var   Variable id whose value is to be set
 * @param ref   Variable reference used to transfer multi-byte data 0 if first byte is set
 *              otherwise nonzero value to indicate corresponding byte being set.
 * @param val   Up to 4 byte data value.
 *                  - If var data type is BYTE, transfer data in val->byte
 *                  - If var data type is WORD, transfer data in val->word
 *                  - If var data type is DWORD, transfer data in val->dword
 *                  - If var data type is IP_ADDRESS, COUNTER32,
 *                    or GAUGE32, value is in val->dword
 *                  - If var data type is OCTET_STRING, ASCII_STRING
 *                    value is in val->byte; multi-byte transfer
 *                    will be performed to transfer remaining
 *                    bytes of data.
 *
 * @return      TRUE if it is OK to set more byte(s)<br>
 *              FALSE if otherwise.
 */
#ifdef THIS_IS_SNMP_SERVER
    extern BOOL SNMPSetVar(SNMP_ID var, SNMP_INDEX index,
                            BYTE ref, SNMP_VAL val);
#endif



/**
 * This is the list of SNMP action a remote NMS can perform.
 * This inforamtion is passed to application via
 * callback SNMPValidate.
 * Application should validate the action for given community
 * string.
 */
typedef enum _SNMP_ACTION
{
    SNMP_GET            = 0xa0,
    SNMP_GET_NEXT       = 0xa1,
    SNMP_GET_RESPONSE   = 0xa2,
    SNMP_SET            = 0xa3,
    SNMP_TRAP           = 0xa4,
    SNMP_ACTION_UNKNOWN = 0
} SNMP_ACTION;

/**
 * This is a callback function called by module.
 * User application must implement this function
 * and verify that community matches with predefined
 * value.
 *
 * Note:            This validation occurs for each NMS request.
 *
 * @preCondition    SNMPInit is already called.
 *
 * @param SNMPAction    SNMP_GET to fetch a variable
 *                      SNMP_SET to write to a variable
 * @param community     Community string as sent by NMS
 *
 * @return              TRUE if password matches with given community<br>
 *                      FALSE if otherwise.
 *
 */
#ifdef THIS_IS_SNMP_SERVER
    extern BOOL SNMPValidate(SNMP_ACTION SNMPAction, char* community);
#endif



/**
 * This function prepares SNMP module to send SNMP
 * trap (notification) to remote host.
 *
 * This is first of series of functions to complete SNMP notification.
 *
 * @preCondition        SNMPInit is already called.
 *
 * @param remoteHost    pointer to remote Host IP address
 * @param community     Community string to use to notify
 * @param communityLen  Community string length
 * @param agentIDVar    System ID to use identify this agent
 * @param notificationCode Notification Code to use
 * @param timestamp     Notification timestamp in 100th of second.
 *
 */
void SNMPNotifyPrepare(IP_ADDR *remoteHost,
                        char *community,
                        BYTE communityLen,
                        SNMP_ID agentIDVar,
                        BYTE notificationCode,
                        DWORD timestamp);


/**
 * This function resolves given remoteHost IP address
 * into MAC address using ARP module.
 * If remoteHost is not aviailable, this function
 * would never return TRUE.
 * Application must implement timeout logic to
 * handle "remoteHost not avialable" situation.
 *
 * @preCondition    SNMPNotifyPrepare is already called and returned TRUE.
 *
 * @param remoteHost pointer to remote Host IP address
 *
 * @return          TRUE if remoteHost IP address is resolved and SNMPNotify may be called.<br>
 *                  FALSE otherwise. This would fail if there were not UDP socket to open.
 */
BOOL SNMPIsNotifyReady(IP_ADDR *remoteHost);


/**
 * This function creates SNMP trap PDU and sends it
 * to previously specified remoteHost.
 *
 * @preCondition    SNMPIsNotified is already called and returned TRUE.
 *
 * @param var       SNMP var ID that is to be used in notification
 * @param val       Value of var. Only value of BYTE, WORD or DWORD can be sent.
 * @param index     Index of var. If this var is a single, index would be 0, or
 *                  else if this var is a sequence, index could be any
 *                  value from 0 to 127.
 *
 * @return          TRUE if SNMP notification was successful sent.
 *                  This does not guarantee that remoteHost recieved it.
 *                  FALSE otherwise.<br>
 *                  This would fail under following contions:
 *                  - Given SNMP_BIB_FILE does not exist in File System
 *                  - Given var does not exist.
 *                  - Previously given agentID does not exist
 *                  - Data type of given var is unknown - only possible if File System itself was corrupted.
 */
BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index);

#endif
