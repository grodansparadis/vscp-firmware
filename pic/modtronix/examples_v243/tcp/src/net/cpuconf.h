/**
 * @brief           CPU Configuration file
 * @file            cpuconf.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.10PL1 or higher
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2006-02-03, David Hosken (DH):
 *    - Added support for PIC18F6627 CPU
 * 2006-01-09, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef _CPUCONF_H_
#define _CPUCONF_H_


/////////////////////////////////////////////////
//Set configuration fuses for Microchip MPLAB C18 compiler. Possible values are
//defined in "C Compiler Addendum.pdf" document
#if defined(__18CXX)    //Microchip C18 compiler
    #if defined(__18F452)
        //HS with Software controlled PLL   
        //#pragma config OSC = HSSWPLL, OSCS = OFF
        #pragma config OSC = HS, OSCS = OFF
        #pragma config PWRT = ON
        #pragma config BOR = OFF, BORV = 42
        #pragma config WDT = OFF
        //#pragma config CCP2MX = OFF
        #pragma config LVP = OFF
        #pragma config DEBUG = OFF
    #elif  defined(__18F458)
        //HS with Software controlled PLL   
        //#pragma config OSC = HSSWPLL, OSCS = OFF
        #pragma config OSC = HS, OSCS = OFF
        #pragma config PWRT = ON
        #pragma config BOR = OFF, BORV = 42
        #pragma config WDT = OFF
        //#pragma config CCP2MX = OFF
        #pragma config LVP = OFF
        #pragma config DEBUG = OFF
    #elif  defined(__18F6527)
        #error "No configuration bits defined for this processor!"
        #pragma config WDTPS = 128   /* Watchdog prescaler = 128. This gives: min=435ms, typ=512ms, max=589ms */
    #elif  defined(__18F6627)
        #pragma config OSC = HSPLL      //HS with 4 x PLL enabled
        #pragma config FCMEN = ON       //Fail save clock monitor off
        #pragma config IESO = OFF       //Two speed start up disabled - board only starts once PLL is stable!
        #pragma config PWRT = ON

        //Microchip changed this value between C18 versions!!!!!!!!!!
        //For version C18 V3.02, use "BORV = 1"
        //For version C18 V3.00, use "BORV = 43"
        //#pragma config BOREN = SBORDIS, BORV = 43   //Enabled, can be disabled in software with SBOREN bit
        #pragma config BOREN = SBORDIS, BORV = 1   //Enabled (4.3V), can be disabled in software with SBOREN bit

        #pragma config WDT = OFF        //Watchdog timer can be enabled in software with SWDTEN bit
        #pragma config WDTPS = 128      // Watchdog prescaler = 128. This gives: min=435ms, typ=512ms, max=589ms
        #pragma config MCLRE = ON
        #pragma config LPT1OSC = OFF    //Low power mode disable
        #pragma config CCP2MX = PORTC   //Multiplexed with RC1
        #pragma config STVREN = ON      //Stack overflow reset on
        #pragma config LVP = OFF
        #pragma config BBSIZ = BB2K     //Boot block size = 2k
        #pragma config XINST = OFF      //Extended instructions off
        #pragma config DEBUG = OFF
        //All code protection off
        #pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, CP4 = OFF, CP5 = OFF
        //Data EEPROM protect off
        #pragma config CPD = OFF, WRTD = OFF
        //All write protection off
        #pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF, WRT4 = OFF, WRT5 = OFF
        //Table read protection off
        #pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTR4 = OFF, EBTR5 = OFF

        //Configuration bits to allow execution of code in lower 2K memory
        #ifdef HAS_BOOTLOADER
            #pragma config CPB = ON
            #pragma config WRTB = ON
            #pragma config WRTC = ON
            #pragma config EBTRB = ON
        #else
            #pragma config CPB = OFF
            #pragma config WRTB = OFF
            #pragma config WRTC = OFF
            #pragma config EBTRB = OFF
        #endif
    #elif  defined(__18F6621)
        //HS with Software controlled PLL
        //#pragma config OSC = HSSWPLL, OSCS = OFF
        #pragma config OSC = HSPLL, OSCS = OFF
        #pragma config PWRT = ON
        #pragma config BOR = OFF, BORV = 42
        #pragma config WDT = OFF
        #pragma config CCP2MX = PORTC          /* Use C1 for CCP2 */
        #pragma config LVP = OFF
        #pragma config DEBUG = OFF
        #pragma config WDTPS = 32   /* Watchdog prescaler = 32. This gives: min=224ms, typ=576ms, max=1056ms */
        #pragma config MCLRE = ON
        #pragma config STVR = ON   /* Stack overflow reset on */

        //All code protection off
        #pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
        //Data EEPROM protect off
        #pragma config CPD = OFF, WRTD = OFF
        //All write protection off
        #pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
        //Table read protection off
        #pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF

        //Configuration bits to allow execution of code in lower 2K memory
        #ifdef HAS_BOOTLOADER
            #pragma config CPB = ON
            #pragma config WRTB = ON
            #pragma config WRTC = ON
            #pragma config EBTRB = ON
        #else
            #pragma config CPB = OFF
            #pragma config WRTB = OFF
            #pragma config WRTC = OFF
            #pragma config EBTRB = OFF
        #endif
    #elif  defined(__18F6680)
        //HS with Software controlled PLL
        //#pragma config OSC = HSSWPLL, OSCS = OFF
        #pragma config OSC = HSPLL, OSCS = OFF
        #pragma config PWRT = ON
        #pragma config BOR = ON, BORV = 42  /* Brown out reset on */
        #pragma config WDT = OFF
        #pragma config CCP2MX = ON          /* Use C1 for CCP2 */
        #pragma config LVP = OFF
        #pragma config DEBUG = OFF
        #pragma config WDTPS = 32   /* Watchdog prescaler = 32. This gives: min=224ms, typ=576ms, max=1056ms */
        #pragma config MCLRE = ON
        #pragma config STVR = ON   /* Stack overflow reset on */

        //All code protection off
        #pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
        //Data EEPROM protect off
        #pragma config CPD = OFF, WRTD = OFF
        //All write protection off
        #pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
        //Table read protection off
        #pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF

        //Configuration bits to allow execution of code in lower 2K memory
        #ifdef HAS_BOOTLOADER
            #pragma config CPB = ON
            #pragma config WRTB = ON
            #pragma config WRTC = ON
            #pragma config EBTRB = ON
        #else
            #pragma config CPB = OFF
            #pragma config WRTB = OFF
            #pragma config WRTC = OFF
            #pragma config EBTRB = OFF
        #endif
    #endif
