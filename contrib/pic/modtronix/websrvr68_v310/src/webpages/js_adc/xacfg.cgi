<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script type="text/javascript">
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
</script>

</head>

<body>
<script type="text/javascript">
	/* The acfg() function displays a page for configuring the ADC. */
	acfg(%k31,%k32 /*adcon1 and adcon2*/ );</script>
</body>
</html>
