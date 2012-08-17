/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Description of the STR75x Flash Library example. 
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

Example description
===================
This example demonstrates the following features of the STR75x FLASH:
- Erase a sector
- Write a word data
- Write a double word data
- Copy one sector content to another sector
- Enable/disable the write protection (WARNING : Write protection can be only
  temporarily disabled)


Directory contents
==================
  - 75x_type.h       : Common data types file
  - flash_conf.h     : STR7 Flash Library configuration file
  - main.c           : Main program
 

How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required STR7 Flash Library file :
  + 7xx_flash.c
- Link all compiled files and load your image into RAM
- Run the example

Important Note
==============
FLASH_WaitForLastOperation() function must be fetched from RAM since
The first Flash Erase/Program operation is started when the WMS bit is set
and after setting this bit, Bank 0 is no longer accessible.

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE*****/
