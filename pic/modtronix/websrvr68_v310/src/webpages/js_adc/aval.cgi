<html>
<head>
<STYLE type="text/css">
  .hdr {font-size: 14pt; font-weight: bold;}
</STYLE>

<title>Modtronix SBC65EC Demo Web Server V2.02</title>
<meta http-equiv="refresh" content="2">
<script type="text/javascript">
/* The tags in the following variables are replaced by their corresponding ADC */
/* values from 0 - 255. This is done by the CGI server in the Modtronix TCP/IP stack */
var AN0 = %n10;
var AN1 = %n11;
var AN2 = %n12;
var AN3 = %n13;
var AN4 = %n14;
var AN5 = %n15;
var AN6 = %n16;
var AN7 = %n17;
var AN8 = %n18;
var AN9 = %n19;
var AN10 = %n1A;
var AN11 = %n1B;

function write5v(i)
{
var num = i/(255/5);
var result = num.toFixed(2);
document.write(result);
}

function write12v(i)
{
var num = i/(255/12);
var result = num.toFixed(2);
document.write(result);
}

function write24v(i)
{
var num = i/(255/24);
var result = num.toFixed(2);
document.write(result);
}
</script>

</head>

<body marginwidth="0" marginheight="0" topmargin="0" bottommargin="0" leftmargin="0" rightmargin="0">

<table><tr>
<td width="20">&nbsp;</td>

<td>
<span class="hdr">ADC 5V scalled values</span><br>
The following values are scalled by JavaScript to display their 5V scalled values. This
will be the correct value if no resistor devider is used on the ADC input, and Vdd
is used as the reference voltage.
<table border="1" width="100%%"><tr><td width="25%%">
AN0 = <script type="text/javascript">write5v(AN0);</script>V
<br>AN1 = <script type="text/javascript">write5v(AN1);</script>V
<br>AN2 = <script type="text/javascript">write5v(AN2);</script>V
</td><td width="25%%">
AN3 = <script type="text/javascript">write5v(AN3);</script>V
<br>AN4 = <script type="text/javascript">write5v(AN4);</script>V
<br>AN5 = <script type="text/javascript">write5v(AN5);</script>V
</td><td width="25%%">
AN6 = <script type="text/javascript">write5v(AN6);</script>V
<br>AN7 = <script type="text/javascript">write5v(AN7);</script>V
<br>AN8 = <script type="text/javascript">write5v(AN8);</script>V
</td><td>
AN9 = <script type="text/javascript">write5v(AN9);</script>V
<br>AN10 = <script type="text/javascript">write5v(AN10);</script>V
<br>AN11 = <script type="text/javascript">write5v(AN11);</script>V
</td></tr></table>

<br><span class="hdr">ADC 12V scalled values</span><br>
The following values are scalled by JavaScript to display their 12V scalled values. This
will be the correct value if a 5/12 resistor devider is used on the ADC input, and Vdd
is used as the reference voltage.
<table border="1" width="100%%"><tr><td width="25%%">
AN0 = <script type="text/javascript">write12v(AN0);</script>V
<br>AN1 = <script type="text/javascript">write12v(AN1);</script>V
<br>AN2 = <script type="text/javascript">write12v(AN2);</script>V
</td><td width="25%%">
AN3 = <script type="text/javascript">write12v(AN3);</script>V
<br>AN4 = <script type="text/javascript">write12v(AN4);</script>V
<br>AN5 = <script type="text/javascript">write12v(AN5);</script>V
</td><td width="25%%">
AN6 = <script type="text/javascript">write12v(AN6);</script>V
<br>AN7 = <script type="text/javascript">write12v(AN7);</script>V
<br>AN8 = <script type="text/javascript">write12v(AN8);</script>V
</td><td>
AN9 = <script type="text/javascript">write12v(AN9);</script>V
<br>AN10 = <script type="text/javascript">write12v(AN10);</script>V
<br>AN11 = <script type="text/javascript">write12v(AN11);</script>V
</td></tr></table>


<br><span class="hdr">ADC 24V scalled values</span><br>
The following values are scalled by JavaScript to display their 24V scalled values. This
will be the correct value if a 5/24 resistor devider is used on the ADC input, and Vdd
is used as the reference voltage.
<table border="1" width="100%%"><tr><td width="25%%">
AN0 = <script type="text/javascript">write24v(AN0);</script>V
<br>AN1 = <script type="text/javascript">write24v(AN1);</script>V
<br>AN2 = <script type="text/javascript">write24v(AN2);</script>V
</td><td width="25%%">
AN3 = <script type="text/javascript">write24v(AN3);</script>V
<br>AN4 = <script type="text/javascript">write24v(AN4);</script>V
<br>AN5 = <script type="text/javascript">write24v(AN5);</script>V
</td><td width="25%%">
AN6 = <script type="text/javascript">write24v(AN6);</script>V
<br>AN7 = <script type="text/javascript">write24v(AN7);</script>V
<br>AN8 = <script type="text/javascript">write24v(AN8);</script>V
</td><td>
AN9 = <script type="text/javascript">write24v(AN9);</script>V
<br>AN10 = <script type="text/javascript">write24v(AN10);</script>V
<br>AN11 = <script type="text/javascript">write24v(AN11);</script>V
</td></tr></table>

</td>

<td width="20">&nbsp;</td>
</tr></table>

</body>
</html>
