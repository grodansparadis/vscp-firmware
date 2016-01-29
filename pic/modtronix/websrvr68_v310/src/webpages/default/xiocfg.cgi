<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
<script src="lib01.js"></script>
<script src="iocfg65.js"></script>
</head>

<body class=ifrmBody>
<script type="text/javascript">
	if (%l03 == 0)
		sectags();
	else {
		/*iocfg(0,1,2,4,5 , 1,2,3,4,5);*/
		iocfg(%k23,%k24,%k25,%k28,%k29/*TRISA,B,C,F,G*/, %k2A,%k2B,%k2C,%k2F,%k30/*DefaultA,B,C,F,G*/,%k87/*expansion brd*/);
	}
</script>
</body>
</html>