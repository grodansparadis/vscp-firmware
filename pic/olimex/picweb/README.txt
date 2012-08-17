0. INTRODUCTION

This is a fast getting started for using the Microchip TCP/IP stack with Olimex PIC-WEB, PIC-MAXI-WEB, PIC_MICRO_WEB boards. 
It is highly recommended to read theexcellent stack documentation from Microchip before proceeding.


1. GETTING THE ORIGINAL MICROCHIP TCP/IP STACK DISTRIBUTION.

The Microchip TCP/IP stack is now part of the Microchip Application Libraries. The installer can be downloaded from:
	http://www.microchip.com/
	
*NOTE: The patch was tested with TCP/IP stack version v5.31. It may work with later releases of the stack but you should test 
this yourself.

Run the installer and install to a directory of you choosing. In order to proceed copy these two directories to a temporary 
location:
	./TCPIP Demo App
	./Microchip

	
2. PATCHING 
quad non me necat fortior me facit
Unzip the patch in the same temporary location. You shouls end up with the following structure:
	./TCPIP Demo App
	./Microchip
	./Prebuilt
	./apply-patch.bat
	./patch.exe
	./README.txt
	./Olimex-PIC-WEB.patch
	
Now execute the "apply-patch.bat" script.

Here is a sample output of applying the patch:
-----------------------------------------------------------------------------------
Please put this batch file, the patch.exe program and the patch
file "Olimex-PIC-WEB.patch" inside the Microchip distribution
directory 'Microchip Solutions' (the directory that contains the
subdirectories "Internet Bootloader App", "Internet Radio App",
"Microchip", "SSL Demo App", "TCPIP Demo App", "TCPIP WebVend App").

WARNING: The only ported project is ".\TCPIP Demo App\TCPIP Demo App-C18.mcw"

WARNING: This firmware is for PIC-WEB board revisions B and later.

Press any key to continue . . .
patching file 'Microchip/Include/TCPIP Stack/SPIFlash.h'
patching file 'Microchip/MPFSImg2.bin'
patching file 'Microchip/TCPIP Stack/CustomHTTPApp.c'
patching file 'Microchip/TCPIP Stack/ENC28J60.c'
patching file 'Microchip/TCPIP Stack/ENC28J60.err'
patching file 'Microchip/TCPIP Stack/ENCX24J600.err'
patching file 'Microchip/TCPIP Stack/SPIEEPROM.c'
patching file 'Microchip/TCPIP Stack/SPIFlash.c'
patching file 'Microchip/TCPIP Stack/SPIFlash.err'
patching file 'Microchip/TCPIP Stack/SPIRAM.err'
patching file 'TCPIP Demo App/CustomHTTPApp.c'
patching file 'TCPIP Demo App/HardwareProfile.h'
patching file 'TCPIP Demo App/HTTPPrint.h'
patching file 'TCPIP Demo App/HTTPPrint.idx'
patching file 'TCPIP Demo App/MPFSImg2.bin'
patching file 'TCPIP Demo App/TCPIPConfig.h'
patching file 'TCPIP Demo App/TCPIP Demo App-C18.mcp'
patching file 'TCPIP Demo App/TCPIP Demo App-C18.mcw'
patching file 'TCPIP Demo App/WebPagesOlimex/auth.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/cookies.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/dyndns/index.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/dynvars.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/email/index.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/footer.inc'
patching file 'TCPIP Demo App/WebPagesOlimex/forms.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/header.inc'
patching file 'TCPIP Demo App/WebPagesOlimex/index.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/leds.cgi'
patching file 'TCPIP Demo App/WebPagesOlimex/mchp.css'
patching file 'TCPIP Demo App/WebPagesOlimex/mchp.gif'
patching file 'TCPIP Demo App/WebPagesOlimex/mchp.js'
patching file 'TCPIP Demo App/WebPagesOlimex/olimex.gif'
patching file 'TCPIP Demo App/WebPagesOlimex/protect/config.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/protect/index.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/protect/reboot.cgi'
patching file 'TCPIP Demo App/WebPagesOlimex/protect/reboot.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/snmp/snmpconfig.htm'
patching file 'TCPIP Demo App/WebPagesOlimex/snmp.bib'
patching file 'TCPIP Demo App/WebPagesOlimex/status.xml'
patching file 'TCPIP Demo App/WebPagesOlimex/upload.htm'


Press any key to continue . . .
-----------------------------------------------------------------------------------

3. RUNNING THE DEMO

Open "./TCPIP Demo App/TCPIP Demo App-C18.mcw" workspace. 

*NOTE: In order to compile successfully you need to have MC18 installed. Please also ensure that project directories are set 
properly to match path of the compiler (i.e. right-click the project->Build Options...->Directories tab).
	
Now you can compile the project and download it to the board. Of course, you can also use the project as a starting project for 
your custom application.
	
	
4. UPDATING THE WEB PAGE

The web pages for PIC-WEB are located in 
	./TCPIP Demo App/WebPagesOlimex"
	
You should run the "./Microchip/TCPIP Stack/Utilities/MPFS2.exe" to regenerate the image file (default is "MPFSImg2.bin") that 
shoulb be uploaded to the flash memory of the board.

Then, assuming PIC-WEB has IP address 192.168.0.30, open the following URL in your web browser: "http://192.168.0.30/mpfsupload". 
Browse to "MPFSImg2.bin" and upload it. After that the new web page should be available.


5. LOCATING PIC-WEB

Run the "./Microchip/TCPIP Stack/Utilities/Microchip Ethernet Discoverer.exe" utility in order to find what IP address has 
PIC-WEB fetched from DHCP. Another option is to connect a serial port to the board RS232 connector, configure your terminal 
application for 19200 8N1, and see the board status messages during power up.

*Note: Default setting of the board is to use DHCP to retrieve network configuration (IP address, etc.).. This can be modified 
by editing the "TCPIPConfig.h".

Last edit: 15 Feb 2011

Special thanks to Paolo for providing this demo :)



