/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : STR75x.h
 *    Description : STR75x specific definitions
 *
 *    History :
 *    1. Date        : September 22, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/
#include <includes.h>

#ifndef __STR75X_H
#define __STR75X_H

#define EIC_IRQGetCmd() ((EIC->ICR&1)?ENABLE:DISABLE)

#define EIC_IRQDisCmd(state) state = (EIC->ICR&1)?ENABLE:DISABLE;EIC_IRQCmd(DISABLE)

#define EIC_IRQRestoreCmd(state) EIC_IRQCmd(state)

#endif //__STR75X_H
