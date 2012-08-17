/*****************************************************************************
 *   LPC214X.h:  Header file for Philips LPC214x Family Microprocessors
 *   The header file is the super set of all hardware definition of the 
 *   peripherals for the LPC214x family microprocessor.  This information
 *   is provided "as is" and will not be supported or maintained by Philips.
 *   We do not guarantee that this information will meet your design requirements.
 *   For complete terms and conditions, please visit 
 *   www.semiconductors.philips.com/acrobat_download/other/usb/generic_shrinkwrap_license_driver.pdf
 *
 *   Copyright(C) 2005, Philips Semiconductors
 *   All rights reserved.

 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *   2005.10.13  ver 1.01    Removed CSPR and DC_REVISION register.
 *                           CSPR can not be accessed at the user level,
 *                           DC_REVISION is no long available.
 *                           All registers use "volatile unsigned long". 
******************************************************************************/

#ifndef __LPC214x_H
#define __LPC214x_H

#define REG_8           volatile unsigned char
#define REG16           volatile unsigned short
#define REG32           volatile unsigned long

/*
#define REG8  (volatile unsigned char*)
#define REG16 (volatile unsigned short*)
#define REG32 (volatile unsigned int*)
*/

#include "lpcWD.h"
#include "lpcTMR.h"
#include "lpcUART.h"
#include "lpcI2C.h"
#include "lpcSPI.h"
#include "lpcRTC.h"
#include "lpcGPIO.h"
#include "lpcPIN.h"
#include "lpcADC.h"
#include "lpcSCB.h"
#include "lpcVIC.h"

///////////////////////////////////////////////////////////////////////////////
// Vectored Interrupt Controller
#define VIC ((vicRegs_t *)0xFFFFF000)

// Vectored Interrupt Controller Registers
#define VICIRQStatus    VIC->irqStatus  /* IRQ Status Register */
#define VICFIQStatus    VIC->fiqStatus  /* FIQ Status Register */
#define VICRawIntr      VIC->rawIntr    /* Raw Interrupt Status Register */
#define VICIntSelect    VIC->intSelect  /* Interrupt Select Register */
#define VICIntEnable    VIC->intEnable  /* Interrupt Enable Register */
#define VICIntEnClear   VIC->intEnClear /* Interrupt Enable Clear Register */
#define VICSoftInt      VIC->softInt    /* Software Interrupt Register */
#define VICSoftIntClear VIC->softIntClear /* Software Interrupt Clear Register */
#define VICProtection   VIC->protection /* Protection Enable Register */
#define VICVectAddr     VIC->vectAddr   /* Vector Address Register */
#define VICDefVectAddr  VIC->defVectAddr /* Default Vector Address Register */
#define VICVectAddr0    VIC->vectAddr0  /* Vector Address 0 Register */
#define VICVectAddr1    VIC->vectAddr1  /* Vector Address 1 Register */
#define VICVectAddr2    VIC->vectAddr2  /* Vector Address 2 Register */
#define VICVectAddr3    VIC->vectAddr3  /* Vector Address 3 Register */
#define VICVectAddr4    VIC->vectAddr4  /* Vector Address 4 Register */
#define VICVectAddr5    VIC->vectAddr5  /* Vector Address 5 Register */
#define VICVectAddr6    VIC->vectAddr6  /* Vector Address 6 Register */
#define VICVectAddr7    VIC->vectAddr7  /* Vector Address 7 Register */
#define VICVectAddr8    VIC->vectAddr8  /* Vector Address 8 Register */
#define VICVectAddr9    VIC->vectAddr9  /* Vector Address 9 Register */
#define VICVectAddr10   VIC->vectAddr10 /* Vector Address 10 Register */
#define VICVectAddr11   VIC->vectAddr11 /* Vector Address 11 Register */
#define VICVectAddr12   VIC->vectAddr12 /* Vector Address 12 Register */
#define VICVectAddr13   VIC->vectAddr13 /* Vector Address 13 Register */
#define VICVectAddr14   VIC->vectAddr14 /* Vector Address 14 Register */
#define VICVectAddr15   VIC->vectAddr15 /* Vector Address 15 Register */
#define VICVectCntl0    VIC->vectCntl0  /* Vector Control 0 Register */
#define VICVectCntl1    VIC->vectCntl1  /* Vector Control 1 Register */
#define VICVectCntl2    VIC->vectCntl2  /* Vector Control 2 Register */
#define VICVectCntl3    VIC->vectCntl3  /* Vector Control 3 Register */
#define VICVectCntl4    VIC->vectCntl4  /* Vector Control 4 Register */
#define VICVectCntl5    VIC->vectCntl5  /* Vector Control 5 Register */
#define VICVectCntl6    VIC->vectCntl6  /* Vector Control 6 Register */
#define VICVectCntl7    VIC->vectCntl7  /* Vector Control 7 Register */
#define VICVectCntl8    VIC->vectCntl8  /* Vector Control 8 Register */
#define VICVectCntl9    VIC->vectCntl9  /* Vector Control 9 Register */
#define VICVectCntl10   VIC->vectCntl10 /* Vector Control 10 Register */
#define VICVectCntl11   VIC->vectCntl11 /* Vector Control 11 Register */
#define VICVectCntl12   VIC->vectCntl12 /* Vector Control 12 Register */
#define VICVectCntl13   VIC->vectCntl13 /* Vector Control 13 Register */
#define VICVectCntl14   VIC->vectCntl14 /* Vector Control 14 Register */
#define VICVectCntl15   VIC->vectCntl15 /* Vector Control 15 Register */

