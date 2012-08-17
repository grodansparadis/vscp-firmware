/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0 
* Description        : This sub-directory contains all the user-modifiable files 
                       needed to create a new  project linked with the STR73x library 
                       and working with ARM RealView Developper Kit software toolchain.
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


- project.prj: A pre-configured project file with the provided library structure
               that produces a debug\release executable image with RealViewTM.

- 73x_init.s: This module performs RAM remapping (if required), Initializes stack 
              pointers and interrupts for each mode and finally branches to "main" 
              in the C library (which eventually calls main()).

- 73x_vect.s: This file contains exception vectors and exception handlers. 
              It is written in assembler and initializes the exception vectors.


- scat_flash.scf:This file is the ARM specific scatter loading file used to load 
                 in Flash and execute code and variables to target memories and regions.
                 You can customize this file to your need.

- scat_ram.scf: This file is the ARM specific scatter loading file used to load in RAM 
                and execute code and variables to target memories and regions.You can 
                customize this file to your need.

Note
====
The files listed above should be only used with the ARM RealView Developper
Kit software toolchain.

How to use it
=============
In order to make the program work, you must do the following:
- launch ARM RealView Developper Kit.
- open project.prj file: Project->Open Project
- load project image:File->Load Image(ctrl+shift+O)
- run program:Debug->Execution Control-> Go (F5)


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
