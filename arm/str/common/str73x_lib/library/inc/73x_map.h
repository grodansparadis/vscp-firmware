/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : 73x_map.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the peripheral register's definitions
*                      and memory mapping.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------- */
#ifndef __73x_MAP_H
#define __73x_MAP_H

#ifndef EXT
  #define EXT extern
#endif /* EXT */

/* Includes --------------------------------------------------------------------*/
#include "73x_conf.h"
#include "73x_type.h"

/********************************************************************************/
/*                          IP registers structures               	        */
/********************************************************************************/

/**************************NATIVE  System memory Map*****************************/

/*-------------------------------FLASH -----------------------------------------*/
typedef struct
{
  vu32  CR0;
  vu32  CR1;
  vu32  DR0;
  vu32  DR1;
  vu32  AR;
  vu32  ER;
} FLASHR_TypeDef;
typedef struct
{
  vu32  NVWPAR;
  u32   EMPTY;
  vu32  NVAPR0;
  vu32  NVAPR1;
} FLASHPR_TypeDef;

/*------------------- Power, Reset and Clocks Unit------------------------------*/
typedef struct
{
  vu32  CCR   ;      /*Clock Control Register*/
  vu32  VRCTR ;      /*Voltage Regulator Control Register*/
  vu32  CFR ;        /*Clock Flag Register*/
  u32   EMPTY1[3];
  vu32  PLLCR  ;     /*PLL Configuration Register*/
  u32   EMPTY2 ;
  vu32  SMR ;        /*System Mode Register*/
  u32   EMPTY3 ;
  vu32  RTCPR ;      /*Real Time Clock Programming Register*/
} PRCCU_TypeDef;

/*----------------   Native Arbiter Registers ----------------------------------*/

typedef  struct
{
  vu32  TOR;
  vu32  PRIOR;
  u32   EMPTY;
  vu32  CTLR;
} ARB_TypeDef;
/*----------------   System Configuration Registers ----------------------------*/
typedef struct
{
  vu32  R0     ;    /*Configuration Register 0*/
  vu32  EITE0  ;    /*External Interrupt Trigger Event Register 0*/
  vu32  PCGR0  ;    /*Peripheral Clock Gating Register 0*/
  vu32  PCGR1  ;    /*Peripheral Clock Gating Register 1*/
  vu32  PECGR0 ;    /*Peripheral Emulation Clock Gating Register 0*/
  vu32  PECGR1 ;    /*Peripheral Emulation Clock Gating Register 1*/
  vu32  PCGRB0 ;    /*Peripheral Clock Gating Register B0*/
  vu32  PCGRB1 ;    /*Peripheral Clock Gating Register B1*/
  vu32  TIMSR  ;    /*TIM External Clock Select Register*/
  vu32  EITE1  ;    /*External Interrupt Trigger Event Register 1*/
  vu32  EITE2  ;    /*External Interrupt Trigger Event Register 2*/
  vu32  ESPR   ;    /*Emulation Serial Protection Register*/
  vu32  R1     ;    /*Configuration Register 1*/
  vuc32 DIDR   ;    /*Device Identification Register*/
}CFG_TypeDef;
/*----------------------------AHB BRIDGE --------------------------------------*/
typedef struct
{
  vu32  TOR;
  vu32  PRIOR;
  u32   EMPTY;
  vu32  CTLR;
} AHB_TypeDef;

/**************************APB  System memory Map*******************************/

/*----------------------------APB BRIDGE --------------------------------------*/

typedef struct
{
  vu32  BSR;
  vu32  TOR;
  vuc32 OMR; 
  vuc32 TOER;
} APB_TypeDef;

/*----------------  Clock Monitor Unit ----------------------------------------*/
typedef  struct
{
  vu16  RCCTL     ;  /* RCOscillator Control Register */
  u16   EMPTY1    ;
  vuc16 FDISP    ;  /* Frequency Display Register*/
  u16   EMPTY2    ;
  vu16  FRH       ;  /* Frequency Reference High Register*/
  u16   EMPTY3    ;
  vu16  FRL       ;  /* Frequency Reference Low Register*/
  u16   EMPTY4    ;
  vu16  CTRL      ;  /* Control Register*/
  u16   EMPTY5    ;
  vu16  STAT      ;  /* Status Register*/
  u16   EMPTY6    ;
  vu16  IS        ;  /* Interrupt Status Register*/
  u16   EMPTY7    ;
  vu16  IM        ;  /* Interrupt Mask Register*/
  u16   EMPTY8    ;
  vu16  EOCV      ;  /* End Of Countvalue Register*/
  u16   EMPTY9    ;
  vu16  WE        ;  /* Writing Enable Register*/
  u16   EMPTY10   ;
} CMU_TypeDef;

