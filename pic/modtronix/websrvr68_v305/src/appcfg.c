/*********************************************************************
 * appcfg.c    -    Application Configuration functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2004-10-19, David Hosken (DH):
 *    - Initial version
 * 2006-09-04, David Hosken (DH):
 *    - Updated _appcfgPutc() and appcfgPutc() functions so they only write to EEPROM if the
 *      given value is different from current EEPROM value - extends life of EEPROM.
 *
 *********************************************************************
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
 *
 ********************************************************************/
#define THIS_IS_APPCFG_MODULE

#include "projdefs.h"
#include "appcfg.h"
#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD0, msgCode)

/////////////////////////////////////////////////
//Uncomment following lines if BAUD rate is statically set. They will calculate SPBRG1 and SPBRGH1
//USART1 is used in 16 bit baud rate generator mode, BRGH high, BRG16 high
//#define BAUD_RATE1       (57600)    // bps
//#define SPBRG1_VAL   ((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) & 0xff)
//#define SPBRGH1_VAL   (((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) >> 8) & 0xff)
//#if SPBRG1_VAL > 255
//    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
//#endif


/////////////////////////////////////////////////
//Variables
APP_CONFIG AppConfig;   //This is used by other stack elements.

//Array containing converted ADC values
#if defined(APP_USE_ADC8)
BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
WORD AdcValues[ADC_CHANNELS];
#endif

/**
 * @preCondition  fsysInit() is already called.
 */
void appcfgInit(void)
{
    BYTE *p;

    /////////////////////////////////////////////////
    //Load configuration from EEPROM to RAM
    //Read contents of BLCONNFIG structure from EEPROM. It is located at the start of the EEPROM
    #if (EEPROM_SIZE > 256)
    EEADRH = 0;
    #endif
    EEADR = 0;
    
    //Configure for EEPROM reading, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x3F;

    p = (BYTE*)&AppConfig;

    for ( ; EEADR < sizeof(AppConfig); EEADR++ ) {
        EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
        *p++ = EEDATA;
    }
}

/**
 * Get a specified byte from the APP_CONFIG configuration structure
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
BYTE appcfgGetc(BYTE offset) {
    //Set address
    EEADR = offset;
    #if (EEPROM_SIZE > 256)
    EEADRH = 0;
    #endif

    //Wait if something is currently being written
    while(EECON1_WR) FAST_USER_PROCESS();

    //Configure for EEPROM reading, EEPGD=0, CFGS=0
    EECON1 &= 0x3F;

    EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
    return EEDATA;
}

/**
 * Set a specified byte in the APP_CONFIG configuration structure to a given value.
 * This function will only update the EEPROM if the given new value is different from the
 * current old value. This saves the EEPROM, and increases it's live!
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
void _appcfgPutc(BYTE offset) {
    BYTE newValue;

    newValue = EEDATA;  //Store new value
    
    //Set address
    EEADR = offset;
    #if (EEPROM_SIZE > 256)
    EEADRH = 0;
    #endif
    
    //Wait if something is currently being written
    while(EECON1_WR);

    /////////////////////////////////////////////////
    //Only write to EEPROM if new value is different from old value - this saves the EEPROM!

    //Configure for EEPROM reading, EEPGD=0, CFGS=0
    EECON1 &= 0x3F;

    EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
    
    //If new value is the same as the old value, return
    if (EEDATA == newValue) {
        return;
    }


    /////////////////////////////////////////////////
    //Write new value to EEPROM
    EEDATA = newValue;      //New value to be written

    //Configure for EEPROM writing, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x2F;

    EECON1_WREN = 1;        //Write enable
    EECON2      = 0x55;
    EECON2      = 0xAA;
    EECON1_WR   = 1;
    Nop();
    //Wait for write to finish
    while(EECON1_WR);
    EECON1_WREN = 0;

    return;
}

/**
 * Save Application Configuartion data to EEPROM
 */
void appcfgSave(void)
{
    /* Currently not required
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;

    for ( c = 0; c < sizeof(AppConfig); c++ ) {
        EEDATA=*p++;
        _appcfgPutc(c);
    }
    */
}


/*
 * Configure USART with AppConfig data
 */
