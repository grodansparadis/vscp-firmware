/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : ldr_sensor.h                                           *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

/***************************************************************************
 * port define
 ***************************************************************************/
#ifndef LDR_SENSOR_H
#define LDR_SENSOR_H
#define ldr_sensor	 2// Ilumination Sensor ADC Channel 0.2
/******************
* functions
******************/
U8 init_ldr_sensor(void);
U16 get_ldr_sensor(void);

#endif

