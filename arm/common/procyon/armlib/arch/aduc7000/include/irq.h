/*  INTERRUPT CONTROLLER    */

#define FIQ_SOURCE_BIT 			0x00000001						
#define SWI_BIT 			0x00000002						
#define RTOS_TIMER_BIT 			0x00000004						
#define GP_TIMER_BIT 			0x00000008						
#define WAKEUP_TIMER_BIT 		0x00000010						
#define WATCHDOG_TIMER_BIT 		0x00000020						
#define FLASHCON_BIT 			0x00000040						
#define ADC_BIT 			0x00000080						
#define PLL_LOCK_BIT 			0x00000100						
#define SM_SLAVE_BIT 			0x00000200						
#define SM_MASTER0_BIT 			0x00000400						
#define SM_MASTER1_BIT 			0x00000800						
#define SPI_SLAVE_BIT 			0x00001000						
#define SPI_MASTER_BIT 			0x00002000						
#define UART_BIT 			0x00004000						
#define XIRQ0_BIT 			0x00008000						
#define MONITOR_BIT 			0x00010000						
#define PSM_BIT 			0x00020000						
#define XIRQ1_BIT 			0x00040000						
#define PLA_IRQ0_BIT 			0x00080000						
#define PLA_IRQ1_BIT 			0x00100000						
#define SPM4_IO_BIT 			0x00200000						
#define SPM5_IO_BIT 			0x00400000						
#define PWM_TRIP_BIT 			0x00800000						
#define PWM_SYNC_BIT 			0x01000000						
                                        
#define IRQBASE 	(*(volatile unsigned long *) 0XFFFF0000)
#define IRQSTA 		(*(volatile unsigned long *) 0XFFFF0000)
#define IRQSIG 		(*(volatile unsigned long *) 0XFFFF0004)
#define IRQEN 		(*(volatile unsigned long *) 0XFFFF0008)
#define IRQCLR 		(*(volatile unsigned long *) 0XFFFF000C)
#define SWICFG 		(*(volatile unsigned long *) 0XFFFF0010)
#define FIQSTA 		(*(volatile unsigned long *) 0XFFFF0100)
#define FIQSIG 		(*(volatile unsigned long *) 0XFFFF0104)
#define FIQEN 		(*(volatile unsigned long *) 0XFFFF0108)
#define FIQCLR 		(*(volatile unsigned long *) 0XFFFF010C)

