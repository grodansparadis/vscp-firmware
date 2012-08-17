/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : STR75x_usb.c
 *    Description : usb module (HAL)
 *
 *    History :
 *    1. Date        : July 28, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : November 20, 2006
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *    Fix problems with double buffered EPs
 *
 *    $Revision: 16170 $
 **************************************************************************/

#define HAL_USB_GLOBAL
#include "STR75x_usb.h"

#pragma section="USB_PACKET_MEMORY"

#pragma location="USB_PACKET_MEMORY"
__root __no_init Int32U USB_PacketMemory[256];

pPacketMemUse_t pPacketMemUse;

PacketMemUse_t PacketMemBuff[EP_MAX_SLOTS*2];

EpInfo_t EpInfo[ENP_MAX_NUMB];

Int32U DlyCnt;

UserFunc_t UsbUserFun [UsbLastEvent] =
{
  // Ep 0 Out
  NULL,
  // Ep 0 In
  NULL,
  // Ep 1 Out
  NULL,
  // Ep 1 In
  NULL,
  // Ep 2 Out
  NULL,
  // Ep 2 Int
  NULL,
  // Ep 3 Out
  NULL,
  // Ep 3 In
  NULL,
  // Ep 4 Out
  NULL,
  // Ep 4 In
  NULL,
  // Ep 5 Out
  NULL,
  // Ep 5 In
  NULL,
  // Ep 6 Out
  NULL,
  // Ep 6 In
  NULL,
  // Ep 7 Out
  NULL,
  // Ep 7 In
  NULL,
  // Ep 8 Out
  NULL,
  // Ep 8 In
  NULL,
  // Ep 9 Out
  NULL,
  // Ep 9 In
  NULL,
  // Ep 10 Out
  NULL,
  // Ep 10 In
  NULL,
  // Ep 11 Out
  NULL,
  // Ep 11 In
  NULL,
  // Ep 12 Out
  NULL,
  // Ep 12 In
  NULL,
  // Ep 13 Out
  NULL,
  // Ep 13 In
  NULL,
  // Ep 14 Out
  NULL,
  // Ep 14 In
  NULL,
  // Ep 15 Out
  NULL,
  // Ep 15 In
  NULL,
  // UsbClassEp0OutPacket
  NULL,
  // UsbVendorEp0OutPacket
  NULL,
  // UsbUserSofEvent
  NULL,
  // UsbUserClass
  NULL,
  // UsbUserVendor
  NULL,
  // UsbUserGetDescriptor
  NULL,
  // UsbUserConfigure
  NULL,
  // UsbUserReset
  NULL,
  // UsbUserConnect
  NULL,
  // UsbUserSuspend
  NULL,
  // UsbUserEpStall
  NULL,
  // UsbUserEpUnStall
  NULL,
};

UsbEpCtrl_t UsbEp0Ctrl;

UsbSetupPacket_t UsbEp0SetupPacket;

#pragma data_alignment=4
Int8U EpCtrlDataBuf[Ep0MaxSize];

UsbDevCtrl_t UsbDevCtrl = {{UsbDevStatusUnknow,UsbDevStatusNoSuspend},0,0,0,0};

CommUserFpnt_t UsbCoreT9Fun;

#include "usb_common.c"

/*************************************************************************
 * Function Name: EpCtrlSet_EA
 * Parameters: volatile pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP Addr
 *
 *************************************************************************/
void EpCtrlSet_EA(volatile pInt32U pReg, Int32U Data)
{
  Data &= 0xF;
  Data |= *pReg & 0x0700 ;
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EA
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int16U
 *
 * Description: Get EP Addr
 *
 *************************************************************************/
inline
Int16U EpCtrlGet_EA(volatile pInt32U pReg)
{
  return(*pReg & 0x0F);
}

/*************************************************************************
 * Function Name: EpCtrlSet_STAT_TX
 * Parameters: volatile pInt32U pReg, EpState_t Data
 *
 * Return: none
 *
 * Description: Set EP IN State
 *
 *************************************************************************/
void EpCtrlSet_STAT_TX (volatile pInt32U pReg, EpState_t Data)
{
Int32U State;
  // The EP can change state between read and write
  // operations from VALID to NAK and result of
  // set operation will be invalid!
  if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 4;
    State ^= *pReg;
    State |= USB_CTRL_CLEAR_ONLY_MASK;
    State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
    *pReg = State;
  }
  else
  {
    return;
  }
  if(EpCtrlGet_STAT_TX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 4;
    State ^= *pReg;
    State |= USB_CTRL_CLEAR_ONLY_MASK;
    State &= ~0x7040;  // Clear the toggle bits without STAT_TX (4,5)
    *pReg = State;
  }
}

/*************************************************************************
 * Function Name: EpCtrlGet_STAT_TX
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP IN Status
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_STAT_TX (volatile pInt32U pReg)
{
Int32U Data = *pReg;
  return((Data & (0x3UL << 4)) >> 4);
}

/*************************************************************************
 * Function Name: EpCtrlSet_DTOG_TX
 * Parameters: volatile pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP DTOG_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlSet_DTOG_TX(volatile pInt32U pReg, Int32U Data)
{
  Data  = Data?(*pReg ^ (1UL<<6)):*pReg;
  Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlToggle_DTOG_TX
 * Parameters: volatile pInt32U pReg
 *
 * Return: none
 *
 * Description: Toggle EP DTOG_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlToggle_DTOG_TX(volatile pInt32U pReg)
{
Int32U Data = *pReg | (1UL << 6);
  Data &= ~0x7030;   // Clear the toggle bits without DTOG_TX (6)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_DTOG_TX
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP DTOG_TX bit (IN)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_DTOG_TX(volatile pInt32U pReg)
{
  return(((*pReg & (1UL<<6))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlClr_CTR_TX
 * Parameters: volatile pInt32U pReg
 *
 * Return: none
 *
 * Description: Clear EP CTR_TX bit (IN)
 *
 *************************************************************************/
