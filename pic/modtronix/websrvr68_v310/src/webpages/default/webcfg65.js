function webcfg(fl,fl2,portL,portH)
{
	document.write(" \
	<form id=formScfg method=GET action=\"XWCFG.CGI\" onsubmit=\"wcfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Web Server Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure the HTTP Web Server.</td></tr> \
		<tr><td class=bSec colspan=2>Authentication</td></tr> \
		<tr><td class=bLbl>Required for all pages</td> \
			<td class=bCtr width=\"100%\"><input type=checkbox id=fl1 " + ((fl&0x02)?"checked":"") + "> \
			<i>If checked, Authentication is required for all Web Pages</i></td></tr> \
		<tr><td class=bLbl>Required for \'x\' pages</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl0 " + ((fl&0x01)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, Authentication is required for all Web Pages that start with a \'x\' character.</td></tr> \
		<tr><td class=bLbl>Required for pages with CGI Commands</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl2 " + ((fl&0x04)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, Authentication is required for all pages that issue CGI commands via the HTTP GET Methods. This is the \
			case for any page that has a name-value command appended to the web address. For example, the following address issues \
			the \"c1=1\" command: <i>http://mxboard?c1=1</i></td></tr> \
		<tr><td class=bLbl>Required for all Dynamic pages</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl3 " + ((fl&0x08)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, Authentication is required for all Dynamic pages. All \'CGI'\ files are Dynamic pages. \
			Additional files can also be configured to be Dynamic.</td></tr> \
		<tr><td class=bLbl>Required for '\CGI'\ Dynamic pages</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl4 " + ((fl&0x10)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, Authentication is required for all \'CGI\' files.</td></tr> \
		<tr><td class=bSec colspan=2>Dynamic Pages and Tags</td></tr> \
		<tr><td class=bDesc colspan=2>Dynamic Pages are web pages that are parsed for tags. If any tags are found in these files \
			they are replaced by dynamic values. All \'CGI\' files are parsed by default! All Tags fall in to one of two categories: \
			<ul><li><b>Standard Tags</b>: These tags are always parsed and displayed.</li> \
			<li><b>Secure Tags</b>: These tags require Authentication before they are parsed and displayed. If no Authentication has been done, \
			they will be displayed as 0. All \"Secure\", and \"Configuration Tags\" are secure tags.</li></ul></td></tr> \
		<tr><td class=bLbl>Authentication required to display Secure Tags</td> \
			<td class=bCtr rowspan=2 valign=top><input type=checkbox id=fl5 " + ((fl&0x20)?"checked":"") + "><br></td></tr> \
		<tr><td class=bCel colspan=2 style=\"padding-left:30;\"> \
			If checked, Authentication is required to display any secure tags. If Checked, and no Authentication is provided, they will be displayed as 0.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Web Server Port</td> \
			<td class=bCtr><input type=text size=6 maxlength=6 id=port value="+(portL+(portH*256))+"></td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k72> \
	<input type=hidden name=k73> \
	<input type=hidden name=k74> \
	<input type=hidden name=k75> \
	</form> \
	");
}

function wcfgSubmit() {
	port = document.getElementById("port").value;
	document.getElementsByName("k74")[0].value = (port%256);
	document.getElementsByName("k75")[0].value = (port-(port%256))/256;
	document.getElementsByName("k72")[0].value = getCbxByte("fl",0);
	document.getElementsByName("k73")[0].value = getCbxByte("fl2",0);
}