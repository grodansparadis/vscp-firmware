function ifrmOl()
{
	if (window.parent && window.parent.document) {
		var h = 2000;
		ifrm = window.parent.document.getElementById('mainIfrm');

		if (ifrm && ifrm.contentDocument) {
			h = ifrm.contentDocument.body.scrollHeight + 100;
		} else if(ifrm && ifrm.contentWindow) {
			h = ifrm.contentWindow.document.body.scrollHeight + 100;
		}

		if (ifrm && (ifrm.height != h)) {
			ifrm.height=h;
		}
	}
}

/* Iframe unload */
function ifrmOu()
{
//	if (window.parent && window.parent.document) {
//		window.parent.document.getElementById('mainIfrm').height = "100%"
//	}
}


/* convert decimal value (0 - 255) to hexadecimal */
function toHex(dec) {
	var arr = "0123456789ABCDEF";
	if (dec < 0)
		return "00";
	if (dec > 255)
		return "FF";
		
	return arr.charAt(dec / 16) + arr.charAt(dec % 16);
}

/* Select the option (in the given Select) with the given hex value (val). */
function selOptHex(sel, val) {
	o = sel.options;
	for (i=0; i<o.length; i++) {
		if (parseInt(o[i].value, 16) == val) {
			o[i].selected = true;
			break;
		}
	}
}

/* Checks given value and returns a value from 0-255.*/
function getByte(val,def) {
	/* Return default value */
	if ((val==null) || isNaN(val)) return def;
	return Math.max(0,Math.min(255,val));
}

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
	<form id=formScfg method=GET action=\"XSCFG.CGI\"> \
	<table width=\"100%\" cellspacing=0 cellpadding=3> \
		<tr align=left " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td width=\"40%\" class=scfgLbl>Modtronix TCP/IP Stack Version: </td> \
			<td>&nbsp; &nbsp;"+stackVer+"</td> \
			<td class=scfgLbli></td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('No additional help available');\"> \
			<td class=scfgLbl>Modtronix SBC65EC Web Server Version: </td> \
			<td>&nbsp; &nbsp;"+appVer+"</td> \
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
			<td>&nbsp; &nbsp;<input type=text size=5 maxlength=5 id=k12 value="+bootDlyStr+"></td> \
			<td class=scfgLbli>In seconds, multiple of 0.8 seconds. Default is 3 seconds, maximum of 8 seconds</td></tr> \
		<tr " + (((b++%2)==1)?"":"bgcolor=\"#F0F0F0\"") + " \
			onMouseover=\"setHelp('On startup, the SBC65EC will write a message out on its serial port and \
				wait for the given time for input from the user. If any input is detected, it will enter \
				serial configuration mode. Setting this time to 0 will disable serial configuration mode!');\"> \
			<td class=scfgLbl>Serial Configuration startup delay: </td> \
			<td>&nbsp; &nbsp;<input type=text size=5 maxlength=5 id=k14 value="+serDlyStr+"></td> \
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
	<table width=\"100%\" cellspacing=10><tr valign=top> \
		<td><input type=button value=Submit onclick=\"scfgSubmit()\"></td> \
		<td nowrap width=30></td> \
		<td height=100px class=helpbox id=helpbox>Hold mouse over item for context sensitive help</td> \
	</tr></table> \
	</form> \
	<form method=\"GET\" action=\"SCFG.CGI\"> \
	<hr> \
		<table width=\"100%\"><tr><td align=center> \
		<input type=hidden name=m value=r> \
		<input type=\"submit\" value=\"!!! Reset Board !!!\"> \
		</td></tr></table> \
	</form> \
	");
}

function scfgSubmit() {
	
	bootDly = getByte(Math.ceil((document.getElementById("k12").value/0.8)), 5);
	bootDly = Math.min(bootDly, 10);	//Max of 8 seconds
	serDly = getByte(Math.ceil((document.getElementById("k14").value/0.05)), 60);

	window.location = document.getElementById("formScfg").action
			+ "?k12=" + bootDly
			+ "&k14=" + serDly
			+ "&k15=" + getCheckboxesByte("sysFlag",0);
}


