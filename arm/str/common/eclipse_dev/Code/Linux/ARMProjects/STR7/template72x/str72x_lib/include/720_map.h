/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_map.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : Peripherals registers definition and memory mapping.
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/


#ifndef __720_MAP_H
#define __720_MAP_H


#ifndef EXT
  #define EXT extern
#endif /* EXT */

#include "720_conf.h"
#include "720_type.h"

/******************************************************************************/
/*                          IP registers structures			      */
/******************************************************************************/


/*------------------------------- ATAPI-IDE Registers ------------------------*/

typedef volatile struct
{
  vu8  DATA        ;      //  Data register
  vu8  ERRFEA      ;      //  Error/Feature register
  vu8  SECCOUNT    ;      //  sector Count register
  vu8  SECNUMB     ;      //  Sector number register
  vu8  CYLLOW      ;      //  Cylinder Low register
  vu8  CYLHIGH     ;      //  Cylinder High register
  vu8  DEVHEAD     ;      //  Device/Head  register
  vu8  STATUSCMD   ;      //  Status / Command register
  vu8  EMPTY1 [7]  ;
  vu8  DEVCTRL     ;      //  Device Control register
  vu8  EMPTY2      ;
  vu8  EMPTY3 [107];
  vu16 VENID       ;      //  Vendor ID register
  vu16 DEVID       ;      //  Device ID register
  vu16 COMMAND     ;      //  Command register
  vu16 STATUS      ;      //  Status register
  vu8  REVID       ;      //  Revision ID register
  vu8  CCPROG      ;      //  Class Code Programming register
  vu16 CLASSCODE   ;      //  Class Code register
  vu16 EMPTY4      ;
  vu8  HEADTYPE    ;      //  Header Type register
  vu8  EMPTY5      ;
  vu16 EMPTY6[24]  ;
  vu16 PIDETIM     ;      //  Primary IDE Timing register
  vu16 EMPTY7      ;
  vu16 SLIDETIM    ;      //  Slave IDE Timing register
} IDE_TypeDef;

/**************************A-APB Sub System memory Map**************************/


/*-----------Asynchronous AHB-APB bridge registers strcture--------------------*/

typedef volatile struct
{
  vu32 BSR;          //Bridge Status Register
  vu32 EMPTY0;
  vu32 PAER;         //Peripheral Address Error Register
  vu32 EMPTY1[21];
  vu32 PCG0;         //Peripheral Clock Gating Register
  vu32 EMPTY2[7];
  vu32 PUR0;         //Peripheral under Reset Register
} A3BRG_TypeDef;


/*----------------  A-APB Global Configuration Register -----------------------*/
typedef volatile struct
{
  vu16 CR1     ;     //A-APB Global Configuration Register 1

} AGCR_TypeDef;

/*---------------------- Buffered Serial Port interface ----------------------*/

typedef volatile struct
{
  vu16 RXR;        //Receive Register
  vu16 EMPTY1;
  vu16 TXR;        //Transmit Register
  vu16 EMPTY2;
  vu16 CSR1;       //control Status Register 1
  vu16 EMPTY3;
  vu16 CSR2;       //Control Status Register 2
  vu16 EMPTY4;
  vu16 CLK;        //Master Clock Divider Register
  vu16 EMPTY5;
  vu16 CSR3;       //Control Status Register 3
} BSPI_TypeDef;

/*------------------- Universal Asynchronous Receiver Transmitter ------------*/
typedef volatile struct
{
  vu16 BRR;          //Baud Rate Register
  vu16 EMPTY1;
  vu16 TBR;          //Transmit Buffer Register
  vu16 EMPTY2;
  vu16 RBR;          //Receive Buffer Register
  vu16 EMPTY3;
  vu16 CR;           //Control Register
  vu16 EMPTY4;
  vu16 IER;          //Interrrut Enable Register
  vu16 EMPTY5;
  vu16 SR;           //Status Register
  vu16 EMPTY6;
  vu16 TOR;          //Time Out Register
  vu16 EMPTY7;
  vu16 TRR;          //Transmit Reset Register
  vu16 EMPTY8;
  vu16 RRR;          //Receive Reset Register
} UART_TypeDef;

