OLIMEX can232
=============

Summary
-------

This project uses an OLIMEX AT90CAN128 Developement Board to implement
a CAN to RS232 bridge. No hardware modification is needed. The AT90CAN
Development Board connects using a straight serial cable to the
PC. Only pins 2, 3 and 5 are needed. Power may be supplied through the
CAN connector or the 2 pin power connector.

VSCPD should be configured to use can232drv.dll with the follwoing
communication parameters: 38400-8N1. The corresponding configuration
in vscpd.conf is:

	<driver>
		<name>Olimex CAN232 Bridge</name>
		<parameter>COM1;38400;</parameter>
		<path>D:\VSCP\canal\drivers\can232drv.dll</path>
		<flags>1</flags>
	</driver>


Note
----

There is no galvanic isolation between the CAN bus and the PC
connected through the RS232.


ToDo
----

* Higher RS232 baudrate.
* Implement CAN speed setting, use queue to buffer messages.
* Use CAN interrupt and put AVR to sleep.
* Status command (F).


History
-------

2009/03/13 Created by Tom


License
-------

//////////////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// canald.cpp
//
// This file is part of the VSCP/firmware (http://sourceforge.net/projects/m2m)
//
// Copyright (C) 2009 Thomas Schulz, tschulz@iprimus.com.au
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
/////////////////////////////////////////////////////////////////////////////
