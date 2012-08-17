/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_regs.h
* Author             : MCD Application Team
* Date First Issued  : 10/27/2003 : V1.0
* Description        : Interface prototype functions to USB cell registers
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_REGS_H
#define __USB_REGS_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

typedef enum _EP_DBUF_DIR{		/* double buffered endpoint direction */
 EP_DBUF_ERR,
 EP_DBUF_OUT,
 EP_DBUF_IN
}EP_DBUF_DIR;

/* endpoint buffer number */
enum EP_BUF_NUM{ 
 EP_NOBUF, 
 EP_BUF0, 
 EP_BUF1
};

/* Exported constants --------------------------------------------------------*/
#ifdef STR7xx

  #ifdef STR71x /*STR71x family*/
  #define RegBase  (0xC0008800L)  /* USB_IP Peripheral Registers base address */
  #define PMAAddr  (0xC0008000L)  /* USB_IP Packet Memory Area base address   */
  #endif /*end of STR71x family*/
  
  #ifdef STR75x /*STR75x family*/
  #define RegBase  (0xFFFFA800L)  /* USB_IP Peripheral Registers base address */
  #define PMAAddr  (0xFFFFA000L)  /* USB_IP Packet Memory Area base address   */
  #endif /*end of STR75x family*/
 
#endif /*end of STR7xx family*/

#ifdef STR91x /*STR91x family*/

  #ifdef STR91x_USB_BUFFERED
  #define RegBase  (0x60000800L)  /* USB_IP Peripheral Registers base address */
  #define PMAAddr  (0x60000000L)  /* USB_IP Packet Memory Area base address */
  #endif
  
  #ifdef STR91x_USB_NON_BUFFERED
  #define RegBase  (0x70000800L)  /* USB_IP Peripheral Registers base address */
  #define PMAAddr  (0x70000000L)  /* USB_IP Packet Memory Area base address */
  #endif
#endif

/* General registers */
#define CNTR    ((volatile unsigned *)(RegBase + 0x40))	/* Control register           */
#define ISTR    ((volatile unsigned *)(RegBase + 0x44))	/* Interrupt status register  */
#define FNR     ((volatile unsigned *)(RegBase + 0x48)) /* Frame number register      */
#define DADDR   ((volatile unsigned *)(RegBase + 0x4C))	/* Device address register    */
#define BTABLE  ((volatile unsigned *)(RegBase + 0x50))	/* Buffer Table address register */

#ifdef STR91x /*STR91x family DMA registers*/

#define DMACR1  ((volatile unsigned *)(RegBase + 0x54)) /* DMA control register 1 */
#define DMACR2  ((volatile unsigned *)(RegBase + 0x58)) /* DMA control register 2 */
#define DMACR3  ((volatile unsigned *)(RegBase + 0x5C)) /* DMA control register 3 */
#define DMABSIZE ((volatile unsigned *)(RegBase + 0x60))/* DMA burst size register */
#define DMALLI  ((volatile unsigned *)(RegBase + 0x64)) /* DMA LLI register */

#endif

/* Endpoint registers */
#define EP0REG  ((volatile unsigned *)(RegBase)) /* endpoint 0 register address */
/* endpoints enumeration */
#define ENDP0	 ((u8)0)
#define ENDP1	 ((u8)1)
#define ENDP2	 ((u8)2)
#define ENDP3	 ((u8)3)
#define ENDP4	 ((u8)4)
#define ENDP5	 ((u8)5)
#define ENDP6	 ((u8)6)
#define ENDP7	 ((u8)7)  /* Only 8  endpoints for STR75x Family */
#define ENDP8	 ((u8)8)
#define ENDP9	 ((u8)9)  /* Only 10 endpoints for STR91x Family */
#define ENDP10	 ((u8)10)
#define ENDP11	 ((u8)11)
#define ENDP12	 ((u8)12)
#define ENDP13	 ((u8)13)
#define ENDP14	 ((u8)14)
#define ENDP15	 ((u8)15)

/*******************************************************************************/
/* 							ISTR interrupt events  							   */
/*******************************************************************************/
#define ISTR_CTR   	(0x8000)	/* Correct TRansfer	        (clear-only bit) */
#define ISTR_DOVR  	(0x4000)	/* DMA OVeR/underrun		(clear-only bit) */
#define ISTR_ERR   	(0x2000)	/* ERRor			(clear-only bit) */
#define ISTR_WKUP  	(0x1000)	/* WaKe UP			(clear-only bit) */
#define ISTR_SUSP  	(0x0800)	/* SUSPend			(clear-only bit) */
#define ISTR_RESET 	(0x0400)	/* RESET			(clear-only bit) */
#define ISTR_SOF   	(0x0200)	/* Start Of Frame		(clear-only bit) */
#define ISTR_ESOF  	(0x0100)	/* Expected Start Of Frame	(clear-only bit) */

