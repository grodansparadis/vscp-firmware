/*
  Copyright (C) 2001 Jesper Hansen <jesperh@telia.com>.

  Rewritten by:	Nikolai Vorontsov <nickviz@mail.be>

  Rewritten by  Romuald Bialy (aka MIS) <romek_b@o2.pl>

  This file is part of the yampp system.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation, 
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __REC80_H__
#define __REC80_H__

#include "Constants.h"
#include "types.h"

#ifdef YAMPP3USB
 #define IR_PORT	PORTE
 #define IR_BIT		PE0
#else
 #define IR_PORT	PORTD
 #define IR_BIT		PD2
#endif

#ifdef ENABLE_IR
 bool rec80_active(void);
 u16 get_rec80(void);
 void remote_setup(void);
#else
 #define rec80_active() 0
 #define get_rec80() 0
 #define remote_setup()
#endif

#include "remote.def"

#ifndef REM_STD
 #error Missing Remote definition file "remote.def"!
#else
 #if (REM_STD < 1) || (REM_STD > 5)
  #error Invalid Remote Standard!
 #endif
#endif

#endif // __REC80_H__
