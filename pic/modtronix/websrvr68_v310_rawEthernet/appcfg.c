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
#include "serint.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD0, msgCode)

/////////////////////////////////////////////////
//Uncomment following lines if BAUD rate is statically set. They will calculate SPBRG1 and SPBRGH1
//USART1 is used in 16 bit baud rate generator mode, BRGH high, BRG16 high
#define BAUD_RATE1       (57600)    // bps
#define SPBRG1_VAL   ((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) & 0xff)
#define SPBRGH1_VAL   (((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) >> 8) & 0xff)
#if SPBRG1_VAL > 255
    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif


/////////////////////////////////////////////////
//Variables
APP_CONFIG AppConfig;   //This is used by other stack elements.

//Array containing converted ADC values
#if defined( APP_USE_ADC8 )
BYTE AdcValues[ ADC_CHANNELS ];
#elif defined( APP_USE_ADC10 )
WORD AdcValues[ ADC_CHANNELS ];
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
    while( EECON1_WR ) FAST_USER_PROCESS();

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
        //serDisable();  //Disable serial port
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
    

    TRISA = appcfgGetc(APPCFG_TRISA);

    //Ensure following TRISB ports are always inputs when NIC INT0 is enabled:
    #if defined(NIC_DISABLE_INT0)
        val = appcfgGetc(APPCFG_TRISB);
    #else
        //B0 = Input from RTL8019AS
        val = appcfgGetc(APPCFG_TRISB) | 0x01;
    #endif

    TRISB = val;

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    val = appcfgGetc(APPCFG_TRISC) | 0xd8;
    
    TRISC = val;

    //Set PORT G and F direction bits
    TRISF = appcfgGetc(APPCFG_TRISF);
    
    //Ensure following TRISG ports are always inputs:
    //G4 = Is connected to IOCHRDY
    TRISG = appcfgGetc(APPCFG_TRISG) | 0x10;

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
    //---- xxxx = ADC Port Configuration bits
    //--xx ---- = Voltage reference configuration bits
    ADCON1 = appcfgGetc(APPCFG_ADCON01);

    //ADCON2 bits are:
    // xxxx x110 - ADC Conversion clock select
    // xx10 1xxx - ADC Aquasition time select
    // 0xxx xxxx - ADC Result format select bit, 0=Left justified, 1=right justified

    //For 8 bit mode, ensure left adjust is enabled
    #if defined( APP_USE_ADC8 )
        // Clear ADFM bit
        ADCON2 = appcfgGetc( APPCFG_ADCON02 ) & ~0x80;
    #elif defined(APP_USE_ADC10)
        // Set ADFM bit
        ADCON2 = appcfgGetc( APPCFG_ADCON02 ) | 0x80;
    #endif

}

/* String used for encoding to base64 */
    static ROM BYTE* pwmCCPRL[] = {(ROM BYTE *)(&CCPR1L), (ROM BYTE *)(&CCPR2L)};

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
    

    //Ensure ch is between 1 to 2
    if ((ch < 1) || (ch > 2))
        return 0;

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

    // Bootloader
    0,

    //MyMACAddr
    MY_MAC_BYTE1,               //Offset 0
    MY_MAC_BYTE2,
    MY_MAC_BYTE3,
    MY_MAC_BYTE4,
    MY_MAC_BYTE5,
    MY_MAC_BYTE6,

    //System Flags
    //xxxx xxx1 = PLL Enabled
    //xxxx xx1x = Blink LED on B7
    0x03,

    //USART1_CFG.baud
    BAUD_57600,
    //USART1_CFG.cfg
    //xxxx xxx1 = Enabled
    0x01,

    //Port Direction Configuration - 7 bytes for port A, B, C, D, E, F and G. A 0 bit=output, 1 bit=input
    0b11111111, //Port A - All inputs
    0b11111111, //Port B - All inputs
    0b11111111, //Port C - All inputs
    0b11111111, //Port D - All inputs
    0b11111111, //Port E - All inputs
    0b11111111, //Port F - All inputs
    0b11111111, //Port G - All inputs

    //Port Default Values - 7 bytes for port A, B, C, D, E, F and G. Gived default values for output ports
    0b00000000, //Port A - All 0s
    0b00000000, //Port B - All 0s
    0b00000000, //Port C - All 0s
    0b00000000, //Port D - All 0s
    0b00000000, //Port E - All 0s
    0b00000000, //Port F - All 0s
    0b00000000, //Port G - All 0s       -
    

    //APPCFG_ADCON1 - adcon1
    0x03,   //A0 - A5 and F0 - F6 = ADC, no digital inputs
    //APPCFG_ADCON0_2 - adcon2
    0x2e,   // xxxx x110 - Fosc/64 clock = 1.6us @ 40MHz
    // xx10 1xxx - 12 Tad = 12 x 1.6 = 19.2uS conversion time
    // 0xxx xxxx - Left justified

    //SerialNumber
    0xAA,                                   
    0x55,


};

//Fill up remaining space so that eeBusConfigArray starts at EEPROM address 256
ROM BYTE eeconfigArrayFill[256-sizeof(eeConfigArray)] = {0};