#ifdef STR91x /*STR91x family*/
#define ISTR_SZDPR      (0x0080)  /* Short or Zero-Length Received Data Packet */
#endif 

#define ISTR_DIR   	(0x0010)	 /* DIRection of transaction	(read-only bit)  */
#define ISTR_EP_ID 	(0x000F)	 /* EndPoint IDentifier		(read-only bit)  */

#define CLR_CTR   	(~ISTR_CTR)	 /* clear Correct TRansfer bit */
#define CLR_DOVR  	(~ISTR_DOVR)     /* clear DMA OVeR/underrun	bit*/
#define CLR_ERR   	(~ISTR_ERR)	 /* clear ERRor	bit */
#define CLR_WKUP  	(~ISTR_WKUP)     /* clear WaKe UP bit		   */
#define CLR_SUSP  	(~ISTR_SUSP)     /* clear SUSPend bit		   */
#define CLR_RESET 	(~ISTR_RESET)    /* clear RESET	bit			   */
#define CLR_SOF   	(~ISTR_SOF)	 /* clear Start Of Frame bit   */
#define CLR_ESOF  	(~ISTR_ESOF) /* clear Expected Start Of Frame bit */

#ifdef STR91x /*STR91x family*/
#define CLR_SZDPR   (~ISTR_SZDPR)/* clear SZDPR bit */
#endif 

/*******************************************************************************/
/*				 CNTR control register bits definitions						   */
/*******************************************************************************/
#define CNTR_CTRM   (0x8000)	/* Correct TRansfer Mask */
#define CNTR_DOVRM  (0x4000)	/* DMA OVeR/underrun Mask */
#define CNTR_ERRM   (0x2000)	/* ERRor Mask */
#define CNTR_WKUPM  (0x1000)	/* WaKe UP Mask */
#define CNTR_SUSPM  (0x0800)	/* SUSPend Mask	*/
#define CNTR_RESETM (0x0400)	/* RESET Mask   */
#define CNTR_SOFM   (0x0200)	/* Start Of Frame Mask */
#define CNTR_ESOFM  (0x0100)	/* Expected Start Of Frame Mask */

#ifdef STR91x /*STR91x family*/
#define CNTR_SZDPRM (0x0080)    /* Short or Zero-Length Received Data Packet Mask*/
#endif

#define CNTR_RESUME (0x0010)	/* RESUME request */
#define CNTR_FSUSP  (0x0008)	/* Force SUSPend */
#define CNTR_LPMODE (0x0004)	/* Low-power MODE	*/
#define CNTR_PDWN   (0x0002)	/* Power DoWN */
#define CNTR_FRES   (0x0001)	/* Force USB RESet */

/*******************************************************************************/
/* 					FNR Frame Number Register bit definitions				   */
/*******************************************************************************/
#define FNR_RXDP	(0x8000)	/* status of D+ data line */
#define FNR_RXDM	(0x4000)	/* status of D- data line */
#define FNR_LCK		(0x2000)	/* LoCKed */
#define FNR_LSOF	(0x1800)	/* Lost SOF */
#define FNR_FN		(0x07FF)	/* Frame Number */
/*******************************************************************************/
/*					DADDR Device ADDRess bit definitions					   */
/*******************************************************************************/
#define DADDR_EF	(0x80)
#define DADDR_ADD	(0x7F)
/*===============================================================================*/
/* Endpoint register */
/*===============================================================================*/
/* bit positions */
#define EP_CTR_RX      (0x8000) /* EndPoint Correct TRansfer RX 	*/
#define EP_DTOG_RX     (0x4000) /* EndPoint Data TOGGLE RX */
#define EPRX_STAT      (0x3000)	/* EndPoint RX STATus bit field */
#define EP_SETUP       (0x0800)	/* EndPoint SETUP */
#define EP_T_FIELD     (0x0600) /* EndPoint TYPE */
#define EP_KIND        (0x0100) /* EndPoint KIND */
#define EP_CTR_TX      (0x0080) /* EndPoint Correct TRansfer TX */
#define EP_DTOG_TX     (0x0040) /* EndPoint Data TOGGLE TX */
#define EPTX_STAT      (0x0030)	/* EndPoint TX STATus bit field */
#define EPADDR_FIELD   (0x000F) /* EndPoint ADDRess FIELD */

/* EndPoint REGister MASK (no toggle fields) */
#define EPREG_MASK     (EP_CTR_RX|EP_SETUP|EP_T_FIELD|EP_KIND|EP_CTR_TX|EPADDR_FIELD)

