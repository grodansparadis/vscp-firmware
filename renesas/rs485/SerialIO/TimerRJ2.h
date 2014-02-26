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
* TimerRJ2.c - R8CM12A TimerRJ2 Function Interface
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  11-28-12	First derived from Dennys\12Segment\Timers.c.
*******************************************************************************                
*/                                                                                             

#ifndef	_TIMERRJ2_H_
#define	_TIMERRJ2_H_

#define	ENABLE_TIMERRJ2_INTERRUPT()		trjie_trjir = 1

#define	DISABLE_TIMERRJ2_INTERRUPT()	trjie_trjir = 0

/* TIMER RB2 Interrupt Processes */
void TimerRJ2_Process(void);

/* initialize Timer RJ2 */
void TimerRJ2_Init(void);

#endif /* _TIMERRJ2_H_ */
