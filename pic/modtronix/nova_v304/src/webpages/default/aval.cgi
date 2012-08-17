<html>
<head>
<meta http-equiv="refresh" content="5">
<link href="mx.css" rel="stylesheet" type="text/css">
<script type="text/javascript" src="68ec303.js"></script>
</head>

<body onLoad="ifrmOl()">
<p class="pgHdr">PIC Analog Input Values</p>
<hr>
<br><b>Analog Input Values:</b><br>
Following is an example of how ADC values (for 5V reference voltage) can be displayed on a web page.<br><br>
<table><tr><td>
<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold;"><td width="100">ADC Port</td><td>Value</td></tr>
<tr><td>A0:</td><td>%n20V</td></tr>
<tr><td>A1:</td><td>%n21V</td></tr>
<tr><td>A2:</td><td>%n22V</td></tr>
<tr><td>A3:</td><td>%n23V</td></tr>
<tr><td>A5:</td><td>%n24V</td></tr>
<tr><td>F0:</td><td>%n25V</td></tr>
</table>
</td><td>&nbsp;</td><td>
<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold;"><td width="100">ADC Port</td><td>Value</td></tr>
<tr><td>F1:</td><td>%n26V</td></tr>
<tr><td>F2:</td><td>%n27V</td></tr>
<tr><td>F3:</td><td>%n28V</td></tr>
<tr><td>F4:</td><td>%n29V</td></tr>
<tr><td>F5:</td><td>%n2AV</td></tr>
<tr><td>F6:</td><td>%n2BV</td></tr>
</table>
</td></tr></table>

<form method=GET action=AVAL.CGI>
<table><tr><td width="20"></td><td><input type=submit value="Update"></td></tr></table>
</form>

<hr>
The HTML document will have a %%nxy tag where it wants to display a ADC channel value. The 'x' part represents the
 format of the displayed channel, and the 'y' part the channel. Valid formats are:
<ul><li>0 - Outputs the channels value in hex, for example <b>FA</b></li>
<li>1 - Outputs the channels value in decimal, for example <b>192</b></li>
<li>2 - Outputs the channels value (2 decimal points) for a 5V voltage reference, for example <b>4.02</b></li>
<li>3 - Outputs <b>&lt;!--</b> if the channels is configured as an ADC channel.</li>
<li>4 - Outputs <b>--&gt;</b> if the channels is configured as an ADC channel.</li>
<li>5 - Outputs <b>&lt;!--</b> if the channels is NOT configured as an ADC channel.</li>
<li>6 - Outputs <b>--&gt;</b> if the channels is NOT configured as an ADC channel.</li>
</ul>For example, to display the hex value of channel AN5, place <b>%%n05</b> on your page.
 To display the 5V scalled value of channel AN11, place <b>%%n2B</b> on your page.
 To display the decimal value (0-255) of channel AN0, place <b>%%n10</b> on your page.
</body>
</html>