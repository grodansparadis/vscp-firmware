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

/*******************************************************
 ***** HTTP Variables - Used for Dynamic Web Pages *****
 *******************************************************/
/** @addtogroup mod_tcpip_httpsrvr_dynamic
@section mod_tcpip_httpsrvr_dynamic_tags Defined Tags
The following list of tags are implemented by the @appname in the "httpexec.c" file.
This file can be modified by the user to add aditional tags:<br>
- Variable group @b a to @b j - @ref mod_tcpip_httpsrvr_dynamic_tags_ios
- Variable group @b n - @ref mod_tcpip_httpsrvr_dynamic_tags_analog
- Variable group @b l - @ref mod_tcpip_httpsrvr_dynamic_tags_general
- Variable group @b k - @ref mod_tcpip_httpsrvr_dynamic_tags_config
*/
 

/////////////////////////////////////////////////
// HTTP Variable Group - Ports
/** @addtogroup mod_tcpip_httpsrvr_dynamic
<hr>
@subsection mod_tcpip_httpsrvr_dynamic_tags_ios PIC Ports
The variable groups @b a to @b j can be used to display the value of any input pin
or port on the PIC. The lower 3 bits of the 8 bit variable value is used to specify the
desired bit of the given port (port is given by variable group). The upper 5 bits are used
to specify how the ports value should be displayed. The following variable groups are defined:
@htmlonly
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr width="100%"><td align="center"><i>Variable Group</i></td><td align="center"><i>Description</i></td></tr>
<tr><td>a</td><td>PIC Port A </td></tr>
<tr><td>b</td><td>PIC Port B </td></tr>
<tr><td>c</td><td>PIC Port C </td></tr>
<tr><td>d</td><td>PIC Port D </td></tr>
<tr><td>e</td><td>PIC Port E </td></tr>
<tr><td>f</td><td>PIC Port F </td></tr>
<tr><td>g</td><td>PIC Port G </td></tr>
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
<tr><td><code>50 - 57</code></td><td>"0" returned if port is configured as output. "1" returned if port is configured as input.</td></tr>
</table>
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example Tag</i></td><td align=center><i>Description</i></td></tr>

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
// HTTP Variable Group - Analog inputs
/** @addtogroup mod_tcpip_httpsrvr_dynamic
<hr>
@subsection mod_tcpip_httpsrvr_dynamic_tags_analog Analog Inputs
The variable group @b n can be used to display the value of any Analog Input on the PIC.

The following variable values(in hex) are defined:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center><i>Variable Value</i></td><td align=center><i>String returned by HTTP Server - displayed on web page</i></td></tr>
<tr><td><code>00 - 0A</code></td><td>2 or 3 digit hex value in capital letters</td></tr>
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
<tr width="100%"><td align=center nowrap ><i>Example Tag</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%n02</code></td><td>Variable group = n, Variable value = 0x02. This example
will display the value of Analog Input 2 in uppercase hex. For example, "A8" will be displayed
on the web page in stead of '%n02' if Analog Input 2 has the value 0xA8.</td></tr>

<tr><td><code>%n1A</code></td><td>Variable group = n, Variable value = 0xaA. This example
will display the value of Analog input 10 in decimal. For example, "210" will be displayed
on the web page in stead of '%n1A' if Analog Input 10 has the value 210.</td></tr>

<tr><td><code>%n32</code></td><td>Variable group = n, Variable value = 0x32. Variable values
30-3A and 40-4A can be used to place HTML comments around HTML code if a PIC port pin is configured as an Analog Input.
For example, to only display an image if a Analog Input 2 is configured as an Analog Input, you could write:<br>
<i>%n32&lt;img src="picture.jpg"&gt;%n42</i></td></tr>
</table>
@endhtmlonly
*/
// The HTTP Variable values for this group is the new port value in decimal
#define VARGROUP_ANALOG ('n')


