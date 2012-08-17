 /**
 * @brief           Commands
 * @file            cmd.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_httpsrvr
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that executes all received commands
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
 //--------------------  Cmd Configuration --------------------
 //*********************************************************************
 //Default "UDP Command Port"
 #define DEFAULT_CMD_UDPPORT (54123ul)

 //Default "UDP Event Port"
 #define DEFAULT_EVENT_UDPPORT (54124ul)

 //"UDP Command Port"
 #define CMD_UDPPORT     DEFAULT_CMD_UDPPORT

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
 *  - Initial version
 * 2009-05-29, David Hosken (DH):
 *  - Added code for MXD2R and IOR5E expansion boards
 *  - Updated LCD functions to allow 4 LCD displays
 *********************************************************************/

#ifndef CMD_H
#define CMD_H



/////////////////////////////////////////////////
//  Tags - documentation module
/** @defgroup mod_tags Tags
@section mod_tags_intro Introduction
Data can be requested from the target board via Tags.
Tags can be:
<ul>
<li>Placed in Dynamic Web Pages</li>
<li>Sent via UDP messages</li>
<li>Sent via the HTTP GET method</li>
</ul>
All Tags must have the format:
<br>\%nxx
<br>Where the '\%' character serves as a control code, 'n' represents the <b>variable group</b>, and "xx" represents a
two-digit <b>variable value</b> ( in upper case hex format).
<br>The <b>variable value</b> has a range of 00-FF (Which translates to 0-255 decimal), and must use upper case characters!
<br>The <b>variable group</b> ('n' character) can be any alpha numeric character ('0-9', 'a-z', 'A-Z'), giving a total of 10+26+26 = 62 groups.
Each group can have 256 variable values. This gives a total of 15872 possible variables.

An example tag is "%a02". This tag will be replaced by a '1' or '0' character representing the current
value of Port A2. If this tag is placed on a Dynamic Web page, it will be shown on a Web Browser requesting
that page as a '1' or '0'.

@section mod_tags_dynamic Dynamic Web Pages
Any of the Tags listed below can be placed on a CGI web page, and will be replaced
by the requested data when sent to the HTTP Client (Web Browser for example).
<p>For details, see @ref mod_tcpip_httpsrvr_dynamic

@section mod_tags_udp UDP Request Command
Any of the Tags listed below can be executed on the target by sending them
to UDP port 54123. All UDP request commands must start with the \% character, followed by the requested tag.
The UDP port is configurable, and can be changed. The reply (requested data) will be
returned to the UDP Socket that sent this Request Command.

For example, to request the value of port B2, returned as a 0 or 1 character (0=0V at input, 1=5V at input),
send the following command string via UDP to port 54123:<br>
<i>\%b02</i><br>
The value of port B2 will be returned via UDP.<br>
Another example could be to request the analog input value of Analog port 2, returned as a decimal value between 0 - 1023.
To do this, send the following command string via UDP to port 54123:<br>
<i>\%n12</i><br>
*/



/////////////////////////////////////////////////
// Command Variables
/** @addtogroup mod_tags
@section mod_tags_defined Defined Tags
The following list of Tags are implemented by the @appname:<br>
- Variable group @b n - @ref mod_tags_ref_analog
- Variable group @b u - @ref mod_tags_ref_buses
- Variable group @b k - @ref mod_tags_ref_config
- Variable group @b x - @ref mod_tags_ref_xbrd
- Variable group @b l - @ref mod_tags_ref_general
- Variable group @b a to @b j - @ref mod_tags_ref_ios
- Variable group @b w - @ref mod_tags_ref_pwm
- Variable group @b s - @ref mod_tags_ref_secure
*/
 


/////////////////////////////////////////////////
// Variable Group - Ports
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_ios Port Tags
The variable groups @b a to @b j can be used to display the value of any input pin
or port on the PIC. The lower 3 bits of the 8 bit variable value is used to specify the
desired bit of the given port (port is given by variable group). The upper 5 bits are used
to specify how the ports value should be displayed. The following variable groups are defined:
@htmlonly
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr width="100%"><td align="center"><i>Variable Group</i></td><td align="center"><i>Description</i></td></tr>
<tr><td>a</td><td>Port A </td></tr>
<tr><td>b</td><td>Port B </td></tr>
<tr><td>c</td><td>Port C </td></tr>
<tr><td>d</td><td>Port D </td></tr>
<tr><td>e</td><td>Port E </td></tr>
<tr><td>f</td><td>Port F </td></tr>
<tr><td>g</td><td>Port G </td></tr>
<tr><td>h</td><td>Reserved for furture use </td></tr>
<tr><td>j</td><td>Reserved for furture use </td></tr>
</table>
@endhtmlonly

The following variable values(in hex) are defined:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>00 - 07</code></td><td>'1' or '0' returned depending on PORT state</td></tr>
<tr><td><code>10 - 17</code></td><td>"on" or "off" returned depending on PORT state</td></tr>
<tr><td><code>20 - 27</code></td><td>"&lt;!--" returned if port is configured as input</td></tr>
<tr><td><code>28 - 2f</code></td><td>"--&gt;" returned if port is configured as input</td></tr>
<tr><td><code>30 - 37</code></td><td>"&lt;!--" returned if port is configured as output</td></tr>
<tr><td><code>38 - 3f</code></td><td>"--&gt;" returned if port is configured as output</td></tr>
<tr><td><code>40 - 47</code></td><td>"checked" returned if port is configured as output</td></tr>
<tr><td><code>48 - 4f</code></td><td>"checked" returned if port is configured as input</td></tr>
<tr><td><code>50 - 57</code></td><td>"0" returned if port is configured as output. "1" returned if port is configured as input.</td></tr>
</table>
For variable values where bit 3 is set (values from x8 to xf):
<br>x8=port bit 0, x9=port bit 1, xa=port bit 2, xb=port bit 3
<br>xc=port bit 4, xd=port bit 5, xe=port bit 6, xf=port bit 8
<br>For example, variable value 48 will return "checked" if port <b>bit 0</b> is configured as an input
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%a02</code></td><td>Variable group = a, Variable value = 0x02. This example
will display the value of port A2 as '1' or '0'. So, if port A2 is set, a '1' will be displayed
on the web page in stead of '%a02'</td></tr>

<tr><td><code>%c17</code></td><td>Variable group = c, Variable value = 0x17. This example
will display the value of port C7 as 'on' or 'off'. So, if port C7 is clear for example, 'off' will be
displayed on the web page in stead of '%c17'</td></tr>

<tr><td><code>%f22</code></td><td>Variable group = f, Variable value = 0x22. Variable values
20 to 2F can be used to place HTML comments around HTML code if a port is configured as an input. For example,
to only display an image if a Port pin RF1 is configured as an output, you could write:<br>
<i>%f21&lt;img src="picture.jpg"&gt;%f29</i></td></tr>

<tr><td><code>%g52</code></td><td>Variable group = g, Variable value = 0x52. This example
will display '1' if port G2 is configured as an input, and '0' if port G2 is configured as an output.</td></tr>

<tr><td><code>%b45</code></td><td>Variable group = b, Variable value = 0x45. This example
will display 'checked' if port B5 is configured as an output.</td></tr>