/*-------------------- Extended Function Timer -------------------------------*/

typedef volatile struct
{
  vu16 ICAR;   // Input Capture A Register
  vu16 EMPTY1;
  vu16 ICBR;   // Input Capture B Register
  vu16 EMPTY2;
  vu16 OCAR;   // Output Compare A Register
  vu16 EMPTY3;
  vu16 OCBR;   // Input Capture B Register
  vu16 EMPTY4;
  vu16 CNTR;   // Counter Register
  vu16 EMPTY5;
  vu16 CR1;    // Control Register 1
  vu16 EMPTY6;
  vu16 CR2;    // Control Register 2
  vu16 EMPTY7;
  vu16 SR;     // Status Register
} EFT_TypeDef;

/*----------------  Analog to Digital Convertor -----------------------*/
typedef volatile struct
{
  vu16 DATA0 ;       //Data Register 0
  vu16 EMPTY1[3];
  vu16 DATA1 ;       //Data Register 1
  vu16 EMPTY2[3];
  vu16 DATA2 ;       //Data Register 2
  vu16 EMPTY3[3];
  vu16 DATA3 ;       //Data Register 3
  vu16 EMPTY4[3];
  vu16 CSR   ;       //Control Status Register
  vu16 EMPTY5[7];
  vu16 CPR   ;       //Clock Prescalar register
} ADC_TypeDef;

/*---------------- Controller Area Network Interface Register ------------------*/

typedef volatile struct
{
  vu16 COMR;
  vu16 EMPTY1;
  vu16 COMM;
  vu16 EMPTY2;
  vu16 MASK1;
  vu16 EMPTY3;
  vu16 MASK2;
  vu16 EMPTY4;
  vu16 ARB1;
  vu16 EMPTY5;
  vu16 ARB2;
  vu16 EMPTY6;
  vu16 MSGC;
  vu16 EMPTY7;
  vu16 DA1;
  vu16 EMPTY8;
  vu16 DA2;
  vu16 EMPTY9;
  vu16 DB1;
  vu16 EMPTY10;
  vu16 DB2;
  vu16 EMPTY11[27];
} CAN_MsgObj_TypeDef;

typedef volatile struct
{
  vu16 CR;
  vu16 EMPTY1;
  vu16 SR;
  vu16 EMPTY2;
  vu16 ERR;
  vu16 EMPTY3;
  vu16 BTR;
  vu16 EMPTY4;
  vu16 INTR;
  vu16 EMPTY5;
  vu16 TESTR;
  vu16 EMPTY6;
  vu16 BRPR;
  vu16 EMPTY7[3];
  CAN_MsgObj_TypeDef sMsgObj[2];
  vu16 EMPTY8[16];
  vu16 TR1;
  vu16 EMPTY9;
  vu16 TR2;
  vu16 EMPTY10[13];
  vu16 NEWD1;
  vu16 EMPTY11;
  vu16 NEWD2;
  vu16 EMPTY12[13];
  vu16 INTP1;
  vu16 EMPTY13;
  vu16 INTP2;
  vu16 EMPTY14[13];
  vu16 MSGV1;
  vu16 EMPTY15;
  vu16 MSGV2;
  vu16 EMPTY16;
} CAN_TypeDef;

/*--------------------------------- USB Registers --------------------------------*/

typedef volatile struct
{
  vu32 ENDP_0;
  vu32 ENDP_1;
  vu32 ENDP_2;
  vu32 ENDP_3;
  vu32 ENDP_4;
  vu32 ENDP_5;
  vu32 ENDP_6;
  vu32 ENDP_7;
  vu32 ENDP_8;
  vu32 ENDP_9;
  vu32 ENDP_10;
  vu32 ENDP_11;
  vu32 ENDP_12;
  vu32 ENDP_13;
  vu32 ENDP_14;
  vu32 ENDP_15;
  vu32 CNTR_;
  vu32 ISTR_;
  vu32 FNR_;
  vu32 DADDR_;
  vu32 BTABLE_;
} USB_TypeDef;

