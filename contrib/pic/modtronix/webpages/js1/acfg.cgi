<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
</head>

<body marginwidth="0" marginheight="0" topmargin="0" bottommargin="0" leftmargin="0" rightmargin="0">

<h2>PIC Analog Input Settings</h2>
<hr>
<br><b>Analog Input Channels:</b><br>
This section is used to configure the Analog to Digital converter.
<form method=GET action=../ACFG.CGI>
<table>
<tr><td>Port Configuration:</td>
<td><select name="A">
<option value="O" %A0F>ADC Off</option>
<option value="N" %A0E>A0</option>
<option value="M" %A0D>A0, A1</option>
<option value="L" %A0C>A0, A1, A2</option>
<option value="K" %A0B>A0, A1, A2, A3</option>
<option value="J" %A0A>A0, A1, A2, A3, A5</option>
<option value="I" %A09>A0, A1, A2, A3, A5, F0</option>
<option value="H" %A08>A0, A1, A2, A3, A5, F0, F1</option>
<option value="G" %A07>A0, A1, A2, A3, A5, F0, F1, F2</option>
<option value="F" %A06>A0, A1, A2, A3, A5, F0, F1, F2, F3</option>
<option value="E" %A05>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4</option>
<option value="D" %A04>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5</option>
<option value="C" %A03>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5, F6</option>
</select> </td></tr>
<tr><td>Voltage Reference Configuration:&nbsp;</td>
<td><select name="B">
<option value="0" %A10>Vref+ = AVdd, Vref- = AVss</option>
<option value="1" %A11>Vref+ = Ext, &nbsp;Vref- = AVss</option>
<option value="2" %A12>Vref+ = AVdd, Vref- = Ext</option>
<option value="3" %A13>Vref+ = Ext,  Vref- = Ext</option>
</select> </td></tr>
<tr><td>Acquisition Time:</td>
<td><select name="C">
<option value="3" %A23>9.6uS</option>
<option value="4" %A24>12.8uS</option>
<option value="5" %A25>19.2uS</option>
<option value="6" %A26>25.6uS</option>
<option value="7" %A27>32uS</option>
</select> </td></tr>
</table>
<br><hr>

<table>
<tr><td width="20"></td><td><input type=submit value="Submit"></td>
</table>

</form>
</body>
</html>