/*---------------------- Buffered Serial Port Interface ----------------------*/

typedef struct
{
  vuc16 RXR;        /*Receive Register*/
  u16   EMPTY1;
  vu16  TXR;        /*Transmit Register*/
  u16   EMPTY2;
  vu16  CSR1;       /*control Status Register 1*/
  u16   EMPTY3;
  vu16  CSR2;       /*Control Status Register 2*/
  u16   EMPTY4;
  vu16  CLK;        /*Master Clock Divider Register*/
  u16   EMPTY5;
  vu16  CSR3;       /*Control Status Register 3*/
  u16   EMPTY6;
} BSPI_TypeDef;

/*---------------------- I2C  ------------------------------------------------*/
typedef  struct
{
  vu8  CR;
  u8   EMPTY1[3];
  vuc8 SR1;
  u8   EMPTY2[3];
  vuc8 SR2;
  u8   EMPTY3[3];
  vu8  CCR;
  u8   EMPTY4[3];
  vu8  OAR1;
  u8   EMPTY5[3];
  vu8  OAR2;
  u8   EMPTY6[3];
  vu8  DR;
  u8   EMPTY7[3];
  vu8  ECCR;
  u8   EMPTY8[3];
} I2C_TypeDef;
/*------------------- Universal Asynchronous Receiver Transmitter ------------*/
typedef struct
{
  vu16  BR;       /*BaudRate register*/
  u16   EMPTY1;
  vu16  TxBUFR;   /*TxBuffer Register*/
  u16   EMPTY2;
  vuc16 RxBUFR;   /*RxBuffer Register*/
  u16   EMPTY3;
  vu16  CR;       /*Control Register*/
  u16   EMPTY4;
  vu16  IER;      /*IntEnable Register*/
  u16   EMPTY5;
  vuc16 SR;       /*Status Register*/
  u16   EMPTY6;
  vu16  TOR;      /*Timeout Register*/
  u16   EMPTY7;
  vu16  TxRSTR;   /*TxReset register*/
  u16   EMPTY8;
  vu16  RxRSTR;   /*RxReset register*/
  u16   EMPTY9;
} UART_TypeDef;

/*----------------  Analog to Digital Converter -----------------------*/
typedef struct
{
  vu16  CLR0 ;
  u16   EMPTY1;
  vu16  CLR1 ;
  u16   EMPTY2;
  vu16  CLR2 ;
  u16   EMPTY3;
  vu16  CLR3 ;
  u16   EMPTY4;
  vu16  CLR4 ;
  u16   EMPTY5;
  vu16  TRA0 ;
  u16   EMPTY6;
  vu16  TRA1 ;
  u16   EMPTY7;
  vu16  TRA2 ;
  u16   EMPTY8;
  vu16  TRA3 ;
  u16   EMPTY9;
  vu16  TRB0 ;
  u16   EMPTY10;
  vu16  TRB1 ;
  u16   EMPTY11;
  vu16  TRB2 ;
  u16   EMPTY12;
  vu16  TRB3 ;
  u16   EMPTY13;
  vu16  DMAR ;
  u16   EMPTY14[7];
  vu16  DMAE ;
  u16   EMPTY15  ;
  vu16  PBR  ;
  u16   EMPTY16;
  vu16  IMR  ;
  u16   EMPTY17;
  vuc16 D0 ;
  u16   EMPTY18;
  vuc16 D1 ;
  u16   EMPTY19;
  vuc16 D2 ;
  u16   EMPTY20;
  vuc16 D3 ;
  u16   EMPTY21;
  vuc16 D4 ;
  u16   EMPTY22;
  vuc16 D5 ;
  u16   EMPTY23;
  vuc16 D6 ;
  u16   EMPTY24;
  vuc16 D7 ;
  u16   EMPTY25;
  vuc16 D8 ;
  u16   EMPTY26;
  vuc16 D9 ;
  u16   EMPTY27;
  vuc16 D10;
  u16   EMPTY28;
  vuc16 D11 ;
  u16   EMPTY29;
  vuc16 D12 ;
  u16   EMPTY30;
  vuc16 D13 ;
  u16   EMPTY31;
  vuc16 D14 ;
  u16   EMPTY32;
  vuc16 D15 ;
  u16   EMPTY33;
} ADC_TypeDef;