/*----------------------------- WatchDoG Timer ---------------------------*/

typedef volatile struct
{
  vu16 CR;        //WDG Control register
  vu16 EMPTY1;
  vu16 PR;        //WDG Presclar register
  vu16 EMPTY2;
  vu16 VR;        //WDG pre-load Value Register
  vu16 EMPTY3;
  vu16 CNT;       //WDG Counter Register
  vu16 EMPTY4;
  vu16 SR;        //WDG Status Register
  vu16 EMPTY5;
  vu16 MR;        //WDG Mask Register
  vu16 EMPTY6;
  vu16 KR;        //WDG Key Register
} WDG_TypeDef;

/**************************S-APB Sub System memory Map*************************/

/*------------------------------DRAM Controller-------------------------------*/

typedef volatile struct
{
  vu32 MB1Config;      //Bank1 configuration register
  vu32 MB2Config;      //Bank2 configuration register
  vu32 MB3Config;      //Bank3 configuration register
  vu32 MB4Config;      //Bank4 configuration register
  vu32 SDRAM1ConfigLo; //Bank1 SDRAM configuration register Low
  vu32 SDRAM1ConfigHi; //Bank1 SDRAM configuration register Hi
  vu32 SDRAM2ConfigLo; //Bank2 SDRAM configuration register Low
  vu32 SDRAM2ConfigHi; //Bank2 SDRAM configuration register Hi
  vu32 SDRAM3ConfigLo; //Bank3 SDRAM configuration register Low
  vu32 SDRAM3ConfigHi; //Bank3 SDRAM configuration register Hi
  vu32 SDRAM4ConfigLo; //Bank4 SDRAM configuration register Low
  vu32 SDRAM4ConfigHi; //Bank4 SDRAM configuration register Hi
  vu32 MemConfig;      //Memory configuaration register
  vu32 Bank1Size;      //Size of Bank1 register
  vu32 Bank2Size;      //Size of Bank2 register
  vu32 Bank3Size;      //Size of Bank3 register
  vu32 Bank4Size;      //Size of Bank4 register
} DRAMC_TypeDef;


/*---------------- External Memory Interface --------------------------------*/

typedef volatile struct
{
  vu16 BCON0      ;      // Bank0 Configuration register
  vu16 EMPTY1     ;
  vu16 BCON1      ;      // Bank1 Configuration register
  vu16 EMPTY2[5]  ;
  vu16 BBASE1     ;      // Bank1 Base Address register
} EMI_TypeDef;

/*------------------------ DMA Controller ------------------------------------*/

typedef volatile struct

