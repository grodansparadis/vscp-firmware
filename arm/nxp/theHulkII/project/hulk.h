//
// File: hulk.h
//
// This is the HULK definitions
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: hulk.h,v $
// $Revision: 1.11 $
//


#define FIRMWARE_MAJOR_VERSION 			0
#define FIRMWARE_MINOR_VERSION			0
#define FIRMWARE_SUB_MINOR_VERSION		5
#define FIRMWARE_TEXT_VERSION			"0.0.5"


// VSCP Switches
#define GUID_IN_EEPROM
#define MANUFACTURER_INFO_IN_EEPROM

#define DEFAULT_STATES0								0x80	// Furnace inactive
#define DEFAULT_STATES1								0x00	// Water heater inactive
#define DEFAULT_ZONE								1		
#define DEFAULT_TRANSPORTB_ALARM_POINT				50		// 50 C
#define DEFAULT_FURNACE_PRESET_TEMP					80		// 80 C
#define DEFAULT_WATER_HEATER_PRESET_TEMP			70		// 70 C
#define DEFAULT_WATER_HEATER_CONTROL				0x03	// Both elements active
#define DEFAULT_WATER_HEATER_STAGE2_DELAY_MSB		0x1C	// Two hours
#define DEFAULT_WATER_HEATER_STAGE2_DELAY_LSB		0x20
#define DEFAULT_FAN_POST_RUN_TIME					5		// Five minutes
#define DEFAULT_TRANSPORTB_POST_RUN_TIME			20		// 20 seconds
#define DEFAULT_TRANSPORT_FUEL_FILL_TIME			30		// 60 seconds
#define DEFAULT_FURNACE_RESTART_ATTEMPTS			3		// 3 attempts
#define DEFAULT_WATER_HEATER_HYSTERERIS				5		// five degrees C
#define DEFAULT_FURNACE_HYSTERERIS					5		// Five degrees C
#define DEFAULT_FIRE_DETECTOR_HYSTERERSIS_MSB		0x00	
#define DEFAULT_FIRE_DETECTOR_HYSTERERSIS_LSB		0x14
#define DEFAULT_FURNACE_RESTART_TIMEOUT				0x03	// 3 minutes

// Time that fire should be detected to go to ACTIVE
#define FIRE_DETECT_TIMEOUT							5		// Seconds

// Delay after non fire detect
#define LIGHTS_OUT_TIMEOUT							10		// Seconds

// Temp on transport B that generates ALARM
#define TRANSPORTB_ALARM_TEMP						60		// Degrees centrigrade

// Subzones
enum vscp_device_subzone {
	subzone_general = 0,			// General for device
	subzone_furnace,				// General for furnace
	subzone_heater,					// General for water heater
	subzone_reserved0,
	subzone_reserved1,
	subzone_reserved2,
	subzone_furnace_temp_out,		// Temp sensor furnace water out.
	subzone_furnace_temp_return,	// Temp sensor furnace water return.
	subzone_furnace_temp_transport,	// Temp sensor transport B.
	subzone_heater_temp_core,		// Temp sensor water heater core.
	subzone_heater_temp_hotwater,	// Temp sensor hot water.
	subzone_reserved3,
	subzone_reserved4,
	subzone_reserved5,
	subzone_reserved6,
	subzone_reserved7,
	subzone_reserved8,
	subzone_transport_A,			// Transport A relay
	subzone_transport_B,			// Transport B relay
	subzone_furnace_fan,			// Furnace FAN
	subzone_furnace_heater,			// Furnace heater
	subzone_reserved9,
	subzone_reserved10,
	subzone_reserved11,
	subzone_reserved12,
	subzone_heater_ElementA,		// Water heater elelemt A
	subzone_heater_ElementB,		// Water heater element B
};

// Measurement Indexes - used for measurement events
enum vscp_measurement_index {
	index_furnace_temp_out = 0,		// Temp sensor furnace water out.
	index_furnace_temp_return,		// Temp sensor furnace water return.
	index_furnace_temp_transport,	// Temp sensor transport B.
	index_heater_temp_core,			// Temp sensor water heater core.
	index_heater_temp_hotwater		// Temp sensor hot water.
};

// * * * States * * *

// Furnace
enum furnace_states {
	FURNACE_STATE_FAILURE = 0,
	FURNACE_STATE_OFF,
	FURNACE_STATE_PREREST,
	FURNACE_STATE_REST,
	FURNACE_STATE_PRESTART,
	FURNACE_STATE_FIREDETECT,
	FURNACE_STATE_START,
	FURNACE_STATE_PREWAIT,
	FURNACE_STATE_WAIT,
	FURNACE_STATE_ACTIVE,
	FURNACE_STATE_LIGHTS_OUT
};

