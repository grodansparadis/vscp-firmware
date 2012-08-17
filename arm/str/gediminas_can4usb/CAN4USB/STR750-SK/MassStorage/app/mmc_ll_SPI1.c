/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : mmc_ll.h
 *    Description : define MMC module
 *
 *    History :
 *    1. Date        : September 22, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/

// Hardware depend definitions
/*************************************************************************
 * Function Name: MmcChipSelect
 * Parameters: Boolean Select
 * Return: none
 *
 * Description: Mmc Chip select control
 * Select = true  - Chip is enable
 * Select = false - Chip is disable
 *
 *************************************************************************/
void MmcChipSelect (Boolean Select)
{
  GPIO_WriteBit(GPIO0,GPIO_Pin_19,Select?Bit_RESET:Bit_SET);
}
/*************************************************************************
 * Function Name: MmcPresent
 * Parameters: none
 * Return: Boolean - true cart present
 *                 - false cart no present
 *
 * Description: Mmc present check
 *
 *************************************************************************/
inline
Boolean MmcPresent (void)
{
  return(GPIO_ReadBit(GPIO2,GPIO_Pin_3) == Bit_RESET);
}

/*************************************************************************
 * Function Name: MmcWriteProtect
 * Parameters: none
 * Return: Boolean - true cart is protected
 *                 - false cart no protected
 *
 * Description: Mmc Write protect check
 *
 *************************************************************************/
inline
Boolean MmcWriteProtect (void)
{
  return(GPIO_ReadBit(GPIO1,GPIO_Pin_14) == Bit_SET);
}

/*************************************************************************
 * Function Name: MmcSetClockFreq
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Set SPI ckl frequency
 *
 *************************************************************************/
Int32U MmcSetClockFreq (Int32U Frequency)
{
Int32U Div = 2;
MRCC_ClocksTypeDef MRCC_ClocksStatus;

  MRCC_GetClocksStatus(&MRCC_ClocksStatus);

  // Check min clk divider (for master mode >= 2)
  while(Frequency * Div <= MRCC_ClocksStatus.PCLK_Frequency)
  {
    Div += 2;
  }

  // Check max clk divider <= 254
  if (Div > 254)
  {
    Div = 254;
  }
  SSP1->PR = Div;

  // Return real frequency
  return(MRCC_ClocksStatus.PCLK_Frequency/Div);
}

/*************************************************************************
 * Function Name: MmcInit
 * Parameters: none
 * Return: none
 *
 * Description: Init SPI, Cart Present, Write Protect and Chip select pins
 *
 *************************************************************************/
void MmcInit (void)
{
SSP_InitTypeDef   SSP_InitStructure;
GPIO_InitTypeDef  GPIO_InitStructure;

  // Enable SSP1 and GPIOs clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_SSP1, ENABLE);

  SSP_DeInit(SSP1);

  // Remap
  GPIO_PinRemapConfig(GPIO_Remap_SSP1,DISABLE);

  // Configure SSP1_CLK, SSP1_MOSI, SSP1_nCS1, Card Present and Write Protect pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_16 | GPIO_Pin_17 | GPIO_Pin_18;
  GPIO_Init (GPIO0, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
  GPIO_Init (GPIO0, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_19;			// nCS1
  GPIO_Init (GPIO0, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			// Card present
  GPIO_Init (GPIO2, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;			// Write Protect
  GPIO_Init (GPIO1, &GPIO_InitStructure);

  // Chip select
  MmcChipSelect(0);

  // Spi init
  SSP_StructInit(&SSP_InitStructure);

  SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
  SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
  SSP_InitStructure.SSP_CPOL = SSP_CPOL_Low;
  SSP_InitStructure.SSP_CPHA = SSP_CPHA_1Edge;
  SSP_InitStructure.SSP_DataSize = SSP_DataSize_8b;
  SSP_InitStructure.SSP_ClockRate = 0;
  SSP_InitStructure.SSP_ClockPrescaler = 2;
//  SSP_InitStructure.SSP_NSS = SSP_NSS_Soft;
  SSP_InitStructure.SSP_NSS = SSP_NSS_Hard;
  SSP_Init(SSP1, &SSP_InitStructure);


  // In master mode, NSS internal must be held high continuously
  SSP_NSSInternalConfig(SSP1,SSP_NSSInternal_Set);

  // SSP1 enable
  SSP_Cmd(SSP1, ENABLE);

  // Clock Freq. Identification Mode < 400kHz
  MmcSetClockFreq(IdentificationModeClock);

}

/*************************************************************************
 * Function Name: MmcTranserByte
 * Parameters: Int8U ch
 * Return: Int8U
 *
 * Description: Transfer byte on the SPI
 *
 *************************************************************************/
Int8U MmcTranserByte (Int8U ch)
{
  SSP_SendData(SSP1, ch);
  while(SSP_GetFlagStatus(SSP1, SSP_FLAG_RxFifoNotEmpty) == RESET);
  return(SSP_ReceiveData(SSP1));
}

/*************************************************************************
 * Function Name: MmcSendBlock
 * Parameters: pInt8U pData, Int32U Size
 *
 * Return: void
 *
 * Description: Send block on the SPI
 *
 *************************************************************************/
void MmcSendBlock (pInt8U pData, Int32U Size)
{
Int32U OutCount = Size;
  while (OutCount--)
  {
    SSP_SendData(SSP1, *pData++);
    while(SSP_GetFlagStatus(SSP1, SSP_FLAG_RxFifoNotEmpty)==RESET);
    volatile Int32U Dummy = SSP_ReceiveData(SSP1);
  }
}

/*************************************************************************
 * Function Name: MmcReceiveBlock
 * Parameters: pInt8U pData, Int32U Size
 *
 * Return: void
 *
 * Description: Receive block on the SPI
 *
 *************************************************************************/
void MmcReceiveBlock (pInt8U pData, Int32U Size)
{
Int32U InCount = Size;
  while (InCount--)
  {
    SSP_SendData(SSP1, 0xFF);
    while(SSP_GetFlagStatus(SSP1, SSP_FLAG_RxFifoNotEmpty)==RESET);
    *pData++ = SSP_ReceiveData(SSP1);
  }
}

/*************************************************************************
 * Function Name: MmcDly_1ms
 * Parameters: Int32U Delay
 * Return: none
 *
 * Description: Delay [msec]
 *
 *************************************************************************/
void MmcDly_1ms (Int32U Delay)
{
volatile Int32U i;
  for(;Delay;--Delay)
  {
    for(i = MMC_DLY_1MSEC;i;--i);
  }
}