{
  vu16 DMASoLo0;          //DMAC source base address low 0 register
  vu16 EMPTY1;
  vu16 DMASoHi0;          //DMAC source base address high 0 register
  vu16 EMPTY2;
  vu16 DMADeLo0;          //DMAC destination base address low 0 register
  vu16 EMPTY3;
  vu16 DMADeHi0;          //DMAC destination base address high 0 register
  vu16 EMPTY4;
  vu16 DMAMax0;           // DMAC Maximum count register
  vu16 EMPTY5;
  vu16 DMACtrl0;          //DMAC Control register
  vu16 EMPTY6;
  vu16 DMASoCurrHi0;      //DMAC Current Source address high 0 register
  vu16 EMPTY7;
  vu16 DMASoCurrLo0;      //DMAC Current Source address low 0 register
  vu16 EMPTY8;
  vu16 DMADeCurrHi0;      //DMAC Current Destination address high 0 register
  vu16 EMPTY9;
  vu16 DMADeCurrLo0;      //DMAC Current Destination address low 0 register
  vu16 EMPTY10;
  vu16 DMATCnt0;          //DMAC Terminal Counter Register
  vu16 EMPTY11;
  vu16 DMALUBuff0;        //DMAC Last Used Buffer location register
  vu16 EMPTY12[9];
  vu16 DMASoLo1;          //DMAC source base address low 1 register
  vu16 EMPTY13;
  vu16 DMASoHi1;          //DMAC source base address high 1 register
  vu16 EMPTY14;
  vu16 DMADeLo1;          //DMAC destination base address low 1 register
  vu16 EMPTY15;
  vu16 DMADeHi1;          //DMAC destination base address high 1 register
  vu16 EMPTY16;
  vu16 DMAMax1;           // DMAC Maximum count register
  vu16 EMPTY17;
  vu16 DMACtrl1;          //DMAC Control register
  vu16 EMPTY18;
  vu16 DMASoCurrHi1;      //DMAC Current Source address high 1 register
  vu16 EMPTY19;
  vu16 DMASoCurrLo1;      //DMAC Current Source address low 1 register
  vu16 EMPTY20;
  vu16 DMADeCurrHi1;      //DMAC Current Destination address high 1 register
  vu16 EMPTY21;
  vu16 DMADeCurrLo1;      //DMAC Current Destination address low 1 register
  vu16 EMPTY22;
  vu16 DMATCnt1;          //DMAC Terminal Counter Register
  vu16 EMPTY23;
  vu16 DMALUBuff1;        //DMAC Last Used Buffer location register
  vu16 EMPTY24[9];
  vu16 DMASoLo2;          //DMAC source base address low 2 register
  vu16 EMPTY25;
  vu16 DMASoHi2;          //DMAC source base address high 2 register
  vu16 EMPTY26;
  vu16 DMADeLo2;          //DMAC destination base address low 2 register
  vu16 EMPTY27;
  vu16 DMADeHi2;          //DMAC destination base address high 2 register
  vu16 EMPTY28;
  vu16 DMAMax2;           // DMAC Maximum count register
  vu16 EMPTY29;
  vu16 DMACtrl2;          //DMAC Control register
  vu16 EMPTY30;
  vu16 DMASoCurrHi2;      //DMAC Current Source address high 2 register
  vu16 EMPTY31;
  vu16 DMASoCurrLo2;      //DMAC Current Source address low 2 register
  vu16 EMPTY32;
  vu16 DMADeCurrHi2;      //DMAC Current Destination address high 2 register
  vu16 EMPTY33;
  vu16 DMADeCurrLo2;      //DMAC Current Destination address low 2 register
  vu16 EMPTY34;
  vu16 DMATCnt2;          //DMAC Terminal Counter Register
  vu16 EMPTY35;
  vu16 DMALUBuff2;        //DMAC Last Used Buffer location register
  vu16 EMPTY36[9];
  vu16 DMASoLo3;          //DMAC source base address low 3 register
  vu16 EMPTY37;
  vu16 DMASoHi3;          //DMAC source base address high 3 register
  vu16 EMPTY38;
  vu16 DMADeLo3;          //DMAC destination base address low 3 register
  vu16 EMPTY39;
  vu16 DMADeHi3;          //DMAC destination base address high 3 register
  vu16 EMPTY40;
  vu16 DMAMax3;           // DMAC Maximum count register
  vu16 EMPTY41;
  vu16 DMACtrl3;          //DMAC Control register
  vu16 EMPTY42;
  vu16 DMASoCurrHi3;      //DMAC Current Source address high 3 register
  vu16 EMPTY43;
  vu16 DMASoCurrLo3;      //DMAC Current Source address low 3 register
  vu16 EMPTY44;
  vu16 DMADeCurrHi3;      //DMAC Current Destination address high 3 register
  vu16 EMPTY45;
  vu16 DMADeCurrLo3;      //DMAC Current Destination address low 3 register
  vu16 EMPTY46;
  vu16 DMATCnt3;          //DMAC Terminal Counter Register
  vu16 EMPTY47;
  vu16 DMALUBuff3;        //DMAC Last Used Buffer location register
  vu16 EMPTY48;
  vu16 DMAMask;           //DMAC Interrupt Mask Register
  vu16 EMPTY49;
  vu16 DMAClr;            //DMAC Interrupt Clear Register
  vu16 EMPTY50;
  vu16 DMAStatus;         //DMAC Interrupt Status Register
  vu16 EMPTY51;
  vu16 DMALast;           //DMAC Last Flag Register
  vu16 EMPTY52;
} DMAC_TypeDef;

