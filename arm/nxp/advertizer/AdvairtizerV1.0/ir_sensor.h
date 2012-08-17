/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Andreas Weschenfelder 2007                                   *
 *    File name   : ir_sensor.h                                            *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Andreas Weschenfelder, Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef INT_TEMP_SENSOR_H
#define INT_TEMP_SENSOR_H

void init_ir_sensor(void);
static void eint3ISR(void);
extern void rc5_init (U8 addr);

#define RC5_ALL 0xff
#define RC5_TIME 1778

volatile int rc5_tmp;                                   // RC5 temp Data
volatile int rc5_tmp_time;
static U8 rc5_addr;

  U32 duration;
  U32 startTime; 

typedef struct RC5
{
  U8 toggle;
  U8 code;
  U8 addr;
} rc5_t;
rc5_t rc5;

#endif

