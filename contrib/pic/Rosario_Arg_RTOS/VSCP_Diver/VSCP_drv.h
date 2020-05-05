//********************************************************************
//                                                                    
// File name: VSCP_drv.h                                               
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   1.0                                 			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   GGeneral VSCP CAN Driver             
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file from PICos18 drv_can. Only change msg struct             
//                                                                    
//            PICos18 is free software; you can redistribute it       
//            and/or modify it under the terms of the GNU General     
//            Public License as published by the Free Software        
//            Foundation; either version 2, or (at your option)       
//            any later version.                                      
//                                                                    
//            PICos18 is distributed in the hope that it will be      
//            useful, but WITHOUT ANY WARRANTY; without even the      
//            implied warranty of MERCHANTABILITY or FITNESS FOR A    
//            PARTICULAR PURPOSE.  See the GNU General Public         
//            License for more details.                               
//                                                                    
//            You should have received a copy of the GNU General      
//            Public License along with gpsim; see the file           
//            COPYING.txt. If not, write to the Free Software         
//            Foundation, 59 Temple Place - Suite 330,                
//            Boston, MA 02111-1307, USA.                             
//                                                                    
//          > A special exception to the GPL can be applied should    
//            you wish to distribute a combined work that includes    
//            PICos18, without being obliged to provide the source    
//            code for any proprietary components.                    
//******************************************************************************


#ifndef _VSCP_DRV_H_
#define _VSCP_DRV_H_

#include "pro_man.h"
#include "even_man.h"
#include "alarm.h"

#include "TypeDefs.h"
/*****************************************************
 * Definition dedicated to the local functions.
 ****************************************************/
#define VSCP_FREE           0x00 // state of message
#define VSCP_FULL           0x01
#define VSCP_MSG_SENT       0x02

#define VSCP_RX_QUEUE_SIZE  0x04 // queue size
#define VSCP_TX_QUEUE_SIZE  0x04 
	
typedef struct _VSCP_frame {
	uint08_t priority;	// Priority for the message 0-7	
	uint16_t class;			// VSCP class
	uint08_t type;			// VSCP type
	uint08_t nickname;	// VSCP originatin node address
  uint08_t length;		// Data length
  uint08_t data[8];		// Data
} VSCP_msg_t, *VSCP_msg_tRef;


////////////////////////////////////////////////////////////////////////////////////
//
//	HARDWARE OVERLAY TYPES......
//	
//	HARDWARE........
//
//	Declare a type that matches the structure of the TX and RX
//	buffers so that we can overlay the structure variables into
//	the correct locations within the buffer.

//
//	TX HW Buffer looks like this
typedef volatile near struct _TXBUF{
	uint08_t	TXCON;
	uint08_t	SIDH;
	uint08_t	SIDL;
	uint08_t	EIDH;
	uint08_t	EIDL;
	uint08_t	DLC;
	uint08_t	DATA[8];
	uint08_t	CANSTAT;
	uint08_t	blank;
} TXBUF_t,	*pTXBUF_t;


//
//	TX HARDWARE BUFFERS
#define TXBUF_START		((TXBUF_t * )&TXB0CON)	//	See CAN Controller Register Map....
#define TXBUF_END		((TXBUF_t * )&TXB2CON)		//	Table 19-1 perhaps?
//
//	RX HW Buffer looks like this.....
typedef volatile near struct _RXBUF {
	uint08_t	RXCON;
	uint08_t	SIDH;
	uint08_t	SIDL;
	uint08_t	EIDH;
	uint08_t	EDIL;
	uint08_t	DLC;
	uint08_t	data[8];
	uint08_t	CANSTAT;
}RXBUF_t,	*pRXBUF_t;

#define	RXBUF_START		((pRXBUF_t)&RXB0CON)	//	See CAN Controller Register Map
#define	RXBUF_END		((pRXBUF_t)&RXB1CON)		//	Table 19-1
#define	RXBUF_STEP		0x01
#define	RX_BUFFER_FULL	0b10000000
//								^-- Mask applied to RXB?CON to test for buffer full

