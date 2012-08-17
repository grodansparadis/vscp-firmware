/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_can.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      CAN bus software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

#include "73x_can.h"
#include "73x_cfg.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* macro to format the timing register value from the timing parameters*/
#define CAN_TIMING(tseg1, tseg2, sjw, brp)	(((tseg2-1) & 0x07) << 12) | (((tseg1-1) & 0x0F) << 8) | (((sjw-1) & 0x03) << 6) | ((brp-1) & 0x3F)
/* Private variables ---------------------------------------------------------*/

/* array of pre-defined timing parameters for standard bitrates*/
u16 CanTimings[] = {             /* value   bitrate     NTQ  TSEG1  TSEG2  SJW  BRP */
	CAN_TIMING(11, 4, 4, 5), /* 0x3AC4  100 kbit/s  16   11     4      4    5   */
	CAN_TIMING(11, 4, 4, 4), /* 0x3AC3  125 kbit/s  16   11     4      4    4   */
	CAN_TIMING( 4, 3, 3, 4), /* 0x2383  250 kbit/s   8    4     3      3    4   */
	CAN_TIMING(13, 2, 1, 1), /* 0x1C00  500 kbit/s  16   13     2      1    1   */
	CAN_TIMING( 4, 3, 1, 1), /* 0x2300  1 Mbit/s     8    4     3      1    1   */
};

/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**********************************************************************************/
/* Function Name  : CAN_DeInit                                                    */
/* Description    : Deinitializes the CANx peripheral registers to their default  */
/*                  reset values.                                                 */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral   */
/* Output         : None                                                          */
/* Return         : None                                                          */
/**********************************************************************************/
void CAN_DeInit (CAN_TypeDef *CANx)
{

 if(CANx == CAN0)
  {
    CFG_PeripheralClockConfig(CFG_CLK_CAN0,DISABLE);
    CFG_PeripheralClockConfig(CFG_CLK_CAN0,ENABLE);
  }
  else if  (CANx == CAN1)
  {

    CFG_PeripheralClockConfig(CFG_CLK_CAN1,DISABLE);
    CFG_PeripheralClockConfig(CFG_CLK_CAN1,ENABLE);
  }
  else if  (CANx == CAN2)
  {
    CFG_PeripheralClockConfig(CFG_CLK_CAN2,DISABLE);
    CFG_PeripheralClockConfig(CFG_CLK_CAN2,ENABLE);
  }

}

/*********************************************************************************/
/* Function Name  : CAN_Init                                                     */
/* Description    : Configure CAN                                                */
/* Input          : - CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                : - [CAN_InitStruct] Initialization structure {Mask, Bitrate}  */
/* Output         : None                                                         */
/* Return         : None                                                         */
/*********************************************************************************/
void CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef* CAN_InitStruct)
{
  CAN_EnterInitMode(CANx, CAN_CR_CCE | CAN_InitStruct->CAN_Mask);
  CAN_SetBitrate(CANx, CAN_InitStruct->CAN_Bitrate);
  CAN_LeaveInitMode(CANx);
  CAN_LeaveTestMode(CANx);
}


/*********************************************************************************/
/* Function Name       : CAN_StructInit		      		                 */
/* Description         : Initialize the CAN Init Structure		         */
/* Input               : CAN_InitStruct : pointer to a CAN_InitTypeDef structure */
/*                       which will be initialized.                              */
/* Return              : None.			                                 */
/*********************************************************************************/
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct)
{
/* Reset CAN init structure parameters values */
  CAN_InitStruct->CAN_Mask = 0x0;
  CAN_InitStruct->CAN_Bitrate = 0x2301;
}

/*********************************************************************************/
/* Function Name  : CAN_SetBitrate                                               */
/* Description    : Setup a standard CAN bitrate                                 */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*		            -One of the CAN_BITRATE_xxx defines                  */
/* Output         : None                                                         */
/* Return         : None                                                         */
/* Note           : CAN must be in initialization mode                           */
/*********************************************************************************/
void CAN_SetBitrate(CAN_TypeDef *CANx, u32 bitrate)
{
   CANx->BTR = CanTimings[bitrate];  /*write the predefined timing value*/
   CANx->BRPR = 0; 		     /*clear the Extended Baud Rate Prescaler*/
}

