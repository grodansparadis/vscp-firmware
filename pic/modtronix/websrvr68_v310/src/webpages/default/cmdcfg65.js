function uccfg(cmdL,cmdH,cmdrL,cmdrH,fl)
{
	document.write(" \
	<form id=formScfg method=GET action=\"XCCFG.CGI\" onsubmit=\"scfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Command Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure Command Settings.</td></tr> \
		<tr><td class=bSec colspan=2>UDP Commands</td></tr> \
		<tr><td class=bLbl>Enable UDP Commands</td> \
			<td class=bCtr width=\"100%\"><input type=checkbox id=fl0 " + ((fl&0x01)?"checked":"") + "> \
			<i>If checked, UDP Commands are enabled</i></td></tr> \
		<tr><td class=bLbl>Authentication required</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl1 " + ((fl&0x02)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, each UDP Command message requires an username and password included with \
			the message. For example, if username is \"admin\" and password is \"pw\", \
			then \"admin:pw c1=1\" command will set Port C1.</td></tr> \
		<tr><td class=bLbl>UDP Command Port</td> \
			<td class=bCtr><input type=text size=6 maxlength=6 id=cp value="+(cmdL+(cmdH*256))+"></td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k5B> \
	<input type=hidden name=k5C> \
	<input type=hidden name=k62> \
	</form> \
	");
}

function scfgSubmit() {
	cp = document.getElementById("cp").value;
	document.getElementsByName("k5B")[0].value = (cp%256);
	document.getElementsByName("k5C")[0].value = (cp-(cp%256))/256;
	document.getElementsByName("k62")[0].value = getCbxByte("fl",0);
}