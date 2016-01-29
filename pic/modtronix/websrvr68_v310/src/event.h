 /**
 * @brief           Events
 * @file            event.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_httpsrvr
 *
 *
 * @section description Description
 **********************************
 * This module contains the code for sending events
 *
 * @section cmd_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //--------------------  Event Configuration --------------------
 //*********************************************************************
 //Default "UDP Event Port"
 #define DEFAULT_EVENT_UDPPORT (54124)

 //"UDP Event Port"
 #define EVENT_UDPPORT DEFAULT_EVENT_UDPPORT

 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2006-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef EVENT_H
#define EVENT_H

#include "net\udp.h"


//This is used by other application modules
#ifndef THIS_IS_EVENT_MODULE

//Configures what event ports are active
extern BYTE activeEventPorts;

extern UDP_SOCKET udpSocketEvt;

#endif

/////////////////////////////////////////////////
//Defines for activeEventPorts variable
#define EVT_PORT_UDP    0b00000001ul
#define EVT_PORT_SER1   0b00000010ul    /* Not currently used */
#define EVT_PORT_SER2   0b00000100ul    /* Not currently used */


/////////////////////////////////////////////////
//  Events - documentation module
/** @defgroup mod_events Events
@section mod_events_intro Introduction
In a control system, there are often events that occur that we have to be notified about. For example, when
an alarm input goes high. Events are sent to an event port. Currently the only implemented
event port is the "UDP Event Port". Each event is sent in a "name=value;" format.
Events have the following format:
@arg <code>eventName=eventValue;</code>

The eventName is the event name (uses @ref mod_tags), and eventValue is it's value. The event name uses text lables
as defined by @ref mod_tags. Each event is terminated with a ';' character. If the value
contains a ';' character, two ';' characters are sent after each other. Multiple events can also be sent in a single
message, and will have the following format:
@arg <code>event1Name=event1Value;event2Name=event2Value;event3Name=event3Value;....</code>

An example event will be when a port changes state. For example, if port A2 changes value from 0 to 1, the
following event will be sent (if the system is configured to send events for port A2):
<br>a02=1;

@section mod_events_activating Activating Events
To activate events, an UDP packet has to be send to port 54124 of the @appname by a remote client. The first two bytes of this packet
must be an uppercase hex value indicating which ports are to be enabled. Each bit in the byte represents an Event Port.
Bit 0 represents the UDP event port, bits 1 to 7 are reserved for future use. For example, to enable the UDP event port, 01 must
be sent to port 54124. After receiving this UDP packet, the @appname will start streaming events back to the sender.

@section mod_events_current Supported Events
Currently the following events are supported:
@htmlonly
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr width="100%"><td align="center"><i><b>Event Name</b></i></td><td align="center"><i><b>Format</b></i></td><td align="center"><i><b>Description</b></i></td></tr>
<tr><td><b>Heartbeat Event</b></td> <td><code>l40=2;</code></td>
    <td>Every second a heartbeat event is sent. The format is "l40=2;". As long as the remote client
    receives this event (in a UDP packet) each second, it knows the server is still alive, and will sent it any events that arise.</td>
</tr>
<tr><td><b>Serial LCD 1 Key Event</b></td> <td><code>l34=x;</code></td>
    <td>If a serial LCD display (I2C address 80) is connected to the I2C port of the @boardname, an event will be sent
    each time a key is pressed (LCD display can have a keyboard connected to it). The x part of the event is the key code, and is a character from 'a' to 'p' (one 
    of 16 possible keys).</td>
</tr>
<tr><td><b>Serial LCD 2 Key Event</b></td> <td><code>l35=x;</code></td>
    <td>Same as <i>Serial LCD 1 Key Event</i>, but for LCD display with I2C address 81.</td>
</tr>
<tr><td><b>Serial LCD 3 Key Event</b></td> <td><code>l36=x;</code></td>
    <td>Same as <i>Serial LCD 1 Key Event</i>, but for LCD display with I2C address 82.</td>
</tr>
<tr><td><b>Serial LCD 4 Key Event</b></td> <td><code>l37=x;</code></td>
    <td>Same as <i>Serial LCD 1 Key Event</i>, but for LCD display with I2C address 83.</td>
</tr>
</table>
@endhtmlonly

The following events are sent if an <b>expansion board</b> (like the IOR5E or MXD2R) is present:
@htmlonly
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr width="100%"><td align="center"><i><b>Event Name</b></i></td><td align="center"><i><b>Format</b></i></td><td align="center"><i><b>Description</b></i></td></tr>
<tr><td><b>Expansion Board Input 1 Event</b></td> <td><code>x40=x;</code></td>
    <td>If the expansion board has an Input 1, this event is sent if this input changes. The event is "x40=0;" if the input is 0,
    or "x40=1" if the input is 1.</td>
</tr>
<tr><td><b>Expansion Board Input 2 Event</b></td> <td><code>x41=x;</code></td>
    <td>Same as <i>Expansion Board Input 1 Event</i>, but for Input 2.</td>
</tr>
<tr><td><b>Expansion Board Input 3 Event</b></td> <td><code>x42=x;</code></td>
    <td>Same as <i>Expansion Board Input 1 Event</i>, but for Input 3.</td>
</tr>
<tr><td><b>Expansion Board Input 4 Event</b></td> <td><code>x43=x;</code></td>
    <td>Same as <i>Expansion Board Input 1 Event</i>, but for Input 4.</td>
</tr>
<tr><td><b>Expansion Board Input 5 Event</b></td> <td><code>x44=x;</code></td>
    <td>Same as <i>Expansion Board Input 1 Event</i>, but for Input 5.</td>
</tr>
<tr><td><b>Expansion Board Input 6 Event</b></td> <td><code>x45=x;</code></td>
    <td>Same as <i>Expansion Board Input 1 Event</i>, but for Input 6.</td>
</tr>
<tr><td><b>Expansion Board Opto Coupler Input 1 Event</b></td> <td><code>x20=x;</code></td>
    <td>If the expansion board has an Opto Coupler Input 1, this event is sent if this input changes. The event is "x20=0;" if the input is off,
    or "x20=1" if the input is active.</td>
</tr>
<tr><td><b>Expansion Board Opto Coupler Input 2 Event</b></td> <td><code>x21=x;</code></td>
    <td>Same as <i>Expansion Board Opto Coupler Input 1 Event</i>, but for Input 2.</td>
</tr>
<tr><td><b>Expansion Board Opto Coupler Input 3 Event</b></td> <td><code>x21=x;</code></td>
    <td>Same as <i>Expansion Board Opto Coupler Input 1 Event</i>, but for Input 3.</td>
</tr>
<tr><td><b>Expansion Board Opto Coupler Input 4 Event</b></td> <td><code>x21=x;</code></td>
    <td>Same as <i>Expansion Board Opto Coupler Input 1 Event</i>, but for Input 4.</td>
</tr>
</table>
@endhtmlonly
*/


/**
 * Initializes "UDP Command Port" and "UDP Event Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void evtInit(void);

/**
 * Must be called every couple of ms
 *
 */
void evtTask(void);

/**
 * Check LCD display every 50ms
 *
 * @return Set's bit 0 in returned byte if data added to UDP buffer
 */
BYTE chkLCD(void);

#endif
