/**********************************************************************
 * mac.c    -       MAC Module for Microchip TCP/IP Stack
 * Dependencies:    string.h, stacktsk.h,  helpers.h, mac.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *********************************************************************
 * File History
 * 2005-12-21, David Hosken (DH):
 *    - Added doxygen documentation
 *    - Added new defines to optimize code for size or speed
 *    - Some speed and code improvements
 * 2004-07-06, Henrik Maier (HM):
 *    - LAT registers used for port E instead of PORT registers. Seems to be
 *      more appropriate and avoids the side-effect mentioned in
 *      chapter 31.8 Design Tips of "PIC18C Reference Manual".
 * 2004-06-23, Henrik Maier (HM):
 *    - Interrupts disabled for a NIC IOW and IOR bus cycles to assure they
 *      are perfomed within a limited time.
 *    - Macro WAIT_FOR_IOCHRDY introduced to insert Nop's as wait cycles after
 *      IOR or IOW has been asserted. On a hardware w/ IOCHRDY signal the macro
 *      should poll IOCHRDY.
 * 2004-06-22, Henrik Maier (HM):
 *    - Delay in MACRxbufGetHdr replaced with DelayMs, saves some code
 *    - RXSTOP modfied to be end of buffer RAM + 1 page, so we won't waste
 *      one page of buffer RAM
 *    - Useful functions NICPut, NICGet, NICSetAddr made non-static
 *    - MACRxbufGetHdr: Overrun handling modified to match NatSemi AN 877.
 *    - MACRxbufGetHdr: Added sanity check for next packet pointer
 *    - Added DMA finished check to MACPut as pointed out in NatSemi AN 877.
 *      AN 877 states without it serious system errors can happen.
 *      Let's believe this.
 *    - 2 byte FIFO threshold leads to shorter DMA bursts and should help
 *      avoiding potential DMA arbitration conflicts.
 *    - MACInit modified NOT to consider the RST bit, some say not always reliable
 * 2004-06-16, Henrik Maier (HM):
 *    - Need for NIC_CTRL_TRIS removed. All TRIS settings must be done
 *      outside of this module.
 * 2004-05-09, Henrik Maier (HM):
 *    - MACGetOffset can be enabled w/ macro STACK_USE_MAC_GETOFFSET
 *    - Fixed mixup of two diff. constants (60 and 64) in MACPutHeader's
 *      minumum frame length adjustment. Acc. to NIC datasheet must be 64!
 * 2004-04-26, Henrik Maier (HM):
 *    - MACIsLinked compiled only if DHCP is enabled
 *    - MACGetOffset not used: disabled to save code space
 *    - STACK_USE_PORTA macro configures PORT A instead of PORT B
 *    - Support added for NIC address on port A using A0-A3 and A5
 *    - Changed WRITE_NIC_ADDR() macro not write TRIS register
 *    - Clearing INTCON2_RBPU bit removed from NICReset()
 *    - Additional NOP for NIC access can be enabled w/ STACK_USE_NOPS
 * 2003-03-21, Nilesh Rajbharti
 *    - Fixed MACIsTxReady() bug where upon certain bus collision, this would always
 *      return FLASE causing Stack to not handle any incoming packets at all.
 * 2002-05-22, Nilesh Rajbharti - Rev 2.0 (See version.log for detail)
 * 2002-02-09, Nilesh Rajbharti - Cleanup
 * 2000-04-27, Nilesh Rajbharti - Original (Rev 1.0)
 ********************************************************************/

#define THIS_IS_MAC_LAYER

#include <string.h>

#include "debug.h"

#include "net\mac.h"
#include "net\helpers.h"
#include "net\delay.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD5, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD5, msgCode, msgStr)

/////////////////////////////////////////////////
//Do some checks
#if defined(STACK_USE_SLIP)
#error Unexpected module is detected.
#error This file must be linked when SLIP module is not in use.
#endif

#if (DEBUG_MAC > LOG_WARN)
    #if !defined(MAC_CNTR1_3)
    #define MAC_CNTR1_3
    #endif
#endif


/*
 * Hardware interface to NIC.
 */

/* SBC44EC - use port B*/
#if defined(BRD_SBC44EC)
#define NIC_CTRL_TRIS       (TRISE)
#define NIC_RESET_IO        (LATE2)
#define NIC_IOW_IO          (LATE1)
#define NIC_IOR_IO          (LATE0)
#define NIC_ADDR_IO         (LATB)
#define NIC_ISR_RDC_BIT     (PORTD_RD6) // HM: Added to speed up things
#define NIC_DATA_IO         (PORTD)
#define NIC_IOCHRDY         (PORTB_RB5)
#define NIC_IOCHRDY_TRIS    (TRISB_RB5)
#define NIC_DATAPORT        (0x10)
#define NIC_RESET           (0x1f)
/* SBC45EC - use port A*/
#elif defined(BRD_SBC45EC)
#define NIC_CTRL_TRIS       (TRISE)
#define NIC_RESET_IO        (LATE2)
#define NIC_IOW_IO          (LATE1)
#define NIC_IOR_IO          (LATE0)
#define NIC_ADDR_IO         (LATA)
#define NIC_ISR_RDC_BIT     (PORTD_RD6) // HM: Added to speed up things
#define NIC_DATA_IO         (PORTD)
#define NIC_IOCHRDY         (PORTA_RA4)
#define NIC_IOCHRDY_TRIS    (TRISA_RA4)
#define NIC_DATAPORT        (0x20)
#define NIC_RESET           (0x2f)
/* SBC65EC - use port E*/
#elif defined(BRD_SBC65EC)
#define NIC_CTRL_TRIS       (TRISE)
#define NIC_RESET_IO        (LATE7)
#define NIC_IOW_IO          (LATE6)
#define NIC_IOR_IO          (LATE5)
#define NIC_ADDR_IO         (LATE)
#define NIC_ISR_RDC_BIT     (PORTD_RD6) // HM: Added to speed up things
#define NIC_DATA_IO         (PORTD)
#define NIC_IOCHRDY         (PORTG_RG4)
#define NIC_IOCHRDY_TRIS    (TRISG_RG4)
#define NIC_DATAPORT        (0x10)
#define NIC_RESET           (0x1f)
#else
#error "No BRD_XXX defined. Please define board type"
#endif


