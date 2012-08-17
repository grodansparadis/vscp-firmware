/*****************************************************************************
 *
 *              Main.c -- user main program
 *
 *****************************************************************************
 * FileName:        HardwareProfile.c
 * Dependencies:
 * Processor:       PIC18F
 * Compiler:        C18 02.20.00 or higher
 *                  PICC 9.50.00 or higher
 *                  C30 02.03.00 or higher
 * Linker:          MPLINK 03.40.00 or higher
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2007-2008 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy and distribute 
 * Software only when embedded on a Microchip microcontroller or digital 
 * signal controller and used with a Microchip radio frequency transceiver, 
 * which are integrated into your product or third party product (pursuant 
 * to the terms in the accompanying license agreement).   
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
 * PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
 * LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
 * CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
 * DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
 * ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
 * LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
 * TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
 * NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *****************************************************************************/

#include "Compiler.h"
#include ".\P2P\SymbolTime.h"
#include "P2PDefs.h"
#include ".\P2P\P2P.h"

#if defined(__18F4620)
        #pragma romdata CONFIG1H = 0x300001
        const rom unsigned char config1H = 0b00000110;      // HSPLL oscillator
    
        #pragma romdata CONFIG2L = 0x300002
        const rom unsigned char config2L = 0b00011111;      // Brown-out Reset Enabled in hardware @ 2.0V, PWRTEN disabled
    
        #pragma romdata CONFIG2H = 0x300003
        const rom unsigned char config2H = 0b00010000; //0b00001010;      // HW WD disabled, 1:32 prescaler
    
        #pragma romdata CONFIG3H = 0x300005
        const rom unsigned char config3H = 0b10000000;      // PORTB digital on RESET
    
        #pragma romdata CONFIG4L = 0x300006
        const rom unsigned char config4L = 0b10000001;      // DEBUG disabled,
                                                            // XINST disabled
                                                            // LVP disabled
                                                            // STVREN enabled
#elif defined(__PIC24F__)
	// Explorer 16 board
	_CONFIG2(FNOSC_PRI & POSCMOD_XT)		// Primary XT OSC with 4x PLL
	_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & WDTPS_PS2)		// JTAG off, watchdog timer off
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
	// Explorer 16 board
	_FOSCSEL(FNOSC_PRI);    //primary osc
	_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
	_FWDT(FWDTEN_OFF & WDTPOST_PS2)				// Disable Watchdog timer
	// JTAG should be disabled as well
#elif defined(__dsPIC30F__)
	// dsPICDEM 1.1 board
	_FOSC(XT_PLL16)					// XT Osc + 16X PLL
	_FWDT(WDT_OFF & WDTPOST_PS2)					// Disable Watchdog timer
	_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
#elif defined(__PIC32MX__)
    #define PIC32MX_SPI1_SDO_SCK_MASK_VALUE      (0x00000140)
    #define PIC32MX_SPI1_SDI_MASK_VALUE          (0x00000080)
    //#define PIC32MX_INT2_MASK_VALUE              (0x00000010)
    #define PIC32MX_INT1_MASK_VALUE              (0x00000008)
    /* MAX SPI CLOCK FREQ SUPPORTED FOR MIWI TRANSCIEVER */
    #define MAX_SPI_CLK_FREQ_FOR_P2P             (20000000)

    #pragma config FPLLMUL = MUL_16, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
    #pragma config POSCMOD = XT, FNOSC = PRIPLL, FPBDIV = DIV_4, WDTPS = PS1024

#endif

#define DEBOUNCE_TIME 0x00008FFF

BOOL PUSH_BUTTON_pressed;
TICK PUSH_BUTTON_press_time;

/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board for the PICDEM-z
 *                  MRF24J40 usage 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize P2P stack or any other function that
 *                  operates on the stack
 ********************************************************************/

