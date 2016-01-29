function bucfg(u1L,u1H,f1,u2L,u2H,f2)
{
	document.write(" \
	<form id=formScfg method=GET action=\"XBUCFG.CGI\" onsubmit=\"bucfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>UDP Port 1 Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure UDP Port 1. The board <b>needs to be reset</b> for any changes to take effect!</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Enable UDP Port 1</td> \
			<td class=bCtr width=\"100%\"><input type=checkbox id=f10 " + ((f1&0x01)?"checked":"") + "> \
			<i>If checked, UDP Port 1 is enabled</i></td></tr> \
		<tr><td class=bLbl>UDP Port</td> \
			<td class=bCtr><input type=text size=6 maxlength=6 id=udp1 value="+(u1L+(u1H*256))+"></td></tr> \
		<tr><td class=bBot colspan=2>&nbsp;</td></tr> \
		<tr><td class=bHdr colspan=2>UDP Port 2 Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure UDP Port 2. The board <b>needs to be reset</b> for any changes to take effect!</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Enable UDP Port 2</td> \
			<td class=bCtr width=\"100%\"><input type=checkbox id=f20 " + ((f2&0x01)?"checked":"") + "> \
			<i>If checked, UDP Port 2 is enabled</i></td></tr> \
		<tr><td class=bLbl>UDP Port</td> \
			<td class=bCtr><input type=text size=6 maxlength=6 id=udp2 value="+(u2L+(u2H*256))+"></td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=u10> \
	<input type=hidden name=u11> \
	<input type=hidden name=u12> \
	<input type=hidden name=u13> \
	<input type=hidden name=u14> \
	<input type=hidden name=u15> \
	</form> \
	");
}

function bucfgSubmit() {
	udp1 = document.getElementById("udp1").value;
	document.getElementsByName("u10")[0].value = (udp1%256);
	document.getElementsByName("u11")[0].value = (udp1-(udp1%256))/256;
	document.getElementsByName("u12")[0].value = getCbxByte("f1",0);

	udp2 = document.getElementById("udp2").value;
	document.getElementsByName("u13")[0].value = (udp2%256);
	document.getElementsByName("u14")[0].value = (udp2-(udp2%256))/256;
	document.getElementsByName("u15")[0].value = getCbxByte("f2",0);
}