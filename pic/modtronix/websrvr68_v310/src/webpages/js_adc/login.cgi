<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<link href="mx.css" rel="stylesheet" type="text/css">
</head>

<body>
<p class="pgHdr">Login</p>
<hr>
<p>Please log in. The default Username and Password is "admin" and "pw".
<b>After loggin in, you have to select the page you want to go to again!</b>
<br><br>Current user logged in is: <b>%l00</b><br><br>
<form method=GET action=LOGIN.CGI>
<table>
	<tr>
		<td width="100">Username:</td>
		<td><input value=admin type=text size=20 maxlength=20 name=lu></td>
	</tr>
	<tr>
		<td width="100">Password:</td>
		<td><input value=pw type=password size=20 maxlength=20 name=lp></td>
	</tr>
	<tr><td><br><input type=submit value="Submit"></td></tr>
</table>
</form>
</body>
</html>