/* EP_TYPE[1:0] EndPoint TYPE */
#define EP_TYPE_MASK   (0x0600) /* EndPoint TYPE Mask */
#define EP_BULK        (0x0000)	/* EndPoint BULK */
#define EP_CONTROL     (0x0200) /* EndPoint CONTROL */
#define EP_ISOCHRONOUS (0x0400) /* EndPoint ISOCHRONOUS */
#define EP_INTERRUPT   (0x0600) /* EndPoint INTERRUPT */
#define EP_T_MASK      (~EP_T_FIELD & EPREG_MASK)


/* EP_KIND EndPoint KIND */
#define EPKIND_MASK    (~EP_KIND & EPREG_MASK)

/* STAT_TX[1:0] STATus for TX transfer */
#define EP_TX_DIS      (0x0000)	/* EndPoint TX DISabled */
#define EP_TX_STALL    (0x0010)	/* EndPoint TX STALLed */
#define EP_TX_NAK      (0x0020) /* EndPoint TX NAKed */
#define EP_TX_VALID    (0x0030)	/* EndPoint TX VALID */
#define EPTX_DTOG1     (0x0010)	/* EndPoint TX Data TOGgle bit1 */
#define EPTX_DTOG2     (0x0020)	/* EndPoint TX Data TOGgle bit2 */
#define EPTX_DTOGMASK  (EPTX_STAT|EPREG_MASK)

/* STAT_RX[1:0] STATus for RX transfer */
#define EP_RX_DIS      (0x0000)	/* EndPoint RX DISabled */
#define EP_RX_STALL    (0x1000)	/* EndPoint RX STALLed */
#define EP_RX_NAK      (0x2000)	/* EndPoint RX NAKed */
#define EP_RX_VALID    (0x3000)	/* EndPoint RX VALID */
#define EPRX_DTOG1     (0x1000)	/* EndPoint RX Data TOGgle bit1 */
#define EPRX_DTOG2     (0x2000)	/* EndPoint RX Data TOGgle bit1 */
#define EPRX_DTOGMASK  (EPRX_STAT|EPREG_MASK)

/* Exported macro ------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/* SetCNTR */
/*----------------------------------------------------------------*/
#define _SetCNTR(wRegValue)	 (*CNTR   = (u16)wRegValue)
/*----------------------------------------------------------------*/
/* SetISTR */
/*----------------------------------------------------------------*/
#define _SetISTR(wRegValue)	 (*ISTR   = (u16)wRegValue)
/*----------------------------------------------------------------*/
/* SetDADDR */
/*----------------------------------------------------------------*/
#define _SetDADDR(wRegValue) (*DADDR  = (u16)wRegValue)
/*----------------------------------------------------------------*/
/* SetBTABLE */
/*----------------------------------------------------------------*/
#define _SetBTABLE(wRegValue)(*BTABLE = (u16)(wRegValue & 0xFFF8))
/*----------------------------------------------------------------*/
/* GetCNTR */
/*----------------------------------------------------------------*/
#define _GetCNTR()   ((u16) *CNTR)
/*----------------------------------------------------------------*/
/* GetISTR */
/*----------------------------------------------------------------*/
#define _GetISTR()   ((u16) *ISTR)
/*----------------------------------------------------------------*/
/* GetFNR */
/*----------------------------------------------------------------*/
#define _GetFNR()    ((u16) *FNR)
/*----------------------------------------------------------------*/
/* GetDADDR */
/*----------------------------------------------------------------*/
#define	_GetDADDR()  ((u16) *DADDR)
/*----------------------------------------------------------------*/
/* GetBTABLE */
/*----------------------------------------------------------------*/
#define _GetBTABLE() ((u16) *BTABLE)
/*----------------------------------------------------------------*/
/* SetENDPOINT */
/*----------------------------------------------------------------*/
#define _SetENDPOINT(bEpNum,wRegValue)  (*(EP0REG + bEpNum)= \
					(u16)wRegValue)
/*----------------------------------------------------------------*/
/* GetENDPOINT */
/*----------------------------------------------------------------*/
#define _GetENDPOINT(bEpNum)  		    ((u16)(*(EP0REG + bEpNum)))
/*----------------------------------------------------------------*/
/* SetEPType */
/* sets the type in the endpoint register(bits EP_TYPE[1:0]) */
/* IN : bEpNum = endpoint number */
/*		wType  = type definition */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPType(bEpNum,wType)	(_SetENDPOINT(bEpNum,\
					((_GetENDPOINT(bEpNum) & EP_T_MASK) | wType)))
