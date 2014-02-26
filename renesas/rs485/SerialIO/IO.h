/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2007-2013 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
*                                                                                              
*       This Information is Proprietary to Ackerman Computer Sciences, Sarasota,               
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another               
* program without the express written consent of ACS. This Information or any                  
* portion thereof remains the property of ACS. The Information contained herein                
* is believed to be accurate and ACS assumes no responsibility or liability for                
* its use in any way and conveys no license or title under any patent or                       
* copyright and makes no representation or warranty that this Information is                   
* free from patent or copyright infringement.                                                  
*------------------------------------------------------------------------------                
* IO.h - ACS Serial IO pin definitions
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                             
* --------------------------------------------                                                 
*  9-27-07	First written.    
*  3-31-11	Changes for use with R8C/M12A instead of z8F.   
*  4-06-11	Changes for 12SegmentFC Rev1. 
*  2-10-12	Changes for Ps2Pwr57 Rev0. 
*  6-07-13	Changes for SerialIO Rev0.                                       
*******************************************************************************                
*/ 
#ifndef	_IO_H_
#define	_IO_H_

/* R8C/M12A Ports and function assignments */

/* Port 1 */
#define	XPD0				p1_0	/* Expansion Data Bus 0 */
#define	XPD1				p1_1	/* Expansion Data Bus 1 */
#define	XPD2				p1_2	/* Expansion Data Bus 2 */
#define	XPD3				p1_3	/* Expansion Data Bus 3 */
#define	XPD4				p1_4	/* Expansion Data Bus 4 */
#define	XPD5				p1_5	/* Expansion Data Bus 5 */
#define	XPD6				p1_6	/* Expansion Data Bus 6 */
#define	XPD7				p1_7	/* Expansion Data Bus 7 */

/* Port 3 */
#define	EXP_SEL_A			p3_3	/* Expansion Bus HC138 Decoder A  */
#define	EXP_SEL_B			p3_4	/* Expansion Bus HC138 Decoder A  */
#define	EXP_SEL_C			p3_5	/* Expansion Bus HC138 Decoder A  */
#define	XALE				p3_7	/* Expansion Bus Address Latch Enable */

#define	SEL_A				B3
#define	SEL_B				B4
#define	SEL_C				B5

/* Port 4 */
#define	TXD0				p4_2	/* TXD0 (KI3-, TRB0, TXD0)*/
#define	CTS					p4_5	/* CTS (INT0-, ADTRG-) */
#define	RXD0				p4_6	/* RXD0 (INT1-, TRJIO, RXD0/TXD0, VCOUT1) */
#define	RTS					p4_7	/* RTS (XOUT, INT2-) */

/* Port A */
#define	RESET_PIN			pa_0	/* RESET- */

#endif	/* _IO_H_ */