<tr><td><code>%c4c</code></td><td>Variable group = c, Variable value = 0x4c. This example
will display 'checked' if port C5 is configured as an intput.</td></tr>
</table>
@endhtmlonly
*/
// The Variable values for this group is the new port value in decimal
#define VARGROUP_PORTA ('a')
#define VARGROUP_PORTB ('b')
#define VARGROUP_PORTC ('c')
#define VARGROUP_PORTD ('d')
#define VARGROUP_PORTE ('e')
#define VARGROUP_PORTF ('f')
#define VARGROUP_PORTG ('g')
#define VARGROUP_PORTH ('h')
#define VARGROUP_PORTJ ('j')



/////////////////////////////////////////////////
// Variable Group - Analog Input Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_analog Analog Inputs Tags
The variable group @b n can be used to display the value of any Analog Input on the PIC.

The following variable values(in hex) are defined:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>00 - 0A</code></td><td>3 digit uppercase hex value, representing requested Analog Input Channel</td></tr>
<tr><td><code>10 - 1A</code></td><td>Decimal value 0 - 255 or 0 - 1023, representing requested Analog Input Channel</td></tr>
<tr><td><code>20 - 2A</code></td><td>Decimale value with 2 decimal places. This is the input voltage of the requested
Analog Channel for 5V reference (default configuration is 5V reference). </td></tr>
<tr><td><code>30 - 3A</code></td><td>"&lt;!--" returned if the channels is configured for ADC</td></tr>
<tr><td><code>40 - 4A</code></td><td>"--&gt;" returned if the channels is configured for ADC</td></tr>
<tr><td><code>50 - 5A</code></td><td>"&lt;!--" returned if the channels is NOT configured for ADC</td></tr>
<tr><td><code>60 - 6A</code></td><td>"--&gt;" returned if the channels is NOT configured for ADC</td></tr>
</table>
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%n02</code></td><td>Variable group = n, Variable value = 0x02. This example
will display the value of Analog Input 2 in uppercase hex. For example, "A8" will be displayed
on the web page in stead of '%n02' if Analog Input 2 has the value 0xA8.</td></tr>

<tr><td><code>%n1A</code></td><td>Variable group = n, Variable value = 0xaA. This example
will display the value of Analog input 10 in decimal. For example, "210" will be displayed
on the web page in stead of '%n1A' if Analog Input 10 has the value 210.</td></tr>

<tr><td><code>%n32</code></td><td>Variable group = n, Variable value = 0x32. Variable values
30-3A and 40-4A can be used to place HTML comments around HTML code if a Port pin is configured as an Analog Input.
For example, to only display an image if a Analog Input 2 is configured as an Analog Input, you could write:<br>
<i>%n32&lt;img src="picture.jpg"&gt;%n42</i></td></tr>
</table>
@endhtmlonly
*/
// The Variable values for this group is the new port value in decimal
#define VARGROUP_ANALOG ('n')



/////////////////////////////////////////////////
// Variable Group - General Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_general General Tags
The variable group @b l is used for displaying general information.

The following variable values(in hex) are defined. 
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td align=center><i>Variable Value</i></td>
    <td align=center><i>String returned by Server - displayed on web page</i></td>
</tr>
<tr><td><code>00</code></td>
    <td><b>Deprecated - use the Username command in the Secure Tags group!</b><br>
    Displays the <b>Username</b> of the current user. For example "Guest" or "Admin". The current user
    will be "Guest" if the user has not logged in. This is a Secure Tag, and requires Authentication to be displayed!</td></tr>
<tr><td><code>01</code></td>
    <td>Displays the <b>TCP/IP stack version</b>. For example "V2.04". </td></tr>
<tr><td><code>02</code></td>
    <td>Displays the <b>Application version</b>. For example "V3.00".</td></tr>
<tr><td><code>03</code></td>
    <td>Returns '1' if <b>Authentication</b> has been provided, else '0'.</td></tr>
<tr><td><code>04</code></td>
    <td>Displays 'Yes' if the board has a <b>bootloader</b>, else 'No'</td></tr>
<tr><td><code>05</code></td>
    <td>Displays our <b>NetBIOS</b> name</td></tr>
<tr><td><code>10</code></td>
    <td>Displays first part of current <b>IP address</b>. For example, will be '5' if our IP is '10.1.0.5'.</td></tr>
<tr><td><code>11</code></td>
    <td>Displays second part of current <b>IP address</b>.</td></tr>
<tr><td><code>12</code></td>
    <td>Displays third part of current <b>IP address</b>.</td></tr>
<tr><td><code>13</code></td>
    <td>Displays fourth part of current <b>IP address</b>. For example, will be '10' if our IP is '10.1.0.5'.</td></tr>
<tr><td><code>14</code></td>
    <td>Displays first part of <b>MAC address</b>. For example, will be '5' if our MAC is '0.1.2.3.4.5'.</td></tr>
<tr><td><code>15</code></td>
    <td>Displays second part of <b>MAC address</b>.</td></tr>
<tr><td><code>16</code></td>
    <td>Displays third part of <b>MAC address</b>.</td></tr>
<tr><td><code>17</code></td>
    <td>Displays fourth part of <b>MAC address</b>.</td></tr>
<tr><td><code>18</code></td>
    <td>Displays fifth part of <b>MAC address</b>.</td></tr>
<tr><td><code>19</code></td>
    <td>Displays sixth part of <b>MAC address</b>. For example, will be '200' if our MAC is '200.1.2.3.4.5'.</td></tr>
<tr><td><code>1A</code></td>
    <td>Displays first part of current Network <b>MASK</b>. For example, will be '255' if our mask is '0.0.0.255'.</td></tr>
<tr><td><code>1B</code></td>
    <td>Displays second part of current Network <b>MASK</b>.</td></tr>
<tr><td><code>1C</code></td>
    <td>Displays third part of current Network <b>MASK</b>.</td></tr>
<tr><td><code>1D</code></td>
    <td>Displays fourth part of current Network <b>MASK</b>. For example, will be '15' if our mask is '15.0.0.255'.</td></tr>
<tr><td><code>1E</code></td>
    <td>Displays first part of current <b>Gateway address</b>. For example, will be '5' if our Gateway Address is '10.1.0.5'.</td></tr>
<tr><td><code>1F</code></td>
    <td>Displays second part of current <b>Gateway address</b>.</td></tr>
<tr><td><code>20</code></td>
    <td>Displays third part of current <b>Gateway address</b>.</td></tr>
<tr><td><code>21</code></td>
    <td>Displays fourth part of current <b>Gateway address</b>. For example, will be '10' for '10.1.0.5'.</td></tr>
<tr><td><code>22</code></td>
    <td>Displays I2C address of <b>LCD Display 1</b>. The default value for a Modtronix I2C LCD display is 80.</td></tr>