/*-------------------- General Purpose IO ports---------------------------*/
typedef struct
{
  vu16  PC0;
  u16   EMPTY1;
  vu16  PC1;
  u16   EMPTY2;
  vu16  PC2;
  u16   EMPTY3;
  vu16  PD;
  u16   EMPTY4;
} GPIO_TypeDef;

/*--------------------------Real Time Clock ------------------------------*/
typedef struct
{
  vu16  CRH;
  u16   EMPTY1;
  vu16  CRL;
  u16   EMPTY2;
  vu16  PRLH;
  u16   EMPTY3;
  vu16  PRLL;
  u16   EMPTY4;
  vu16  DIVH;
  u16   EMPTY5;
  vu16  DIVL;
  u16   EMPTY6;
  vu16  CNTH;
  u16   EMPTY7;
  vu16  CNTL;
  u16   EMPTY8;
  vu16  ALRH;
  u16   EMPTY9;
  vu16  ALRL;
  u16   EMPTY10;
} RTC_TypeDef;

/*-------------- Enhanced Interrupt Controller ----------------------------*/
typedef struct
{
  vu32  ICR      ;   /*  Interrupt Control Register*/
  vuc32 CICR     ;   /*  Current Interrupt Channel Register*/
  vu32  CIPR     ;   /*  Current Interrupt Priority Register*/
  u32   EMPTY1   ;   
  vu32  FIER     ;   /*  Fast Interrupt Enable Register*/
  vu32  FIPR     ;   /*  Fast Interrupt Pending Register*/
  vu32  IVR      ;   /*  Interrupt Vector Register*/
  vu32  FIR      ;   /*  Fast Interrupt Register*/
  vu32  IER0     ;   /*  Interrupt Enable Register0*/
  vu32  IER1     ;   /*  Interrupt Enable Register1*/
  u32   EMPTY3[6];
  vu32  IPR0     ;   /*  Interrupt Pending Register0*/
  vu32  IPR1     ;   /*  Interrupt Pending Register1*/
  u32   EMPTY4[6];
  vu32  SIRn[64] ;   /*  Source Interrupt registers- Channel n*/
} EIC_TypeDef;

/*-------------------------- Wake-up/Interrupt Unit-----------------------*/

typedef  struct
{
  vu32  CTRL  ;   /*Control Register*/
  vu32  MR    ;   /*Mask Register*/
  vu32  TR    ;   /*Trigger Register*/
  vu32  PR    ;   /*Pending Register*/
  vu32  INTR  ;   /*Software Interrupt Register*/
} WIU_TypeDef;

/*----------------------------------- Wake-Up Timer-----------------------*/
typedef struct
{
  vu16  CR    ;   /*Control Register*/
  u16   EMPTY1;
  vu16  PR    ;   /*Prescaler Register*/
  u16   EMPTY2;
  vu16  VR    ;   /*Pre-load Value Register*/
  u16   EMPTY3;
  vuc16 CNT  ;   /*Counter Register*/
  u16   EMPTY4;
  vu16  SR    ;   /*Status Register*/
  u16   EMPTY5;
  vu16  MR    ;   /*Mask Register*/
  u16   EMPTY6;
} WUT_TypeDef;
/*-------------------- Extended Function TIMer ---------------------------*/

typedef  struct
{
  vuc16 ICAR;   /* Input Capture A Register*/
  u16   EMPTY1;
  vuc16 ICBR;   /* Input Capture B Register*/
  u16   EMPTY2;
  vu16  OCAR;   /* Output Compare A Register*/
  u16   EMPTY3;
  vu16  OCBR;   /* Input Capture B Register*/
  u16   EMPTY4;
  vu16  CNTR;   /* Counter Register */
  u16   EMPTY5;
  vu16  CR1;    /* Control Register 1 */
  u16   EMPTY6;
  vu16  CR2;    /* Control Register 2 */
  u16   EMPTY7;
  vu16  SR;     /* Status Register */
  u16   EMPTY8;
} TIM_TypeDef;