/*----------------------------------------------------------------*/
/* GetEPType */
/* gets the type in the endpoint register(bits EP_TYPE[1:0]) */
/* IN : bEpNum  = endpoint number */
/* OUT: type definition */
/*----------------------------------------------------------------*/
#define _GetEPType(bEpNum) (_GetENDPOINT(bEpNum) & EP_T_FIELD)
/*----------------------------------------------------------------*/
/* SetEPTxStatus */
/* sets the status for tx transfer (bits STAT_TX[1:0]) */
/* IN : bEpNum = endpoint number */
/*		wState = new state */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPTxStatus(bEpNum,wState) {\
 register u16 _wRegVal; 			   \
	_wRegVal = _GetENDPOINT(bEpNum) & EPTX_DTOGMASK;\
	/* toggle first bit ? */		   \
	if((EPTX_DTOG1 & wState)!= 0)  	   \
		_wRegVal ^= EPTX_DTOG1;    	   \
	/* toggle second bit ?  */ 	   	   \
	if((EPTX_DTOG2 & wState)!= 0)  	   \
		_wRegVal ^= EPTX_DTOG2;    	   \
	_SetENDPOINT(bEpNum, _wRegVal);	   \
} /* _SetEPTxStatus */

/*----------------------------------------------------------------*/
/* SetEPRxStatus */
/* sets the status for rx transfer (bits STAT_TX[1:0]) */
/* IN : bEpNum = endpoint number */
/*		wState = new state */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPRxStatus(bEpNum,wState) {\
 register u16 _wRegVal;			\
	\
	_wRegVal = _GetENDPOINT(bEpNum) & EPRX_DTOGMASK;\
	/* toggle first bit ?	*/		\
	if((EPRX_DTOG1 & wState)!= 0)	\
		_wRegVal ^= EPRX_DTOG1;		\
	/* toggle second bit ?	*/		\
	if((EPRX_DTOG2 & wState)!= 0)	\
		_wRegVal ^= EPRX_DTOG2;		\
	_SetENDPOINT(bEpNum, _wRegVal);	\
} /* _SetEPRxStatus */
/*----------------------------------------------------------------*/
/* GetEPTxStatus / GetEPRxStatus */
/* gets the status for tx/rx transfer (bits STAT_TX[1:0]/STAT_RX[1:0]) */
/* IN : bEpNum  = endpoint number */
/* OUT: u16 status  */
/*----------------------------------------------------------------*/
#define _GetEPTxStatus(bEpNum) ((u16)_GetENDPOINT(bEpNum) & EPTX_STAT)
#define _GetEPRxStatus(bEpNum) ((u16)_GetENDPOINT(bEpNum) & EPRX_STAT)
/*----------------------------------------------------------------*/
/* SetEPTxValid / SetEPRxValid */
/* sets directly the VALID tx/rx-status into the enpoint register */
/* IN : bEpNum = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPTxValid(bEpNum)     (_SetEPTxStatus(bEpNum, EP_TX_VALID))
#define _SetEPRxValid(bEpNum)     (_SetEPRxStatus(bEpNum, EP_RX_VALID))
/*----------------------------------------------------------------*/
/* GetTxStallStatus / GetRxStallStatus */
/* checks stall condition in an endpoint */
/* IN : bEpNum = endpoint number */
/* OUT: TRUE = endpoint in stall condition */
/*----------------------------------------------------------------*/
#define _GetTxStallStatus(bEpNum) (_GetEPTxStatus(bEpNum) \
				   == EP_TX_STALL)
#define _GetRxStallStatus(bEpNum) (_GetEPRxStatus(bEpNum) \
				   == EP_RX_STALL)