void appcfgUSART(void) 
{
    WORD w;

#if defined(__18F452) || defined(_18F452) || defined(__18F458) || defined(_18F458)
    //Initialize USART1 Control registers
    TXSTA = 0b00100100;     //High BRG speed
    RCSTA = 0b10010000;

    #if (CLOCK_FREQ == 20000000)
    //Get USART BAUD rate setting
    switch (appcfgGetc(APPCFG_USART1_BAUD))
    {
    case BAUD_2400:
        TXSTA = 0b00100000;     //Low BRG speed
        SPBRG = 129;    //0.13% Error
        break;
    case BAUD_4800:
        TXSTA = 0b00100000;     //Low BRG speed
        SPBRG = 64; //0.16% Error
        break;
    case BAUD_9600:
        SPBRG = 32; //1.36% Error
        break;
    case BAUD_19200:
        SPBRG = 64; //0.16% Error
        break;
    case BAUD_38400:
        SPBRG = 32; //1.3% Error
        break;
    case BAUD_57600:
        SPBRG = 21; //1.3% Error
        break;
    case BAUD_115200:
        SPBRG = 10; //1.3% Error
        break;
    default:
        SPBRG = 21; //1.3% Error
        break;
    }
    #else
    #error "appcfgUSART() does not support selected clock frequency"
    #endif

#elif defined(__18F6621) || defined(_18F6621)       \
        || defined(__18F6527) || defined(_18F6527)  \
        || defined(__18F6627) || defined(_18F6627)  \
        || defined(__18F6680) || defined(_18F6680)
    //Initialize USART1 Control registers
    TXSTA = 0b00100100;     //High BRG speed
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;

    #if (CLOCK_FREQ == 40000000)
    //Get USART BAUD rate setting
    switch (appcfgGetc(APPCFG_USART1_BAUD))
    {
    case BAUD_300:
        w = 33332;
        break;
    case BAUD_1200:
        w = 8332;
        break;
    case BAUD_2400:
        w = 4165;
        break;
    case BAUD_4800:
        w = 2082;
        break;
    case BAUD_9600:
        w = 1040;
        break;
    case BAUD_19200:
        w = 520;
        break;
    case BAUD_38400:
        w = 259;
        break;
    case BAUD_57600:
        w = 172;
        break;
    case BAUD_115200:
        w = 86;
        break;
    default:
        w = 172;
        break;
    }

    SPBRG =  (BYTE)w;
    SPBRGH = (BYTE)(w >> 8);
    #else
    #error "appcfgUSART() does not support selected clock frequency"
    #endif
#else
    #error "CPU type not defined!"
#endif
}

/*
 * Configure the CPU I/O ports direction - Input or output
 */
void appcfgCpuIO(void)
{
#if defined(BRD_SBC44EC)
    TRISA = appcfgGetc(APPCFG_TRISA);

    //Configure TRISB port:
    //Port B0 - B4 are always outputs for the SBC44EC!
    //Port B5 is always an input - IOCHRDY signal
    TRISB = (appcfgGetc(APPCFG_TRISB) | 0b00100000) & 0b11100000;

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    TRISC = appcfgGetc(APPCFG_TRISC) | 0xd8;
#elif defined(BRD_SBC45EC)
    //Configure TRISA port:
    //Port A0, A1, A2, A3 and A5 are always outputs for the SBC45EC!
    //Port A4 is always an input - IOCHRDY signal
    TRISA = (appcfgGetc(APPCFG_TRISA) | 0b00010000) & 0b11010000;

    //Configure TRISB port:
    TRISB = appcfgGetc(APPCFG_TRISB);

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    TRISC = appcfgGetc(APPCFG_TRISC) | 0xd8;
#elif defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
    TRISA = appcfgGetc(APPCFG_TRISA);

    //Ensure following TRISB ports are always inputs when NIC INT0 is enabled:
    #if defined(NIC_DISABLE_INT0)
        TRISB = appcfgGetc(APPCFG_TRISB);
    #else
        //B0 = Input from RTL8019AS
        TRISB = appcfgGetc(APPCFG_TRISB) | 0x01;
    #endif

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    TRISC = appcfgGetc(APPCFG_TRISC) | 0xd8;

    //Set PORT G and F direction bits
    TRISF = appcfgGetc(APPCFG_TRISF);
    
    //Ensure following TRISG ports are always inputs:
    //G4 = Is connected to IOCHRDY
    TRISG = appcfgGetc(APPCFG_TRISG) | 0x10;
#else
    #error "Board type not defined!"
#endif
}

