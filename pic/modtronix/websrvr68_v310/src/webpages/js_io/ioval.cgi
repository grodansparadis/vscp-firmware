<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<title>Modtronix SBC65EC Demo Web Server</title>

<script type="text/javascript">
<!-- The following function will display a button if the given port pin is configured as an -->
<!-- output, or show the port pin value as "0" or "1" if it configured as a input -->
function ln(io,val,name) {
	document.write("<td align=center>");
	if(io==0) {
		document.write("<input class=pinbtn type=submit value="+val+" name=\""+name+"\">");
	}
	else {
		document.write("<span class=pinlbl>"+val+"</span>");
	}
	document.write("</td>");
}
</script>

</head>


<body marginwidth="0" marginheight="0" topmargin="0" bottommargin="0" leftmargin="0" rightmargin="0">

<br>
<p>
The following example shows how to set port A0, A1, B2 and B3 pins via JavaScript.
Before being able to set a pin, it has to be configured as an output. Select "Port Settings"
in the menu on the left, and configure Port A0, A1, B2 and B3 as outputs.
</p>
<p>
If the Pin is configured as an input, it's value will be shown by a '0' or '1' character.
</p>
<p>
If the Pin is configured as an output, it will be shown as a button. Clicking on this
button will toggle the pin's state.
</p>
<br>

<form method=GET action=IOVAL.CGI>
<table>
	<tr>
		<td><b>Pin A0: </b></td>
		<td><script type="text/javascript">ln(%a50,%a00,"ax0");</script></td>
	</tr>
	<tr>
		<td><b>Pin A1: </b></td>
		<td><script type="text/javascript">ln(%a51,%a01,"ax1");</script></td>
	</tr>
	<tr>
		<td><b>Pin B2: </b></td>
		<td><script type="text/javascript">ln(%b52,%b02,"bx2");</script></td>
	</tr>
	<tr>
		<td><b>Pin B3: </b></td>
		<td><script type="text/javascript">ln(%b53,%b03,"bx3");</script></td>
	</tr>
</table>
</form>

</body>
</html>