/* Ethernet definitions.. */
#define MINFRAME  60    /* Minimum length of Ethernet frame, excluding CRC */
#define MINFRAMEC 64    /* Minimum length of Ethernet frame, including CRC */
#define CRCLEN    4
#define MAXFRAME  1514  /* Maximum length of Ethernet frame, excluding CRC */
#define MAXFRAMEC 1518  /* Maximum length of Ethernet frame, including CRC */

/* 8390 Network Interface Controller (NIC) page0 register offsets */
#define CMDR    0x00            /* command register for read & write */
#define PSTART  0x01            /* page start register for write */
#define PSTOP   0x02            /* page stop register for write */
#define BNRY    0x03            /* boundary reg for rd and wr */
#define TPSR    0x04            /* tx start page start reg for wr */
#define TXSTAT  TPSR
#define TBCR0   0x05            /* tx byte count 0 reg for wr */
#define TBCR1   0x06            /* tx byte count 1 reg for wr */
#define ISR     0x07            /* interrupt status reg for rd and wr */
#define RSAR0   0x08            /* low byte of remote start addr */
#define RSAR1   0x09            /* hi byte of remote start addr */
#define RBCR0   0x0A            /* remote byte count reg 0 for wr */
#define RBCR1   0x0B            /* remote byte count reg 1 for wr */
#define RCR     0x0C            /* rx configuration reg for wr */
#define TCR     0x0D            /* tx configuration reg for wr */
#define DCR     0x0E            /* data configuration reg for wr */
#define IMR     0x0F            /* interrupt mask reg for wr */
#define CNTR0   0x0D            /* Counter 0 */
#define CNTR1   0x0E            /* Counter 0 */
#define CNTR2   0x0F            /* Counter 0 */


/* NIC page 1 register offsets */
#define PAR0    0x01            /* physical addr reg 0 for rd and wr */
#define CURRP   0x07            /* current page reg for rd and wr */
#define MAR0    0x08            /* multicast addr reg 0 for rd and WR */

/* NIC page 3 register offsets */
#define RTL9346CR   0x01          /* RTL 9346 command reg */
#define RTLCONF0    0x03            /* RTL config reg 0 */
#define RTLCONF1    0x04            /* RTL config reg 1 */
#define RTLCONF2    0x05            /* RTL config reg 3 */
#define RTLCONF3    0x06            /* RTL config reg 3 */

#define NIC_PAGE_SIZE       (256)

/////////////////////////////////////////////////
//NIC RAM definitions
//When using the RTL8019AS in 8 bit mode, there is 8Kbytes of SRAM available for the user
//to use. They are devided into 32 x 256byte pages, starting at 0x40 and going to 0x5F.
//Of this SRAM, some has to be reserved for the NIC to use as a receive buffer. The
//receive buffer is initialized by setting the PSTART and PSTOP registers with the value
//of RXSTART and RXSTOP.
#define RAMPAGES 0x20               /* Total number of 256-byte RAM pages */
#define TXSTART  0x40               /* Tx buffer start page - NE2000 mode */
#define TXPAGES (MAC_TX_BUFFER_COUNT * (MAC_TX_BUFFER_SIZE/NIC_PAGE_SIZE))
#define RXSTART  (TXSTART+TXPAGES)  /* Rx buffer start page */
#define RXSTOP   0x60               /* Last Rx buffer page + 1 */

//Configure FIFO threshold, smaller threshold leads to shorter DMA bursts
//#define DCRVAL   0x68         /* 8-bit DMA, big-endian, 12 byte FIFO threshold */
//#define DCRVAL   0x48         /* 8-bit DMA, big-endian, 8 byte FIFO threshold */
#define DCRVAL   0x08         /* 8-bit DMA, big-endian, 2 byte FIFO threshold */


#define RXPAGES (RXSTOP - RXSTART)


#if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
    #define WRITE_NIC_ADDR(a)   (NIC_ADDR_IO = a)
//These boards use upper 3 bits of NIC_ADDR_IO for IOWR, IORD and RSTDRV
//Always set IOWR and IORD (bits 5 and 6) and clear RSTDRV
#elif defined(BRD_SBC65EC)
    #define WRITE_NIC_ADDR(a)   (NIC_ADDR_IO = (a | 0x60))
#endif


typedef struct _IEEE_HEADER
{
    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Len;
    BYTE            LSAPControl[3];
    BYTE            OUI[3];
    WORD_VAL        Protocol;
} IEEE_HEADER;

#define ETHER_IP        (0x00)
#define ETHER_ARP       (0x06)


typedef struct _DATA_BUFFER
{
    BYTE Index; //Page of this buffer in NIC's SRAM
    BOOL bFree;
} DATA_BUFFER;

static DATA_BUFFER TxBuffers[(TXPAGES*NIC_PAGE_SIZE)/MAC_TX_BUFFER_SIZE];
#define MAX_DATA_BUFFERS        (sizeof(TxBuffers)/sizeof(TxBuffers[0]))

typedef struct _ETHER_HEADER
{
    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} ETHER_HEADER;


typedef struct _NE_RCR
{
    unsigned int PRX:1;
    unsigned int CRC:1;
    unsigned int FAE:1;
    unsigned int FO:1;
    unsigned int MPA:1;
    unsigned int PHY:1;
    unsigned int DIS:1;
    unsigned int DFR:1;
} NE_RCR;


//This is the data contains in the beginning of a Receive buffer.
// - The first 4 bytes of data is used to give the status of this receive buffer, and to
//   point to the next Ethernet Packet header in the receive buffer
// - The following 14 bytes is the Ethernet header.
typedef struct _NE_PREAMBLE /* 18 bytes long */
{
    NE_RCR Status;
    BYTE NextPacketPointer;
    WORD ReceivedBytes;     //Lenght of received packet - does NOT included first 4 bytes

    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} NE_PREAMBLE;

