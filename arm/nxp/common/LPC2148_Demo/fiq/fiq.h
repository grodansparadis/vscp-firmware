//
//  $Id: fiq.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/fiq/fiq.h $
//

#ifndef _FIQ_H_
#define _FIQ_H_

void fiqInit (void);
int fiqEnable (void);
int fiqDisable (void);
unsigned int fiqGetCount (void);
void fiqClearCount (void);
void fiqISR (void);
unsigned char *fiqFIQISRCopyToRAM (void);

#endif
