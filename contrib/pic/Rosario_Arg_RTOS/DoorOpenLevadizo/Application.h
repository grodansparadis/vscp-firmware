//******************************************************************************
// File name: Application.h                                         
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   DoorOpenBatiente v1.0                            			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Application Specific 																	  
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file                                  
//
// Some portions of code and ideas are taken from 
// VSCP (Very Simple Control Protocol) 
// http://www.vscp.org
// akhe@eurosource.se
// 
// Copyright (C) 1995-2005 Ake Hedman, eurosource
// 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// 
// This file is part of VSCP - Very Simple Control Protocol 	
// http://www.vscp.org
//******************************************************************************

#ifndef DOOROPEN_H
#define DOOROPEN_H


#define FIRMWARE_MAJOR_VERSION	0x01
#define FIRMWARE_MINOR_VERSION	0x00
#define FIRMWARE_SUB_MINOR_VERSION	0x00

// Function Prototypes
void DoorOpen_init_app( void );
void init_app_ram( void );
void init_app_eeprom( void ); 

#define ACTION_DOOR_OPEN	1
#define ACTION_ALERT_ON		2
#define ACTION_ALERT_OFF	3	

#endif