/*---------------- Synchronous APB Global Control Registers ------------------*/

typedef volatile struct
{
  vu16 CR1      ;      // Synchronous APB Global Control Register 1
  vu16 EMPTY1   ;
  vu16 CR2      ;      // Synchronous APB Global Control Register 2
  vu16 EMPTY2   ;
  vu16 CR3      ;      // Synchronous APB Global Control Register 3
  vu16 EMPTY3   ;
} SAPB_TypeDef;

/*------------------- Reset and Clock Control Unit ---------------------------*/

typedef volatile struct
{
  vu16 PLLCONF    ;    // PLL Configuration Register
  vu16 EMPTY1     ;
  vu16 DIVCONF    ;    // Dividers configuration Register
  vu16 EMPTY2     ;
  vu16 CLKFLAG    ;    // Clock Flag Register
  vu16 EMPTY3     ;
  vu16 CLKCTL     ;    // Clock Control Register
  vu16 EMPTY4     ;
  vu16 MSKCTL     ;    // Mask control Register
  vu16 EMPTY5     ;
  vu16 SYSPROT    ;    // System Protection Register
} RCCU_TypeDef;


/*--------------- General Purpose Input Output -------------------------------*/
typedef volatile struct
{
  vu16 PC0;         //Port configuration register 0
  vu16 EMPTY1;
  vu16 PC1;         //Port configuration register 1
  vu16 EMPTY2;
  vu16 PC2;         //Port configuration register 2
  vu16 EMPTY3;
  vu16 PD;          //Input/Output Data Register
  vu16 EMPTY4;
} GPIO_TypeDef;


/*------------------- Wake-up / Interrupt management Unit----------------------*/

typedef volatile struct
{
  vu16  CTRL  ;      //Control Register
  vu16  EMPTY1;
  vu16  MR    ;      //Mask Register
  vu16  EMPTY2;
  vu16  TR    ;      //Trigger Register
  vu16  EMPTY3;
  vu16  PR    ;      //Pending Register
} WIU_TypeDef;

/*----------------Real Time Clock registers structure ------------------------*/

typedef volatile struct
{
  vu16 CR;        //RTC Control Register
  vu16 EMPTY1;
  vu16 CNTL;      //RTC Coutner Low Register
  vu16 EMPTY2;
  vu16 CNTM;      //RTC Counter Middle Register
  vu16 EMPTY3;
  vu16 CNTH;      //RTC Counter High Register
  vu16 EMPTY4;
  vu16 AL;        //RTC Alarm low register
  vu16 EMPTY5;
  vu16 AM;        //RTC Alarm Middle register
  vu16 EMPTY6;
  vu16 AH;        //RTC Alarm High register
  vu16 EMPTY7;
  vu16 PL;        //RTC Periodic Value Low register
  vu16 EMPTY8;
  vu16 PH;        //RTC Periodic Value High register
} RTC_TypeDef;

/*---------------- Clock Gating Control Block --------------------------------*/

typedef volatile struct
{
  vu16 PCG1     ;      // Peripheral Clock Gating Register 1
  vu16 EMPTY1   ;
  vu16 PUR1     ;      // Peripheral Under Reset register 1
  vu16 EMPTY2   ;
  vu16 EMU1     ;      // Emulation Register 1
  vu16 EMPTY3   ;
  vu16 PCG2     ;      // Peripheral Clock Gating Register 2
  vu16 EMPTY4   ;
  vu16 PUR2     ;      // Peripheral Under Reset Register 2
  vu16 EMPTY5   ;
  vu16 EMU2     ;      // Emulation Register 2
  vu16 EMPTY6   ;
}CGC_TypeDef;