<tr><td><code>30</code></td>
    <td>Read status byte of <b>LCD display 1</b> (default address 0x80) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>31</code></td>
    <td>Read status byte of <b>LCD display 2</b> (default address 0x81) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>32</code></td>
    <td>Read status byte of <b>LCD display 3</b> (default address 0x82) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>33</code></td>
    <td>Read status byte of <b>LCD display 4</b> (default address 0x83) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>34</code></td>
    <td>Read keypad data from <b>LCD display 1</b> (default address 0x80) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>35</code></td>
    <td>Read keypad data from <b>LCD display 2</b> (default address 0x81) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>36</code></td>
    <td>Read keypad data from <b>LCD display 3</b> (default address 0x82) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>37</code></td>
    <td>Read keypad data from <b>LCD display 4</b> (default address 0x83) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>38</code></td>
    <td>Read GPIO1, GPIO2 and GPIO3 Inputs from <b>LCD display 1</b> (default address 0x80) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>39</code></td>
    <td>Read GPIO1, GPIO2 and GPIO3 Inputs from <b>LCD display 2</b> (default address 0x81) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>3A</code></td>
    <td>Read GPIO1, GPIO2 and GPIO3 Inputs from <b>LCD display 3</b> (default address 0x82) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>3B</code></td>
    <td>Read GPIO1, GPIO2 and GPIO3 Inputs from <b>LCD display 4</b> (default address 0x83) if present. This tag <b>has not been implemented yet!</b></td></tr>
<tr><td><code>3C</code></td>
    <td>Reserved for future LCD tags</td></tr>
<tr><td><code>3D</code></td>
    <td>Reserved for future LCD tags</td></tr>
<tr><td><code>3E</code></td>
    <td>Reserved for future LCD tags</td></tr>
<tr><td><code>3F</code></td>
    <td>Reserved for future LCD tags</td></tr>
<tr><td><code>40</code></td>
    <td>Status from UDP Event Port. Value 1=OK, 2=heartbeat</td></tr>
</table>
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>%l00</code></td><td>Variable group = l, Variable value = 00. This example
will display the name of the user currently logged in. If the username of the user currently logged in
is "Admin", then "Admin" will be displayed on the web page in stead of '%l00'</td></tr>
<tr><td><code>%l10.%l11.%l12.%l13</code></td><td>Variable group = l, Variable value = 10 to 13. This example
will display our IP address.</td></tr>
</table>
@endhtmlonly
*/
#define VARGROUP_GENERAL ('l')
// Variable values for General Group
#define VARVAL_GEN_USERNAME            (0x00)
#define VARVAL_GEN_STACK_VER_STR       (0x01)
#define VARVAL_GEN_APP_VER_STR         (0x02)
#define VARVAL_GEN_LOGGED_IN           (0x03)
#define VARVAL_GEN_HAS_BL              (0x04)
#define VARVAL_GEN_NETBIOS_NAME        (0x05)
#define VARVAL_GEN_DEMO_MODE           (0x06)
#define VARVAL_GEN_IP0                 (0x10)
#define VARVAL_GEN_IP1                 (0x11)
#define VARVAL_GEN_IP2                 (0x12)
#define VARVAL_GEN_IP3                 (0x13)
#define VARVAL_GEN_MAC0                (0x14)
#define VARVAL_GEN_MAC1                (0x15)
#define VARVAL_GEN_MAC2                (0x16)
#define VARVAL_GEN_MAC3                (0x17)
#define VARVAL_GEN_MAC4                (0x18)
#define VARVAL_GEN_MAC5                (0x19)
#define VARVAL_GEN_MASK0               (0x1A)
#define VARVAL_GEN_MASK1               (0x1B)
#define VARVAL_GEN_MASK2               (0x1C)
#define VARVAL_GEN_MASK3               (0x1D)
#define VARVAL_GEN_GATEWAY0            (0x1E)
#define VARVAL_GEN_GATEWAY1            (0x1F)
#define VARVAL_GEN_GATEWAY2            (0x20)
#define VARVAL_GEN_GATEWAY3            (0x21)
#define VARVAL_GEN_LCD1ADR             (0x22)



/////////////////////////////////////////////////
// Variable Group - Config Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_config Configuration Tags
The variable group @b k is used for displaying the contents of the @ref APP_CONFIG_ENUM
"Application Configuration bytes". The values are displayed in decimal!
The variable value (in uppercase hex) is used to specify the offset of the configuration byte to display.
All of these Tags are <b>Secure Tags</b>, and will only parsed if Authentication has been provided! If no
Authentication has been provided, they will return 0. The "Authentication" tag in the "General Tags"
group can be used to determine if Authentication has been provided.

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%k00.%k01.%k02.%k03</code></td><td>This example will display the currently configured IP
address. If the IP address is "10.1.0.1", then "10.1.0.1" will be displayed on the web page in stead
of "%k00.%k01.%k02.%k03".</td></tr>

<tr><td><code>%k0A.%k0B.%k0C.%k0D</code></td><td>This example will display the currently configured
network MASK. If the MASK is "255.0.0.0", then "255.0.0.0" will be displayed on the web page in stead
of "%k0A.%k0B.%k0C.%k0D".</td></tr>
</table>
@endhtmlonly
*/
// The Variable values for this group is the new value (in decimal) for the config byte
#define VARGROUP_CONFIG ('k')



/////////////////////////////////////////////////
// Variable Group - Bus Config Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_buses Bus Configuration Tags
The variable group @b u is used for displaying the contents of the @ref BUS_CONFIG_ENUM
"Bus Configuration bytes". The values are displayed in decimal!
The variable value (in uppercase hex) is used to specify the offset of the configuration byte to display.
All of these Tags are <b>Secure Tags</b>, and will only parsed if Authentication has been provided! If no
Authentication has been provided, they will return 0. The "Authentication" tag in the "General Tags"
group can be used to determine if Authentication has been provided.

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%u01</code></td><td>This tag will be replaced with the receive buffer size of Serial Bus 1.</td></tr>

</table>
@endhtmlonly
*/
// The Variable values for this group is the new value (in decimal) for the config byte
#define VARGROUP_BUSCFG ('u')



/////////////////////////////////////////////////
// Variable Group - PWM Output Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_pwm PWM Tags
The variable group @b w can be used to display the current settings of the PWM ouputs.

The following variable values(in hex) will display the current value of a PWM channel.
@if BRD_SBC65EC
The @boardname has 4 PWM channels, so the second character can have a value from 1-4.
@endif
@if BRD_SBC68EC
The @boardname has 2 PWM channels, so the second character can have a value from 1-2.
@endif
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>01 - 04</code></td><td>PWM Channel's value as a 3 digit uppercase hex value</td></tr>
<tr><td><code>11 - 14</code></td><td>Decimal value 0 - 255 (for 8-bit mode) or 0 - 1023 (for 10-bit mode)</td></tr>
<tr><td><code>31 - 34</code></td><td>"&lt;!--" returned if the PWM output is enabled</td></tr>
<tr><td><code>41 - 44</code></td><td>"--&gt;" returned if the PWM output is enabled</td></tr>
<tr><td><code>51 - 54</code></td><td>"&lt;!--" returned if the PWM output is disabled</td></tr>
<tr><td><code>61 - 64</code></td><td>"--&gt;" returned if the PWM output is disabled</td></tr>
</table>
@endhtmlonly

The following additional variable values are defined for the PWM channels.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>F0</code></td><td>Display the set frequency value. The frequency is given by a constant value
    between 0 to 3.
    <br>For <b>8-bit mode</b>, the following values are defined: <i>0 and 1</i>=9.766kHz, <i>2</i>=39.062kHz, <i>3</i>=156.25kHz
    <br>For <b>10-bit mode</b>, the following values are defined: <i>0</i>=2.441kHz, <i>1</i>=9.766kHz, <i>2 and 3</i>=39.062kHz
