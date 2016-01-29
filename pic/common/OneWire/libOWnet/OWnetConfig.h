/*********************************************************************
** Title:		Configuration file for 1-Wire Net library for fedC/wizC
**
** Author:		Marcel van Lieshout
**
** Copyright:	(c) 2006, HMCS, Marcel van Lieshout
**				Parts of this software are
**			 	Copyright (C) 2000 Dallas Semiconductor Corporation.
**
** License:		This software is released to the public domain and comes
**				without	warranty and/or guarantees of any kind. You have
**				the right to use, copy, modify and/or (re-)distribute the
**				software as long as the reference to the author and all
**				other copyright holders is maintained in the software
**				(in it's original or in a modified form) is used.
*********************************************************************/
#ifndef OWNET_CONFIG_H
#define OWNET_CONFIG_H
/*-----------------------------------------------------------
 ** Application specific definitions.
 */

/*
** The CRC calculation routines come in two flavours: One uses a lookup table, the other
** calculates the CRC. The choice is yours: the lookuptable version is fast but uses more
** ROM, the calculator takes less ROM space but takes more cycles to complete.
*/
#define OWCRC_USELOOKUP           // Comment this line to use the alternative algorithm

/*
** Define the OneWire Network connections. Up to 8 networks can be defined.
** When only a single net is defined (OWNET0), the SingleBus library is used, otherwise
** the MultiBus library is linked with your application.
**
** When a connection is not needed/present, just comment the line(s)
*/
//#define OWNET0_SENSE_PORT        B
//#define OWNET0_SENSE_PIN         0
//#define OWNET0_DRIVE_PORT        B
//#define OWNET0_DRIVE_PIN         0
//#define OWNET0_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET0_DPU_PORT          B
//#define OWNET0_DPU_PIN           0
//#define OWNET0_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET0_SPU_PORT          B
//#define OWNET0_SPU_PIN           1
//#define OWNET0_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET0_PPU_PORT          B
//#define OWNET0_PPU_PIN           0
//#define OWNET0_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET1_SENSE_PORT        B
//#define OWNET1_SENSE_PIN         2
//#define OWNET1_DRIVE_PORT        B
//#define OWNET1_DRIVE_PIN         3
//#define OWNET1_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET1_DPU_PORT          B
//#define OWNET1_DPU_PIN           4
//#define OWNET1_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET1_SPU_PORT          B
//#define OWNET1_SPU_PIN           5
//#define OWNET1_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET1_PPU_PORT          B
//#define OWNET1_PPU_PIN           6
//#define OWNET1_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET2_SENSE_PORT        B
//#define OWNET2_SENSE_PIN         2
//#define OWNET2_DRIVE_PORT        B
//#define OWNET2_DRIVE_PIN         3
//#define OWNET2_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET2_DPU_PORT          B
//#define OWNET2_DPU_PIN           4
//#define OWNET2_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET2_SPU_PORT          B
//#define OWNET2_SPU_PIN           5
//#define OWNET2_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET2_PPU_PORT          B
//#define OWNET2_PPU_PIN           6
//#define OWNET2_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET3_SENSE_PORT        B
//#define OWNET3_SENSE_PIN         2
//#define OWNET3_DRIVE_PORT        B
//#define OWNET3_DRIVE_PIN         3
//#define OWNET3_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET3_DPU_PORT          B
//#define OWNET3_DPU_PIN           4
//#define OWNET3_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET3_SPU_PORT          B
//#define OWNET3_SPU_PIN           5
//#define OWNET3_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET3_PPU_PORT          B
//#define OWNET3_PPU_PIN           6
//#define OWNET3_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET4_SENSE_PORT        B
//#define OWNET4_SENSE_PIN         2
//#define OWNET4_DRIVE_PORT        B
//#define OWNET4_DRIVE_PIN         3
//#define OWNET4_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET4_DPU_PORT          B
//#define OWNET4_DPU_PIN           4
//#define OWNET4_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET4_SPU_PORT          B
//#define OWNET4_SPU_PIN           5
//#define OWNET4_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET4_PPU_PORT          B
//#define OWNET4_PPU_PIN           6
//#define OWNET4_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET5_SENSE_PORT        B
//#define OWNET5_SENSE_PIN         2
//#define OWNET5_DRIVE_PORT        B
//#define OWNET5_DRIVE_PIN         3
//#define OWNET5_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET5_DPU_PORT          B
//#define OWNET5_DPU_PIN           4
//#define OWNET5_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET5_SPU_PORT          B
//#define OWNET5_SPU_PIN           5
//#define OWNET5_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET5_PPU_PORT          B
//#define OWNET5_PPU_PIN           6
//#define OWNET5_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET6_SENSE_PORT        B
//#define OWNET6_SENSE_PIN         2
//#define OWNET6_DRIVE_PORT        B
//#define OWNET6_DRIVE_PIN         3
//#define OWNET6_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET6_DPU_PORT          B
//#define OWNET6_DPU_PIN           4
//#define OWNET6_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET6_SPU_PORT          B
//#define OWNET6_SPU_PIN           5
//#define OWNET6_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET6_PPU_PORT          B
//#define OWNET6_PPU_PIN           6
//#define OWNET6_PPU_POLARITY      OWLEVEL_IDLE_LOW

//#define OWNET7_SENSE_PORT        B
//#define OWNET7_SENSE_PIN         2
//#define OWNET7_DRIVE_PORT        B
//#define OWNET7_DRIVE_PIN         3
//#define OWNET7_DRIVE_POLARITY    OWLEVEL_IDLE_LOW
//#define OWNET7_DPU_PORT          B
//#define OWNET7_DPU_PIN           4
//#define OWNET7_DPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET7_SPU_PORT          B
//#define OWNET7_SPU_PIN           5
//#define OWNET7_SPU_POLARITY      OWLEVEL_IDLE_LOW
//#define OWNET7_PPU_PORT          B
//#define OWNET7_PPU_PIN           6
//#define OWNET7_PPU_POLARITY      OWLEVEL_IDLE_LOW

#endif /* OWNET_CONFIG_H */
