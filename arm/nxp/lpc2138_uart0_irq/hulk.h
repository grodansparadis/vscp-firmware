//
// File: hulk.h
//
// This is the HULK definitions
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: hulk.h,v $
// $Revision: 1.1 $
//


#define FIRMWARE_MAJOR_VERSION 			0
#define FIRMWARE_MINOR_VERSION			0
#define FIRMWARE_SUB_MINOR_VERSION		2
#define FIRMWARE_TEXT_VERSION			"0.0.2"


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
#define DEFAULT_TRANSPORT_FUEL_FILL_TIME			60		// 60 seconds
#define DEFAULT_FURNACE_RESTART_ATTEMPTS			3		// 3 attempts
#define DEFAULT_WATER_HEATER_HYSTERERIS				5		// five degrees C
#define DEFAULT_FURNACE_HYSTERERIS					5		// Five degrees C
#define DEFAULT_FIRE_DETECTOR_HYSTERERSIS_MSB		0x00	
#define DEFAULT_FIRE_DETECTOR_HYSTERERSIS_LSB		0x14
#define DEFAULT_FURNACE_RESTART_TIMEOUT				0x03	// 3 minutes

// Time that fire should be detected to go to ACTIVE
#define FIRE_DETECT_TIMEOUT							5		// Seconds

// Delay after non fire detect
#define LIGHTS_OUT_TIMEOUT							5		// Seconds

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

#define REGISTER_ZONE							0		
#define REGISTER_STATES0						1
#define REGISTER_STATES1						2
#define REGISTER_FIRE_DETECTOR_MSB				3
#define REGISTER_FIRE_DETECTOR_LSB				4
#define REGISTER_FIRE_DETECTOR_REFERENCE_MSB	5
#define	REGISTER_FIRE_DETECTOR_REFERENCE_LSB	6
#define REGISTER_FURNACE_WATER_OUT_TEMP			7
#define REGISTER_FURNACE_WATER_RETURN_TEMP		8
#define REGISTER_TRANSPORTB_TEMP				9
#define REGISTER_TRANSPORTB_ALARM_POINT			10
#define REGISTER_FURNACE_PRESET_TEMP			11
#define REGISTER_WATER_HEATER_PRESET_TEMP		12
#define REGISTER_WATER_HEATER_CORE_TEMP			13
#define REGISTER_SMOKE_GAS_TEMP_MSB				14
#define REGISTER_SMOKE_GAS_TEMP_LSB				15
#define REGISTER_CO_MEASUREMENT_MSB				16
#define REGISTER_CO_MEASUREMENT_LSB				17
#define REGISTER_WATER_HEATER_CONTROL			18
#define REGISTER_WATER_HEATER_STAGE2_DELAY_MSB	19
#define REGISTER_WATER_HEATER_STAGE2_DELAY_LSB	20
#define REGISTER_FAN_POST_RUN_TIME				21		// Time to run fan after preset temp reached
#define REGISTER_TRANSPORTB_POST_RUN_TIME		22		// Time to run transport B after preset temp reached
#define REGISTER_TRANSPORT_FUEL_FILL_TIME		23		// Time to run trabpsorts to fill fule
#define REGISTER_FURNACE_RESTART_ATTEMPTS		24		// Attempts to start
#define REGISTER_WATER_HEATER_HYSTERERIS		25
#define REGISTER_FURNACE_HYSTERERIS				26
#define REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB	27
#define REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB	28
#define REGISTER_WATER_HEATER_WATER_TEMP		29		// Warm water temperature
#define REGISTER_FURNACE_RESTART_TIMEOUT		30		// Timeout before restart
#define REGISTER_RESERVED0						31
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP0	32
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP1	33
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP2	34
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP3	35
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP4	36
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP5	37
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP6	38
#define REGISTER_ONEWIRE_ID_FURNACE_OUT_TEMP7	39
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP0	40
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP1	41
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP2	42
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP3	43
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP4	44
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP5	45
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP6	46
#define REGISTER_ONEWIRE_ID_FURNACE_RETURN_TEMP7	47
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP0		48
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP1	49	
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP2	50
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP3	51
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP4	52
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP5	53
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP6	54
#define REGISTER_ONEWIRE_ID_TRANSPORTB_TEMP7	55
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP0		56
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP1		57
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP2		58
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP3		59
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP4		60
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP5		61
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP6		62
#define REGISTER_ONEWIRE_ID_HOTWATER_TEMP7		63
#define REGISTER_ONEWIRE_ID_RESERVED0			64
#define REGISTER_ONEWIRE_ID_RESERVED1			65
#define REGISTER_ONEWIRE_ID_RESERVED2			66
#define REGISTER_ONEWIRE_ID_RESERVED3			67
#define REGISTER_ONEWIRE_ID_RESERVED4			68
#define REGISTER_ONEWIRE_ID_RESERVED5			69
#define REGISTER_ONEWIRE_ID_RESERVED6			70
#define REGISTER_ONEWIRE_ID_RESERVED7			71
#define REGISTER_FURNACE_TEMP_MSB				72
#define REGISTER_FURNACE_TEMP_LSB				73



// ID's for buttons
 
#define 	BUTTON_STATE0			0
#define 	BUTTON_FORWARD			1
#define 	BUTTON_UP				2
#define 	BUTTON_DOWN				3
#define 	BUTTON_ENTER			4
#define 	BUTTON_FORCE_MOTOR_ON	5


extern char gdisplaybuf1[ 17 ];
extern char gdisplaybuf2[ 17 ];