#endif
//Set configuration fuses for Hi-Tech compiler.
#if defined(HITECH_C18)
    #if defined(_18F452)
        __CONFIG(1, HS);      //HS with Software controlled PLL
        __CONFIG(2, PWRTEN & WDTDIS);
        __CONFIG(4, LVPDIS & DEBUGDIS);
    #elif defined(_18F458)
        __CONFIG(1, HS);      //HS with Software controlled PLL
        __CONFIG(2, PWRTEN & WDTDIS);
        __CONFIG(4, LVPDIS & DEBUGDIS);
    #elif defined(_18F6527)
        __CONFIG(1, HSPLL);                 /* HS osc + HW 4xPLL */
        /* 4.3V Brown out (SBOREN controlled), Power up timer, Watchdog disabled (SWDTEN controlled), Watchdog prescaler = 128. This gives: min=435ms, typ=512ms, max=589ms */
        __CONFIG(2, SWBOREN & BORV43 & PWRTEN & WDTDIS & WDTPS128);
        /* Low power mode for TMR1 disabled, master clear enabled */
        __CONFIG(3, LPT1DIS & MCLREN);
        /* LVP Disabled, Debug mode disabled, Extended mode disable, Boot block is 1K word */
        __CONFIG(4, STVREN & LVPDIS & DEBUGDIS & XINSTDIS & BBSIZ1K);
        #ifdef HAS_BOOTLOADER
        __CONFIG(5, CPB);
        __CONFIG(6, WPB & WPC);
        __CONFIG(7, TRPB);
        #else
        __CONFIG(5, 0xffff);
        __CONFIG(6, 0xffff);
        __CONFIG(7, 0xffff);
        #endif
    #elif  defined(_18F6621)
        // TEST TEST Should be SW controlled
        //__CONFIG(1, HSSW);      //HS with Software controlled PLL
        __CONFIG(1, HSHW);                  /* HS osc + HW 4xPLL */
        /* 4.2V Brown out, Power up timer, Watchdog disabled, Watchdog prescaler = 32. This gives: min=224ms, typ=576ms, max=1056ms */
        __CONFIG(2, BORV42 & PWRTEN & WDTDIS & WDTPS32);
        __CONFIG(3, 0xffff);
        __CONFIG(4, LVPDIS & DEBUGDIS);     /* LVP Disabled, Debug mode disabled */
        #ifdef HAS_BOOTLOADER
        __CONFIG(5, CPB);
        __CONFIG(6, WPB & WPC);
        __CONFIG(7, TRPB);
        #else
        __CONFIG(5, 0xffff);
        __CONFIG(6, 0xffff);
        __CONFIG(7, 0xffff);
        #endif
    #elif  defined(_18F6680)
        // TEST TEST Should be SW controlled
        //__CONFIG(1, HSSW);      //HS with Software controlled PLL
        __CONFIG(1, HSHW);                  /* HS osc + HW 4xPLL */
        /* 4.2V Brown out, Power up timer, Watchdog disabled, Watchdog prescaler = 32. This gives: min=224ms, typ=576ms, max=1056ms */
        __CONFIG(2, BORV42 & PWRTEN & WDTDIS & WDTPS32);
        __CONFIG(3, 0xffff);
        __CONFIG(4, LVPDIS & DEBUGDIS);     /* LVP Disabled, Debug mode disabled */
        #ifdef HAS_BOOTLOADER
        __CONFIG(5, CPB);
        __CONFIG(6, WPB & WPC);
        __CONFIG(7, TRPB);
        #else
        __CONFIG(5, 0xffff);
        __CONFIG(6, 0xffff);
        __CONFIG(7, 0xffff);
        #endif
    #elif  defined(_18F6627)
        __CONFIG(1, HSPLL);                 /* HS osc + HW 4xPLL */
        /* 4.3V Brown out (SBOREN controlled), Power up timer, Watchdog disabled (SWDTEN controlled), Watchdog prescaler = 128. This gives: min=435ms, typ=512ms, max=589ms */
        __CONFIG(2, SWBOREN & BORV43 & PWRTEN & WDTDIS & WDTPS128);
        /* Low power mode for TMR1 disabled, master clear enabled */
        __CONFIG(3, LPT1DIS & MCLREN);
        /* LVP Disabled, Debug mode disabled, Extended mode disable, Boot block is 1K word */
        __CONFIG(4, STVREN & LVPDIS & DEBUGDIS & XINSTDIS & BBSIZ1K);
        #ifdef HAS_BOOTLOADER
        __CONFIG(5, CPB);
        __CONFIG(6, WPB & WPC);
        __CONFIG(7, TRPB);
        #else
        __CONFIG(5, 0xffff);
        __CONFIG(6, 0xffff);
        __CONFIG(7, 0xffff);
        #endif
    #endif
#endif

#endif	//_CPUCONF_H_