/********************************************************************************/
/* Function Name  : CAN_SetTiming                                               */
/* Description    : Setup the CAN timing with specific parameters               */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Time Segment before the sample point position, from 1 to 16*/
/*                  -Time Segment after the sample point position, from 1 to 8  */
/*                  -Synchronisation Jump Width, from 1 to 4                    */
/*                  -Baud Rate Prescaler, from 1 to 1024                        */
/* Output         : None                                                        */
/* Return         : None                                                        */
/* Note           : CAN must be in initialization mode                          */
/***************************************************************************** **/
void CAN_SetTiming(CAN_TypeDef *CANx, u32 tseg1, u32 tseg2, u32 sjw, u32 brp)
{
	CANx->BTR = CAN_TIMING(tseg1, tseg2, sjw, brp);
	CANx->BRPR = ((brp-1) >> 6) & 0x0F;
}

/*******************************************************************************/
/* Function Name  : CAN_GetFreeIF                                              */
/* Description    : Search the first free message interface, starting from 0   */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/* Output         : None                                                       */
/* Return         : A free message interface number (0 or 1) if found, else 2  */
/*******************************************************************************/
static u32 CAN_GetFreeIF(CAN_TypeDef *CANx)
{
	if ((CANx->sMsgObj[0].CRR & CAN_CRR_BUSY) == 0)
		return 0;
	else if ((CANx->sMsgObj[1].CRR & CAN_CRR_BUSY) == 0)
		return 1;
	else return 2;
}

/*-------------------------------------------------------------------------------------*/
/* Macro Name     : xxx_ID_MSK, xxx_ID_ARB                                             */
/* Description    : Form the Mask and Arbitration registers value to filter a range    */
/*                  of identifiers or a fixed identifier, for standard and extended IDs*/
/*-------------------------------------------------------------------------------------*/
#define RANGE_ID_MSK(range_start, range_end)	(~((range_end) - (range_start)))
#define RANGE_ID_ARB(range_start, range_end)	((range_start) & (range_end))

#define FIXED_ID_MSK(id)	RANGE_ID_MSK((id), (id))
#define FIXED_ID_ARB(id)	RANGE_ID_ARB((id), (id))

#define STD_RANGE_ID_MSK(range_start, range_end)	((u16)((RANGE_ID_MSK((range_start), (range_end)) & 0x7FF) << 2))
#define STD_RANGE_ID_ARB(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) << 2))

#define STD_FIXED_ID_MSK(id)	((u16)((FIXED_ID_MSK(id) & 0x7FF) << 2))
#define STD_FIXED_ID_ARB(id)	((u16)(FIXED_ID_ARB(id) << 2))

#define EXT_RANGE_ID_MSK_L(range_start, range_end)	((u16)(RANGE_ID_MSK((range_start), (range_end)) >> 11))
#define EXT_RANGE_ID_MSK_H(range_start, range_end)	((u16)(STD_RANGE_ID_MSK((range_start), (range_end)) | ((RANGE_ID_MSK((range_start), (range_end)) >> 27) & 0x03)))
#define EXT_RANGE_ID_ARB_L(range_start, range_end)	((u16)(RANGE_ID_ARB((range_start), (range_end)) >> 11))
#define EXT_RANGE_ID_ARB_H(range_start, range_end)	((u16)(STD_RANGE_ID_ARB((range_start), (range_end)) | ((RANGE_ID_ARB((range_start), (range_end)) >> 27) & 0x03)))