///////////////////////////////////////////////////////////////////////////////
// Software Interrupt register
#define SIR_WDT 0
#define SIR_ARMCore0 2
#define SIR_ARMCore1 3
#define SIR_TIMER0 4
#define SIR_TIMER1 5
#define SIR_UART0 6
#define SIR_UART1 7
#define SIR_PWM0 8
#define SIR_I2C0 9
#define SIR_SPI0 10
#define SIR_SPI1_SSP 11
#define SIR_PLL 12
#define SIR_RTC 13
#define SIR_EINT0 14
#define SIR_EINT1 15
#define SIR_EINT2 16
#define SIR_EINT3 17
#define SIR_AD0 18
#define SIR_I2C1 19
#define SIR_BOD 20
#define SIR_AD1 21
#define SIR_USB 22

///////////////////////////////////////////////////////////////////////////////
// Pin Connect Block
#define PINSEL          ((pinRegs_t *)0xE002C000)

// Pin Connect Block Registers
#define PINSEL0         PINSEL->sel0    /* Pin Function Select Register 0 */
#define PINSEL1         PINSEL->sel1    /* Pin Function Select Register 1 */
#define PINSEL2         PINSEL->sel2    /* Pin Function Select Register 2 */

///////////////////////////////////////////////////////////////////////////////
// General Purpose Input/Output
#define GPIO            ((gpioRegs_t *)0xE0028000)

// GPIO Registers
#define IOPIN0           GPIO->in0       /* Pin Value Register */
#define IOSET0           GPIO->set0      /* Pin Output Set Register */
#define IODIR0           GPIO->dir0      /* Pin Direction Register */
#define IOCLR0           GPIO->clr0      /* Pin Output Clear Register */

#define IOPIN1           GPIO->in1       /* Pin Value Register */
#define IOSET1           GPIO->set1      /* Pin Output Set Register */
#define IODIR1           GPIO->dir1      /* Pin Direction Register */
#define IOCLR1           GPIO->clr1      /* Pin Output Clear Register */

///////////////////////////////////////////////////////////////////////////////
// Fast enhanced Input/Output
#define F_IO_BASE_ADDR		0x3FFFC000
#define F_IO0DIR        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x00)) 
#define F_IO0MASK       (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x10))
#define F_IO0PIN        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x14))
#define F_IO0SET        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x18))
#define F_IO0CLR        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x1C))
#define F_IO1DIR        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x20)) 
#define F_IO1MASK       (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x30))
#define F_IO1PIN        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x34))
#define F_IO1SET        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x38))
#define F_IO1CLR        (*(volatile unsigned long *)(F_IO_BASE_ADDR + 0x3C))