/*-------------------------------- TimeBase timer-------------------------*/
typedef  struct
{
  vu16  CR  ;
  u16   EMPTY1;
  vu16  PR  ;
  u16   EMPTY2;
  vu16  VR  ;
  u16   EMPTY3;
  vuc16 CNT ;
  u16   EMPTY4;
  vu16  SR  ;
  u16   EMPTY5;
  vu16  MR  ;
  u16   EMPTY6;
} TB_TypeDef;

/*----------------------------- Pulse Width Modulator----------------------*/
typedef  struct
{
  vu16  PRS0  ;
  u16   EMPTY1;
  vu16  PRS1  ;
  u16   EMPTY2;
  vu16  PEN  ;
  u16   EMPTY3;
  vu16  PLS ;
  u16   EMPTY4;
  vu16  CPI  ;
  u16   EMPTY5;
  vu16  IM  ;
  u16   EMPTY6[3];
  vu16  DUT  ;
  u16   EMPTY7;
  vu16  PER  ;
  u16   EMPTY8;

} PWM_TypeDef;
/*----------------------------- WatchDoG Timer ---------------------------*/
typedef  struct
{
  vu16  CR;        /*WDG Control register*/
  u16   EMPTY1;
  vu16  PR;        /*WDG Presclar register*/
  u16   EMPTY2;
  vu16  VR;        /*WDG pre-load Value Register*/
  u16   EMPTY3;
  vuc16 CNT;       /*WDG Counter Register*/
  u16   EMPTY4;
  vu16  SR;        /*WDG Status Register*/
  u16   EMPTY5;
  vu16  MR;        /*WDG Mask Register*/
  u16   EMPTY6;
  vu16  KR;        /*WDG Key Register*/
  u16   EMPTY7;

} WDG_TypeDef;

/*------------------------ DMA Controller ------------------------------------*/

typedef  struct

