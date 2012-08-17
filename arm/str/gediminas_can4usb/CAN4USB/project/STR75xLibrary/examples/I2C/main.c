/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Main program body
********************************************************************************
* History: 
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_ee.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define EEPROM_WriteAddress 0x5
#define EEPROM_ReadAddress  0x5
#define BufferSize (countof(Tx_Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
u8  Tx_Buffer[] = "/* STR75x I2C Software Library Example */";
u8 Rx_Buffer[BufferSize];
TestStatus TransferStatus;
ErrorStatus OSC4MStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
TestStatus Buffercmp(u8* pBuffer, u8* pBuffer1, u16 BufferLength);
void MRCC_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main()
{
  #ifdef DEBUG
    debug();
  #endif

  /* MRCC Configuration */
  MRCC_Configuration();

  /* Initialize the I2C EEPROM driver */
  I2C_EE_Init();  

/* Perform a write in the memory followed by a read of the written data ------*/
  /* Write the Tx_Buffer on I2C EEPROM starting from address 0x5 */
  I2C_EE_BufferWrite(Tx_Buffer, EEPROM_WriteAddress, BufferSize); 

  /* Read the Rx_Buffer from I2C EEPROM starting from address 0x5 */
  I2C_EE_BufferRead(Rx_Buffer, EEPROM_ReadAddress, BufferSize); 

  /* Check if the data written to the memory is read correctly */
  TransferStatus = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
  /* TransferStatus = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
  /* TransferStatus = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */

  while(1);
}

/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configures the different system clocks
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MRCC_Configuration(void)
{
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();

  if(OSC4MStartUpStatus == SUCCESS)
  {
    /* Set HCLK to 30MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div2);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 15MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Set CKSYS to 60MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 

  /* I2C and GPIO clock source enable */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_I2C | MRCC_Peripheral_GPIO, ENABLE); 
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer, pBuffer1: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer identical to pBuffer1
*                  FAILED: pBuffer differs from pBuffer1
*******************************************************************************/
TestStatus Buffercmp(u8* pBuffer, u8* pBuffer1, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
  }

  return PASSED;  
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