///////////////////////////////////////////////////////////////////////////////
// System Contol Block
#define SCB             ((scbRegs_t *)0xE01FC000)

// Memory Accelerator Module Registers (MAM)
#define MAMCR           SCB->mam.cr     /* Control Register */
#define MAMTIM          SCB->mam.tim    /* Timing Control Register */

// Memory Mapping Control Register
#define MEMMAP          SCB->memmap

// Phase Locked Loop Registers (PLL)
#define PLLCON          SCB->pll.con    /* Control Register */
#define PLLCFG          SCB->pll.cfg    /* Configuration Register */
#define PLLSTAT         SCB->pll.stat   /* Status Register */
#define PLLFEED         SCB->pll.feed   /* Feed Register */

// Power Control Registers
#define PCON            SCB->p.con      /* Control Register */
#define PCONP           SCB->p.conp     /* Peripherals Register */

// VPB Divider Register
#define VPBDIV          SCB->vpbdiv

// External Interrupt Registers
#define EXTINT          SCB->ext.flag   /* Flag Register */
#define INTWAKE         SCB->ext.wake   /* Wakeup Register */
/* System Control Block(SCB) modules include Memory Accelerator Module,
Phase Locked Loop, VPB divider, Power Control, External Interrupt, 
Reset, and Code Security/Debugging */
#define EXTWAKE0    BIT0 //External Interrupt Wakeup register EINT0
#define EXTWAKE1    BIT1 //External Interrupt Wakeup register EINT1
#define EXTWAKE2    BIT2 //External Interrupt Wakeup register EINT2
#define EXTWAKE3    BIT3 //External Interrupt Wakeup register EINT3
#define USBWAKE     BIT5 //External Interrupt Wakeup register USB
#define BODWAKE     BIT14 //External Interrupt Wakeup register BOD
#define RTCWAKE     BIT15 //External Interrupt Wakeup register RTC


#define SCB_BASE_ADDR	0xE01FC000
#define EXTMODE          SCB->ext.mode   /* Mode Register */
#define EXTPOLAR         SCB->ext.polar   /* Polar Register */
//#define EXTMODE        (*(volatile unsigned long *)(SCB_BASE_ADDR + 0x148))
//#define EXTPOLAR       (*(volatile unsigned long *)(SCB_BASE_ADDR + 0x14C))

#define EXTMODE0  BIT0 //External Interrupt Mode register EINT0
#define EXTMODE1  BIT1 //External Interrupt Mode register EINT1
#define EXTMODE2  BIT2 //External Interrupt Mode register EINT2
#define EXTMODE3  BIT3 //External Interrupt Mode register EINT3

#define EXTPOLAR0 BIT0 //External Interrupt Polarity register EXTPOLAR0
#define EXTPOLAR1 BIT1 //External Interrupt Polarity register EXTPOLAR1
#define EXTPOLAR2 BIT2 //External Interrupt Polarity register EXTPOLAR2
#define EXTPOLAR3 BIT3 //External Interrupt Polarity register EXTPOLAR3

/* Reset */
#define RSIR           (*(volatile unsigned long *)(SCB_BASE_ADDR + 0x180))
#define POR      BIT0 //Power-On Reset (POR) event sets this bit
#define EXTR     BIT1 //Assertion of the RESET signal sets this bit
#define WDTR     BIT2 //This bit is set when the Watchdog Timer times out and the WDTRESET
                      //bit in the Watchdog Mode Register is 1.
#define BODR     BIT3 //This bit is set when the 3.3 V power reaches a level below 2.6 V.

/* System Controls and Status */
#define SCS            (*(volatile unsigned long *)(SCB_BASE_ADDR + 0x1A0))	

