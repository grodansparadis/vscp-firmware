<html>
    <head>
        <noscript>
        <!--
            We have the "refresh" meta-tag in case the user's browser does
            not correctly support JavaScript or has JavaScript disabled.

            Notice that this is nested within a "noscript" block.
        -->
        <meta http-equiv="refresh" content="2">

        </noscript>

        <script language="JavaScript">
        <!--

        var sURL = unescape(window.location.pathname);
        var t;

        function doLoad()
        {
            // the timeout value should be the same as in the "refresh" meta-tag
            value = document.getElementById('refreshRate').value;
            
            t = setTimeout( "refresh()", value*1000 );
        }

        function setRefresh()
        {
            var value;

            clearTimeout(t);
            
            value = document.getElementById('refreshRate').value;

            t = setTimeout( "refresh()", value*1000 );
            
        }
        
        function refresh()
        {
            //  This version of the refresh function will cause a new
            //  entry in the visitor's history.  It is provided for
            //  those browsers that only support JavaScript 1.0.
            //
            window.location.href = sURL;
        }
        
        function stopRefresh()
        {
            clearTimeout(t)
        }
        //-->
        
        </script>

        <script language="JavaScript1.1">
        <!--
        function refresh()
        {
            //  This version does NOT cause an entry in the browser's
            //  page view history.  Most browsers will always retrieve
            //  the document from the web-server whether it is already
            //  in the browsers page-cache or not.
            //  
            window.location.replace( sURL );
        }
        //-->
        </script>

        <script language="JavaScript1.2">
        <!--
        function refresh()
        {
            //  This version of the refresh function will be invoked
            //  for browsers that support JavaScript version 1.2
            //
    
            //  The argument to the location.reload function determines
            //  if the browser should retrieve the document from the
            //  web-server.  In our example all we need to do is cause
            //  the JavaScript block in the document body to be
            //  re-evaluated.  If we needed to pull the document from
            //  the web-server again (such as where the document contents
            //  change dynamically) we would pass the argument as 'true'.
            //  
            window.location.reload( false );
        }
        //-->
        </script>

    </head>
  <body style="color:black; font-size:10pt; font-weight:bold; font-family:arial" onload="doLoad()">

    <p style="line-height:10pt; font-weight:bold; font-size:14pt; color:blue; text-align:center">System Status</p>
    <span style="color:black; font-size:11pt;">LED 1 (D10):</span>
    <span style="color:blue; font-size:11pt;">  O%01</span>
    </br>
    <span style="color:black; font-size:11pt;">LED 2 (D9):</span>
    <span style="color:blue; font-size:11pt;">  O%02</span>
    </br>
    </br>
    <span style="color:black; font-size:12pt;">LCD</span>
    <br/>
    <span style="color:black; font-size:11pt;">Line 1:  </span>
    <span style="color:green; font-size:12pt;"> %03</span>
    <br/>
    <span style="color:black; font-size:11pt;">Line 2:  </span>
    <span style="color:green; font-size:12pt;"> %04</span>
    <br/><br/>
    Refresh rate: <input type="text"    name="rate"     value="5" id="refreshRate" size="3">
    <input type="button"  value="Set"     onclick="setRefresh()">
    <input type="button"  value="Off"     onclick="stopRefresh()">
    <input type="button"  value="Refresh" onclick="refresh()">
    
  </body>
</html>
