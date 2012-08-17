<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<title>Modtronix SBC65EC Demo Webserver</title>
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">PIC PWM Value on PC2</p>
<hr>
<br>
<b>Port Values:</b><br>
This section is used to set the PWM value for PC2. The value can be set to a number in the interval 0-1023.

<form method=GET action=PWMVAL.CGI>
<p>PWM value: <input type="text" name="P" size="7" value="%P00"></p>
<input type=submit value="Update">
</form>

<hr>
The HTML document will have a %%Pxy tag where it wants to display a PWM value.
 The 'P' part represents the stands for PWM.
 The 'x' part represents the format of the displayed PWM value and  the 'y' part the PIN number (0). Valid formats are:
 <ul><li>0 - Outputs the current PWM value.</li>
 </ul>For example:<br>
- To display PWM value for RC2, place <b>%%P00</b> on your page.<br>

</body>
</html>
