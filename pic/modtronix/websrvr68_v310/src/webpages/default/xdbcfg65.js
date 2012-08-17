function xdbcfg(xbrdtype) {
	document.write(" \
	<form id=formXdbcfg method=GET action=\"XXDBCFG.CGI\" onsubmit=\"xdbcfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Expansion Board Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure an expansion board that can be plugged into the Daughter Board Connector.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Expansion Board Type:</td> \
			<td class=bCtr><select id=selectXdb> \
			<option value=0>None</option> \
			<option value=1>MXD2R</option> \
			<option value=2>IOR5E</option> \
			</select></td> \
		</tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k87> \
	</form> \
	");

	/* Select correct options, s because option value is a string */
	selOpt(document.getElementById("selectXdb"), 's', xbrdtype);
}

function xdbcfgSubmit() {
	document.getElementsByName("k87")[0].value = document.getElementById("selectXdb").value;
}
