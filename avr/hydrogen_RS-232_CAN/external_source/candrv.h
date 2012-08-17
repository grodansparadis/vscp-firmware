// File: candrv.h
//

/**
   This file include general can definitions for the candrv

   Copyright (C) 2002 Ake Hedman, CC Sytems AB
   The code is originally  derived from code written by 
   Arnaud Westenberg  email:arnaud@wanadoo.nl in his can driver.
 */

#include <linux/types.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <linux/pci.h>
#include <linux/poll.h>
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,2,19))
#include <asm/spinlock.h>
#else
#include <linux/spinlock.h>
#endif
#include <asm/io.h>
#include "can.h"

#ifndef ___CAN_DRIVER___
#define ___CAN_DRIVER___

#endif








