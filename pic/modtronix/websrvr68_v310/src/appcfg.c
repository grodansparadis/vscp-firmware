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
#include "buses.h"
#include "busser1.h"
#include "busser2.h"
#include "ior5e.h"
#include "mxd2r.h"
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
    EEADRH = 0;
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
BYTE appcfgGetc(WORD offset) {
    //Set address
    EEADR = (BYTE)offset;
    EEADRH = (BYTE)(offset >> 8);

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
 * IMPORTANT - EEADRH must be set before calling this function! This configures the EEPROM page.
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
void _appcfgPutc(WORD offset) {
    BYTE newValue;

    newValue = EEDATA;  //Store new value
    
    //Set address
    EEADR = (BYTE)offset;
    EEADRH = (BYTE)(offset >> 8);
    
    //Wait if something is currently being written
    while(EECON1_WR) FAST_USER_PROCESS();

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
    while(EECON1_WR) FAST_USER_PROCESS();
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
    BYTE cfg;
    
    cfg = appcfgGetc(APPCFG_USART1_CFG);

#if defined(__18F452) || defined(_18F452) || defined(__18F458) || defined(_18F458)

    //USART is enabled
    if (cfg & APPCFG_USART_ENABLE) {
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

        serEnable();   //Enable serial port
    }
    //USART is disbled
    else {
        serDisable();  //Disable serial port
    }
#elif defined(__18F6621) || defined(_18F6621)       \
        || defined(__18F6527) || defined(_18F6527)  \
        || defined(__18F6627) || defined(_18F6627)  \
        || defined(__18F6680) || defined(_18F6680)

    //USART is enabled
    if (cfg & APPCFG_USART_ENABLE) {
        TXSTA_BRGH = 1;     //High baud rate mode
        
        //xxxx 1xxx - 16bit baud rate generator
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
        
        serEnable();   //Enable serial port
    }
    //USART is disbled
    else {
        serDisable();  //Disable serial port
    }

#else
    #error "CPU type not defined!"
#endif
}


/*
 * Configure USART2 with AppConfig data
 */
void appcfgUSART2(void) 
{
    WORD w;
    BYTE cfg;

#if defined(__18F6621) || defined(_18F6621)       \
        || defined(__18F6527) || defined(_18F6527)  \
        || defined(__18F6627) || defined(_18F6627)
        
    cfg = appcfgGetc(APPCFG_USART2_CFG);
    
    //Continue if USART is enabled
    if (cfg & APPCFG_USART_ENABLE) {
        TXSTA2_BRGH = 1;     //High baud rate mode
        
        //xxxx 1xxx - 16bit baud rate generator
        BAUDCON2 = 0b00001000;

        #if (CLOCK_FREQ == 40000000)
        //Get USART BAUD rate setting
        switch (appcfgGetc(APPCFG_USART2_BAUD))
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

        SPBRG2 =  (BYTE)w;
        SPBRGH2 = (BYTE)(w >> 8);
        #else
        #error "appcfgUSART2() does not support selected clock frequency"
        #endif

        ser2Enable();   //Enable serial port
    }
    //USART is disbled
    else {
        ser2Disable();  //Disable serial port
    }
#elif defined(__18F6680) || defined(_18F6680)
    //Do nothing, no second USART!
#else
    #error "CPU type not defined!"
#endif
}


/*
 * Configure the CPU I/O ports direction - Input or output
 */
void appcfgCpuIO(void)
{
    BYTE val;
    
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
        val = appcfgGetc(APPCFG_TRISB);
    #else
        //B0 = Input from RTL8019AS
        val = appcfgGetc(APPCFG_TRISB) | 0x01;
    #endif
    //If IOR5E is used, ensure B0, B1, B4 and B5 are inputs
    if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
        val |= 0b00110011;
    }
    //If MXD2R is used, ensure B4 and B5 are outputs
    else if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_MXD2R) {
        val &= 0b11001111;
    }
    TRISB = val;

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    val = appcfgGetc(APPCFG_TRISC) | 0xd8;
    
    //If IOR5E is used, ensure C0, C1, C2 and C5 are outputs
    if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
        val &= 0b11011000;
    }
    //If MXD2R is used, ensure C0, C1 and C2 are outputs
    else if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_MXD2R) {
        val &= 0b11111000;
    }
    TRISC = val;

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
    BYTE val;
    
    //Set PORT A,B,D and F direction bits
    LATA = appcfgGetc(APPCFG_PORTA);
    LATB = appcfgGetc(APPCFG_PORTB);

    val = appcfgGetc(APPCFG_PORTC);
    //If IOR5E is used, ensure RC5 is set at this stage. This will disable outputs of latches.
    //They are only enabled in the IOR5E initialization code once they are initialized.
    if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
        val |= 0b00100000;
    }
    LATC = val;

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

    //---- xxxx = ADC Port Configuration bits
    //--xx ---- = Voltage reference configuration bits
    ADCON1 = appcfgGetc(APPCFG_ADCON1);

    //ADCON2 bits are:
    // xxxx x110 - ADC Conversion clock select
    // xx10 1xxx - ADC Aquasition time select
    // 0xxx xxxx - ADC Result format select bit, 0=Left justified, 1=right justified

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

