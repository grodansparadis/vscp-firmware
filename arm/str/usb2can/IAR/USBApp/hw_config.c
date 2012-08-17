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
#include "75x_lib.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus         OSC4MStartUpStatus;
GPIO_InitTypeDef    GPIO_InitStructure;
UART_InitTypeDef    UART_InitStructure;
EIC_IRQInitTypeDef  EIC_IRQInitStructure;
GPIO_InitTypeDef    CAN_RX;
GPIO_InitTypeDef    CAN_TX;
CAN_InitTypeDef     CAN_Struct;
TB_InitTypeDef      TB_InitStructure; 

unsigned long aaa;
extern canmsg TxCanMsg;
extern canmsg RxCanMsg;
extern volatile u32 GlobalTick;

/* External variables --------------------------------------------------------*/
extern u8 buffer_in[VIRTUAL_COM_PORT_DATA_SIZE];
extern u32 count_in;
extern LINE_CODING linecoding;
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System
* Description    : Set System clock.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
GPIO_InitTypeDef    GPIO_InitStructure;

//  EXTIT_InitTypeDef   EXTIT_InitStructure;
  
/* MRCC configuration --------------------------------------------------------*/
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();
  
  // Wait for OSC4M start-up
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();
  
    if ( SUCCESS == OSC4MStartUpStatus ) {     
    // Set HCLK (Configures the AHB clock) 30MHZ
    MRCC_HCLKConfig( MRCC_CKSYS_Div2 ); // Div1
  
    // Set CKTIM (Configures the TIM clock) 30MHZ
    MRCC_CKTIMConfig( MRCC_HCLK_Div1 );  // Div2
  
    // Set PCLK (Configures the APB clock) 30MHz
    MRCC_PCLKConfig( MRCC_CKTIM_Div1 ); // Div1
  
  /* Enable FLASH burst mode */
  //  CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);    
    
    // Set CKSYS to 64 MHz
    MRCC_CKSYSConfig( MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15 ); // 60 MHz      
  }  

/*  
    if ( SUCCESS == OSC4MStartUpStatus ) {     
      
    // Set HCLK (Configures the AHB clock) 64MHZ
    MRCC_HCLKConfig( MRCC_CKSYS_Div1 );
  
    // Set CKTIM (Configures the TIM clock) 16MHZ
    MRCC_CKTIMConfig( MRCC_HCLK_Div4 );
  
    // Set PCLK (Configures the APB clock) 8MHz
    MRCC_PCLKConfig( MRCC_CKTIM_Div2 );

    // Enable FLASH burst mode
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  
    // Set CKSYS to 64 MHz
    MRCC_CKSYSConfig( MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_16 ); // 64 MHz      
  }   
*/

  /* Enbale No Clock Detected interrupt */
  MRCC_ITConfig(MRCC_IT_NCKD, ENABLE);

  // GPIO,UART and CAN clock enabling 
  MRCC_PeripheralClockConfig( MRCC_Peripheral_GPIO |
                              //MRCC_Peripheral_CAN |
                              //MRCC_Peripheral_UART0 |  
                              //MRCC_Peripheral_UART1 | 
                              MRCC_Peripheral_I2C |  
                              MRCC_Peripheral_USB |
                              MRCC_Peripheral_TB ,  
                              ENABLE );
   
   
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
   
  GPIO_DeInit( GPIO0 );
  GPIO_DeInit( GPIO1 );
  GPIO_DeInit( GPIO2 );
   
  /* Configure P2.18 ( USB Suspend/Resume ) as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_19;
  GPIO_Init(GPIO2, &GPIO_InitStructure);

  GPIO_WriteBit(GPIO2, GPIO_Pin_19, (BitAction)(0));

   /* Configure P1.0 (D+ Pull up)  as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIO1, &GPIO_InitStructure);
  
   /* Configure P0 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //// status LEDS ///
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10;
  GPIO_Init(GPIO0, &GPIO_InitStructure);   
  
  /* Configure the UART1_Tx as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_21;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
  
  /* Configure the UART1_Rx and UART1_CTS as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_20;
  GPIO_Init(GPIO0, &GPIO_InitStructure); 
    
  ///////////// CAN /////////////////////////
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIO0, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIO0, &GPIO_InitStructure); 
}
/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Set USB kernel clock to 48 MHz */
  MRCC_CKUSBConfig (MRCC_CKUSB_Internal);
  CFG_USBFilterConfig(CFG_USBFilter_Enable);
}
/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
/*    GPIO_WriteBit(GPIO2, GPIO_Pin_19, (BitAction)(1));

     EXTIT_ClearITPendingBit(EXTIT_ITLine14);


     // Enter STOP mode with OSC4M off, FLASH off and MVREG off
      MRCC_EnterSTOPMode(MRCC_STOPParam_MVREGOff);

     //Clear LP_DONE flag
      MRCC_ClearFlag(MRCC_FLAG_LPDONE);*/
}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  /* EXTIT_ClearITPendingBit(EXTIT_ITLine14);

   // wake up and enable all clocks in STR75x
   //WaitForOSC4MStartUp();

    // Clear No Clock Detected flag
    MRCC_ClearFlag(MRCC_FLAG_NCKD);
    // Set CKSYS to 60MHz
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);

    Set_USBClock();*/
}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : configure and enable the USB interrupt Lines
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
/* Enable and configure the priority of the USB_LP IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = USB_LP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable and configure the priority of the USB_HP IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = USB_HP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 3;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure); 
   
  /* Enable and configure the priority of the TB IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TB_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 4;     
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);  
  
  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);
} 
/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
     if (NewState == ENABLE)
       GPIO_WriteBit(GPIO1, GPIO_Pin_0, (BitAction)(0));
     else
       GPIO_WriteBit(GPIO1, GPIO_Pin_0, (BitAction)(1));
}
/*******************************************************************************
* Function Name  :  UART0_Config_Default.  
* Description    :  configure the UART 0 with default values.
* Input          :  None.
* Return         :  None.
*******************************************************************************/
void UART0_Config_Default(void)
{
  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No ;
  UART_InitStructure.UART_BaudRate = 9600;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Disable;
  UART_Init(UART0, &UART_InitStructure);
  UART_Cmd(UART0, DISABLE);
}