#define EXT_FIXED_ID_MSK_L(id)	((u16)(FIXED_ID_MSK(id) >> 11))
#define EXT_FIXED_ID_MSK_H(id)	((u16)(STD_FIXED_ID_MSK(id) | ((FIXED_ID_MSK(id) >> 27) & 0x03)))
#define EXT_FIXED_ID_ARB_L(id)	((u16)(FIXED_ID_ARB(id) >> 11))
#define EXT_FIXED_ID_ARB_H(id)	((u16)(STD_FIXED_ID_ARB(id) | ((FIXED_ID_ARB(id) >> 27) & 0x03)))

/*********************************************************************************/
/* Function Name  : CAN_SetUnusedMsgObj                                          */
/* Description    : Configures the message object as unused                      */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*		    -Message object number, from 0 to 31                         */
/* Output         : None                                                         */
/* Return         : None                                                         */
/*********************************************************************************/
void CAN_SetUnusedMsgObj(CAN_TypeDef *CANx, u32 msgobj)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF(CANx)) == 2);

  	CANx->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
  	                            | CAN_CMR_MASK
  	                            | CAN_CMR_ARB
  	                            | CAN_CMR_CONTROL
  	                            | CAN_CMR_DATAA
  	                            | CAN_CMR_DATAB;

  	CANx->sMsgObj[msg_if].M1R = 0;
  	CANx->sMsgObj[msg_if].M2R = 0;

  	CANx->sMsgObj[msg_if].A1R = 0;
  	CANx->sMsgObj[msg_if].A2R = 0;

  	CANx->sMsgObj[msg_if].MCR = 0;

  	CANx->sMsgObj[msg_if].DA1R = 0;
  	CANx->sMsgObj[msg_if].DA2R = 0;
  	CANx->sMsgObj[msg_if].DB1R = 0;
  	CANx->sMsgObj[msg_if].DB2R = 0;

  	CANx->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/*********************************************************************************/
/* Function Name  : CAN_SetTxMsgObj                                              */
/* Description    : Configure the message object as TX                           */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*                  -Message object number, from 0 to 31                         */
/*                  -CAN_STD_ID or CAN_EXT_ID                                    */
/* Output         : None                                                         */
/* Return         : None                                                         */
/*********************************************************************************/
void CAN_SetTxMsgObj(CAN_TypeDef *CANx, u32 msgobj, u32 idType)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF(CANx)) == 2);

  	CANx->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
  	                            | CAN_CMR_MASK
  	                            | CAN_CMR_ARB
  	                            | CAN_CMR_CONTROL
  	                            | CAN_CMR_DATAA
  	                            | CAN_CMR_DATAB;

	CANx->sMsgObj[msg_if].M1R = 0;
	CANx->sMsgObj[msg_if].A1R = 0;

	if (idType == CAN_STD_ID)
	{
  		CANx->sMsgObj[msg_if].M2R = CAN_M2R_MDIR;
  		CANx->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR;
  	}
  	else
  	{
  		CANx->sMsgObj[msg_if].M2R = CAN_M2R_MDIR | CAN_M2R_MXTD;
  		CANx->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_DIR | CAN_A2R_XTD;
  	}

  	CANx->sMsgObj[msg_if].MCR = CAN_MCR_TXIE | CAN_MCR_EOB;

  	CANx->sMsgObj[msg_if].DA1R = 0;
  	CANx->sMsgObj[msg_if].DA2R = 0;
  	CANx->sMsgObj[msg_if].DB1R = 0;
  	CANx->sMsgObj[msg_if].DB2R = 0;

  	CANx->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/**************************************************************************************/
