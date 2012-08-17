;*********************************************************************
;*
;*             PIC Configuration bits for Microchip TCP/IP Stack
;*
;*********************************************************************
;* FileName:        C18Cf.asm
;* Dependencies:    P18C452.INC
;* Processor:       PIC18CXXX
;* Complier:        MPLAB 5.4.00
;* Company:         Microchip Technology, Inc.
;*
;* Software License Agreement
;*
;* The software supplied herewith by Microchip Technology Incorporated
;* (the “Company”) for its PICmicro® Microcontroller is intended and
;* supplied to you, the Company’s customer, for use solely and
;* exclusively on Microchip PICmicro Microcontroller products. The
;* software is owned by the Company and/or its supplier, and is
;* protected under applicable copyright laws. All rights are reserved.
;* Any use in violation of the foregoing restrictions may subject the
;* user to criminal sanctions under applicable laws, as well as to
;* civil liability for the breach of the terms and conditions of this
;* license.
;*
;* THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
;* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
;* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
;* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
;* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;*
;*
;*
;* This file defines configuration bits only.
;*
;* Author               Date        Comment
;*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;* Nilesh Rajbharti     9/19/01     Original (Rev. 1.0)
;* Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
;********************************************************************/

#ifdef __18C252
    #include <p18c252.inc>
#endif

#ifdef __18C452
    #include <p18c452.inc>

    __CONFIG    _CONFIG0, _CP_OFF_0
    __CONFIG    _CONFIG1, _OSCS_OFF_1 & _HS_OSC_1
    __CONFIG    _CONFIG2, _BOR_OFF_2 & _PWRT_ON_2
    __CONFIG    _CONFIG3, _WDT_OFF_3
    __CONFIG    _CONFIG5, _CCP2MX_OFF_5
    __CONFIG    _CONFIG6, _STVR_ON_6
#endif

#ifdef __18C658
    #include <p18c658.inc>

    __CONFIG    _CONFIG0, _CP_OFF_0
    __CONFIG    _CONFIG1, _OSCS_OFF_1 & _HS_OSC_1
    __CONFIG    _CONFIG2, _BOR_OFF_2 & _PWRT_ON_2
    __CONFIG    _CONFIG3, _WDT_OFF_3
    __CONFIG    _CONFIG6, _STVR_ON_6

#endif

#ifdef __18C858
    #include <p18c658.inc>

    __CONFIG    _CONFIG0, _CP_OFF_0
    __CONFIG    _CONFIG1, _OSCS_OFF_1 & _HS_OSC_1
    __CONFIG    _CONFIG2, _BOR_OFF_2 & _PWRT_ON_2
    __CONFIG    _CONFIG3, _WDT_OFF_3
    __CONFIG    _CONFIG6, _STVR_ON_6

#endif


#ifdef __18F452
    #include <p18f452.inc>

    __CONFIG    _CONFIG1H, _OSCS_OFF_1H & _HS_OSC_1H
    __CONFIG    _CONFIG2H, _WDT_OFF_2H
    __CONFIG    _CONFIG3H, _CCP2MX_OFF_3H 
    __CONFIG    _CONFIG4L, _LVP_OFF_4L & _DEBUG_OFF_4L
#endif


; Add new configuration as new micro is added...

    END
