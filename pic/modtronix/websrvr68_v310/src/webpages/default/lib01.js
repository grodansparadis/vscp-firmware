/* page contains secure tags, and was are not logged in */
function sectags() {
	document.write("This page requires Authentication! Please first log in before requesting this page!");	
}

function demomsg() {
	document.write("<div style=\"color:#FF0000;\"><h4>This is a Demo, many critical functions have been disabled!</h4></div>");	
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

/* Select the option (in the given Select) with the given string value.
   Type is [s, i, x] for [str, int, hex] option */
function selOpt(sel, type, val) {
	o = sel.options;
	for (i=0; i<o.length; i++) {
		v = o[i].value;
		if (type=='x')
			v = parseInt(o[i].value, 16);
		if (type=='i')
			v = parseInt(o[i].value, 10);
		if (v == val) {
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

/*
 * Returns the uppercase hex value of 8 checkboxes with id's = id0, id1 .... id7
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - inverted
 */
function getCbxHex(id,inverted) {
	return toHex(getCbxByte(id,inverted));
}

/*
 * Returns the decimal byte value of 8 checkboxes with id's = id0, id1 .... id7
 * - id = id or each checkbox. 0 to 7 is appended to id string
 * - inverted
 */
function getCbxByte(id,inverted) {
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