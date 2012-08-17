/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : DRAMC.c
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the DRAMC software functions
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "720_dramc.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************
* Function Name  : DRAMC_BankConfig
* Description    : This routine is used to configure a specific parameters for
                   the specified external bank (data width,data latency,wait
                   state, idle time and column width,Bank Size).
* Input          : Bankx  (u32)
*                : Config (u16)
*                : Size   (u32)
* Return         : None
*******************************************************************************/
void DRAMC_BankConfig (vu32 Bankx, vu16 Config, vu32 Size )
{      vu32 tmp;
       tmp = (vu32)(( Size * 1024 )/64 )-1;
switch (Bankx)
       {

       case DRAMC_Bank1:    DRAMC-> MB1Config = Config;
                            DRAMC-> Bank1Size =  tmp ;
       break;

       case DRAMC_Bank2:    DRAMC-> MB2Config = Config;
                            DRAMC-> Bank2Size =  tmp ;

       break;

       case DRAMC_Bank3:    DRAMC-> MB3Config = Config;
                            DRAMC-> Bank3Size =  tmp ;

       break;

       case DRAMC_Bank4:    DRAMC-> MB4Config = Config;
                            DRAMC-> Bank4Size =  tmp ;
       break;
       }
}
/******************************************************************************
* Function Name  : DRAMC_ModeRegisterSet
* Description    : This routine is used to configure the internal
                   mode register of the  specified memory bank.
* Input          : Bankx (u32)
*                : Config (u16)
* Return         : None
**********************************************************************************/
void DRAMC_ModeRegisterSet ( vu32 Bankx, vu16 Config )
{
switch (Bankx)
       {

       case DRAMC_Bank1:
                         {
                          #define SDRAM1ConfigLo 0x10
                          #define SDRAM1ConfigHi 0x14
                          //vu32 R0,R1,R2;
                            {
                               //R2 = (vu32)Config&0x0000FFFF;
                               /*__asm
                                {
                                 MOV R0, 0xf0000000
                                 STR R2, [R0, #SDRAM1ConfigLo]
                                 MOV R1, #0x7
                                 STR R1, [R0, #SDRAM1ConfigHi]
                                }*/
                                asm volatile("MOV R2, %0" : : "r" (Config) );
								asm ("LDR R0, =0xf0000000");
								asm volatile("STRH R2, [R0, %0]" : : "i" (SDRAM1ConfigLo) );
								asm volatile("MOV R1, #0x7");
								asm volatile("STRH R1, [R0, %0]" : : "i" (SDRAM1ConfigHi) );
                             }
                         }
      break;

       case DRAMC_Bank2:
                        {
                          #define SDRAM2ConfigLo 0x18
                          #define SDRAM2ConfigHi 0x1C
                          //vu32 R0,R1,R2;
                            {
                               //R2 = (vu32)Config&0x0000FFFF;
                               /*__asm
                                {
                                 MOV R0, 0xf0000000
                                 STR R2, [R0, #SDRAM2ConfigLo]
                                 MOV R1, #0x7
                                 STR R1, [R0, #SDRAM2ConfigHi]
                                }*/
                                asm volatile("MOV R2, %0" : : "r" (Config) );
								asm volatile("LDR R0, =0xf0000000");
								asm volatile("STRH R2, [R0, %0]" : : "i" (SDRAM2ConfigLo) );
								asm volatile("MOV R1, #0x7");
								asm volatile("STRH R1, [R0, %0]" : : "i" (SDRAM2ConfigHi) );
                             }
                         }
       break;

       case DRAMC_Bank3:
                         {
                          #define SDRAM3ConfigLo 0x20
                          #define SDRAM3ConfigHi 0x24
                          //vu32 R0,R1,R2;
                            {
                               //R2 = (vu32)Config&0x0000FFFF;
                               /*__asm
                                {
                                 MOV R0, 0xf0000000
                                 STR R2, [R0, #SDRAM3ConfigLo]
                                 MOV R1, #0x7
                                 STR R1, [R0, #SDRAM3ConfigHi]
                                }*/
                                asm volatile("MOV R2, %0" : : "r" (Config) );
								asm volatile("LDR R0, =0xf0000000");
								asm volatile("STRH R2, [R0, %0]" : : "i" (SDRAM3ConfigLo) );
								asm volatile("MOV R1, #0x7");
								asm volatile("STRH R1, [R0, %0]" : : "i" (SDRAM3ConfigHi) );
                             }
                         }
       break;

       case DRAMC_Bank4:
                         {
                          #define SDRAM4ConfigLo 0x28
                          #define SDRAM4ConfigHi 0x2C
                          //vu32 R0,R1,R2;
                            {
                               //R2 = (vu32)Config&0x0000FFFF;
                               /*__asm
                                {
                                 MOV R0, 0xf0000000
                                 STR R2, [R0, #SDRAM4ConfigLo]
                                 MOV R1, #0x7
                                 STR R1, [R0, #SDRAM4ConfigHi]
                                }*/
                                asm volatile("MOV R2, %0" : : "r" (Config) );
								asm volatile("LDR R0, =0xf0000000");
								asm volatile("STRH R2, [R0, %0]" : : "i" (SDRAM4ConfigLo) );
								asm volatile("MOV R1, #0x7");
								asm volatile("STRH R1, [R0, %0]" : : "i" (SDRAM4ConfigHi) );
                             }
                         }
       break;
       }
}

