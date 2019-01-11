///////////////////////////////////////////////////////////////////////////////
// File: vscp_server.h
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2019 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
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
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	https://www.vscp.org
 *
 * ******************************************************************************
*/

//
// The daemon interface is described here 
// http://www.vscp.org/wiki/doku.php/vscp_daemon_control_interface

#ifndef VSCP_SERVER_H
#define VSCP_SERVER_H

#include <vscp_level2.h>


#define MSG_COPYRIGHT                       "OK Copyright (c) 2000-2019, Grodans Paradis AB, http://www.grodansparadis.com\r\n"
#define MSG_WELCOME                         "OK Welcome to the embedded VSCP daemon.\r\n"
#define MSG_OK                              "+OK - Success.\r\n"
#define MSG_GOODBY                          "+OK - Connection closed by client.\r\n"
#define MSG_USENAME_OK                      "+OK - User name accepted, password please\r\n"
#define MSG_PASSWORD_OK                     "+OK - Ready to work.\r\n"
#define MSG_QUEUE_CLEARED                   "+OK - All events cleared.\r\n"
#define MSG_RECEIVE_LOOP                    "+OK - Receive loop enetered. Close session to terminate.\r\n"
#define MSG_CAN_MODE                        "+OK - CAN mode set.\r\n"

#define MSG_ERROR                           "-OK - Error\r\n"
#define MSG_UNKNOWN_COMMAND                 "-OK - Unknown command\r\n"
#define MSG_PARAMETER_ERROR                 "-OK - Invalid parameter or format\r\n"
#define MSG_BUFFER_FULL                     "-OK - Buffer Full\r\n"
#define MSG_NO_MSG                          "-OK - No event(s) available\r\n"

#define MSG_PASSWORD_ERROR                  "-OK - Invalid username or password.\r\n"
#define MSG_NOT_ACCREDITED                  "-OK - Need to log in to perform this command.\r\n"
#define MSG_INVALID_USER                    "-OK - Invalid user.\r\n"
#define MSG_NEED_USERNAME                   "-OK - Need a Username before a password can be entered.\r\n"

#define MSG_MAX_NUMBER_OF_CLIENTS           "-OK - Max number of clients connected.\r\n"
#define MSG_INTERNAL_ERROR                  "-OK - Server Internal error.\r\n"
#define MSG_INTERNAL_MEMORY_ERROR           "-OK - Internal Memory error.\r\n"
#define MSG_INVALID_REMOTE_ERROR            "-OK - Invalid or unknown peer.\r\n"

#define MSG_LOW_PRIVILEGE_ERROR             "-OK - User need higher privilege level to perform this operation.\r\n"
#define MSG_INTERFACE_NOT_FOUND             "-OK - Interface not found.\r\n"

#define MSG_NOT_IMPLEMENTED                 "-OK - Command not implemented\r\n"


// Error codes
#define VSCP_SRV_ERROR_SUCCESS              0   // Command was executed successfully
#define VSCP_SRV_ERROR_GENERAL              -1  // General error
#define VSCP_SRV_ERROR_ARGUMENT             -2  // Argument error


// Statistics structure
typedef struct _vscp_statistics {
    uint32_t cntBusOff;
    uint32_t cntBusWarnings;
    uint32_t cntOverruns;
    uint32_t cntReceiveData;
    uint32_t cntReceiveFrames;
    uint32_t cntTransmitData;
    uint32_t cntTransmitFrames;
} vscp_statistics;	

// Info structure
typedef struct _vscp_interface_info {
    uint32_t channel_status;
    uint32_t lasterrorcode;
    uint32_t lasterrorsubcode;
    char lasterrorstr[ 80 ];
} vscp_interface_info;

/*!
    a2ul - convert ASCII substring to unsigned long number
    Copyright (C) 1998, 1999  Henry Spencer.
    http://www.koders.com/c/fid83F5660A86069B2E2D29B9D3FC9013F76A9BCEB7.aspx
    
    @param src NULL terminated string to convert.
    @param srclen �ength of string, 0 means strlen( src )
    @param base The base for the comversion, 0 means figure it out.
    @param resultp Pointer that holds unsigned long result after conversion.
    @return  NULL for success, else string literal 
    
*/
const char *a2ul( const char *src, uint8_t srclen, uint8_t base, uint32_t *resultp );

/*!
    Convert decimal byte to hex string
    @param d Decimal byte to convert
    @param pBuf Pointer to string buffer that will hold the result.
    @param len Number of digits to convert.
*/
void dec2hex( uint8_t d, char * pBuf, uint16_t len );


/*!
    Convert hexadecimal integer to a decimal value
    @param pHex Pointer to hexadecimal string.
    @return Converted value in decimal form.
*/
uint32_t hex2dec( const char *pHex );

/*!
    Read a value (hex or decimal)
    @param pString Pointe to value. A hex value should be
    be preceded by "0x" or "0X"
    @return Converted value in decimal form.  
*/
uint32_t readStringValue ( char *pString );

/*!
    Get GUID from string
    @param pArg Pointer to null terminated string that holds the GUID
    on the standard form "MSB:MSB-1:MSB-2:.......
*/
void vscp_server_ParseGUID( char *pArg, uint8_t *pGUID );