/* String used for encoding to base64 */
#if defined(BRD_SBC65EC)
    static ROM BYTE* pwmCCPRL[] = {(ROM BYTE *)(&CCPR1L), (ROM BYTE *)(&CCPR2L), (ROM BYTE *)(&CCPR3L), (ROM BYTE *)(&CCPR4L), (ROM BYTE *)(&CCPR5L)};
#elif defined(BRD_SBC68EC)
    static ROM BYTE* pwmCCPRL[] = {(ROM BYTE *)(&CCPR1L), (ROM BYTE *)(&CCPR2L)};
#endif

/**
 * Set the given PWM channel to the given value
 *
 * @param ch PWM channnel, is a value from 1 to 5
 * @param val The value to set the channel.
 *            - For 8 bit mode, is a value from 0-255
 *            - For 10 bit mode, is a value from 0-1023
 * @param remember If 1, store in EEPROM and remember
 *
 */
void pwmSetValue(BYTE ch, WORD_VAL val, BOOL remember) {
    BYTE* p;
    BYTE* pCCPCON;
    BYTE tmp;
    
    #if defined(BRD_SBC65EC)
    //Ensure ch is between 1 to 5
    if ((ch < 1) || (ch > 5))
        return;
    #elif defined(BRD_SBC68EC)
    //Ensure ch is between 1 to 2
    if ((ch < 1) || (ch > 2))
        return;
    #else
    return;
    #endif

    //Remember settings
    if (remember) {
        appcfgPutc(APPCFG_PWM_1L + ((ch-1) * 2), val.v[0]);
        appcfgPutc(APPCFG_PWM_1H + ((ch-1) * 2), val.v[1]);
    }

    //Get pointer to CCPRxL register
    p = (BYTE *)pwmCCPRL[ch-1];
    pCCPCON = p - 1;

    //Get upper 8 bits in MSB
    val.Val <<= 6;
    
    //Get content to be writtent to LSB. We do this now so the delay between writing to
    //CCPRxL and CCPxCON is as small as possible
    tmp = (*pCCPCON & 0xcf) | ((val.v[0] >> 2) & 0x30);
    
    // Write the high byte into CCPRxL
    *p = val.v[1];
    
    // Write the low byte into CCPxCON5:4
    *pCCPCON = tmp;
}


/**
 * Get the value of the given PWM channel
 *
 * @param ch PWM channnel, is a value from 1 to 5
 *
 * @return The given PWM channels value
 *
 */
WORD pwmGetValue(BYTE ch) {
    BYTE* p;
    BYTE* pCCPCON;
    WORD_VAL ret;
    
    ret.Val = 0;
    
    #if defined(BRD_SBC65EC)
    //Ensure ch is between 1 to 5
    if ((ch < 1) || (ch > 5))
        return 0;
    #elif defined(BRD_SBC68EC)
    //Ensure ch is between 1 to 2
    if ((ch < 1) || (ch > 2))
        return 0;
    #else
    return;
    #endif

    //Get pointer to CCPRxL register
    p = (BYTE *)pwmCCPRL[ch-1];
    pCCPCON = p - 1;

    //Get upper 8 bits (bits 2 to 9) of 10-bit value
    ret.Val = ((WORD)*p) << 2;

    //Get lower 2 bits of 10-bit value
    ret.v[0] |= ((*pCCPCON & 0x30) >> 4);
    
    return ret.Val;
}