/* Function Name  : CAN_SetRxMsgObj                                                   */
/* Description    : Configure the message object as RX                                */
/* Input          : - CANx: where x can be 0, 1 or 2 to select the CAN peripheral     */
/*                  - Message object number, from 0 to 31                             */
/*                  - CAN_STD_ID or CAN_EXT_ID                                        */
/*                  - low part of the identifier range used for acceptance filtering  */
/*                  - high part of the identifier range used for acceptance filtering */
/*                  - TRUE for a single receive object or a FIFO receive object that  */
/*                    is the last one of the FIFO                                     */
/*                  - FALSE for a FIFO receive object that is not the last one        */
/* Output         : None                                                              */
/* Return         : None                                                              */
/**************************************************************************************/
void CAN_SetRxMsgObj(CAN_TypeDef *CANx, u32 msgobj, u32 idType, u32 idLow, u32 idHigh, bool singleOrFifoLast)
{
	u32 msg_if;
    while ((msg_if = CAN_GetFreeIF(CANx)) == 2);

  	CANx->sMsgObj[msg_if].CMR = CAN_CMR_WRRD
  	                            | CAN_CMR_MASK
  	                            | CAN_CMR_ARB
  	                            | CAN_CMR_CONTROL
  	                            | CAN_CMR_DATAA
  	                            | CAN_CMR_DATAB;

	if (idType == CAN_STD_ID)
	{
  		CANx->sMsgObj[msg_if].M1R = 0;
  		CANx->sMsgObj[msg_if].M2R = STD_RANGE_ID_MSK(idLow, idHigh);

  		CANx->sMsgObj[msg_if].A1R = 0;
  		CANx->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | STD_RANGE_ID_ARB(idLow, idHigh);
  	}
  	else
  	{
  		CANx->sMsgObj[msg_if].M1R = EXT_RANGE_ID_MSK_L(idLow, idHigh);
  		CANx->sMsgObj[msg_if].M2R = CAN_M2R_MXTD | EXT_RANGE_ID_MSK_H(idLow, idHigh);

  		CANx->sMsgObj[msg_if].A1R = EXT_RANGE_ID_ARB_L(idLow, idHigh);
  		CANx->sMsgObj[msg_if].A2R = CAN_A2R_MSGVAL | CAN_A2R_XTD | EXT_RANGE_ID_ARB_H(idLow, idHigh);
  	}

  	CANx->sMsgObj[msg_if].MCR = CAN_MCR_RXIE | CAN_MCR_UMASK | (singleOrFifoLast ? CAN_MCR_EOB : 0);

  	CANx->sMsgObj[msg_if].DA1R = 0;
  	CANx->sMsgObj[msg_if].DA2R = 0;
  	CANx->sMsgObj[msg_if].DB1R = 0;
  	CANx->sMsgObj[msg_if].DB2R = 0;

  	CANx->sMsgObj[msg_if].CRR = 1 + msgobj;
}

/*******************************************************************************/
/* Function Name  : CAN_InvalidateAllMsgObj				                       */
/* Description    : Configure all the message objects as unused                */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/* Output         : None                                                       */
/* Return         : None                                                       */
/*******************************************************************************/
void CAN_InvalidateAllMsgObj(CAN_TypeDef *CANx)
{
	u32 i;
	for (i = 32; i != 0; i--)
		CAN_SetUnusedMsgObj(CANx, 32-i);
}


/*********************************************************************************/
/* Function Name  : CAN_ReleaseMessage					                         */
/* Description    : Release the message object                                   */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*                  -Message object number, from 0 to 31                         */
/* Output         : None                                                         */
/* Return         : None                                                         */
/*********************************************************************************/
void CAN_ReleaseMessage(CAN_TypeDef *CANx, u32 msgobj)
{
	u32 i;
    while ((i = CAN_GetFreeIF(CANx)) == 2);

	CANx->sMsgObj[i].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
	CANx->sMsgObj[i].CRR = 1 + msgobj;
}