</td></tr>
<tr><td><code>F4</code></td><td>Displays the set mode. A '8' indicates we are currently configured for 8-bit
    mode. A 'a' indicates we are currently configured for 10-bit mode.</td></tr>
<tr><td><code>F8</code></td><td>Displays the enabled PWM channels. An uppercase hex value is returned that
    indicates what channels are enabled. Each bit in the hex value represents a PWM channel.</td></tr>
</table>
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%w02</code></td><td>Variable group = w, Variable value = 0x03. This example
will return the current set value of PWM channel 2. The value is represented as a 3 character
long uppercase hex value.</td></tr>

<tr><td><code>%w14</code></td><td>Variable group = w, Variable value = 0x14. This example
will return the current set value of PWM channel 2. The value is represented as a decimal string.</td></tr>

<tr><td><code>%wf8</code></td><td>Variable group = w, Variable value = 0xf8. This example
will return a two byte hex value. Each bit in the byte represents a PWM channel, and indicates if it
is enabled or not. For example, if only PWM channel 1 is enabled, it will return 01. If only PWM channel 3 is
enabled it will return 04 (binary 00000100). If PWM channel 2 and 4 are enabled, it will return
0A (binary 00001001).
</td></tr>
</table>
@endhtmlonly
*/
// The Variable values for this group is the new port value in decimal
#define VARGROUP_PWM ('w')



/////////////////////////////////////////////////
// Variable Group - Secure Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_secure Secure Tags
The variable group @b s is used for displaying secure information.
All of these Tags are <b>Secure Tags</b>, and will only parsed if Authentication has been provided! If no
Authentication has been provided, they will return 0. The "Authentication" tag in the "General Tags"
group can be used to determine if Authentication has been provided.

The following variable values(in hex) are defined. 
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td align=center><i>Variable Value</i></td>
    <td align=center><i>String returned by Server - displayed on web page</i></td>
</tr>
<tr><td><code>00</code></td>
    <td>Displays the <b>Username</b> of the current user. For example "Guest" or "Admin". The current user
    will be "Guest" if the user has not provided Authentication.</td></tr>
<tr><td><code>01</code></td>
    <td>Displays the <b>Password</b> of the current user.</td></tr>
</table>
@endhtmlonly
*/
#define VARGROUP_SECURE ('s')
// Variable values for General Group
#define VARVAL_SEC_USERNAME            (0x00)
#define VARVAL_SEC_PASSWORD            (0x01)



/////////////////////////////////////////////////
// Variable Group - Expansion Board Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_xbrd Expansion Board Tags
The variable group @b x can be used to display the current settings of the Expansion Board.

The following variable values(in hex) will display the state of the expansion board's <b>relays</b> (if it has any).
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>00 - 07</code></td><td>'1' or '0' returned depending on state of relay. 00 returns state of relay 1, 01 relay 2, ..., and 07 relay 8. </td></tr>
<tr><td><code>10 - 17</code></td><td>"on" or "off" returned depending on state of relay. 10 returns state of relay 1, 11 relay 2, ..., and 17 relay 8. </td></tr>
</table>
@endhtmlonly

The following variable values(in hex) will display the state of the expansion board's <b>opto couple inputs</b> (if it has any).
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>20 - 27</code></td><td>'1' or '0' returned depending on state of Opto Coupled input. 20 returns state of opto 1, 21 opto input 2, ..., and 27 opto input 8. </td></tr>
<tr><td><code>30 - 37</code></td><td>"on" or "off" returned depending on state of Opto Coupled input. 30 returns state of opto 1, 31 opto input 2, ..., and 37 opto input 8. </td></tr>
</table>
@endhtmlonly

The following variable values(in hex) will display the state of the expansion board's <b>inputs</b> (if it has any). <b>These tags are
only currently implemented in @ref mod_events.</b>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by Server - displayed on web page</i></td></tr>
<tr><td><code>40 - 4f</code></td><td>'1' or '0' returned depending on state of Input. 40 returns state of input 1, 41 opto input 2, ..., and 4f input 16. </td></tr>
</table>
@endhtmlonly

<b>Examples:</b>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>%x02</code></td><td>Variable group = x, Variable value = 0x02. This example
will return the current state of relay 3. A '1' is returned if relay is active, and '0' if the relay of off.</td></tr>
<tr><td><code>%x31</code></td><td>Variable group = x, Variable value = 0x31. This example
will return the current state of opto coupler input 2. "on" is returned if it is currently on, and 'off' if it is off.</td></tr>

</td></tr>
</table>
@endhtmlonly
*/
#define VARGROUP_XBRD ('x')



/////////////////////////////////////////////////
//  PWM - documentation module
/** @defgroup mod_pwm PWM Module
@section mod_pwm_intro Introduction
The @boardname has @if BRD_SBC65EC 4 @elseif BRD_SBC68EC 2 @endif PWM (Pulse Width Modulator) channels
that can be configured, controlled and monitored via @ref mod_cmd_ref_pwm "PWM Commands" and @ref mod_tags_ref_pwm "PWM Tags".
An example use for the PWM Channels will be to create a variable DC output voltage. This could for example be used to dim a light.

To use the PWM Module, the following has to be done:
<ul>
<li>Set the desired PWM channel as an output via the @ref page_webpages "Web interface" (on "Port Settings" page). This can also be done via @ref mod_cmd.
@if BRD_SBC65EC The @boardname has 4 PWM channels, C1, C2, G0 and G3.
@elseif BRD_SBC68EC The @boardname has 2 PWM channels, C1 and C2.@endif</li>
<li>Enable the desired PWM channels with the "we=xx" @ref mod_cmd_ref_pwm "command". Each bit in the "xx" part represents
a PWM channel. For example, to enable all 4 channels via the @ref mod_cmd_httpget "HTTP GET Method" (with default board NetBIOS name of mxboard),
type the following into a web browser "http://mxboard/?we=0f". To issue this command via @ref mod_cmd_udp "UDP", send the following UDP message to port 54123 of the board "we=0f".</li>
<li>Set the PWM value for the enabled channel with the "wn=value" @ref mod_cmd_ref_pwm "command". The value is a value from
0-1023, where 0 is 0% duty cycle, and 1023 is 100% duty cycle. For example, to set the duty cycle of channel 1 (C1) to
512 (50%) via the @ref mod_cmd_httpget "HTTP GET Method" (with default board NetBIOS name of mxboard), type the following into a web browser "http://mxboard/?w1=512". To issue
this command via @ref mod_cmd_udp "UDP", send the following UDP message to port 54123 of the board "w1=512"</li>
</ul>

Additionally the PWM module can be configured for 8-bit or 10-bit mode. Default is 10-bit mode.
<ul><li>For 8-bit mode, the PWM frequency can be set to 9.766kHz, 39.062kHz, or 156.25kHz. The Value for 8-bit mode is a value between 0-255, which corresponds to a duty cycle between 0% and 100%.</li>
<li>For 10-bit mode, the PWM frequency can be set to 2.441kHz, 9.766kHz, or 39.062kHz. The Value for 10-bit mode is a value between 0-1023, which corresponds to a duty cycle between 0% and 100%.</li>
</ul>
*/










