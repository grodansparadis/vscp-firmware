/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      WUT software library.
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
#define  TB0Prescaler  0x63
#define  TB0Preload    0x270F
#define  WUTPrescaler  0x63
#define  WUTPreload    0xB53
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CMU_InitTypeDef    CMU_InitStructure;
TB_InitTypeDef     TB_InitStructure;
WUT_InitTypeDef    WUT_InitStructure;
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
  
  /* RC oscillator frequency:= 29KHz in Stop mode */   
  CMU_Lock(DISABLE);
  CMU_ModeOscConfig(CMU_Stop_Low); 


/* peripheral clock configuration --------------------------------------------*/
  /* enable GPIO4 clock */
  CFG_PeripheralClockConfig(CFG_CLK_GPIO4, ENABLE);

  /* enable WUT clock */
  CFG_PeripheralClockConfig(CFG_CLK_WUT, ENABLE);

  /* enable WIU clock */
  CFG_PeripheralClockConfig(CFG_CLK_WIU, ENABLE);

  /* enable TB0 clock */
  CFG_PeripheralClockConfig(CFG_CLK_TB0, ENABLE);

  /* enable EIC clock */
  CFG_PeripheralClockConfig(CFG_CLK_EIC, ENABLE);  

/* P4.4 configuration --------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 
  GPIO_InitStructure.GPIO_Pins = GPIO_PIN_4 ;
  GPIO_Init(GPIO4, &GPIO_InitStructure);
  

/* configure TB0 to generate an interrupt each 250 ms (used to toggle the
   P4.4 pin) */
  TB_InitStructure.TB_CLK_Source = TB_CLK_INTERNAL; /* MCLK = 4MHz */ 
  TB_InitStructure.TB_Prescaler = TB0Prescaler;
  TB_InitStructure.TB_Preload = TB0Preload;
  TB_Init(TB0, &TB_InitStructure);

  /* enable TB0 End of Count interrupt */
  TB_ITConfig(TB0, ENABLE);
  
  /* EIC configuration */
  EIC_IRQChannelConfig(TB0_IRQChannel, ENABLE);
  EIC_IRQChannelPriorityConfig(TB0_IRQChannel, 1);
  EIC_IRQCmd(ENABLE);

  /* start TB0 counting */
  TB_Cmd(TB0, ENABLE);


/* configure the WUT to wake-up the MCU from Stop mode in 10s ----------------*/
  /* WUT End of Count event connected to WIU input line 0 */
  WUT_InitStructure.WUT_Mode = WUT_Mode_WakeUp; 
  
  /* WUT clock 29KHz (when the MCU enter Stop mode) */
  WUT_InitStructure.WUT_CLK_Source = WUT_CLK_EXTERNAL;   
  
  WUT_InitStructure.WUT_Prescaler = WUTPrescaler;
  WUT_InitStructure.WUT_Preload = WUTPreload;     
  WUT_DeInit();
  WUT_Init(&WUT_InitStructure);

  /* select the WUT to wake-up the MCU from Stop mode */
  WIU_InitStructure.WIU_Mode = WIU_Mode_WakeUp;  
  WIU_InitStructure.WIU_TriggerEdge = WIU_FallingEdge;
  WIU_InitStructure.WIU_Line = WIU_Line0;  
  WIU_DeInit();
  WIU_Init(&WIU_InitStructure);  

  while(1)
  {
    /* insert a delay before entering Stop mode ------------------------------*/ 
    for(j = nCount; j != 0; j--);

    /* start WUT */
    WUT_Cmd(ENABLE); 


    /* Stop mode entering sequence -------------------------------------------*/
    do
    {  
      WIU_EnterStopMode();
    
    }/* assure the Stop mode was really entered STOP_I = 1  STOP = 0 */
    while((PRCCU_FlagStatus(PRCCU_FLAG_STOP_I) == RESET)||
          ((WIU->CTRL & WIU_STOP_Bit) == SET));   


    /* at this stage the MCU entered and exited Stop mode --------------------*/
    /* clear STOP_I flag */ 
    PRCCU_FlagClear(PRCCU_FLAG_STOP_I);  
  
    /* stop WUT */
    WUT_Cmd(DISABLE);
  
    /* clear WUT End of Count flag */
    WUT_FlagClear();  
  
    /* clear wake-up line 0 flag */
    WIU_PendingBitClear(WIU_Line0); 

    /* set system clock to 4MHz */
    CMU_Init(&CMU_InitStructure);

    /* RC oscillator frequency:= 29KHz in Stop mode */      
    CMU_Lock(DISABLE);
    CMU_ModeOscConfig(CMU_Stop_Low);                                                  

  }
}

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