/*
 * Configure the CPU I/O ports default power up values
 */
void appcfgCpuIOValues(void)
{
    //Set PORT A,B,D and F direction bits
    LATA = appcfgGetc(APPCFG_PORTA);
    LATB = appcfgGetc(APPCFG_PORTB);
    LATC = appcfgGetc(APPCFG_PORTC);
    #if defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
    LATF = appcfgGetc(APPCFG_PORTF);
    LATG = appcfgGetc(APPCFG_PORTG);
    #endif
}

/*
 * Configure the ADC unit
 */
void appcfgADC(void)
{
#if defined(BRD_SBC44EC)
    //For 8 bit mode, ensure left adjust is enabled
    #if defined(APP_USE_ADC8)
        //Clear ADFM bit
        ADCON1 = appcfgGetc(APPCFG_ADCON1) & ~0x80;
    #elif defined(APP_USE_ADC10)
        //Set ADFM bit
        ADCON1 = appcfgGetc(APPCFG_ADCON1) |= 0x80;
    #endif

    //Use ADCON2 for ADCON0
    ADCON0 = appcfgGetc(APPCFG_ADCON0_2);
#elif defined(BRD_SBC45EC)  //ADC is not used
#elif defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
    ADCON1 = appcfgGetc(APPCFG_ADCON1);

    //For 8 bit mode, ensure left adjust is enabled
    #if defined(APP_USE_ADC8)
        //Clear ADFM bit
        ADCON2 = appcfgGetc(APPCFG_ADCON0_2) & ~0x80;
    #elif defined(APP_USE_ADC10)
        //Set ADFM bit
        ADCON2 = appcfgGetc(APPCFG_ADCON0_2) | 0x80;
    #endif
#else
    #error "Board type not defined!"
#endif
}

//This contains the default values of the APP_CONFIG structure, and has to be placed at off
//0 of the EEPROM = 0xf00000.
//To do this with HiTech PICC18:
//  add "-L-Peeconfig=f00000h" to the linker command line (without the " characters)
//To do this with MPLAB C18 compiler:
//  nothing has to be done!
//
#if defined(HI_TECH_C)  //Hi-Tech compiler
    #pragma psect const=eeconfig
#elif defined(__18CXX)  //MPLAB C18 compiler
    #pragma romdata EESECT=0xf00000
    //#pragma romdata eeconfig=0xf00000