{
  vu16  SOURCEL0;   /*DMA source base address low 0 register*/
  u16   EMPTY1;
  vu16  SOURCEH0;   /*DMA source base address high 0 register*/
  u16   EMPTY2;
  vu16  DESTL0;     /*DMA destination base address low 0 register*/
  u16   EMPTY3;
  vu16  DESTH0;     /*DMA destination base address high 0 register*/
  u16   EMPTY4;
  vu16  MAX0;       /*DMA Maximum count register*/
  u16   EMPTY5;
  vu16  CTRL0;      /*DMA Control register*/
  u16   EMPTY6;
  vuc16 SOCURRH0;    /*DMA Current Source address high 0 register*/
  u16   EMPTY7;
  vuc16 SOCURRL0;    /*DMA Current Source address low 0 register*/
  u16   EMPTY8;
  vuc16 DECURRH0;    /*DMA Current Destination address high 0 register*/
  u16   EMPTY9;
  vuc16 DECURRL0;    /*DMA Current Destination address low 0 register*/
  u16   EMPTY10;
  vuc16 TCNT0;       /*DMA Terminal Counter Register*/
  u16   EMPTY11;
  vu16  LUBUff0;    /*DMA Last Used Buffer location register*/
  u16   EMPTY12[9];
  vu16  SOURCEL1;   /*DMA source base address low 1 register*/
  u16   EMPTY13;
  vu16  SOURCEH1;   /*DMA source base address high 1 register*/
  u16   EMPTY14;
  vu16  DESTL1;     /*DMA destination base address low 1 register*/
  u16   EMPTY15;
  vu16  DESTH1;     /*DMA destination base address high 1 register*/
  u16   EMPTY16;
  vu16  MAX1;       /*DMA Maximum count register*/
  u16   EMPTY17;
  vu16  CTRL1;      /*DMA Control register*/
  u16   EMPTY18;
  vuc16 SOCURRH1;   /*DMA Current Source address high 1 register*/
  u16   EMPTY19;
  vuc16 SOCURRL1;   /*DMA Current Source address low 1 register*/
  u16   EMPTY20;
  vuc16 DECURRH1;   /*DMA Current Destination address high 1 register*/
  u16   EMPTY21;
  vuc16 DECURRL1;   /*DMA Current Destination address low 1 register*/
  u16   EMPTY22;
  vuc16 TCNT1;      /*DMA Terminal Counter Register*/
  u16   EMPTY23;
  vu16  LUBUff1;    /*DMA Last Used Buffer location register*/
  u16   EMPTY24[9];
  vu16  SOURCEL2;   /*DMA source base address low 2 register*/
  u16   EMPTY25;
  vu16  SOURCEH2;   /*DMA source base address high 2 register*/
  u16   EMPTY26;
  vu16  DESTL2;     /*DMA destination base address low 2 register*/
  u16   EMPTY27;
  vu16  DESTH2;     /*DMA destination base address high 2 register*/
  u16   EMPTY28;
  vu16  MAX2;       /*DMA Maximum count register*/
  u16   EMPTY29;
  vu16  CTRL2;      /*DMA Control register*/
  u16   EMPTY30;
  vuc16 SOCURRH2;   /*DMA Current Source address high 2 register*/
  u16   EMPTY31;
  vuc16 SOCURRL2;   /*DMA Current Source address low 2 register*/
  u16   EMPTY32;
  vuc16 DESCURRH2;  /*DMA Current Destination address high 2 register*/
  u16   EMPTY33;
  vuc16 DESCURRL2;  /*DMA Current Destination address low 2 register*/
  u16   EMPTY34;
  vuc16 TCNT2;      /*DMA Terminal Counter Register*/
  u16   EMPTY35;
  vu16  LUBUff2;    /*DMA Last Used Buffer location register*/
  u16   EMPTY36[9];
  vu16  SOURCEL3;   /*DMA source base address low 3 register*/
  u16   EMPTY37;
  vu16  SOURCEH3;   /*DMA source base address high 3 register*/
  u16   EMPTY38;
  vu16  DESTL3;     /*DMA destination base address low 3 register*/
  u16   EMPTY39;
  vu16  DESTH3;     /*DMA destination base address high 3 register*/
  u16   EMPTY40;
  vu16  MAX3;       /*DMA Maximum count register*/
  u16   EMPTY41;
  vu16  CTRL3;      /*DMA Control register*/
  u16   EMPTY42;
  vuc16 SOCURRH3;   /*DMA Current Source address high 3 register*/
  u16   EMPTY43;
  vuc16 SOCURRL3;   /*DMA Current Source address low 3 register*/
  u16   EMPTY44;
  vuc16 DECURRH3;   /*DMA Current Destination address high 3 register*/
  u16   EMPTY45;
  vuc16 DECURRL3;   /*DMA Current Destination address low 3 register*/
  u16   EMPTY46;
  vuc16 TCNT3;      /*DMA Terminal Counter Register*/
  u16   EMPTY47;
  vu16  LUBUff3;    /*DMA Last Used Buffer location register*/
  u16   EMPTY48;
  vu16  MASK;       /*DMA Interrupt Mask Register*/
  u16   EMPTY49;
  vu16  CLR;        /*DMA Interrupt Clear Register*/
  u16   EMPTY50;
  vuc16 STATUS;     /*DMA Interrupt Status Register*/
  u16   EMPTY51;
  vu16  Last;       /*DMA Last Flag Register*/
  u16   EMPTY52;
} DMA_TypeDef;

/*------------------------------ Controller Area Network  ------------------*/

typedef  struct
{
  vu16  CRR;			/*IFn Command request Register*/
  u16   EMPTY1;
  vu16  CMR;			/*IFn Command Mask Register*/
  u16   EMPTY2;
  vu16  M1R;			/*IFn Message Mask 1 Register*/
  u16   EMPTY3;
  vu16  M2R;			/*IFn Message Mask 2 Register*/
  u16   EMPTY4;
  vu16  A1R;			/*IFn Message Arbitration 1 Register*/
  u16   EMPTY5;
  vu16  A2R;			/*IFn Message Arbitration 2 Register*/
  u16   EMPTY6;
  vu16  MCR;			/*IFn Message Control Register*/
  u16   EMPTY7;
  vu16  DA1R;		        /*IFn DATA A 1 Register*/
  u16   EMPTY8;
  vu16  DA2R;		        /*IFn DATA A 2 Register*/
  u16   EMPTY9;
  vu16  DB1R;		        /*IFn DATA B 1 Register*/
  u16   EMPTY10;
  vu16  DB2R;		        /*IFn DATA B 2 Register*/
  u16   EMPTY11[27];
} CAN_MsgObj_TypeDef;


