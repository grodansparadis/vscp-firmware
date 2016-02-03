/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Meta include file
*******************************************************************************/

#include "vscp_compiler.h"

// avr default includes
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/power.h>
#include <avr/wdt.h>

// default common vscp includes (from vscp source tree)
#include <vscp_type.h>
#include <vscp_registers.h>
#include <vscp_class.h>
#include <vscp_firmware.h>

// project specific vscp includes from project directory
#include "vscp_projdefs.h"
#include "can_at90can128.h"

//hardware related includes
//#include "uart.h"
#include "avr_adc.h"
#include "avr_spi.h"
#include "mcp355xs.h"

#include "vscp_mandatory.h"
#include "digiprobe_gcc.h"
#include "digiprobe_system.h"
#include "digiprobe_work.h"
