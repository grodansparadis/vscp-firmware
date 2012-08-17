/* Enable and disable functions "ripped" from a sample by R O Software.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact. */

#include "VIClowlevel.h"
 
#define IRQ_MASK 0x00000080

static inline unsigned long asm_get_cpsr(void)
{
  unsigned long retval;
  asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
  return retval;
}

static inline void asm_set_cpsr(unsigned val)
{
  asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
}

unsigned long enableIRQ(void)
{
  unsigned long _cpsr;

  _cpsr = asm_get_cpsr();
  asm_set_cpsr(_cpsr & ~IRQ_MASK);
  return _cpsr;
}

unsigned long disableIRQ(void)
{
  unsigned long _cpsr;

  _cpsr = asm_get_cpsr();
  asm_set_cpsr(_cpsr | IRQ_MASK);
  return _cpsr;
}

unsigned long restoreIRQ(unsigned oldCPSR)
{
  unsigned long _cpsr;

  _cpsr = asm_get_cpsr();
  asm_set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
  return _cpsr;
}
/* end of R O code */