//If MAC_USE_ACCESSRAM has been defined, we use access RAM variables
//to optiomize speed and code size
#ifdef MAC_USE_ACCESSRAM
    #define MAC_NEAR near

    //Microchip C18 compiler
    #if defined(__18CXX)
        #pragma udata access accessram  //Indicate following data is in Access RAM section
    #endif
#else
    #define MAC_NEAR 
#endif

//MAC Receive Ring Buffer variables
MAC_NEAR BYTE MACCurrRxbuf;   // First page of current MAC Receive Ring Buffer
MAC_NEAR WORD_VAL DMAAddr;    // Current DMA address. This variable keeps track of the Remote DMA RSAR0 and
                        //RSAR1 registers (Remote Start Address). This is the address used by Remote
                        //DMA for read and write commands.

MAC_NEAR BYTE MACCurrTxbuf;

#ifdef MAC_USE_ACCESSRAM
    //Microchip C18 compiler
    #if defined(__18CXX)
        #pragma udata   //Return to normal section
    #endif
#endif


BYTE iochrdyCnt;


#ifdef MAC_CNTR1_3
WORD_VAL cntr0;
WORD_VAL cntr1;
WORD_VAL cntr2;
#endif

#if (DEBUG_MAC >= LOG_WARN)
BYTE cntrSum;
#endif


void NICReset(void);
void NICPut(BYTE reg, BYTE val);
BYTE NICGet(BYTE reg);
void NICSetAddr(WORD addr);

void    MACInit(void)
{
    BYTE i;

    /* SBC45EC - use port B*/
    #if defined(BRD_SBC44EC)
        TRISB = 0xe0;   //RB0-4 are outputs
         //NIC_DISABLE_IOCHRDY can be defined if the MAC should NOT use the IOCHRDY signal on the RTL8019AS chip.
         // - For SBC44EC PIC port pin B5 will be available for user IO. Solder jumper SJ5 must be opened!
        #if !defined(NIC_DISABLE_IOCHRDY)
            NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
        #endif
    /* SBC45EC - use port A*/
    #elif defined(BRD_SBC45EC)
        TRISA = 0xd0;   //RA0-3 and RA5 are outputs
         //NIC_DISABLE_IOCHRDY can be defined if the MAC should NOT use the IOCHRDY signal on the RTL8019AS chip.
         // - For SBC45EC PIC port pin A4 will be available for user IO. Solder jumper SJ5 must be opened!
        #if !defined(NIC_DISABLE_IOCHRDY)
            NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
        #endif
    /* SBC65EC - use port E*/
    #elif defined(BRD_SBC65EC)
        TRISE = 0x00;           //RE0-7 are outputs
         //NIC_DISABLE_IOCHRDY can be defined if the MAC should NOT use the IOCHRDY signal on the RTL8019AS chip.
         // - For SBC65EC and SBC68EC this frees up PIC pin RG4. This pin is currently however not routed to an connectors
        //#if !defined(NIC_DISABLE_IOCHRDY)
            NIC_IOCHRDY = 1;
            NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
        //#endif
    #endif

    //Reset all error counters
    #ifdef MAC_CNTR1_3
    cntr0.Val = 0;
    cntr1.Val = 0;
    cntr2.Val = 0;
    #endif

    #if (DEBUG_MAC >= LOG_WARN)
    cntrSum = 0;
    #endif


    // On Init, all transmit buffers are free.
    for ( i = 0; i < MAX_DATA_BUFFERS; i++ )
    {
        //Set to NIC SRAM page
        TxBuffers[i].Index = TXSTART + (i * (MAC_TX_BUFFER_SIZE/NIC_PAGE_SIZE));
        TxBuffers[i].bFree = TRUE;
    }
    MACCurrTxbuf = 0;

    NICReset();
    DelayMs(5);     // Give RTL8019AS time to load EEPROM values - takes about 2ms
    NICPut(NIC_RESET, NICGet(NIC_RESET));   //Writing to NIC_RESET register caused NIC to reset
    // mimimum Delay of 1.6 ms
    DelayMs(5);     // Give RTL8019AS time to load EEPROM values - takes about 2ms

    //Some documentation states that reset bit is unreliable - only check it for 100ms
    //if ( (NICGet(ISR) & 0x80) != 0 )
    {
        #if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
        //Because the EEPROM read pin is floating on some boards, the registers initialized by the
        //EEPROM will all contain non defined values. Write them with correct values
        //MACInitEepromRegs();  // TEST TEST
        #endif
        
        // Select Page 0 AND issue STOP Command
        NICPut(CMDR, 0x21);
        DelayMs(5);

        // Initialize Data Configuration Register
        NICPut(DCR, DCRVAL);

        // Clear Remote Byte Count Registers
        NICPut(RBCR0, 0);
        NICPut(RBCR1, 0);

        // Initialize Receive Configuration Register
        NICPut(RCR, 0x04);

        // Place NIC in LOOPBACK mode 1
        NICPut(TCR, 0x02);

        // Initialize Transmit buffer queue
        NICPut(TPSR, TxBuffers[MACCurrTxbuf].Index);

        // Initialize Receive Buffer Ring
        NICPut(PSTART, RXSTART);
        NICPut(PSTOP, RXSTOP);
        NICPut(BNRY, RXSTART);  //Receive buffer GET pointer

        // Initialize Interrupt Mask Register
        // Clear all status bits
        NICPut(ISR, 0xff);
        // No interrupt enabled.
        NICPut(IMR, 0x00);

        // Select Page 1
        NICPut(CMDR, 0x61);

        // Initialize Physical Address Registers
        NICPut(PAR0, MY_MAC_BYTE1);
        NICPut(PAR0+1, MY_MAC_BYTE2);
        NICPut(PAR0+2, MY_MAC_BYTE3);
        NICPut(PAR0+3, MY_MAC_BYTE4);
        NICPut(PAR0+4, MY_MAC_BYTE5);
        NICPut(PAR0+5, MY_MAC_BYTE6);

        // Initialize Multicast registers
        for ( i = 0; i < 8; i++ )
            NICPut(MAR0+i, 0xff);

        // Initialize CURRent pointer - this is the RX Buffer PUT pointer
        NICPut(CURRP, RXSTART);

        // TCP layer uses this value to calculate window size. Without init the
        // first window size value sent would be wrong if no packet has been received before.
        MACCurrRxbuf = RXSTART;

        // Page 0, Abort Remote DMA and Activate the transmitter.
        NICPut(CMDR, 0x22);

        // Set Normal Mode
        NICPut(TCR, 0x00);
    }

    #if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
    //Because the EEPROM read pin is floating on some boards, the registers initialized by the
    //EEPROM will all contain non defined values. Write them with correct values
    //MACInitEepromRegs();  // TEST TEST
    #endif

    //Disable RTL8019AS interrupts. All INT lines go tri-state, and PIC pin is available to user. The
    //PIC port pin that becomes available is:
    // - For SBC44EC this has no affect - no PIC pins are connected to the interrupt pins
    // - For SBC45EC this has no affect - no PIC pins are connected to the interrupt pins
    // - For SBC65EC and SBC68EC this frees up PIC pin RB0.
    #if (defined(BRD_SBC65EC) || defined(BRD_SBC68EC)) && defined(NIC_DISABLE_INT0)
    NICPut(CMDR, 0xe0);         //Select page 3
    NICPut(RTL9346CR, 0xc0);    //Config register write enable
    NICPut(RTLCONF1, 0);        //Interrupt disable - will cause INT0 to be high impedance.
    NICPut(RTL9346CR, 0x00);    //Normal operating mode
    NICPut(CMDR, 0x20);         //Reset to default = Page 0
    #endif
}