/**********************************************************************************
* Function Name  : DRAMC_MemoryConfig
* Description    : This routine is used to configure parameters that are common
                   for all banks wich are Power save mode, memory type , Bank
                   enable and refresh period .
* Input          : Config (u16)
*                : T_Refresh (u32)
* Return         : None
************************************************************************************/
void DRAMC_MemoryConfig (vu16 Config , vu32 T_Refresh)
{
	vu8 tmp;
	tmp =((RCCU_GetFrequency(RCCU_AHBClock))/1000000) * T_Refresh/16;
	DRAMC->MemConfig = Config | tmp ;
}
/***********************************************************************************
* Function Name  : DRAMC_CommandSend
* Description    : this routine is used to send specified command to the external memory.
* Input          : Cmd(DRAMC_Command)
* Return         : None
*************************************************************************************/
void DRAMC_CommandSend ( vu32 Bankx , DRAMC_Command Cmd )
{
	vu32 Bank;
	
	if(Bankx==DRAMC_Bank1)
	   Bank=1;
	else 
	if(Bankx==DRAMC_Bank2)
	   Bank=2;
	else 
	if(Bankx==DRAMC_Bank3)
	   Bank=3;
	else 
	if(Bankx==DRAMC_Bank4)
	   Bank=4;

  switch (Cmd)
      {

       case MRS:             * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_MRS_Command;
       break;

       case CBR:             * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_CBR_Command;
       break;

       case PRECH:           * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_PRECH_Command;
       break;

       case PALL:{
                    * (vu16 *) ( DRAMC_BASE + 8  +( 8 * Bank ) ) =0x0400;
                    * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_PALL_Command;
                 }
       break;

       case ACT:             * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_ACT_Command;
       break;

       case WRIT:            * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_WRIT_Command;
       break;

       case WRITA:{
                    * (vu16 *) ( DRAMC_BASE + 8  +( 8 * Bank ) ) = 0x0400;
                    * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_WRITA_Command;
                  }
       break;

       case READ:            * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_READ_Command;
       break;

       case READA:{
                    * (vu16 *) ( DRAMC_BASE + 8  +( 8 * Bank ) ) =0x0400;
                    * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_READA_Command;
                  }
       break;

       case BST:             * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_BST_Command;
       break;

       case NOP:             * (vu16 *) ( DRAMC_BASE + 12 +( 8 * Bank ) ) = DRAMC_NOP_Command;
       break;
       }

    }


/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/

