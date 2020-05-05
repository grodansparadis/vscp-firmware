<html>
<head>
<meta http-equiv="refresh" content="5">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="../mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">Scaled Analog Input Values</p>
<hr>
<br><b>Analog Input Values:</b><br>
Following is an example of how Scaled ADC values can be displayed on a web page.<br><br>
<table><tr><td>
<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold;"><td width="100">ADC Port</td><td>Value</td></tr>
<tr><td>A0:</td><td>%S00</td></tr>
<tr><td>A1:</td><td>%S01</td></tr>
<tr><td>A2:</td><td>%S02</td></tr>
<tr><td>A3:</td><td>%S03</td></tr>
<tr><td>A5:</td><td>%S04</td></tr>
<tr><td>F0:</td><td>%S05</td></tr>
<tr><td>F1:</td><td>%S06</td></tr>
</table>

 

<form method=GET action=SVAL.CGI>
<table><tr><td width="20"></td><td><input type=submit value="Update"></td></table>
</form>

<hr>
The HTML document will have a %%Sxy tag where it wants to display a ADC channel value. The 'x' part represents the
 format of the displayed channel, and the 'y' part the channel. Valid formats are:
<ul><li>0 - Outputs the channels value as a one decimal floating point value, for example <b>13.2</b></li>
</ul>For example, to display the scaled value of channel 1, place <b>%%S01</b> on your page.  
</body>
</html>