/**********************************************************************************/
/* Function Name  : CAN_SendMessage                                               */
/* Description    : Start transmission of a message                               */
/* Input          : - CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*                  - Message object number, from 0 to 31                         */
/*                : - Pointer to the message structure containing data to transmit*/
/* Output         : None                                                          */
/* Return         : 1 if transmission was OK, else 0                              */
/**********************************************************************************/
u32 CAN_SendMessage(CAN_TypeDef *CANx, u32 msgobj, canmsg* pCanMsg)
{
  while (CANx->sMsgObj[0].CRR & CAN_CRR_BUSY);

  CANx->SR &= ~CAN_SR_TXOK;

  /* read the Arbitration and Message Control*/
  CANx->sMsgObj[0].CMR = CAN_CMR_ARB | CAN_CMR_CONTROL;

  CANx->sMsgObj[0].CRR = 1 + msgobj;

  while (CANx->sMsgObj[0].CRR & CAN_CRR_BUSY);

  /* update the contents needed for transmission*/
  CANx->sMsgObj[0].CMR = CAN_CMR_WRRD
  	                 | CAN_CMR_ARB
  	                 | CAN_CMR_CONTROL
  	                 | CAN_CMR_DATAA
  	                 | CAN_CMR_DATAB;

  if ((CANx->sMsgObj[0].A2R & CAN_A2R_XTD) == 0)
  {
  	/* standard ID*/
	CANx->sMsgObj[0].A1R = 0;
	CANx->sMsgObj[0].A2R = (CANx->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
  }
  else
  {
  	/* extended ID*/
	CANx->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
	CANx->sMsgObj[0].A2R = (CANx->sMsgObj[0].A2R & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
  }

  CANx->sMsgObj[0].MCR = (CANx->sMsgObj[0].MCR & 0xFEF0) | CAN_MCR_NEWDAT | CAN_MCR_TXRQST | pCanMsg->Dlc;

  CANx->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
  CANx->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
  CANx->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
  CANx->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];

  CANx->sMsgObj[0].CRR = 1 + msgobj;

  return 1;
}

/************************************************************************************/
/* Function Name  : CAN_ReceiveMessage                                              */
/* Description    : Get the message, if received                                    */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral    */
/*                   -Message object number, from 0 to 31                           */
/*                  -if TRUE, the message object is released when getting the data  */
/*                   if FALSE, the message object is not released                   */
/*                  -Pointer to the message structure where received data is stored */
/* Output         : None                                                            */
/* Return         : 1 if reception was OK, else 0 (no message pending)              */
/************************************************************************************/
u32 CAN_ReceiveMessage(CAN_TypeDef *CANx, u32 msgobj, bool release, canmsg* pCanMsg)
{
   u32  tmpId0 = 0;
   u32  tmpId1 = 0;

   if (!CAN_IsMessageWaiting(CANx, msgobj))
    	return 0;

	CANx->SR &= ~CAN_SR_RXOK;

    /* read the message contents*/
    CANx->sMsgObj[1].CMR = CAN_CMR_MASK
	                   | CAN_CMR_ARB
	                   | CAN_CMR_CONTROL
	                   | CAN_CMR_CLRINTPND
	                   | (release ? CAN_CMR_TXRQSTNEWDAT : 0)
	                   | CAN_CMR_DATAA
	                   | CAN_CMR_DATAB;

	CANx->sMsgObj[1].CRR = 1 + msgobj;

	while (CANx->sMsgObj[1].CRR & CAN_CRR_BUSY);

    if ((CANx->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
    {
  	/* standard ID*/
  	pCanMsg->IdType = CAN_STD_ID;
      	pCanMsg->Id = (CANx->sMsgObj[1].A2R >> 2) & 0x07FF;
    }
    else
    {
        /* extended ID*/
        pCanMsg->IdType = CAN_EXT_ID;
        tmpId0 = ((u32)CANx->sMsgObj[1].A1R << 11);
        tmpId1 = (((u32)CANx->sMsgObj[1].A2R & 0x0003) << 27);
        pCanMsg->Id = ((CANx->sMsgObj[1].A2R >> 2) & 0x07FF) | tmpId0 |tmpId1 ;
    }

    pCanMsg->Dlc = CANx->sMsgObj[1].MCR & 0x0F;

    pCanMsg->Data[0] = (u8) CANx->sMsgObj[1].DA1R;
    pCanMsg->Data[1] = (u8)(CANx->sMsgObj[1].DA1R >> 8);
    pCanMsg->Data[2] = (u8) CANx->sMsgObj[1].DA2R;
    pCanMsg->Data[3] = (u8)(CANx->sMsgObj[1].DA2R >> 8);
    pCanMsg->Data[4] = (u8) CANx->sMsgObj[1].DB1R;
    pCanMsg->Data[5] = (u8)(CANx->sMsgObj[1].DB1R >> 8);
    pCanMsg->Data[6] = (u8) CANx->sMsgObj[1].DB2R;
    pCanMsg->Data[7] = (u8)(CANx->sMsgObj[1].DB2R >> 8);

    return 1;
}

/********************************************************************************/
/* Function Name  : CAN_WaitEndOfTx                                             */
/* Description    : Wait until current transmission is finished                 */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/* Output         : None                                                        */
/* Return         : None                                                        */
/********************************************************************************/
void CAN_WaitEndOfTx(CAN_TypeDef *CANx)
{
	while ((CANx->SR & CAN_SR_TXOK) == 0);
	CANx->SR &= ~CAN_SR_TXOK;
}

/*********************************************************************************/
/* Function Name  : CAN_BasicSendMessage                                         */
/* Description    : Start transmission of a message in BASIC mode                */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/*                  -Pointer to the message structure containing data to transmit*/
/* Output         : None                                                         */
/* Return         : 1 if transmission was OK, else 0                             */
/* Note           : CAN must be in BASIC mode                                    */
/*********************************************************************************/
u32 CAN_BasicSendMessage(CAN_TypeDef *CANx, canmsg* pCanMsg)
{
	/* clear NewDat bit in IF2 to detect next reception*/
	CANx->sMsgObj[1].MCR &= ~CAN_MCR_NEWDAT;

	CANx->SR &= ~CAN_SR_TXOK;

	CANx->sMsgObj[0].CMR = CAN_CMR_WRRD
  	                       | CAN_CMR_ARB
  	                       | CAN_CMR_CONTROL
  	                       | CAN_CMR_DATAA
  	                       | CAN_CMR_DATAB;

	if (pCanMsg->IdType == CAN_STD_ID)
	{
	  	/* standard ID*/
		CANx->sMsgObj[0].A1R = 0;
		CANx->sMsgObj[0].A2R = (CANx->sMsgObj[0].A2R & 0xE000) | STD_FIXED_ID_ARB(pCanMsg->Id);
	}
	else
	{
  		/* extended ID*/
		CANx->sMsgObj[0].A1R = EXT_FIXED_ID_ARB_L(pCanMsg->Id);
		CANx->sMsgObj[0].A2R = ((CANx->sMsgObj[0].A2R) & 0xE000) | EXT_FIXED_ID_ARB_H(pCanMsg->Id);
	}

	CANx->sMsgObj[0].MCR = (CANx->sMsgObj[0].MCR & 0xFCF0) | pCanMsg->Dlc;

	CANx->sMsgObj[0].DA1R = ((u16)pCanMsg->Data[1]<<8) | pCanMsg->Data[0];
	CANx->sMsgObj[0].DA2R = ((u16)pCanMsg->Data[3]<<8) | pCanMsg->Data[2];
	CANx->sMsgObj[0].DB1R = ((u16)pCanMsg->Data[5]<<8) | pCanMsg->Data[4];
	CANx->sMsgObj[0].DB2R = ((u16)pCanMsg->Data[7]<<8) | pCanMsg->Data[6];

	/* request transmission*/
	CANx->sMsgObj[0].CRR = CAN_CRR_BUSY | (1 + 0);

	return 1;
}

/***********************************************************************************/
/* Function Name  : CAN_BasicReceiveMessage                                        */
/* Description    : Get the message in BASIC mode, if received                     */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral   */
/*                  -Pointer to the message structure where received data is stored*/
/* Output         : None                                                           */
/* Return         : 1 if reception was OK, else 0 (no message pending)             */
/* Note           : CAN must be in BASIC mode                                      */
/***********************************************************************************/
u32 CAN_BasicReceiveMessage(CAN_TypeDef *CANx, canmsg* pCanMsg)
{
   u32  tmpId0 = 0;
   u32  tmpId1 = 0;

        if ((CANx->sMsgObj[1].MCR & CAN_MCR_NEWDAT) == 0)
		return 0;

	CANx->SR &= ~CAN_SR_RXOK;

	CANx->sMsgObj[1].CMR = CAN_CMR_ARB
  	                       | CAN_CMR_CONTROL
  	                       | CAN_CMR_DATAA
  	                       | CAN_CMR_DATAB;

    if ((CANx->sMsgObj[1].A2R & CAN_A2R_XTD) == 0)
    {
  	  /* standard ID*/
  	  pCanMsg->IdType = CAN_STD_ID;
          pCanMsg->Id = (CANx->sMsgObj[1].A2R >> 2) & 0x07FF;
    }
    else
    {
          /* extended ID*/
  	  pCanMsg->IdType = CAN_EXT_ID;
          tmpId0 = ((u32)CANx->sMsgObj[1].A1R << 11);
          tmpId1 = (((u32)CANx->sMsgObj[1].A2R & 0x0003) << 27);
          pCanMsg->Id = ((CANx->sMsgObj[1].A2R >> 2) & 0x07FF) |tmpId0 | tmpId1;
    }

    pCanMsg->Dlc = CANx->sMsgObj[1].MCR & 0x0F;

    pCanMsg->Data[0] = (u8) CANx->sMsgObj[1].DA1R;
    pCanMsg->Data[1] = (u8)(CANx->sMsgObj[1].DA1R >> 8);
    pCanMsg->Data[2] = (u8) CANx->sMsgObj[1].DA2R;
    pCanMsg->Data[3] = (u8)(CANx->sMsgObj[1].DA2R >> 8);
    pCanMsg->Data[4] = (u8) CANx->sMsgObj[1].DB1R;
    pCanMsg->Data[5] = (u8)(CANx->sMsgObj[1].DB1R >> 8);
    pCanMsg->Data[6] = (u8) CANx->sMsgObj[1].DB2R;
    pCanMsg->Data[7] = (u8)(CANx->sMsgObj[1].DB2R >> 8);

    return 1;
}


/********************************************************************************/
/* Function Name  : CAN_EnterInitMode                                           */
/* Description    : Switch the CAN into initialization mode                     */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Any binary value formed from the CAN_CR_xxx defines        */
/* Output         : None                                                        */
/* Return         : None                                                        */
/* Note           : CAN_LeaveInitMode must be called when all is done           */
/********************************************************************************/
void CAN_EnterInitMode(CAN_TypeDef *CANx, u8 mask)
{
	CANx->CR = mask | CAN_CR_INIT;
	CANx->SR = 0;					/* reset the status*/
}

/********************************************************************************/
/* Function Name  : CAN_LeaveInitMode                                           */
/* Description    : Leave the initialization mode (switch into normal mode)     */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral */
/* Output         : None                                                        */
/* Return         : None                                                        */
/********************************************************************************/
void CAN_LeaveInitMode(CAN_TypeDef *CANx)
{
	CANx->CR &= ~(CAN_CR_INIT | CAN_CR_CCE);
}

/********************************************************************************/
/* Function Name  : CAN_EnterTestMode                                           */
/* Description    : Switch the CAN into test mode                               */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Any binary value formed from the CAN_TST_xxx defines       */
/* Output         : None                                                        */
/* Return         : None                                                        */
/* Note           : CAN_LeaveTestMode must be called when all is done           */
/********************************************************************************/
void CAN_EnterTestMode(CAN_TypeDef *CANx, u8 mask)
{
	CANx->CR |= CAN_CR_TEST;
	CANx->TESTR |= mask;
}

/*******************************************************************************/
/* Function Name  : CAN_LeaveTestMode                                          */
/* Description    : Leave the current test mode (switch into normal mode)      */
/* Input          : CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/* Output         : None                                                       */
/* Return         : None                                                       */
/*******************************************************************************/
void CAN_LeaveTestMode(CAN_TypeDef *CANx)
{
	CANx->CR |= CAN_CR_TEST;
	CANx->TESTR &= ~(CAN_TESTR_LBACK | CAN_TESTR_SILENT | CAN_TESTR_BASIC);
	CANx->CR &= ~CAN_CR_TEST;
}

/********************************************************************************/
/* Function Name  : CAN_ReleaseTxMessage                                        */
/* Description    : Release the transmit message object                         */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Message object number, from 0 to 31                        */
/* Output         : None                                                        */
/* Return         : None                                                        */
/* Note           : assume that message interface 0 is free                     */
/********************************************************************************/
void CAN_ReleaseTxMessage(CAN_TypeDef *CANx, u32 msgobj)
{
	CANx->sMsgObj[0].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
	CANx->sMsgObj[0].CRR = 1 + msgobj;
}

/********************************************************************************/
/* Function Name  : CAN_ReleaseRxMessage                                        */
/* Description    : Release the receive message object                          */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Message object number, from 0 to 31                        */
/* Output         : None                                                        */
/* Return         : None                                                        */
/* Note           : assume that message interface 1 is free                     */
/********************************************************************************/
void CAN_ReleaseRxMessage(CAN_TypeDef *CANx, u32 msgobj)
{
	CANx->sMsgObj[1].CMR = CAN_CMR_CLRINTPND | CAN_CMR_TXRQSTNEWDAT;
	CANx->sMsgObj[1].CRR = 1 + msgobj;
}

/********************************************************************************/
/* Function Name  : CAN_IsMessageWaiting                                        */
/* Description    : Test the waiting status of a received message               */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Message object number, from 0 to 31                        */
/* Output         : None                                                        */
/* Return         : A non-zero value if the corresponding message object has    */
/*                  received a message waiting to be copied, else 0             */
/********************************************************************************/
u32 CAN_IsMessageWaiting(CAN_TypeDef *CANx, u32 msgobj)
{
	return (msgobj < 16 ? CANx->ND1R & (1 << msgobj) : CANx->ND2R & (1 << (msgobj-16)));
}

/********************************************************************************/
/* Function Name  : CAN_IsTransmitRequested                                     */
/* Description    : Test the request status of a transmitted message            */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral*/
/*                  -Message object number, from 0 to 31                        */
/* Output         : None                                                        */
/* Return         : A non-zero value if the corresponding message is requested  */
/*                  to transmit, else 0                                         */
/********************************************************************************/
u32 CAN_IsTransmitRequested(CAN_TypeDef *CANx, u32 msgobj)
{
	return (msgobj < 16 ? CANx->TXR1R & (1 << msgobj) : CANx->TXR2R & (1 << (msgobj-16)));
}

/**********************************************************************************/
/* Function Name  : CAN_IsInterruptPending                                        */
/* Description    : Test the interrupt status of a message object                 */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral  */
/*                  -Message object number, from 0 to 31                          */
/* Output         : None                                                          */
/* Return         : A non-zero value if the corresponding message has an interrupt*/
/*                  pending, else 0                                               */
/**********************************************************************************/
u32 CAN_IsInterruptPending(CAN_TypeDef *CANx, u32 msgobj)
{
	return (msgobj < 16 ? CANx->IP1R & (1 << msgobj) : CANx->IP2R & (1 << (msgobj-16)));
}

/**********************************************************************************/
/* Function Name  : CAN_IsObjectValid                                             */
/* Description    : Test the validity of a message object (ready to use)          */
/* Input          : -CANx: where x can be 0, 1 or 2 to select the CAN peripheral  */
/*                  -Message object number, from 0 to 31                          */
/* Output         : None                                                          */
/* Return         : A non-zero value if the corresponding message object is valid,*/
/*                  else 0                                                        */
/**********************************************************************************/
u32 CAN_IsObjectValid(CAN_TypeDef *CANx, u32 msgobj)
{
	return (msgobj < 16 ? CANx->MV1R & (1 << msgobj) : CANx->MV2R & (1 << (msgobj-16)));
}


/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
