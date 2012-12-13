//
// special_arm.c - Copyright (C) 2012, Stefan Langer, dev@datenheim.de
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

// #include whatever is needed

// keep in mind that special_???.c / special_???.h must work
// for the whole family thus use manufacturers standard libs
// wherever possible
// if further split is required for different brands seperate it from 
// this file to the underlying nxp/common and str/common for instance

void platform_reset(void)
{
// do the hardest reset possible with CPU built in measures
}

void platform_wait_ms(double tins)
{
// do a blocking wait for specified number of milliseconds
// it must work for any clock frequency specified by typical means
// e.g. use manufacturer provided constants, macros or defines to
// calculate right number of cycles to wait.
}
