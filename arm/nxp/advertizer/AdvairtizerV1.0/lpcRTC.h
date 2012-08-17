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
#ifndef INC_LPC_RTC_H
#define INC_LPC_RTC_H
#include "typedefs.h"

//Interrupt Location Register (ILR)
#define RTCCIF BIT0 //When one, the Counter Increment Interrupt block generated an interrupt.
                    //Writing a one to this bit location clears the counter increment interrupt.
#define RTCALF BIT1 //When one, the alarm registers generated an interrupt. Writing a one to
                    //this bit location clears the alarm interrupt.
//Clock Control Register (CCR)
#define CLKEN  BIT0 //Clock enable
#define CTCRST BIT1 //When one, the elements in the Clock Tick Counter are
                    //reset. The elements remain reset until CTCRST is changed to zero.
#define CLKSRC BIT4 //If this bit is 1, the CTC takes its clock from the 32 kHz oscillator

//Counter Increment Interrupt Register (CIRR)
#define IMSEC  BIT0   //When 1, an increment of the Second value generates an interrupt.
#define IMMIN  BIT1   //When 1, an increment of the Minute value generates an interrupt. NA
#define IMHOUR BIT2   //When 1, an increment of the Hour value generates an interrupt. NA
#define IMDOM  BIT3   //When 1, an increment of the Day of Month value generates an interrupt. NA
#define IMDOW  BIT4   //When 1, an increment of the Day of Week value generates an interrupt. NA
#define IMDOY  BIT5   //When 1, an increment of the Day of Year value generates an interrupt. NA
#define IMMON  BIT6   //When 1, an increment of the Month value generates an interrupt. NA
#define IMYEAR BIT7   //When 1, an increment of the Year value generates an interrupt. NA

 typedef struct tTimestamp
{
  REG_8 ilr;                            // Interrupt Location Register
  REG_8 _pad0[3];
  REG16 ctc;                            // Clock Tick Counter
  REG16 _pad1;
  REG_8 ccr;                            // Clock Control Register
  REG_8 _pad2[3];
  REG_8 ciir;                           // Counter Increment Interrupt Register
  REG_8 _pad3[3];
  REG_8 amr;                            // Alarm Mask Register
  REG_8 _pad4[3];
  REG32 ctime0;                         // Consolidated Time Register 0
  REG32 ctime1;                         // Consolidated Time Register 1
  REG32 ctime2;                         // Consolidated Time Register 2
  REG_8 sec;                            // Seconds Register
  REG_8 _pad5[3];
  REG_8 min;                            // Minutes Register
  REG_8 _pad6[3];
  REG_8 hour;                           // Hours Register
  REG_8 _pad7[3];
  REG_8 dom;                            // Day Of Month Register
  REG_8 _pad8[3];
  REG_8 dow;                            // Day Of Week Register
  REG_8 _pad9[3];
  REG16 doy;                            // Day Of Year Register
  REG16 _pad10;
  REG_8 month;                          // Months Register
  REG_8 _pad11[3];
  REG16 year;                           // Years Register
  REG32 _pad12[8];
  REG_8 alsec;                          // Alarm Seconds Register
  REG_8 _pad13[3];
  REG_8 almin;                          // Alarm Minutes Register
  REG_8 _pad14[3];
  REG_8 alhour;                         // Alarm Hours Register
  REG_8 _pad15[3];
  REG_8 aldom;                          // Alarm Day Of Month Register
  REG_8 _pad16[3];
  REG_8 aldow;                          // Alarm Day Of Week Register
  REG_8 _pad17[3];
  REG16 aldoy;                          // Alarm Day Of Year Register
  REG16 _pad18;
  REG_8 almon;                          // Alarm Months Register
  REG_8 _pad19[3];
  REG16 alyear;                         // Alarm Years Register
  REG16 _pad20;
  REG16 preint;                         // Prescale Value Register (integer)
  REG16 _pad21;
  REG16 prefrac;                        // Prescale Value Register (fraction)
  REG16 _pad22;
} rtcRegs_t;

#endif