/*----------------------------------------------------------------*/
/* SetEP_KIND / ClearEP_KIND */
/* IN : bEpNum  = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEP_KIND(bEpNum)    (_SetENDPOINT(bEpNum, \
			       (_GetENDPOINT(bEpNum) | EP_KIND) & EPREG_MASK))
#define _ClearEP_KIND(bEpNum)  (_SetENDPOINT(bEpNum, \
			       (_GetENDPOINT(bEpNum) & EPKIND_MASK)))
/*----------------------------------------------------------------*/
/* Set_Status_Out / Clear_Status_Out */
/* sets/clears directly STATUS_OUT bit in the endpoint register */
/* to be used only during control transfers */
/* IN : bEpNum = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _Set_Status_Out(bEpNum)	   _SetEP_KIND(bEpNum)
#define _Clear_Status_Out(bEpNum)  _ClearEP_KIND(bEpNum)
/*----------------------------------------------------------------*/
/* SetEPDoubleBuff / ClearEPDoubleBuff */
/* sets/clears directly EP_KIND bit in the endpoint register */
/* IN : bEpNum = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPDoubleBuff(bEpNum)   _SetEP_KIND(bEpNum)
#define _ClearEPDoubleBuff(bEpNum) _ClearEP_KIND(bEpNum)
/*----------------------------------------------------------------*/
/* ClearEP_CTR_RX / ClearEP_CTR_TX */
/* clears bit CTR_RX / CTR_TX in the endpoint register */
/* IN : bEpNum = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _ClearEP_CTR_RX(bEpNum)   (_SetENDPOINT(bEpNum,\
				   _GetENDPOINT(bEpNum) & 0x7FFF & EPREG_MASK))
#define _ClearEP_CTR_TX(bEpNum)   (_SetENDPOINT(bEpNum,\
				   _GetENDPOINT(bEpNum) & 0xFF7F & EPREG_MASK))
/*----------------------------------------------------------------*/
/* ToggleDTOG_RX / ToggleDTOG_TX */
/* toggles DTOG_RX / DTOG_TX bit in the endpoint register */
/* IN : bEpNum  = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _ToggleDTOG_RX(bEpNum)    (_SetENDPOINT(bEpNum, \
				   EP_DTOG_RX | _GetENDPOINT(bEpNum) & EPREG_MASK))
#define _ToggleDTOG_TX(bEpNum)    (_SetENDPOINT(bEpNum, \
			           EP_DTOG_TX | _GetENDPOINT(bEpNum) & EPREG_MASK))
/*----------------------------------------------------------------*/
/* ClearDTOG_RX / ClearDTOG_TX */
/* IN : bEpNum  = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _ClearDTOG_RX(bEpNum)  if((_GetENDPOINT(bEpNum) & EP_DTOG_RX) != 0)\
			 	_ToggleDTOG_RX(bEpNum)
#define _ClearDTOG_TX(bEpNum)  if((_GetENDPOINT(bEpNum) & EP_DTOG_TX) != 0)\
			 	_ToggleDTOG_TX(bEpNum)
/*----------------------------------------------------------------*/
/* SetEPAddress */
/* sets address in an endpoint register */
/* IN : bEpNum  = endpoint number */
/* 		bAddr   = address */
/* OUT: none */
/*----------------------------------------------------------------*/

#define _SetEPAddress(bEpNum,bAddr) _SetENDPOINT(bEpNum,\
			           _GetENDPOINT(bEpNum) & EPREG_MASK | bAddr)
/*----------------------------------------------------------------*/
/* GetEPAddress */
/* IN : bEpNum  = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _GetEPAddress(bEpNum) ((u8)(_GetENDPOINT(bEpNum) & EPADDR_FIELD))
/*----------------------------------------------------------------*/
#ifdef STR7xx /*STR7xx family*/
#define _pEPTxAddr(bEpNum)	((u32 *)((_GetBTABLE()+bEpNum*8  )*2 + PMAAddr))
#define _pEPTxCount(bEpNum)	((u32 *)((_GetBTABLE()+bEpNum*8+2)*2 + PMAAddr))
#define _pEPRxAddr(bEpNum)	((u32 *)((_GetBTABLE()+bEpNum*8+4)*2 + PMAAddr))
#define _pEPRxCount(bEpNum)	((u32 *)((_GetBTABLE()+bEpNum*8+6)*2 + PMAAddr))
#endif 

#ifdef STR91x /*STR91x family*/
/* Pointers on endpoint(bEpNum) Count & Addr registers on PMA */
#define _pEPBufCount(bEpNum)	((u32 *)(_GetBTABLE()+bEpNum*8 + 4   + PMAAddr))
#define _pEPBufAddr(bEpNum)	((u32 *)(_GetBTABLE()+bEpNum*8         + PMAAddr))
#endif
/*----------------------------------------------------------------*/
/* SetEPTxAddr / SetEPRxAddr */
/* sets address of the tx/rx buffer */
/* IN : bEpNum = endpoint number */
/*      wAddr  = address to be set ( must be word aligned ) */
/* OUT: none */
/*----------------------------------------------------------------*/

#ifdef STR7xx /*STR7xx family*/
#define _SetEPTxAddr(bEpNum,wAddr) (*_pEPTxAddr(bEpNum) = ((wAddr >> 1) << 1))
#define _SetEPRxAddr(bEpNum,wAddr) (*_pEPRxAddr(bEpNum) = ((wAddr >> 1) << 1))
#endif

#ifdef STR91x /*STR91x family*/
#define _SetEPTxAddr(bEpNum , wAddr) {\
 *_pEPBufAddr(bEpNum) &=0xFFFF0000; \
 *_pEPBufAddr(bEpNum) |=((wAddr)&0x0FFFC);\
 }
#define _SetEPRxAddr(bEpNum, wAddr) {\
 *_pEPBufAddr(bEpNum) &=0x0000FFFF;\
 *_pEPBufAddr(bEpNum) |=((wAddr<<16)&0xFFFC0000);\
}
#endif

/*----------------------------------------------------------------*/
/* GetEPTxAddr / GetEPRxAddr */
/* gets address of the tx/rx buffer */
/* IN : bEpNum = endpoint number */
/* IN : */
/* OUT: address of the buffer */
/*----------------------------------------------------------------*/