/**
 * Function for performing MAC related tasks
 *
 * @preCondition    Must be called every couple of ms
 */
void MACTask(void)
{
    #if (DEBUG_MAC >= LOG_WARN)
    BYTE tmpSum;
    char buf[5];
    #endif

    #if defined(MAC_CNTR1_3)
    cntr0.Val += NICGet(CNTR0);
    cntr1.Val += NICGet(CNTR1);
    cntr2.Val += NICGet(CNTR2);
    #endif

    #if (DEBUG_MAC >= LOG_WARN)
    tmpSum = LSB(cntr0) + MSB(cntr0) + LSB(cntr1) + MSB(cntr1) + LSB(cntr2) + MSB(cntr2);
    //If any of the error counter have changed, print them out!
    if (cntrSum != tmpSum) {
        debugPutMsg(1);     //@mxd:1:Frame Alignment=%s, CRC=%s, Missed Packets=%s

        itoa(cntr0.Val, buf);
        debugPutString(buf);
        itoa(cntr1.Val, buf);
        debugPutString(buf);
        itoa(cntr2.Val, buf);
        debugPutString(buf);

        cntrSum = tmpSum;
    }
    #endif
}


#if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
/**
 * Initializes registers that should have been initialized with the contents
 * of the EEPROM
 *
 * @preCondition     Must be called after chip is initialized
 *
 */
void MACInitEepromRegs(void) {
        // Select Page 3
        NICPut(CMDR, 0xe0);

        //Because the EEPROM read pin is floating on some boards, the registers initialized by the
        //EEPROM will all contain non defined values. Write them with correct values
        NICPut(RTL9346CR, 0xc0);    //Config register write enable
        NICPut(RTLCONF1, 0x00);     //Interrupt disable
        NICPut(RTL9346CR, 0xc0);    //Config register write enable
        NICPut(RTLCONF2, 0x00);     //TP/CX link test enabled (10BaseT link test is enabled)
        NICPut(RTL9346CR, 0xc0);    //Config register write enable
        NICPut(RTLCONF3, 0x00);     //Not sleep mode, Not powerdown mode
        NICPut(RTL9346CR, 0x00);    //Normal operating mode
 
        //Reset to default = Page 0
        NICPut(CMDR, 0x20);
}
#endif

/**
 * Check if ready for next transmission.
 *
 * @return      TRUE if transmit buffer is empty <br>
 *              FALSE if transmit buffer is not empty
 *
 */
BOOL    MACIsTxReady(void)
{
    // MACCurrTxbuf always points to free buffer, if there is any.
    // If there is none, MACCurrTxbuf will be a in 'Use' state.
    //return TxBuffers[MACCurrTxbuf].bFree;
    // Check to see if previous transmission was successful or not.
    return !(NICGet(CMDR) & 0x04);
}

#if defined STACK_DISABLES_INTS
    #define BEGIN_IO_CYCLE() { INTCON_GIEH = 0; } // Disable interrupts
    #define LEAVE_IO_CYCLE() { INTCON_GIEH = 1; } // Enable interrupts
#else
    #define BEGIN_IO_CYCLE()
    #define LEAVE_IO_CYCLE()
#endif

#if defined(NIC_DISABLE_IOCHRDY)
    #if (CLOCK_FREQ == 40000000)
        #define WAIT_FOR_IOCHRDY() {Nop(); Nop(); Nop(); Nop(); Nop(); Nop();}  // Delay for 600 ns
    #elif (CLOCK_FREQ == 20000000)
        #define WAIT_FOR_IOCHRDY() {Nop(); Nop(); Nop(); }                      // Delay for 600 ns
    #endif
#else
    #define WAIT_FOR_IOCHRDY() {while(NIC_IOCHRDY == 0);}      //Wait for IOCHRDY pin to go high
#endif

#if defined(STACK_USE_FAST_NIC)

    #define FastNICPut(a, b) \
    { \
        WRITE_NIC_ADDR(a); \
        NIC_DATA_IO = (b); \
        BEGIN_IO_CYCLE(); \
        NIC_IOW_IO = 0; \
        WAIT_FOR_IOCHRDY(); \
        NIC_IOW_IO = 1; \
        LEAVE_IO_CYCLE(); \
    }

    //Write again to the same address
    #define FastNICPutAgain(a, b) \
    { \
        NIC_DATA_IO = (b); \
        BEGIN_IO_CYCLE(); \
        NIC_IOW_IO = 0; \
        WAIT_FOR_IOCHRDY(); \
        NIC_IOW_IO = 1; \
        LEAVE_IO_CYCLE(); \
    }

    #define FirstFastNICPut(a, b) \
    { \
        TRISD = 0x00; \
        FastNICPut(a, b); \
    }
