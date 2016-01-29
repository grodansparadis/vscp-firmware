Readme for the Arduino VSCP CAN Library
========================================
Version 0.0.1 2012-08-29
Copyright (C) Åke Hedman, Grodans Paradis AB
akhe@grodansparadis.com, http://www.vscp.org

This is a simple implementation of VSCP on an Arduino Duemilanove board
(http://arduino.cc/en/Main/arduinoBoardDuemilanove) with a CAN shield
from sparkfun.com (https://www.sparkfun.com/products/10039). It may or
may not work on other board combinations.

As the CAN library written for the shield did not handle extended id's
which are needed by VSCP and is very poorly written I decided to use a
library by David Harding and changed this code to my needs.

If you are new to VSCP please start by reading this short primer
http://sourceforge.net/projects/m2m/files/VSCP%20Documentation/VSCP_primer.pdf/download
also there is some introduction videos at http://vscp.org/docs.php and you
can also find the full specification at that address.

VSCP is constructed to make it easy (and fun) to build control systems that
can be controlled remotely and work with or without a master device. VSCP is free
and will always be free to use and implement i products.

A software suite called VSCP & Friends is available that can be run on Windows
and Linux. Can be downloaded here 
https://sourceforge.net/projects/m2m/files/vscp/

-------------------------------------------------------------------------------

