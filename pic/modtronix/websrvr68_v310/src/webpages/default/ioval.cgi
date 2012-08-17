<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script type="text/javascript">
function lnb() {
	document.write("<td class=bCel>&nbsp;</td>");
}
function ln(io,val,name) {
	document.write("<td class=bCel align=center style=\"padding:1px;\">");
	if(io==0) {
		document.write("<input style=\"width:38px;\" type=submit value="+val+" name=\""+name+"\">");
	}
	else {
		document.write(val);
	}
	document.write("</td>");
}
function tdh(s) {
	document.write("<td class=bCel align=center><b><div style=\"width:30px;\">"+s+"</div></b></td>");
}
</script>
</head>

<bodyclass=ifrmBody>

<form method=GET action=IOVAL.CGI>
<table class=bBox cellpadding=3 cellspacing=1>

<tr><td class=bHdr colspan=10>Digital Inputs and Outputs</td></tr>

<tr><td class=bDesc colspan=10>This section is used to monitor and control the Digital Input and Output Ports.
<ul><li>Outputs are represented by a button with a 1 or 0 on it. When pressed, the output will be toggled.</li>
<li>Inputs can only be monitored, and are represented by a 1 or 0 character.</li></ul>
A port can be configured to be an input or output on the <a href="xiocfg.cgi">Port Settings</a> page.
</td></tr>

<tr><td class=bSec colspan=10>Port Values</td></tr>
							
<tr>
	<td class=bLbl><b>Port Pin</b></td>
	<script type="text/javascript">
	tdh("7");
	tdh("6");
	tdh("5");
	tdh("4");
	tdh("3");
	tdh("2");
	tdh("1");
	tdh("0");
	</script>
	<td class=bCtr rowspan=6 width=100%%></td>
</tr>

<tr>
	<td class=bLbl>Port A</td>
	<script type="text/javascript">
	lnb();
	lnb();
	ln(%a55,%a05,"ax5");
	ln(%a54,%a04,"ax4");
	ln(%a53,%a03,"ax3");
	ln(%a52,%a02,"ax2");
	ln(%a51,%a01,"ax1");
	ln(%a50,%a00,"ax0");
	</script>
</tr>

<tr>
	<td class=bLbl>Port B</td>
	<script type="text/javascript">
	xbd=%k87;
	if(xbd!=2) /*Not IOR5E*/ ln(%b57,%b07,"bx7"); else lnb();
	if(%k15&0x02) {lnb()}
	else {ln(%b56,%b06,"bx6")}
	if(xbd!=2) ln(%b55,%b05,"bx5"); else lnb();
	if(xbd!=2) ln(%b54,%b04,"bx4"); else lnb();
	if(xbd!=2) ln(%b53,%b03,"bx3"); else lnb();
	if(xbd!=2) ln(%b52,%b02,"bx2"); else lnb();
	if(xbd!=2) ln(%b51,%b01,"bx1"); else lnb();
	if(xbd!=2) ln(%b50,%b00,"bx0"); else lnb();
	</script>
</tr>

<tr>
	<td class=bLbl>Port C</td>
	<script type="text/javascript">
	xbd=%k87;
	lnb();
	lnb();
	if(xbd!=2) /*Not IOR5E*/ln(%c55,%c05,"cx5"); else lnb();
	lnb();
	lnb();
	if(xbd!=2) ln(%c52,%c02,"cx2"); else lnb();
	if(xbd!=2) ln(%c51,%c01,"cx1"); else lnb();
	if(xbd!=2) ln(%c50,%c00,"cx0"); else lnb();
	</script>
</tr>

<tr>
	<td class=bLbl>Port F</td>
	<script type="text/javascript">
	ln(%f57,%f07,"fx7");
	ln(%f56,%f06,"fx6");
	ln(%f55,%f05,"fx5");
	ln(%f54,%f04,"fx4");
	ln(%f53,%f03,"fx3");
	ln(%f52,%f02,"fx2");
	ln(%f51,%f01,"fx1");
	ln(%f50,%f00,"fx0");
	</script>
</tr>

<tr>
	<td class=bLbl>Port G</td>
	<script type="text/javascript">
	lnb();
	lnb();
	lnb();
	lnb();
	ln(%g53,%g03,"gx3");
	ln(%g52,%g02,"gx2");
	ln(%g51,%g01,"gx1");
	ln(%g50,%g00,"gx0");
	</script>
</tr>

<tr><td class=bBot colspan=10><input type=submit value="Update"></td></tr>
</table>
</form>

<hr>
For a list of all port commands, <a href="cmd.htm#port">click here</a>.
<hr>
The HTML document will have a %%pxy tag where it wants to display a Digital PIN's state.
 The 'p' part represents the port name, and can be <b>a</b>, <b>b</b>, <b>c</b> or <b>f</b>.
 The 'x' part represents the format of the displayed PIN and  the 'y' part the PIN number (0 to 7). Valid formats are:
 <ul><li>0 - Outputs <b>1</b> or <b>0</b>, representing the PIN's state.</li>
<li>1 - Outputs <b>on</b> or <b>off</b>, representing the PIN's state.</li>
<li>2 - Outputs <b>&lt;!--</b> if PIN is configured as an input.</li>
<li>2 - Outputs <b>--&gt;</b> if PIN is configured as an input (Add 0x08 to y part - pin number).</li>
<li>3 - Outputs <b>&lt;!--</b> if PIN is configured as an output.</li>
<li>3 - Outputs <b>--&gt;</b> if PIN is configured as an output(Add 0x08 to y part - pin number).</li>
</ul>For example:<br>
- To display 1 or 0 to represent RB5, place <b>%%b05</b> on your page.<br>
- To display on or off to represent RF7, place <b>%%f17</b> on your page.<br>
- To print <b>--&gt;</b> if RA4 is an input, place <b>%%a2C</b> on your page (note the y part is the pin number + 8 in hex = 4 + 8 = C).<br>

</body>
</html>
