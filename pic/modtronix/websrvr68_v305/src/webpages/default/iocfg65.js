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
		<tr><td width=20></td><td><input type=button value=Submit onclick=\"iocfgSubmit();\"></td></tr> \
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


function acfg(adcon1, adcon2) {
	document.write(" \
	<p class=pgHdr>PIC Analog Input Settings</p> \
	<hr> \
	<br><b>Analog Input Channels:</b><br> \
	This section is used to configure the Analog to Digital converter. \
	<form id=formAcfg method=GET action=\"XACFG.CGI\" onsubmit=\"acfgSubmit(" + adcon2 + ");\"> \
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
	<tr><td width=20></td><td><input type=submit value=Submit></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k31> \
	<input type=hidden name=k32> \
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

	document.getElementsByName("k31")[0].value=adcon1;
	document.getElementsByName("k32")[0].value=adcon2;
}