void EpCtrlClr_CTR_TX(volatile pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 7);
  Data |= 1UL <<15;   // Set RX_CTR
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_CTR_TX
 * Parameters: volatile pInt32U pReg
 *
 * Return: UserFunc_t
 *
 * Description: Get EP CTR_TX bit (IN)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_CTR_TX(volatile pInt32U pReg)
{
  return(((*pReg & (0x1UL << 7))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlSet_EP_KIND
 * Parameters: volatile pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set EP KIND bit
 *
 *************************************************************************/
void EpCtrlSet_EP_KIND (volatile pInt32U pReg, Int32U Data)
{
  Data  = Data ? (1UL << 8):0;
  Data |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (1UL << 8));
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EP_KIND
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP KIND bit
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_EP_KIND(volatile pInt32U pReg)
{
  return(*pReg & (0x1UL << 8)?1:0);
}

/*************************************************************************
 * Function Name: EpCtrlSet_EP_TYPE
 * Parameters: volatile pInt32U pReg, EpType_t Data
 *
 * Return: none
 *
 * Description: Set EP type
 *
 *************************************************************************/
void EpCtrlSet_EP_TYPE(volatile pInt32U pReg, EpType_t Data)
{
Int32U Type;
  Type = Data;
  Type <<= 9;
  Type  |= *pReg & ~(USB_CTRL_TOGGLE_MASK | (0x3UL << 9));
  Type  |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Type;
}

/*************************************************************************
 * Function Name: EpCtrlGet_EP_TYPE
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP type
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_EP_TYPE(volatile pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= (3UL<<9);
  return(Data >> 9);
}

/*************************************************************************
 * Function Name: EpCtrlGet_SETUP
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP SETUP bit
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_SETUP(volatile pInt32U pReg)
{
Int32U Data = *pReg & (0x1UL << 11);
  return(Data?1:0);
}

/*************************************************************************
 * Function Name: EpCtrlSet_STAT_RX
 * Parameters: volatile pInt32U pReg, EpState_t Data
 *
 * Return: none
 *
 * Description: Set EP STAT_RX (OUT)
 *
 *************************************************************************/
void EpCtrlSet_STAT_RX (volatile pInt32U pReg, EpState_t Data)
{
Int32U State;
  // The EP can change state between read and write
  // operations from VALID to NAK and result of
  // set operation will be invalid!
  if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 12;
    State  ^= *pReg;
    State  |= USB_CTRL_CLEAR_ONLY_MASK;
    State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
    *pReg  = State;
  }
  else
  {
    return;
  }
  if(EpCtrlGet_STAT_RX(pReg) != (Int32U)Data)
  {
    State = Data;
    State <<= 12;
    State  ^= *pReg;
    State  |= USB_CTRL_CLEAR_ONLY_MASK;
    State  &= ~0x4070;  // Clear the toggle bits without STAT_RX (12,13)
    *pReg  = State;
  }
}

/*************************************************************************
 * Function Name: EpCtrlGet_STAT_RX
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP STAT_RX (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_STAT_RX(volatile pInt32U pReg)
{
Int32U Data = *pReg &( 0x3UL << 12);
  return(Data >> 12);
}

/*************************************************************************
 * Function Name: EpCtrlSet_DTOG_RX
 * Parameters: volatile pInt32U pReg, Int32U Data
 *
 * Return: none
 *
 * Description: Set DTOG_RX bit
 *
 *************************************************************************/
void EpCtrlSet_DTOG_RX(volatile pInt32U pReg, Int32U Data)
{
  Data  = Data?(*pReg ^ (1UL<<14)):*pReg;
  Data &= ~0x3070;   // Clear the toggle bits without DTOG_RX (14)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlToggle_DTOG_RX
 * Parameters: volatile Int16U pReg
 *
 * Return: none
 *
 * Description: Toggle DTOG_RX bit
 *
 *************************************************************************/
void EpCtrlToggle_DTOG_RX(volatile pInt32U pReg)
{
Int32U Data = *pReg | (1UL << 14);
  Data &= ~0x3070;   // Clear the toggle bits without DTOG_TX (14)
  Data |= USB_CTRL_CLEAR_ONLY_MASK;
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_DTOG_RX
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP DTOG_RX bit (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_DTOG_RX(volatile pInt32U pReg)
{
  return(((*pReg & (1UL<<14))?1:0));
}

/*************************************************************************
 * Function Name: EpCtrlClr_CTR_RX
 * Parameters: volatile pInt32U pReg
 *
 * Return: none
 *
 * Description: Clear EP CTR_RX bit (OUT)
 *
 *************************************************************************/
void EpCtrlClr_CTR_RX(volatile pInt32U pReg)
{
Int32U Data = *pReg;
  Data &= ~(USB_CTRL_TOGGLE_MASK | 1UL << 15);
  Data |= 1UL << 7;   // Set TX_CTR
  *pReg = Data;
}

/*************************************************************************
 * Function Name: EpCtrlGet_CTR_RX
 * Parameters: volatile pInt32U pReg
 *
 * Return: Int32U
 *
 * Description: Get EP CTR_RX bit (OUT)
 *
 *************************************************************************/
inline
Int32U EpCtrlGet_CTR_RX(volatile pInt32U pReg)
{
  return(*pReg & (0x1UL << 15)?1:0);
}

/*************************************************************************
 * Function Name: USB_Init
 * Parameters: Int32U IntrSlotHi, Int32U IntrSlotLo,
 *             CommUserFpnt_t UserCoreConfigure
 *
 * Return: none
 *
 * Description: Init USB
 *
 *************************************************************************/
void USB_Init(Int32U IntrSlotHi, Int32U IntrSlotLo,
              CommUserFpnt_t UserCoreConfigure)
{
EIC_IRQInitTypeDef EIC_IRQInitStructure;

  // Init variables
  UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
  UsbCoreT9Fun = NULL;
  UserCoreConfigure(NULL);
  USB_UserFuncRegistering(UserCoreConfigure,UsbUserConfigure);
  UsbSetDevState(UsbDevStatusUnknow);

  // Reset USB Engine
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_USB,ENABLE);
  // Init USB Clock
  MRCC_PeripheralClockConfig(MRCC_Peripheral_USB,ENABLE);
  // Release reset USB Engine
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_USB,DISABLE);

  // Enable 48MHz clock
#if USB_EXT_CLK > 0
GPIO_InitTypeDef  GPIO_InitStructure;
  // Enable GPIO clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
  // Release GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_GPIO,DISABLE);
  // Assign the P0.19 for a clock input of the USB engine
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = 1UL<<19;
  GPIO_Init(GPIO0,&GPIO_InitStructure);
  MRCC_CKUSBConfig(MRCC_CKUSB_External);
#else
  MRCC_CKUSBConfig(MRCC_CKUSB_Internal);
#endif // USB_EXT_CLK > 0

  // Force USB Reset &  Disable USB interrupts
  USB_CNTR = 1;

  // USB interrupt connect to VIC
#if USB_HIGH_PRIORITY_EVENT > 0
  EIC_IRQInitStructure.EIC_IRQChannel = USB_HP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrSlotHi;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
#endif // USB_HIGH_PRIORITY_EVENT > 0

  EIC_IRQInitStructure.EIC_IRQChannel = USB_LP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrSlotLo;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  // Disconnect device
  USB_ConnectRes(FALSE);
  // Init controls endpoints
  USB_HwReset();
  // Clear spurious pending interrupt
  USB_ISTR = 0;
  // Enable Device interrupts
  USB_CNTR = bmCTRM | bmRESETM | bmSUSPM | bmWKUPM |
         (USB_SOF_EVENT   ? bmSOFM  : 0) |
         (USB_ERROR_EVENT ? bmERRM  : 0) |
         ( USB_DOVR_EVENT ? bmDOVRM : 0) ;
}

/*************************************************************************
 * Function Name: USB_HwReset
 * Parameters: none
 *
 * Return: none
 *
 * Description: Reset Usb engine
 *
 *************************************************************************/
void USB_HwReset (void)
{
Int32U Count;

  for(Count = 0; Count < ENP_MAX_NUMB; Count++)
  {
    EpInfo[Count].pEpCtrl = NULL;
  }
  // Free entire USB packet memory
  for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
  {
    PacketMemBuff[Count].Size = 0;
  }

  // Resume delay counter
  DlyCnt = 0;

  // Filter disabled
  CFG_USBFilterConfig(CFG_USBFilter_Disable);
  // Clear USB Reset
  USB_CNTR_bit.FRES   = 0;
  // Exit from suspend
  USB_CNTR_bit.ESOFM  = 0;
  USB_CNTR_bit.RESUME = 0;
  USB_CNTR_bit.LPMODE = 0;
  USB_CNTR_bit.FSUSP  = 0;  // Must be 0
  // USB_Configure
  USB_Configure(FALSE);
  // Set Buffer table address
  USB_BTABLE = USB_OffsetOfDBT;
  // Init Packet memory table
  pPacketMemUse = NULL;
  // Endpoint Init
  USB_UserFuncRegistering((UserFunc_t)UsbCtrlEp,UsbEp0Out);
  USB_UserFuncRegistering((UserFunc_t)UsbCtrlEp,UsbEp0In);
  USB_RealizeEp(CTRL_ENP_IN,Ep0MaxSize,EP_CTRL,EP_SLOT0);
  // Set address 0
  USB_SetDefAdd();
}

/*************************************************************************
 * Function Name: UsbMalloc
 * Parameters: none
 *
 * Return: void *
 *
 * Description: Return address of free element of the PacketMemBuff massive
 *
 *************************************************************************/
static
void * UsbMalloc(void)
{
Int32U Count;
  for (Count = 0; Count < EP_MAX_SLOTS*2; Count++)
  {
    if (PacketMemBuff[Count].Size == 0)
    {
      return(&PacketMemBuff[Count]);
    }
  }
  return(NULL);
}

/*************************************************************************
 * Function Name: UsbFree
 * Parameters: pPacketMemUse_t pPntr
 *
 * Return: none
 *
 * Description: Free element of the PacketMemBuff massive
 *
 *************************************************************************/
inline static
void UsbFree(pPacketMemUse_t pPntr)
{
  pPntr->Size = 0;
}

/*************************************************************************
 * Function Name: USB_AllocateBuffer
 * Parameters: pInt16U pOffset,Int32U PacketSize,
 *             USB_Endpoint_t EndPoint
 *
 * Return: Boolean
 *
 * Description: Allocate a new buffer
 *
 *************************************************************************/
static
Boolean USB_AllocateBuffer (pInt16U pOffset,pInt32U pPacketSize,
                            USB_Endpoint_t EndPoint)
{
pPacketMemUse_t  pPacketMem = pPacketMemUse, pPacketMemNext, pPacketMemUseNew;
//Int32U MaxPacketSize = *pPacketSize+4;  // for CRC
Int32U MaxPacketSize = *pPacketSize;
  if(MaxPacketSize > 62)
  {
    // coarse-granularity size alignment by 32
    MaxPacketSize +=  (32-1);
    MaxPacketSize &= ~(32-1);
  }
  else
  {
    // fine-granularity size alignment by 2
    MaxPacketSize +=  (2-1);
    MaxPacketSize &= ~(2-1);
  }
  *pOffset = 0;
  // Finding free memory block from the USB packet memory
  while(pPacketMem != NULL)
  {
    *pOffset = pPacketMem->Start + pPacketMem->Size;
    // Offset alignment by 4
    *pOffset += (4-1);
    *pOffset &= ~(4-1);
    pPacketMemNext = pPacketMem->pNext;
    if((pPacketMem->pNext == NULL) ||
       (pPacketMemNext->Start >= *pOffset + MaxPacketSize))
    {
      break;
    }
    pPacketMem = pPacketMem->pNext;
  }
  // Is block find?
  if((*pOffset + MaxPacketSize) >=
     (((Int32U)__segment_size("USB_PACKET_MEMORY") -
       sizeof(USB_BuffDeskTbl_t) * (MaxIndOfRealizeEp+1))&~7))
  {
    // Not enough space in the USB packet memory
    return(FALSE);
  }

  pPacketMemUseNew = UsbMalloc();
  if(pPacketMemUseNew == NULL)
  {
    // too many point are realized
    return(FALSE);
  }
  if(pPacketMemUse == NULL)
  {
    pPacketMemUse = pPacketMemUseNew;
    pPacketMemUse->pNext = NULL;
  }
  else
  {
    pPacketMemUseNew->pNext  = pPacketMem->pNext;
    pPacketMem->pNext        = pPacketMemUseNew;
  }
  pPacketMemUseNew->RpAddr = EndPoint;
  pPacketMemUseNew->Start  = *pOffset;
  pPacketMemUseNew->Size   = MaxPacketSize;
  *pPacketSize = MaxPacketSize;
  return(TRUE);
}

/*************************************************************************
 * Function Name: USB_ReleaseBuffer
 * Parameters: USB_Endpoint_t EndPoint
 *
 * Return: none
 *
 * Description: Release the buffer/s
 *
 *************************************************************************/
static
void USB_ReleaseBuffer (USB_Endpoint_t EndPoint)
{
pPacketMemUse_t  pPacketMem, pPacketMemPrev;
  pPacketMem = pPacketMemUse;
  while(pPacketMem != NULL)
  {
    if(pPacketMem->RpAddr == EndPoint)
    {
      pPacketMemPrev->pNext = pPacketMem->pNext;
      UsbFree(pPacketMem);
      pPacketMem = pPacketMemPrev->pNext;
    }
    else
    {
      pPacketMemPrev = pPacketMem;
      pPacketMem = pPacketMem->pNext;
    }
    if (pPacketMem == NULL)
    {
      break;
    }
  }
}

/*************************************************************************
 * Function Name: USB_RealizeEp
 * Parameters: USB_Endpoint_t EndPoint, Int32U MaxPacketSize,
 *             EpType_t EpKind, EpSlot_t EpSlot,
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Enable or disable endpoint
 *
 *************************************************************************/
USB_ErrorCodes_t USB_RealizeEp( USB_Endpoint_t EndPoint, Int32U MaxPacketSize,
                                EpType_t EpType, EpSlot_t EpSlot)
{
volatile pInt32U pEpCtrlBaseAddr;
Int16U Offset,Offset1;
Int32U MaxPacketSizeTmp;
  if (MaxPacketSize)
  {
    // Allocate packet memory for EP buffer/s
    // calculate actual size only for the OUT EPs
    MaxPacketSizeTmp = MaxPacketSize;
    if(!USB_AllocateBuffer(&Offset,&MaxPacketSizeTmp,EndPoint))
    {
      return(USB_MEMORY_FULL);
    }
    if((EpType == EP_ISO) || (EpType == EP_BULK_DOUB_BUF) || (EpType == EP_CTRL))
    {
      // Allocate packet second buffer
      MaxPacketSizeTmp = MaxPacketSize;
      if(!USB_AllocateBuffer(&Offset1,&MaxPacketSizeTmp,EndPoint))
      {
        // release buffer
        USB_ReleaseBuffer(EndPoint);
        return(USB_MEMORY_FULL);
      }
    }
    pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
    pEpCtrlBaseAddr += EpSlot;
    // Set Ep Address
    EpCtrlSet_EA(pEpCtrlBaseAddr,EndPoint >> 1);

    if(EpType == EP_CTRL)
    {
      EndPoint |= 1;
    }
  sec_pass: // only for control EP, because this type realize both directions
    EpInfo[EndPoint].EpType  = EpType;
    EpInfo[EndPoint].pEpCtrl = pEpCtrlBaseAddr;
    EpInfo[EndPoint].EpSlot  = EpSlot;
    EpInfo[EndPoint].MaxSize = MaxPacketSize;
    if(EndPoint & 1)
    {
      // IN EP
      // Disable EP
      EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Tx toggle
      EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
      // Clear Correct Transfer for transmission flag
      EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
      // Update EP description table
      WriteEpDTB_AddrTx(EpSlot,Offset);
      WriteEpDTB_CountTx(EpSlot,0);
      // Set EP Kind & enable
      switch(EpType)
      {
      case EP_BULK_SING_BUFF:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        break;
      case EP_CTRL:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        // Realize OUT direction
        EndPoint &= ~1;
        Offset = Offset1;
        goto sec_pass;
      case EP_ISO:
        // Disable receiving (only singe direction is possible)
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
        WriteEpDTB_AddrRx(EpSlot,Offset1);
        WriteEpDTB_CountRx(EpSlot,0);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case EP_INTERRUPT:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
        break;
      default:  // EP_BULK_DOUB_BUF
        // Disable receiving (only singe direction is possible)
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
        // All buffers are empties
        EpInfo[EndPoint].EpIntrFl = 1;
        // Clear Tx Software toggle
        WriteEpDTB_AddrRx(EpSlot,Offset1);
        WriteEpDTB_CountRx(EpSlot,0);
        EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,1);
        // Enable EP
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_NAK);
      }
      // Clear Correct Transfer for transmission flag
      EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
    }
    else
    {
      RxCount_t RxCount = {0};
      // OUT EP
      // Disable EP
      EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Rx toggle
      EpCtrlSet_DTOG_RX(pEpCtrlBaseAddr,0);
      // Clear Correct Transfer for reception flag
      EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
      // Update EP description table
      RxCount.BlSizeField = (MaxPacketSize > 62);
      RxCount.NubBlockField = (MaxPacketSize > 62) ? (MaxPacketSize>>5)-1:MaxPacketSizeTmp>>1;
      WriteEpDTB_AddrRx(EpSlot,Offset);
      WriteEpDTB_CountRx(EpSlot,RxCount.Count);

      // Set EP Kind & enable
      switch(EpType)
      {
      case EP_BULK_SING_BUFF:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case EP_CTRL:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_CTRL);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_NAK);
        break;
      case EP_ISO:
        WriteEpDTB_CountTx(EpSlot,RxCount.Count);
        // Disable transmitting (only singe direction is possible)
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        WriteEpDTB_AddrTx(EpSlot,Offset1);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_ISO);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      case EP_INTERRUPT:
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_INTERRUPT);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,0);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
        break;
      default:  // EP_BULK_DOUB_BUF
        // All buffers are empties
        EpInfo[EndPoint].EpIntrFl = 0;
        // Disable transmitting (only singe direction is possible)
        EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
        WriteEpDTB_CountTx(EpSlot,RxCount.Count);
        WriteEpDTB_AddrTx(EpSlot,Offset1);
        EpCtrlSet_DTOG_TX(pEpCtrlBaseAddr,0);
        EpCtrlSet_EP_TYPE(pEpCtrlBaseAddr,EP_BULK_SING_BUFF);
        EpCtrlSet_EP_KIND(pEpCtrlBaseAddr,1);
        // Enable EP
        EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_VALID);
      }
    }
  }
  else
  {
    pEpCtrlBaseAddr  = (pInt32U)&USB_EP0R;
    pEpCtrlBaseAddr += EpSlot;
    if(EndPoint & 1)
    {
      // Disable IN EP
      EpCtrlSet_STAT_TX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Correct Transfer for reception flag
      EpCtrlClr_CTR_TX(pEpCtrlBaseAddr);
    }
    else
    {
      // Disable OUT EP
      EpCtrlSet_STAT_RX(pEpCtrlBaseAddr,EP_DISABLED);
      // Clear Correct Transfer for reception flag
      EpCtrlClr_CTR_RX(pEpCtrlBaseAddr);
    }
    // release buffer
    USB_ReleaseBuffer(EndPoint);
    EpInfo[EndPoint].pEpCtrl = NULL;
  }
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_SetAdd
 * Parameters: Int32U DevAdd - device address between 0 - 127
 *
 * Return: none
 *
 * Description: Set device address
 *
 *************************************************************************/
