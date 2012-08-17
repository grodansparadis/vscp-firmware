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
 #define DEFAULT_CMD_UDPPORT (54123)

 //Default "UDP Command Responce Port"
 #define DEFAULT_CMDRESP_UDPPORT (54124)

 //"UDP Command Port"
 #define CMD_UDPPORT     DEFAULT_CMD_UDPPORT

 //"UDP Command Responce Port"
 #define CMDRESP_UDPPORT DEFAULT_CMDRESP_UDPPORT

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

#ifndef CMD_H
#define CMD_H


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

@section mod_tags_udp Tags via UDP command
<b>This feature has not been implemented yet! Will be implemented in a future version! </b>
Any of the Tags listed below can be executed on the target by sending them
to UDP port 54123. This port is configurable, and can be changed. The reply (requested data) will be
returned to the UDP Socket that sent this Request Command.

@section mod_tags_httpget Tags via HTTP GET method
<b>This feature has not been implemented yet! Will be implemented in a future version! </b>
Any of the Tags listed below can be executed on the target by sending them via the HTTP GET method.
The reply (requested data) will be returned to the UDP Request Command Port (54124 by default).
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
// HTTP Command Group - Config
/** @addtogroup mod_cmd
<hr>
@subsection mod_cmd_ref_config Config Commands
The Config Command is used to modifying the @ref APP_CONFIG_ENUM "Application Configuration bytes".
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

/************************************************
 ***** Command Update flags                 *****
 ************************************************/
#define CMD_UD_UNDEFINED    0x0001      /* Update a valid, but undefined settings */ 
#define CMD_UD_NETWORK      0x0002      /* Update the network settings */
#define CMD_UD_USART        0x0004      /* Update the USART settings */
#define CMD_UD_CPU_IO       0x0008      /* Update the CPU IO settings */
#define CMD_UD_ADC          0x0010      /* Update the ADC settings */
#define CMD_UD_TFTP         0x0020      /* Update the TFTP settings */
#define CMD_UD_SMTP         0x0040      /* Update the SMTP settings */
#define CMD_UD_SYSTEM       0x0080      /* Update the System settings */
#define CMD_UD_PWM          0x0100      /* Update the PWM settings */
#define CMD_UD_WEB          0x0200      /* Update the Web Server settings */


/**
 * Initializes "UDP Command Port" and "UDP Command Responce Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void cmdUdpInit(void);

/**
 * Must be called every couple of ms
 *
 */
void cmdProcess(void);

/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command
 * @param value The value part of the name-value command
 * @param user The user level, is a HTTP_USER_XX constant
 *
 * @return Returns an UD_XXX flag (or multiple ones ORed together) indicating what has been updated. Not
 * all name-value commands will return a value here.
 */
WORD execNameValueCmd(BYTE * name, BYTE * value, BYTE user);

/**
 * Structure for passing variables to getVarCmd() function
 */
typedef struct _CMD_GET_VAR
{
    BYTE value;
    BYTE group;

    /**
     * Input and output variable.
     * - Will contain HTTP_START_OF_VAR for fist call
     * - If there are no more values left for this variable, it will contain HTTP_END_OF_VAR on return
     * - If there are still bytes to send, it will contain a value other than HTTP_START_OF_VAR and
     *   HTTP_END_OF_VAR reference on return
     */
    BYTE ref;

    BYTE* buf;
    BYTE bufLength;
    BYTE user;

    struct
    {
        unsigned char bFinished : 1;    //If set, getVarCmd() returned all requested data
    } flags;
} CMD_GET_VAR;

/**
 * Get the requested variable command. The command might not be executed if a higher user level is
 * required.
 *
 * @param pGetVar Pointer to CMD_GET_VAR structure.
 *
 * @return Returns number of bytes read. If this value is less then the size of the buffer
 *         given, then all requested data has been returned (flags.bFinished flag will also be set)
 */
WORD getVarCmd(CMD_GET_VAR * pGetVar);

#endif
