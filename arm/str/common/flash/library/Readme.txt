/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Description of the STR7 Flash Library.
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

Flash Driver Description
========================
The STR7 Flash Library is a collection of routines and data structures that cover
the STR7 embedded Flash memory programming.

Directory contents
==================
 + inc
   - 7xx_flash.h     : Header for '7xx_flash.c' file 
   - flash_map.h     : Flash registers definition and memory mapping
   
 + src
   - 7xx_flash.c     : Flash software functions
 
 
How to use it
=============
This driver can be used with the STR71x, STR73x and STR75x standard libraries.
You have just to include it in the correspondent project and choose in the 
'flash_conf.h' file your STR7 product. 

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
