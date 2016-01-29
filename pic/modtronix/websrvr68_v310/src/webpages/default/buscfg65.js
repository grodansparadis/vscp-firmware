function bufcfg(tx0, rx0, tx1, rx1, tx2, rx2, linkUDP1, linkUDP2) {
	document.write(" \
	<form method=GET action=\"XBUSES.CGI\" onsubmit=\"busesSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Bus Linking</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to link buses. Ensure that the desired buses are enabled on the <i>Setting</i> pages (can be selected via menu on left).</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>UDP Bus 1</td> \
			<td class=bCtr width=\"100%\"><select name=u16 id=selLinkUDP1 size=1> \
			<option value=0>Serial Port1 &nbsp;</option> \
			<option value=2>I2C Bus1</option> \
			<option value=3>SPI Bus1</option> \
			<option value=255>None</option> \
			</select></td> \
		</tr> \
		<tr><td class=bLbl>UDP Bus 2</td> \
			<td class=bCtr width=\"100%\"><select name=u17 id=selLinkUDP2 size=1> \
			<option value=0>Serial Port1 &nbsp;</option> \
			<option value=2>I2C Bus1</option> \
			<option value=3>SPI Bus1</option> \
			<option value=255>None</option> \
			</select></td> \
		</tr> \
		<tr><td class=bBot colspan=2>&nbsp;</td></tr> \
		\
		<tr><td class=bHdr colspan=2>Buffer Sizes</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure the Receive and Transmit buffer sizes of the serial busses. \
			The total size of the buffers must be less than 512 bytes. \
			<br>Current total size: " + (tx0+rx0+tx1+rx1+tx2+rx2) + "</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Serial Port 1 Transmit Buffer</td> \
		<td class=bCtr width=\"100%\"><input type=text size=10 maxlength=3 value=" + tx0 + " name=u00></td></tr> \
		<tr><td class=bLbl>Serial Port 1 Receive Buffer</td> \
		<td class=bCtr width=\"100%\"><input type=text size=10 maxlength=3 value=" + rx0 + " name=u01></td></tr> \
		\
		<tr><td class=bLbl>I2C Bus 1 Transmit Buffer</td> \
		<td class=bCtr width=\"100%\"><input type=text size=10 maxlength=3 value=" + tx2 + " name=u04></td></tr> \
		<tr><td class=bLbl>I2C Bus 1 Receive Buffer</td> \
		<td class=bCtr width=\"100%\"><input type=text size=10 maxlength=3 value=" + rx2 + " name=u05></td></tr> \
		\
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	</form>");

	/* Select correct options, i because option value is a integer number */
	selOpt(document.getElementById("selLinkUDP1"), 'i', linkUDP1);
	selOpt(document.getElementById("selLinkUDP2"), 'i', linkUDP2);
}

function busesSubmit() {
	//document.getElementsByName("k20")[0].value = getCbxByte("ser1_",0);
}
