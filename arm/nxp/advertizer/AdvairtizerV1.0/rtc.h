/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : rtc.h                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

void init_rtc(void);
static void rtc0ISR(void);
void RTC_SetDate(U8 d, U8 mo, U16 y);
void RTC_SetTime(U8 h, U8 m, U8 s);
void RTC_SetDOW(uint8_t dow);
U8 RTC_GetSEC(void);
U8 RTC_GetMIN(void);
U8 RTC_GetHOUR(void);
extern char* const gWeekDays[];
void RTC_GetTime(void);