void USB_SetAdd(Int32U DevAdd)
{
  USB_DADDR = DevAdd | 0x80;
}

/*************************************************************************
 * Function Name: USB_ConnectRes
 * Parameters: Boolean Conn
 *
 * Return: none
 *
 * Description: Enable Pull-Up resistor
 *
 *************************************************************************/
void USB_ConnectRes (Boolean Conn)
{
}

/*************************************************************************
 * Function Name: USB_Configure
 * Parameters: Boolean Configure
 *
 * Return: none
 *
 * Description: Configure device
 *  When Configure != 0 enable all Realize Ep
 *
 *************************************************************************/
static
void USB_Configure (Boolean Configure)
{
}

/*************************************************************************
 * Function Name: USB_Suspend
 * Parameters: void
 *
 * Return: none
 *
 * Description: Suspend the USB engine
 *
 *************************************************************************/
static
void USB_Suspend(void)
{
  USB_CNTR_bit.FSUSP  = 1;
  USB_CNTR_bit.LPMODE = 1;
  CFG_USBFilterConfig(CFG_USBFilter_Enable);
}

#if USB_REMOTE_WAKEUP != 0
/*************************************************************************
 * Function Name: USB_WakeUp
 * Parameters: none
 *
 * Return: none
 *
 * Description: Wake up Usb
 *
 *************************************************************************/
