<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="../mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">PIC Analog Input Settings</p>
<hr>
<br><b>Analog Input Channels:</b><br>
This section is used to configure the Analog to Digital converter.
<form method=GET action=../ACFG.CGI>
<table>
<tr><td>Port Configuration:</td>
<td><select name="A">
%AF0"O" %A0F>ADC Off%AF1
%AF0"N" %A0E>A0%AF1
%AF0"M" %A0D>A0, A1%AF1
%AF0"L" %A0C>A0, A1, A2%AF1
%AF0"K" %A0B>A0, A1, A2, A3%AF1
%AF0"J" %A0A>A0, A1, A2, A3, A5%AF1
%AF0"I" %A09>A0, A1, A2, A3, A5, F0%AF1
%AF0"H" %A08>A0, A1, A2, A3, A5, F0, F1%AF1
%AF0"G" %A07>A0, A1, A2, A3, A5, F0, F1, F2%AF1
%AF0"F" %A06>A0, A1, A2, A3, A5, F0, F1, F2, F3%AF1
%AF0"E" %A05>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4%AF1
%AF0"D" %A04>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5%AF1
%AF0"C" %A03>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5, F6%AF1
</select> </td></tr>
<tr><td>Voltage Reference Configuration:&nbsp;</td>
<td><select name="B">
%AF0"0" %A10>Vref+ = AVdd, Vref- = AVss%AF1
%AF0"1" %A11>Vref+ = Ext, &nbsp;Vref- = AVss%AF1
%AF0"2" %A12>Vref+ = AVdd, Vref- = Ext%AF1
%AF0"3" %A13>Vref+ = Ext,  Vref- = Ext%AF1
</select> </td></tr>
<tr><td>Acquisition Time:</td>
<td><select name="C">
%AF0"3" %A23>9.6uS%AF1
%AF0"4" %A24>12.8uS%AF1
%AF0"5" %A25>19.2uS%AF1
%AF0"6" %A26>25.6uS%AF1
%AF0"7" %A27>32uS%AF1
</select> </td></tr>
</table>
<br><hr>
<table>
<tr><td width="20"></td><td><input type=submit value="Submit"></td>
</table>
</form>
</body>
</html>
