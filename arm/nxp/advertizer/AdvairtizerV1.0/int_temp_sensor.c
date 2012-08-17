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
#include <global.h>

const U16 TEMP_Celcius_pos[] =    // Positive Celcius temperatures (ADC-value)
        {                        // from 0 to 60 degrees
            1472,1470,1468,1466,1464,1462,1460,1458,1456,1454,1452,1450,1448,1446,1444,
            1442,1440,1438,1436,1434,1432,1430,1428,1426,1424,1422,1420,1418,1416,1414,
            452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
            297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
            187 };

/************************************************************
* Internal Temperatur Sensor (ADC 0.1)
************************************************************/
U8 init_int_temp_sensor(void){
    U16 adcval = ReadChannel0(int_temp_sensor); // Temperatur Sensor
	adcval = 0;
return 1;
}

U16 get_int_temp_sensor(void){
    U16 adcval = ReadChannel0(int_temp_sensor); // Temperatur Sensor
U8 i;
// Find the temperature
for (i=0; i<=141; i++){
    if (adcval > TEMP_Celcius_pos[i]){
       break;
     }
    }      

return i;
//return adcval;
}