/**
 * Sets the PWM frequency to the given value
 *
 * @param freq New frequency, a value from 0-3.
 *             For 8 bit mode:  0 and 1=9.766kHz, 2=39.062kHz, 3=156.25kHz
 *             For 10 bit mode: 0=2.441kHz, 1=9.766kHz, 2 and 3=39.062kHz
 * @param remember If 1, store in EEPROM and remember
 */
void pwmSetFreq(BYTE freq, BOOL remember) {
    BYTE f;
    
    f = freq;

    //Remember settings
    if (remember) {
        appcfgPutc(APPCFG_PWM_FREQ, f);
    }
    
    //PWM is in 10 bit mode
    if (pwmIs10BitMode() == TRUE) {
        //T2CON<0:1> For 10 bit mode: 3 and 2 =2.441kHz, 1=9.766kHz,  0=39.062kHz
        
        //Input parameter 3 must be adjusted to 0
        if (f == 3) {
            f = 0;
        }
        //Convert f from 0-2 to 2-0 value
        else {
            //If f=3, then result will be 3
            f = 2 - f;
        }
    }
    //PWM is in 8 bit mode
    else {
        //T2CON<0:1> For 8 bit mode:  3 and 2=9.766kHz, 1=39.062kHz, 0=156.25kHz
        
        //Input parameter 0 must be adjusted to 1, both should set T2CON<0:1> to 2
        if (f == 0) {
            f = 2;
        }
        //Convert f from 0-2 to 2-0 value
        else {
            f = 3 - f;
        }
    }

    f &= 0x03;      //Ensure only bits 0 and 1 are set
    
    T2CON &= ~0x03; //Clear bits 0 and 1

    //Configure TMR2 prescaler (bit 0 and 1), is used for PWM module
    //---- --xx - Prescaler is APPCFG_PWM_FREQ 
    T2CON |= f;
}


/**
 * Gets a constant value representing the current PWM frequency
 *
 * @return Frequency constant, a value from 0-3.
 *          For 8 bit mode:  1=9.766kHz, 2=39.062kHz, 3=156.25kHz
 *          For 10 bit mode: 0=2.441kHz, 1=9.766kHz, 2=39.062kHz
 */
BYTE pwmGetFreq(void) {

    BYTE t2con01;
    
    //Get bits 0 and 1 of T2CON
    t2con01 = T2CON & 0x03;

    //Adjust from 3 to 2
    if (t2con01 == 3) {
        t2con01--;
    }
    
    //PWM is in 10 bit mode
    if (pwmIs10BitMode()) {
        //T2CON<0:1> For 10 bit mode: 3 and 2 =2.441kHz, 1=9.766kHz,  0=39.062kHz
        
        return 2 - t2con01;
    }

    //PWM is in 8 bit mode

    //T2CON<0:1> For 8 bit mode:  3 and 2=9.766kHz, 1=39.062kHz, 0=156.25kHz
        
    return 3 - t2con01;
}


/**
 * Enables/Disables given PWM channels
 *
 * @param chan Bits 0-4 indicate if PWM channels 1-5 must be enabled or disabled
 * @param remember If 1, store in EEPROM and remember
 */