#else
    #define FastNICPut(a, b) NICPut(a, b)
    #define FastNICPutAgain(a, b) NICPut(a, b)
    #define FirstFastNICPut(a, b) NICPut(a, b)
#endif

/*
static void WaitForDmaToFinish(void)
{
#if defined(__18CXX)
    overlay
#endif
    BYTE i;

    // Wait for DMA to have finished for sure, needed according to AN877
    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(ISR);
    i = 255;
    while(i--)
    {
       BEGIN_IO_CYCLE();
       NIC_IOR_IO = 0;
       WAIT_FOR_IOCHRDY();
       if (NIC_ISR_RDC_BIT)
          i = 0; // Leave loop
       NIC_IOR_IO = 1;
       LEAVE_IO_CYCLE();
    }
}
*/

/**
 * Write a single byte to the Remote DMA. The byte is written to the current Remote DMA address.
 *
 * @param val Byte to write to Remote DMA
 */
void    MACPut(BYTE val)
{
    //Set the data byte count of remote DMA - is only one byte in this case
    FirstFastNICPut(RBCR0, 1);
    FastNICPut(RBCR1, 0);

    // TODO Datasheet mentions that CMDR bits RD2, RD1 and RD0 must first be written with 001 to ensure correct operation????

    //Remote DMA Write command - after this command, a write to the I/O port will write to the NIC's
    //RAM via Remote DMA
    FastNICPut(CMDR, 0x12);

    FastNICPut(ISR, 0x40);  //Clear DMA bit

    //Write a byte to NIC I/O port. After this command, the Remote DMA address is incremented
    FastNICPut(NIC_DATAPORT, val);

    // TODO Is this needed??
    //WaitForDmaToFinish();

    //Update Remote DMA address pointer
    DMAAddr.Val++;
}

/**
 * Write given array of bytes to NIC's RAM
 *
 * @param val Pointer to byte array of bytes that are to be written to the NIC's RAM
 * @param len Lengh of array given in val
 */
void    MACPutArray(BYTE *val, WORD len)
{
#if defined(__18CXX)
    overlay
#endif
    WORD_VAL length;
    WORD_VAL t;

    t.Val = len + (len & 1);    //Ensure it is an even value. If 1, set to 2, If 2, leave. If 3, set to 4. .....

    FirstFastNICPut(ISR, 0x40); // Clear DMA bit

    //Set the data byte count of remote DMA - is size of array
    FastNICPut(RBCR0, t.byte.LSB);
    FastNICPut(RBCR1, t.byte.MSB);

    //Remote DMA Write command - after this command, a write to the I/O port will write to the NIC's
    //RAM via Remote DMA. After writing "len" bytes, DMA finished bit is set in ISR register
    FastNICPut(CMDR, 0x12);

    //Update Remote DMA address pointer
    DMAAddr.Val += len;

    length.Val = len;
    do {
        while (length.byte.LSB-- > 0) {
            //Write a byte to the I/O port = Writes a byte to RAM via Remote DMA
            FastNICPut(NIC_DATAPORT, *val++);
        }
        length.byte.LSB = 0xff;
    } while (length.byte.MSB-- > 0);

    // TODO Is this needed??
    //WaitForDmaToFinish();
}


/**
 * Reads a single byte via Remote DMA from the current MAC Receive buffer.
 * If the last byte of the RX Buffer was read, this function automatically
 * updates the Remote DMA read address to the first page of the RX Buffer.
 * See PreConditions for more info.
 *
 * @preCondition:    Remote DMA address has to be set up prior to calling this function.
 *                  The Remote DMA registers are NOT configured by this function,
 *                  and simply continue reading from the current "Remote DMA Address". A function
 *                  like MACRxbufGetHdr() can be called prior to this function to configure Remote
 *                  DMA to read the next packet in RX Buffer (situated in Remote DMA RAM)
 *
 * @return      Read byte
 */
BYTE    MACRxbufGet(void)
{
#if defined(__18CXX)
    overlay
#endif
    BYTE b;

    //Configure RBCR (Remote Byte Count Register) for bytes to be read via DMA
    FirstFastNICPut(RBCR0, 1);
    FastNICPut(RBCR1, 0);

    //Last byte of MAC RX Buffer is read wronge by RTL8019AS - Address has to be set again!
    if ((DMAAddr.v[0] == 0xff) && (DMAAddr.v[1] == (RXSTOP-1))) {
        //Set NIC Remote DMA read again - this fixes the RTL8019AS bug!!!!
        NICSetAddr((RXSTOP << 8) - 1);

        //Remote DMA Read command. This causes given (RBCR0 & 1) number of bytes, starting at given
        //address (RSAR0 & 1) set by NICSetAddr() to be read with each I/O read cycle.
        FastNICPut(CMDR, 0x0a); 

        b = NICGet(NIC_DATAPORT);   //Read I/O port

        //Reset to first byte of MAC RX Buffer - this function also updates DMAAddr
        //This fixes the RTL8019AS bug where the address point is not updated to first byte of Receive Buffer
        NICSetAddr(RXSTART << 8);
    }
    else {
        //Remote DMA Read command. This causes given (RBCR0 & 1) number of bytes, starting at given
        //address (RSAR0 & 1) set by NICSetAddr() to be read with each I/O read cycle.
        FastNICPut(CMDR, 0x0a); 

        //Read a byte from the I/O port
        TRISD = 0xff;   //Input
        WRITE_NIC_ADDR(NIC_DATAPORT);
        BEGIN_IO_CYCLE();
        NIC_IOR_IO = 0;
        WAIT_FOR_IOCHRDY();
        b = NIC_DATA_IO;
        NIC_IOR_IO = 1;
        LEAVE_IO_CYCLE();

        //The above I/O port operation updated the Remote DMA address
        DMAAddr.Val++;
    }

    return (b);
}

