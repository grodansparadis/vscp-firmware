/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      BSPI software library and an associate SPI EEPROM driver
*                      to communicate with an M95080 SPI bus EEPROM (mounted on
*                      the STR73x-EVAL board). 
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "spi_ee.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAIL = 0, PASS = !FAIL } Status;

/* Private define ------------------------------------------------------------*/
#define BufferSize    (countof(Tx_Buffer)-1)
#define WriteAddr     0x65

/* Private macro -------------------------------------------------------------*/
#define countof(a)    (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
PRCCU_InitTypeDef  PRCCU_InitStructure;
CMU_InitTypeDef    CMU_InitStructure;

u8 Tx_Buffer[] = "/* STR73x BSPI Software Library Example */";
u8 Rx_Buffer[BufferSize];

Status  TransferStatus;

/* Private function prototypes -----------------------------------------------*/
Status Buffercmp(u8* pBuffer, u8* pBuffer1, u16 BufferLength);

/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : main function, where program start. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{

#ifdef DEBUG
    debug();
#endif
  
/* set the system clock to 32MHz, using external 8MHz quartz -----------------*/
  /* select Quartz oscillator as PRCCU clock input */
  CMU_StructInit(&CMU_InitStructure);
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;    
  CMU_Init(&CMU_InitStructure);
  
  /* set MCLK clock to 32 MHz */
  PRCCU_InitStructure.PRCCU_DIV2 = ENABLE;
  PRCCU_InitStructure.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL ;
  PRCCU_InitStructure.PRCCU_PLLDIV = 2 ;
  PRCCU_InitStructure.PRCCU_PLLMUL = PRCCU_PLLMUL_16;
  PRCCU_Init(&PRCCU_InitStructure);

/* perform a write in the memory followed by a read of the written data ------*/
  /* initialize the SPI EEPROM driver */
  SPI_EE_Init();

  /* erase the memory */
  SPI_EE_Erase();
  
  SPI_EE_BufferWrite(Tx_Buffer, WriteAddr, BufferSize);

  SPI_EE_DMABufferRead(Rx_Buffer, WriteAddr, BufferSize); 
 
/* check if the data written to the memory is read correctly -----------------*/
  TransferStatus = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
  /* TransferStatus = PASS, if the data read from the memory is correct */
  /* TransferStatus = FAIL, if the data read from the memory is corrupted */

  SPI_EE_DeInit();

  while(1);
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer, pBuffer1: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASS: pBuffer identical to pBuffer1
*                  FAIL: pBuffer differs from pBuffer1
*******************************************************************************/
Status Buffercmp(u8* pBuffer, u8* pBuffer1, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAIL;
    }
    
    pBuffer++;
    pBuffer1++;
  }
  return PASS;  
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/