/////////////////////////////////////////////////
//  Commands - documentation module
/** @defgroup mod_cmd Commands
@section mod_cmd_intro Introduction
Commands can be sent to the target board via UDP messages, or the HTTP GET command.
All commands follow the HTTP GET syntax of name=value.

<p>For example, to set Port pin A2, we can:<br>
Send the following <b>HTTP GET command</b> to the target board:
<br><i>http://10.1.0.1/?a2=1</i>
<br>Or we can send the following <b>UDP message</b> to the UDP Command Port:
<br><i>a2=1</i>
<p>In this example the name part is "a2" and the value part is '1'.

@section mod_cmd_udp UDP Commands
Any of the commands listed below can be executed on the target by sending them
to UDP port 54123. This port is configurable, and can be changed.
<p>For example, to set Port pin A2, we can send the following UDP message
to the target board:
<br><i>a2=1</i>
<p>Multiple commands can be send by seperating each command with a '&' character.
For example, to set Port pin A2, and clear Port C0, we can send the following UDP
message to the target board:
<br><i>a2=1&c0=0</i>

@section mod_cmd_httpget HTTP GET Commands
Any of the commands listed below can be executed on the target by using the HTTP GET
command. To issue a HTTP GET command, simply append the command to the web page
address, after a '?' character.
<p>For example, to set Port pin A2, we can send the following HTTP GET
command to the target board:
<br><i>http://10.1.0.1/?a2=1</i>
<p>Multiple commands can be send by seperating each command with a '&' character.
For example, to set Port pin A2, and clear Port C0, we can send the following HTTP GET
command to the target board:
<br><i>http://10.1.0.1/?a2=1&c0=0</i>
<p>For details, see @ref mod_tcpip_httpsrvr_cgi

@section mod_cmd_defined Defined Commands
The following list of commands are implemented by the @appname in the "httpexec.c" file.
This file can be modified by the user to add aditional commands:<br>
- Command group @b a to @b j - @ref mod_cmd_ref_ios
- Command group @b k - @ref mod_cmd_ref_config
- Command group @b l - @ref mod_cmd_ref_general
- Command group @b m - @ref mod_cmd_ref_short
- Command group @b m - @ref mod_cmd_ref_serial
- Command group @b w - @ref mod_cmd_ref_pwm
- Command group @b x - @ref mod_cmd_ref_xbrd
*/


/////////////////////////////////////////////////
// HTTP Command Group - Ports
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_ios Port Commands
The following syntax is used in the Port commands:
- <b>p</b> - is the <i>Command Group</i>, and represents the Port. It has a value from @b a to @b g.
- <b>[x,y]</b> - This should be replaced by either x or y
- <b>[x-z]</b> - This should be replaced by a value in the range from x to z

The following Port commands are implemented by the @appname.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td nowrap align=center><i>Command Syntax</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>p=XX</code></td><td><b>Set Port</b> - set the given Port to the given hex value. The given hex
value (<i>XX</i> part or command) must be in uppercase! For example:
    <br>"<code>a=8A</code>" will set Port A to 0x8A.
</td></tr>
<tr><td><code>p[0-7]=[1,0]</code></td><td><b>Set Port Pin</b> - set or clear the given Port pin. For example:
    <br>"<code>a0=0</code>" will clear Port A0 (Port A, pin 0)
    <br>"<code>f7=1</code>" will set Port F7 (Port F, pin 7)
</td></tr>
<tr><td><code>px[0-7]=[1,0]</code></td><td><b>Inverted Set Port Pin</b> - Set or clear the given Port pin. The value is inverted! For example:
    <br>"<code>ax0=0</code>" will set Port A0 (Port A, pin 0)
    <br>"<code>fx7=1</code>" will clear Port F7 (Port F, pin 7)
</td></tr>
<tr><td><code>p[0-7]r=[1,0]</code></td><td><b>Set Port Pin, Remember</b> -  - Same as "Set Port Pin" command, except that
    the settings are saved to the application configuration, and are remembered after power up.
</td></tr>
<tr><td><code>p[0-7]p=[0-9]</code></td><td><b>Pulse Port Pin, Positive</b> - The given Port pin is set high and
    then low again. The value gives the 'pulse width' specifying for how many micro seconds the pin should be kept
    high. If 0, the pin is kept high for a minimum time, which is usually between 200ns to 400ns. For example:
    <br>"<code>a0p=0</code>" will set Port A0 high and after 200ns to 400ns low again
    <br>"<code>f7p=9</code>" will set Port F7 high and after 9us low again
</td></tr>
<tr><td><code>p[0-7]n=[0-9]</code></td><td><b>Pulse Port Pin, Negative</b> - The given Port pin is set low and
    then high again. The value gives the 'pulse width' specifying for how many micro seconds the pin should be kept
    low. If 0, the pin is kept low for a minimum time, which is usually about 200ns to 400ns. For example:
    <br>"<code>a0n=0</code>" will set Port A0 low and after 200ns to 400ns high again
    <br>"<code>f7n=5</code>" will set Port F7 low and after 5us high again
</td></tr>
<tr><td><code>pc[0-7]=[1,0]</code></td><td><b>Port Pin Configuration</b> - Configure given Port pin as input or output. '0' will configure
    pin to be an output, and '1' an input. The port direction configured with this command will NOT be remembered
    after the next power up, use the "Port Pin Configuration, Remember" command to achieve this!  Example to use this command:
    <br>"<code>ac0=0</code>" will configure Port A0 (Port A, pin 0) to be an output.
    <br>"<code>fc7=1</code>" will configure Port F7 (Port F, pin 7) to be an input
</td></tr>
<tr><td><code>pc[0-7]r=[1,0]</code></td><td><b>Port Pin Configuration, Remember</b> - Same as "Port Pin Configuration" command, except that
    the settings are saved to the application configuration, and are remembered after power up.
</td></tr>
</table>
@endhtmlonly

Examples:<br>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example GET command</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>f=01</code></td><td>This example will set Port F to 0x01</td></tr>
<tr><td><code>c5=1</code></td><td>This example will set Port C5</td></tr>
<tr><td><code>bx0=1</code></td><td>This example will clear Port B0</td></tr>
</table>
@endhtmlonly
An example of sending a GET command to a target board with an IP address of "10.1.0.1" is:<br>
http://10.1.0.1/?f=01
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
// The HTTP Command values for this group is the new port value in decimal
#define CMDGROUP_PORTA ('a')
#define CMDGROUP_PORTB ('b')
#define CMDGROUP_PORTC ('c')
#define CMDGROUP_PORTD ('d')
#define CMDGROUP_PORTE ('e')
#define CMDGROUP_PORTF ('f')
#define CMDGROUP_PORTG ('g')
#define CMDGROUP_PORTH ('h')
#define CMDGROUP_PORTJ ('i')



/////////////////////////////////////////////////
// HTTP Command Group - Application Config
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_config Application Config Commands
The Application Config Command is used to modifying the @ref APP_CONFIG_ENUM "Application Configuration bytes".
To see what can be configured with this command, please have a look at the list of
@ref APP_CONFIG_ENUM "Application Configuration bytes". 

