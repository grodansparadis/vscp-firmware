/* Draws a row of checkboxes
 * - label = row label
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - mask = mask of what checkboxes to create
 * - value = what values are selected
 */
function drawCheckboxesRow(label,id, mask, value) {
	document.write("<tr><td class=bLbl nowrap align=center>" + label + "</td>");
	for(i=7;i>=0;i--) {
		m = 0x01 << i;
		if (mask & m) {
			document.write("<td class=bCel align=center><input style=\"width:25px;\" type=checkbox id="+id+i+" " + ((value&m)?"checked":"") + "></td>");
		}
		else {
			document.write("<td class=bCel><div style=\"width:25px;\">&nbsp;</div></td>");
		}
	}
	document.write("</tr>");
}

function iocfg(ta,tb,tc,tf,tg,pa,pb,pc,pf,pg,xdb)
{
	document.write(" \
	<form id=formIocfg method=GET action=\"XIOCFG.CGI\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=10>Port Settings</td></tr> \
		<tr><td class=bDesc colspan=10>This section is used to configured the ports.</td></tr> \
		<tr><td class=bSec colspan=10>Port Direction</td></tr> \
		<tr><td class=bDesc colspan=10>This section is used to set the direction of the spare I/O pins on the CPU. \
		When checked, the port is configured as an output pin. When not checked, it is configured as an input pin.</td></tr> \
		<tr><td class=bLbl><b>Port Pin</b></td> \
			<td class=bCel align=center><b>7</b></td> \
			<td class=bCel align=center><b>6</b></td> \
			<td class=bCel align=center><b>5</b></td> \
			<td class=bCel align=center><b>4</b></td> \
			<td class=bCel align=center><b>3</b></td> \
			<td class=bCel align=center><b>2</b></td> \
			<td class=bCel align=center><b>1</b></td> \
			<td class=bCel align=center><b>0</b></td> \
			<td class=bCtr rowspan=6 width=\"100%\"></td> \
		</tr>");

		drawCheckboxesRow("Port A:","trisa",0x3f,~ta);
		drawCheckboxesRow("Port B:","trisb",(xdb!=2)?0xff:0,~tb);
		drawCheckboxesRow("Port C:","trisc",(xdb!=2)?0x27:0,~tc);
		drawCheckboxesRow("Port F:","trisf",0xff,~tf);
		drawCheckboxesRow("Port G:","trisg",0x0f,~tg);

	document.write(" \
		<tr><td class=bSec colspan=10>Default Port Values</td></tr> \
		<tr><td class=bDesc colspan=10>This section is used to set the default port values on powerup. \
		When checked, the port is set (5V). When not checked, the port is clear (0V).</td></tr> \
		<tr><td class=bLbl><b>Port Pin</b></td> \
			<td class=bCel align=center><b>7</b></td> \
			<td class=bCel align=center><b>6</b></td> \
			<td class=bCel align=center><b>5</b></td> \
			<td class=bCel align=center><b>4</b></td> \
			<td class=bCel align=center><b>3</b></td> \
			<td class=bCel align=center><b>2</b></td> \
			<td class=bCel align=center><b>1</b></td> \
			<td class=bCel align=center><b>0</b></td> \
			<td class=bCtr rowspan=6 width=\"100%\"></td> \
		</tr>");
		
		drawCheckboxesRow("Port A:","porta",0x3f,pa);
		drawCheckboxesRow("Port B:","portb",(xdb!=2)?0xff:0,pb);
		drawCheckboxesRow("Port C:","portc",(xdb!=2)?0x27:0,pc);
		drawCheckboxesRow("Port F:","portf",0xff,pf);
		drawCheckboxesRow("Port G:","portg",0x0f,pg);

	document.write(" \
		<tr><td class=bBot colspan=10><input type=button value=Submit onclick=\"iocfgSubmit();\"></td></tr> \
	");
}

function iocfgSubmit() {
	window.location = document.getElementById("formIocfg").action
			+ "?k23=" + getCbxByte("trisa",1)
			+ "&k24=" + getCbxByte("trisb",1)
			+ "&k25=" + getCbxByte("trisc",1)
			+ "&k28=" + getCbxByte("trisf",1)
			+ "&k29=" + getCbxByte("trisg",1)
			+ "&k2A=" + getCbxByte("porta",0)
			+ "&k2B=" + getCbxByte("portb",0)
			+ "&k2C=" + getCbxByte("portc",0)
			+ "&k2F=" + getCbxByte("portf",0)
			+ "&k30=" + getCbxByte("portg",0);
}

function acfg(adcon1, adcon2) {
	document.write(" \
	<form id=formAcfg method=GET action=\"XACFG.CGI\" onsubmit=\"acfgSubmit(" + adcon2 + ");\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Analog Input Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure the Analog to Digital converter.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Port Configuration:</td> \
			<td class=bCtr><select id=selectChan> \
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
		<tr><td class=bLbl>Voltage Reference Configuration:&nbsp;</td> \
			<td class=bCtr><select id=selectVRef> \
			<option value=00>Vref+ = AVdd, Vref- = AVss</option> \
			<option value=10>Vref+ = Ext, &nbsp;Vref- = AVss</option> \
			<option value=20>Vref+ = AVdd, Vref- = Ext</option> \
			<option value=30>Vref+ = Ext,  Vref- = Ext</option> \
			</select></td> \
		</tr> \
		<tr><td class=bLbl>Acquisition Time:</td> \
			<td class=bCtr><select id=selectAqt> \
			<option value=38>9.6uS</option> \
			<option value=30>12.8uS</option> \
			<option value=28>19.2uS</option> \
			<option value=18>25.6uS</option> \
			<option value=10>32uS</option> \
			</select></td> \
		</tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k31> \
	<input type=hidden name=k32> \
	</form> \
	");

	/* Select correct options, x because option value is a hex number */
	selOpt(document.getElementById("selectChan"), 'x', adcon1 & 0x0f);
	selOpt(document.getElementById("selectVRef"), 'x', adcon1 & 0xf0);
	selOpt(document.getElementById("selectAqt"), 'x', adcon2 & 0x38);
}

function acfgSubmit(adcon2) {
	/* Calculate ADCON1 and ADCON2 values */
	adcon1 = parseInt(document.getElementById("selectChan").value, 16) | parseInt(document.getElementById("selectVRef").value, 16);
	/* Only modify bits 4 to 6 */
	adcon2 = parseInt(document.getElementById("selectAqt").value, 16) | (adcon2 & ~0x38);

	document.getElementsByName("k31")[0].value=adcon1;
	document.getElementsByName("k32")[0].value=adcon2;
}
