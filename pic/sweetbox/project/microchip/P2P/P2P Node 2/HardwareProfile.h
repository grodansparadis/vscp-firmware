#ifndef _HARDWARE_PROFILE_H
#define _HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"

#ifdef __PIC32MX__
    #define RCON_SLEEP_MASK (0x8)
    #define RCON_IDLE_MASK  (0x4)
    #define        Sleep()      PowerSaveSleep()
    
    void Enable_PB_1_2_Interrupts(void);
#endif

BYTE ButtonPressed(void);
void BoardInit(void);

#endif
