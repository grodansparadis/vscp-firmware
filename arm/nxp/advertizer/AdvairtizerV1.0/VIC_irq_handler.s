/*****************************************************************************
  IRQ handler for a VIC

  Copyright (c) 2005 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software. 
 
  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************************/

  .code 32
  .global irq_handler

irq_handler:
  // check if I bit is set in SPSR to detect spurious interrupts
  stmfd sp!, {r0}
  mrs r0, spsr
  tst r0, #0x80
  ldmfd sp!, {r0}
  subnes pc, lr, #4
#ifdef CTL_TASKING
  // store all registers in the tasking case
  stmfd sp!, {r0-r12, lr}
  mrs r0, spsr
  stmfd sp!, {r0}
  //ctl_interrupt_count++;
  ldr r2, =ctl_interrupt_count
  ldrb r3, [r2]
  add r3, r3, #1
  strb r3, [r2]
#else 
  // store the APCS registers in the non-tasking case
  stmfd sp!, {r0-r4, r12, lr}
#endif
  //((CTL_ISR_FN_t)VICVectAddr)();
  ldr r4, =0xFFFFF030
  ldr r3, [r4]
  mov lr, pc
  bx r3
  // VICVectAddr = 0;
  mov r3, #0
  str r3, [r4]
#ifdef CTL_TASKING
  // return from interrupt via ctl_exit_isr
  mov r0, sp;
  ldr r1, =ctl_exit_isr
  bx r1
#else
  // return from interrupt
  ldmfd sp!, {r0-r4, r12, lr}
  subs pc, lr, #4
#endif
