/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      WIU software library.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
********************************************************************************
THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A
RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH
SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN
IN CONNECTION WITH THEIR PRODUCTS.
********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "73x_lib.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  nCount        0xFFFFF
#define  WIU_STOP_Bit  0x04
#define  TB1Prescaler  0x63
#define  TB1Preload    0x270F

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CMU_InitTypeDef    CMU_InitStructure;
TB_InitTypeDef     TB_InitStructure;
WIU_InitTypeDef    WIU_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;

u32 j=0; 
/* Private function prototypes -----------------------------------------------*/
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

/* set system clock (MCLK) to 4MHz -------------------------------------------*/
  CMU_StructInit(&CMU_InitStructure);
  CMU_InitStructure.CMU_EndCountValue = 0xFF;
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init(&CMU_InitStructure);


/* peripheral clock configuration --------------------------------------------*/
  /* enable GPIO4 and GPIO2 clocks */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4, ENABLE);
  CFG_PeripheralClockConfig(CFG_CLK_GPIO2, ENABLE);

  /* enable WIU clock */
  CFG_PeripheralClockConfig(CFG_CLK_WIU, ENABLE);

  /* enable TB1 clock */
  CFG_PeripheralClockConfig(CFG_CLK_TB1, ENABLE);

  /* enable EIC clock */
  CFG_PeripheralClockConfig(CFG_CLK_EIC, ENABLE);  


/* GPIO pins configuration ---------------------------------------------------*/
  /* P4.4 configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_4 ;
  GPIO_Init(GPIO4, &GPIO_InitStructure);

  /* P2.10 configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_10;
  GPIO_Init(GPIO2, &GPIO_InitStructure);


/* EIC configuration ---------------------------------------------------------*/
  EIC_IRQChannelConfig(TB1_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(TB1_IRQChannel, 1);

  EIC_IRQChannelConfig(WIU_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(WIU_IRQChannel, 2);

  EIC_IRQCmd(ENABLE);


/* configure TB1 to generate an interrupt each 250 ms (used to toggle the 
   P4.4 pin) */
  TB_InitStructure.TB_CLK_Source = TB_CLK_INTERNAL; /* MCLK = 4MHz */ 
  TB_InitStructure.TB_Prescaler = TB1Prescaler;
  TB_InitStructure.TB_Preload = TB1Preload;
  TB_Init(TB1, &TB_InitStructure);  

  /* enable TB1 End of Count interrupt */
  TB_ITConfig(TB1, ENABLE);
  
  /* start TB1 counting  */
  TB_Cmd(TB1, ENABLE);

  while(1)
  {
    /* insert a delay before entering Stop mode ------------------------------*/  
    for(j = nCount; j != 0; j--);
  

    /*  a falling edge on P2.10 pin will wake-up the MCU from Stop mode ------*/
    WIU_InitStructure.WIU_Mode = WIU_Mode_WakeUpInterrupt;
    WIU_InitStructure.WIU_TriggerEdge = WIU_FallingEdge;
    WIU_InitStructure.WIU_Line = WIU_Line16;  
    WIU_DeInit();
    WIU_Init(&WIU_InitStructure);
    

    /* Stop mode entering sequence -------------------------------------------*/    
    do
    {  
      WIU_EnterStopMode();
    
    }/* assure the Stop mode was really entered STOP_I = 1 and STOP = 0 */
    while((PRCCU_FlagStatus(PRCCU_FLAG_STOP_I) == RESET) || 
          ((WIU->CTRL & WIU_STOP_Bit) == SET));   


    /* at this stage the MCU entered and exited Stop mode --------------------*/
    /* clear STOP_I flag */ 
    PRCCU_FlagClear(PRCCU_FLAG_STOP_I);  
  
    /* set system clock to 4MHz */
    CMU_Init(&CMU_InitStructure);
  }                                                  
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
