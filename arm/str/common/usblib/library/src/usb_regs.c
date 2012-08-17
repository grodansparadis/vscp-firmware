/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_regs.c
* Author             : MCD Application Team
* Date First Issued  : 10/27/2003 : V1.0
* Description        : Interface functions to USB cell registers
********************************************************************************
* History:
* 09/18/2006 : V3.0
* 09/01/2006 : V2.0
* 10/27/2003 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : SetCNTR
* Description    :
* Input          : wRegValue
* Output         : None
* Return         : None
*******************************************************************************/
void SetCNTR(u16 wRegValue)
{
	_SetCNTR(wRegValue);
}
/*******************************************************************************
* Function Name  : GetCNTR
* Description    :
* Input          : None
* Output         : None
* Return         : CNTR register Value
*******************************************************************************/
u16 GetCNTR(void)
{
	return(_GetCNTR());
}
/*******************************************************************************
* Function Name  : SetISTR
* Description    :
* Input          : wRegValue
* Output         : None
* Return         : None
*******************************************************************************/
void SetISTR(u16 wRegValue)
{
	_SetISTR(wRegValue);
}
/*******************************************************************************
* Function Name  : GetISTR
* Description    :
* Input          :
* Output         :
* Return         : ISTR register Value
*******************************************************************************/
u16 GetISTR(void)
{
	return(_GetISTR());
}
/*******************************************************************************
* Function Name  : GetFNR
* Description    :
* Input          :
* Output         :
* Return         : FNR register Value
*******************************************************************************/
u16 GetFNR(void)
{
	return(_GetFNR());
}
/*******************************************************************************
* Function Name  : SetDADDR
* Description    :
* Input          : wRegValue
* Output         :
* Return         :
*******************************************************************************/
void SetDADDR(u16 wRegValue)
{
	_SetDADDR(wRegValue);
}
/*******************************************************************************
* Function Name  : GetDADDR
* Description    :
* Input          :
* Output         :
* Return         : DADDR register Value
*******************************************************************************/
u16 GetDADDR(void)
{
	return(_GetDADDR());
}
/*******************************************************************************
* Function Name  : SetBTABLE
* Description    :
* Input          : wRegValue
* Output         :
* Return         :
*******************************************************************************/
void SetBTABLE(u16 wRegValue)
{
	_SetBTABLE(wRegValue);
}

