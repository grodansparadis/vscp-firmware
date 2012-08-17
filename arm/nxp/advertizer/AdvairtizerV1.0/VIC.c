// CrossWorks Tasking Library.
//
// Copyright (c) 2005 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include "ctl_api.h"

#define VICIRQStatus (*(volatile unsigned long *)0xFFFFF000)
#define VICFIQStatus (*(volatile unsigned long *)0xFFFFF004)
#define VICRawIntr (*(volatile unsigned long *)0xFFFFF008)
#define VICIntSelect (*(volatile unsigned long *)0xFFFFF00C)
#define VICIntEnable (*(volatile unsigned long *)0xFFFFF010)
#define VICIntEnClr (*(volatile unsigned long *)0xFFFFF014)
#define VICSoftInt (*(volatile unsigned long *)0xFFFFF018)
#define VICSoftIntClear (*(volatile unsigned long *)0xFFFFF01C)
#define VICProtection (*(volatile unsigned long *)0xFFFFF020)
#define VICVectAddr (*(volatile unsigned long *)0xFFFFF030)
#define VICDefVectAddr (*(volatile unsigned long *)0xFFFFF034)
#define VICVectAddr0 (*(volatile unsigned long *)0xFFFFF100)
#define VICVectAddr1 (*(volatile unsigned long *)0xFFFFF104)
#define VICVectAddr2 (*(volatile unsigned long *)0xFFFFF108)
#define VICVectAddr3 (*(volatile unsigned long *)0xFFFFF10C)
#define VICVectAddr4 (*(volatile unsigned long *)0xFFFFF110)
#define VICVectAddr5 (*(volatile unsigned long *)0xFFFFF114)
#define VICVectAddr6 (*(volatile unsigned long *)0xFFFFF118)
#define VICVectAddr7 (*(volatile unsigned long *)0xFFFFF11C)
#define VICVectAddr8 (*(volatile unsigned long *)0xFFFFF120)
#define VICVectAddr9 (*(volatile unsigned long *)0xFFFFF124)
#define VICVectAddr10 (*(volatile unsigned long *)0xFFFFF128)
#define VICVectAddr11 (*(volatile unsigned long *)0xFFFFF12C)
#define VICVectAddr12 (*(volatile unsigned long *)0xFFFFF130)
#define VICVectAddr13 (*(volatile unsigned long *)0xFFFFF134)
#define VICVectAddr14 (*(volatile unsigned long *)0xFFFFF138)
#define VICVectAddr15 (*(volatile unsigned long *)0xFFFFF13C)
#define VICVectCntl0 (*(volatile unsigned long *)0xFFFFF200)
#define VICVectCntl1 (*(volatile unsigned long *)0xFFFFF204)
#define VICVectCntl2 (*(volatile unsigned long *)0xFFFFF208)
#define VICVectCntl3 (*(volatile unsigned long *)0xFFFFF20C)
#define VICVectCntl4 (*(volatile unsigned long *)0xFFFFF210)
#define VICVectCntl5 (*(volatile unsigned long *)0xFFFFF214)
#define VICVectCntl6 (*(volatile unsigned long *)0xFFFFF218)
#define VICVectCntl7 (*(volatile unsigned long *)0xFFFFF21C)
#define VICVectCntl8 (*(volatile unsigned long *)0xFFFFF220)
#define VICVectCntl9 (*(volatile unsigned long *)0xFFFFF224)
#define VICVectCntl10 (*(volatile unsigned long *)0xFFFFF228)
#define VICVectCntl11 (*(volatile unsigned long *)0xFFFFF22C)
#define VICVectCntl12 (*(volatile unsigned long *)0xFFFFF230)
#define VICVectCntl13 (*(volatile unsigned long *)0xFFFFF234)
#define VICVectCntl14 (*(volatile unsigned long *)0xFFFFF238)
#define VICVectCntl15 (*(volatile unsigned long *)0xFFFFF23C)
#define VICVectAddrn(n) (*(&VICVectAddr0 + n))
#define VICVectCntl(n) (*(&VICVectCntl0 + n))

static CTL_ISR_FN_t softwareVectorAddresses[16];
static unsigned softwareVectorControl[16];
static void defaultISR(void);

int
ctl_set_isr(unsigned int number, unsigned int priority, CTL_ISR_TRIGGER_t trigger, CTL_ISR_FN_t isr, CTL_ISR_FN_t *oldisr)
{
  if (priority < 16)
    {
      // hardware vectoring
      if (oldisr)
        {
          int i;
          *oldisr = 0;
          for (i = 0; i < 16; ++i)
            if ((VICVectCntl(i) & 0x1F) == number)
              *oldisr = (CTL_ISR_FN_t)VICVectAddrn(i);
        }
      if (isr)
        {
          /* Installing an ISR */
    #ifdef CHECK_INTERRUPT_INSTALLED
          if (VICVectCntl(priority))
            return 0; /* ISR already installed at this interrupt priority */
    #endif
          VICIntSelect &= ~(1 << number); /* IRQ Interrupt */
          VICVectCntl(priority) = (1 << 5) | number;
          VICVectAddrn(priority) = (unsigned int)isr;
        }
      else
        {
          /* Removing an ISR */
          VICVectCntl(priority) = 0;
          VICVectAddrn(priority) = 0;
        }
    }
  else // priority >= 16
    {
      // software vectoring
      if (oldisr)
        {
          int i;
          *oldisr = 0;
          for (i = 0; i < 16; ++i)
            if ((softwareVectorControl[i] & 0x1F) == number)
              *oldisr = (CTL_ISR_FN_t)softwareVectorAddresses[i];
        }
      VICDefVectAddr = (unsigned int)defaultISR;
      VICIntSelect &= ~(1 << number); /* IRQ Interrupt */
      softwareVectorControl[priority-16] = (1<<5) | number;
      softwareVectorAddresses[priority-16] = isr;
    }
  return 1;
}

void 
defaultISR(void)
{
  int i, IRQStatus = VICIRQStatus; // record the status register prior to turning on interrupts
  ctl_global_interrupts_re_enable_from_isr(); 
  for (i=0;i<16;i++)
    {
      unsigned cntrl = softwareVectorControl[i];
      if ((cntrl & (1<<5)) && (IRQStatus & (1<<(cntrl & 0x1f))))
        {
          ((CTL_ISR_FN_t)softwareVectorAddresses[i])();
          break;
        }
    }
  ctl_global_interrupts_un_re_enable_from_isr();
}

int
ctl_unmask_isr(unsigned int number)
{
  VICIntEnable = 1 << number;
  return 1;
}

int
ctl_mask_isr(unsigned int number)
{
  VICIntEnClr = 1 << number;
  return 1;
}