typedef  struct
{
  vu16  CR;		/*Control Register*/
  u16   EMPTY1;
  vu16  SR;	        /*Status Register*/
  u16   EMPTY2;
  vuc16 ERR;		/*Error counter Register*/
  u16   EMPTY3;
  vu16  BTR;		/*Bit Timing Register*/
  u16   EMPTY4;
  vuc16 IDR;		/*Interrupt Identifier Register*/
  u16   EMPTY5;
  vu16  TESTR;		/*Test Register*/
  u16   EMPTY6;
  vu16  BRPR;		/*BRP Extension Register*/
  u16   EMPTY7[3];
  CAN_MsgObj_TypeDef sMsgObj[2];
  u16   EMPTY8[16];
  vuc16 TXR1R;		/*Transmission request 1 Register*/
  u16   EMPTY9;
  vuc16 TXR2R;		/*Transmission Request 2 Register*/
  u16   EMPTY10[13];
  vuc16 ND1R;		/*New Data 1 Register*/
  u16   EMPTY11;
  vuc16 ND2R;		/*New Data 2 Register*/
  u16   EMPTY12[13];
  vuc16 IP1R;		/*Interrupt Pending 1 Register*/
  u16   EMPTY13;
  vuc16 IP2R;		/*Interrupt Pending 2 Register*/
  u16   EMPTY14[13];
  vuc16 MV1R;		/*Message Valid 1 Register*/
  u16   EMPTY15;
  vuc16 MV2R;		/*Message VAlid 2 Register*/
  u16   EMPTY16;
} CAN_TypeDef;


/******************************************************************************
*                   Peripherals' Base addresses
******************************************************************************/

#define APB                 0xFFFF8000     
#define FLASHR_BASE         0x80100000
#define FLASHPR_BASE        0x8010DFB0     
#define CFG_BASE            0x40000000
#define PRCCU_BASE          0x60000000
#define ARB_BASE            0x20000000

#define APB_BASE        APB
#define I2C0_BASE       (APB + 0x400)
#define I2C1_BASE       (APB + 0x800)
#define TB0_BASE        (APB + 0x1800)
#define TB1_BASE        (APB + 0x1900)
#define TB2_BASE        (APB + 0x1A00)
#define UART0_BASE      (APB + 0x1C00)
#define UART1_BASE      (APB + 0x2000)
#define UART2_BASE      (APB + 0x1E00)
#define UART3_BASE      (APB + 0x2200)
#define WDG_BASE        (APB + 0x2400)
#define WUT_BASE        (APB + 0x2600)
#define TIM0_BASE       (APB + 0x2800)
#define TIM1_BASE       (APB + 0x2C00)
#define TIM5_BASE       (APB + 0x3000)
#define TIM6_BASE       (APB + 0x3080)
#define TIM7_BASE       (APB + 0x3100)
#define TIM8_BASE       (APB + 0x3180)
#define TIM9_BASE       (APB + 0x3200)
#define WIU_BASE        (APB + 0x3800)
#define CAN0_BASE       (APB + 0x4400)
#define CAN1_BASE       (APB + 0x4800)
#define CAN2_BASE       (APB + 0x4C00)
#define PWM0_BASE       (APB + 0x5000)
#define PWM1_BASE       (APB + 0x5040)
#define PWM2_BASE       (APB + 0x5080)
#define PWM3_BASE       (APB + 0x50C0)
#define PWM4_BASE       (APB + 0x5100)
#define PWM5_BASE       (APB + 0x5140)
#define GPIO0_BASE      (APB + 0x5400)
#define GPIO1_BASE      (APB + 0x5410)
#define GPIO2_BASE      (APB + 0x5420)
#define GPIO3_BASE      (APB + 0x5430)
#define GPIO4_BASE      (APB + 0x5440)
#define GPIO5_BASE      (APB + 0x5450)
#define GPIO6_BASE      (APB + 0x5460)
#define BSPI0_BASE      (APB + 0x5800)
#define BSPI1_BASE      (APB + 0x5C00)
#define BSPI2_BASE      (APB + 0x6000)
#define TIM2_BASE       (APB + 0x6400)
#define TIM3_BASE       (APB + 0x6800)
#define TIM4_BASE       (APB + 0x6C00)
#define DMA0_BASE       (APB + 0x7000)
#define DMA1_BASE       (APB + 0x7100)
#define DMA2_BASE       (APB + 0x7200)
#define DMA3_BASE       (APB + 0x7300)
#define RTC_BASE        (APB + 0x7400)
#define CMU_BASE        (APB + 0x7600)
#define ADC_BASE        (APB + 0x7800)
#define EIC_BASE        (APB + 0x7C00)