static
void USB_WakeUp (void)
{
  USB_Resume(USB_RESUME_SOFT_EVENT);
}
#endif // USB_REMOTE_WAKEUP != 0

/*************************************************************************
 * Function Name: USB_Resume
 * Parameters: UsbResumeEvent_t UsbResumeEvent
 *
 * Return: none
 *
 * Description: USB Resume implement
 *
 *************************************************************************/
static
void USB_Resume (UsbResumeEvent_t UsbResumeEvent)
{
Int32U LineStates;
  switch(UsbResumeEvent)
  {
  case USB_RESUME_SOF_EVENT:
    if(DlyCnt)
    {
      // Waiting 8ms (not take more than 10ms) before release signal to host
      if(--DlyCnt == 0)
      {
        USB_CNTR_bit.ESOFM  = 0;
        USB_CNTR_bit.RESUME = 0;
        USB_CNTR_bit.FSUSP  = 0;  // Must be 0
        if(UsbUserFun[UsbUserSuspend] != NULL)
        {
          UsbUserFun[UsbUserSuspend]((void *)0);
        }
      }
    }
    break;
  case USB_RESUME_SOFT_EVENT: // resume by USER
    CFG_USBFilterConfig(CFG_USBFilter_Disable);
    USB_CNTR_bit.LPMODE = 0;
    DlyCnt = USB_RESUME_DLY;
    USB_CNTR_bit.RESUME = 1;
    USB_CNTR_bit.ESOFM  = 1;
    break;
  case USB_RESUME_WAKE_UP_EVENT:  // resume by HOST
    LineStates = USB_FNR & 0xC000;
    if((LineStates == 0x8000) && (LineStates == 0x3000))
    {
      // noise return to suspend again
      USB_Suspend();
    }
    else
    {
      // wake-up
      CFG_USBFilterConfig(CFG_USBFilter_Disable);
      USB_CNTR_bit.LPMODE = 0;
      USB_CNTR_bit.FSUSP  = 0;  // Must be 0
    }
    break;
  }
}

/*************************************************************************
 * Function Name: USB_GetDevStatus
 * Parameters: USB_DevStatusReqType_t Type
 *
 * Return: Boolean
 *
 * Description: Return USB device status
 *
 *************************************************************************/
Boolean USB_GetDevStatus (USB_DevStatusReqType_t Type)
{
  switch (Type)
  {
  case USB_DevConnectStatus:
    return(TRUE);
  case USB_SuspendStatus:
    return(USB_CNTR_bit.FSUSP);
  case USB_ResetStatus:
    return(USB_CNTR_bit.FRES);
  }
  return(FALSE);
}

/*************************************************************************
 * Function Name: USB_SetStallEP
 * Parameters: USB_Endpoint_t EndPoint, Boolean Stall
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: The endpoint stall/unstall
 *
 *************************************************************************/