The following Config commands are implemented by the @appname.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Description</i></td>
</tr>
<tr><td><code>kxx=[0-255]</code></td>
    <td><b>Set Config Byte to given decimal value</b> - set the given config byte to the given decimal value. The 'xx' part
    gives the offset (in uppercase hex) of the config byte in the
    @endhtmlonly
    @ref APP_CONFIG_ENUM "Application Configuration bytes"
    @htmlonly
    structure.
    The value has to be a decimal number from 0 - 255.
    <br>Examples to use this command:
    <p>"k00=100" Will set the the "Application Configuration byte" at 0x00 (MSB of IP address) to 100
    <p>"k2A=16" Will set the "Application Configuration byte" at 0x2A (Port F direction) to 16 (0x0f).
        Address 0x2A contains the TRIS register for Port F. Setting a bit to 0 will configure the port as
        an output, and 1 as an input. In this example we set TRISF to 0x0f = binary 00001111. This will configure port F
        pins 0-3 as inputs, and pins 4-7 as outputs.
    <p>"k2F=16" Will set the "Application Configuration byte" at 0x2F (Port F default value) to 10 (0x0a).
        Address 0x2F contains the default value for Port F at power up. In this example we set it to 0x0a = binary 00001010. This will
        set the default value of port F pins 1 and 3 to 1 (5V), and pins 0, 2, 4, 5, 6, 7 to 0 (0V) at power up.
    </td>
</tr>
<tr><td><code>kxx=Hyy</code></td>
    <td><b>Set Config Byte to given hex value</b> - set the given config byte to the given hex value. The 'xx' part
    gives the offset (in uppercase hex) of the config byte in the
    @endhtmlonly
    @ref APP_CONFIG_ENUM "Application Configuration bytes"
    @htmlonly
    structure.
    The value ('yy' part of command) has to be an uppercase hex value from 00 - FF.
    <br>Examples to use this command:
    <p>"k00=H1F" Will set the "Application Configuration byte" at 0x00 (MSB of IP address) to 0x1F
    </td>
</tr>
<tr><td><code>kxxmzz=Hyy</code></td>
    <td><b>Set Config Byte to given hex value, using mask</b> - set the given config byte to the
    given hex value, using the given mask. The 'xx' part gives the offset (in uppercase hex) of
    the config byte in the
    @endhtmlonly
    @ref APP_CONFIG_ENUM "Application Configuration bytes"
    @htmlonly
    structure. The 'zz' part gives the mask (in uppercase hex) of the bits in the config byte to change.
    If the mask is 00, no bits are changed. If the mask is FF, all bits are set to the given value.
    The value ('yy' part of command) has to be an uppercase hex value from 00 - FF.
    <br>Examples to use this command:
    <p>"k05m04=HFF" Will set the third bit (bit position 2) of the "Application Configuration byte" at
    0x05.
    <p>"k1Am80=H00" Will clear the eighth bit (bit position 7) of the "Application Configuration byte" at
    0x1A.
    </td>
</tr>
<tr><td><code>kxxby=[0,1]</code></td>
    <td><b>Set or Clear bit in Config Byte</b> - set or clear a bit in the given config byte to the
    given value. The 'xx' part gives the offset (in uppercase hex) of the config byte in the
    @endhtmlonly
    @ref APP_CONFIG_ENUM "Application Configuration bytes"
    @htmlonly
    structure. The 'y' part gives the position (value from '0' to '7') of the bit in the config byte to change.
    The value has to be a '0' or '1' character, indicating if the given bit is to be cleared or set.
    <br>Examples to use this command:
    <p>"k05b6=1" Will set the seventh bit (bit position 6) of the "Application Configuration byte" at 0x05.
    <p>"kA5b0=0" Will clear the first bit (bit position 0) of the "Application Configuration byte" at 0xA5.
    </td>
</tr>
</table>
@endhtmlonly


Examples:<br>
An example of sending a GET command to a target board with an IP address of "10.1.0.1" is:<br>
http://10.1.0.1/?k1A=22
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
<br>An example of sending a GET command to a target board with an NetBIOS address of "MXBOARD" is:<br>
http://mxboard/?k1A=22
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
// The HTTP Command values for this group is the new value (in decimal) for the config byte
#define CMDGROUP_CONFIG ('k')


/////////////////////////////////////////////////
// HTTP Command Group - Bus Config
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_busconfig Bus Config Commands
The Bus Config Command is used to modifying the @ref BUS_CONFIG_ENUM "Bus Configuration bytes".
To see what can be configured with this command, please have a look at the list of
@ref BUS_CONFIG_ENUM "Bus Configuration bytes". 
This command has exactly the same systext as the @ref mod_cmd_ref_config command, except that it
starts with a 'u' in stead of a 'k' character.
*/
#define CMDGROUP_BUSCONFIG ('u')


/////////////////////////////////////////////////
// HTTP Command Group - General commands. 
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_general General Commands
The General command has the following syntax: <code>"lx=v"</code>
- The 'l' part is the <i>Command Group</i>, and indicates this is a <i>General Command</i>.
- The 'x' part is the <i>Command Code</i>, and specifies the command that is to be executed.
- The 'v' part is the command operand, and is command specific.

The following General commands are implemented by the @appname.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Description</i></td>
</tr>
<tr><td><code>la=str</code></td>
    <td><b>New Username</b> - assigns a new Username. The given "str" is the new username, and must be an alpha numeric
        string of 1 to 8 characters. For example:
        <br>"<code>la=wombat</code>" will set the new username to "wombat".</td>
</tr>
<tr><td><code>lb=str</code></td>
    <td><b>New Password</b> - assigns a new Password. The given "str" is the new password, and must be an alpha numeric
        string of 1 to 8 characters. For example:
        <br>"<code>lb=gumtree</code>" will set the new username to "gumtree".</td>
</tr>
<tr><td><code>le=XX</code></td>
    <td><b>Enable Events</b>. This command will enable/disable the given Event ports.
    The command value (XX part) is a two character, uppercase hex value indicating which ports are to be enabled.
    Each bit in the byte represents an Event Port. Bit 0 represents the UDP event port, bits 1 to 7 are reserved for future use.
    <br>Examples to use this command are:
    <br>"le=01" will enable the UDP event port, and disable all others.
    <br>"le=00" will disable all event ports.
    </td>
</tr>
<tr><td><code>lk=str</code></td>
    <td><b>Write Byte Array to LCD Display</b> - write given bytes to the LCD display.
        A '_' escape character can be used to send hex values.
        The two bytes following the escape character gives the hex code. See LCD2S documentation for
        list of all commands, and their syntax.
        For example, the syntax for the LCD brighness command is <i>"0x81, value"</i>, where <i>value</i>
        is the new brightness value in hex from 00-FF (decimal 0-255). To set the brighness to a hex value
        of A0 (160 decimal), the following command can be send:
        <br>"<code>lk=_81_A0</code>"
        <br>To instruct the LCD display to remember the brightness setting after the power is removed, the
        command above must be preceded by a "Remember Command" (0x8D). Commands are seperated by a '&' character.
        The syntax for sending a remember command, followed by the set brighness command shown above will be:
        <br>"<code>lk=_8D&lk=_81_A0</code>"
        <br><br>To include a '_' character, two '_' characters must be send after each other.
        </td>