#ifdef STR7xx /*STR7xx family*/
#define _GetEPTxAddr(bEpNum) ((u16)*_pEPTxAddr(bEpNum))
#define _GetEPRxAddr(bEpNum) ((u16)*_pEPRxAddr(bEpNum))
#endif

#ifdef STR91x /*STR91x family*/
#define _GetEPTxAddr(bEpNum) ((u16)(*_pEPBufAddr(bEpNum) &0x0000FFFF))
#define _GetEPRxAddr(bEpNum) ((u16)((*_pEPBufAddr(bEpNum)&0xFFFF0000)>>16))
#endif
/*----------------------------------------------------------------*/
/* SetEPCountRxReg */
/* sets counter of rx buffer with no. of blocks */
/* IN : pdwReg = pointer to counter */
/*      wCount = counter */
/* OUT: none */
/*----------------------------------------------------------------*/

#ifdef STR7xx /*STR7xx family*/
#define _BlocksOf32(dwReg,wCount,wNBlocks) {\
	 	wNBlocks = wCount >> 5;\
	 	if((wCount & 0x1f) == 0)\
	 			wNBlocks--;\
		*pdwReg = (u32)((wNBlocks << 10) | 0x8000);\
}/* _BlocksOf32 */

#define _BlocksOf2(dwReg,wCount,wNBlocks) {\
	 	wNBlocks = wCount >> 1;\
	 	if((wCount & 0x1) != 0)\
	 			 wNBlocks++;\
		*pdwReg = (u32)(wNBlocks << 10);\
}/* _BlocksOf2 */

#define _SetEPCountRxReg(dwReg,wCount)  {\
 u16 wNBlocks;\
	 if(wCount > 62){_BlocksOf32(dwReg,wCount,wNBlocks);}\
	 else {_BlocksOf2(dwReg,wCount,wNBlocks);}\
}/* _SetEPCountRxReg */



#define _SetEPRxDblBuf0Count(bEpNum,wCount) {\
 u32 *pdwReg = _pEPTxCount(bEpNum); \
 	 _SetEPCountRxReg(pdwReg, wCount);\
}
#endif 
/*----------------------------------------------------------------*/
/* SetEPTxCount / SetEPRxCount */
/* sets counter for the tx/rx buffer */
/* IN : bEpNum = endpoint number */
/*		wCount = counter value */
/* OUT: none */
/*----------------------------------------------------------------*/

#ifdef STR7xx /*STR7xx family*/
#define _SetEPTxCount(bEpNum,wCount) (*_pEPTxCount(bEpNum) = wCount)
#define _SetEPRxCount(bEpNum,wCount) {\
 u32 *pdwReg = _pEPRxCount(bEpNum); \
 	 _SetEPCountRxReg(pdwReg, wCount);\
}
#endif 

#ifdef STR91x /*STR91x family*/
#define _SetEPTxCount(bEpNum,wCount) {\
 *_pEPBufCount(bEpNum) &=0xFFFFFC00;\
 *_pEPBufCount(bEpNum) |=wCount;\
}

#define _SetEPRxCount(bEpNum,wCount) {\
u32 BLsize=0;\
u32 Blocks;\
if (wCount < 64) Blocks = wCount>>1;\
else\
{\
  BLsize = 0x80000000;\
  Blocks = wCount>>6;\
}\
*_pEPBufCount(bEpNum) &=~0x80000000;\
*_pEPBufCount(bEpNum) |=BLsize;\
*_pEPBufCount(bEpNum)  &=0x83FFFFFF;\
*_pEPBufCount(bEpNum) |=Blocks<<26;\
*_pEPBufCount(bEpNum) &=0xFC00FFFF;\
}
#endif 
/*----------------------------------------------------------------*/
/* GetEPTxCount / GetEPRxCount */
/* gets counter of the tx buffer */
/* IN : bEpNum = endpoint number */
/* OUT: counter value */
/*----------------------------------------------------------------*/
#ifdef STR7xx /*STR7xx family*/
#define _GetEPTxCount(bEpNum)((u16)(*_pEPTxCount(bEpNum)) & 0x3ff)
#define _GetEPRxCount(bEpNum)((u16)(*_pEPRxCount(bEpNum)) & 0x3ff)
#endif 

#ifdef STR91x /*STR91x family*/
#define _GetEPTxCount(bEpNum) (u16)(*_pEPBufCount(bEpNum)&0x3FF)
#define _GetEPRxCount(bEpNum) (u16)((*_pEPBufCount(bEpNum)&0x3FF0000)>>16)
#endif 
/*----------------------------------------------------------------*/
/* SetEPDblBuf0Addr / SetEPDblBuf1Addr */
/* sets buffer 0/1 address in a double buffer endpoint */
/* IN : bEpNum = endpoint number */
/*      wBuf0Addr = buffer 0 address */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPDblBuf0Addr(bEpNum,wBuf0Addr) {_SetEPTxAddr(bEpNum, wBuf0Addr);}
#define _SetEPDblBuf1Addr(bEpNum,wBuf1Addr) {_SetEPRxAddr(bEpNum, wBuf1Addr);}

