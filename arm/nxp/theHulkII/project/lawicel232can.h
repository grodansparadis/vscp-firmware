///////////////////////////////////////////////////////////////////////////////
//
// File: lawicel232can.h
//
// Lawicel CAN232 handler
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: lawicel232can.h,v $
// $Revision: 1.3 $
//

#include "types.h"

boolean SendCANFrame( uint32_t id, uint8_t size, uint8_t *pData );
boolean getCANFrame( uint32_t *pid, uint8_t *pze, uint8_t *pData );