///////////////////////////////////////////////////////////////////////////////
// Timer 0
#define TMR0            ((pwmTmrRegs_t *)0xE0004000)
#define TMR0_BASE_ADDR		0xE0004000
// Timer 0 Registers
#define T0IR            TMR0->ir        /* Interrupt Register */
#define T0TCR           TMR0->tcr       /* Timer Control Register */
#define T0TC            TMR0->tc        /* Timer Counter */
#define T0PR            TMR0->pr        /* Prescale Register */
#define T0PC            TMR0->pc        /* Prescale Counter Register */
#define T0MCR           TMR0->mcr       /* Match Control Register */
#define T0MR0           TMR0->mr0       /* Match Register 0 */
#define T0MR1           TMR0->mr1       /* Match Register 1 */
#define T0MR2           TMR0->mr2       /* Match Register 2 */
#define T0MR3           TMR0->mr3       /* Match Register 3 */
#define T0CCR           TMR0->ccr       /* Capture Control Register */
#define T0CR0           TMR0->cr0       /* Capture Register 0 */
#define T0CR1           TMR0->cr1       /* Capture Register 1 */
#define T0CR2           TMR0->cr2       /* Capture Register 2 */
#define T0CR3           TMR0->cr3       /* Capture Register 3 */
#define T0EMR           TMR0->emr       /* External Match Register */
#define T0_CTCR         (*(volatile unsigned long *)(TMR0_BASE_ADDR + 0x70))

///////////////////////////////////////////////////////////////////////////////
// Timer 1
#define TMR1            ((pwmTmrRegs_t *)0xE0008000)
// ---- Timer 1 -------------------------------------
#define TMR1_BASE_ADDR		0xE0008000
// Timer 1 Registers
#define T1IR            TMR1->ir        /* Interrupt Register */
#define T1TCR           TMR1->tcr       /* Timer Control Register */
#define T1TC            TMR1->tc        /* Timer Counter */
#define T1PR            TMR1->pr        /* Prescale Register */
#define T1PC            TMR1->pc        /* Prescale Counter Register */
#define T1MCR           TMR1->mcr       /* Match Control Register */
#define T1MR0           TMR1->mr0       /* Match Register 0 */
#define T1MR1           TMR1->mr1       /* Match Register 1 */
#define T1MR2           TMR1->mr2       /* Match Register 2 */
#define T1MR3           TMR1->mr3       /* Match Register 3 */
#define T1CCR           TMR1->ccr       /* Capture Control Register */
#define T1CR0           TMR1->cr0       /* Capture Register 0 */
#define T1CR1           TMR1->cr1       /* Capture Register 1 */
#define T1CR2           TMR1->cr2       /* Capture Register 2 */
#define T1CR3           TMR1->cr3       /* Capture Register 3 */
#define T1EMR           TMR1->emr       /* External Match Register */
#define T1_CTCR         (*(volatile unsigned long *)(TMR1_BASE_ADDR + 0x70))

///////////////////////////////////////////////////////////////////////////////
// Pulse Width Modulator (PWM)
#define PWM             ((pwmTmrRegs_t *)0xE0014000)

// PWM Registers
#define PWMIR           PWM->ir         /* Interrupt Register */
#define PWMTCR          PWM->tcr        /* Timer Control Register */
#define PWMTC           PWM->tc         /* Timer Counter */
#define PWMPR           PWM->pr         /* Prescale Register */
#define PWMPC           PWM->pc         /* Prescale Counter Register */
#define PWMMCR          PWM->mcr        /* Match Control Register */
#define PWMMR0          PWM->mr0        /* Match Register 0 */
#define PWMMR1          PWM->mr1        /* Match Register 1 */
#define PWMMR2          PWM->mr2        /* Match Register 2 */
#define PWMMR3          PWM->mr3        /* Match Register 3 */
#define PWMMR4          PWM->mr4        /* Match Register 4 */
#define PWMMR5          PWM->mr5        /* Match Register 5 */
#define PWMMR6          PWM->mr6        /* Match Register 6 */
#define PWMPCR          PWM->pcr        /* Control Register */
#define PWMLER          PWM->ler        /* Latch Enable Register */

#define PWMSEL2         2  // Selects double edge controlled mode or Single edge mode for the PWM output.
#define PWMSEL3         3  // Selects double edge controlled mode or Single edge mode for the PWM output.
#define PWMSEL4         4  // Selects double edge controlled mode or Single edge mode for the PWM output.
#define PWMSEL5         5  // Selects double edge controlled mode or Single edge mode for the PWM output.
#define PWMSEL6         6  // Selects double edge controlled mode or Single edge mode for the PWM output.
#define PWMENA1         9  // The PWM output enable bit
#define PWMENA2         10 // The PWM output enable bit
#define PWMENA3         11 // The PWM output enable bit
#define PWMENA4         12 // The PWM output enable bit
#define PWMENA5         13 // The PWM output enable bit
#define PWMENA6         14 // The PWM output enable bit

