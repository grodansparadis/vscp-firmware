/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0 
* Description        : This sub-directory contains all the user-modifiable files 
*                      needed to create a new project linked with the necessary 
*                      files of STR73x library and working with RIDE STR7 toolchain
*                      (version BN742 and Later):
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
*********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************



Directory contents
===================

- project.prj: A pre-configured project file with the provided library 
               structure that produces a debug executable image 
               with RIDE STR7 BN742.

- 73x_init.s: This file contains exception vectors and exception handlers. 
              and it allows to  initialize the exception vectors, stack  pointers and 
              interrupts for each mode  and finally branches to "main" in the C library
              (which eventually calls main()).


Note
====
- The files listed above should be only used with the RIDE STR7 software toolchain
  (GNU compiler).



How to use it
=============
In order to make the program work, you must do the following:

- you must have installed RIDE BN742 (or later) version 
- Connect UART0 to your Windows PC  via a RS232 cable 
- Configure Windows Hyperterminal application : 9600bps,8 data, Parity even, stop bits 1
- double click on project.prj file.
- Load  project image: Debug -> Start Project.elf (CTRL+D) 
- Run program: Debug ->Run(ctrl+F9)


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