/*****************************************************************************
                          IPs' declaration
*****************************************************************************/

/*----------------- Non Debug Mode -----------------------------------------*/

#ifndef DEBUG
/******************** Peripherals ******************************/
#define FLASHR        ((FLASHR_TypeDef  *)  FLASHR_BASE)
#define FLASHPR       ((FLASHPR_TypeDef *)  FLASHPR_BASE)
#define PRCCU         ((PRCCU_TypeDef   *)  PRCCU_BASE)
#define ARB           ((ARB_TypeDef     *)  ARB_BASE)
#define CFG           ((CFG_TypeDef     *)  CFG_BASE)
#define I2C0          ((I2C_TypeDef     *)  I2C0_BASE)
#define I2C1          ((I2C_TypeDef     *)  I2C1_BASE)
#define TB0           ((TB_TypeDef      *)  TB0_BASE)
#define TB1           ((TB_TypeDef      *)  TB1_BASE)
#define TB2           ((TB_TypeDef      *)  TB2_BASE)
#define UART0         ((UART_TypeDef    *)  UART0_BASE)
#define UART1         ((UART_TypeDef    *)  UART1_BASE)
#define UART2         ((UART_TypeDef    *)  UART2_BASE)
#define UART3         ((UART_TypeDef    *)  UART3_BASE)
#define WDG           ((WDG_TypeDef     *)  WDG_BASE)
#define WUT           ((WUT_TypeDef     *)  WUT_BASE)
#define TIM0          ((TIM_TypeDef     *)  TIM0_BASE)
#define TIM1          ((TIM_TypeDef     *)  TIM1_BASE)
#define TIM5          ((TIM_TypeDef     *)  TIM5_BASE)
#define TIM6          ((TIM_TypeDef     *)  TIM6_BASE)
#define TIM7          ((TIM_TypeDef     *)  TIM7_BASE)
#define TIM8          ((TIM_TypeDef     *)  TIM8_BASE)
#define TIM9          ((TIM_TypeDef     *)  TIM9_BASE)
#define WIU           ((WIU_TypeDef     *)  WIU_BASE)
#define CAN0          ((CAN_TypeDef     *)  CAN0_BASE)
#define CAN1          ((CAN_TypeDef     *)  CAN1_BASE)
#define CAN2          ((CAN_TypeDef     *)  CAN2_BASE)
#define PWM0          ((PWM_TypeDef     *)  PWM0_BASE)
#define PWM1          ((PWM_TypeDef     *)  PWM1_BASE)
#define PWM2          ((PWM_TypeDef     *)  PWM2_BASE)
#define PWM3          ((PWM_TypeDef     *)  PWM3_BASE)
#define PWM4          ((PWM_TypeDef     *)  PWM4_BASE)
#define PWM5          ((PWM_TypeDef     *)  PWM5_BASE)
#define GPIO0         ((GPIO_TypeDef    *)  GPIO0_BASE)
#define GPIO1         ((GPIO_TypeDef    *)  GPIO1_BASE)
#define GPIO2         ((GPIO_TypeDef    *)  GPIO2_BASE)
#define GPIO3         ((GPIO_TypeDef    *)  GPIO3_BASE)
#define GPIO4         ((GPIO_TypeDef    *)  GPIO4_BASE)
#define GPIO5         ((GPIO_TypeDef    *)  GPIO5_BASE)
#define GPIO6         ((GPIO_TypeDef    *)  GPIO6_BASE)
#define BSPI0         ((BSPI_TypeDef    *)  BSPI0_BASE)
#define BSPI1         ((BSPI_TypeDef    *)  BSPI1_BASE)
#define BSPI2         ((BSPI_TypeDef    *)  BSPI2_BASE)
#define TIM2          ((TIM_TypeDef     *)  TIM2_BASE)
#define TIM3          ((TIM_TypeDef     *)  TIM3_BASE)
#define TIM4          ((TIM_TypeDef     *)  TIM4_BASE)
#define DMA0          ((DMA_TypeDef     *)  DMA0_BASE)
#define DMA1          ((DMA_TypeDef     *)  DMA1_BASE)
#define DMA2          ((DMA_TypeDef     *)  DMA2_BASE)
#define DMA3          ((DMA_TypeDef     *)  DMA3_BASE)
#define RTC           ((RTC_TypeDef     *)  RTC_BASE)
#define CMU           ((CMU_TypeDef     *)  CMU_BASE)
#define ADC           ((ADC_TypeDef     *)  ADC_BASE)
#define EIC           ((EIC_TypeDef     *)  EIC_BASE)
#else   /* DEBUG */