/*
|----------------------------------------------------
| UART0 & UART1
|----------------------------------------------------
*/
/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define U0_PINSEL       (0x00000005)    /* PINSEL0 Value for UART0 */
#define U0_PINMASK      (0x0000000F)    /* PINSEL0 Mask for UART0 */
// UART0 Registers
#define UART0_BASE_ADDR		0xE000C000
#define UART0_RBR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x00))
#define UART0_THR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x00))
#define UART0_DLL          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x00))
#define UART0_DLM          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x04))
#define UART0_IER          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x04))
#define UART0_IIR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x08))
#define UART0_FCR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x08))
#define UART0_LCR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x0C))
#define UART0_MCR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x10))
#define UART0_LSR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x14))
#define UART0_MSR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x18))
#define UART0_SCR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x1C))
#define UART0_ACR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x20))
#define UART0_FDR          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x28))
#define UART0_TER          (*(volatile unsigned long *)(UART0_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define U1_PINSEL       (0x00050000)    /* PINSEL0 Value for UART1 */
#define U1_PINMASK      (0x000F0000)    /* PINSEL0 Mask for UART1 */

// UART1 Registers
#define UART1_BASE_ADDR		0xE0010000
#define UART1_RBR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x00))
#define UART1_THR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x00))
#define UART1_DLL          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x00))
#define UART1_DLM          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x04))
#define UART1_IER          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x04))
#define UART1_IIR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x08))
#define UART1_FCR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x08))
#define UART1_LCR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x0C))
#define UART1_MCR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x10))
#define UART1_LSR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x14))
#define UART1_MSR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x18))
#define UART1_SCR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x1C))
#define UART1_ACR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x20))
#define UART1_FDR          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x28))
#define UART1_TER          (*(volatile unsigned long *)(UART1_BASE_ADDR + 0x30))
///////////////////////////////////////////////////////////////////////////////
// I2C Interface
#define I2C0             ((i2cRegs_t *)0xE001C000)

/* I2C Interface 0 */
#define I2C0_CONSET        I2C0->conset     /* Control Set Register */
#define I2C0_STAT          I2C0->stat       /* Status Register */
#define I2C0_DAT           I2C0->dat        /* Data Register */
#define I2C0_ADR           I2C0->adr        /* Slave Address Register */
#define I2C0_SCLH          I2C0->sclh       /* SCL Duty Cycle Register (high half word) */
#define I2C0_SCLL          I2C0->scll       /* SCL Duty Cycle Register (low half word) */
#define I2C0_CONCLR        I2C0->conclr     /* Control Clear Register */

#define I2C1             ((i2cRegs_t *)0xE005C000)

/* I2C Interface 1 */
#define I2C1_CONSET        I2C1->conset     /* Control Set Register */
#define I2C1_STAT          I2C1->stat       /* Status Register */
#define I2C1_DAT           I2C1->dat        /* Data Register */
#define I2C1_ADR           I2C1->adr        /* Slave Address Register */
#define I2C1_SCLH          I2C1->sclh       /* SCL Duty Cycle Register (high half word) */
#define I2C1_SCLL          I2C1->scll       /* SCL Duty Cycle Register (low half word) */
#define I2C1_CONCLR        I2C1->conclr     /* Control Clear Register */

///////////////////////////////////////////////////////////////////////////////
// Serial Peripheral Interface
#define SPI0             ((spiRegs_t *)0xE0020000)

// SPI Registers
#define SPI0_SPCR            SPI0->cr         /* Control Register */
#define SPI0_SPSR            SPI0->sr         /* Status Register */
#define SPI0_SPDR            SPI0->dr         /* Data Register */
#define SPI0_SPCCR           SPI0->ccr        /* Clock Counter Register */
#define SPI0_SPINT           SPI0->flag       /* Interrupt Flag Register */

