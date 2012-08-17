/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : ldr_sensor.c                                           *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>
/************************************************************
* LDR Sensor
************************************************************/
U8 init_ldr_sensor(void){
    adcval = ReadChannel0(ldr_sensor); // Ilumination Sensor
	adcval = 0;
return 1;
}

U16 get_ldr_sensor(void){
    adcval = ReadChannel0(ldr_sensor); // Ilumination Sensor
return adcval;
}