// Water heater
enum heater_states {
	HEATER_STATE_OFF = 0,
	HEATER_STATE_REST,
	HEATER_STATE_ON_LEVEL1,
	HEATER_STATE_ON_LEVEL2
};


// Display
enum display_states {
	DISPLAY_STATE_STARTUP = 0,
	DISPLAY_STATE_HULK,
	DISPLAY_STATE_HEATER,
	DISPLAY_STATE_STATUS1,
	DISPLAY_STATE_STATUS2,
	DISPLAY_STATE_STATUS3,
	DISPLAY_STATE_PRECONFIGURE,
	DISPLAY_STATE_CONFIGURE,
	DISPLAY_STATE_ALARM
};

// Configure
enum configure_states {
	CONFIG_STATE_PRESET_HEATER_TEMP = 0,	// Water heater preset temp
	CONFIG_STATE_PRESET_FURNACE_TEMP,		// Furnace preset temp
	CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME,	// Start fuel fill time
	CONFIG_STATE_FUEL_TRANSPORT_POST_TIME,	// Transport B run time after temp reached
	CONFIG_STATE_FAN_END_TIME,
	CONFIG_STATE_RESTART_TIMEOUT,
	CONFIG_STATE_RESTART_ATTEMPTS,
	CONFIG_STATE_FURNACE_ENABLE,
	CONFIG_STATE_HEATER_ENABLE,
	CONFIG_STATE_ALARM_ENABLE,
	CONFIG_STATE_LEAVE
};

// * * * Registers * * *
enum hulk_registers {
	REGISTER_ZONE = 0,		
	REGISTER_STATES0,
	REGISTER_STATES1,
	REGISTER_FIRE_DETECTOR_MSB,
	REGISTER_FIRE_DETECTOR_LSB,
	REGISTER_FIRE_DETECTOR_REFERENCE_MSB,
	REGISTER_FIRE_DETECTOR_REFERENCE_LSB,
	REGISTER_FURNACE_WATER_OUT_TEMP,
	REGISTER_FURNACE_WATER_RETURN_TEMP,
	REGISTER_TRANSPORTB_TEMP,
	REGISTER_TRANSPORTB_ALARM_POINT,
	REGISTER_FURNACE_PRESET_TEMP,
	REGISTER_WATER_HEATER_PRESET_TEMP,
	REGISTER_WATER_HEATER_CORE_TEMP,
	REGISTER_SMOKE_GAS_TEMP_MSB,
	REGISTER_SMOKE_GAS_TEMP_LSB,
	REGISTER_CO_MEASUREMENT_MSB,
	REGISTER_CO_MEASUREMENT_LSB,
	REGISTER_WATER_HEATER_CONTROL,
	REGISTER_WATER_HEATER_STAGE2_DELAY_MSB,
	REGISTER_WATER_HEATER_STAGE2_DELAY_LSB,
	REGISTER_FAN_POST_RUN_TIME,				// Time to run fan after preset temp reached
	REGISTER_TRANSPORTB_POST_RUN_TIME,		// Time to run transport B after preset temp reached
	REGISTER_TRANSPORT_FUEL_FILL_TIME,		// Time to run trabpsorts to fill fule
	REGISTER_FURNACE_RESTART_ATTEMPTS,		// Attempts to start
	REGISTER_WATER_HEATER_HYSTERERIS,
	REGISTER_FURNACE_HYSTERERIS,
	REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB,
	REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB,
	REGISTER_WATER_HEATER_WATER_TEMP,		// Warm water temperature
	REGISTER_FURNACE_RESTART_TIMEOUT,		// Timeout before restart
	REGISTER_RESERVED0,
	REGISTER_FURNACE_TEMP_MSB,
	REGISTER_FURNACE_TEMP_LSB
};


// ID's for buttons
 
#define 	BUTTON_STATE0			0
#define 	BUTTON_FORWARD			1
#define 	BUTTON_UP				2
#define 	BUTTON_DOWN				3
#define 	BUTTON_ENTER			4
#define 	BUTTON_FORCE_MOTOR_ON	5


// ID's for temperature sensors
enum temp_sensors {
	TEMPSENSOR_FURNACE_OUT = 0,
	TEMPSENSOR_FURNACE_RETURN,
	TEMPSENSOR_HOTWATER,
	TEMPSENSOR_TRANSPORTB,
};



extern char gdisplaybuf1[ 17 ];
extern char gdisplaybuf2[ 17 ];