USB_ErrorCodes_t USB_SetStallEP (USB_Endpoint_t EndPoint, Boolean Stall)
{
  if( EpInfo[EndPoint].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }

  if(Stall)
  {
    if(EndPoint & 1)
    {
      // IN EP
      EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_STALL);
    }
    else
    {
      // OUT EP
      EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_STALL);
    }
  }
  else if(EndPoint & 1)
  {
    // IN EP
    // reset Data Toggle bit
    switch(EpInfo[EndPoint].EpType)
    {
    case EP_BULK_DOUB_BUF:
      EpInfo[EndPoint].EpIntrFl = 1;
      EpCtrlSet_DTOG_RX(EpInfo[EndPoint].pEpCtrl,0);
    case EP_INTERRUPT:
    case EP_ISO:
    case EP_BULK_SING_BUFF:
      EpCtrlSet_DTOG_TX(EpInfo[EndPoint].pEpCtrl,0);
    }
    EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
  }
  else
  {
    // OUT EP
    // reset Data Toggle bit
    switch(EpInfo[EndPoint].EpType)
    {
    case EP_BULK_DOUB_BUF:
      EpInfo[EndPoint].EpIntrFl = 0;
      EpCtrlSet_DTOG_TX(EpInfo[EndPoint].pEpCtrl,0);
    case EP_INTERRUPT:
    case EP_ISO:
    case EP_BULK_SING_BUFF:
      EpCtrlSet_DTOG_RX(EpInfo[EndPoint].pEpCtrl,0);
    }
    EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
  }
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_GetStallEP
 * Parameters: USB_Endpoint_t EndPoint, pBoolean pStall
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Get stall state of the endpoint
 *
 *************************************************************************/
static
USB_ErrorCodes_t USB_GetStallEP (USB_Endpoint_t EndPoint, pBoolean pStall)
{
  if(EpInfo[EndPoint].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }
  *pStall = (EndPoint & 1)?
            (EpCtrlGet_STAT_TX(EpInfo[EndPoint].pEpCtrl) == EP_STALL):  // IN EP
            (EpCtrlGet_STAT_RX(EpInfo[EndPoint].pEpCtrl) == EP_STALL);  // OUT EP
  return (USB_OK);
}

/*************************************************************************
 * Function Name: USB_EpValidate
 * Parameters: USB_Endpoint_t EndPoint, Boolean State
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Validate/Unvalidate EP buffer
 *
 *************************************************************************/
USB_ErrorCodes_t USB_EpValidate( USB_Endpoint_t EndPoint, Boolean State)
{
  if( EpInfo[EndPoint].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }

  if(EndPoint & 1)
  {
  // IN EP (Tx)
    if (State)
    {
      // Validate buffer
       EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
    }
    else
    {
      // Unvalidate buffer
      EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
      EpCtrlClr_CTR_TX(EpInfo[EndPoint].pEpCtrl);
    }
  }
  else
  {
  // OUT EP (Rx)
    if (State)
    {
      // Validate buffer
      EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
    }
    else
    {
      // Unvalidate buffer
      EpCtrlSet_STAT_RX(EpInfo[EndPoint].pEpCtrl,EP_NAK);
      EpCtrlClr_CTR_RX(EpInfo[EndPoint].pEpCtrl);
    }
  }
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_EpWrite
 * Parameters: USB_Endpoint_t EndPoint, pInt8U pData, pInt32U pCount
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Endpoint Write (IN)
 *
 *************************************************************************/
USB_ErrorCodes_t USB_EpWrite (USB_Endpoint_t EndPoint, pInt8U pData,
                              pInt32U pCount)
{
__usb_epr_bits EpState;
pInt32U pDst;
Int32U  ActSize, Temp, Count, CurrSize, SizeHold;

  ActSize = *pCount;
  *pCount = 0;
  if(EpInfo[EndPoint].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }

  EpState = *(__usb_epr_bits *)EpInfo[EndPoint].pEpCtrl;

  if(EpState.STATTX == EP_DISABLED)
  {
    // Fatal error (require restart of the USB)
    return(USB_EP_FATAL_ERROR);
  }

  switch(EpInfo[EndPoint].EpType)
  {
  case EP_INTERRUPT:
    if (EpState.STATTX == EP_STALL)
    {
      // EP stalled
      return(USB_EP_STALLED);
    }
    else if(EpState.STATTX == EP_VALID)
    {
      // Data sending in progress
      return(USB_OK);
    }
    // Get is smaller of user buffer and received data.
    SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
    WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
    pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
    for (Count = 0; Count < SizeHold; ++Count)
    {
      Temp  = *pData++;
      if (++Count < SizeHold)
      {
        Temp |= (*pData++) << 8;
      }
      *pDst++ = Temp;
    }
    // Validate buffer
    EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
    break;
  case EP_CTRL:
    if(EpState.STATTX == EP_VALID)
    {
      // Data sending in progress
      return(USB_OK);
    }
    // Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
    WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
    pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
    for (Count = 0; Count < SizeHold; ++Count)
    {
      Temp  = *pData++;
      if (++Count < SizeHold)
      {
        Temp |= (*pData++) << 8;
      }
      *pDst++ = Temp;
    }
    // Validate buffer
    EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
    break;
  case EP_ISO:
    if(EpState.DTOGTX)
    {
      // Tx Buffer
      pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
      // Get smaller of the user buffer and the received data.
      SizeHold = MIN(EpInfo[EndPoint].MaxSize,SizeHold);
      for (Count = 0; Count < SizeHold; ++Count)
      {
        Temp  = *pData++;
        if (++Count < SizeHold)
        {
          Temp |= (*pData++) << 8;
        }
        *pDst++ = Temp;
      }
      WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);
    }
    else
    {
      // Rx Buffer
      pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
      // Get smaller of the user buffer and the received data.
      SizeHold = MIN(EpInfo[EndPoint].MaxSize,SizeHold);
      for (Count = 0; Count < SizeHold; ++Count)
      {
        Temp  = *pData++;
        if (++Count < SizeHold)
        {
          Temp |= (*pData++) << 8;
        }
        *pDst++ = Temp;
      }
      WriteEpDTB_CountRx(EpInfo[EndPoint].EpSlot,SizeHold);
    }
    break;
  case EP_BULK_SING_BUFF:
    if (EpState.STATTX == EP_STALL)
    {
      // EP stalled
      return(USB_EP_STALLED);
    }
    else if(EpState.STATTX == EP_VALID)
    {
      // Data sending in progress
      return(USB_OK);
    }
    /// Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpInfo[EndPoint].MaxSize,ActSize);
    WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,SizeHold);

    pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
    for (Count = 0; Count < SizeHold; ++Count)
    {
      Temp  = *pData++;
      if (++Count < SizeHold)
      {
        Temp |= (*pData++) << 8;
      }
      *pDst++ = Temp;
    }
    // Validate buffer
    EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
    break;
  case EP_BULK_DOUB_BUF:
    // Double buffered
    if (EpState.STATTX == EP_STALL)
    {
      // EP stalled
      return(USB_EP_STALLED);
    }

    SizeHold = 0;
    do
    {
      // Check for a pending TX interrupt
      if(!EpCtrlGet_CTR_TX(EpInfo[EndPoint].pEpCtrl) &&
         !EpInfo[EndPoint].EpIntrFl &&
         (EpState.DTOGRX == EpCtrlGet_DTOG_TX(EpInfo[EndPoint].pEpCtrl)))
      {
        // All buffers are filled
        break;
      }

      // Get smaller of the rest of the user buffer and the received data.
      CurrSize = MIN(EpInfo[EndPoint].MaxSize,(ActSize-SizeHold));

      if(!EpState.DTOGRX)
      {
        // Tx Buffer
        pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
        WriteEpDTB_CountTx(EpInfo[EndPoint].EpSlot,CurrSize);
      }
      else
      {
        // Rx Buffer
        pDst = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
        WriteEpDTB_CountRx(EpInfo[EndPoint].EpSlot,CurrSize);
      }

      for (Count = 0; Count < CurrSize; ++Count)
      {
        Temp  = *pData++;
        if (++Count < CurrSize)
        {
          Temp |= (*pData++) << 8;
        }
        *pDst++ = Temp;
      }

      EpInfo[EndPoint].EpIntrFl = 0;
      // Clear for pending TX interrupt
      EpCtrlClr_CTR_TX(EpInfo[EndPoint].pEpCtrl);
      EpCtrlToggle_DTOG_RX(EpInfo[EndPoint].pEpCtrl);
      // Update RX toggle status
      EpState.DTOGRX ^= 1;
      EpCtrlSet_STAT_TX(EpInfo[EndPoint].pEpCtrl,EP_VALID);
      SizeHold += CurrSize;
    } while(SizeHold < ActSize);
    break;
  default:
    // Fatal error (require restart of the USB)
    return(USB_EP_FATAL_ERROR);
  }

  *pCount = SizeHold;
  return(USB_OK);
}

