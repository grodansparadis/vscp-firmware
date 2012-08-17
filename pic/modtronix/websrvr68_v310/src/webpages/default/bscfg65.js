function bscfg(baud1, ser1Cfg, baud2, ser2Cfg) {
	document.write(" \
	<form method=GET action=\"XBSCFG.CGI\" onsubmit=\"bscfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>USART 1 Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure USART1, which is used for the RS232 port.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Enable USART 1</td> \
			<td class=bCtr><input type=checkbox id=ser1_0 "+((ser1Cfg&0x01)?"checked":"")+"> &nbsp; \
			<i>If checked, USART 1 is enabled.</i></td> \
		</tr> \
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
		<tr><td class=bBot colspan=2>&nbsp;</td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k20> \
	<input type=hidden name=k22> \
	</form>");

	/* Select correct options, x because option value is a hex number */
	selOpt(document.getElementById("selectBaud"), 'x', baud1);
}

function bscfgSubmit() {
	document.getElementsByName("k20")[0].value = getCbxByte("ser1_",0);
}