// SPCR Control Register 0
#define SPI0_MSTR    BIT5 // SPI MODE Master/Slave
#define SPI0_CPOL    BIT4 // Clock Out Polarity
#define SPI0_CPHA    BIT3 // Clock Out Phase

// SPSR Status Register
#define SPI0_SPIF    BIT7 // BUSY

/*
|----------------------------------------------------
| SSP - Synchronous Serial Port
|----------------------------------------------------
*/

#define SSP_BASE_ADDR		0xE0068000
#define SSP_CR0         (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x00))
#define SSP_CR1         (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x04))
#define SSP_DR          (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x08))
#define SSP_SR          (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x0C))
#define SSP_CPSR        (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x10))
#define SSP_IMSC        (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x14))
#define SSP_RIS         (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x18))
#define SSP_MIS         (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x1C))
#define SSP_ICR         (*(volatile unsigned long * )(SSP_BASE_ADDR + 0x20))

#define SSP_FIFO_SIZE 8

// SSPCR0 Control Register 0
#define CPHA      BIT7 // Clock Out Phase
#define CPOL      BIT6 // Clock Out Polarity
#define FRF_SPI   0x00 // SPI MODE
#define FRF_SSI   0x10 // SSI MODE
#define FRF_MICRO 0x20 // Microwire MODE

#define DSS_04_BIT 0x03
#define DSS_08_BIT 0x07
#define DSS_09_BIT 0x08
#define DSS_10_BIT 0x09
#define DSS_11_BIT 0x0A
#define DSS_12_BIT 0x0B
#define DSS_13_BIT 0x0C
#define DSS_14_BIT 0x0D
#define DSS_15_BIT 0x0E
#define DSS_16_BIT 0x0F

// SSPCR1 Control Register 1
#define SOD BIT3 // Slave Output Disable
#define MS  BIT2 // Master/Slave, 0 = MASTER
#define SSE BIT1 // SSP Enable
#define LBM BIT0 // Loop Back Mode

// SSPSR Status Register
#define BSY BIT4 // BUSY
#define RFF BIT3 // Receive FIFO Full
#define RNE BIT2 // Receive FIFO Not Empty
#define TNF BIT1 // Transmit FIFO Not Full
#define TFE BIT0 // Transmit FIFO Empty

// SSPIMSC Interrupt Mask Set/Clear Register
#define TXIM  BIT3 // IRQ Tx FIFO enable
#define RXIM  BIT2 // IRQ Rx FIFO enable
#define RTIM  BIT1 // IRQ Receive Timeout enable
#define RORIM BIT0 // IRQ Receive Overrun enable

// SSPRIS Raw Interrupt Status Register
#define TXRIS  BIT3 // Tx FIFO is at least half empty
#define RXRIS  BIT2 // Rx FIFO is at least half full
#define RTRIS  BIT1 // Receive Timeout condition
#define RORRIS BIT0 // completely received while the RxFIFO was full

// SSPMIS Masked Interrupt Status Register
#define TXMIS  BIT3 // Tx FIFO is at least half empty, and this interrupt is enabled
#define RXMIS  BIT2 // Rx FIFO is at least half full, and this interrupt is enabled
#define RTMIS  BIT1 // Receive Timeout condition and this interrupt is enabled
#define RORMIS BIT0 // another frame was completely received while the RxFIFO was full, and this interrupt is enabled

// SSPICR Interrupt Clear Register
#define RTIC  BIT1 // clears the Receive Timeout interrupt
#define RORIC BIT0 // clears the ?frame was received when RxFIFO was full? interrupt

// SPI-IOSET-Maske
#define SSP_IOSET_MASK 0x000002A8 // SS,SCK,MISO und MOSI für SPI1 einschalten

///////////////////////////////////////////////////////////////////////////////
// Real Time Clock
#define RTC             ((rtcRegs_t *)0xE0024000)

