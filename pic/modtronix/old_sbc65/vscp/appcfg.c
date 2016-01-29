/*********************************************************************
 * appcfg.c    -    Application Configuration functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
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

#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif


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


/*********************************************************************
 * Function:        void appcfgInit(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void appcfgInit(void)
{
#if defined(MPFS_USE_EEPROM)
    BYTE c;
    BYTE *p;
#endif

    /////////////////////////////////////////////////
    //Load default configuration into RAM.

    //Network parameters
    AppConfig.MyIPAddr.v[0]     = MY_DEFAULT_IP_ADDR_BYTE1;
    AppConfig.MyIPAddr.v[1]     = MY_DEFAULT_IP_ADDR_BYTE2;
    AppConfig.MyIPAddr.v[2]     = MY_DEFAULT_IP_ADDR_BYTE3;
    AppConfig.MyIPAddr.v[3]     = MY_DEFAULT_IP_ADDR_BYTE4;

    AppConfig.MyMask.v[0]       = MY_DEFAULT_MASK_BYTE1;
    AppConfig.MyMask.v[1]       = MY_DEFAULT_MASK_BYTE2;
    AppConfig.MyMask.v[2]       = MY_DEFAULT_MASK_BYTE3;
    AppConfig.MyMask.v[3]       = MY_DEFAULT_MASK_BYTE4;

    AppConfig.MyGateway.v[0]    = MY_DEFAULT_GATE_BYTE1;
    AppConfig.MyGateway.v[1]    = MY_DEFAULT_GATE_BYTE2;
    AppConfig.MyGateway.v[2]    = MY_DEFAULT_GATE_BYTE3;
    AppConfig.MyGateway.v[3]    = MY_DEFAULT_GATE_BYTE4;

    AppConfig.MyMACAddr.v[0]    = MY_DEFAULT_MAC_BYTE1;
    AppConfig.MyMACAddr.v[1]    = MY_DEFAULT_MAC_BYTE2;
    AppConfig.MyMACAddr.v[2]    = MY_DEFAULT_MAC_BYTE3;
    AppConfig.MyMACAddr.v[3]    = MY_DEFAULT_MAC_BYTE4;
    AppConfig.MyMACAddr.v[4]    = MY_DEFAULT_MAC_BYTE5;
    AppConfig.MyMACAddr.v[5]    = MY_DEFAULT_MAC_BYTE6;

    //Flags
#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    AppConfig.Flags.bIsDHCPEnabled = TRUE;
#else
    AppConfig.Flags.bIsDHCPEnabled = FALSE;
#endif
    AppConfig.Flags.bPLLOn = TRUE;          //Todo - make PLL software configurable
    AppConfig.Flags.bFreezeADCBuf = FALSE;  //AdcValues buffer can be updated

    //USART Parameters
    AppConfig.USART1Cfg.baud = BAUD_57600;
    AppConfig.USART2Cfg.baud = BAUD_57600;

    //TRIS Port directions
    AppConfig.PortCfg.trisa = 0xff;
    AppConfig.PortCfg.trisb = 0xbf; //B6 is configured as an output
    AppConfig.PortCfg.trisc = 0xff;
    AppConfig.PortCfg.trisf = 0xff;

    //ADC
    AppConfig.ADCCfg.adcon1 = 0x0a;     // A0 - A5 = ADC, rest = digital inputs
    AppConfig.ADCCfg.adcon2 = 0x2e;     // xxxx x110 - Fosc/64 clock = 1.6us @ 40MHz
                                        // xx10 1xxx - 12 Tad = 12 x 1.6 = 19.2uS conversion time
                                        // 0xxx xxxx - Left justified


    /////////////////////////////////////////////////
    // - If EEPROM contains config data (first byte = 0x55), load it
    // - If EEPROM contains no data, save default data to EEPROM
#if defined(MPFS_USE_EEPROM)
    p = (BYTE*)&AppConfig;

    //Read first byte of EEPROM
    XEEBeginRead(EEPROM_CONTROL, 0x00);
    c = XEERead();
    XEEEndRead();

    //Load configuration from EEPROM (contains config if first byte = 0x55)
    if ( c == 0x55 )
    {
        XEEBeginRead(EEPROM_CONTROL, 0x01);
        for ( c = 0; c < sizeof(AppConfig); c++ )
            *p++ = XEERead();
        XEEEndRead();
    }
    //Write configuration to EEPROM
    else {
        appcfgSave();
    }
#endif
}

#if defined(MPFS_USE_EEPROM)
void appcfgSave(void)
{
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;
    XEEBeginWrite(EEPROM_CONTROL, 0x00);
    XEEWrite(0x55);
    for ( c = 0; c < sizeof(AppConfig); c++ )
    {
        XEEWrite(*p++);
    }

    XEEEndWrite();
}


/*
 * Configure USART with AppConfig data
 */
void appcfgUSART(void)
{
    WORD w;

    //Initialize USART1 Control registers
    TXSTA1 = 0b00100100;     //High BRG speed
    RCSTA1 = 0b10010000;
    BAUDCON1 = 0b00001000;

#if (CLOCK_FREQ == 40000000)
    //Get USART BAUD rate setting
    switch (AppConfig.USART1Cfg.baud)
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
    
    SPBRG1 =  w & 0xff;
    SPBRGH1 = (w >> 8) & 0xff;
#else
#error "appcfgUSART() does not support selected clock frequency"
#endif
}

/*
 * Configure the CPU I/O ports
 */
void appcfgCpuIO(void)
{
    //Ensure following TRISB ports are always inputs:
    //B0 = Input from RTL8019AS
    AppConfig.PortCfg.trisb |= 0x01;

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    AppConfig.PortCfg.trisc |= 0xd8;

    //Set PORT A,B,D and F direction bits
    TRISA = AppConfig.PortCfg.trisa;
    TRISB = AppConfig.PortCfg.trisb;
    TRISC = AppConfig.PortCfg.trisc;
    TRISF = AppConfig.PortCfg.trisf;
}

/*
 * Configure the ADC unit
 */
void appcfgADC(void)
{
    //For 8 bit mode, ensure left adjust is enabled
#if defined(APP_USE_ADC8)
    AppConfig.ADCCfg.adcon2 &= ~0x80;	// Clear ADFM bit
#elif defined(APP_USE_ADC10)
    AppConfig.ADCCfg.adcon2 |= 0x80;   	// Set ADFM bit
#endif

    ADCON1 = AppConfig.ADCCfg.adcon1;
    ADCON2 = AppConfig.ADCCfg.adcon2;
}


/*
 * Configure PWM
 */
void appcfgPWM(void)
{
	// Timer 2 is used for PWM
	// Output is on pin PC2
	
	// 2.44kHz 14-bit resolution
	PR2 = 0xff;	// PWM Period
	
	// Duty cycle = 0
	CCP1CON = 0x0c; 
	CCPR1L = 0x00;	
	
	// Start the timer
	T2CON = 0x07;
	
	// RC2 is output now
	AppConfig.PortCfg.trisc &= 0xfb;
	TRISC = AppConfig.PortCfg.trisc;
}
	
#endif