void pwmEnable(BYTE chan, BOOL remember) {

    //Remember settings
    if (remember) {
        appcfgPutc(APPCFG_PWM_ENABLE, chan);
    }

    //PIC18F6680 Errata:
    //CCP2CON<3:0> value of ‘1110’ or ‘1111’ produces incorrect PWM output. Use 110x in stead!

    //Enable PWM 1
    if (chan & 0x01) {
        //Configure PWM 1
        //xxxx 1100 - PWM mode
        //xx-- xxxx - Two LSB bits of PWM duty cycle - DON'T CHANGE!
        //00xx xxxx - Single output PWM (only valid for enhanced PWM modules)
        CCP1CON |= 0b00001100;  //PWM Mode
    }
    else {
        CCP1CON &= 0b11110000;   //Disable CCP1 Module
    }

    //Enable PWM 2
    if (chan & 0x02) {
        //Configure PWM 2
        //xxxx 1100 - PWM mode
        //xx-- xxxx - Two LSB bits of PWM duty cycle - DON'T CHANGE!
        //00xx xxxx - Single output PWM (only valid for enhanced PWM modules)
        CCP2CON |= 0b00001100;   //PWM Output
    }
    else {
        CCP2CON &= 0b11110000;   //Disable CCP2 Module
    }

    //Only SBC65EC has PWM channels 3 and 4
    #if defined(BRD_SBC65EC)
    //Enable PWM 3
    if (chan & 0x04) {
        //Configure PWM 3
        //xxxx 1100 - PWM mode
        //xx-- xxxx - Two LSB bits of PWM duty cycle - DON'T CHANGE!
        //00xx xxxx - Single output PWM (only valid for enhanced PWM modules)
        CCP3CON |= 0b00001100;   //PWM Output
        CCP3CON &= 0b00111100;   //Single Output, only required for enhanced PWM modules
    }
    else {
        CCP3CON &= 0b11110000;   //Disable CCP3 Module
    }

    //Enable PWM 4
    if (chan & 0x08) {
        //Configure PWM 4
        //xxxx 1100 - PWM mode
        //xx-- xxxx - Two LSB bits of PWM duty cycle - DON'T CHANGE!
        //00xx xxxx - Single output PWM (only valid for enhanced PWM modules)
        CCP4CON |= 0b00001100;
        CCP4CON &= 0b00111100;   //Single Output, only required for enhanced PWM modules
    }
    else {
        CCP4CON &= 0b11110000;   //Disable CCP4 Module
    }
    #endif
    
    //If any PWM module is enabled, TMR2 has to be enabled
    if (chan != 0) {
        T2CON_TMR2ON = 1;     //Turn TMR2 on
    }
    else {
        T2CON_TMR2ON = 0;     //Turn TMR2 off
    }
}


/**
 * Configure the PWM ports with default power up values
 */
