<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
</head>

<body class=ifrmBody>
<h3>Examples of how to display PIC Digital Pin Values</h3>
To test the following examples, configure the relevant port as an output or input on the
 <a href="xiocfg.cgi">Port Settings<a> page and set it's value on the
 <a href="ioval.cgi">Port Values<a> page. After doing this and returning to this page,
the examples will reflect the changes made.

<hr>
<h4>1. Example of how to display PIC Digital Pin Values as '1' or '0'</h4>
The following example can be used to display the current value of port pin B1
 as '1' or '0' depending on it's state.
 This is achieve by using the %%b0y tag, where y is replaced by the port number (0-7).
 The dynamic CGI server in the Modtronix TCP/IP Stack will replace this tag
 with '1' or '0' depending on the state of B1.
 See <a href="ioval.cgi">Port Values<a> page for more details on tags.<br>
 The code for this example is:<br>
 <pre>
 Port B1 = %%b01
 </pre>
This code above is displayed as:<br>
Port B1 = %b01

<hr>
<h4>2. Example of how to display PIC Digital Pin Values as 'on' or 'off'</h4>
The following example can be used to display the current value of port pin C2
 as 'on' or 'off' depending on it's state.
 This is achieve by using the %%c1y tag, where y is replaced by the port number (0-7).
 The dynamic CGI server in the Modtronix TCP/IP Stack will replace this tag
 with 'on' or 'off' depending on the state of C2.
 See <a href="ioval.cgi">Port Values<a> page for more details on tags.<br>
 The code for this example is:<br>
 <pre>
 Port C2 = %%c12
 </pre>

This code above is displayed as:<br>
Port C2 = %c12

<hr>
<h4>3. The following example can be used to monitor OR toggle the value of port pin B4.</h4>
 <ul>
 <li>If pin B4 is configured as an input, it will be display as a '0' or '1' character
 depending on it's current state.</li>
 <li>If pin B4 is configured as an output, a button will be shown that allows it's current
 state to be toggled.</li>
 </ul>
 This is achieve by using the %%b2y and %%b3y tags. The dynamic CGI server in the Modtronix
 TCP/IP Stack will replace these tags with opening and closing HTML comment elements
 depending if the port pin given by y is configured as an input or output. See
 <a href="ioval.cgi">Port Values<a> page for more details on tags.<br>
 The code for this example is:<br>
 <pre>
 &lt;form method=GET action=../EXP1.CGI&gt;<br>
 Port B4 =<br>
 %%b24&lt;input class="pinbtn" type="submit" value="%%b04" name="bx4"&gt;%%b2C %%b34 %%b04 %%b3C<br>
 &lt;/form&gt;
 </pre>

This code above is displayed as:<br>
<form method=GET action=../EXP1.CGI>
 Port B4 =
 %b24<input class="pinbtn" type="submit" value="%b04" name="bx4">%b2C %b34 %b04 %b3C
</form>

</b>
</body>
</html>
