//
//  $Id: pwm.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/pwm/pwm.h $
//

#ifndef _PWM_H_
#define _PWM_H_

int pwmDutyCycle (int percentage);
int pwmFrequency (int frequency);
void pwmInit (void);

#endif