void appcfgPWM(void)
{
    WORD_VAL val;   //PWM Value

    //If NO PWM modules are enabled, return. No modifications are made to any CPU
    //registers! This allows the user to use them in his/her code.
    if (appcfgGetc(APPCFG_PWM_ENABLE) == 0) {
        return;
    }
    
    //PIC18F6627 Errata! TMR4 can NOT be used for PWM4 and PWM5! Use TMR2
    
    //Configure Timer2 to be used as the PWM clock
    //-x-- x--- = 00 = Timer1 and Timer2 are the clock sources for ECCP1, ECCP2, ECCP3, CCP4 and CCP5
    T3CON &= 0b10110111;


    //When in 10 bit mode, we must set PR2 to 255 to be able to get a duty cycle range from 0 to 100%.
    //When in 10 bit mode, the PWM value is written to the CCPRxL register AND CCPxCON<5:4>
    //PWM Frequency can be the following values when in 10-bit mode:
    //Prescaler = 1:16 = 2.441 kHz
    //Prescaler = 1:4  = 9.766 kHz
    //Prescaler = 1:1  = 39.062 kHz

    //When in 8 bit mode, we must set PR2 to 63 to be able to get a duty cycle range from 0 to 100%.
    //When in 8 bit mode, the PWM value is written to CCPRxL<6:0> AND CCPxCON<5:4>
    //PWM Frequency can be the following values when in 8-bit mode:
    //Prescaler = 1:16 = 9.766 kHz
    //Prescaler = 1:4  = 39.062 kHz
    //Prescaler = 1:1  = 156.25 kHz

    //PWM is in 10 bit mode
    if (appcfgGetc(APPCFG_PWM_FLAGS) & 0x01) {
        //Set PWM period - is 255 for 10 bit mode.
        // = (PR2 + 1) x Tosc x 4 x (TMR2 prescaler)
        // = 256 x 25ns x 4 x (TMR2 prescaler)
        // = 256 x 100ns x (TMR2 prescaler)
        PR2 = 255;
    }
    //PWM is in 8 bit mode
    else {
        //Set PWM period - is 63 for 8 bit mode.
        // = (PR2 + 1) x Tosc x 4 x (TMR2 prescaler)
        // = 64 x 25ns x 4 x (TMR2 prescaler)
        // = 64 x 100ns x (TMR2 prescaler)
        PR2 = 63;
    }
    
    //Configure frequency
    pwmSetFreq(appcfgGetc(APPCFG_PWM_FREQ), FALSE);

    //Configure PWM Modules
    val.v[0] = appcfgGetc(APPCFG_PWM_1L);
    val.v[1] = appcfgGetc(APPCFG_PWM_1H);
    pwmSetValue(1, val, FALSE);

    val.v[0] = appcfgGetc(APPCFG_PWM_2L);
    val.v[1] = appcfgGetc(APPCFG_PWM_2H);
    pwmSetValue(2, val, FALSE);

    //Only SBC65EC has PWM channels 3 and 4
    #if defined(BRD_SBC65EC)
    val.v[0] = appcfgGetc(APPCFG_PWM_3L);
    val.v[1] = appcfgGetc(APPCFG_PWM_3H);
    pwmSetValue(3, val, FALSE);

    val.v[0] = appcfgGetc(APPCFG_PWM_4L);
    val.v[1] = appcfgGetc(APPCFG_PWM_4H);
    pwmSetValue(4, val, FALSE);
    #endif

    pwmEnable(appcfgGetc(APPCFG_PWM_ENABLE), FALSE);
}


/**
 * Configure the Expansion Board
 */