/////////////////////////////////////////////////
// HTTP Variable Group - General
/** @addtogroup mod_tcpip_httpsrvr_dynamic
<hr>
@subsection mod_tcpip_httpsrvr_dynamic_tags_general General
The variable group @b l is used for displaying general information.

The following variable values(in hex) are defined. If the <i>Logged In</i> column contains a 'X'
character then this tag will only be parsed if the current user is logged in!
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td align=center><i>Variable Value</i></td>
    <td align=center><i>Logged In</i></td>
    <td align=center><i>String returned by HTTP Server - displayed on web page</i></td>
</tr>
<tr><td><code>00</code></td><td align=center>X</td>
    <td>Displays the Username of the current user. For example "Guest" or "Admin". The current user
    will be "Guest" if the user has not logged in.</td></tr>
<tr><td><code>01</code></td></td><td align=center>X</td>
    <td>Displays the version of the TCP/IP stack. For example "V2.04". </td></tr>
<tr><td><code>02</code></td></td><td align=center>X</td>
    <td>Displays the version of the Application. For example "V3.00".</td></tr>
<tr><td><code>03</code></td></td><td align=center>&nbsp;</td>
    <td>Returns '0' if we are currently not logged in, or '1' if we are.</td></tr>
<tr><td><code>04</code></td></td><td align=center>X</td>
    <td>Displays 'Yes' if the board has a bootloader, else 'No'</td></tr>
</table>
@endhtmlonly

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example Tag</i></td><td align=center><i>Description</i></td></tr>

<tr><td><code>%l00</code></td><td>Variable group = l, Variable value = 00. This example
will display the name of the user currently logged in. If the username of the user currently logged in
is "Admin", then "Admin" will be displayed on the web page in stead of '%l00'</td></tr>
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

/////////////////////////////////////////////////
// HTTP Variable Group - Config
/** @addtogroup mod_tcpip_httpsrvr_dynamic
<hr>
@subsection mod_tcpip_httpsrvr_dynamic_tags_config Configuration Values
The variable group @b k is used for displaying the contents of the @ref APP_CONFIG_ENUM
"Application Configuration bytes". The values are displayed in decimal!
The variable value (in uppercase hex) is used to specify the offset of the configuration byte to display.
All of these tags are only parsed if the current user is logged in!

Examples:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example Tag</i></td><td align=center><i>Description</i></td></tr>

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



/*******************************************************
 ***** HTTP Commands - Used for CGI Server         *****
 *******************************************************/
/** @addtogroup mod_tcpip_httpsrvr_cgi
@section mod_tcpip_httpsrvr_cgi_cmd Defined Commands
The following list of commands are implemented by the @appname in the "httpexec.c" file.
This file can be modified by the user to add aditional commands:<br>
- Command group @b a to @b j - @ref mod_tcpip_httpsrvr_cgi_cmd_ios
- Command group @b k - @ref mod_tcpip_httpsrvr_cgi_cmd_config
- Command group @b l - @ref mod_tcpip_httpsrvr_cgi_cmd_general
- Command group @b m - @ref mod_tcpip_httpsrvr_cgi_cmd_short
*/

/////////////////////////////////////////////////
// HTTP Command Group - Ports
/** @addtogroup mod_tcpip_httpsrvr_cgi
<hr>
@subsection mod_tcpip_httpsrvr_cgi_cmd_ios PIC Port Commands
A PIC Port command has the following syntax: <code>"pxx=v"</code>
- The 'p' part is the <i>Command Group</i>, and represents the PIC port. It has a value from @b a to @b g.
- The 'xx' part is the <i>Command code</i>, and specifies the command to execute on the given port.
- The 'v' part is the command operand, and is command specific. It usually is '1' or '0'.

The following PIC port commands are implemented by the @appname:
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td nowrap align=center><i>Command Syntax</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>p=YY</code></td><td><b>Port Command</b> - set the given PIC port to the given hex value. The given hex value must be
in uppercase! For example:
    <br>"<code>a=8A</code>" will set PIC port A to 0x8A.
</td></tr>
<tr><td><code>p[0-7]=[1,0]</code></td><td><b>Port Pin Command</b> - set or clear the given PIC port pin. For example:
    <br>"<code>a0=0</code>" will clear PIC port A0 (Port A, pin 0)
    <br>"<code>f7=1</code>" will set PIC port F7 (Port F, pin 7)
</td></tr>
<tr><td><code>px[0-7]=[1,0]</code></td><td><b>Inverted Port Pin Command</b> - Set or clear the given PIC port pin. The value is inverted! For example:
    <br>"<code>ax0=0</code>" will set PIC port A0 (Port A, pin 0)
    <br>"<code>f7=1</code>" will clear PIC port F7 (Port F, pin 7)
</td></tr>
</table>
@endhtmlonly

Examples:<br>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example GET command</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>f=01</code></td><td>This example will set PIC port F to 0x01</td></tr>
<tr><td><code>c5=1</code></td><td>This example will set PIC port C5</td></tr>
<tr><td><code>bx0=1</code></td><td>This example will clear PIC port B0</td></tr>
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
// HTTP Command Group - Config
/** @addtogroup mod_tcpip_httpsrvr_cgi
<hr>
@subsection mod_tcpip_httpsrvr_cgi_cmd_config Config Commands
The Config Command is used to modifying the @ref APP_CONFIG_ENUM "Application Configuration bytes".
To see what can be configured with this command, please have a look at the list of
@ref APP_CONFIG_ENUM "Application Configuration bytes". 

The Config command has the following syntax: <code>"kxx=v"</code>
- The 'k' part is the <i>Command Group</i>, and indicates this is a <i>Config Command</i>.
- The 'xx' part is the <i>Command Code</i>, and specifies the offset (in hex) of the @ref APP_CONFIG_ENUM "Application Configuration bytes".
- The 'v' part is the new value (in decimal) that has to be assigned to the given configuration byte. It is a decimal value.

All Config commands can <b>only</b> be executed by a logged in user!

Examples:<br>
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%"><td align=center nowrap ><i>Example GET command</i></td><td align=center><i>Description</i></td></tr>
<tr><td><code>k00=100</code></td><td>This example will set the the "Application Configuration byte" at 0x00 (MSB of IP address) to 100</td></tr>
<tr><td><code>k2A=16</code></td><td>This example will set the "Application Configuration byte" at 0x2A (Port F direction) to 16 (0x0f).
Address 0x2A contains the TRIS register for PIC Port F. Setting a bit to 0 will configure the port as
an output, and 1 as an input. In this example we set TRISF to 0x0f = binary 00001111. This will configure port F
pins 0-3 as inputs, and pins 4-7 as outputs.</td></tr>
<tr><td><code>k2F=16</code></td><td>This example will set the "Application Configuration byte" at 0x2F (Port F default value) to 10 (0x0a).
Address 0x2F contains the default value for PIC Port F at power up. In this example we set it to 0x0a = binary 00001010. This will
set the default value of port F pins 1 and 3 to 1 (5V), and pins 0, 2, 4, 5, 6, 7 to 0 (0V) at power up.</td></tr>
</table>
@endhtmlonly
An example of sending a GET command to a target board with an IP address of "10.1.0.1" is:<br>
http://10.1.0.1/?k1A=22
<br>This command can be sent by entering the line above in a standard web browser, or could be
send by custom software.
*/
// The HTTP Command values for this group is the new value (in decimal) for the config byte
#define CMDGROUP_CONFIG ('k')