</tr>
<tr><td><code>ll=str</code></td>
    <td><b>Write String to LCD Display</b> - writes the given string to the LCD display.
        A '_' escape character can be used to include one of the following LCD string escape sequences:
        <br>_n = Go to beginning of next line.
        <br>_f = Clear display and go to beginning of first line.
        <br>_r = Go to beginning of first line
        <br>_b = Cursor left
        <br>_t = Cursor Right
        For example:
        <br><br>Clear display, go to the beginning of the first line and write "Hello":
        <br>"<code>ll=_fHello</code>"
        <br><br>Clear display, write "Hello" on first line, and "world" on second line:
        <br>"<code>ll=_fHello_nworld</code>"
        <br><br>To include a '_' character, two '_' characters must be send after each other.
        </td>
</tr>
<tr><td><code>ln=str</code></td>
    <td><b>New NetBIOS name</b> - assigns a new NetBIOS name. The given "str" is the new name, and must be an alpha numeric
        string of 1 to 15 characters. For example:
        <br>"<code>lb=CROC1</code>" will set the new username to "CROC1".</td>
</tr>
</table>
@endhtmlonly

Examples:<br>
The following example will log in an user with username="admin" and password="pw" to a target with an IP address of "10.1.0.1":<br>
http://10.1.0.1/?lu=admin&lp=pw
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.

*/
//the command code, and yyy is the value
#define CMDGROUP_GENERAL ('l')
// The HTTP Command codes for this group
#define CMDCODE_GEN_NEW_USERNAME ('a')      //New username
#define CMDCODE_GEN_NEW_PASSWORD ('b')      //New password
#define CMDCODE_GEN_LCD_ARRAY ('k')         //LCD Byte Array
#define CMDCODE_GEN_LCD_STRING ('l')        //LCD String
#define CMDCODE_GEN_NEW_NETBIOS_NAME ('n')  //New NetBIOS name
#define CMDCODE_GEN_USERNAME ('u')          //The entered username
#define CMDCODE_GEN_PASSWORD ('p')          //The entered password



/////////////////////////////////////////////////
// HTTP Command Group - Short Commands.
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_short Short Commands
Short commands do not have any command operands. They always have the following syntax: <code>"m=v"</code>
- The 'm' part is the <i>Command Group</i>, and indicates this is a <i>Short Command</i>.
- The 'v' part is the <i>Command Code</i>, and specifies the command that is to be executed.

The following Short commands are implemented by the @appname.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Description</i></td>
</tr>
<tr><td><code>m=r</code></td>
    <td><b>Reset Board</b> - reset's the board. This command can be used to remotely reset the board. This can
    be used to update the firmware via the network bootloader for example.</td>
</tr>
<tr><td><code>m=o</code></td>
    <td><b>Log Out</b> - log out the current user. This command is only used for the HTTP Get Method!</td>
</tr>
<tr><td><code>m=l</code></td>
    <td><b>Log In</b> - For Web Server, this command will request the user to log in.</td>
</tr>
</table>
@endhtmlonly

Examples:<br>
The following example will reset a target with an IP address of "10.1.0.1":<br>
http://10.1.0.1/?m=r
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
//where x is the command code. These commands have no value
#define CMDGROUP_GENERAL_NOVAL ('m')
// The HTTP Command codes for this group
#define CMDCODE_GENNOVAL_RESET ('r')     //Reset board
#define CMDCODE_GENNOVAL_LOGOUT ('o')    //Log Out
#define CMDCODE_GENNOVAL_LOGIN ('l')    //Log in


/////////////////////////////////////////////////
// HTTP Command Group - Serial Communication Commands
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_serial Serial Communication Commands
The following Serial Communication commands are implemented by the @appname.
<b>To be added in future version !</b>
*/
//where x is the command code. These commands have no value
#define CMDGROUP_SPI ('s')
// The HTTP Command codes for this group
#define CMDCODE_SPI_CFGCLK ('r')




/////////////////////////////////////////////////
// HTTP Command Group - PWM Communication Commands
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_pwm PWM Commands
The following PWM (Pulse Width Modulator) commands are implemented by the @appname.
These commands are used to enable and set the frequency and values of the PWM channels.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Description</i></td>
</tr>
@endhtmlonly

@if BRD_SBC65EC
@htmlonly
<tr><td nowrap><code>w[1-4]=[0-1023]</code></td>
@endhtmlonly
@endif

@if BRD_SBC68EC
@htmlonly
<tr><td nowrap><code>w[1-2]=[0-1023]</code></td>
@endhtmlonly
@endif

@htmlonly
    <td><b>Set PWM Value</b>. This command will set the value (Duty Cycle) of the given PWM channel.
    The possible values must be in the range 0-255 for 8-bit mode, and 0-1023 for 10-bit mode.
    A value of 0 will give a duty cycle of 0% (output off), where a maximum value will give a duty
    cycle of 100% (permanently on).
    <br>Examples to use this command:
    <br>"w1=128" will set the duty cycle of PWM channel 1 to 50% when in 8-bit mode
    <br>"w3=256" will set the duty cycle of PWM channel 3 to 25% when in 10-bit mode
    </td>
</tr>
@endhtmlonly

@if BRD_SBC65EC
@htmlonly
<tr><td nowrap><code>w[1-4]r=[0-1023]</code></td>
@endhtmlonly
@endif

@if BRD_SBC68EC
@htmlonly
<tr><td nowrap><code>w[1-2]r=[0-1023]</code></td>
@endhtmlonly
@endif

@htmlonly
    <td><b>Set PWM Value, Remember</b>. Same as the <i>Set PWM Value</i> command, except that the
    settings are saved to the application configuration, and are remembered after power up.</b></td>
</tr>
<tr><td><code>wf=[0-3]</code></td>
    <td><b>Set PWM Frequency</b>. This command will set the PWM frequency. The available frequencies
    depends on the PWM mode (8-bit or 10-bit) used. The frequency is given by a constant value between
    0 to 3.
    <br>For <b>8-bit mode</b>, the following values are defined: <i>0 and 1</i>=9.766kHz, <i>2</i>=39.062kHz, <i>3</i>=156.25kHz
    <br>For <b>10-bit mode</b>, the following values are defined: <i>0</i>=2.441kHz, <i>1</i>=9.766kHz, <i>2 and 3</i>=39.062kHz
    </td>
</tr>
<tr><td><code>wfr=[0-3]</code></td>
    <td><b>Set PWM Frequency, Remember</b>. Same as the <i>Set PWM Frequency</i> command, except that the
    settings are saved to the application configuration, and are remembered after power up.</b></td>
</tr>
<tr><td><code>we=XX</code></td>
    <td><b>Enable PWM Channels</b>. This command will enable/disable the given PWM channels.
    The command value (XX part) is a two character, uppercase hex value indicating which channels are to be enabled.
    Each bit in the byte represents a PWM channel. Bit 0 represents PWM channel 1, bit 1 PWM channel 2, ...., and bit 7 PWM channel 8.
    @endhtmlonly
    @if BRD_SBC65EC The @boardname has 4 PWM channels (C1, C2, G0 and G3), so only the fist 4 bits are used.
    @elseif BRD_SBC68EC The @boardname has 2 PWM channels, so only the fist 4 bits are used.
    @endif
    @htmlonly
    Examples to use this command are:
    <br>"we=00" will disable all channels
    <br>"we=1F" will enable PWM channels 1 to 5, and disable all the rest (if any)
    <br>"we=14" will enable PWM channel 3 and 5, and disable all the rest (if any)
    </td>