/*******************************************************************************
* Function Name  : GetBTABLE
* Description    :
* Input          :
* Output         :
* Return         : BTABLE address
*******************************************************************************/
u16 GetBTABLE(void)
{
	return(_GetBTABLE());
}
/*******************************************************************************
* Function Name  : SetENDPOINT
* Description    :
* Input          : bEpNum, wRegValue
* Output         :
* Return         :
*******************************************************************************/
void SetENDPOINT(u8 bEpNum, u16 wRegValue)
{
	_SetENDPOINT(bEpNum,wRegValue);
}
/*******************************************************************************
* Function Name  : GetENDPOINT
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetENDPOINT(u8 bEpNum)
{
	return(_GetENDPOINT(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPType
* Description    :
* Input          : bEpNum, wType
* Output         :
* Return         :
*******************************************************************************/
void SetEPType(u8 bEpNum, u16 wType)
{
	_SetEPType(bEpNum, wType);
}
/*******************************************************************************
* Function Name  : GetEPType
* Description    :
* Input          : bEpNum,
* Output         :
* Return         : Endpoint Type
*******************************************************************************/
u16 GetEPType(u8 bEpNum)
{
	return(_GetEPType(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPTxStatus
* Description    :
* Input          : bEpNum, wState
* Output         :
* Return         :
*******************************************************************************/
void SetEPTxStatus(u8 bEpNum, u16 wState)
{
	_SetEPTxStatus(bEpNum,wState);
}
/*******************************************************************************
* Function Name  : SetEPRxStatus
* Description    :
* Input          : bEpNum, wState
* Output         :
* Return         :
*******************************************************************************/
void SetEPRxStatus(u8 bEpNum, u16 wState)
{
	_SetEPRxStatus(bEpNum,wState);
} 
/*******************************************************************************
* Function Name  : SetDouBleBuffEPStall
* Description    : sets the status for Double Buffer Endpoint to STALL
* Input          : bEpNum = endpoint number
	           bDir = Endpoint direction
* Output         :
* Return         :
*******************************************************************************/
void SetDouBleBuffEPStall(u8 bEpNum,u8 bDir)
{
  u16 Endpoint_DTOG_Status;
  Endpoint_DTOG_Status = GetENDPOINT(bEpNum);
 if(bDir== EP_DBUF_OUT)
   { /* OUT double buffered endpoint */
   _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPRX_DTOG1);
   }
   else if(bDir == EP_DBUF_IN)
   { /* IN double buffered endpoint */
   _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPTX_DTOG1);
   }
}
/*******************************************************************************
* Function Name  : GetEPTxStatus
* Description    :
* Input          : bEpNum
* Output         :
* Return         : Endpoint TX Status
*******************************************************************************/
u16 GetEPTxStatus(u8 bEpNum)
{
	return(_GetEPTxStatus(bEpNum));
}
/*******************************************************************************
* Function Name  : GetEPRxStatus
* Description    :
* Input          : bEpNum
* Output         :
* Return         : Endpoint RX Status
*******************************************************************************/
u16 GetEPRxStatus(u8 bEpNum)
{
	return(_GetEPRxStatus(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPTxValid
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void SetEPTxValid(u8 bEpNum)
{
 	_SetEPTxStatus(bEpNum, EP_TX_VALID);
}
/*******************************************************************************
* Function Name  : SetEPRxValid
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void SetEPRxValid(u8 bEpNum)
{
 	_SetEPRxStatus(bEpNum, EP_RX_VALID);
}
/*******************************************************************************
* Function Name  : SetEP_KIND
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void SetEP_KIND(u8 bEpNum)
{
	_SetEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : ClearEP_KIND
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearEP_KIND(u8 bEpNum)
{
	_ClearEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : Clear_Status_Out
* Description    : Clear the Status Out of the related Endpoint
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void Clear_Status_Out(u8 bEpNum)
{
	 _ClearEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : Set_Status_Out
* Description    : Set the Status Out of the related Endpoint
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void Set_Status_Out(u8 bEpNum)
{
	_SetEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : SetEPDoubleBuff
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void SetEPDoubleBuff(u8 bEpNum)
{
	 _SetEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : ClearEPDoubleBuff
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearEPDoubleBuff(u8 bEpNum)
{
	 _ClearEP_KIND(bEpNum);
}
/*******************************************************************************
* Function Name  : GetTxStallStatus
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetTxStallStatus(u8 bEpNum)
{
	return(_GetTxStallStatus(bEpNum));
}
/*******************************************************************************
* Function Name  : GetRxStallStatus
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetRxStallStatus(u8 bEpNum)
{
	return(_GetRxStallStatus(bEpNum));
}
/*******************************************************************************
* Function Name  : ClearEP_CTR_RX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearEP_CTR_RX(u8 bEpNum)
{
	_ClearEP_CTR_RX(bEpNum);
}
/*******************************************************************************
* Function Name  : ClearEP_CTR_TX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearEP_CTR_TX(u8 bEpNum)
{
	_ClearEP_CTR_TX(bEpNum);
}
/*******************************************************************************
* Function Name  : ToggleDTOG_RX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ToggleDTOG_RX(u8 bEpNum)
{
	_ToggleDTOG_RX(bEpNum);
}
/*******************************************************************************
* Function Name  : ToggleDTOG_TX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ToggleDTOG_TX(u8 bEpNum)
{
	_ToggleDTOG_TX(bEpNum);
}
/*******************************************************************************
* Function Name  : ClearDTOG_RX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearDTOG_RX(u8 bEpNum)
{
	_ClearDTOG_RX(bEpNum);
}
/*******************************************************************************
* Function Name  : ClearDTOG_TX
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
void ClearDTOG_TX(u8 bEpNum)
{
	_ClearDTOG_TX(bEpNum);
}
/*******************************************************************************
* Function Name  : SetEPAddress
* Description    :
* Input          : bEpNum, bAddr
* Output         :
* Return         :
*******************************************************************************/
void SetEPAddress(u8 bEpNum,u8 bAddr)
{
	_SetEPAddress(bEpNum,bAddr);
}
/*******************************************************************************
* Function Name  : GetEPAddress
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u8 GetEPAddress(u8 bEpNum)
{
	return(_GetEPAddress(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPTxAddr
* Description    :
* Input          : bEpNum,  wAddr
* Output         :
* Return         :
*******************************************************************************/
void SetEPTxAddr(u8 bEpNum, u16 wAddr)
{
	_SetEPTxAddr(bEpNum,wAddr);
}
/*******************************************************************************
* Function Name  : SetEPRxAddr
* Description    :
* Input          : bEpNum,  wAddr
* Output         :
* Return         :
*******************************************************************************/
void SetEPRxAddr(u8 bEpNum, u16 wAddr)
{
	 _SetEPRxAddr(bEpNum,wAddr);
}
/*******************************************************************************
* Function Name  : GetEPTxAddr
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPTxAddr(u8 bEpNum)
{
	 return(_GetEPTxAddr(bEpNum));
} 
/*******************************************************************************
* Function Name  : GetEPRxAddr
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPRxAddr(u8 bEpNum)
{
	 return(_GetEPRxAddr(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPTxCount
* Description    :
* Input          : bEpNum, wCount
* Output         :
* Return         :
*******************************************************************************/
void SetEPTxCount(u8 bEpNum, u16 wCount)
{
	_SetEPTxCount(bEpNum,wCount);
}
/*******************************************************************************
* Function Name  : SetEPCountRxReg
* Description    :
* Input          : *pdwReg, wCount
* Output         :
* Return         :
*******************************************************************************/
#ifdef STR7xx
void SetEPCountRxReg(u32 *pdwReg, u16 wCount)
{
	_SetEPCountRxReg(dwReg, wCount);
}
#endif
/*******************************************************************************
* Function Name  : SetEPRxCount
* Description    :
* Input          : bEpNum, wCount
* Output         :
* Return         :
*******************************************************************************/
void SetEPRxCount(u8 bEpNum, u16 wCount)
{
	_SetEPRxCount(bEpNum,wCount);
}
/*******************************************************************************
* Function Name  : GetEPTxCount
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPTxCount(u8 bEpNum)
{
	 return(_GetEPTxCount(bEpNum));
}
/*******************************************************************************
* Function Name  : GetEPRxCount
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPRxCount(u8 bEpNum)
{
	 return(_GetEPRxCount(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPDblBuffAddr
* Description    :
* Input          : bEpNum, wBuf0Addr, wBuf1Addr
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuffAddr(u8 bEpNum, u16 wBuf0Addr, u16 wBuf1Addr)
{
	_SetEPDblBuffAddr(bEpNum, wBuf0Addr, wBuf1Addr);
}
/*******************************************************************************
* Function Name  : SetEPDblBuf0Addr
* Description    :
* Input          : bEpNum, wBuf0Addr
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuf0Addr(u8 bEpNum,u16 wBuf0Addr)
{
	 _SetEPDblBuf0Addr(bEpNum, wBuf0Addr);
}
/*******************************************************************************
* Function Name  : SetEPDblBuf1Addr
* Description    :
* Input          : bEpNum, wBuf1Addr
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuf1Addr(u8 bEpNum,u16 wBuf1Addr)
{
	_SetEPDblBuf1Addr(bEpNum, wBuf1Addr);
}
/*******************************************************************************
* Function Name  : GetEPDblBuf0Addr
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPDblBuf0Addr(u8 bEpNum)
{
	return(_GetEPDblBuf0Addr(bEpNum));
}
/*******************************************************************************
* Function Name  : GetEPDblBuf1Addr
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPDblBuf1Addr(u8 bEpNum)
{
	return(_GetEPDblBuf1Addr(bEpNum));
}
/*******************************************************************************
* Function Name  : SetEPDblBuffCount
* Description    :
* Input          : bEpNum,bDir, wCount
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuffCount(u8 bEpNum, u8 bDir, u16 wCount)
{
	#ifdef STR7xx /*STR7xx family */
	 _SetEPDblBuffCount(bEpNum, bDir, wCount);
	#endif
	
	#ifdef STR91x /*STR91x family*/
	SetEPDblBuf0Count(bEpNum, bDir,wCount);
	SetEPDblBuf1Count(bEpNum, bDir,wCount);
	#endif	
}
/*******************************************************************************
* Function Name  : SetEPDblBuf0Count
* Description    :
* Input          : bEpNum, bDir,  wCount
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuf0Count(u8 bEpNum, u8 bDir,u16 wCount)
{
	#ifdef STR7xx /*STR7xx family */
	_SetEPDblBuf0Count(bEpNum,bDir,wCount);
	#endif
	
	#ifdef STR91x /*STR91x family*/
	u32 BLsize=0;
	u32 Blocks;
	if(bDir == EP_DBUF_IN)   					
	/* IN double bufferd endpoint */						
	SetEPTxCount(bEpNum,wCount);
	else if(bDir == EP_DBUF_OUT) 
	{				
	/* OUT double bufferd endpoint */			

   		if (wCount < 64) Blocks = wCount>>1;
   		else
   		{
   			BLsize = 0x8000;
   			Blocks = wCount>>6;
   		}
	 *_pEPBufCount(bEpNum) &=~0x8000;
	 *_pEPBufCount(bEpNum) |=BLsize;
	 *_pEPBufCount(bEpNum)  &=~0x7C00;
	 *_pEPBufCount(bEpNum) |=Blocks<<10;
	 *_pEPBufCount(bEpNum) &=0xFFFFFC00;
 	}
 	#endif
}
/*******************************************************************************
* Function Name  : SetEPDblBuf1Count
* Description    :
* Input          : bEpNum,  bDir,  wCount
* Output         :
* Return         :
*******************************************************************************/
void SetEPDblBuf1Count(u8 bEpNum, u8 bDir,u16 wCount)
{
	#ifdef STR7xx /*STR7xx family */
	_SetEPDblBuf1Count(bEpNum,bDir,wCount);
	#endif
	
	#ifdef STR91x /*STR91x family*/
	if(bDir == EP_DBUF_IN)					
 	/* IN double buffered endpoint */						
 	{
   	*_pEPBufCount(bEpNum)&= 0x000FFFF;
   	*_pEPBufCount(bEpNum)|=(wCount<<16);
 	}
 	else if(bDir == EP_DBUF_OUT)				
 	/* OUT double buffered endpoint */						
  	_SetEPRxCount(bEpNum, wCount);
  	#endif	
}
/*******************************************************************************
* Function Name  : GetEPDblBuf0Count
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPDblBuf0Count(u8 bEpNum)
{
	 return(_GetEPDblBuf0Count(bEpNum));
}
/*******************************************************************************
* Function Name  : GetEPDblBuf1Count
* Description    :
* Input          : bEpNum
* Output         :
* Return         :
*******************************************************************************/
u16 GetEPDblBuf1Count(u8 bEpNum)
{
	 return(_GetEPDblBuf1Count(bEpNum));
}
#ifdef STR7xx /*STR7xx family */
/*******************************************************************************
* Function Name  : GetEPDblBufDir
* Description    : gets direction of the double buffered endpoint
* Input          : bEpNum
* Output         : EP_DBUF_OUT, EP_DBUF_IN,
                   EP_DBUF_ERR if the endpoint counter not yet programmed
* Return         :
*******************************************************************************/
EP_DBUF_DIR GetEPDblBufDir(u8 bEpNum) {
	 if((u16)(*_pEPRxCount(bEpNum) & 0xFC00) != 0)
	 		return(EP_DBUF_OUT);
	 else if(((u16)(*_pEPTxCount(bEpNum)) & 0x03FF) != 0)
	 		return(EP_DBUF_IN);
	 else
	 		return(EP_DBUF_ERR);
}
#endif
/*******************************************************************************
* Function Name  : FreeUserBuffer
* Description    : free buffer used from the application realising it to the line
                   toggles bit SW_BUF in the double buffered endpoint register
* Input          : bEpNum, bDir
* Output         :
* Return         :
*******************************************************************************/
void FreeUserBuffer(u8 bEpNum, u8 bDir)
{
   if(bDir== EP_DBUF_OUT)
   { /* OUT double buffered endpoint */
    _ToggleDTOG_TX(bEpNum);
   }
   else if(bDir == EP_DBUF_IN)
   { /* IN double buffered endpoint */
    _ToggleDTOG_RX(bEpNum);
   }
}

/*******************************************************************************
* Function Name  : ToWord
* Description    :
* Input          : bh, bl
* Output         :
* Return         :
*******************************************************************************/
u16 ToWord(u8 bh, u8 bl)
{
 u16 wRet;
   wRet = (u16)bl | ((u16)bh << 8);
   return(wRet);
}
/*******************************************************************************
* Function Name  : ByteSwap
* Description    :
* Input          : wSwW
* Output         :
* Return         :
*******************************************************************************/
u16 ByteSwap(u16 wSwW)
{
 u8 bTemp;
 u16 wRet;
   bTemp = (u8)(wSwW & 0xff);
   wRet =  (wSwW >> 8) | ((u16)bTemp << 8);
   return(wRet);
}

/* DMA Functions only for STR91x */
#ifdef STR91x /*str91x family*/
/*******************************************************************************
* Function Name  : SetDMAburstTxSize
* Description    : Configure the Burst Size for a Tx Endpoint
* Input          : DestBsize: Destination Burst Size
* Output         : None
* Return         : None
*******************************************************************************/
void SetDMABurstTxSize(u8 DestBsize)
{
  *DMABSIZE &=~0xEF;
  *DMABSIZE = (DestBsize<<4);
}

/*******************************************************************************
* Function Name  : SetDMABurstRxSize
* Description    : Configure the Burst Size for a Rx Endpoint
* Input          : SrcBsize: Source Burst
* Output         : None
* Return         : None
*******************************************************************************/
void SetDMABurstRxSize(u8 SrcBsize)
{
	*DMABSIZE &=~0x7;
	*DMABSIZE = SrcBsize;
}

/*******************************************************************************
* Function Name  : DMAUnlinkedModeTxConfig
* Description    : Configure a Tx Endpoint to trigger TX Unlinked DMA request
* Note           : Up to three endpoints could be configured to trigger DMA
                   request, an index[0:2] must be associated to an endpoint
* Input          : -bEpNum: endpoint number[0:9]
*                  -index: 0,1 or 2
* Output         : None
* Return         : None
*******************************************************************************/
void DMAUnlinkedModeTxConfig(u8 bEpNum ,u8 index)
{
  *DMACR2 &=~(0x0F<<(4*index));
  *DMACR2 |=bEpNum<<(4*index);
}

/*******************************************************************************
* Function Name  : DMAUnlinkedModeTxEnable
* Description    : Enable a Tx endpoint to trigger Tx DMA request
* Input          : -index :0,1 or 2 = index associated to endpoint in function
*                   "DMAUnlinkedModeTxConfig"
* Output         : None
* Return         : None
*******************************************************************************/
void DMAUnlinkedModeTxEnable(u8 index)
{
	*DMACR3 &=~0x01;  /*DMA Tx linked mode disabled*/
	*DMACR2 &=~0x3000;
	*DMACR2 |=(index+1)<<12;
}
	
/*******************************************************************************
* Function Name  : DMAUnlinkedModeTxDisable
* Description    : Enable a Tx endpoint to trigger Tx DMA request
* Input          : index :0,1 or 2 = index associated to endpoint in function
*                   "DMAUnlinkedModeTxConfig"
* Output         : None
* Return         : None
*******************************************************************************/	
void DMAUnlinkedModeTxDisable(u8 index)
{
	*DMACR2 &=~0x3000;
}

/*******************************************************************************
* Function Name  : DMAUnlinkedModeRxEnable
* Description    : Enable a Rx Endpoint to trigger Rx DMA
* Input          : bEpNum: endpoint number[0:9]
* Output         : None
* Return         : None
*******************************************************************************/
void DMAUnlinkedModeRxEnable(u8 bEpNum)
{
	*DMACR3 &=~0x80;   /*DMA Rx linked mode disabled*/
	*DMACR1 |=(0x1<<bEpNum);
}

/*******************************************************************************
* Function Name  : DMAUnlinkedModeRxDisable
* Description    : Disable a Rx Endpoint to trigger Rx DMA
* Input          : bEpNum: endpoint number[0:9]
* Output         : None
* Return         : None
*******************************************************************************/
void DMAUnlinkedModeRxDisable(u8 bEpNum)
{
	*DMACR1 &=~(0x1<<bEpNum);
}

/*******************************************************************************
* Function Name  : DMALinkedModeRxConfig
* Description    : Configure a Rx endpoint to trigger DMA linked request
* Input          : bEpNum: endpoint number[0:9]
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeRxConfig(u8 bEpNum)
{
	*DMACR3 &=~0x1E00;
	*DMACR3 |=bEpNum<<9;
}

/*******************************************************************************
* Function Name  : DMALinkedModeTxConfig
* Description    : Configure a Tx endpoint to trigger DMA linked request
* Input          : bEpNum: endpoint number[0:9]
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeTxConfig(u8 bEpNum)
{
	*DMACR3 &=~0x1E;
	*DMACR3 |=bEpNum<<1;
}

/*******************************************************************************
* Function Name  : DMALinkedModeRxEnable
* Description    : Enable the DMA Linked Rx mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeRxEnable(void)
{
	*DMACR3 |=0x100;
	*DMACR3 |=0x2000;
}

/*******************************************************************************
* Function Name  : DMALinkedModeTxEnable
* Description    : Enable the DMA Linked Tx mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeTxEnable(void)
{
	*DMACR3 |=0x1;
	*DMACR3 |=0x20;
}
/*******************************************************************************
* Function Name  : DMALinkedModeRxDisable
* Description    : Disable the DMA Linked Rx mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeRxDisable(void)
{
	*DMACR3 &=~0x100;
	*DMACR3 &=~0x2000;
}

/*******************************************************************************
* Function Name  : DMALinkedModeTxDisable
* Description    : Disable the DMA Linked Tx mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMALinkedModeTxDisable(void)
{
	*DMACR3 &=~0x1;
	*DMACR3 &=~0x20;
}
/*******************************************************************************
* Function Name  : USB_DMASynchEnable
* Description    : Enable the Synchronization Logic
* Input          : TRUE or FALSE
* Output         : None
* Return         : None
*******************************************************************************/
void DMASynchEnable(void)
{
	*DMACR3 |=0x40;
}

/*******************************************************************************
* Function Name  : USB_DMASynchDisable
* Description    : Disable the Synchronization Logic
* Input          : TRUE or FALSE
* Output         : None
* Return         : None
*******************************************************************************/
void DMASynchDisable(void)
{
	*DMACR3 &=~0x40;
}

/*******************************************************************************
* Function Name  : SetDMALLITxLength
* Description    : Set the DMA LLI Tx length
* Input          : length
* Output         : None
* Return         : None
*******************************************************************************/
void SetDMALLITxLength(u8 length)
{
	*DMALLI &=~0xFF;
	*DMALLI |= length;
}

/*******************************************************************************
* Function Name  : SetDMALLIRxLength
* Description    : Set the DMA LLI Rx length
* Input          : length
* Output         : None
* Return         : None
*******************************************************************************/
void SetDMALLIRxLength(u8 length )
{
        *DMALLI &=~0xFF00;
        *DMALLI |= length<<8;
}

/*******************************************************************************
* Function Name  : SetDMALLIRxPacketNum
* Description    : Set the LLI_RX_NPACKETS field in register USB_DMABSIZE register
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetDMALLIRxPacketNum(u8 PacketNum)
{
	*DMABSIZE &=0xFF;
	*DMABSIZE |=(PacketNum<<8);
}

/*******************************************************************************
* Function Name  : GetDMALLIPacketNum
* Description    : gets the LLI_RX_NPACKETS field value
* Input          : None
* Output         : None
* Return         : LLI_RX_NPACKETS field value
*******************************************************************************/
u8 GetDMALLIRxPacketNum(void)
{
	return((u8)(*DMABSIZE & 0xFF00)>>8);
}
#endif
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

