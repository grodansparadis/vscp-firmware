<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script type="text/javascript">
function rly(ch,val) {
	document.write("<tr><td class=bLbl>Relay <b>"+ch+"</b> is <b>"+val+"</b></td>");
	document.write("<td class=bCtr width=100%%><input style=\"width:38px;font-size:12px;\" type=submit value=1 name=\"xr"+ch+"\"><input style=\"width:38px;font-size:12px;\" type=submit value=0 name=\"xr"+ch+"\"></td>");
	document.write("</tr>");
}
function op(ch,val) {
	document.write("<tr><td class=bLbl colspan=2>Opto Coupler Channel <b>"+ch+"</b> is <b>"+val+"</b></td>");
	document.write("</tr>");
}
/* ch=io channel number,  t=type 9-configurable, 0-output only, s=lable, an=AN number of PIC ADC, io 0=output 1=input, v=dig val, va=analog val*/
function ioa(ch,t,s,an,io,v,va) {
ac1=((~%k31)&0x0f);
	if(ac1 > an) {
		document.write("<tr><td class=bLbl>I/O <b>"+ch+"</b> ("+s+") is configured as an Analog Input</td>");
		document.write("<td class=bCtr width=100%%>Analog Value = <b>"+va+"</b></td>");
	}
	else {
		//Error, must be output!
		if((t==0)&&(io==1)) {
			document.write("<tr><td class=bLbl colspan=2>O <b>"+ch+"</b> ("+s+"): ERROR! Must be configured as an output</td>");
		}
		else {
			document.write("<tr><td class=bLbl>I/O <b>"+ch+"</b> ("+s+") is configured as a ");
			if(io==0) {
				document.write("Digital Output, current state is <b>"+v+"</b></td>");
				document.write("<td class=bCtr width=100%%><input style=\"width:38px;font-size:12px;\" type=submit value=1 name=\"xo"+ch+"\"><input style=\"width:38px;font-size:12px;\" type=submit value=0 name=\"xo"+ch+"\"></td>");
			}
			else {
				document.write("Digital Input</td>");
				document.write("<td class=bCtr width=100%%>Input = <b>"+v+"</b></td>");
			}
		}
	}
	document.write("</tr>");
}
</script>
</head>

<bodyclass=ifrmBody>

<form method=GET action=_XDB.CGI>
<table class=bBox cellpadding=3 cellspacing=1>
<tr><td class=bHdr colspan=2>Expansion Board</td></tr>
<tr><td class=bDesc colspan=2>This section is used to monitor and control the configured Expansion Board.
An expansion board, like the MXD2R or IOR5E, can be plugged into the Daughter Board Connector of the SBC68EC.
The Expansion Board can be configured <a href="xxdbcfg.cgi">here</a>.
</td></tr>
<tr><td class=bSec colspan=2>Relays</td></tr>
<tr><td class=bDesc colspan=2>Set the value of the relays. To turn on, click button with '1'. To turn off, click button with '0'.</td></tr>
<script type="text/javascript">
xbd=%k87;
if(xbd==1) {
	rly(1,"%x10");
	rly(2,"%x11");
}
else if(xbd==2) {
	rly(1,"%x10");
	rly(2,"%x11");
	rly(3,"%x12");
	rly(4,"%x13");
	rly(5,"%x14");
}
else {
	document.write("<tr><td class=bLbl>Current Expansion Board does not have relays</td>");
}
</script>
<tr><td class=bSec colspan=2>Opto Inputs</td></tr>
<tr><td class=bDesc colspan=2>Monitor the value of the Opto Coupler Inputs.</td></tr>
<script type="text/javascript">
xbd=%k87;
if(xbd==2) {
	op(1,"%x30");
	op(2,"%x31");
	op(3,"%x32");
	op(4,"%x33");
}
else {
	document.write("<tr><td class=bLbl colspan=2>Current Expansion Board does not have opto coupler inputs</td></tr>");
}
</script>
<tr><td class=bSec colspan=2>IO Channels</td></tr>
<tr><td class=bDesc colspan=2>Monitor and Control the 6 I/O channels.
<script type="text/javascript">
xbd=%k87;
if(xbd==1) {
	document.write("<ul><li>I/O channel 1 to 3 can be configured to be a Digital Input, Digital Output, or Analog Input.</li><li>I/O channel 4 to 6 are open collector outputs.</li></ul>");
	document.write("I/O channels 1 to 6 are connected to port A0, A1, A2, C2, C1 and C0 respectively. They are configured on the <a href=\"xacfg.cgi\">Analog Settings</a> and <a href=\"xiocfg.cgi\">Port Settings</a> pages.</td></tr>");
	ioa(1,9,"A0",0,"%a50","%a00","%n20");
	ioa(2,9,"A1",1,"%a51","%a01","%n21");
	ioa(3,9,"A2",2,"%a52","%a02","%n22");
	ioa(4,0,"C2",99,"%c52","%c02",0);
	ioa(5,0,"C1",99,"%c51","%c01",0);
	ioa(6,0,"C0",99,"%c50","%c00",0);
}
else if(xbd==2) {
	document.write("<ul><li>I/O channel 1 to 5 can be configured as a Digital Input, Digital Output, or Analog Input.</li><li>I/O channel 6 can be configured as a Digital Input or Output.</li></ul>");
	document.write("I/O channels 1 to 6 are connected to port A0, A1, A2, A3, A5 and A4 respectively. They are configured on the <a href=\"xacfg.cgi\">Analog Settings</a> and <a href=\"xiocfg.cgi\">Port Settings</a> pages.</td></tr>");
	ioa(1,9,"A0",0,"%a50","%a00","%n20");
	ioa(2,9,"A1",1,"%a51","%a01","%n21");
	ioa(3,9,"A2",2,"%a52","%a02","%n22");
	ioa(4,9,"A3",3,"%a53","%a03","%n23");
	ioa(5,9,"A5",4,"%a55","%a05","%n24");
	ioa(6,9,"A4",99,"%a54","%a04",0);
}
else {
	document.write("</td></tr>");
	document.write("<tr><td class=bLbl>Current Expansion Board does not have opto coupler inputs</td>");
}
</script>

<tr><td class=bBot colspan=10><input type=submit value="Update"></td></tr>
</table>
</form>

</body>
</html>