#endif
ROM BYTE eeconfigArray[] = {
    //MyIPAddr
    MY_DEFAULT_IP_ADDR_BYTE1,   //Offset 0
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,
    MY_DEFAULT_IP_ADDR_BYTE4,
    //MyMACAddr
    MY_DEFAULT_MAC_BYTE1,       //Offset 4
    MY_DEFAULT_MAC_BYTE2,
    MY_DEFAULT_MAC_BYTE3,
    MY_DEFAULT_MAC_BYTE4,
    MY_DEFAULT_MAC_BYTE5,
    MY_DEFAULT_MAC_BYTE6,
    //MyMask
    MY_DEFAULT_MASK_BYTE1,      //Offset 10
    MY_DEFAULT_MASK_BYTE2,
    MY_DEFAULT_MASK_BYTE3,
    MY_DEFAULT_MASK_BYTE4,
    //MyGateway
    MY_DEFAULT_GATE_BYTE1,      //Offset 14
    MY_DEFAULT_GATE_BYTE2,
    MY_DEFAULT_GATE_BYTE3,
    MY_DEFAULT_GATE_BYTE4,
    //wait4bl - Delay how long we wait for the bootloader, value from 0-n. Time = 0.8sec x n
    //Default value = 2.4
    4,
    //flashHasProgram - If 1, the FLASH has been programmed and contain a valid program.
    1,
    //Startup serial delay
    40,                         //Offset 20 = 40 x 50ms = 2 seconds
    //System Flags
    //xxxx xxx1 = PLL Enabled
    //xxxx xx1x = Blink LED on B7
    0x03,                       //Offset 21
    //Network Flags
    //xxxx xxx1 = DHCP Enabled
    0x01,                       //Offset 22     
    
    //SMTPServerAddr
    MY_DEFAULT_IP_ADDR_BYTE1,
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,   //Offset 25
    MY_DEFAULT_IP_ADDR_BYTE4,
    //TFTPServerAddr
    MY_DEFAULT_IP_ADDR_BYTE1,
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,
    MY_DEFAULT_IP_ADDR_BYTE4,   //Offset 30
    //USART1_CFG.baud
    BAUD_57600,
    //USART1_CFG.cfg
    0,
    //USART2_CFG.baud
    BAUD_57600,
    //USART2_CFG.cfg
    0,
    //PORT_CFG - 7 bytes for port A, B, C, D, E, F and G. A 0 bit=output, 1 bit=input
    0b11111111, //Port A - All inputs   - Offset 35
    0b11111111, //Port B - All inputs
    0b11111111, //Port C - All inputs
    0b11111111, //Port D - All inputs
    0b11111111, //Port E - All inputs
    0b11111111, //Port F - All inputs   - Offset 40
    0b11111111, //Port G - All inputs
    //PORT_VALUE - 7 bytes for port A, B, C, D, E, F and G. Gived default values for output ports
    0b00000000, //Port A - All 0s
    0b00000000, //Port B - All 0s
    0b00000000, //Port C - All 0s
    0b00000000, //Port D - All 0s       - Offset 45
    0b00000000, //Port E - All 0s
    0b00000000, //Port F - All 0s
    0b00000000, //Port G - All 0s       - Offset 48
    
    #if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
        //APPCFG_ADCON1 - adcon1            - Offset 49
        // xxxx 0010 - A0, A1, A2, A3 and A5 = ADC Channels
        // 0xxx xxxx - Left justified
        0x02,
        //APPCFG_ADCON0_2 - adcon0          - Offset 50
        0x80,   // 10xx xxxx - Fosc/32 clock = 1.6us @ 20MHz
    #elif defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
        //APPCFG_ADCON1 - adcon1            - Offset 49
        0x03,   //A0 - A5 and F0 - F6 = ADC, no digital inputs
        //APPCFG_ADCON0_2 - adcon2          - Offset 50
        0x2e,   // xxxx x110 - Fosc/64 clock = 1.6us @ 40MHz
                // xx10 1xxx - 12 Tad = 12 x 1.6 = 19.2uS conversion time
                // 0xxx xxxx - Left justified
    #else
        #error "Board type not defined!"
    #endif

    //SerialNumber
    0,                                  //Offset 51
    0,
    /** Username - Is a null terminated string. Max 8 char + null termination char*/
    'a',                                //Offset = 53
    'd',                                //Offset = 54
    'm',                                //Offset = 55
    'i',                                //Offset = 56
    'n',                                //Offset = 57
    '\0',                               //Offset = 58
    '\0',                               //Offset = 59
    '\0',                               //Offset = 60
    '\0',                               //Offset = 61
    /** Password - Is a null terminated string. Max 8 char + null termination char*/
    'p',                                //Offset = 62
    'w',                                //Offset = 63
    '\0',                               //Offset = 64
    '\0',                               //Offset = 65
    '\0',                               //Offset = 66
    '\0',                               //Offset = 67
    '\0',                               //Offset = 68
    '\0',                               //Offset = 69
    '\0',                               //Offset = 70


    /** NetBIOS Name - Is a 16 character name, all in uppercase. All unused characters must be spaces! Must end with 0!*/
    'M',                                // Offset = 71
    'X',                                // Offset = 72
    'B',                                // Offset = 73
    'O',                                // Offset = 74
    'A',                                // Offset = 75
    'R',                                // Offset = 76
    'D',                                // Offset = 77
    ' ',                                // Offset = 78
    ' ',                                // Offset = 79
    ' ',                                // Offset = 80
    ' ',                                // Offset = 81
    ' ',                                // Offset = 82
    ' ',                                // Offset = 83
    ' ',                                // Offset = 84
    ' ',                                // Offset = 85
    0,                                  // Offset = 86
    
    /** Primary DNS server. First entry is MSB byte of IP address, for example in "192.168.1.0" it will be 192 */
    MY_DEFAULT_DNS_BYTE1,               // Offset = 87
    MY_DEFAULT_DNS_BYTE2,               // Offset = 88
    MY_DEFAULT_DNS_BYTE3,               // Offset = 89
    MY_DEFAULT_DNS_BYTE4                // Offset = 90
};
