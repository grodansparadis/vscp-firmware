/**********************************************************************/
/*                                                                    */
/* File name: device.h                                                */
/*                                                                    */
/* Since:     2004-Sept-20                                            */
/*                                                                    */
/* Version:   PICos18 v2.10                                           */
/*            Copyright (C) 2003, 2004, 2005 Pragmatec.               */
/*                                                                    */
/* Author:    Designed by Pragmatec S.A.R.L.        www.pragmatec.net */
/*            MONTAGNE Xavier [XM]      xavier.montagne@pragmatec.net */
/*            DEVINE Dan      [DD]                                    */
/*                                                                    */
/* Purpose:   Specify the PIC device header file.                     */
/*                                                                    */
/* Distribution: This file is part of PICos18.                        */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/*                                                                    */
/* History:                                                           */
/* 2003/08/03 [XM]  Original idea of DD.                              */
/* 2003/02/20 [XM]  Added of the information related to the tasks     */
/*                  or the application (event defines, ...).          */
/*                                                                    */
/**********************************************************************/

#ifndef _DEVICE_H
#define _DEVICE_H

#include "p18cxxx.h"
#include "pro_man.h"
#include "even_man.h"
#include "alarm.h"
#include "int_man.h"

#ifndef ON
#define ON                   1
#endif
#ifndef OFF
#define OFF                  0
#endif

/***********************************************************************
 * -------------------------- Buffer sizes -----------------------------
 **********************************************************************/
/* Should be equal to .tempdata + MATH_DATA size (from map file)      */
#define TEMP_SIZE         0x20


#endif /* _DEVICE_H */


/* End of File : device.h */