function iocfg(ta,tb,tc,tf,tg,pa,pb,pc,pf,pg)
{
	document.write(" \
	<p class=pgHdr>PIC Port Settings</p> \
	<hr> \
	<form id=formIocfg method=GET action=\"XIOCFG.CGI\"> \
	<table cellspacing=10><tr> \
		<td> \
			<table class=tbrd cellpadding=3 cellspacing=0> \
				<tr bgcolor=\"#CCCCCC\" style=\"font-weight:bold\"> \
					<td nowrap>Port Pin</td> \
					<td nowrap align=center>&nbsp; 7 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 6 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 5 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 4 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 3 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 2 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 1 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 0 &nbsp;</td> \
				</tr>");
	
					drawCheckboxesRow("#FFFFFF","Port A:","trisa",0x3f,~ta);
					drawCheckboxesRow("#EEEEEE","Port B:","trisb",0xff,~tb);
					drawCheckboxesRow("#FFFFFF","Port C:","trisc",0x27,~tc);
					drawCheckboxesRow("#EEEEEE","Port F:","trisf",0xff,~tf);
					drawCheckboxesRow("#FFFFFF","Port G:","trisg",0x0f,~tg);
	
	document.write(" \
			</table> \
		</td> \
		<td valign=top> \
			<b>Port Direction:</b><br>This section is used to set the direction of \
			the spare I/O pins on the CPU. \
			<table border=1 width=100%> \
				<tr><td>&nbsp; <input disabled type=checkbox checked></input> = Output pin</td></tr> \
				<tr><td>&nbsp; <input disabled type=checkbox></input> = Input pin</td></tr> \
			</table> \
		</td> \
	</tr></table> \
	<hr> \
	<table cellspacing=10><tr> \
		<td> \
			<table class=tbrd cellpadding=3 cellspacing=0> \
				<tr bgcolor=\"#CCCCCC\" style=\"font-weight:bold\"> \
					<td nowrap>Port Pin</td> \
					<td nowrap align=center>&nbsp; 7 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 6 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 5 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 4 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 3 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 2 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 1 &nbsp;</td> \
					<td nowrap align=center>&nbsp; 0 &nbsp;</td> \
				</tr>");
	
					drawCheckboxesRow("#FFFFFF","Port A:","porta",0x3f,pa);
					drawCheckboxesRow("#EEEEEE","Port B:","portb",0xff,pb);
					drawCheckboxesRow("#FFFFFF","Port C:","portc",0x27,pc);
					drawCheckboxesRow("#EEEEEE","Port F:","portf",0xff,pf);
					drawCheckboxesRow("#FFFFFF","Port G:","portg",0x0f,pg);
	
	document.write(" \
			</table> \
		</td> \
		<td valign=top> \
			<b>Default Port Values:</b><br>This section is used to set the default port \
			values on powerup. \
			<table border=1 width=100%> \
				<tr><td>&nbsp; <input disabled type=checkbox checked></input> = 1 (pin is set)</td></tr> \
				<tr><td>&nbsp; <input disabled type=checkbox></input> = 0 (pin is clear)</td></tr> \
			</table> \
		</td> \
	</tr></table> \
	<hr> \
	<table> \
		<tr><td width=20></td><td><input type=button value=Submit onclick=\"iocfgSubmit()\"></td></tr> \
	</table> \
	</form> \
	");
}

function iocfgSubmit() {
	window.location = document.getElementById("formIocfg").action
			+ "?k23=" + getCheckboxesByte("trisa",1)
			+ "&k24=" + getCheckboxesByte("trisb",1)
			+ "&k25=" + getCheckboxesByte("trisc",1)
			+ "&k28=" + getCheckboxesByte("trisf",1)
			+ "&k29=" + getCheckboxesByte("trisg",1)
			+ "&k2A=" + getCheckboxesByte("porta",0)
			+ "&k2B=" + getCheckboxesByte("portb",0)
			+ "&k2C=" + getCheckboxesByte("portc",0)
			+ "&k2F=" + getCheckboxesByte("portf",0)
			+ "&k30=" + getCheckboxesByte("portg",0);
}