</tr>
<tr><td><code>wer=XX</code></td>
    <td><b>Enable PWM Channels,Remember</b>. Same as the <i>Enable PWM Channels</i> command, except that the
    settings are saved to the application configuration, and are remembered after power up.</b></td>
</tr>
<tr><td><code>wm=[8,a]</code></td>
    <td><b>Set PWM Mode</b>. This command will set the PWM mode. When the value is '8', the mode
    is set to 8-bit mode. When the value is 'a', the mode is set to 10-bit mode.
    </td>
</tr>
<tr><td><code>wmr=[8,a]</code></td>
    <td><b>Set PWM Mode, Remember</b>. Same as the <i>Set PWM Mode</i> command, except that the
    settings are saved to the application configuration, and are remembered after power up.</b></td>
</tr>
</table>
@endhtmlonly

Examples:<br>
The following example will enable PWM channels 1 and 2 (bit 0 and 1 = hex 0x03) of a target with
a NetBIOS address of "MXBOARD":<br>
http://MXBOARD/?we=03
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
//where x is the command code. These commands have no value
#define CMDGROUP_PWM ('w')




/////////////////////////////////////////////////
// HTTP Command Group - Expansion Board Commands
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_xbrd Expansion Board Commands
The following Expansion Board commands are implemented by the @appname.
These commands are used to configure and control a possible expansion board that could be plugged
into the daughter board connector of the @boardname.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Description</i></td>
</tr>

<tr><td><code>xr1=[0,1]</code></td>
    <td><b>Set Relay 1 of Expansion board</b> - 0 will turn relay off, 1 will turn it on. Only executed if expansion board has such a relay.</td>
</tr>
<tr><td><code>xr2=[0,1]</code></td>
    <td><b>Set Relay 2 of Expansion board</b> - 0 will turn relay off, 1 will turn it on. Only executed if expansion board has such a relay.</td>
</tr>
<tr><td><code>xr3=[0,1]</code></td>
    <td><b>Set Relay 3 of Expansion board</b> - 0 will turn relay off, 1 will turn it on. Only executed if expansion board has such a relay.</td>
</tr>
<tr><td><code>xr4=[0,1]</code></td>
    <td><b>Set Relay 4 of Expansion board</b> - 0 will turn relay off, 1 will turn it on. Only executed if expansion board has such a relay.</td>
</tr>
<tr><td><code>xr5=[0,1]</code></td>
    <td><b>Set Relay 5 of Expansion board</b> - 0 will turn relay off, 1 will turn it on. Only executed if expansion board has such a relay.</td>
</tr>

<tr><td><code>xo1=[0,1]</code></td>
    <td><b>Set Digital Output 1</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo2=[0,1]</code></td>
    <td><b>Set Digital Output 2</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo3=[0,1]</code></td>
    <td><b>Set Digital Output 3</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo4=[0,1]</code></td>
    <td><b>Set Digital Output 4</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo5=[0,1]</code></td>
    <td><b>Set Digital Output 5</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo6=[0,1]</code></td>
    <td><b>Set Digital Output 6</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo7=[0,1]</code></td>
    <td><b>Set Digital Output 7</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>
<tr><td><code>xo8=[0,1]</code></td>
    <td><b>Set Digital Output 8</b> - 0 will turn output off (set to 0V for most boards), 1 will turn it on (set to 5V for most boards). Only executed if expansion board has such an output, and it is enabled.</td>
</tr>

</table>
@endhtmlonly

Examples:<br>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example GET command</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>xr1=1</code></td><td>This example will set enable relay 1</td></tr>
<tr><td><code>xr3=0</code></td><td>This example will set disable relay 3</td></tr>
<tr><td><code>xo2=1</code></td><td>This example will set digital output 2</td></tr>
</table>
@endhtmlonly
An example of sending a GET command to a target board with an IP address of "10.1.0.1" is:<br>
http://10.1.0.1/?xr3=0
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
//where x is the command code. These commands have no value
#define CMDGROUP_XBRD ('x')


/************************************************
 ***** Command Update flags                 *****
 ************************************************/
#define CMD_UD_UNDEFINED    0x0001ul    /* Update a valid, but undefined settings */
#define CMD_UD_NETWORK      0x0002ul    /* Update the network settings */
#define CMD_UD_USART        0x0004ul    /* Update the USART settings */
#define CMD_UD_CPU_IO       0x0008ul    /* Update the CPU IO settings */
#define CMD_UD_ADC          0x0010ul    /* Update the ADC settings */
#define CMD_UD_TFTP         0x0020ul    /* Update the TFTP settings */
#define CMD_UD_SMTP         0x0040ul    /* Update the SMTP settings */
#define CMD_UD_SYSTEM       0x0080ul    /* Update the System settings */
#define CMD_UD_PWM          0x0100ul    /* Update the PWM settings */
#define CMD_UD_WEB          0x0200ul    /* Update the Web Server settings */
#define CMD_UD_I2C_LCD      0x0400ul    /* Update the I2C LCD settings */
#define CMD_UD_BUSBUF       0x0800ul    /* Update the Serial Buses Buffer settings */
#define CMD_UD_BUSNET       0x1000ul    /* Update the Bus Network settings */
#define CMD_UD_XBOARD       0x2000ul    /* Update the Expansion Board settings */

/**
 * Initializes command processing
 */
void cmdInit(void);


/**
 * Must be called every couple of ms
 *
 */
void cmdTask(void);

/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command
 * @param value The value part of the name-value command
 * @param user The user level, is a USER_XX constant
 *
 * @return Returns an UD_XXX flag (or multiple ones ORed together) indicating what has been updated. Not
 * all name-value commands will return a value here.
 */
WORD execNameValueCmd(BYTE * name, BYTE * value, BYTE user);

/**
 * Structure for passing variables to getVarCmd() function
 */
typedef struct _GETTAG_INFO
{
    /** Requested variable has a group and value */
    BYTE tagVal;
    BYTE tagGroup;

    /**
     * Input and output variable.
     * - Will contain HTTP_START_OF_VAR for fist call
     * - If there are no more values left for this variable, it will contain HTTP_END_OF_VAR on return
     * - If there are still bytes to send, it will contain a value other than HTTP_START_OF_VAR and
     *   HTTP_END_OF_VAR reference on return
     */
    WORD ref;

    /** Pointer to buffer for writing variable to */
    BYTE* val;

    /** For future version!!!!
    /** Availble space in val for writing data to */
    //BYTE valSize;

    /** The current user, is a USER_XX constant */
    BYTE user;

    struct
    {
        unsigned char bFinished : 1;    //If set, getVarCmd() returned all requested data
    } flags;
} GETTAG_INFO;

/**
 * Get the requested tag. The command might not be executed if a higher user level is
 * required.
 *
 * @param pGetTagInfo Pointer to GETTAG_INFO structure.
 *
 * @return Returns number of bytes written to pGetTagInfo->buf. If this value is less then the size of
 *         the buffer given, then all requested data has been returned (flags.bFinished flag will also be set)
 */
WORD cmdGetTag(GETTAG_INFO* pGetTagInfo);



#endif
