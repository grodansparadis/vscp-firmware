<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">PIC Digital Pin Values</p>
<hr>
<br>
<b>Port Values:</b><br>
This section is used to monitor and set the digital port pins of the CPU. Port G is currently not available via this
 page, but will be added in the next version.
<ul><li>Output pins are represented by a button with a 1 or 0 on it. When pressed, the output will be toggled.</li>
<li>Input Pins can only be monitored, and are represented by a 1 or 0 character.</li></ul>
<form method=GET action=IOVAL.CGI>

<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold;">
<td width="100">Port Pin</td>
%I017</td>
%I016</td>
%I015</td>
%I014</td>
%I013</td>
%I012</td>
%I011</td>
%I010</td>

<tr bgcolor="#FFFFFF">
<td>Port A:</td>
<td>&nbsp;</td>
<td>&nbsp;</td>
%I02%a25%I03"%a05" name="A5">%a2D%a35%I04%a05</span>%a3D</td>
%I02%a24%I03"%a04" name="A4">%a2C%a34%I04%a04</span>%a3C</td>
%I02%a23%I03"%a03" name="A3">%a2B%a33%I04%a03</span>%a3B</td>
%I02%a22%I03"%a02" name="A2">%a2A%a32%I04%a02</span>%a3A</td>
%I02%a21%I03"%a01" name="A1">%a29%a31%I04%a01</span>%a39</td>
%I02%a20%I03"%a00" name="A0">%a28%a30%I04%a00</span>%a38</td>
</tr>

<tr bgcolor="#EEEEEE">
<td>Port B:</td>
%I02%b27%I03"%b07" name="B7">%b2F%b37%I04%b07</span>%b3F</td>
%I02%b26%I03"%b06" name="B6">%b2E%b36%I04%b06</span>%b3E</td>
%I02%b25%I03"%b05" name="B5">%b2D%b35%I04%b05</span>%b3D</td>
%I02%b24%I03"%b04" name="B4">%b2C%b34%I04%b04</span>%b3C</td>
%I02%b23%I03"%b03" name="B3">%b2B%b33%I04%b03</span>%b3B</td>
%I02%b22%I03"%b02" name="B2">%b2A%b32%I04%b02</span>%b3A</td>
%I02%b21%I03"%b01" name="B1">%b29%b31%I04%b01</span>%b39</td>
<td>&nbsp;</td>
</tr>

<tr bgcolor="#FFFFFF">
<td>Port C:</td>
<td>&nbsp;</td>
<td>&nbsp;</td>
%I02%c25%I03"%c05" name="C5">%c2D%c35%I04%c05</span>%c3D</td>
<td>&nbsp;</td>
<td>&nbsp;</td>
%I02%c22%I03"%c02" name="C2">%c2A%c32%I04%c02</span>%c3A</td>
%I02%c21%I03"%c01" name="C1">%c29%c31%I04%c01</span>%c39</td>
%I02%c20%I03"%c00" name="C0">%c28%c30%I04%c00</span>%c38</td>
</tr>

<tr bgcolor="#EEEEEE">
<td>Port F:</td>
%I02%f27%I03"%f07" name="F7">%f2F%f37%I04%f07</span>%f3F</td>
%I02%f26%I03"%f06" name="F6">%f2E%f36%I04%f06</span>%f3E</td>
%I02%f25%I03"%f05" name="F5">%f2D%f35%I04%f05</span>%f3D</td>
%I02%f24%I03"%f04" name="F4">%f2C%f34%I04%f04</span>%f3C</td>
%I02%f23%I03"%f03" name="F3">%f2B%f33%I04%f03</span>%f3B</td>
%I02%f22%I03"%f02" name="F2">%f2A%f32%I04%f02</span>%f3A</td>
%I02%f21%I03"%f01" name="F1">%f29%f31%I04%f01</span>%f39</td>
%I02%f20%I03"%f00" name="F0">%f28%f30%I04%f00</span>%f38</td>
</tr></table>

<table><tr><td width="20"></td><td><input type=submit value="Update"></td></table>

</form>

<hr>
The HTML document will have a %%pxy tag where it wants to display a Digital PIN's state.
 The 'p' part represents the port name, and can be <b>a</b>, <b>b</b>, <b>c</b> or <b>f</b>.
 The 'x' part represents the format of the displayed PIN and  the 'y' part the PIN number (0 to 7). Valid formats are:
 <ul><li>0 - Outputs <b>1</b> or <b>0</b>, representing the PIN's state.</li>
<li>1 - Outputs <b>on</b> or <b>off</b>, representing the PIN's state.</li>
<li>2 - Outputs <b>&lt;!--</b> if PIN is configured as an input.</li>
<li>2 - Outputs <b>--&gt;</b> if PIN is configured as an input (Add 0x08 to y part - pin number).</li>
<li>3 - Outputs <b>&lt;!--</b> if PIN is configured as an output.</li>
<li>3 - Outputs <b>--&gt;</b> if PIN is configured as an output(Add 0x08 to y part - pin number).</li>
</ul>For example:<br>
- To display 1 or 0 to represent RB5, place <b>%%b05</b> on your page.<br>
- To display on or off to represent RF7, place <b>%%f17</b> on your page.<br>
- To print <b>--&gt;</b> if RA4 is an input, place <b>%%a2C</b> on your page (note the y part is the pin number + 8 in hex = 4 + 8 = C).<br>

</body>
</html>
