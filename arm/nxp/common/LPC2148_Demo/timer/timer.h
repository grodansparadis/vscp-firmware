//
//  $Id: timer.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/timer/timer.h $
//

#ifndef _TIMER_H_
#define _TIMER_H_

//
//
//
void timerInit (void);
void timerBeepOn (unsigned int hz);
void timerBeepOff (void);
void timerBeepMHALL (void);
void timerBeepSMOTW (void);

#endif