/*******************************************************************************
* Function Name  :  UART0_Config_Default.  
* Description    :  configure the UART 0 with default values.
* Input          :  None.
* Return         :  None.
*******************************************************************************/
void UART1_Config_Default(void)
{
/* UART1 configuration -------------------------------------------------------*/

  UART_InitStructure.UART_WordLength = UART_WordLength_8D;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No ;
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Enable;
  UART_InitStructure.UART_TxFIFOLevel = UART_FIFOLevel_1_2; 
  UART_InitStructure.UART_RxFIFOLevel = UART_FIFOLevel_1_2;
  
  UART_Init(UART1, &UART_InitStructure); 
  /* Enable the UART1 */
  UART_Cmd(UART1, DISABLE);  
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
  /* set the bit rate*/
  if (linecoding.bitrate > 115200)
  {
    UART0_Config_Default();
    return (FALSE);
  }
  /* set the Stop bit*/
  switch (linecoding.format)
  {
  case 0: UART_InitStructure.UART_StopBits = UART_StopBits_1;
           break;
  case 2: UART_InitStructure.UART_StopBits = UART_StopBits_2;
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
  case 0: UART_InitStructure.UART_Parity = UART_Parity_No;
           break;
  case 1: UART_InitStructure.UART_Parity = UART_Parity_Even;
           break;
  case 2: UART_InitStructure.UART_Parity = UART_Parity_Odd;
           break;
  case 3: UART_InitStructure.UART_Parity = UART_Parity_OddStick;
           break;
  case 4: UART_InitStructure.UART_Parity = UART_Parity_EvenStick;
           break;
  default :
    {
      UART0_Config_Default();
      return (FALSE);
    }
  }
  /*set the data type*/
  switch (linecoding.datatype)
  {
  case 0x08: UART_InitStructure.UART_WordLength = UART_WordLength_8D;
           break;
  case 0x07: UART_InitStructure.UART_WordLength = UART_WordLength_7D;
           break;
  case 0x06: UART_InitStructure.UART_WordLength = UART_WordLength_6D;
           break;
  case 0x05: UART_InitStructure.UART_WordLength = UART_WordLength_5D;
           break;
  default : 
    {
      UART0_Config_Default();
      return (FALSE);
    }
  }

  UART_InitStructure.UART_BaudRate = linecoding.bitrate;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Tx_Rx;
  UART_InitStructure.UART_FIFO = UART_FIFO_Disable;
  UART_Init(UART0, &UART_InitStructure);
  UART_Cmd(UART0, ENABLE);
  return (TRUE);
}

/*******************************************************************************
* Function Name  :  CAN_Config  
* Description    :  Configure the CAN
* Input          :  None.
* Return         :  None                                        
******************************************************************************/

void CAN_Config(void)
{
 u8   temp;  

  CAN_DeInit();    
  
  MRCC_PeripheralClockConfig( MRCC_Peripheral_CAN,ENABLE );
  
      // Enable CAN interrupt and priority
  EIC_IRQInitStructure.EIC_IRQChannel=CAN_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd=ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);  
  
  // configure the message objects  
  CAN_InvalidateAllMsgObj();
  
/************** STD TX Objects *******************/
  
  CAN_SetTxMsgObj( 28 , CAN_STD_ID );
  CAN_SetTxMsgObj( 29 , CAN_STD_ID );
  