/*--------------- Advanced high performance bus Error Decoder ----------------*/

typedef volatile struct
{
  vu32 AERRP    ;      // AHB Error Pending register
  vu32 AERRM    ;      // AHB Error Mask register
  vu32 AERRFA   ;      // AHB Error Faulty Address
} AHB_TypeDef;


/*-------------- Enhanced Interrupt Controller --------------------------------*/
typedef volatile struct
{
  vu32 ICR;       // Interrupt Control Register
  vu32 CICR;      // Current Interrupt Channel Register
  vu32 CIPR;      // Current Interrupt Priority Register
  vu32 EMPTY1[3];
  vu32 IVR;       // Interrupt Vector Register
  vu32 FIR;       // Fast Interrupt Register
  vu32 IER;       // Interrupt Enable Register
  vu32 EMPTY2[7];
  vu32 IPR;       //  Interrupt Pending Register
  vu32 EMPTY3[7];
  vu32 SIR[32]  ; // Source Interrupt Registers - Channel n
} EIC_TypeDef;


/******************************************************************************
*                   Peripherals' Base addresses
******************************************************************************/

#define IDE_BASE          0xC0000000     // ATAPI IDE base Address
#define AsAPB             0xE0000000     // Asynchronous APB bridge Base Address
#define SyAPB             0xF0000000     // Synchronous  APB bridge Base Address

/*------------ Asynchronous Peripherals---------------------------------------*/

#define A3BRG_BASE         AsAPB           // Asynchronous AHB-APB base address
#define AGCR_BASE         (AsAPB + 0x1000) // A-APB Global Configuration Register base address
#define GPIO2_BASE        (AsAPB + 0x2000) // General Purpose I/O 2 Base Address
#define BSPI1_BASE        (AsAPB + 0x3000) // Buffered Serial Port 1 Interface Base Address
#define BSPI2_BASE        (AsAPB + 0x4000) // Buffered Serial Port 2 Interface Base Address
#define UART1_BASE        (AsAPB + 0x5000) // Universal Asynchronous Receiver Transmitter 1 Base Address
#define UART2_BASE        (AsAPB + 0x6000) // Universal Asynchronous Receiver Transmitter 2 Base Address
#define EFT1_BASE         (AsAPB + 0x7000) // Extended Funtion Timer 1 Base Address
#define EFT2_BASE         (AsAPB + 0x8000) // Extended Funtion Timer 2 Base Address
#define ADC_BASE          (AsAPB + 0x9000) // Analog to Digital Convertor Base Address
#define CAN_BASE          (AsAPB + 0xA000) // CAN Base Address
#define USB_BASE	  (AsAPB + 0xB800) // USB Registers Base Address
#define WDG_BASE          (AsAPB + 0xC000) // Watch-Dog Timer Base Address

/*------------ Synchronous Peripherals ---------------------------------------*/

#define DRAMC_BASE       SyAPB 	   	 // SDRAM controller Base Address
#define EMI_BASE        (SyAPB + 0x0400) // External Memory Interface Base                                         // Address
#define DMAC_BASE       (SyAPB + 0x0800) // DMA Controller base address
#define SAPB_BASE       (SyAPB + 0x0C00) // Synchronous APB Global Control                                          // Registers Base Address
#define RCCU_BASE       (SyAPB + 0x1000) // Reset and Clock Control Unit Base                                          // Address
#define GPIO3_BASE      (SyAPB + 0x1400) // General Purpose I/O 3 Base Address
#define GPIO4_BASE      (SyAPB + 0x1800) // General Purpose I/O 4 Base Address
#define WIU_BASE        (SyAPB + 0x2400) // External Interrupt Management Base                                          // Address
#define RTC_BASE        (SyAPB + 0x2800) // Real Time clock base address
#define CGC_BASE        (SyAPB + 0x2C00) // Clock Gating Control Block Base Address
#define AHB_BASE        (SyAPB + 0x3000) // AHB Error Detector block
#define EIC_BASE        0xFFFFFC00	 // EIC Base Address


