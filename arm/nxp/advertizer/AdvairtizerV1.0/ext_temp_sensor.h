#ifndef EXT_TEMP_SENSOR_H
#define EXT_TEMP_SENSOR_H

/***************************************************************************
 * port define
 ***************************************************************************/
#define ext_temp_sensor 2// Temperatur Sensor Bit 2 (AD1.2)

/******************
* functions
******************/
U8 init_ext_temp_sensor(void);
U16 get_ext_temp_sensor(void);

#endif
