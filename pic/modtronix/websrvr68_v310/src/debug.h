/**
 * @brief           Code Debugging
 * @file            debug.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_debug
 *
 *
 * @section description Description
 **********************************
 * Debug implementation. This file used to define the following debug function:
 * - dbPutByte()
 * - dbPutRomString()
 * - dbPutRomString()
 *
 * The default implementation is to define them to use the serial port. This can
 * however be changed and a different implementation is required. For example,
 * it can be changed so that the debug information is sent via a UDP socket
 * in stead of the serial port.
 *
 * For details on debugging, see @ref mod_sys_debug "Debugging" module.
 *
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * David Hosken         1 Sep, 2005 Created documentation
 ********************************************************************/


 /**
@defgroup mod_sys_debug Debugging
@ingroup mod_sys
@section mod_sys_debug_introduction Introduction
For code development, the debug feature can be enabled. To do this, comment out
the following line in the "projdefs.h" file:
@code #define DEBUG_OFF @endcode
For the production version, please be sure to <b>disable</b> the debug feature! If enabled, it will
slow the code down a bit. All pre-built hex files shipped with the @appname have debugging switched off!
To use debugging, the above mentioned define must be removed (commented out) and the project rebuilt!

Additionally, the debug levels have to be set for the different code modules. This is done in the lines
following the "#define DEBUG_OFF" line in the "projdefs.h" file.
The following debug levels can be enabled for each file:
- Error   = LOG_ERROR - Only errors are outputted via the debug port.
- Warning = LOG_WARN - Errors and warnings are outputted via the debug port.
- Info    = LOG_INFO - Error, warnings and information is outputted via the debug port.
- Debug   = LOG_DEBUG - Error, warnings, information and debug information is outputted via the debug port.

Each debug module has to be configured for its desired debug level. For example, if detailed debug
information is required for the "http" module, the "#define DEBUG_HTTP LOG_WARN" line can be changed
to "#define DEBUG_HTTP LOG_DEBUG".

In the source code debug information is written out with lines that look like this:
@code
#if (DEBUG_HTTP >= LOG_INFO)
debugPutMsg(1);
#endif
@endcode

This example will display the debug message defined in the project file in the "HTTP" section with a 
"message code" = 1. The debug message will only be written out if the debug level for the HTTP module
is configured (in the projdefs.h file) to be LOG_INFO or higher (LOG_ERROR, LOG_WARN or LOG_INFO).

After the project has been configured for the desired debug level, it has to be recompiled and the
target board programmed with the new firmware.

@section mod_sys_debug_add Adding Debug Messages
Adding debug messages to new code is simple:
- Add a debugPutMsg(n) line to your code that will output a defined debug message.
- Define this message in the project file. The default project file delivered with this project is
<i>"../src/debug.xml"</i>

After following these steps, the code has to be recompiled and the new firmware uploaded to the target board.

********************************************************************/

/**
@page page_debug Debugging
Debugging is a very important part of developing new code. Without a good way to debug code, it becomes
very difficult to find and fix bugs in the code. The @ref tools_mxd was developed to make it easier
developing complex projects that can be created with the Modtronix TCP/IP stack.
For details, click @ref tools_mxd "here".

The @ref tools_mxd is an application that will display detailed debug information for very short messages
received from the hardware being debugged. These detailed messages are defined in XML project files. Each
project can have multiple project files. The @appname is shipped with two project files:
- The one is in the "net" folder at <i>"../src/net/mxdebug.xml"</i>. This project file should not be modified
  by the user (unless the user makes modifications to the TCP/IP stack), and contains debug defines for
  the Modtronix TCP/IP Stack.
- The second is in the "src" folder at <i>"../src/mxdebug.xml"</i>. This project file contains debug messages
  for the application. All code added by the user should be in the "src" folder. Any debug info the user wants
  to add should be done in this project file.

At delivery, the @appname has debugging disabled. All pre-built hex files shipped with the @appname also have debugging
switched off! For details on changing the debug information outputted, or disabling it, see
the @ref mod_sys_debug section of this document.

To enable and view the debug information outputted by the @appname, do the following:
- Download the @appname source code, and comment out the "#define DEBUG_OFF" line in the "projdefs.h" file.
- Open one of the supplied MPLAB project files in MPLAB, and build the project. The project will now be built
  with the debugger enabled - debug messages will be output on the serial port.
- Download, install and start the @ref tools_mxd
- Select the project folder with the "File", "Select Project Folder" menu item. Set it to
  the <i>"../src"</i> folder of the dowloaded source code. After doing this multiple tabs will be created in
  the @ref tools_mxd.
- Connect the target board to the serial port of the PC running the @ref tools_mxd. The serial port used can
  be configured in the program options. The default speed is 57600.
- Apply power to the target board. It will now start outputting debug information that will be displayed
 in the @ref tools_mxd.
********************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

/*******************************************************
Defines for USART1 implementation
********************************************************/
//#include "serbasic.h"
#include "busser1.h"

