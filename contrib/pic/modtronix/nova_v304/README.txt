Nova readme
===========

1.) First edit the "projdefs.h" file. Set ip-address (MY_DEFAULT_IP_ADDR_BYTEn), ip-mask (MY_DEFAULT_MASK_BYTEn) 
and default gateway (MY_DEFAULT_GATE_BYTEn). If you have more then one nova unit then also the 
MAC address (MY_DEFAULT_MAC_BYTEn) must be set.

2.) Now compile and flash. It should be possible to ping the unit. Also it is possible to read web pages if the 
EEPROM has veen loaded with data (See instructions that comes with Microchip TCP/IP stack. If EEPROM is
empty or invalid "Not found." found is returned in the browser. Note that STACK_USE_HTTP_SERVER must be define for
the HTTP server ro be enabled. This is the default.

3.) Customize your Nova by editing the "vscp_projdefs.h" file. Documentation is available in the file.
  
