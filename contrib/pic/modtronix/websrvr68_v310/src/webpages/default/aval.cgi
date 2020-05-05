<html>
<head>
<meta http-equiv="refresh" content="5">
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
</head>

<body class=ifrmBody>

<form method=GET action=AVAL.CGI>
<table class=bBox cellpadding=3 cellspacing=1>

<tr><td class=bHdr colspan=6>Analog Input Value</td></tr>

<tr><td class=bDesc colspan=6>This section is used to monitor (for 5V reference voltage) the Analog Inputs. Before using the Analog Inputs, they have to
be configured on the <a href="xacfg.cgi">Analog Input Settings</a> page.</td></tr>

<tr><td class=bSec colspan=6>Analog Input Values</td></tr>

<tr><td class=bLbl><b>ADC Port</b></td><td class=bCtr nowrap><b>ADC Value</b></td> <td class=bCtr rowspan=7></td> <td class=bLbl><b>ADC Port</b></td><td class=bCtr nowrap><b>ADC Value</b></td> <td class=bCtr width="100%%" rowspan=7></td> </tr>
<tr><td class=bLbl>A0</td><td class=bCtr>%n20V</td> <td class=bLbl>F1</td><td class=bCtr>%n26V</td> </tr>
<tr><td class=bLbl>A1</td><td class=bCtr>%n21V</td> <td class=bLbl>F2</td><td class=bCtr>%n27V</td> </tr>
<tr><td class=bLbl>A2</td><td class=bCtr>%n22V</td> <td class=bLbl>F3</td><td class=bCtr>%n28V</td> </tr>
<tr><td class=bLbl>A3</td><td class=bCtr>%n23V</td> <td class=bLbl>F4</td><td class=bCtr>%n29V</td> </tr>
<tr><td class=bLbl>A5</td><td class=bCtr>%n24V</td> <td class=bLbl>F5</td><td class=bCtr>%n2AV</td> </tr>
<tr><td class=bLbl>F0</td><td class=bCtr>%n25V</td> <td class=bLbl>F6</td><td class=bCtr>%n2BV</td> </tr>

<tr><td class=bBot colspan=6><input type=submit value="Update"></td></tr>

</table>
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