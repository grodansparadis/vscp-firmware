;*********************************************************************
;*
;*             PIC Configuration bits for Microchip TCP/IP Stack
;*
;*********************************************************************
;* FileName:        C18Cf.asm
;* Dependencies:    P18C6525.INC
;* Processor:       PIC18CXXX
;* Complier:        MPLAB 5.4.00
;* Company:         Microchip Technology, Inc.
;*
;* Software License Agreement
;*
;* The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
;* of the Microchip TCP/IP stack. The original code base is owned by Microchip
;* Technology Incorporated (the Company), and is protected under applicable
;* copyright laws. The modifications are owned by Modtronix Engineering, and is
;* protected under applicable copyright laws. The software supplied herewith is
;* intended and supplied to you, the Company customer, for use solely and
;* exclusively on Microchip PICmicro Microcontroller based products manufactured
;* by Modtronix Engineering. The code may be modified and can be used free of charge
;* for non commercial and commercial applications. All rights are reserved. Any use
;* in violation of the foregoing restrictions may subject the user to criminal
;* sanctions under applicable laws, as well as to civil liability for the breach
;* of the terms and conditions of this license.
;*
;* THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
;* IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
;* COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
;* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;*
;*
;*
;* This file defines configuration bits only.
;*
;* Author               Date        Comment
;*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;* David Hosken         2004/10/05  Added 18F6621
;********************************************************************/

#ifdef __18F6621
    #include <p18f6621.inc>
    CONFIG OSC=HSPLL, OSCS=OFF, PWRT=ON, BOR=ON, BORV=42, WDT=OFF
    CONFIG LVP=OFF, DEBUG = OFF
    ;__CONFIG    _CONFIG3H, _CCP2MX_OFF_3H
#endif

    END
