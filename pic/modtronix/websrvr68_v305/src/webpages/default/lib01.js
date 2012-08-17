function ifrmOl()
{
	/*
	if (window.parent && window.parent.document) {
		var h = 2000;
		ifrm = window.parent.document.getElementById('mainIfrm');

		if (ifrm && ifrm.contentDocument) {
			h = ifrm.contentDocument.body.scrollHeight + 100;
		} else if(ifrm && ifrm.contentWindow) {
			h = ifrm.contentWindow.document.body.scrollHeight + 100;
		}

		if (ifrm && (ifrm.height != h)) {
			ifrm.height=h;
		}
	}
	*/
}

/* Iframe unload */
function ifrmOu()
{
//	if (window.parent && window.parent.document) {
//		window.parent.document.getElementById('mainIfrm').height = "100%"
//	}
}


/* convert decimal value (0 - 255) to hexadecimal */
function toHex(dec) {
	var arr = "0123456789ABCDEF";
	if (dec < 0)
		return "00";
	if (dec > 255)
		return "FF";
		
	return arr.charAt(dec / 16) + arr.charAt(dec % 16);
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

/* Checks given value and returns a value from 0-255.*/
function getByte(val,def) {
	/* Return default value */
	if ((val==null) || isNaN(val)) return def;
	return Math.max(0,Math.min(255,val));
}

/* Draws a row of checkboxes
 * - color = color string, for example #FF0000
 * - label = row label
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - mask = mask of what checkboxes to create
 * - value = what values are selected
 */
function drawCheckboxesRow(color,label,id, mask, value) {
	document.write("<tr bgcolor=\"" + color + "\"><td nowrap>" + label + "</td>");
	for(i=7;i>=0;i--) {
		m = 0x01 << i;
		if (mask & m) {
			document.write("<td align=center><input type=checkbox id="+id+i+" " + ((value&m)?"checked":"") + "></td>");
		}
		else {
			document.write("<td>&nbsp;</td>");
		}
	}
}

/*
 * Returns the byte value of 8 checkboxes with id's = id0, id1 .... id7
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - inverted
 */
function getCheckboxesByte(id,inverted) {
	var ret = 0;
	for(i=7;i>=0;i--) {
		ele = document.getElementById("" + id + i);
		if (ele != null) {
			if (ele.checked == true) {
				ret = ret | (0x01 << i);
			}
		}
	}

	if (inverted)
		ret = ((~ret) & 0xff);
		
	return ret;
}


