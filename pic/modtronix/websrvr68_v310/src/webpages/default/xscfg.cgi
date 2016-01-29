<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script src="syscfg65.js"></script>

<style type="text/css">
.tt 
{
visibility:hidden;
position:absolute;
top:0;
left:0;
z-index:2;
font:normal 8pt sans-serif; 
padding:3px;
border:solid 1px #000000;
background-color:#FFFFDD;
width:30em;
}
</style>

</head>

<body class=ifrmBody>
<script type="text/javascript">
	if (%l06) demomsg();

	if (%l03 == 0)
		sectags();
	else {
		/* scfg("Guest", "V2.04", "V3.00", 3, 60, 0x03); //Uncomment for testing */
		scfg("%l00","%l01","%l02",%k12,%k14,%k15,"%l04");
		ttInit();	//ToolTip Init
	}
</script>
</body>
</html>