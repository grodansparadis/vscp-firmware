<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">Network Settings</p>
<hr>
<br>
<form method=GET action=NCFG.CGI>
<table>
<tr><td width="150">IP Address:</td>
%NF0A value=%N00> . </td>
%NF0B value=%N01> . </td>
%NF0C value=%N02> . </td>
%NF0D value=%N03></td>
</table>
<br>
<table>
<tr><td width="150">Subnet Mask:</td>
%NF0E value=%N04> . </td>
%NF0F value=%N05> . </td>
%NF0G value=%N06> . </td>
%NF0H value=%N07></td>
</table>
<br>
<table>
<tr><td width="150">Gateway Address:</td>
%NF0I value=%N08> . </td>
%NF0J value=%N09> . </td>
%NF0K value=%N0A> . </td>
%NF0L value=%N0B></td>
</table>
<br>
<table>
<tr><td width="150">MAC Address:</td>
%NF0M value=%N10 %NE0> . </td>
%NF0N value=%N11 %NE0> . </td>
%NF0O value=%N12 %NE0> . </td>
%NF0P value=%N13 %NE0> . </td>
%NF0Q value=%N14 %NE0> . </td>
%NF0R value=%N15 %NE0></td>
</table>
<br>
<hr>
<table>
<tr><td width="20"></td><td><input type=submit value="Submit"></td>
</table>

</form>

</body>
</html>
