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
	document.write("<p class=pgHdr>Network Settingsss</p> \
	<hr> \
	\
	<b>Current Values</b> \
	<table> \
	<tr><td>IP Address:</td><td>"+a[0]+"."+a[1]+"."+a[2]+"."+a[3]+"</td></tr> \
	<tr><td>Subnet Mask:</td><td>"+a[10]+"."+a[11]+"."+a[12]+"."+a[13]+" </td></tr> \
	<tr><td>Gateway Address:</td><td>"+a[14]+"."+a[15]+"."+a[16]+"."+a[17]+" </td></tr> \
	<tr><td>MAC Address:</td><td>"+a[4]+"."+a[5]+"."+a[6]+"."+a[7]+"."+a[8]+"."+a[9]+" </td></tr> \
	</table><hr> \
	\
	<form id=formNcfg method=GET action=\"XNCFG.CGI\" onsubmit=\"ncfgSubmit();\"> \
	<b>Default Settings</b><br> \
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
			<td align=left nowrap>Enable DHCP: </td> \
			<td>&nbsp; &nbsp;<input type=checkbox id=netFlag0 "+((netFlags&0x01)?"checked":"")+"></td> \
			<td class=scfgLbli>If checked, DHCP is enabled. This module will automatically be assigned an \
			IP, Subnet and Gateway address.</td> \
		</tr> \
		<tr> \
			<td align=left nowrap>NetBIOS name: </td> \
			<td colspan=2><input type=text size=20 maxlength=15 name=ln value="+n+"></td> \
		</tr> \
	</table> \
	<br><div class=stxt>The IP, Subnet and Gateway addresses are only used in the following situations: \
	<br> - When  DHCP is disabled \
	<br> - When DHCP is enabled, but there is no DHCP server available on the network \
	<br> - For the network bootloader \
	<br>If DHCP is enabled and available on the network, all these values will be obtained from the DHCP server. \
	</div> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=k16> \
	<br><br> \
	<table> \
		<tr><td width=20></td><td><input type=submit value=Submit></td></tr> \
	</table> \
	</form>");
}

function ncfgSubmit() {
	document.getElementsByName("k16")[0].value = getCheckboxesByte("netFlag",0);
}
