//
//  $Id: cpu.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/cpu/cpu.h $
//

#ifndef _CPU_H_
#define _CPU_H_

void cpuSetupHardware (void);
void cpuSetupFIQISR (void *FIQInterrupt);
void cpuPLLDisable (void);
void cpuT1Disable (void);
void cpuGPIOInitialize (void);
void cpuToggleLED (unsigned portBASE_TYPE uxLED);

#endif
