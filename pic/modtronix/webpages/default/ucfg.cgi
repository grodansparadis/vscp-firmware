<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">USART 1 Settings</p>
<hr>
<br>
<form method=GET action=UCFG.CGI>
<table>
<tr><td width="150">Baud Rate:</td>
<td><select name="A" size="1">
<option %U00 value="0">300</option>
<option %U01 value="1">1200</option>
<option %U02 value="2">2400</option>
<option %U03 value="3">4800</option>
<option %U04 value="4">9600</option>
<option %U05 value="5">19200</option>
<option %U06 value="6">38400</option>
<option %U07 value="7">57600</option>
<option %U08 value="8">115200</option>
</select> </td></tr>
</table>
<br>
<hr>
<table>
<tr><td width="20"></td><td><input type=submit></td>
</table>

</form>

</body>
</html>
