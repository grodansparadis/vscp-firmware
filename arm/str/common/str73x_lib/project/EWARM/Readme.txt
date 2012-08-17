/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0 
* Description        : This sub-directory contains all the user-modifiable files 
*                      needed to create a new roject linked with the STR73x library
*                      and working with IAR Embedded Workbench for ARM software 
*                      toolchain (version 4.30A and Later):
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

- project .ewd/.eww/.ewp: A pre-configured project file with the provided library 
                          structure that produces a debug or Release executable 
                          image with IAR Embedded Workbench.

- 73x_init.s: This module performs RAM remapping (if required), Initializes stack 
              pointers and interrupts for each mode and finally branches to "main" 
              in the C library (which eventually calls main()).

- 73x_vect.s: This file contains exception vectors and exception handlers. 
              It is written in assembler and initializes the exception vectors.

- lnkarm_flash.xcl: This file is the IAR specific linking and loading file used to load 
                    in Flash and execute code and variables to target memories and regions.
                    You can customize this file to your need.

- lnkarm_ram.xcl: This file is the IAR specific linking and loading file used to load 
                  in RAM and execute code and variables to target memories and regions.
                  You can customize this file to your need..

Note
====
The files listed above should be only used with IAR Embedded Workbench for 
ARM software toolchain.

How to use it
=============
In order to make the program work, you must do the following:

- double click on project.eww file.
- load project image: Project -> Debug(ctrl+D)
- Run program:Debug->Go(F5)


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