/*************************************************************************
 * Function Name: USB_EpRead
 * Parameters: USB_Endpoint_t EndPoint, pInt8U pData, pInt32U pCount
 *
 * Return: USB_ErrorCodes_t
 *
 * Description: Endpoint Read (OUT)
 *
 *************************************************************************/
USB_ErrorCodes_t USB_EpRead (USB_Endpoint_t EndPoint, pInt8U pData,
                             pInt32U pCount)
{
volatile pInt32U pSrc;
Int32U ActSize = *pCount, EpCount;
__usb_epr_bits EpState;
Int32U Temp;
Int32U CurrSize,SizeHold;
Int32U Count;

  *pCount = 0;

  if(EpInfo[EndPoint].pEpCtrl == NULL)
  {
    return(USB_EP_NOT_VALID);
  }

  EpState = *(__usb_epr_bits *)EpInfo[EndPoint].pEpCtrl;
  if(EpState.STATRX == EP_DISABLED)
  {
    return(USB_EP_FATAL_ERROR);
  }

  switch (EpInfo[EndPoint].EpType)
  {
  case EP_CTRL:
    if(EpState.STATRX == EP_VALID)
    {
      // Data is not received yet.
      return(UB_EP_SETUP_UNDERRUN);
    }
    // Get received bytes number
    EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
    // Get address of the USB packet buffer for corresponding EP
    pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
    // Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpCount,ActSize);

    for(Count = 0; Count < SizeHold; ++Count)
    {
      Temp = *pSrc++;
      *pData++ = Temp;
      if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
      {
        *pData++ = Temp>>8;
      }
    }
    *pCount = SizeHold;
    if(EpCtrlGet_SETUP(EpInfo[EndPoint].pEpCtrl))
    {
      return(UB_EP_SETUP_OVERWRITE);
    }

    break;
  case EP_ISO:
    if(EpState.DTOGRX)
    {
      EpCount = ReadEpDTB_CountTx(EpInfo[EndPoint].EpSlot) & 0x3FF;
      pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
    }
    else
    {
      EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
      pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
    }
    // Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpCount,ActSize);

    for(Count = 0; Count < SizeHold; ++Count)
    {
      Temp = *pSrc++;
      *pData++ = Temp;
      if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
      {
        *pData++ = Temp>>8;
      }
    }
    break;
  case EP_INTERRUPT:
    if (EpState.STATRX == EP_STALL)
    {
      return(USB_EP_STALLED);
    }
    else if (EpState.STATRX == EP_VALID)
    {
      return(UB_EP_SETUP_UNDERRUN);
    }
    EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
    pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
    // Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpCount,ActSize);

    for(Count = 0; Count < SizeHold; ++Count)
    {
      Temp = *pSrc++;
      *pData++ = Temp;
      if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
      {
        *pData++ = Temp>>8;
      }
    }
    break;
  case EP_BULK_SING_BUFF:
    // Single buffer
    if (EpState.STATRX == EP_STALL)
    {
      return(USB_EP_STALLED);
    }
    else if (EpState.STATRX == EP_VALID)
    {
      return(UB_EP_SETUP_UNDERRUN);
    }
    EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
    pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
    // Get smaller of the user buffer and the received data.
    SizeHold = MIN(EpCount,ActSize);

    for(Count = 0; Count < SizeHold; ++Count)
    {
      Temp = *pSrc++;
      *pData++ = Temp;
      if(++Count <= SizeHold) // fix overwrite problem with odd number of bytes
      {
        *pData++ = Temp>>8;
      }
    }
    break;
  case EP_BULK_DOUB_BUF:
    if (EpState.STATRX == EP_STALL)
    {
      return(USB_EP_STALLED);
    }
    SizeHold = 0;
    do
    {
      if(!EpInfo[EndPoint].EpIntrFl)
      {
        // Buffers are empties
        break;
      }
      if(!EpState.DTOGTX)
      {
        EpCount = ReadEpDTB_CountTx(EpInfo[EndPoint].EpSlot) & 0x3FF;
        pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrTx(EpInfo[EndPoint].EpSlot));
      }
      else
      {
        EpCount = ReadEpDTB_CountRx(EpInfo[EndPoint].EpSlot) & 0x3FF;
        pSrc = (pInt32U)__Offset2Addr_PackMem(ReadEpDTB_AddrRx(EpInfo[EndPoint].EpSlot));
      }

      // the buffer size must be enough bigger to get all available data from
      // the EP in other case remaining data will be lost
      if(EpCount > (ActSize-SizeHold))
      {
        *pCount = SizeHold;
        return (USB_BUF_OVERFLOW);
      }
      CurrSize = EpCount;
      for(Count = 0; Count < CurrSize; ++Count)
      {
        Temp = *pSrc++;
        *pData++ = Temp;
        if(++Count <= CurrSize) // fix overwrite problem with odd number of bytes
        {
          *pData++ = Temp>>8;
        }
      }
      EpCtrlToggle_DTOG_TX(EpInfo[EndPoint].pEpCtrl);
      EpState.DTOGTX ^= 1;
      SizeHold += CurrSize;
      --EpInfo[EndPoint].EpIntrFl;
    } while(SizeHold < ActSize);
    break;
  default:
    return(USB_EP_FATAL_ERROR);
  }
  *pCount = SizeHold;
  return (USB_OK);
}

/*************************************************************************
 * Function Name: USB_EpLogToPhysAdd
 * Parameters: Int8U EpLogAdd
 *
 * Return: USB_Endpoint_t
 *
 * Description: Convert the logical to physical address
 *
 *************************************************************************/
USB_Endpoint_t USB_EpLogToPhysAdd (Int8U EpLogAdd)
{
USB_Endpoint_t Address = (USB_Endpoint_t)((EpLogAdd & 0x0F)<<1);
  if(EpLogAdd & 0x80)
  {
    ++Address;
  }
  return(Address);
}

#if USB_SOF_EVENT > 0
/*************************************************************************
 * Function Name: USB_GetFrameNumb
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return current value of SOF number
 *
 *************************************************************************/
Int32U USB_GetFrameNumb (void)
{
  return(USB_FNR_bit.FN);
}
#endif // USB_SOF_EVENT > 0