/**
 * Reads the given amount of bytes via Remote DMA from the current MAC Receive buffer.
 * If the end of the RX Buffer is reached, this function automatically rolls over to
 * the first page of the RX Ring Buffer. See PreConditions above for more info.
 *
 * @preCondition    Remote DMA address has to be set up prior to calling this function.
 *                  The Remote DMA registers are NOT configured by this function,
 *                  and simply continue reading from the current "Remote DMA Address". A function
 *                  like MACRxbufGetHdr() can be called prior to this function to configure Remote
 *                  DMA to read the next packet in RX Buffer (situated in Remote DMA RAM)
 *
 * @param len       Length of array to be read
 * @param val       Buffer to read packet into
 *
 * @return          Number of bytes read
 *
 */
WORD    MACRxbufGetArray(BYTE *val, WORD len)
{
#if defined(__18CXX)
    overlay
#endif
    WORD_VAL t;

    t.Val = len;

    //Configure RBCR (Remote Byte Count Register) for bytes to be read via DMA
    FirstFastNICPut(RBCR0, t.byte.LSB);
    FastNICPut(RBCR1, t.byte.MSB);

    //Remote DMA Read command. This causes given (RBCR0 & 1) number of bytes, starting at given
    //address (RSAR0 & 1) set by NICSetAddr() to be read with each I/O read cycle.
    FastNICPut(CMDR, 0x0a); 

    //Update DMAAddr for what it will be after all requested bytes are read
    DMAAddr.Val += len;
    if (DMAAddr.v[1] >= RXSTOP) {
        //If end of MAC Receive buffer is reached, roll over to beginning of buffer
        DMAAddr.Val -= (RXPAGES << 8);
    }

    //Read requested number of bytes from I/O port
    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(NIC_DATAPORT);
    while( len-- > 0 )
    {
        BEGIN_IO_CYCLE();
        NIC_IOR_IO = 0;
        WAIT_FOR_IOCHRDY();
        *val++ = NIC_DATA_IO;
        NIC_IOR_IO = 1;
        LEAVE_IO_CYCLE();

    }

    #if (DEBUG_MAC >= LOG_ERROR)
    if (DMAAddr.Val != MACGetNICAddr()) {
        debugPutMsg(2);
    }
    #endif

    return t.Val;
}

/**
 * This function reserves a given transmit buffer and marks it as unavailable. This function
 * is useful for TCP layer where a message would be queued until it is correctly
 * acknowledged by remote host.
 *
 * @param buffer Buffer identifier, is a number from 0-255 identifying the buffer
 */
void MACReserveTxBuffer(BUFFER buffer)
{
    TxBuffers[buffer].bFree = FALSE;
}


/**
 * Discard the given transmit buffer
 *
 * @param buffer Buffer identifier, is a number from 0-255 identifying the buffer
 */
void MACDiscardTx(BUFFER buffer)
{
    TxBuffers[buffer].bFree = TRUE;
    MACCurrTxbuf = buffer;
}

/**
 * Discard the contents of the current RX buffer. 
 */
void MACRxbufDiscard(void)
{
    BYTE newBoundary;
    WORD_VAL t;

    //Read the "Next Packet Pointer" from current MAC RX buffer. The first 4 bytes of the current
    //buffer contains the status, next packet pointer and length bytes.
    //Set 
    t.v[1] = MACCurrRxbuf;   //Page poiner to current MAC Rx Buffer
    t.v[0] = 1;              //"Next packet pointer" is second byte of packet
    NICSetAddr(t.Val);
    newBoundary = MACRxbufGet();

    //Update BNRY pointer with new value - this will remove current MAC RX Buffer from ring!
    //NICPut(CMDR, 0x20);         // Select PAGE 0, Abort remote DMA
    NICPut(BNRY, newBoundary);  //BNRY is the receive buffer's GET pointer
    return;
}


/**
 * This function returns total receive buffer size available for future data packets.
 */
WORD MACGetFreeRxSize(void)
{
    BYTE NICWritePtr;   //CURR register - RX buffer's PUT pointer
    BYTE temp;
    WORD_VAL tempVal;

    //Read current page pointer - Curr is the RX Buffer's PUT pointer
    NICPut(CMDR, 0x60);         //Set page 1
    NICWritePtr = NICGet(CURRP); //Curr is the RX Buffer's PUT pointer
    NICPut(CMDR, 0x20);         //Reset to page 0

    if ( NICWritePtr < MACCurrRxbuf )
        temp = (RXSTOP - MACCurrRxbuf) + NICWritePtr;
    else
        temp = NICWritePtr - MACCurrRxbuf;

    temp = RXPAGES - temp;
    tempVal.v[1] = temp;
    tempVal.v[0] = 0;
    return tempVal.Val;
}


#if defined(STACK_USE_DHCP) || defined(STACK_CLIENT_MODE)
/**
 * Check if the MAC is linked
 *
 * @return TRUE if linked, else FALSE
 */
BOOL MACIsLinked(void)
{
    BYTE_VAL temp;

    // Select Page 3
    NICPut(CMDR, 0xe0);

    // Read CONFIG0.
    temp.Val = NICGet(0x03);

    // Reset to page 0.
    NICPut(CMDR, 0x20);

    // Bit 2 "BNC" will be '0' if LINK is established.
    return (temp.bits.b2 == 0);
}
#endif


/**
 * Check if the MAC Receive Buffer has any received packets.
 * Reads the following data from the next available packet in the RX buffer: <ul>
 *  <li> The MAC 4 bytes RX packet header (status, nex receive packet, length) </li>
 *  <li> The 14 byte Ethernet header </li></ul>
 * Once a data packet is fetched by calling MACRxbufGetHdr, the complete data
 * packet must be fetched (using MACRxbufGet) and discarded (using MACRxbufDiscard).
 * Users cannot call MACRxbufGetHdr multiple times to receive multiple packets
 * and fetch data later on.
 *
 * @param[out] remote   Pointer to a MAC_ADDR structure. This function will write the MAC address of the
 *                      node who sent this packet to this structure
 * @param[out] type     Pointer to byte. This function will write the type of header to this variable.
 *                      Can be ETHER_IP, ETHER_ARP or MAC_UNKNOWN
 *
 * @return              True if RX Buffer contained a packet and it's header was read, False if not
 *
 */