/*****************************************************************************
                          IPs' declaration
*****************************************************************************/

/*----------------- Non Debug Mode -----------------------------------------*/

#ifndef LIBDEBUG

#define IDE     ((IDE_TypeDef *) IDE_BASE)

/******************** Asynchronous Peripherals ******************************/

#define A3BRG   ((A3BRG_TypeDef *)   A3BRG_BASE)
#define AGCR    ((AGCR_TypeDef  *)   AGCR_BASE)
#define GPIO2   ((GPIO_TypeDef  *)   GPIO2_BASE)
#define BSPI1   ((BSPI_TypeDef  *)   BSPI1_BASE)
#define BSPI2   ((BSPI_TypeDef  *)   BSPI2_BASE)
#define UART1   ((UART_TypeDef  *)   UART1_BASE)
#define UART2   ((UART_TypeDef  *)   UART2_BASE)
#define EFT1    ((EFT_TypeDef   *)   EFT1_BASE)
#define EFT2    ((EFT_TypeDef   *)   EFT2_BASE)
#define ADC     ((ADC_TypeDef   *)   ADC_BASE)
#define CAN     ((CAN_TypeDef   *)   CAN_BASE)
#define USB     ((USB_TypeDef   *)   USB_BASE)
#define WDG     ((WDG_TypeDef   *)   WDG_BASE)

/******************** Synchronous Peripherals *********************************/

#define DRAMC   ((DRAMC_TypeDef  *)      DRAMC_BASE)
#define EMI     ((EMI_TypeDef    *)      EMI_BASE)
#define DMAC    ((DMAC_TypeDef   *)      DMAC_BASE)
#define SAPB    ((SAPB_TypeDef   *)      SAPB_BASE)
#define RCCU    ((RCCU_TypeDef   *)      RCCU_BASE)
#define GPIO3   ((GPIO_TypeDef   *)      GPIO3_BASE)
#define GPIO4   ((GPIO_TypeDef   *)      GPIO4_BASE)
#define WIU     ((WIU_TypeDef    *)      WIU_BASE)
#define RTC     ((RTC_TypeDef    *)      RTC_BASE)
#define CGC     ((CGC_TypeDef    *)      CGC_BASE)
#define AHB     ((AHB_TypeDef    *)      AHB_BASE)
#define EIC     ((EIC_TypeDef    *)      EIC_BASE)

#else   /* LIBDEBUG */

/*------------------------ Debug Mode ----------------------------------------*/

EXT IDE_TypeDef    	*IDE;

/******************** Asynchronous Peripherals ********************************/

EXT A3BRG_TypeDef 	*A3BRG;
EXT AGCR_TypeDef  	*AGCR;
EXT GPIO_TypeDef  	*GPIO2;
EXT BSPI_TypeDef  	*BSPI1;
EXT BSPI_TypeDef  	*BSPI2;
EXT UART_TypeDef  	*UART1;
EXT UART_TypeDef  	*UART2;
EXT EFT_TypeDef   	*EFT1;
EXT EFT_TypeDef   	*EFT2;
EXT ADC_TypeDef   	*ADC;
EXT CAN_TypeDef   	*CAN;
EXT USB_TypeDef   	*USB;
EXT WDG_TypeDef   	*WDG;

/******************** Synchronous Peripherals *********************************/

EXT DRAMC_TypeDef 	*DRAMC;
EXT EMI_TypeDef    	*EMI;
EXT DMAC_TypeDef   	*DMAC;
EXT SAPB_TypeDef   	*SAPB;
EXT RCCU_TypeDef   	*RCCU;
EXT GPIO_TypeDef   	*GPIO3;
EXT GPIO_TypeDef   	*GPIO4;
EXT WIU_TypeDef    	*WIU;
EXT RTC_TypeDef    	*RTC;
EXT CGC_TypeDef    	*CGC;
EXT AHB_TypeDef    	*AHB;
EXT EIC_TypeDef    	*EIC;

#endif  /* LIBDEBUG */

#endif  /* __720_MAP_H*/

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/

