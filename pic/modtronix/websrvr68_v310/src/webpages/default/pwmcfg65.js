function pwmcfg(mode, enable, freq) {
	document.write(" \
	<form id=formPcfg method=GET action=\"XPCFG.CGI\" onsubmit=\"pcfgSubmit();\"> \
	<table class=bBox cellpadding=3 cellspacing=1> \
		<tr><td class=bHdr colspan=2>PWM Settings</td></tr> \
		<tr><td class=bDesc colspan=2>This section is used to configure the PWM channels.</td></tr> \
		<tr><td class=bSec colspan=2>Mode</td></tr> \
		<tr><td class=bDesc colspan=2>The PWM can be configured for 8-bit or 10-bit mode. When in 8-bit \
		mode, channel values are from 0 to 255. When in 10-bit mode, channel values are from 0 to 1023.</td></tr> \
		<tr> \
			<td class=bLbl>Mode</td> \
			<td class=bCtr width=\"100%\"> \
			<select id=selectMode name=wm> \
			<option value=8>8-bit Mode</option> \
			<option value=a>10-bit Mode</option> \
			</select> \
			</td> \
		</tr> \
		<tr><td class=bSec colspan=2>PWM Frequency</td></tr> \
		<tr><td class=bDesc colspan=2>Select the frequency of all PWM channels. The frequency range for 8-bit mode \
		is from 9.766 to 156.25 kHz. The frequency range for 10-bit mode is from 2.441 to 39.062 kHz.</td></tr> \
		<tr> \
			<td class=bLbl>Frequency</td> \
			<td class=bCtr> \
			<select id=selectFreq name=wf> \
			<option value=0>2.441 kHz</option> \
			<option value=1>9.766 kHz</option> \
			<option value=2>39.062 kHz</option> \
			<option value=3>156.25 kHz</option> \
			</select> \
			</td> \
		</tr> \
		<tr><td class=bSec colspan=2>PWM Channels</td></tr> \
		<tr><td class=bDesc colspan=2>Enable PWM Channels. All PWM Channels should also be configured as \
		outputs via the <a href=\"xiocfg.cgi\">Port Settings</a> page.</td></tr> \
		<tr><td class=bLbl>Channel 1 (Port C1)</td><td class=bCtr><input type=checkbox id=en0 " + ((enable&0x01)?"checked":"") + "></td></tr> \
		<tr><td class=bLbl>Channel 2 (Port C2)</td><td class=bCtr><input type=checkbox id=en1 " + ((enable&0x02)?"checked":"") + "></td></tr> \
		<tr><td class=bBot colspan=2><input type=submit value=Update> &nbsp;  &nbsp; Remember Settings After Power Up <input type=checkbox id=rem></td></tr> \
	</table> \
	<!-- Hidden elements - values calculated during submit --> \
	<input type=hidden name=we> \
	</form> \
	");

	/* Select correct options, s because option value is a string */
	selOpt(document.getElementById("selectMode"), 's', mode);
	selOpt(document.getElementById("selectFreq"), 's', freq);
}

function pcfgSubmit() {
	document.getElementsByName("we")[0].value = getCbxHex("en",0);

	if (document.getElementById("rem").checked == true) {
		document.getElementsByName("we")[0].name = 'wer';
		document.getElementsByName("wm")[0].name = 'wmr';
		document.getElementsByName("wf")[0].name = 'wfr';
	}
}