BOOL MACRxbufGetHdr(MAC_ADDR *remote, BYTE* type)
{
    NE_PREAMBLE header;
    BYTE NICWritePtr;
    BYTE NICReadPtr;
    WORD_VAL temp;

    *type = MAC_UNKNOWN;

    //Read CURR and BNDY registers = RX Buffer's GET and PUT pointers
    NICPut(CMDR, 0x60);             //Set page 1
    NICWritePtr = NICGet(CURRP);    //Curr is the RX Buffer's PUT pointer
    NICPut(CMDR, 0x20);             //Reset to page 0
    NICReadPtr = NICGet(BNRY);      //Get BNRY pointer

    // Reset NIC if overrun has occured.
    if ( NICGet(ISR) & 0x10 )
    {
        BYTE resend;

        #if (DEBUG_MAC >= LOG_ERROR)
        debugPutMsg(3); //@mxd:3:Overrun error! Reseting MAC!
        #endif

        // Save TXP bit
        resend = NICGet(CMDR) & 0x04;

        //Issue STOP command to NIC and delay 2ms
        NICPut(CMDR, 0x21);
        DelayMs(2);

        //Clear Remote Byte count registers
        NICPut(RBCR0, 0);
        NICPut(RBCR1, 0);

        // TXP bit set?
        if (resend)
           // No re-send if Packet Transmitted or Transmit Error bit is set
           resend = !(NICGet(ISR) & 0x0A);

        //Put NIC in loopback mode and issue start command
        NICPut(TCR, 0x02);
        NICPut(CMDR, 0x22); // HM: Modified, changed to 22 to re-start NIC

        // Empty ring buffer completely
        MACCurrRxbuf = NICWritePtr;
        MACRxbufDiscard();

        //Reset ISR by writing FF to it
        NICPut(ISR, 0xff);

        //Take NIC out of loopback mode
        NICPut(TCR, 0x00);

        //Resend the packet
        if(resend)
           NICPut(CMDR, 0x26);
        return FALSE;
    }

    //Is there something in the buffer
    if ( NICWritePtr != NICReadPtr )
    {
        temp.v[1] = NICReadPtr; //Pointer to RAM page, is MSB of address
        temp.v[0] = 0;          //LSB is 0
        NICSetAddr(temp.Val);

        //Read the following from the current MAC RX Buffer:
        // - The MAC 4 bytes RX packet header (status, nex receive packet, length)
        // - The 14 byte Ethernet header
        MACRxbufGetArray((BYTE*)&header, sizeof(header));

        // Validate packet length and status as.
        if ( header.Status.PRX && (header.ReceivedBytes >= MINFRAMEC) && (header.ReceivedBytes <= MAXFRAMEC) &&
             (header.NextPacketPointer < RXSTOP) && (header.NextPacketPointer >= RXSTART) )
        {
            header.Type.Val = swaps(header.Type.Val);

            memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

            if ( (header.Type.v[1] == 0x08) && ((header.Type.v[0] == ETHER_IP) || (header.Type.v[0] == ETHER_ARP)) )
                *type = header.Type.v[0];

            MACCurrRxbuf = NICReadPtr;   //Set MACCurrRxbuf with page address of current RX Buffer
            return TRUE;
        }

        //ERROR!!! Invalid packet received!
        #if (DEBUG_MAC >= LOG_ERROR)
        debugPutMsg(4); //@mxd:4:Invalid Packet Error!
        #endif

        //Ring now contains garbage
        MACCurrRxbuf = NICWritePtr; // Empty ring buffer completely
        MACRxbufDiscard();
    }
    return FALSE;

}


/**
 * This function writes the MAC header (Ethernet header) part of a packet that
 * has to be transmitted to the current TX buffer (page in NIC RAM).
 * After this function, the data must still be written to the TX buffer.
 * After both header and data has been written, bits are set to instruct
 * NIC to transmit transmit buffer. This function does the following: <ul>
 *  <li> Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer </li>
 *  <li> Write the given header to the current TX Buffer </li>
 *  <li> Set the NIC Remote DMA byte count to the given len. This configures the
 *    Remote DMA to receive the given number of bytes. </li></ul>
 *
 * @param[out] remote   Pointer to a MAC_ADDR structure. This function will write the MAC address of the
 *                      node who sent this packet to this structure
 * @param type          Type of header. Can be ETHER_IP, ETHER_ARP or MAC_UNKNOWN
 * @param dataLen       Size of the Data of this Ethernet packet. A Ethernet packet consists of: <br>
 *                      - 6 Bytes = Desitination MAC address <br>
 *                      - 6 Bytes = Source MAC address <br>
 *                      - 2 Bytes = Type field, currently only IP or ARP <br>
 *                      - n Bytes = Data. Minimum length of 46 bytes <br><br>
 *                      The data will be written to the TX buffer following this command.
 *
 * @return              True if header was read, False if not
 */