/*!
    Convert GUID to string
    @param pGUID Pointer to GUID array.
    @parm pbuf Pointer to buffe that will hold GUID on string form on exit.
*/
void vscp_server_GUID2String( uint8_t *pGUID, char *pbuf );


/*!
    Get VSCP data from string
    A maximum oc 512-25 byte will be read from the string. The values should
    be stored as a comma separated list and the values can be in decimal or
    hexadecimal from.
    @param pArg Pointer to null terminated string that holds the data.
    @param pData Pointer to a byte array that will hold the colected data.
    @param pcntData Pointer to an unsigned int that will hold the collected datacount.
    @return  the collected datacount.
*/
uint16_t vscp_server_ParseData( char *pArg, uint8_t *pData, uint16_t *pcntData );

/*!
    Fill a string with data in textform
    @param pData Pointer to data array.
    @param size Size of data array.
    @param pbuf String that will be filled with data in coma separated text form. The
    buffer must have room for the data.
*/
void vscp_server_Data2String( uint8_t *pData, uint16_t size, char *pbuf );

/*!
    parse the send argument
    @param pEvent Pointer to event that will be filled with data
    @return VSCP_SRV_ERROR_SUCCESS on success errocode on error.
 */
int vscp_server_parse_send( char *pArg, vscpEvent *pEvent );



// ************************************************************************************************


//  
// Callbacks
// The following routines should be implemented by the application
// that use this code
//


// ************************************************************************************************



/*!
    Send reply to client.
    @param pReply Null terminated string to send as reply.
*/

void vscp_server_sendReply( char *pReply );

/*!
    Handle quit command. 
    Standard procedure is to wait for a short while and
    then disconnect the connection.
*/

void vscp_server_command_quit( void );

/*!
    Handle the send event command
    The handler should send the event and return
    @param pEvent Event to send.
*/

void vscp_server_command_send( vscpEvent *pEvent );

/*!
    Handle the retreive event command.
    Fill the pointed event with data. 
    @param Pointer to event that should rceive the event
    @return Number of events availabel before this call. This means that
    zero is returned if there is no events in the queue.
*/

int vscp_server_command_retr( vscpEvent *pevent );


/*!
    Check if there is events to read
    @return Number of events available to read.
*/

uint32_t vscp_server_command_cdta( void );

/*!
    Clear the event queue
*/

void vscp_server_command_clra( void );

/*!
    Get interface statistics
    The routine fill the structure with data. If no statistics is collected just
    fill the structure with zeros.The structure is defined for CAN but busoff
    etc members can be used for other application needs as the developer whiches.
    @param pStatistics Pointer to statistics structure.
*/
void vscp_server_command_stat( vscp_statistics *pStatistics );

/*!
    Get status information for interface
    Fill the structure with status information for the interface. If it
    does not make sense for your applicatin just null it.
*/
void vscp_server_command_info( vscp_interface_info *pInfo );

/*!
    Get the channel id for the communication channel. 
    Each channel for on a server should have a unique channel id. 
    @return Channel id for the selected communication channel.
*/
uint32_t vscp_server_command_chid( void );

/*!
    Set GUID for channel
    Each channel for on a server should have a unique GUID.
    @param pGUID GUID for interface.
*/
void vscp_server_command_sgid( uint8_t *pGUID );

/*!
    Get GUID for channel
    @param pGUID GUID for interface.
*/
void vscp_server_command_ggid( uint8_t *pGUID );

/*!
    Get version of VSCP server
    @param major Major version.
    @param minor Minor version.
    @param sub Sub version.
*/
void vscp_server_command_ver( uint8_t *major, uint8_t *minor, uint8_t *sub );

/*!
    Set the interface filter
    @param filter_priority Filter for VSCP priority.
    @param filter_class Filter for VSCP class.
    @param filter_type Filter for VSCP type.
    @param filter_GUID Filter for GUID.
*/
void vscp_server_command_sflt( uint8_t filter_priority,
                                    uint16_t filter_class,
                                    uint8_t filter_type,
                                    uint8_t *filter_GUID );
                                    
/*!
    Set the interface mask
    @param mask_priority Mask for VSCP priority.
    @param mask_class Mask for VSCP class.
    @param mask_type Mask for VSCP type.
    @param mask_GUID Mask for GUID.
*/
void vscp_server_command_smsk( uint8_t mask_priority,
                                    uint16_t mask_class,
                                    uint8_t mask_type,
                                    uint8_t *mask_GUID );  
     
/*!
    Set user name
    This routine just saves the username and mark that a username
    has been set.
    @param pUser Pointer to username
*/                                    
void vscp_server_command_user( char *pUser );  

/*!
    Set password
    Always return zero if no username has been set.
    If password is wrong also zero username and username set flag.
    @param pPassword Pointer to password
    @return 0 if password is invalid, non-zero if valid
*/                                    
int vscp_server_command_password( char *pPassword );     

/*!
    Get ticks
    @return System ticks. Typically in milliseconds.
*/
uint32_t vscp_server_getTicks( void );                                                               

#endif