void BoardInit(void)
{
    #if defined(PICDEMZ)
        WDTCONbits.SWDTEN = 0; //disable WDT
        
        // Switches S2 and S3 are on RB5 and RB4 respectively. We want interrupt-on-change
        INTCON = 0x00;
        
        // There is no external pull-up resistors on S2 and S3. We will use internal pull-ups.
        // The MRF24J40 is using INT0 for interrupts
        // Enable PORTB internal pullups
        INTCON2 = 0x00;
        INTCON3 = 0x00;
    
        // Make PORTB as input - this is the RESET default
        TRISB = 0xff;
    
        // Set PORTC control signal direction and initial states
        // disable chip select
        LATC = 0xfd;
    
        // Set the SPI module for use by Stack
        TRISC = 0xD0;
    
        // Set the SPI module
        SSPSTAT = 0xC0;
        SSPCON1 = 0x20;
    
        // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
        // Make PORTA as digital I/O.
        // The TC77 temp sensor CS is on RA2.
        ADCON1 = 0x0F;
    
        // Deselect TC77 (RA2)
        LATA = 0x04;
    
        // Make RA0, RA1, RA2 and RA4 as outputs.
        TRISA = 0xF8;
        
        PHY_CS = 1;             //deselect the MRF24J40
        PHY_CS_TRIS = 0;        //make chip select an output   
        
        RFIF = 0;               //clear the interrupt flag
    
        RCONbits.IPEN = 1;
        
        INTCON2bits.INTEDG0 = 0;
    #elif defined(EXPLORER16)
        #ifdef __PIC32MX__
    
           unsigned int pbFreq;
    
           /* Clear SPI1CON register */
           SPI1CONCLR = 0xFFFFFFFF;
    
           /* Enable SPI1, Set to Master Mode & Set CKE bit : Serial output data changes on transition 
              from active clock state to Idle clock state */
           SPI1CON = 0x00008120;
           
           /* Peripheral Bus Frequency = System Clock / PB Divider */
           pbFreq = (DWORD)CLOCK_FREQ / (1 << mOSCGetPBDIV() );
    
           /* PB Frequency can be maximum 40 MHz */
           if( pbFreq > ( 2 * MAX_SPI_CLK_FREQ_FOR_P2P) )
           {
               {
                   unsigned int SPI_Clk_Freq;
                   
                   unsigned char SPI_Brg = 1;
                   
                   /* Continue the loop till you find SPI Baud Rate Register Value */
                   while(1)
                   {
                      /* SPI Clock Calculation as per PIC32 Manual */
                      SPI_Clk_Freq = pbFreq / ( 2 * ( SPI_Brg + 1) );
                      
                      if( SPI_Clk_Freq <= MAX_SPI_CLK_FREQ_FOR_P2P )
                      {
                          break;
                      }
                   
                      SPI_Brg++;
                   }
           
                  mSpiChnSetBrg(1,SPI_Brg);
               
               }
           }
           else
           {
               /* Set SPI1 Baud Rate */
               mSpiChnSetBrg(1,0);
           }
    
           /* Set the Port Directions of SDO, SDI, Clock & Slave Select Signal */
           mPORTFSetPinsDigitalOut(PIC32MX_SPI1_SDO_SCK_MASK_VALUE);
           mPORTFSetPinsDigitalIn(PIC32MX_SPI1_SDI_MASK_VALUE);
    
           /* Set the INT1 port pin to input */
           mPORTESetPinsDigitalIn(PIC32MX_INT1_MASK_VALUE);
    
           /* Set the Interrupt Priority */
           mINT1SetIntPriority(4);
    
           /* Set Interrupt Subpriority Bits for INT2 */
           mINT1SetIntSubPriority(2);
    
           /* Set INT2 to falling edge */
           mINT1SetEdgeMode(0);
    
           /* Enable Multi Vectored Interrupts */
           INTEnableSystemMultiVectoredInt();    
           
        #else

            SPI1CON1 = 0b0000000100111110;
            SPI1STAT = 0x8000;
        
            INTCON2bits.INT1EP = 1;
        #endif
        
        // Make RB0 as Digital input
        AD1PCFGbits.PCFG2 = 1;
        
        PHY_RESETn = 0;
        PHY_RESETn_TRIS = 0;
        PHY_CS = 1;
        PHY_CS_TRIS = 0;
        PHY_WAKE = 1;
        PHY_WAKE_TRIS = 0;
        LED_1_TRIS = 0;
        LED_2_TRIS = 0;
        PUSH_BUTTON_1_TRIS = 1;
        PUSH_BUTTON_2_TRIS = 1;
        RFIF = 0;
        RFIE = 1;
        
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }
    #else
        #error "Unknown demo board.  Please properly initialize the part for the board."
    #endif
}


/*********************************************************************
 * Function:        BYTE ButtonPressed(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Byte to indicate which button has been pressed. 
 *                  Return 0 if no button pressed.
 *
 * Side Effects:    
 *
 * Overview:        This function check if any button has been pressed
 *
 * Note:            
 ********************************************************************/
BYTE ButtonPressed(void)
{

    TICK tickDifference;
        
    if(PUSH_BUTTON_1 == 0)
    {
        //if the button was previously not pressed
        if(PUSH_BUTTON_pressed == FALSE)
        {
            PUSH_BUTTON_pressed = TRUE;
            PUSH_BUTTON_press_time = TickGet();
            return 1;
        }
    }
    else if(PUSH_BUTTON_2 == 0)
    {
        //if the button was previously not pressed
        if(PUSH_BUTTON_pressed == FALSE)
        {
            PUSH_BUTTON_pressed = TRUE;
            PUSH_BUTTON_press_time = TickGet();
            return 2;
        }
    } 
    else
    {
        //get the current time
        TICK t = TickGet();
        
        //if the button has been released long enough
        tickDifference.Val = TickGetDiff(t,PUSH_BUTTON_press_time);
        
        //then we can mark it as not pressed
        if(tickDifference.Val > DEBOUNCE_TIME)
        {
            PUSH_BUTTON_pressed = FALSE;
        }
    }
    
    return 0;
}


