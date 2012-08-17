 /**
 * @brief           HTTP Callbacks
 * @file            httpexec.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_httpsrvr
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements that 
 *
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
 * 2004-10-19, David Hosken (DH):
 *    - Initial version
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#ifndef HTTPEXEC_H
#define HTTPEXEC_H


/////////////////////////////////////////////////
// HTTP Variables - Used for Dynamic Web Pages
/** @addtogroup mod_tags
@section mod_tags_defined Defined Tags
The following list of Tags are implemented by the @appname:<br>
- Variable group @b n - @ref mod_tags_ref_analog
- Variable group @b k - @ref mod_tags_ref_config
- Variable group @b l - @ref mod_tags_ref_general
- Variable group @b a to @b j - @ref mod_tags_ref_ios
- Variable group @b w - @ref mod_tags_ref_pwm
- Variable group @b s - @ref mod_tags_ref_secure
*/
 


/////////////////////////////////////////////////
// HTTP Variable Group - Ports
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
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by HTTP Server - displayed on web page</i></td></tr>
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
// The HTTP Variable values for this group is the new port value in decimal
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
// HTTP Variable Group - Analog Input Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_analog Analog Inputs Tags
The variable group @b n can be used to display the value of any Analog Input on the PIC.

The following variable values(in hex) are defined:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by HTTP Server - displayed on web page</i></td></tr>
<tr><td><code>00 - 0A</code></td><td>2 or 3 digit uppercase hex value</td></tr>
<tr><td><code>10 - 1A</code></td><td>Decimal value 0 - 255 or 0 - 1023</td></tr>
<tr><td><code>20 - 2A</code></td><td>Decimale value, 2 decimal places for 5V reference</td></tr>
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
// The HTTP Variable values for this group is the new port value in decimal
#define VARGROUP_ANALOG ('n')



/////////////////////////////////////////////////
// HTTP Variable Group - General Tags
/** @addtogroup mod_tags
<hr>
@subsection mod_tags_ref_general General Tags
The variable group @b l is used for displaying general information.

The following variable values(in hex) are defined. 
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td align=center><i>Variable Value</i></td>
    <td align=center><i>String returned by HTTP Server - displayed on web page</i></td>
</tr>
<tr><td><code>00</code></td>
    <td><b>Deprecated - use the Username command in the Secure Tags group!</b><br>
    Displays the <b>Username</b> of the current user. For example "Guest" or "Admin". The current user
    will be "Guest" if the user has not logged in. This is a Secure Tag, and requires Authentication to be displayed!</td></tr>
<tr><td><code>01</code></td></td>
    <td>Displays the <b>TCP/IP stack version</b>. For example "V2.04". </td></tr>
<tr><td><code>02</code></td></td>
    <td>Displays the <b>Application version</b>. For example "V3.00".</td></tr>
<tr><td><code>03</code></td></td>
    <td>Returns '1' if <b>Authentication</b> has been provided, else '0'.</td></tr>
<tr><td><code>04</code></td></td>
    <td>Displays 'Yes' if the board has a <b>bootloader</b>, else 'No'</td></tr>
<tr><td><code>05</code></td></td>
    <td>Displays our <b>NetBIOS</b> name</td></tr>
<tr><td><code>10</code></td></td>
    <td>Displays first part of current <b>IP address</b>. For example, will be '5' if our IP is '10.1.0.5'.</td></tr>
<tr><td><code>11</code></td></td>
    <td>Displays second part of current <b>IP address</b>.</td></tr>
<tr><td><code>12</code></td></td>
    <td>Displays third part of current <b>IP address</b>.</td></tr>
<tr><td><code>13</code></td></td>
    <td>Displays fourth part of current <b>IP address</b>. For example, will be '10' if our IP is '10.1.0.5'.</td></tr>
<tr><td><code>14</code></td></td>
    <td>Displays first part of <b>MAC address</b>. For example, will be '5' if our MAC is '0.1.2.3.4.5'.</td></tr>
<tr><td><code>15</code></td></td>
    <td>Displays second part of <b>MAC address</b>.</td></tr>
<tr><td><code>16</code></td></td>
    <td>Displays third part of <b>MAC address</b>.</td></tr>
<tr><td><code>17</code></td></td>
    <td>Displays fourth part of <b>MAC address</b>.</td></tr>
<tr><td><code>18</code></td></td>
    <td>Displays fifth part of <b>MAC address</b>.</td></tr>
<tr><td><code>19</code></td></td>
    <td>Displays sixth part of <b>MAC address</b>. For example, will be '200' if our MAC is '200.1.2.3.4.5'.</td></tr>
<tr><td><code>1A</code></td></td>
    <td>Displays first part of current Network <b>MASK</b>. For example, will be '255' if our mask is '0.0.0.255'.</td></tr>
<tr><td><code>1B</code></td></td>
    <td>Displays second part of current Network <b>MASK</b>.</td></tr>
<tr><td><code>1C</code></td></td>
    <td>Displays third part of current Network <b>MASK</b>.</td></tr>
<tr><td><code>1D</code></td></td>
    <td>Displays fourth part of current Network <b>MASK</b>. For example, will be '15' if our mask is '15.0.0.255'.</td></tr>
<tr><td><code>1E</code></td></td>
    <td>Displays first part of current <b>Gateway address</b>. For example, will be '5' if our Gateway Address is '10.1.0.5'.</td></tr>
<tr><td><code>1F</code></td></td>
    <td>Displays second part of current <b>Gateway address</b>.</td></tr>
<tr><td><code>20</code></td></td>
    <td>Displays third part of current <b>Gateway address</b>.</td></tr>
<tr><td><code>21</code></td></td>
    <td>Displays fourth part of current <b>Gateway address</b>. For example, will be '10' for '10.1.0.5'.</td></tr>
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
// HTTP Variable values for General Group
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



/////////////////////////////////////////////////
// HTTP Variable Group - Config Tags
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
// The HTTP Variable values for this group is the new value (in decimal) for the config byte
#define VARGROUP_CONFIG ('k')


/////////////////////////////////////////////////
// HTTP Variable Group - PWM Output Tags
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
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by HTTP Server - displayed on web page</i></td></tr>
<tr><td><code>01 - 05</code></td><td>2 or 3 digit uppercase hex value</td></tr>
<tr><td><code>11 - 15</code></td><td>Decimal value 0 - 255 (for 8-bit mode) or 0 - 1023 (for 10-bit mode)</td></tr>
<tr><td><code>31 - 35</code></td><td>"&lt;!--" returned if the PWM output is enabled</td></tr>
<tr><td><code>41 - 45</code></td><td>"--&gt;" returned if the PWM output is enabled</td></tr>
<tr><td><code>51 - 55</code></td><td>"&lt;!--" returned if the PWM output is disabled</td></tr>
<tr><td><code>61 - 65</code></td><td>"--&gt;" returned if the PWM output is disabled</td></tr>
</table>
@endhtmlonly

The following additional variable values are defined for the PWM channels.
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by HTTP Server - displayed on web page</i></td></tr>
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
*/
// The HTTP Variable values for this group is the new port value in decimal
#define VARGROUP_PWM ('w')


/////////////////////////////////////////////////
// HTTP Variable Group - Secure Tags
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
    <td align=center><i>String returned by HTTP Server - displayed on web page</i></td>
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
// HTTP Variable values for General Group
#define VARVAL_SEC_USERNAME            (0x00)
#define VARVAL_SEC_PASSWORD            (0x01)


/**
 * Initializes global variables.
 */
void htpexecInit(void);

#endif