/////////////////////////////////////////////////
// HTTP Command Group - General commands. 
/** @addtogroup mod_tcpip_httpsrvr_cgi
<hr>
@subsection mod_tcpip_httpsrvr_cgi_cmd_general General Commands
The General command has the following syntax: <code>"lx=v"</code>
- The 'l' part is the <i>Command Group</i>, and indicates this is a <i>General Command</i>.
- The 'x' part is the <i>Command Code</i>, and specifies the command that is to be executed.
- The 'v' part is the command operand, and is command specific.

The following General commands are implemented by the @appname. If the <i>Logged In</i> column contains a 'X'
character then this command can only be executed by a logged in user!
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Logged<br>In</i></td>
    <td align=center><i>Description</i></td>
</tr>
<tr><td><code>la=str</code></td><td align=center>X</td>
    <td><b>New Username</b> - assigns a new Username. The given "str" is the new username, and must be an alpha numeric
        string of 1 to 8 characters. For example:
        <br>"<code>la=wombat</code>" will set the new username to "wombat".</td>
</tr>
<tr><td><code>lb=str</code></td><td align=center>X</td>
    <td><b>New Password</b> - assigns a new Password. The given "str" is the new password, and must be an alpha numeric
        string of 1 to 8 characters. For example:
        <br>"<code>lb=gumtree</code>" will set the new username to "gumtree".</td>
</tr>
<tr><td><code>lu=str</code></td><td align=center>&nbsp;</td>
    <td><b>Username</b> - gives the username of an user attempting to log in. The given "str" is the username, and must be an alpha numeric
        string of 1 to 8 characters. This command must be followed by the Password command. For example:
        <br>"<code>lu=admin</code>" will send a request to log in for the "admin" user.</td>
</tr>
<tr><td><code>lp=str</code></td><td align=center>&nbsp;</td>
    <td><b>Password</b> - gives the password of an user attempting to log in. The given "str" is the password, and must be an alpha numeric
        string of 1 to 8 characters. This command must followed the Username command. For example:
        <br>"<code>lp=pw</code>" will send a request to log with the "pw" password.</td>
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
#define CMDCODE_GEN_NEW_USERNAME ('a')  //New username
#define CMDCODE_GEN_NEW_PASSWORD ('b')  //New password
#define CMDCODE_GEN_USERNAME ('u')      //The entered username
#define CMDCODE_GEN_PASSWORD ('p')      //The entered password

/////////////////////////////////////////////////
// HTTP Command Group - Short Commands.
/** @addtogroup mod_tcpip_httpsrvr_cgi
<hr>
@subsection mod_tcpip_httpsrvr_cgi_cmd_short Short Commands
Short commands do not have any command operands. They always have the following syntax: <code>"m=v"</code>
- The 'm' part is the <i>Command Group</i>, and indicates this is a <i>Short Command</i>.
- The 'v' part is the <i>Command Code</i>, and specifies the command that is to be executed.

The following Short commands are implemented by the @appname. If the <i>Logged In</i> column contains a 'X'
character then this command can only be executed by a logged in user!
@htmlonly
<table class=compactTable cellspacing=0 cellpadding=2 border=1>
<tr width="100%">
    <td nowrap align=center><i>Command Syntax</i></td>
    <td align=center><i>Logged<br>In</i></td>
    <td align=center><i>Description</i></td>
</tr>
<tr><td><code>m=r</code></td><td align=center>X</td>
    <td><b>Reset Board</b> - reset's the board. This command can be used to remotely reset the board. This can
    be used to update the firmware via the network bootloader for example.</td>
</tr>
<tr><td><code>m=o</code></td><td align=center>X</td>
    <td><b>Log Out</b> - log out the current user.</td>
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



/**
 * Initializes global variables.
 */
void htpexecInit(void);

#endif
