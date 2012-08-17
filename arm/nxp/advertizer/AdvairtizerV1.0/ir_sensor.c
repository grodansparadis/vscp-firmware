/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Andreas Weschenfelder 2007                                   *
 *    File name   : ir_sensor.c                                            *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Andreas Weschenfelder, Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>

void rc5_init (U8 addr)
{rc5_addr = addr;}

/**********************************************
* IR_SENSOR input BIT(30) // P0.30 IR_SENSOR
***********************************************/

void init_ir_sensor(void){
    PINSEL1 |= (U32)(0x02<<28);   // Connect to eint3
    EXTMODE |= (EXTMODE3);        // EINT3 is edge sensitive.
    EXTPOLAR &= ~(EXTPOLAR3);     // EINT3 is low-active or fowling-edge sensitive
    rc5_init (RC5_ALL);
    ctl_set_isr(SIR_EINT3, PRIO_IR_SENSOR, CTL_ISR_TRIGGER_DUAL_EDGE, eint3ISR, 0);// eint2ISR registrieren
    ctl_unmask_isr(SIR_EINT3);//enable interrupt
  }

static void eint3ISR(void){
    ctl_global_interrupts_re_enable_from_isr(); // To allow interrupts of a higher priority to occur
                                                  // global interrupts are now enabled so another interrupt can be handled.
    ctl_mask_isr(SIR_EINT3);//disable interrupt
        duration = getElapsedSysTICs(startTime) / ONE_US;// duration in us
        startTime = getSysTICs();
//For Edge-Detection
  if(EXTPOLAR & (EXTPOLAR3)){//wenn 1(high active) dann low activ werden
    EXTPOLAR &= ~(EXTPOLAR3);// EINT3 is low-active or fowling-edge sensitive
  }else{
    EXTPOLAR |= (EXTPOLAR3);// EINT3 is high-active or rising-edge sensitive
  }

if (duration > RC5_TIME*1.5)       //Zu laenge kein neues Bit-> Bit muss 1. Startbit seinuart0Puts("ISR");
  {
  rc5_tmp = 1;                    //=1.Startbit
  rc5_tmp_time = 0;               //Zeitspanne nullen
  }
  else
  {
  rc5_tmp_time = rc5_tmp_time + duration;
  if ((rc5_tmp_time<1.2*RC5_TIME)&&(rc5_tmp_time>0.8*RC5_TIME))
    {
    rc5_tmp_time = 0;                   //zeit wieder nullen
    rc5_tmp = rc5_tmp << 1;             //schieben

     if (F_IO0PIN & IR_SENSOR) 
      {}
      else
      {rc5_tmp = rc5_tmp | 0x01;}
    if (rc5_tmp & 0x2000)
      {
      if (( ((rc5_tmp>>6)&0x1F) == rc5_addr) || (rc5_addr == RC5_ALL))
        {
        rc5.toggle = (rc5_tmp>>11)&0x01;
        rc5.addr = (rc5_tmp>>6)&0x1F;
        rc5.code = rc5_tmp&0x3F;
        }
      }
    }
  }
    EXTINT |= BIT3;//(1<<3); //clear interrupt
    ctl_global_interrupts_un_re_enable_from_isr();// disable interrupts on exit
    ctl_unmask_isr(SIR_EINT3);//enable interrupt
}

