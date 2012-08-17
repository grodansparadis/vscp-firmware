/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : rtc.c                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>
#include <uip/uip.h>
#include <uip/uip_arp.h>
#include <uip/timer.h>

#ifdef GERMAN
char* const gWeekDays[] = {"Mo","Di","Mi","Do","Fr","Sa","So"};
#else
char* const gWeekDays[] = {"Mo","Tu","We","Th","Fr","Sa","So"};
#endif
/*
/**********************************************************************
 * rtc0 Interrupt
 * Interrupt Service Routine (ISR) is Running @ 1 Hz
***********************************************************************/
static void rtc0ISR(void){
lcd_clock(1,1,1,blue);                  // x,y,mode,color
if(F_IO1PIN & HB_LED){                  // Heartbeat led toggle
  F_IO1CLR = HB_LED;
  }else{
  F_IO1SET = HB_LED;
  }
  RTCILR |= RTCCIF;                     // clears the counter increment interrupt.
}
/***************************************************************************
 * set the interrupt rtc0
 ***************************************************************************/
void init_rtc(void){
   ctl_set_isr(SIR_RTC, PRIO_RTC, CTL_ISR_TRIGGER_FIXED, rtc0ISR, 0);
   ctl_unmask_isr(SIR_RTC);
   RTCILR = (RTCCIF | RTCALF);   // Counter Increment Interrupt block generated an interrupt,
                                  // clears the counter increment interrupt.
   RTCCCR &= ~CTCRST;             // When one, the elements in the Clock Tick Counter are reset.
   RTCCCR = (CLKSRC | CLKEN);    // 32'767Hz quartz source
   RTCCIIR = IMSEC;               // Interupt every second
}

void RTC_SetTime(U8 h, U8 m, U8 s)
{
	RTCCCR |= CTCRST ; /* Reset value in Clock Tick Counter (CTC) and disable clock */
	RTCSEC = (U32)s;
	RTCMIN = (U32)m;
	RTCHOUR = (U32)h;
        RTCCCR &= ~CTCRST;
	RTCCCR |= CLKEN; /* re-enable RTC */
}

void RTC_SetDate(U8 d, U8 mo, U16 y)
{
	RTCDOM   = (U32)d;
	RTCMONTH = (U32)mo;
	RTCYEAR  = (U32)y;
}

void RTC_SetDOW(uint8_t dow)
{
	RTCDOW = (U32)dow;
}
U8 RTC_GetSEC(void){
U8 sec = (RTCCTIME0 & 0x3f); 
        return sec;
}
U8 RTC_GetMIN(void){
U8 min = ((RTCCTIME0 >> 8) & 0x3f);
        return min;
}
U8 RTC_GetHOUR(void){
U8 hour = ((RTCCTIME0 >> 16) & 0x1F);
        return hour;
}
void RTC_GetTime(void)
{
}

