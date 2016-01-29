<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script type="text/javascript" src="68ec303.js"></script>
<script type="text/javascript">
function lnb() {
	document.write("<td>&nbsp;</td>");
}
function ln(io,val,name) {
	document.write("<td align=center>");
	if(io==0) {
		document.write("<input class=pinbtn type=submit value="+val+" name=\""+name+"\">");
	}
	else {
		document.write("<span class=pinlbl>"+val+"</span>");
	}
	document.write("</td>");
}
</script>
</head>

<body onLoad="ifrmOl()">
<p class="pgHdr">PIC Digital Pin Values</p>
<hr>
<br>
<b>Port Values:</b><br>
This section is used to monitor and set the digital port pins of the CPU. 
<ul><li>Output pins are represented by a button with a 1 or 0 on it. When pressed, the output will be toggled.</li>
<li>Input Pins can only be monitored, and are represented by a 1 or 0 character.</li></ul>
<form method=GET action=IOVAL.CGI>

<table class="tbrd" cellpadding="3" cellspacing="0">
<tr bgcolor="#CCCCCC" style="font-weight:bold;">
<td width=100>Port Pin</td>
<td width=40 align=center>7</td>
<td width=40 align=center>6</td>
<td width=40 align=center>5</td>
<td width=40 align=center>4</td>
<td width=40 align=center>3</td>
<td width=40 align=center>2</td>
<td width=40 align=center>1</td>
<td width=40 align=center>0</td>

<tr bgcolor="#FFFFFF">
<td>Port A:</td>
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

<tr bgcolor="#EEEEEE">
<td>Port B:</td>
<script type="text/javascript">
ln(%b57,%b07,"bx7");
ln(%b56,%b06,"bx6");
ln(%b55,%b05,"bx5");
ln(%b54,%b04,"bx4");
ln(%b53,%b03,"bx3");
ln(%b52,%b02,"bx2");
ln(%b51,%b01,"bx1");
ln(%b50,%b00,"bx0");
</script>
</tr>

<tr bgcolor="#FFFFFF">
<td>Port C:</td>
<script type="text/javascript">
lnb();
lnb();
ln(%c55,%c05,"cx5");
lnb();
lnb();
ln(%c52,%c02,"cx2");
ln(%c51,%c01,"cx1");
ln(%c50,%c00,"cx0");
</script>
</tr>

<tr bgcolor="#EEEEEE">
<td>Port F:</td>
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

<tr bgcolor="#FFFFFF">
<td>Port G:</td>
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
</tr></table>

<table><tr><td width="20"></td><td><input type=submit value="Update"></td></table>

</form>

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
