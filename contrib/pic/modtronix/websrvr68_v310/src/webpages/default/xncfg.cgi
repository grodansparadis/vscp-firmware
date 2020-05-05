<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script src="netcfg01.js"></script>
</head>

<body class=ifrmBody>
<script type="text/javascript">
	var a=new Array(%l10,%l11,%l12,%l13,%l14,%l15,%l16,%l17,%l18,%l19,%l1A,%l1B,%l1C,%l1D,%l1E,%l1F,%l20,%l21);	//AppConfig Array

	if (%l06) demomsg();

	if (%l03 == 0)
		sectags();
	else {
		/*ncfg(10,1,0,1,  10,1,0,254,  255,255,255,0,  255,255,255,0,3,5,0x00);*/
		ncfg(/*IP*/%k00,%k01,%k02,%k03,/*Gateway*/%k0E,%k0F,%k10,%k11,/*Mask*/%k0A,%k0B,%k0C,%k0D,/*MAC*/%k04,%k05,%k06,%k07,%k08,%k09,/*FLAGS*/%k16,a,/*NetBIOS*/"%l05");
	}
</script>
</body>
</html>
