/* irq.h
 * Header file for the Linux CAN-bus driver.
 * Written by Arnaud Westenberg email:arnaud@wanadoo.nl
 * This software is released under the GPL-License.
 * Version 0.7  6 Aug 2001
 */

void i82527_irq_handler(int irq, void *dev_id, struct pt_regs *regs);
void sja1000_irq_handler(int irq, void *dev_id, struct pt_regs *regs);
