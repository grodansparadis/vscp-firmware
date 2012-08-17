/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: taskTemperature.h
 *
 *  SweetBox Module
 * 	Version information in version.c
 * 	akhe@eurosource.se
 *
 * Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

// Standard includes. 
#include <stdio.h>
#include <string.h>

// Scheduler includes. 
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "mcp9800.h"            // Temp sensor support

#include "sweetbox.h"
#include "taskTemperature.h"


/////////////////////////////////////////////////////////////////////////////////
// taskTemperature
//

void  taskTemperature( void *pvParameters )
{
    uint16_t temp;
    
    // Initialize temperature device
    mcp9800_initTempSensor( 7,
                    MCP9800_CONFIG_ONESHOT_DISABLED |
                    MCP9800_CONFIG_RESOLUTION_12BIT |
                    MCP9800_CONFIG_FAULTQ_1BIT |
                    MCP9800_CONFIG_ALERT_POL_LOW |
                    MCP9800_CONFIG_COMPARATOR_MODE |
                    MCP9800_CONFIG_SHUTDOWN_DISABLE 
                  );      
      
    // TODO: 
    //      Address is 3 instea dof 7 on real board.
    //      Write to register.        
    while ( 1 ) {       
        // Test read temperature                
        temp = mcp9800_readTempSensor( 7 ); 
        
        vTaskDelay( 5000 ); // Read five seconds  
    }
        
}

