/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Hardware Configuration & Setup
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

/* Includes ------------------------------------------------------------------*/
#include "71x_lib.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_prop.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern u8 buffer_in[VIRTUAL_COM_PORT_DATA_SIZE];
extern u32 count_in;
extern LINE_CODING linecoding;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  u32 Tmp=0 ;

     RCCU_MCLKConfig ( RCCU_DEFAULT );
     RCCU_FCLKConfig ( RCCU_RCLK_2 );
     RCCU_PCLKConfig ( RCCU_RCLK_2 );



     /* Configure the PLL1 ( * 16 , / 2 )*/
     /*RCCU_PLL1Config ( RCCU_PLL1_Mul_12 , RCCU_Div_2 ) ;*/

     Tmp = ( RCCU->PLL1CR & ~0x30 ) | ( RCCU_PLL1_Mul_12 << 0x04 );
     RCCU->PLL1CR = ( Tmp & ~0x7 ) | RCCU_Div_2 | 0x40;

     for ( Tmp = 0x7f ; Tmp> 0; Tmp --);

    /* Select PLL1_Output as RCLK clock */

    /*RCCU_RCLKSourceConfig ( RCCU_PLL1_Output ) ;*/
     RCCU->CFR = RCCU->CFR | RCCU_CK2_16_Mask;
     if (( RCCU->PLL1CR & 0x0007 ) != 7)
     while(!(RCCU->CFR & RCCU_LOCK_Mask));
     /*  Deselect The CKAF */
     RCCU->CCR  &= ~RCCU_CKAF_SEL_Mask;
     /*  Select The CSU_CKSEL */
     RCCU->CFR |= RCCU_CSU_CKSEL_Mask;
    /* at this step the MCLK clock should be equal to 48Mhz */
     
    /*  Configure the GPIO pins */
    GPIO_Config(GPIO0, UART0_Tx_Pin, GPIO_AF_PP);
    GPIO_Config(GPIO0, UART0_Rx_Pin, GPIO_IN_TRI_CMOS);
}
/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Configure the PLL2 ( * 12 , / 1 ) assuming HCLK=4MHz */
  /* RCCU_PLL2Config (RCCU_PLL2_Mul_12,RCCU_Div_1)*/
  /* PCU->PLL2CR  = 0x81D0; */
}
/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  EIC_IRQChannelPriorityConfig(USBLP_IRQChannel,2);
  EIC_IRQChannelConfig(USBLP_IRQChannel,ENABLE);
  EIC_IRQConfig(ENABLE);
}
/*******************************************************************************
* Function Name  :  UART0_Config_Default.  
* Description    :  configure the UART 0 with default values.
* Input          :  None.
* Return         :  None.
*******************************************************************************/
void UART0_Config_Default(void)
{
   /*  Turn UART0 on */
  UART_OnOffConfig(UART0, ENABLE);
  /*  Disable FIFOs */       
  UART_FifoConfig (UART0, DISABLE); 
  /*  Reset the UART_RxFIFO */     
  UART_FifoReset  (UART0 , UART_RxFIFO);
  /*  Reset the UART_TxFIFO */ 
  UART_FifoReset  (UART0 , UART_TxFIFO); 
  /*  Disable Loop Back */
  UART_LoopBackConfig(UART0 , DISABLE);  
                                         /* Configure the UART0 as following:
                                            - Baudrate = 9600 Bps
                                            - No parity
                                            - 8 data bits
                                            - 1 stop bit */
  UART_Config(UART0,115200,UART_NO_PARITY,UART_1_StopBits,UARTM_8D);
  /*  Enable Rx */
  UART_RxConfig(UART0 ,ENABLE);
  
  /* Configure the EIC channel interrupt */
  EIC_IRQChannelPriorityConfig(UART0_IRQChannel, 1);
  EIC_IRQChannelConfig(UART0_IRQChannel, ENABLE);
  EIC_IRQConfig(ENABLE);

  UART_ItConfig(UART0,UART_RxBufFull, ENABLE);
}
/*******************************************************************************
* Function Name  :  UART0_Config.  
* Description    :  Configure the UART 0 according to the linecoding structure.
* Input          :  None.
* Return         :  Configuration status 
                    TRUE : configuration done with success  
                    FALSE : configuration aborted.
*******************************************************************************/
bool UART0_Config(void)
{
  u32 Selected_BaudRate;
  UARTParity_TypeDef Selected_Parity;
  UARTStopBits_TypeDef Selected_StopBits;
  UARTMode_TypeDef Selected_Mode;
  
   /* set the bit rate*/
  if (linecoding.bitrate > 115200)
  {
    UART0_Config_Default();
    return (FALSE);
  }
  else 
    Selected_BaudRate = linecoding.bitrate;
  
  /* set the Stop bit*/
  switch (linecoding.format)
  {
  case 0: Selected_StopBits = UART_1_StopBits;
           break;
  case 1: Selected_StopBits = UART_1_5_StopBits;
           break;
  case 2: Selected_StopBits = UART_2_StopBits;
           break;
  default :
    {
      UART0_Config_Default();
      return (FALSE);
    }
  }
  /* set the parity bit*/
  switch (linecoding.paritytype)
  {
  case 0: Selected_Parity = UART_NO_PARITY;
           break;
  case 1: Selected_Parity = UART_EVEN_PARITY;
           break;
  case 2: Selected_Parity = UART_ODD_PARITY;
  default :
    {
      UART0_Config_Default();
      return (FALSE);
    }
  }
   /*set the data type*/
  if (Selected_Parity == UART_NO_PARITY)
  {
    if (linecoding.datatype == 0x08)
      Selected_Mode = UARTM_8D;
    else
    {
      UART0_Config_Default();
      return (FALSE);
    }
  }
  else 
  {
      switch (linecoding.datatype)
      {
      case 0x08: Selected_Mode = UARTM_8D_P;
           break;
      case 0x07: Selected_Mode = UARTM_7D_P;
           break;
      default : 
        {
          UART0_Config_Default();
          return (FALSE);
        }
      }
  }
  UART_Config(UART0,Selected_BaudRate,Selected_Parity,Selected_StopBits,Selected_Mode);
  /*  Enable Rx */
  UART_RxConfig(UART0 ,ENABLE);
  return (TRUE);
}
/*******************************************************************************
* Function Name  : USB_To_UART_Send_Data. 
* Description    : send the received data from USB to the UART 0. 
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send. 
* Return         : none.
*******************************************************************************/
void USB_To_UART_Send_Data(u8* data_buffer, u8 Nb_bytes)
{
  u32 i;
  
  for(i=0;i<Nb_bytes;i++)
  {
    UART_ByteSend(UART0, &(data_buffer[i]));
  }
}
/*******************************************************************************
* Function Name  : UART_To_USB_Send_Data. 
* Description    : send the received data from UART 0 to USB. 
* Input          : None.
* Return         : none.
*******************************************************************************/
void UART_To_USB_Send_Data(void)
{
   /*  Get the received byte, set the guard time to 0xFF */
  UART_ByteReceive(UART0, &buffer_in[count_in], 0xFF);
  count_in++;
  UserToPMABufferCopy(buffer_in, ENDP1_TXADDR, count_in);
  SetEPTxCount(ENDP1,count_in);
  SetEPTxValid(ENDP1);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