/************** EXT TX Objects *******************/
  
  CAN_SetTxMsgObj( 30 , CAN_EXT_ID );
  CAN_SetTxMsgObj( 31 , CAN_EXT_ID );  
  
/************** STD RX Objects *******************/
  
  for(temp = BEGIN_RX_STD_OBJ;temp <= END_RX_STD_OBJ;temp++)
  {
    CAN_SetRxMsgObj( temp, CAN_STD_ID, 0, CAN_LAST_STD_ID, TRUE );
  }  
  
/************** EXT RX Objects ******************/
  
  for(temp = BEGIN_RX_EXT_OBJ;temp <= END_RX_EXT_OBJ;temp++)
  {
    CAN_SetRxMsgObj( temp, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
  }       
  
  //CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;//| CAN_CR_EIE | CAN_CR_SIE;  
  //CAN_Struct.CAN_Bitrate=CAN_BITRATE_1M;
  //CAN_Init( &CAN_Struct );  
  
//  MRCC_PeripheralClockConfig( MRCC_Peripheral_CAN,DISABLE );
}

void CAN_ENABLE(void)
{  
 MRCC_PeripheralClockConfig( MRCC_Peripheral_CAN,ENABLE );
   
 // CAN_Struct.CAN_ConfigParameters = CAN_CR_IE ;//| CAN_CR_EIE | CAN_CR_SIE;  
 // CAN_Struct.CAN_Bitrate=CAN_BITRATE_125K;
 // CAN_Init( &CAN_Struct );  
}

void CAN_DISABLE(void)
{
  //CAN_Struct.CAN_ConfigParameters = 0;
  //CAN_Init( &CAN_Struct );
  
 MRCC_PeripheralClockConfig( MRCC_Peripheral_CAN,DISABLE );
}

///////////////////////////////////////////////////////////////////////////////
// LED_ON
//

void Led_ON( u8 LED ) 
{
  switch ( LED ) {
    case RED:
      GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_SET );
      //GPIO_WriteBit( GPIO1, GPIO_Pin_1, Bit_SET );
    break;
    case GREEN:
      GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_SET );
     //GPIO_WriteBit( GPIO1, GPIO_Pin_2, Bit_SET );
    break;
    default:;
  }
}

///////////////////////////////////////////////////////////////////////////////
// LED_OFF
//

void Led_OFF( u8 LED )
{
  switch ( LED ) {
    case RED:
      GPIO_WriteBit( GPIO0, GPIO_Pin_1, Bit_RESET );
      //GPIO_WriteBit( GPIO1, GPIO_Pin_1, Bit_RESET );
    break;
    case GREEN:
      GPIO_WriteBit( GPIO0, GPIO_Pin_2, Bit_RESET );
      //GPIO_WriteBit( GPIO1, GPIO_Pin_2, Bit_RESET );
    break;
    default:;
  }
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
    UART_SendData(UART0, *(data_buffer + i));
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
  buffer_in[count_in] = UART_ReceiveData(UART0);
  count_in++;
//  UserToPMABufferCopy(buffer_in,ENDP1_TXADDR, count_in);
  SetEPTxCount(ENDP1,count_in);
  SetEPTxValid(ENDP1);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

void Set_TB(void)
{
 /* TB configuration in Timing mode ------------------------------------------*/
  /* TB is configured to generate an update every 1 ms */ /* CKTIM=16MHz */
  TB_InitStructure.TB_Mode = TB_Mode_Timing; 
  TB_InitStructure.TB_ClockSource = TB_ClockSource_CKTIM;
  TB_InitStructure.TB_Prescaler = 120; //252;  //was63; // 64-1 //239; /* TB clock = 60MHz / (239+1) = 250KHz*/
  TB_InitStructure.TB_CounterMode = TB_CounterMode_Up; 
  TB_InitStructure.TB_AutoReload = 249;// 250-1 // 499; /* update each:  250 KHz/(499+1)= 500HZ */     
   
  TB_Init(&TB_InitStructure);

  /* Enable TB update interrupt */
  TB_ITConfig(TB_IT_Update, ENABLE);

  /* Enable TB counter */
  TB_Cmd(ENABLE);
}

/*******************************************************************************
* Function Name  : Reset_Device
* Description    : Reset the device using the watchdog Reset
* Input          : None.
* Return         : None.
*******************************************************************************/

void Reset_Device(void)
{
   WDG_InitTypeDef WDG_InitStructure;
  /* Soft USB disconnection */
  USB_Cable_Config(DISABLE);
  /* Configure the WDG to generate a system reset signal */
  WDG_InitStructure.WDG_Mode = WDG_Mode_WDG;
  WDG_InitStructure.WDG_Preload = 0x7FF;
  WDG_InitStructure.WDG_Prescaler = 0xFF;
  WDG_Init(&WDG_InitStructure);

  /* Enable the WDG */
  WDG_Cmd(ENABLE);

}