/*------------------------ Debug Mode ----------------------------------------*/

EXT FLASHR_TypeDef         *FLASHR;
EXT FLASHPR_TypeDef        *FLASHPR;
EXT PRCCU_TypeDef          *PRCCU;
EXT ARB_TypeDef            *ARB;
EXT CFG_TypeDef            *CFG;
EXT I2C_TypeDef            *I2C0;
EXT I2C_TypeDef            *I2C1;
EXT TB_TypeDef             *TB0;
EXT TB_TypeDef             *TB1;
EXT TB_TypeDef             *TB2;
EXT UART_TypeDef           *UART0;
EXT UART_TypeDef           *UART1;
EXT UART_TypeDef           *UART2;
EXT UART_TypeDef           *UART3;
EXT WDG_TypeDef            *WDG;
EXT WUT_TypeDef            *WUT;
EXT TIM_TypeDef            *TIM0;
EXT TIM_TypeDef            *TIM1;
EXT TIM_TypeDef            *TIM5;
EXT TIM_TypeDef            *TIM6;
EXT TIM_TypeDef            *TIM7;
EXT TIM_TypeDef            *TIM8;
EXT TIM_TypeDef            *TIM9;
EXT WIU_TypeDef            *WIU;
EXT CAN_TypeDef            *CAN0;
EXT CAN_TypeDef            *CAN1;
EXT CAN_TypeDef            *CAN2;
EXT PWM_TypeDef            *PWM0;
EXT PWM_TypeDef            *PWM1;
EXT PWM_TypeDef            *PWM2;
EXT PWM_TypeDef            *PWM3;
EXT PWM_TypeDef            *PWM4;
EXT PWM_TypeDef            *PWM5;
EXT GPIO_TypeDef           *GPIO0;
EXT GPIO_TypeDef           *GPIO1;
EXT GPIO_TypeDef           *GPIO2;
EXT GPIO_TypeDef           *GPIO3;
EXT GPIO_TypeDef           *GPIO4;
EXT GPIO_TypeDef           *GPIO5;
EXT GPIO_TypeDef           *GPIO6;
EXT BSPI_TypeDef           *BSPI0;
EXT BSPI_TypeDef           *BSPI1;
EXT BSPI_TypeDef           *BSPI2;
EXT TIM_TypeDef            *TIM2;
EXT TIM_TypeDef            *TIM3;
EXT TIM_TypeDef            *TIM4;
EXT DMA_TypeDef            *DMA0;
EXT DMA_TypeDef            *DMA1;
EXT DMA_TypeDef            *DMA2;
EXT DMA_TypeDef            *DMA3;
EXT RTC_TypeDef            *RTC;
EXT CMU_TypeDef            *CMU;
EXT ADC_TypeDef            *ADC;
EXT EIC_TypeDef            *EIC;

#endif  /* DEBUG */

#endif  /* __73x_MAP_H*/

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