//Debug logging levels. In proj
#define LOG_OFF    (0)
#define LOG_FATAL  (10)
#define LOG_ERROR  (20)
#define LOG_WARN   (30)
#define LOG_INFO   (40)
#define LOG_DEBUG  (50)


//If any debug is enabled:
//- define all debug functions - are mapped to functions that will write to USART or somewhere else


/**
 * Sent the given debug message to the "Modtronix Embedded Debugger". The following is sent:
 * [debugCode][msgCode][msgString]
 *
 * @param debugCode The Debug Message's "Debug Code". This will be the first byte sent.
 * @param msgCode The Debug Message's "Message Code". This will be the second byte sent.
 */
#define debugPut2Bytes(debugCode, msgCode) serPut2Bytes(debugCode, msgCode)


#define debugPut2BytesAndStr(debugCode, msgCode) serPut2Bytes(debugCode, msgCode)

/**
 * Sent the given debug message to the "Modtronix Embedded Debugger". The following is sent:
 * [debugCode][msgCode][msgString]
 *
 * @param debugCode The Debug Message's "Debug Code". This will be the first byte sent.
 * @param msgCode The Debug Message's "Message Code". This will be the second byte sent.
 * @param msgString The Debug Message's "Message String", has to be a NULL terminated string. This will be sent after the msgCode.
 */
#define debugMsgRomStr(debugCode, msgCode, msgString) serPutDebugMsgRomStr(debugCode, msgCode, msgString)

/**
 * Writes the given byte out to the serial port in ascii hex format. For example, if
 * decimal 11 is passed to this function, it will write "0B" will be written to the
 * serial port.
 * @param a     Byte to write to the serial port.
 */ 
#define debugPutByteHex(a) serPutByteHex(a)

/**
 * Writes the given byte out to the serial port as is.
 * @param a     Byte to write to the serial port.
 */ 
#define debugPutByte(a) serPutByte(a)


/**
 * Writes the given null terminated string out to the serial port. The NULL is ALSO written!
 * @param a     Null terminated string to write out to the serial port. The trailing NULL
 *              is ALSO written!
 */ 
#define debugPutRomString(a) serPutRomStringAndNull(a)


/**
 * Writes the given null terminated string out to the serial port. The NULL is ALSO written!
 * @param a     Null terminated string to write out to the serial port. The trailing NULL
 *              is ALSO written.
 */ 
#define debugPutString(a) serPutStringAndNull((BYTE* )a)

/**
 * Writes the given null terminated string out to the serial port. The NULL is excluded,
 * and is NOT written!
 * @param a     Null terminated string to write out to the serial port. The trailing NULL
 *              is NOT written!
 */ 
#define debugPutRomStringXNull(a) serPutRomString(a)


/**
 * Writes the given null terminated string out to the serial port. The NULL is excluded,
 * and is NOT written!
 * @param a     Null terminated string to write out to the serial port. The trailing NULL
 *              is NOT written.
 */ 
#define debugPutStringXNull(a) serPutString((BYTE *)a)

#endif  //_DEBUG_H_
