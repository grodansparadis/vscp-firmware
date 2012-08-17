#include <global.h>

/************************************************************
* Temperatur Sensor (ADC 1.2)
************************************************************/
U8 init_ext_temp_sensor(void){
    U16 adcval = ReadChannel1(ext_temp_sensor); // Temperatur Sensor
	adcval = 0;
return 1;
}

U16 get_ext_temp_sensor(void){
    U16 adcval = ReadChannel1(ext_temp_sensor); // Temperatur Sensor
return adcval;
}
