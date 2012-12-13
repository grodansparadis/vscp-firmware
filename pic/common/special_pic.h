//
// special_pic.h - Copyright (C) 2012, Stefan Langer, dev@datenheim.de
//
// this file is part of vscp - very simple control protocol with
// Copyright (C) 2002-2004 Ake Hedman, eurosource, akhe@eurosource.se
//
// This software is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
// General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#ifndef VSCP_SPECIAL_PIC_H_
#define VSCP_SPECIAL_PIC_H_
void platform_reset(void);
void platform_wait_ms(double tins);
#endif
