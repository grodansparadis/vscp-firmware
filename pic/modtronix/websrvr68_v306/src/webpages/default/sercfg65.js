function ucfg(baud1) {
	document.write(" \
	<form method=GET action=\"XUCFG.CGI\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>USART 1 Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure USART1, which is used for the RS232 port.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Baud Rate</td> \
			<td class=bCtr width=\"100%\"><select name=k1F id=selectBaud size=1> \
			<option value=0>300</option> \
			<option value=1>1200</option> \
			<option value=2>2400</option> \
			<option value=3>4800</option> \
			<option value=4>9600</option> \
			<option value=5>19200</option> \
			<option value=6>38400</option> \
			<option value=7>57600</option> \
			<option value=8>115200</option> \
			</select></td> \
		</tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	</form>");

	/* Select current baud rate */
	var x=document.getElementById("selectBaud");
	x.selectedIndex = baud1;
}