// RTC Registers
#define RTCILR          RTC->ilr        /* Interrupt Location Register */
#define RTCCTC          RTC->ctc        /* Clock Tick Counter */
#define RTCCCR          RTC->ccr        /* Clock Control Register */
#define RTCCIIR         RTC->ciir       /* Counter Increment Interrupt Register */
#define RTCAMR          RTC->amr        /* Alarm Mask Register */
#define RTCCTIME0       RTC->ctime0     /* Consolidated Time Register 0 */
#define RTCCTIME1       RTC->ctime1     /* Consolidated Time Register 1 */
#define RTCCTIME2       RTC->ctime2     /* Consolidated Time Register 2 */
#define RTCSEC          RTC->sec        /* Seconds Register */
#define RTCMIN          RTC->min        /* Minutes Register */
#define RTCHOUR         RTC->hour       /* Hours Register */
#define RTCDOM          RTC->dom        /* Day Of Month Register */
#define RTCDOW          RTC->dow        /* Day Of Week Register */
#define RTCDOY          RTC->doy        /* Day Of Year Register */
#define RTCMONTH        RTC->month      /* Months Register */
#define RTCYEAR         RTC->year       /* Years Register */
#define RTCALSEC        RTC->alsec      /* Alarm Seconds Register */
#define RTCALMIN        RTC->almin      /* Alarm Minutes Register */
#define RTCALHOUR       RTC->alhour     /* Alarm Hours Register */
#define RTCALDOM        RTC->aldom      /* Alarm Day Of Month Register */
#define RTCALDOW        RTC->aldow      /* Alarm Day Of Week Register */
#define RTCALDOY        RTC->aldoy      /* Alarm Day Of Year Register */
#define RTCALMON        RTC->almon      /* Alarm Months Register */
#define RTCALYEAR       RTC->alyear     /* Alarm Years Register */
#define RTCPREINT       RTC->preint     /* Prescale Value Register (integer) */
#define RTCPREFRAC      RTC->prefrac    /* Prescale Value Register (fraction) */

/*
|----------------------------------------------------
| AD - Analog to Digital Converter
|----------------------------------------------------
*/
/* A/D Converter 0 (AD0) */
#define AD0             ((adcRegs_t *)0xE0034000)
//#define AD0_BASE_ADDR		0xE0034000
//#define AD0_CR          (*(volatile unsigned long *)(AD0_BASE_ADDR + 0x00))
//#define AD0_DR          (*(volatile unsigned long *)(AD0_BASE_ADDR + 0x04))
#define AD0_CR          AD0->cr          //
#define AD0_GDR         AD0->dr          //
//#define AD0_SEL         AD0->CR.sel     //

/* A/D Converter 1 (AD1) */
#define AD1             ((adcRegs_t *)0xE0060000)
//#define AD1_BASE_ADDR		0xE0060000
//#define AD1_CR          (*(volatile unsigned long *)(AD1_BASE_ADDR + 0x00))
//#define AD1_DR          (*(volatile unsigned long *)(AD1_BASE_ADDR + 0x04))
#define AD1_CR          AD1->cr          //
#define AD1_GDR         AD1->dr          //

/*
|----------------------------------------------------
| DA - Digital to Analog Converter
|----------------------------------------------------
*/
#define DAC_BASE_ADDR		0xE006C000
#define DA_DACR           (*(volatile unsigned long *)(DAC_BASE_ADDR + 0x00))

///////////////////////////////////////////////////////////////////////////////
// Watchdog
#define WD              ((wdRegs_t *)0xE0000000)

// Watchdog Registers
#define WDMOD           WD->mod         /* Watchdog Mode Register */
#define WDTC            WD->tc          /* Watchdog Time Constant Register */
#define WDFEED          WD->feed        /* Watchdog Feed Register */
#define WDTV            WD->tv          /* Watchdog Time Value Register */

//WDMOD Register
#define WDEN      BIT0          // WDEN Watchdog interrupt Enable bit (Set Only).
#define WDRESET   BIT1          // WDRESET Watchdog Reset Enable bit (Set Only).
#define WDTOF     BIT2          // WDTOF Watchdog Time-Out Flag.
#define WDINT     BIT3          // WDINT Watchdog interrupt Flag (Read Only).