/*----------------------------------------------------------------*/
/* SetEPDblBuffAddr */
/* sets addresses in a double buffer endpoint */
/* IN : bEpNum = endpoint number */
/*      wBuf0Addr = buffer 0 address */
/*      wBuf1Addr = buffer 1 address */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _SetEPDblBuffAddr(bEpNum,wBuf0Addr,wBuf1Addr) { \
					_SetEPDblBuf0Addr(bEpNum, wBuf0Addr);\
					_SetEPDblBuf1Addr(bEpNum, wBuf1Addr);\
} /* _SetEPDblBuffAddr */
/*----------------------------------------------------------------*/
/* GetEPDblBuf0Addr / GetEPDblBuf1Addr */
/* gets buffer 0/1 address of a double buffer endpoint */
/* IN : bEpNum = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _GetEPDblBuf0Addr(bEpNum) (_GetEPTxAddr(bEpNum))
#define _GetEPDblBuf1Addr(bEpNum) (_GetEPRxAddr(bEpNum))
/*----------------------------------------------------------------*/
/* SetEPDblBuffCount / SetEPDblBuf0Count / SetEPDblBuf1Count */
/* sets both buffers or buff0 or buff1 counter for double buffering */
/* IN :         bEpNum  = endpoint number */
/*		bDir    = endpoint dir  EP_DBUF_OUT = OUT */
/*				        EP_DBUF_IN  = IN */
/*		wCount  = counter value     */
/* OUT: none */
/*----------------------------------------------------------------*/

#ifdef STR7xx /*STR7xx family*/

#define _SetEPDblBuf0Count(bEpNum, bDir, wCount)  { \
		 if(bDir == EP_DBUF_OUT)\
		     /* OUT endpoint */	\
		     {_SetEPRxDblBuf0Count(bEpNum,wCount);} \
		 else if(bDir == EP_DBUF_IN)\
		     /* IN endpoint */	\
			 *_pEPTxCount(bEpNum) = (u32)wCount;  \
} /* SetEPDblBuf0Count*/

#define _SetEPDblBuf1Count(bEpNum, bDir, wCount)  { \
		 if(bDir == EP_DBUF_OUT)\
		     /* OUT endpoint */	\
		     {_SetEPRxCount(bEpNum,wCount);}\
		 else if(bDir == EP_DBUF_IN)\
		     /* IN endpoint */\
			 *_pEPRxCount(bEpNum) = (u32)wCount; \
} /* SetEPDblBuf1Count */

#define _SetEPDblBuffCount(bEpNum, bDir, wCount) {\
			_SetEPDblBuf0Count(bEpNum, bDir, wCount); \
	 		_SetEPDblBuf1Count(bEpNum, bDir, wCount); \
} /* _SetEPDblBuffCount	 */
#endif 
/*----------------------------------------------------------------*/
/* GetEPDblBuf0Count / GetEPDblBuf1Count */
/* gets buffer 0/1 rx/tx counter for double buffering */
/* IN : bEpNum  = endpoint number */
/* OUT: none */
/*----------------------------------------------------------------*/
#define _GetEPDblBuf0Count(bEpNum) (_GetEPTxCount(bEpNum))
#define _GetEPDblBuf1Count(bEpNum) (_GetEPRxCount(bEpNum))


/* External variables --------------------------------------------------------*/
extern volatile u16 wIstr;  /* ISTR register last read value */

