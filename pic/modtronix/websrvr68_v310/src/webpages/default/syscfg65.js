function elePos(e) {
	var ex = ey = 0;
	if (e.offsetParent) {
		ex = e.offsetLeft
		ey = e.offsetTop
		while (e = e.offsetParent) {
			ex += e.offsetLeft
			ey += e.offsetTop
		}
	}
	return [ex,ey];
}

/* ToolTip Show */
function ttShow(evt)
{
    ev = evt ? evt : window.event; /* IE, Mozilla */
    srcEle = ev.target ? ev.target : ev.srcElement;	/* IE, Mozilla */
    ttEle = document.getElementById(""+srcEle.id+"_");
    
    xy = elePos(srcEle);
    ttEle.style.top = xy[1] + srcEle.offsetHeight;
    ttEle.style.left = xy[0];

    ttEle.style.visibility = 'visible'; 
}

/* ToolTip Hide */
function ttHide(evt)
{
    ev = evt ? evt : window.event; /* IE, Mozilla */
    srcEle = ev.target ? ev.target : ev.srcElement;	/* IE, Mozilla */
    ttEle = document.getElementById(""+srcEle.id+"_");
    ttEle.style.visibility = 'hidden';
}

function ttInit()
{
	for(i=20;i>=0;i--) {
		ele = document.getElementById("tt" + i);
		if (ele != null) {
			ele.onmouseover = ttShow;
			ele.onmouseout = ttHide;
		}
	}
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

	//ToolTips	
	document.write(" \
	<div id=tt1_ class=tt>On startup, the SBC68EC will wait in bootloader mode for a while. \
		During this time it will check for a connection request from the bootloader client. If found, \
		it will connect with the client and wait for further instructions. <b>If you have difficulty \
		connecting to the SBC68EC with the bootloader client, increase this time!</b> Setting this time to 0 \
		will disable the bootloader!</div> \
	<div id=tt2_ class=tt>On startup, the SBC68EC will write a message out on its serial port and \
		wait the configured time for input from the user. If any input is detected, it will enter \
		serial configuration mode. Setting this time to 0 will disable serial configuration mode!</div> \
	");

	document.write(" \
	<form id=formScfg method=GET action=\"XSCFG.CGI\" onsubmit=\"scfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>System Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure System Settings.</td></tr> \
		<tr><td class=bSec colspan=2>Parameters</td></tr> \
		<tr><td class=bLbl>Modtronix TCP/IP Stack Version</td> \
			<td class=bCtr width=\"100%\">"+stackVer+"</td></tr> \
		<tr><td class=bLbl>Modtronix SBC68EC Web Server Version</td> \
			<td class=bCtr>"+appVer+"</td></tr> \
		<tr><td class=bLbl>Board has bootloader</td> \
			<td class=bCtr>"+hasBL+"</td></tr> \
		<tr><td class=bLbl>Currently logged in as</td> \
			<td class=bCtr>"+user+"</td></tr> \
		<tr><td class=bLbl>Blink System LED</td> \
			<td class=bCtr><input type=checkbox id=sysFlag1 " + ((sysFlags&0x02)?"checked":"") + "> \
			<i>If checked, System LED is toggled every 500ms</i></td></tr> \
		<tr><td class=bLbl id=tt1>Bootloader startup delay</td> \
			<td class=bCtr><input type=text size=5 maxlength=5 id=bootDly value="+bootDlyStr+">&nbsp;<i>In seconds, multiple of 0.8 seconds. Default is 3 seconds, maximum of 8 seconds</i></td></tr> \
		<tr><td class=bLbl id=tt2>Serial Configuration startup delay</td> \
			<td class=bCtr><input type=text size=5 maxlength=5 id=serDly value="+serDlyStr+">&nbsp;<i>In seconds. Default is 3.2 seconds, maximum of 12 seconds</i></td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k12> \
	<input type=hidden name=k14> \
	<input type=hidden name=k15> \
	</form> \
	");

	document.write(" \
	To change the username and password, <a href=\"XUSER.CGI\">click here</a>! \
	<hr> \
	<form method=GET action=\"XSCFG.CGI\"> \
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
	document.getElementsByName("k15")[0].value = getCbxByte("sysFlag",0);
}