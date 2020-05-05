This folder contains example web pages that can be loaded onto the onto the Modtronix
SBC65EC Demo Web Server.

The compiled web pages are in the *.img files in this folder. These files can be uploaded
to the Modtronix SBC65EC Demo Web Server via FTP or serial port (Xmodem protocol). For
details on doing this, refer to the documentation.

The folders contain the source code for the web pages. To compile the source files, use
the Modtronix Network Bootloader application. See www.modtronix.com/soft/netloader for
details. Each folder contains a *.mxweb file which is used by the Modtronix Network
Bootloader to determine which file to included in the created web pages.

***** default *****
This folder contains the default web page contained on the Modtronix SBC65EC Web Server.

***** compress *****
This folder contains an example of how to use the HTTP Compression feature of the
Modtronix Web Server.

***** js_adc *****
This folder contains some web pages that demonstrate how to use JavaScript to scale
Analog values read by the ADC.

***** webterm *****
This folder contains an example of how to use a Java Applet with the Modtronix SBC65EC
Demo Web Server.

***** pinstate *****
This folder contains a VERY simple example of how to display the state of a PIC port pin
on a web page. It will display the state of port pin B1 on a web page. To change the
port pin, simple edit the %b01 line in the "main.cgi" file.