/* Draws a row of checkboxes
 * - color = color string, for example #FF0000
 * - label = row label
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - mask = mask of what checkboxes to create
 * - value = what values are selected
 */
function drawCheckboxesRow(color,label,id, mask, value) {
	document.write("<tr bgcolor=\"" + color + "\"><td nowrap>" + label + "</td>");
	for(i=7;i>=0;i--) {
		m = 0x01 << i;
		if (mask & m) {
			document.write("<td align=center><input type=checkbox id="+id+i+" " + ((value&m)?"checked":"") + "></td>");
		}
		else {
			document.write("<td>&nbsp;</td>");
		}
	}
}

/*
 * Returns the byte value of 8 checkboxes with id's = id0, id1 .... id7
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - inverted
 */
function getCheckboxesByte(id,inverted) {
	var ret = 0;
	for(i=7;i>=0;i--) {
		ele = document.getElementById("" + id + i);
		if (ele != null) {
			if (ele.checked == true) {
				ret = ret | (0x01 << i);
			}
		}
	}

	if (inverted)
		ret = ((~ret) & 0xff);
		
	return ret;
}


function acfg(adcon1, adcon2) {
	document.write(" \
	<p class=pgHdr>PIC Analog Input Settings</p> \
	<hr> \
	<br><b>Analog Input Channels:</b><br> \
	This section is used to configure the Analog to Digital converter. \
	<form id=formAcfg method=GET action=\"XACFG.CGI\"> \
	<table> \
		<tr><td>Port Configuration:</td> \
			<td><select id=selectChan> \
			<option value=0F>ADC Off</option> \
			<option value=0E>A0</option> \
			<option value=0D>A0, A1</option> \
			<option value=0C>A0, A1, A2</option> \
			<option value=0B>A0, A1, A2, A3</option> \
			<option value=0A>A0, A1, A2, A3, A5</option> \
			<option value=09>A0, A1, A2, A3, A5, F0</option> \
			<option value=08>A0, A1, A2, A3, A5, F0, F1</option> \
			<option value=07>A0, A1, A2, A3, A5, F0, F1, F2</option> \
			<option value=06>A0, A1, A2, A3, A5, F0, F1, F2, F3</option> \
			<option value=05>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4</option> \
			<option value=04>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5</option> \
			<option value=03>A0, A1, A2, A3, A5, F0, F1, F2, F3, F4, F5, F6</option> \
			</select></td> \
		</tr> \
		<tr><td>Voltage Reference Configuration:&nbsp;</td> \
			<td><select id=selectVRef> \
			<option value=00>Vref+ = AVdd, Vref- = AVss</option> \
			<option value=10>Vref+ = Ext, &nbsp;Vref- = AVss</option> \
			<option value=20>Vref+ = AVdd, Vref- = Ext</option> \
			<option value=30>Vref+ = Ext,  Vref- = Ext</option> \
			</select></td> \
		</tr> \
		<tr><td>Acquisition Time:</td> \
			<td><select id=selectAqt> \
			<option value=38>9.6uS</option> \
			<option value=30>12.8uS</option> \
			<option value=28>19.2uS</option> \
			<option value=18>25.6uS</option> \
			<option value=10>32uS</option> \
			</select></td> \
		</tr> \
	</table> \
	<br><hr> \
	<table> \
	<tr><td width=20></td><td><input type=button value=Submit onclick=\"acfgSubmit(" + adcon2 + ")\"></td></tr> \
	</table> \
	</form> \
	");

	/* Select correct options */
	selOptHex(document.getElementById("selectChan"), (adcon1 & 0x0f));
	selOptHex(document.getElementById("selectVRef"), (adcon1 & 0xf0));
	selOptHex(document.getElementById("selectAqt"), (adcon2 & 0x38));
}