#if USB_HIGH_PRIORITY_EVENT > 0
/*************************************************************************
 * Function Name: USB_HIGH_ISR
 * Parameters: none
 *
 * Return: none
 *
 * Description: High priority USB interrupt subroutine
 *
 *************************************************************************/
void USB_HIGH_ISR (void)
{
IntrStatus_t IntrStatus;
Int32U EpPhAddr;
Int32U Status;
  IntrStatus.Status  = USB_ISTR;
  IntrStatus.Status &= USB_CNTR | 0x1F;
  // Get masked interrupt flags
  if(IntrStatus.CTR)
  {
    // find corresponding EP
    pInt32U pReg = (pInt32U)&USB_EP0R;
    pReg += IntrStatus.EP_ID;
    EpPhAddr = ((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1);
    if(EpInfo[EpPhAddr].EpType == EP_BULK_DOUB_BUF)
    {
      EpInfo[EpPhAddr].EpIntrFl = 1;
    }
    if(IntrStatus.DIR == 1)
    {
      // OUT EP
      EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
      Status = EpCtrlGet_DTOG_TX(EpInfo[EpPhAddr].pEpCtrl);
      if(Status == EpCtrlGet_DTOG_RX(EpInfo[EpPhAddr].pEpCtrl))
      {
        // The both buffers are filled
        ++EpInfo[EpPhAddr].EpIntrFl;
      }
      // Some times USB engine receive second packet
      // after resetting of interrupt flag and before testing of
      // toggle bits
      if(EpCtrlGet_CTR_RX(EpInfo[EpPhAddr].pEpCtrl))
      {
        // Now both buffers are filled.
        EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        EpInfo[EpPhAddr].EpIntrFl = 2;
      }
      if(UsbUserFun[EpPhAddr] != NULL)
      {
        UsbUserFun[EpPhAddr]((void *)UsbDataOutPacket);
      }
    }
    else
    {
      // IN EP
      EpCtrlClr_CTR_TX(EpInfo[EpPhAddr].pEpCtrl);
      Status = EpCtrlGet_DTOG_RX(EpInfo[EpPhAddr].pEpCtrl);
      if(Status == EpCtrlGet_DTOG_TX(EpInfo[EpPhAddr].pEpCtrl))
      {
        // The both buffers are drained
        ++EpInfo[EpPhAddr].EpIntrFl;
      }
      if(UsbUserFun[EpPhAddr] != NULL)
      {
        UsbUserFun[EpPhAddr]((void *)UsbDataInPacket);
      }
    }
  }
}
#endif // USB_HIGH_PRIORITY_EVENT > 0

/*************************************************************************
 * Function Name: USB_ISR
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB interrupt subroutine
 *
 *************************************************************************/
void USB_ISR (void)
{
IntrStatus_t IntrStatus;
Int32U EpPhAddr;
Boolean SetupPacket;

  // Get masked interrupt flags
  IntrStatus.Status  = USB_ISTR;
  IntrStatus.Status &= USB_CNTR | 0x1F;

#if USB_DOVR_EVENT > 0
  // DMA over / underrun (for speed up retry process)
  if(IntrStatus.DOVR)
  {
    USB_ISTR = ~bmDOVR;
    if(UsbUserFun[UsbDovrEvent] != NULL)
    {
      UsbUserFun[UsbDovrEvent]((void *)Val);
    }
  }
#endif

#if USB_ERROR_EVENT > 0
  // USB engine error interrupt
  if(IntrStatus.ERR)
  {
    USB_ISTR = ~bmERRM;
    if(UsbUserFun[UsbErrorEvent] != NULL)
    {
      UsbUserFun[UsbErrorEvent](NULL);
    }
  }
#endif

  // Device reset
  if(IntrStatus.RESET)
  {
    USB_ISTR = ~bmRESETM;
    USB_HwReset();
    UsbDevConnectCallback(TRUE);
    UsbDevResetCallback();
    UsbDevSuspendCallback(FALSE);
  }

#if USB_SOF_EVENT > 0
  // Frame interrupt
  if(IntrStatus.SOF)
  {
    USB_ISTR = ~bmSOFM;
    if(UsbUserFun[UsbUserSofEvent] != NULL)
    {
      UsbUserFun[UsbUserSofEvent]((void *)NULL);
    }
  }
#endif

  // Wake-up interrupt
  if(IntrStatus.WKUP)
  {
    USB_ISTR = ~bmWKUPM;
    USB_Resume(USB_RESUME_WAKE_UP_EVENT);
    UsbDevSuspendCallback(FALSE);
  }

  // Device suspend
  if(IntrStatus.SUSP)
  {
    USB_Suspend();
    USB_ISTR = ~bmSUSPM;
    UsbDevSuspendCallback(TRUE);
  }

  // Expected Frame interrupt
  if(IntrStatus.ESOF)
  {
    USB_ISTR = ~bmESOFM;
    USB_Resume(USB_RESUME_SOF_EVENT);
  }

  // Low priority EP interrupt
  if(IntrStatus.CTR)
  {
    // find corresponding EP
    pInt32U pReg = (pInt32U)&USB_EP0R;
    pReg += IntrStatus.EP_ID;
    EpPhAddr = ((*pReg & 0xF) << 1) + (IntrStatus.DIR?0:1);
    // The ISO and Double buffered endpoints have own interrupt (HIGH_INTR)
    if((EpInfo[EpPhAddr].EpType != EP_ISO) &&
       (EpInfo[EpPhAddr].EpType != EP_BULK_DOUB_BUF))
    {
      if(EpPhAddr == CTRL_ENP_OUT)
      {
        // determinate type of packet (only for control EP)
        SetupPacket = EpCtrlGet_SETUP(EpInfo[EpPhAddr].pEpCtrl);
        EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        if(UsbUserFun[UsbEp0Out] != NULL)
        {
          // call appropriate callback function
          if(SetupPacket)
          {
            UsbUserFun[UsbEp0Out]((void *)UsbSetupPacket);
          }
          else
          {
            UsbUserFun[UsbEp0Out]((void *)UsbDataOutPacket);
          }
        }
      }
      else if (EpPhAddr & 1)
      {
        // OUT EP
        EpCtrlClr_CTR_TX(EpInfo[EpPhAddr].pEpCtrl);
        if(UsbUserFun[EpPhAddr] != NULL)
        {
          UsbUserFun[EpPhAddr]((void *)UsbDataInPacket);
        }
      }
      else
      {
        // IN EP
        EpCtrlClr_CTR_RX(EpInfo[EpPhAddr].pEpCtrl);
        if(UsbUserFun[EpPhAddr] != NULL)
        {
          UsbUserFun[EpPhAddr]((void *)UsbDataOutPacket);
        }
      }
    }
  }
}

/*************************************************************************
 * Function Name: UsbCtrlEp
 * Parameters:  void * pArg
 *
 * Return: none
 *
 * Description: USB Ctrl EP Callback
 *
 *************************************************************************/
static inline
void UsbCtrlEp (USB_PacketType_t Packet)
{
Int32U CurrCount;

  if(Packet == UsbSetupPacket)
  {
    // Setup packet
    UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;

    UsbEp0Ctrl.pData   = EpCtrlDataBuf;
    UsbEp0Ctrl.Counter = Ep0MaxSize;
    USB_EpRead(CTRL_ENP_OUT,EpCtrlDataBuf,&UsbEp0Ctrl.Counter);

    // Copy new setup packet int setup buffer
    memcpy((Int8U *)&UsbEp0SetupPacket,EpCtrlDataBuf,sizeof(UsbEp0SetupPacket));
    if(UsbEp0Ctrl.Counter != sizeof(UsbSetupPacket_t))
    {
      UsbEp0SetupPacket.mRequestType.Type = UsbTypeReserved;
    }
    else
    {
      // validate OUT/IN EP
      USB_EpValidate(CTRL_ENP_OUT,TRUE);
      USB_SetStallEP(CTRL_ENP_IN,FALSE);
    }

    switch(UsbEp0SetupPacket.mRequestType.Type)
    {
    // Standard
    case UsbTypeStandart:
      // Decoding standard request
      switch (UsbEp0SetupPacket.bRequest)
      {
      case GET_STATUS:
        UsbGetStatus();
        break;
      case CLEAR_FEATURE:
        UsbClearFeature();
        break;
      case SET_FEATURE:
        UsbSetFeature();
        break;
      case SET_ADDRESS:
        UsbSetAddress();
        break;
      case GET_DESCRIPTOR:
        if(UsbEp0SetupPacket.mRequestType.Recipient == UsbRecipientDevice)
        {
          UsbGetDescriptor();
        }
        // Only get descriptor for device is standard request
        else if ((UsbEp0SetupPacket.mRequestType.Dir == UsbDevice2Host) &&
                 (UsbUserFun[UsbUserGetDescriptor] != NULL) &&
                 ((Int32U)UsbUserFun[UsbUserGetDescriptor](&UsbEp0Ctrl) == UsbPass))
        {
          if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
          {
            UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
            UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
          }
          else
          {
            UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
          }
          UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
        }
        else
        {
          UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
        }
        break;
      case SET_DESCRIPTOR:
        // Optional (only for configuration and string descriptors)
        UsbSetDescriptor();
        break;
      case GET_CONFIGURATION:
        UsbGetConfiguration();
        break;
      case SET_CONFIGURATION:
        UsbSetConfiguration();
        break;
      case GET_INTERFACE:
        UsbGetInterface();
        break;
      case SET_INTERFACE:
        UsbSetInterface();
        break;
      case SYNCH_FRAME:
        UsbSynchFrame();
        break;
      default:
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    // Class
    case UsbTypeClass:
      if(UsbUserFun[UsbUserClass] != NULL &&
         ((Int32U)UsbUserFun[UsbUserClass](&UsbEp0Ctrl) == UsbPass))
      {
        if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
        {
          UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
          UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        }
        else
        {
          UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
        }
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    // Vendor
    case UsbTypeVendor:
      if(UsbUserFun[UsbUserVendor] != NULL &&
        ((Int32U)UsbUserFun[UsbUserVendor](&UsbEp0Ctrl) == UsbPass))
      {
        if(UsbEp0Ctrl.Counter >= UsbEp0SetupPacket.wLength.Word)
        {
          UsbEp0Ctrl.Counter = UsbEp0SetupPacket.wLength.Word;
          UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        }
        else
        {
          UsbEp0Ctrl.EpStatus.NoZeroLength = FALSE;
        }
        UsbEp0Ctrl.EpStatus.Status = UsbDataPhase;
      }
      else
      {
        UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
      }
      break;
    // Other
    default:
      UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
    }
  }

  switch (UsbEp0Ctrl.EpStatus.Status)
  {
  case UsbDataPhase:
    if (UsbEp0SetupPacket.mRequestType.Dir == UsbHost2Device)
    {
      if (Packet == UsbDataOutPacket)
      {
        // Data receiving
        CurrCount = Ep0MaxSize;
        USB_EpRead(CTRL_ENP_OUT,UsbEp0Ctrl.pData,&CurrCount);
        UsbEp0Ctrl.pData += CurrCount;
        UsbEp0Ctrl.Counter -= CurrCount;
        // Receiving of data complete when all data was received or
        // when host send short packet
        if ((UsbEp0Ctrl.Counter == 0) ||
            (CurrCount < Ep0MaxSize))
        {
          // Find appropriate callback function depending of current request
          // after data was received
          switch (UsbEp0SetupPacket.mRequestType.Type)
          {
          case UsbTypeStandart:
            if ((UsbCoreT9Fun != NULL) &&
                ((Int32U)UsbCoreT9Fun(NULL) == UsbPass))
            {
              UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
            }
            else
            {
            // Stall EP if callback function isn't set, but host
            // send this type request or when callback function
            // return an error.
              UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
            }
            UsbCoreT9Fun = NULL;
            break;
          case UsbTypeClass:
            if ((UsbUserFun[UsbClassEp0OutPacket] != NULL) &&
                ((Int32U)UsbUserFun[UsbClassEp0OutPacket](NULL) == UsbPass))
            {
              UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
            }
            else
            {
              UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
            }
            break;
          case UsbTypeVendor:
            if ((UsbUserFun[UsbVendorEp0OutPacket] != NULL) &&
                ((Int32U)UsbUserFun[UsbVendorEp0OutPacket](NULL) == UsbPass))
            {
              UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
            }
            else
            {
              UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
            }
            break;
          default:
            UsbEp0Ctrl.EpStatus.Status = UsbEpStall;
          }
          if(UsbEp0Ctrl.EpStatus.Status == UsbStatusPhase)
          {
            CurrCount = 0;
            USB_EpWrite(CTRL_ENP_IN,NULL,&CurrCount);
            UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
          }
        }
      }
      else if (UsbEp0Ctrl.Counter == 0)
      {
        CurrCount = 0;
        USB_EpWrite(CTRL_ENP_IN,NULL,&CurrCount);
        UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
      }
    }
    else
    {
       // Data transmit
      if (Packet == UsbDataOutPacket)
      {
        USB_EpValidate(CTRL_ENP_OUT,TRUE);
        UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        break;
      }
      CurrCount = Ep0MaxSize;
      if ((UsbEp0Ctrl.Counter == 0) && UsbEp0Ctrl.EpStatus.NoZeroLength)
      {
        UsbEp0Ctrl.EpStatus.Status = UsbStatusPhase;
        break;
      }
      else if(UsbEp0Ctrl.Counter < Ep0MaxSize)
      {
        UsbEp0Ctrl.EpStatus.NoZeroLength = TRUE;
        CurrCount = UsbEp0Ctrl.Counter;
      }
      USB_EpWrite(CTRL_ENP_IN,UsbEp0Ctrl.pData,&CurrCount);
      UsbEp0Ctrl.Counter -= CurrCount;
      UsbEp0Ctrl.pData += CurrCount;
    }
    break;
  case UsbStatusPhase:
    if (UsbEp0SetupPacket.bRequest == SET_ADDRESS)
    {
      // Device address is set after status phase of set address request
      UsbDevCtrl.DevAdd = UsbEp0SetupPacket.wValue.Lo;
      USB_SetAdd(UsbDevCtrl.DevAdd);
      if(UsbDevCtrl.DevAdd)
      {
        UsbSetDevState(UsbDevStatusAddress);
      }
      else
      {
        // when address is 0 put device int configuration state
        UsbSetDevState(UsbDevStatusDefault);
      }
    }
    if (Packet == UsbDataOutPacket)
    {
      USB_EpValidate(CTRL_ENP_OUT,TRUE);
    }
    UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
    break;
  default:
    // Error
    USB_SetStallEP(CTRL_ENP_IN,TRUE);
    USB_SetStallEP(CTRL_ENP_OUT,TRUE);
    UsbEp0Ctrl.EpStatus.Status = UsbSetupPhase;
  }
}

