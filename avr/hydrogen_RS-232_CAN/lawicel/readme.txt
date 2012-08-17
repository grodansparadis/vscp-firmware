Installation Instructions                 WiCAN - Demo for M161
---------------------------------------------------------------

* Unzip the wicandemo_M161.zip file into a new empty directory

* Move the libwicandemo_M161.a file to the ICCAVR LIB
  folder (normally C:\ICCAVR\LIB\)

Tesing the demo project with latest ICCAVR

  * Start ICCAVR and open the WiCANdemo_M161.prj from the
    Project->Open menu. All settings etc. should now
    be done automatically. If not, do as described in
    the manual section below.

  * Press the build button and you have created the HEX
    file that can be downloaded and tested in the CANDIP.

Manually creating a project:


  * Start ICCAVR and make a new project and save it to the
    location where the sample files where installed.
    (Project->New)

  * Edit the options (Project->Options) and set the target
    to M161 and enter wicandemo_M161 in the additional
    lib. and press OK

  * Then add files to the project (Project->Add File(s))
    and add both WiCANdemo_M161.C and WiCAN.H files. They
    will then show up in the right Project window.

  * Press the build button and you have created the HEX
    file that can be downloaded and tested in the CANDIP.


Important Information.
---------------------------------------------------------

The DemoLIB is only capable to hold 4 CAN frames in
both FIFO queues and is only capable of running at
20kbit, so all other speeds will cause an error.
But it support both 11- and 29bit ID's.

For support and questions e-mail wican@candip.com

This DemoLIB can be freely distributed as long as all
files are given away (the original zip file).

LAWICEL HB
Klubbgatan 3
SE-282 32  TYRINGE
SWEDEN

http://www.candip.com
http://www.lawicel.com
