<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">PIC Port Settings</p>
<hr>
<br><b>Port Direction:</b><br>This section is used to set the direction of
 the spare I/O pins on the CPU. Port G is currently not available via this
 page, but will be added in the next version.<br>
<br><i>Checked = Output pin, Unchecked = Input pin</i>
<form method=GET action=IOCFG.CGI>

<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold">
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
%I00"A5" %a45></td>
%I00"A4" %a44></td>
%I00"A3" %a43></td>
%I00"A2" %a42></td>
%I00"A1" %a41></td>
%I00"A0" %a40></td>
</tr>

<tr bgcolor="#EEEEEE">
<td>Port B:</td>
%I00"B7" %b47></td>
%I00"B6" %b46></td>
%I00"B5" %b45></td>
%I00"B4" %b44></td>
%I00"B3" %b43></td>
%I00"B2" %b42></td>
%I00"B1" %b41></td>
<td>&nbsp;</td>
</tr>

<tr bgcolor="#FFFFFF">
<td>Port C:</td>
<td>&nbsp;</td>
<td>&nbsp;</td>
%I00"C5" %c45></td>
<td>&nbsp;</td>
<td>&nbsp;</td>
%I00"C2" %c42></td>
%I00"C1" %c41></td>
%I00"C0" %c40></td>
</tr>

<tr bgcolor="#EEEEEE">
<td>Port F:</td>
%I00"F7" %f47></td>
%I00"F6" %f46></td>
%I00"F5" %f45></td>
%I00"F4" %f44></td>
%I00"F3" %f43></td>
%I00"F2" %f42></td>
%I00"F1" %f41></td>
%I00"F0" %f40></td>
</tr></table>

<br>
<hr>
<table>
<tr><td width="20"></td><td><input type=submit value="Submit"></td>
</table>

</form>
</body>
</html>
