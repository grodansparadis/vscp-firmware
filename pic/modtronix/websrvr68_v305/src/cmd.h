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
 * 2006-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef CMD_H
#define CMD_H


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
<tr><td><code>p[0-7]r=[1,0]</code></td><td><b>Port Pin Command, Remember</b> -  - Same as "Port Pin Command" command, except that
    the settings are saved to the application configuration, and is remembered after power up.
</td></tr>
<tr><td><code>pc[0-7]=[1,0]</code></td><td><b>Port Pin Configuration</b> - Configure given PIC port pin as input or output. '0' will configure
    pin to be an output, and '1' an input. The port direction configured with this command will NOT be remembered
    after the next power up, use the "Port Pin Configuration, Remember" command to achieve this!  Example to use this command:
    <br>"<code>ac0=0</code>" will configure PIC port A0 (Port A, pin 0) to be an output.
    <br>"<code>fc7=1</code>" will configure PIC port F7 (Port F, pin 7) to be an input
</td></tr>
<tr><td><code>pc[0-7]r=[1,0]</code></td><td><b>Port Pin Configuration, Remember</b> - Same as "Port Pin Configuration" command, except that
    the settings are saved to the application configuration, and is remembered after power up.
</td></tr>
<tr><td><code>px[0-7]=[1,0]</code></td><td><b>Inverted Port Pin Command</b> - Set or clear the given PIC port pin. The value is inverted! For example:
    <br>"<code>ax0=0</code>" will set PIC port A0 (Port A, pin 0)
    <br>"<code>fx7=1</code>" will clear PIC port F7 (Port F, pin 7)
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
<tr><td><code>ln=str</code></td><td align=center>X</td>
    <td><b>New NetBIOS name</b> - assigns a new NetBIOS name. The given "str" is the new name, and must be an alpha numeric
        string of 1 to 15 characters. For example:
        <br>"<code>lb=CROC1</code>" will set the new username to "CROC1".</td>
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
#define CMDCODE_GEN_NEW_USERNAME ('a')      //New username
#define CMDCODE_GEN_NEW_PASSWORD ('b')      //New password
#define CMDCODE_GEN_NEW_NETBIOS_NAME ('n')  //New NetBIOS name
#define CMDCODE_GEN_USERNAME ('u')          //The entered username
#define CMDCODE_GEN_PASSWORD ('p')          //The entered password

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

#endif