//////////////////////////////////////////////////////////////////////////////////////////
////	Generic RX Filter and Mask type
////
////	It looks like all of the filters follow this type of repeating pattern
////	within the register area.  Use that to our advantage to define
////	an overlay for this area too...
//typedef volatile near struct _RXFM {
//	uint08_t	SIDH;
//	uint08_t	SIDL;
//	uint08_t	EIDH;
//	uint08_t	EIDL;
//}RXFilterMask_t,	*pRXFilterMask_t;
//
//#define	RXFILTER_START	0xF00			//	See CAN Controller Register Map
//#define	RXFILTER_END		0xF1C			//	Table 19-1
//#define	RXFILTER_STEP		0x04
//

////////////////////////////////////////////////////////////////////////////////////
////
////	Function macros for Transmit Buffer Control
////
//#define		TX_ABORTED(psTXBuf)				((psTXBuf->TXCON)&0x40)
//#define		TX_LOSTARB(psTXBuf)				((psTXBuf->TXCON)&0x20)
//#define		TX_ERROR(psTXBuf)					((psTXBuf->TXCON)&0x10)
#define		SET_TXREQ(psTXBuf)				((psTXBuf->TXCON) |= 0x08)
//#define		CLEAR_TXREQ(psTXBuf)				((psTXBuf->TXCON) &= (~(1<<4)))
//#define		GET_TXPRIORITY(psTXBuf)			((psTXBuf->TXCON) & 0x03)			
//#define		SET_TXPRIORITY(psTXBuf,u8Priority)	((psTXBuf->TXCON) = u8Priority & 0x03)
//
//
//
////	Add some fancy macro definitions for 11-bit identifiers
////	Example usage: regularID = GET_ID(&RX0BUFFER);
//#define		GET_TXID(psTXBuf)					(((psTXBuf->SIDH) << 3) + ((psTXBuf->SIDL & 0x0E) >> 5))
//#define		SET_TXID(psTXBuf,u16ID)			psTXBuf->SIDH=((u16ID&0x7FF)>>3);psTXBuf->SIDL = ((u16ID & 0x07)<< 5)
//#define		SET_TXDLC(psTXBuf,u8Length)		psTXBuf->DLC = (u8Length & 0x0F)
//
///////////////////////////////////////////////////////////////////////////////////////////
////
////	Function macros for Receive Buffer Control
////
//
//#define		GET_RXID(psRXBuf)				(((unsigned long)(psRXBuf->SIDH)<<3)|(((unsigned long)(psRXBuf->SIDL)>>5)&0x07))
//#define 	GET_RXDLC(psRXBuf)				(psRXBuf->DLC & 0x0F)
//
//#define		RX_IS_FULL(psRXBuf)				((psRXBuf->RXCON) & 0x80)
//#define		GET_RXMODE(psRXBuf)				(((psRXBuf->RXCON) &0x60) >> 5)
//#define		SET_RXMODE(psRXBuf,u8Mode)		(psRXBuf->RXCON = ((u8Mode) & 0x03) << 5)
//#define		IS_RTR(psRXBuf)					(((psRXBuf->RXCON) & 0x04) >> 3)
//#define		SET_RXDBEN						(RXB0CON |= 0x04)


/////////////////////////////////////////////////////////////////////////////////////////
//
//	Other Hardware based defines
//


//	Set interrupts on receive only
#define	VSCP_PIE_INT_MSK	0b00000011

#define	TXCON_ERRORS	0b01110000
#define TXCON_EMPTY		0b00001000

StatusType VSCP_FindSpeed(uint08_t * speed);
StatusType VSCP_SetSpeed(uint08_t speed);
StatusType VSCP_RCV_Register( void);
StatusType VSCP_enqMsgTx(VSCP_msg_tRef toEnqueue, uint08_t RxEnqueue);
StatusType VSCP_deqMsgTx(VSCP_msg_tRef toDequeue);
StatusType VSCP_enqMsgRx(VSCP_msg_tRef toEnqueue);
StatusType VSCP_deqMsgRx(VSCP_msg_tRef toDequeue);
StatusType CopyHard2FrameBuffer(void);
StatusType CopyFrameBuffer2Hard(pTXBUF_t pTxbuf);
StatusType WriteCANBuffer(void);
StatusType ReadCANBuffer(void);
uint08_t 	 VSCP_CountMsgRx( void);
void       VSCP_config(void);
void 			 VSCP_INT(void);
void 			 SetRxEvent(void);


#endif /* _VSCP_DRV_H_ */

/* End of File : VSCP_drv.h */
