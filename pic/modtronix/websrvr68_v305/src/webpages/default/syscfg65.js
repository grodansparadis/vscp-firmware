function setHelp(str) {
	document.getElementById("helpbox").innerHTML = str;
}

function scfg(user, stackVer, appVer, bootDly, serDly, sysFlags, hasBL)
{
	if (isNaN(serDly)) serDly = 60;
	if (isNaN(bootDly)) bootDly = 5;
	serDlyStr = (serDly * 0.05).toString();
	bootDlyStr = (bootDly * 0.8).toString();

	i = serDlyStr.indexOf(".");
	if (i != -1) serDlyStr = serDlyStr.slice(0, i+2);

	i = bootDlyStr.indexOf(".");
	if (i != -1) bootDlyStr = bootDlyStr.slice(0, i+2);
	
	b = 0;
	document.write(" \
	<p class=pgHdr>System Settings</p> \
	<hr> \
	<table width=\"100%\" cellspacing=0 cellpadding=3> \
		<tr align=left " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td width=\"40%\" class=scfgLbl>Modtronix TCP/IP Stack Version: </td> \
			<td nowrap>&nbsp; &nbsp;"+stackVer+"</td> \
			<td class=scfgLbli></td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Modtronix SBC65EC Web Server Version: </td> \
			<td nowrap>&nbsp; &nbsp;"+appVer+"</td> \
			<td class=scfgLbli></td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Board has bootloader: </td> \
			<td>&nbsp; &nbsp;"+hasBL+"</td> \
			<td class=scfgLbli></td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Currently logged in as: </td> \
			<td>&nbsp; &nbsp;"+user+"</td> \
			<td class=scfgLbli></td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('On startup, the SBC65EC will wait in bootloader mode for a while. \
				During this time it will check for a connection request from the bootloader client. If found, \
				it will connect with the client and wait for further instructions. <b>If you have difficulty \
				connecting to the SBC65EC with the bootloader client, increase this time!</b> Setting this time to 0 \
				will disable the bootloader!');\"> \
			<td class=scfgLbl>Bootloader startup delay: </td> \
			<td>&nbsp; &nbsp;<input type=text size=5 maxlength=5 id=bootDly value="+bootDlyStr+"></td> \
			<td class=scfgLbli>In seconds, multiple of 0.8 seconds. Default is 3 seconds, maximum of 8 seconds</td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('On startup, the SBC65EC will write a message out on its serial port and \
				wait for the given time for input from the user. If any input is detected, it will enter \
				serial configuration mode. Setting this time to 0 will disable serial configuration mode!');\"> \
			<td class=scfgLbl>Serial Configuration startup delay: </td> \
			<td>&nbsp; &nbsp;<input type=text size=5 maxlength=5 id=serDly value="+serDlyStr+"></td> \
			<td class=scfgLbli>In seconds. Default is 3.2 seconds, maximum of 12 seconds</td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Blink Sysem LED: </td> \
			<td>&nbsp; &nbsp;<input type=checkbox id=sysFlag1 " + ((sysFlags&0x02)?"checked":"") + "></td> \
			<td class=scfgLbli>If checked, System LED is toggled every 500ms</td></tr> \
		<!-- Disable this feature seeing that is currently does not work with HiTech Compiler \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Set system clock to 40MHz: </td> \
			<td>&nbsp; &nbsp;<input type=checkbox id=sysFlag0 " + ((sysFlags&0x01)?"checked":"") + "></td> \
			<td class=scfgLbli>If checked, system clock is 40MHz, else it is 10MHz</td></tr> \
		--> \
	</table> \
	<hr> \
	<form method=GET action=\"XSCFG.CGI\" onsubmit=\"scfgSubmit();\"> \
	<table width=\"100%\" cellspacing=10><tr valign=top> \
		<td><input type=submit value=Submit></td> \
		<td nowrap width=30></td> \
		<td height=100px class=helpbox id=helpbox>Hold mouse over item for context sensitive help</td> \
	</tr></table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k12> \
	<input type=hidden name=k14> \
	<input type=hidden name=k15> \
	</form> \
	<form method=\"GET\" action=\"SCFG.CGI\"> \
	<hr> \
		<table width=\"100%\"><tr><td align=center> \
		<input type=hidden name=m value=r> \
		<input type=submit value=\"!!! Reset Board !!!\"> \
		</td></tr></table> \
	</form> \
	");
}

function scfgSubmit() {
	bootDly = getByte(Math.ceil((document.getElementById("bootDly").value/0.8)), 5);
	document.getElementsByName("k12")[0].value = Math.min(bootDly, 10);	//Max of 8 seconds
	document.getElementsByName("k14")[0].value = getByte(Math.ceil((document.getElementById("serDly").value/0.05)), 60);
	document.getElementsByName("k15")[0].value = getCheckboxesByte("sysFlag",0);
}