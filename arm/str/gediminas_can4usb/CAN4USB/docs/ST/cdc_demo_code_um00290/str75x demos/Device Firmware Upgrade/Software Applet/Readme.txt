/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006
* Description        : Description of the PC APPLI DfuSe - 
                     : DFU STMicroelectronics Extension.
********************************************************************************
* History:
* 10/01/2006 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

Directory contents
==================

     - Setup.exe		Install file PC DFU application
     + images                   Directory containing STR7/9 demos images

How to use it
=============

  1-  Run "Setup.exe" to install the PC APPLI DfuSe program to the folder of your choice
      (default path: C:\Program Files\STMicroelectronics\DfuSe)
      The directory contents the following files :
      
       - DfuSeDemo.exe		Sample GUI for DFU demonstration
       - DfuFileMgr.exe	        GUI helping people to make DFU files from S19, Hex, bin
       - STDFU.dll		Dll that issues basic DFU and HID for DFU requests
       - STDFUPRT.dll	        Dll that implements Protocol for upload and download. 
       - STDFUFiles.dll	        Dll that implements .dfu files. 
       - STTub203.sys		Driver to be loaded for demoboard
       - STTubeDevice203.dll	Dll layer for easier driver access
       - STDFU.inf		Configuration file for driver
       
  2-  Load your firmware project using your prefered Toolchain and load the DFU image
      using JTAG for the first time. 
    
      For STR91x , you can use also CAPS Software and download the DFU CAPS image included
      in the "images" directory using the flashlink ( BANK1 Must be remapped at 0 at
      boot time).
      
  3-  Run the Board and then Connect your USB cable, The first time you have to
      Install your device with the driver and the inf file already included in the install
      directory.
      
      Please note this inf file and the GUI sample are configured only
      for devices in DFU mode (VID 0x0483, PID 0xDF11)
      
  4- Use it ! : Upgrade your board using the included images and then "Leave DFU Mode"

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******