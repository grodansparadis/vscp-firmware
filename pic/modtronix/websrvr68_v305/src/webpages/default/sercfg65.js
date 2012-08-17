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