/* Exported functions ------------------------------------------------------- */
void SetCNTR(u16 /*wRegValue*/);
void SetISTR(u16 /*wRegValue*/);
void SetDADDR(u16 /*wRegValue*/);
void SetBTABLE(u16 /*wRegValue*/);
void SetBTABLE(u16 /*wRegValue*/);
u16 GetCNTR(void);
u16 GetISTR(void);
u16 GetFNR(void);
u16 GetDADDR(void);
u16 GetBTABLE(void);
void SetENDPOINT(u8 /*bEpNum*/,u16 /*wRegValue*/);
u16 GetENDPOINT(u8 /*bEpNum*/);
void SetEPType(u8 /*bEpNum*/,u16 /*wType*/);
u16 GetEPType(u8 /*bEpNum*/);
void SetEPTxStatus(u8 /*bEpNum*/,u16 /*wState*/);
void SetEPRxStatus(u8 /*bEpNum*/,u16 /*wState*/);
void SetDouBleBuffEPStall(u8 /*bEpNum*/,u8 bDir);
u16 GetEPTxStatus(u8 /*bEpNum*/);
u16 GetEPRxStatus(u8 /*bEpNum*/);
void SetEPTxValid(u8 /*bEpNum*/);
void SetEPRxValid(u8 /*bEpNum*/);
u16 GetTxStallStatus(u8 /*bEpNum*/);
u16 GetRxStallStatus(u8 /*bEpNum*/);
void SetEP_KIND(u8 /*bEpNum*/);
void ClearEP_KIND(u8 /*bEpNum*/);
void Set_Status_Out(u8 /*bEpNum*/);
void Clear_Status_Out(u8 /*bEpNum*/);
void SetEPDoubleBuff(u8 /*bEpNum*/);
void ClearEPDoubleBuff(u8 /*bEpNum*/);
void ClearEP_CTR_RX(u8 /*bEpNum*/);
void ClearEP_CTR_TX(u8 /*bEpNum*/);
void ToggleDTOG_RX(u8 /*bEpNum*/);
void ToggleDTOG_TX(u8 /*bEpNum*/);
void ClearDTOG_RX(u8 /*bEpNum*/);
void ClearDTOG_TX(u8 /*bEpNum*/);
void SetEPAddress(u8 /*bEpNum*/,u8 /*bAddr*/);
u8 GetEPAddress(u8 /*bEpNum*/);
void SetEPTxAddr(u8 /*bEpNum*/,u16 /*wAddr*/);
void SetEPRxAddr(u8 /*bEpNum*/,u16 /*wAddr*/);
u16 GetEPTxAddr(u8 /*bEpNum*/);
u16 GetEPRxAddr(u8 /*bEpNum*/);
void SetEPCountRxReg(u32 * /*pdwReg*/, u16 /*wCount*/);
void SetEPTxCount(u8 /*bEpNum*/,u16 /*wCount*/);
void SetEPRxCount(u8 /*bEpNum*/,u16 /*wCount*/);
u16 GetEPTxCount(u8 /*bEpNum*/);
u16 GetEPRxCount(u8 /*bEpNum*/);
void SetEPDblBuf0Addr(u8 /*bEpNum*/,u16 /*wBuf0Addr*/);
void SetEPDblBuf1Addr(u8 /*bEpNum*/,u16 /*wBuf1Addr*/);
void SetEPDblBuffAddr(u8 /*bEpNum*/,u16 /*wBuf0Addr*/,u16 /*wBuf1Addr*/);
u16 GetEPDblBuf0Addr(u8 /*bEpNum*/);
u16 GetEPDblBuf1Addr(u8 /*bEpNum*/);
void SetEPDblBuffCount(u8 /*bEpNum*/, u8 /*bDir*/, u16 /*wCount*/);
void SetEPDblBuf0Count(u8 /*bEpNum*/, u8 /*bDir*/, u16 /*wCount*/);
void SetEPDblBuf1Count(u8 /*bEpNum*/, u8 /*bDir*/, u16 /*wCount*/);
u16 GetEPDblBuf0Count(u8 /*bEpNum*/);
u16 GetEPDblBuf1Count(u8 /*bEpNum*/);
EP_DBUF_DIR GetEPDblBufDir(u8 /*bEpNum*/);
void FreeUserBuffer(u8 bEpNum/*bEpNum*/,u8 bDir);
u16 ToWord(u8,u8);
u16 ByteSwap(u16);

#ifdef STR91x /*STR91x family*/
/* DMA Functions */
void SetDMABurstTxSize(u8 /*DestBsize*/);
void SetDMABurstRxSize(u8 /*SrcBsize*/);
void DMAUnlinkedModeTxConfig(u8 /*bEpNum*/ ,u8 /*index*/);
void DMAUnlinkedModeTxEnable(u8 /*index*/);
void DMAUnlinkedModeTxDisable(u8 /*index*/);
void DMAUnlinkedModeRxEnable(u8 /*bEpNum*/);
void DMAUnlinkedModeRxDisable(u8 /*bEpNum*/);
void DMALinkedModeRxConfig(u8 /*bEpNum*/);
void DMALinkedModeTxConfig(u8 /*bEpNum*/);
void DMALinkedModeRxEnable(void);
void DMALinkedModeTxEnable(void);
void DMALinkedModeRxDisable(void);
void DMALinkedModeTxDisable(void);
void DMASynchEnable(void);
void DMASynchDisable(void);
void SetDMALLITxLength(u8 /*length*/);
void SetDMALLIRxLength(u8 /*length*/ );
void SetDMALLIRxPacketNum(u8 /*PacketNum*/);
u8 GetDMALLIRxPacketNum(void);
#endif /* End of STR91x family*/

#endif /* __USB_REGS_H */


/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
