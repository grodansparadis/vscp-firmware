/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2012-2013 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
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
* TimerRC.c - R8CM12A TimerRC Function Interface
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  2-15-12	First derived from TimerRB2.h.
*******************************************************************************                
*/

#ifndef	_TIMERRC_H_
#define	_TIMERRC_H_

#define	ENABLE_TIMERRC_INTERRUPT()	ilvl34 = 1
#define	DISABLE_TIMERRC_INTERRUPT()	ilvl34 = 0

/* TIMER RC Interrupt Processes */
void TimerRC_Process(void);

/* initialize Timer RC */
void TimerRC_Init(void);

#endif /* _TIMERRC_H_ */

