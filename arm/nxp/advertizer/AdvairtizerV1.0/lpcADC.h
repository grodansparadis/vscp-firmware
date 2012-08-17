/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * Header file for Philips LPC ARM Processors.
 * Copyright 2004 R O SoftWare
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/
#ifndef INC_LPC_ADC_H
#define INC_LPC_ADC_H
#include "typedefs.h"

// A/D Control Register (AD0CR)
#define SEL0  BIT0  //Selects which of the AD0.7:0/AD1.7:0 pins is (are) to be sampled and converted. For
                    //AD0, bit 0 selects Pin AD0.0, and bit 7 selects pin AD0.7
#define SEL1  BIT1
#define SEL2  BIT2
#define SEL3  BIT3
#define SEL4  BIT4
#define SEL5  BIT5
#define SEL6  BIT6
#define SEL7  BIT7
#define BURST BIT16 //The AD converter does repeated conversions at the rate selected by the CLKS field
#define CLKS1 BIT17 //This field selects the number of clocks used for each conversion in Burst mode
#define CLKS2 BIT18
#define CLKS3 BIT19
#define PDN   BIT21 //The A/D converter is operational.

// A/D Converter Registers
/*
typedef struct
{

  //  REG32 cr;                             // Control Register
  struct
    {
    REG_8 sel;                           // Selects which of the AD0.7:0/AD1.7:0 pins is (are) to be sampled and converted. (SEL)
    REG_8 _pad0[8];
    REG_8 clkdiv;                        // The VPB clock (PCLK) is divided by (this value plus one) to produce the clock for the A/D converter
    REG_8 _pad1[8];

    } cr;

  //REG32 dr;                             // Data Register

  struct
    {
    REG_8 sel;                           // Selects which of the AD0.7:0/AD1.7:0 pins is (are) to be sampled and converted. (SEL)
    REG_8 _pad0[8];
    REG_8 clkdiv;                           // Selects which of the AD0.7:0/AD1.7:0 pins is (are) to be sampled and converted. (SEL)
    REG_8 _pad0[8];

    } dr;


} adcRegs_t;
*/

// A/D Converter Registers
typedef struct
{
  REG32 cr;                             // Control Register
  REG32 dr;                             // Data Register
} adcRegs_t;

#endif