void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen)
{
    WORD_VAL mytemp;
    BYTE etherType;

    NICPut(ISR, 0x0a);

    //Set the address of the current Remote DMA. This is the address that all future MACPut()
    //functions will write data to in the current TX Buffer. This address is set to the start
    //address of the current MAC TX Buffer. The current TX Buffer will thus point to the first
    //byte in the packet that will be transmitted = first byte of Ethernet header
    mytemp.v[1] = TxBuffers[MACCurrTxbuf].Index;
    mytemp.v[0] = 0;
    NICSetAddr(mytemp.Val);

    /////////////////////////////////////////////////
    //Write Ethernet header part (MAC header) = Dest MAC Adrr, Source MAC Adr, Type
    MACPutArray((BYTE*)remote, sizeof(*remote));

    MACPut(MY_MAC_BYTE1);
    MACPut(MY_MAC_BYTE2);
    MACPut(MY_MAC_BYTE3);
    MACPut(MY_MAC_BYTE4);
    MACPut(MY_MAC_BYTE5);
    MACPut(MY_MAC_BYTE6);

    if ( type == MAC_IP )
        etherType = ETHER_IP;
    else
        etherType = ETHER_ARP;

    MACPut(0x08);
    MACPut(etherType);

    dataLen += (WORD)sizeof(ETHER_HEADER);

    //If datalength smaller than minimum Ethernet packet (not including CRC), pad till minimum size
    if ( dataLen < MINFRAME )
       dataLen = MINFRAME;

    mytemp.Val = dataLen;

    NICPut(TBCR0, mytemp.v[0]);
    NICPut(TBCR1, mytemp.v[1]);
}


/**
 * Flush the MAC - transmit the current TX buffer 
 */
void MACFlush(void)
{
    BYTE i;

    //Configure to send a packet. To configure the NIC to send a packet, 3 registers have to
    //be set:
    // - TPSR: This gives the NIC RAM page of the packet to send
    // - TBCR0 & 1: These two resisters give the size of the packet to transmit
    NICPut(TPSR, TxBuffers[MACCurrTxbuf].Index);

    NICPut(CMDR, 0x24);

    // After every transmission, adjust transmit pointer to
    // next free transmit buffer.
    for ( i = 0; i < MAX_DATA_BUFFERS; i++ )
    {
        if ( TxBuffers[i].bFree )
        {
            MACCurrTxbuf = i;
            return;
        }
    }
}


/**
 * Reset the NIC
 */
void NICReset(void)
{
    #if (DEBUG_MAC >= LOG_INFO)
    debugPutMsg(5); //@mxd:5:Reset MAC
    #endif

    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(0);

    NIC_IOW_IO = 1;
    NIC_IOR_IO = 1;
    NIC_RESET_IO = 1;
    NIC_CTRL_TRIS = 0x00;

    // Reset pulse must be at least 800 ns.
    Delay10us(1);
    DelayMs(5);

    NIC_RESET_IO = 0;
}

/**
 * Write a byte to the I/O Port
 */
void NICPut(BYTE reg, BYTE val)
{
    WRITE_NIC_ADDR(reg);
    NIC_DATA_IO = val;
    TRISD = 0x00;   //Output
    BEGIN_IO_CYCLE();
    NIC_IOW_IO = 0;
    WAIT_FOR_IOCHRDY();
    NIC_IOW_IO = 1;
    LEAVE_IO_CYCLE();
}

/**
 * Read a byte from the I/O Port
 */
BYTE NICGet(BYTE reg)
{
    BYTE val;

    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(reg);
    BEGIN_IO_CYCLE();
    NIC_IOR_IO = 0;
    WAIT_FOR_IOCHRDY();
    val = NIC_DATA_IO;
    NIC_IOR_IO = 1;
    LEAVE_IO_CYCLE();
    return val;
}

void NICSetAddr(WORD addr)
{
    WORD_VAL t;

    //The DMAAddr variable keeps track of the Remote DMA current address. Reads or writes to the
    //Remote DMA will access this address, and increment it when done.
    DMAAddr.Val = addr;

    //Set the RSAR (Remote start address) registers to point to given address. This is the address
    //of the RAM that will be accessed with subsequent read/write operations.
    t.Val = addr;
    //NICPut(ISR, 0x40);    //ISR bit 6 is set when Remote DMA has completed - why is it set here???
    NICPut(RSAR0, t.v[0]);
    NICPut(RSAR1, t.v[1]);
}

/**
 * Get the current Remote DMA address. This is the address set by the NICSetAddr register.
 * It will however be incremented after each Remote DMA read.
 * The MACRxbufGet() function for example does a remote DMA read.
 *
 * @return The current Remote DMA address.
 */ 
WORD MACGetNICAddr(void)
{
    WORD_VAL t;

    t.v[1] = NICGet(RSAR1);
    t.v[0] = NICGet(RSAR0);

    return t.Val;
}

/**
 * This function sets the access location for the active receive buffer,
 * relative to the first byte following the Ethernet header. If the given
 * offset is thus 0, the access location will be set to the first byte of
 * whatever follows the Ethernet header, for example the IP packet or ARP packet.
 * Users must make sure that the supplied offset does not go beyond
 * current receive buffer content. If offset overruns the current receive
 * buffer, all subsequent access to that buffer would yield invalid data.
 *
 * @param offset    Location (with respect to first byte following Ethernet header)
 *                  where next access is to occur.
 *
 */
void MACSetRxBuffer(WORD offset)
{
    WORD_VAL t;

    //Get pointer to first byte after Ethernet header
    t.v[1] = MACCurrRxbuf;       //Contains RAM page, is MSB of address
    t.v[0] = sizeof(NE_PREAMBLE);   //Preample data contained in RX Buffer

    t.Val += offset;    //Now add given offset to it

    NICSetAddr(t.Val);
}


/**
 * This function makes the given transmit buffer active, and sets it's access
 * pointer to be: <br>
 * - At the given offset after the Ethernet header <br>
 * - In the given TX Buffer <br><br>
 * So, if we pass 0 in as the offset, we will set the pointer to the
 * first byte after the Ethernet header.
 * Users must make sure that the supplied offset does not go beyond current transmit buffer content.
 * If offset overruns the current transmit buffer, all subsequent access to
 * that buffer would yield invalid data.
 *
 * @param buffer    A transmit buffer where this access offset be applied
 * @param offset    Location (with respect to beginning of buffer) where next access is to occur
 *
 */
void MACSetTxBuffer(BUFFER buffer, WORD offset)
{
    WORD_VAL t;

    MACCurrTxbuf = buffer;
    t.v[1] = TxBuffers[MACCurrTxbuf].Index;
    t.v[0] = sizeof(ETHER_HEADER);

    t.Val += offset;

    NICSetAddr(t.Val);
}