void appcfgXboard(void)
{
    if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_MXD2R) {
        mxd2rInit();
    }
    else if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
        ior5eInit();
    }
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
ROM BYTE eeConfigArray[] = {
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
    #if defined(DEMO_MODE)
    //xxxx xxx0 = DHCP Disabled
    0x00,                       //Offset 22
    #else
    //xxxx xxx1 = DHCP Enabled
    0x01,                       //Offset 22
    #endif
    
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
    //xxxx xxx1 = Enabled
    0x01,
    //USART2_CFG.baud
    BAUD_57600,
    //USART2_CFG.cfg
    //xxxx xxx0 = Not Enabled by default
    0,  //Not enabled by default
    //Port Direction Configuration - 7 bytes for port A, B, C, D, E, F and G. A 0 bit=output, 1 bit=input
    0b11111111, //Port A - All inputs   - Offset 35
    0b11111111, //Port B - All inputs
    0b11111111, //Port C - All inputs
    0b11111111, //Port D - All inputs
    0b11111111, //Port E - All inputs
    0b11111111, //Port F - All inputs   - Offset 40
    0b11111111, //Port G - All inputs
    //Port Default Values - 7 bytes for port A, B, C, D, E, F and G. Gived default values for output ports
    0b00000000, //Port A - All 0s
    0b00000000, //Port B - All 0s
    0b00000000, //Port C - All 0s
    0b00000000, //Port D - All 0s       - Offset 45
    0b00000000, //Port E - All 0s
    0b00000000, //Port F - All 0s
    0b00000000, //Port G - All 0s       - Offset 48
    
    #if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
        //APPCFG_ADCON1 - adcon1            - Offset 49 = 0x31
        // xxxx 0010 - A0, A1, A2, A3 and A5 = ADC Channels
        // 0xxx xxxx - Left justified
        0x02,
        //APPCFG_ADCON0_2 - adcon0          - Offset 50 = 0x32
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
    //Username - Is a null terminated string. Max 8 char + null termination char
    'a',                                //Offset = 53
    'd',                                //Offset = 54
    'm',                                //Offset = 55
    'i',                                //Offset = 56
    'n',                                //Offset = 57
    '\0',                               //Offset = 58
    '\0',                               //Offset = 59
    '\0',                               //Offset = 60
    '\0',                               //Offset = 61
    //Password - Is a null terminated string. Max 8 char + null termination char
    'p',                                //Offset = 62
    'w',                                //Offset = 63
    '\0',                               //Offset = 64
    '\0',                               //Offset = 65
    '\0',                               //Offset = 66
    '\0',                               //Offset = 67
    '\0',                               //Offset = 68
    '\0',                               //Offset = 69
    '\0',                               //Offset = 70


    //NetBIOS Name - Is a 16 character name, all in uppercase. All unused characters must be spaces! Must end with 0!
    'M',                                // Offset = 71
    'X',                                // Offset = 72
    #if defined(DEMO_MODE)
    'D',                                // Offset = 73
    'E',                                // Offset = 74
    'M',                                // Offset = 75
    'O',                                // Offset = 76
    ' ',                                // Offset = 77
    #else
    'B',                                // Offset = 73
    'O',                                // Offset = 74
    'A',                                // Offset = 75
    'R',                                // Offset = 76
    'D',                                // Offset = 77
    #endif
    ' ',                                // Offset = 78
    ' ',                                // Offset = 79
    ' ',                                // Offset = 80
    ' ',                                // Offset = 81
    ' ',                                // Offset = 82
    ' ',                                // Offset = 83
    ' ',                                // Offset = 84
    ' ',                                // Offset = 85
    0,                                  // Offset = 86
    
    // Primary DNS server. First entry is MSB byte of IP address, for example in "192.168.1.0" it will be 192
    MY_DEFAULT_DNS_BYTE1,               // Offset = 87
    MY_DEFAULT_DNS_BYTE2,               // Offset = 88
    MY_DEFAULT_DNS_BYTE3,               // Offset = 89
    MY_DEFAULT_DNS_BYTE4,               // Offset = 90
    
    // UDP Command Port
    (BYTE)DEFAULT_CMD_UDPPORT,          // Offset = 91
    (BYTE)(DEFAULT_CMD_UDPPORT>>8),     // Offset = 92

    //UDP reserved bytes
    0,                                  // Offset = 93
    0,                                  // Offset = 94
    0,                                  // Offset = 95
    0,                                  // Offset = 96
    0,                                  // Offset = 97

    //Command Flags
    //xxxxxxx1 = UDP Commands Enabled
    //xxxxxx0x = Authentication required for UDP Commands
    0b00000001,                         // Offset = 98

    // PWM Channel 1-8 Enable Register. 0=off, 1=on. All Off by default
    0,                                  // Offset = 99
    
    // PWM Frequency for all channels.
    //For 8 bit mode:  0 and 1=9.766kHz, 2=39.062kHz, 3=156.25kHz
    //For 10 bit mode: 0=2.441kHz, 1=9.766kHz, 2 and 3=39.062kHz
    2,                                  // Offset = 100
    
    //PWM Flags
    //---- ---x = Configures for 8 or 10 bit PWM. 0=8 bit, 1=10 bit
    1,                                  // Offset = 101

    // PWM Channel 1 (C1) default value (10 bit value)
    (BYTE)0,                            // Offset = 102
    (BYTE)(0>>8),                       // Offset = 103

    // PWM Channel 2 (C2) default value (10 bit value)
    (BYTE)0,                            // Offset = 104
    (BYTE)(0>>8),                       // Offset = 105

    // PWM Channel 3 (G0) default value (10 bit value)
    (BYTE)0,                            // Offset = 106
    (BYTE)(0>>8),                       // Offset = 107

    // PWM Channel 4 (G3) default value (10 bit value)
    (BYTE)0,                            // Offset = 108
    (BYTE)(0>>8),                       // Offset = 109

    // PWM Channel 5 (G4) default value (10 bit value)
    (BYTE)0,                            // Offset = 110
    (BYTE)(0>>8),                       // Offset = 111

    //UDP reserved bytes
    0,                                  // Offset = 112
    0,                                  // Offset = 113

    //Web Server Flags                  // Offset = 114
    //xxxxxxx1 = Authentication required for pages that start with 'x' character.
    //xxxxxx0x = Authentication required for ALL pages.
    //xxxxx1xx = Authentication required for HTTP Get Method.
    //xxxx0xxx = Authentication required for all Dynamic files.
    //xxx1xxxx = Authentication required for all CGI files.
    //xx1xxxxx = Authentication required to display secure tags
    0b00110101,
    
    //Web Server reserved bytes         // Offset = 115
    0,

    //HTTP Server Port, the default HTTP Server Port is
    (BYTE)DEFAULT_HTTPSRVR_PORT,        // Offset = 116
    (BYTE)(DEFAULT_HTTPSRVR_PORT>>8),   // Offset = 117
    
    // Serial LCD Display address
    (BYTE)LCD2S_DEFAULT_I2C_ADR,        // Offset = 118
    
    //LCD Reserved Bytes
    0,                                  // Offset = 119
    0,                                  // Offset = 120
    0,                                  // Offset = 121
    0,                                  // Offset = 122
    0,                                  // Offset = 123

    //UDP Event Port
    (BYTE)DEFAULT_EVENT_UDPPORT,        // Offset = 124
    (BYTE)(DEFAULT_EVENT_UDPPORT>>8),   // Offset = 125

    //UDP Event Flags
    0,                                  // Offset = 126

    //Event Reserved bytes
    0,                                  // Offset = 127
    0,                                  // Offset = 128
    0,                                  // Offset = 129
    0,                                  // Offset = 130
    0,                                  // Offset = 131
    0,                                  // Offset = 132
    0,                                  // Offset = 133
    0,                                  // Offset = 134

    //Expansion Board Type, is a XBRD_TYPE_XXX define
    XBRD_TYPE_NONE,                     // Offset = 135

    //Expansion Board Analog Input 1-8 input range, 0=0V to 5V, 1=0V to 28V</b> NOT USED IN CODE YET!!!!
    0,                                  // Offset = 136
    
    //Expansion Board reserved bytes
    0,                                  // Offset = 137
    0,                                  // Offset = 138
    0,                                  // Offset = 139
    0,                                  // Offset = 140
    0,                                  // Offset = 141
    0,                                  // Offset = 142
    0,                                  // Offset = 143
    0                                   // Offset = 144
};