/* USB Controller */
#define USB_BASE_ADDR		0xE0090000			/* USB Base Address */
/* Device Interrupt Registers */
#define USB_DEV_INT_STAT    (*(volatile unsigned long *)(USB_BASE_ADDR + 0x00))
#define USB_DEV_INT_EN      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x04))
#define USB_DEV_INT_CLR     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x08))
#define USB_DEV_INT_SET     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x0C))
#define USB_DEV_INT_PRIO    (*(volatile unsigned long *)(USB_BASE_ADDR + 0x2C))

/* Endpoint Interrupt Registers */
#define USB_EP_INT_STAT     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x30))
#define USB_EP_INT_EN       (*(volatile unsigned long *)(USB_BASE_ADDR + 0x34))
#define USB_EP_INT_CLR      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x38))
#define USB_EP_INT_SET      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x3C))
#define USB_EP_INT_PRIO     (*(volatile unsigned long *)(USB_BASE_ADDR + 0x40))

/* Endpoint Realization Registers */
#define USB_REALIZE_EP      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x44))
#define USB_EP_INDEX        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x48))
#define USB_MAXPACKET_SIZE  (*(volatile unsigned long *)(USB_BASE_ADDR + 0x4C))

/* Command Reagisters */
#define USB_CMD_CODE        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x10))
#define USB_CMD_DATA        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x14))

/* Data Transfer Registers */
#define USB_RX_DATA         (*(volatile unsigned long *)(USB_BASE_ADDR + 0x18))
#define USB_TX_DATA         (*(volatile unsigned long *)(USB_BASE_ADDR + 0x1C))
#define USB_RX_PLENGTH      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x20))
#define USB_TX_PLENGTH      (*(volatile unsigned long *)(USB_BASE_ADDR + 0x24))
#define USB_USB_CTRL        (*(volatile unsigned long *)(USB_BASE_ADDR + 0x28))

/* DMA Registers */
#define USB_DMA_REQ_STAT        (*((volatile unsigned long *)USB_BASE_ADDR + 0x50))
#define USB_DMA_REQ_CLR         (*((volatile unsigned long *)USB_BASE_ADDR + 0x54))
#define USB_DMA_REQ_SET         (*((volatile unsigned long *)USB_BASE_ADDR + 0x58))
#define USB_UDCA_HEAD           (*((volatile unsigned long *)USB_BASE_ADDR + 0x80))
#define USB_EP_DMA_STAT         (*((volatile unsigned long *)USB_BASE_ADDR + 0x84))
#define USB_EP_DMA_EN           (*((volatile unsigned long *)USB_BASE_ADDR + 0x88))
#define USB_EP_DMA_DIS          (*((volatile unsigned long *)USB_BASE_ADDR + 0x8C))
#define USB_DMA_INT_STAT        (*((volatile unsigned long *)USB_BASE_ADDR + 0x90))
#define USB_DMA_INT_EN          (*((volatile unsigned long *)USB_BASE_ADDR + 0x94))
#define USB_EOT_INT_STAT        (*((volatile unsigned long *)USB_BASE_ADDR + 0xA0))
#define USB_EOT_INT_CLR         (*((volatile unsigned long *)USB_BASE_ADDR + 0xA4))
#define USB_EOT_INT_SET         (*((volatile unsigned long *)USB_BASE_ADDR + 0xA8))
#define USB_NDD_REQ_INT_STAT    (*((volatile unsigned long *)USB_BASE_ADDR + 0xAC))
#define USB_NDD_REQ_INT_CLR     (*((volatile unsigned long *)USB_BASE_ADDR + 0xB0))
#define USB_NDD_REQ_INT_SET     (*((volatile unsigned long *)USB_BASE_ADDR + 0xB4))
#define USB_SYS_ERR_INT_STAT    (*((volatile unsigned long *)USB_BASE_ADDR + 0xB8))
#define USB_SYS_ERR_INT_CLR     (*((volatile unsigned long *)USB_BASE_ADDR + 0xBC))
#define USB_SYS_ERR_INT_SET     (*((volatile unsigned long *)USB_BASE_ADDR + 0xC0))    
#define USB_MODULE_ID           (*((volatile unsigned long *)USB_BASE_ADDR + 0xFC))

#endif  // __LPC214x_H
