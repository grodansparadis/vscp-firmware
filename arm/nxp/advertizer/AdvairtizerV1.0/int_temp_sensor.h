/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : int_temp_sensor.c                                      *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef INC_INT_TEMP_SENSOR_H
#define INC_INT_TEMP_SENSOR_H

/***************************************************************************
 * port define
 * max 0,1A @25C°
 * 100K @ 25C°
 * -40 to +125C°
 ***************************************************************************/
#define int_temp_sensor 1// Temperatur Sensor Bit 1 (AD0.1)

/******************
* functions
******************/
U8 init_int_temp_sensor(void);
U16 get_int_temp_sensor(void);

#endif
