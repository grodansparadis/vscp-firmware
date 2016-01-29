/*
 * Displays network configuration page
 * - i1 to i4 = IP address
 * - g1 to g4 = Gateway address
 * - s1 to s4 = Subnet Mask
 * - m1 to m6 = MAC address
 * - netFlags = Netflags
 * - a = AppConfig array
 * - n = NetBIOS name
 */
function ncfg(i1,i2,i3,i4,g1,g2,g3,g4,s1,s2,s3,s4,m1,m2,m3,m4,m5,m6,netFlags,a,n)
{
	document.write(" \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>Current Network Value</td></tr> \
		<tr><td class=bDesc colspan=2>This section shows the current network values.</td></tr> \
		<tr><td class=bLbl>IP Address:</td><td class=bCtr width=\"100%\">"+a[0]+"."+a[1]+"."+a[2]+"."+a[3]+"</td></tr> \
		<tr><td class=bLbl>Subnet Mask:</td><td class=bCtr>"+a[10]+"."+a[11]+"."+a[12]+"."+a[13]+" </td></tr> \
		<tr><td class=bLbl>Gateway Address:</td><td class=bCtr>"+a[14]+"."+a[15]+"."+a[16]+"."+a[17]+" </td></tr> \
		<tr><td class=bLbl>MAC Address:</td><td class=bCtr>"+a[4]+"."+a[5]+"."+a[6]+"."+a[7]+"."+a[8]+"."+a[9]+" </td></tr> \
	</table><br> \
	");

	document.write(" \
	<form id=formNcfg method=GET action=\"XNCFG.CGI\" onsubmit=\"ncfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=8>Network Settings</td></tr> \
		<tr><td class=bDesc colspan=8>The IP, Subnet and Gateway addresses are only used in the following situations: \
			<ul><li>When  DHCP is disabled</li> \
			<li>When DHCP is enabled, but there is no DHCP server available on the network</li> \
			<li>For the network bootloader</li></ul> \
			If DHCP is enabled and available on the network, all these values will be obtained from the DHCP server. \
		</td></tr> \
		<tr> \
		<td class=bLbl>IP Address:</td> \
			<td class=bCel style=\"padding-left:10;\"><input type=text size=4 maxlength=3 name=k00 value="+i1+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k01 value="+i2+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k02 value="+i3+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k03 value="+i4+"></td> \
			<td class=bCel colspan=3></td> \
		</tr> \
		<tr> \
			<td class=bLbl>Subnet Mask:</td> \
			<td class=bCel style=\"padding-left:10;\"><input type=text size=4 maxlength=3 name=k0A value="+s1+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k0B value="+s2+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k0C value="+s3+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k0D value="+s4+"></td> \
			<td class=bCel colspan=3></td> \
		</tr> \
		<tr> \
			<td class=bLbl>Gateway Address:</td> \
			<td class=bCel style=\"padding-left:10;\"><input type=text size=4 maxlength=3 name=k0E value="+g1+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k0F value="+g2+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k10 value="+g3+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k11 value="+g4+"></td> \
			<td class=bCel colspan=3></td> \
		</tr> \
		<tr> \
			<td class=bLbl>MAC Address:</td> \
			<td class=bCel style=\"padding-left:10;\"><input type=text size=4 maxlength=3 name=k04 value="+m1+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k05 value="+m2+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k06 value="+m3+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k07 value="+m4+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k08 value="+m5+"></td> \
			<td class=bCel><input type=text size=4 maxlength=3 name=k09 value="+m6+"></td> \
			<td class=bCel width=\"100%\"></td> \
		</tr> \
		<tr><td class=bSec colspan=8>Parameters</td></tr> \
		<tr><td class=bLbl>Enable DHCP</td> \
			<td class=bCtr colspan=7><input type=checkbox id=netFlag0 "+((netFlags&0x01)?"checked":"")+"> &nbsp; \
			<i>If checked, DHCP is enabled. This module will automatically be assigned an IP, Subnet and Gateway address.</i></td> \
		</tr> \
		<tr><td class=bLbl>NetBIOS name</td> \
			<td class=bCtr colspan=7><input type=text size=20 maxlength=15 name=ln value="+n+"></td> \
		</tr> \
		<tr><td class=bBot colspan=8><input type=submit value=Update></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k16> \
	</form> \
	");
}

function ncfgSubmit() {
	document.getElementsByName("k16")[0].value = getCbxByte("netFlag",0);
}