function acfgSubmit(adcon2) {
	/* Calculate ADCON1 and ADCON2 values */
	adcon1 = parseInt(document.getElementById("selectChan").value, 16) | parseInt(document.getElementById("selectVRef").value, 16);
	/* Only modify bits 4 to 6 */
	adcon2 = parseInt(document.getElementById("selectAqt").value, 16) | (adcon2 & ~0x38);
	
	window.location = document.getElementById("formAcfg").action + "?k31=" + adcon1 + "&k32=" + adcon2;
}

function ucfg(baud1) {
	document.write("<p class=pgHdr>USART 1 Settings</p> \
	<hr> \
	<br> \
	<form method=GET action=\"XUCFG.CGI\"> \
	<table> \
		<tr> \
			<td width=150>Baud Rate:</td> \
			<td><select name=k1F id=selectBaud size=1> \
				<option value=0>300</option> \
				<option value=1>1200</option> \
				<option value=2>2400</option> \
				<option value=3>4800</option> \
				<option value=4>9600</option> \
				<option value=5>19200</option> \
				<option value=6>38400</option> \
				<option value=7>57600</option> \
				<option value=8>115200</option> \
				</select> \
			</td> \
		</tr> \
	</table> \
	<br> \
	<hr> \
	<table> \
		<tr><td width=20></td><td><input type=submit value=Submit></td> \
	</table> \
	</form>");

	/* Select current baud rate */
	var x=document.getElementById("selectBaud");
	x.selectedIndex = baud1;
}

function ncfg(i1,i2,i3,i4,g1,g2,g3,g4,s1,s2,s3,s4,m1,m2,m3,m4,m5,m6,netFlags)
{
	document.write("<p class=pgHdr>Network Settings</p> \
	<hr> \
	<br> \
	<form id=formNcfg method=GET action=\"XNCFG.CGI\"> \
	<table> \
		<tr> \
		<td>IP Address:</td> \
		<td><input type=text size=5 maxlength=3 name=k00 value="+i1+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k01 value="+i2+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k02 value="+i3+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k03 value="+i4+"></td> \
		<td collspan=2>&nbsp;</td> \
		</tr> \
		<tr> \
		<td>Subnet Mask:</td> \
		<td><input type=text size=5 maxlength=3 name=k0A value="+s1+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k0B value="+s2+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k0C value="+s3+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k0D value="+s4+"></td> \
		<td collspan=2>&nbsp;</td> \
		</tr> \
		<tr> \
		<td>Gateway Address:</td> \
		<td><input type=text size=5 maxlength=3 name=k0E value="+g1+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k0F value="+g2+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k10 value="+g3+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k11 value="+g4+"></td> \
		<td collspan=2>&nbsp;</td> \
		</tr> \
		<tr> \
		<td>MAC Address:</td> \
		<td><input type=text size=5 maxlength=3 name=k04 value="+m1+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k05 value="+m2+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k06 value="+m3+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k07 value="+m4+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k08 value="+m5+"> . </td> \
		<td><input type=text size=5 maxlength=3 name=k09 value="+m6+"></td> \
		</tr> \
	</table> \
	<br> \
	<table> \
		<tr> \
			<td class=scfgLbl>Enable DHCP: </td> \
			<td>&nbsp; &nbsp;<input type=checkbox id=netFlag0 "+((netFlags&0x01)?"checked":"")+"></td> \
			<td class=scfgLbli>If checked, DHCP is enabled. This module will automatically be assigned an \
			IP address.</td> \
		</tr> \
	</table> \
	<!-- Hidden input is used for k16 - value is calculated in ncfgSubmit function --> \
	<input type=hidden id=netFlags name=k16> \
	<br><hr> \
	<table> \
		<tr><td width=20></td><td><input type=button value=Submit onclick=\"ncfgSubmit();\"></td></tr> \
	</table> \
	</form>");
}

function ncfgSubmit() {
	document.getElementById("netFlags").value = getCheckboxesByte("netFlag",0);
	document.getElementById("formNcfg").submit();
}
