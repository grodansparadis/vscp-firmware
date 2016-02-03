<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script type="text/javascript">
function doSubmit() {
	for (i=1;i<3;i++) {
		if (document.getElementById("rem").checked == true) {
			document.getElementsByName("w"+i)[0].name = "w"+i+"r";
		}
	}
}
function getDisabled(ch) {
	var e=0x%wF8;
	return (e&(1<<(ch-1))) ? "" : "disabled";
}
function pv(ch,p,v,name) {
	document.write("<tr><td class=bLbl>Channel "+ch+" (Port "+p+"):</td>");
	document.write("<td class=bCtr width=100%%><input " + getDisabled(ch) + " type=text size=10 maxlength=5 value="+v+" name=\""+name+"\"></td>");
	document.write("</tr>");
}
</script>
</head>

<body class=ifrmBody>
<form method=GET action=PWMVAL.CGI onsubmit="doSubmit();">
<br>
<table class=bBox cellpadding=3 cellspacing=1>
<tr><td class=bHdr colspan=2>PWM Values</td></tr>
<tr><td class=bDesc colspan=2>This section is used to monitor and set the current PWM channels. Before using a PWM channel, it has to
be configured as a PWM channel on the <a href="xpcfg.cgi">PWM Channel Settings</a> page.</td></tr>
<tr><td class=bSec colspan=2>PWM Channel Values
<script type="text/javascript">
	document.write( ('%wF4'=='8') ? "(0 - 255)" : "(0 - 1023)");
</script>
</td></tr>
<tr><td class=bDesc colspan=2>Set PWM Channel to given value</td></tr>
<script type="text/javascript">
pv(1, "C1", %w11, "w1");
pv(2, "C2", %w12, "w2");
</script>
<tr><td class=bBot colspan=2><input type=submit value="Update">&nbsp;  &nbsp; Remember Settings After Power Up <input type=checkbox id=rem></td></tr>
</table>

<hr>
For a list of all PWM commands, <a href="cmd.htm#pwm">click here</a>.

</form>
</body>
</html>
