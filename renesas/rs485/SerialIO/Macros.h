/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* Macros.h - ACS SerialIO Software Macro definitions
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*
*  date		comments                                                               
* --------------------------------------------                                                 
*  2-14-12	First written.
*  6-07-13	Changes for SerialIO.
*******************************************************************************                
*/                                                                                             
#ifndef	_MACROS_H_
#define _MACROS_H_

#include "IO.h"		/* I/O pin definitions */

/* MACROS */

/* Interrupt control */

static unsigned short near flags_reg; 

/* this is used to turn on the maskable interrupts */
#define 	ENABLE_INTERRUPTS() 	_asm("FSET I")

/* this is used to turn off the maskable interrupts */
#define 	DISABLE_INTERRUPTS() 	_asm("STC FLG,_flags_reg"); _asm("FCLR I")

/* this is used to restore I flag to previous setting */
#define 	RESTORE_INTERRUPTS() 	_asm("LDC _flags_reg,FLG")

/* expansion bus manipulation */
#define		XPD_OUTPUTS()			pd1 = 0xff
#define		XPD_INPUTS()			pd1 = 0
#define		XPD_DATA				p1

#define		ALE_HI()				XALE = 1; _asm("NOP")
#define		ALE_LO()				XALE = 0; _asm("NOP")

#define		EXPRD0()				p3 = 0; _asm("NOP")
#define		EXPWR0()				p3 = SEL_A; _asm("NOP")
#define		EXPRD1()				p3 = SEL_B; _asm("NOP")
#define		EXPWR1()				p3 = (SEL_B | SEL_A); _asm("NOP")
#define		OPTOSW()				p3 = SEL_C; _asm("NOP")
#define		RLYLED()				p3 = (SEL_C | SEL_A); _asm("NOP")
#define		DIPSW()					p3 = (SEL_C | SEL_B); _asm("NOP")
#define		NONE()					p3 = (SEL_C | SEL_B | SEL_A); _asm("NOP")

/* OPTOSW bits */
#define		INP25					B0
#define		INP26					B1
#define		SW1						B6
#define		SW2						B7

/* RLYLED bits */
#define		RLY1					B0
#define		RLY2					B1
#define		GREEN_LED				B6
#define		RED_LED					B7

// Port pin initialization  ... all referenced pins are GPIO
#define	Macro_Init_OtherIO()		ALE_LO(); NONE(); pur1 = 0xff; pd3 = B7 | B5 | B4 | B3

#endif	/* _MACROS_H_ */