//Fill up remaining space so that eeBusConfigArray starts at EEPROM address 256
ROM BYTE eeconfigArrayFill[256-sizeof(eeConfigArray)] = {0};

ROM BYTE eeBusConfigArray[] = {
    //IMPORTANT All 8 buses TX and RX buffer sizes must be located after each other
    BUS_SER1_TXBUFSIZE,     //Serial Bus 1 TX buffer size
    BUS_SER1_RXBUFSIZE,     //Serial Bus 1 RX buffer size
    BUS_SER2_TXBUFSIZE,     //Serial Bus 2 TX buffer size
    BUS_SER2_RXBUFSIZE,     //Serial Bus 2 RX buffer size
    BUS_I2C1_TXBUFSIZE,     //Serial I2C 1 TX buffer size
    BUS_I2C1_RXBUFSIZE,     //Serial I2C 1 RX buffer size
    BUS_SPI1_TXBUFSIZE,     
    BUS_SPI1_RXBUFSIZE,
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved
    0,                                              //Reserved

    // UDP Port 1
    (BYTE)DEFAULT_BUS_UDP1PORT,
    (BYTE)(DEFAULT_BUS_UDP1PORT>>8),

    //UDP Port 1 Flags
    //xxxxxxx1 = UDP Port 1 Enabled
    1,

    // UDP Port 2
    (BYTE)DEFAULT_BUS_UDP2PORT,
    (BYTE)(DEFAULT_BUS_UDP2PORT>>8),

    //UDP Port 2 Flags
    //xxxxxxx0 = UDP Port 2 Disabled
    0,

    // BUS that UDP Port 1 is linked to, 0xff is non
    0xff,

    // BUS that UDP Port 2 is linked to, 0xff is non
    0xff,

    0x11,
    0x22,
    0x33,
    0x99,
    
};
