    /*********************************************************************
 * mac.c    -       MAC Module for Microchip TCP/IP Stack
 * Dependencies:    string.h, stacktsk.h,  helpers.h, mac.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Timimg:
 * =======
 * At this stage it is not known if the RealTek chip applies IOCHRDY wait
 * states to ordinary NIC register access. It must be assumed
 * as the NE1000 does insert wait cycles upon register access while a
 * local DMA is in progress (It must, there is only one data bus for all).
 * - Register Read (as per NatSemi DP8390C datasheet)
 *   Min. time of 10 ns to latch address before IOR shall be asserted.
 *   Min time of 55 ns after IOCHRDY is asserted before data is valid.
 *   NatSemi DP9390C datasheet states that it can take
 *   max. 55 ns + 30 ns * n*50 ns defore data is valid after falling edge
 *   of /RD. The time n*bcyc is not specified, it's an integral number.
 *   Additional bcyc are inserted if a local or remote DMA takes place.
 *   Assuming that n corresponds to cycles needed for the DMA bursts
 *   this totals to 55 + 30 + (2 * 4 * 50) = 485 ns for 2 byte bursts.
 * - Register Write (as per NatSemi DP8390C datasheet)
 *   Min. time of 15 ns to latch address before IOW shall be asserted.
 *   Min. time of 50 ns after IOCHRDY is asserted before data is sampled.
 *   NatSemi DP9390C datasheet states that it can take
 *   max. 50 + 30 ns * n*50 ns defore data is sampled after falling edge
 *   of /WR. The time n*bcyc is not specified, it's an integral number.
 *   Additional bcyc are inserted if a local or remote DMA takes place.
 *   Assuming that n corresponds to cycles needed for the DMA bursts
 *   this totals to 50 + 30 + (2 * 4 * 50) = 480 ns for 2 byte bursts.
 *
 * Known Issues:
 * =============
 * - When writing into the NIC transmit buffer we must assure that we are not
 *   overwring data of a currently transmitting packet. Usually this
 *   will not happen because we are writing slower to the NIC then
 *   a packet will leave the NIC. However if transmission is delayed because
 *   of collisions then this assumption is not correct anymore and we will
 *   overwrite the currently buffered packet with a new one.
 *   This issue can be avoided by improving the TX buffer management
 *   and making sure before allocating a TX buffer that there is no TX in
 *   progress, e.g. doing "while (NICGet(CMDR) & 0x04);" in MACReserveTxBuffer.
 *   However currently only TCP is using MACReserveTxBuffer.
 *   UDPIsPutReady, ARPIsTxReady, ICMPIsTxReady eventually call MACIsTXReady.
 *   Some more thoughts are needed to resolve this issue satisfactory.
 *
 *********************************************************************
 * File History
 *
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
 *    - MACPut... and MACGet... routines speeded up, uses more code though
 *      Approx. 28% better LAN throughput.
 *    - Delay in MACGetHeader replaced with DelayMs, saves some code
 *    - RXSTOP modfied to be end of buffer RAM + 1 page, so we won't waste
 *      one page of buffer RAM
 *    - Useful functions NICPut, NICGet, NICSetAddr made non-static
 *    - MACGetHeader: Overrun handling modified to match NatSemi AN 877.
 *    - MACGetHeader: Added sanity check for next packet pointer
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
 ********************************************************************/

#define THIS_IS_MAC_LAYER

#include <string.h>
#include "mac.h"
#include <appcfg.h>
#include "helpers.h"
#include "delay.h"

/*
 * Define DEBUG_HTTP in projdefs.h (or command line) if debug output is required.
 */
#if defined(DEBUG_MAC)
#include "debug.h"
#else
#define debugPutByte(a)
#define debugPutRomString(a)
#define debugPutString(a)
#endif


#if defined(STACK_USE_SLIP)
#error Unexpected module is detected.
#error This file must be linked when SLIP module is not in use.
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
#define MINFRAME  60
#define MINFRAMEC 64
#define CRCLEN    4
#define MAXFRAME  1514
#define MAXFRAMEC 1518

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

/* NIC RAM definitions */
#define RAMPAGES 0x20           /* Total number of 256-byte RAM pages */
#define TXSTART  0x40           /* Tx buffer start page - NE2000 mode */
#define TXPAGES (MAC_TX_BUFFER_COUNT * (MAC_TX_BUFFER_SIZE/NIC_PAGE_SIZE))
#define RXSTART  (TXSTART+TXPAGES)      /* Rx buffer start page */
// HM: Modified, PSTOP is the last page + 1, so why waste 256 bytes of buffer?
//#define RXSTOP   (TXSTART+RAMPAGES-1)   /* Last Rx buffer page */
#define RXSTOP   (TXSTART+RAMPAGES)   /* Last Rx buffer page + 1 */

// HM: Modified, a 2 byte FIFO threshold leads to shorter DMA bursts
//#define DCRVAL   0x48         /* Value for data config reg */
#define DCRVAL   0x08           /* Value for data config reg */
                                /* 8-bit DMA, big-endian, 1 DMA, Normal */

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
    BYTE Index;
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



typedef struct _NE_PREAMBLE
{
    NE_RCR Status;
    BYTE NextPacketPointer;
    WORD ReceivedBytes;

    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} NE_PREAMBLE;


BYTE NICReadPtr;                // Next page that will be used by NIC to load new packet.
BYTE NICCurrentRdPtr;           // Page that is being read...
BYTE NICCurrentTxBuffer;

BYTE iochrdyCnt;

#ifdef MAC_CNTR1_3
WORD cntr0;
WORD cntr1;
WORD cntr2;
#endif


void NICReset(void);
void NICPut(BYTE reg, BYTE val);
void NICPutCurr(BYTE val);
BYTE NICGet(BYTE reg);
BYTE NICGetCurr(void);
void NICSetAddr(WORD addr);


void    MACInit(void)
{
    BYTE i;

    /* SBC45EC - use port B*/
    #if defined(BRD_SBC44EC)
    TRISB = 0xe0;   //RB0-4 are outputs
    //NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
    /* SBC45EC - use port A*/
    #elif defined(BRD_SBC45EC)
    TRISA = 0xd0;   //RA0-3 and RA5 are outputs
    //NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
    /* SBC65EC - use port E*/
    #elif defined(BRD_SBC65EC)
    TRISE = 0x00;           //RE0-7 are outputs
    NIC_IOCHRDY_TRIS = 1;   //IOCHRDY is an input
    NIC_IOCHRDY = 1;
    #endif

    //Reset all error counters
    #ifdef MAC_CNTR1_3
    cntr0 = 0;
    cntr1 = 0;
    cntr2 = 0;
    #endif

    // On Init, all transmit buffers are free.
    for ( i = 0; i < MAX_DATA_BUFFERS; i++ )
    {
        TxBuffers[i].Index = TXSTART + (i * (MAC_TX_BUFFER_SIZE/NIC_PAGE_SIZE));
        TxBuffers[i].bFree = TRUE;
    }
    NICCurrentTxBuffer = 0;

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
        NICPut(TPSR, TxBuffers[NICCurrentTxBuffer].Index);

        // Initialize Receive Buffer Ring
        NICPut(PSTART, RXSTART);
        NICPut(PSTOP, RXSTOP);
        NICPut(BNRY, (BYTE)(RXSTOP-1));

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

        // Initialize CURRent pointer
        //NICPut(CURRP, RXSTART);   // TEST TEST
        NICPutCurr(RXSTART);

        // Remember current receive page
        NICReadPtr = RXSTART;

        // TCP layer uses this value to calculate window size. Without init the
        // first window size value sent would be wrong if no packet has been received before.
        NICCurrentRdPtr = RXSTART;

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
}


/*********************************************************************
 * Function:        void MACTask(void)
 *
 * PreCondition:    Must be called every couple of ms
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
void MACTask(void)
{
#if defined(DEBUG_MAC)
    cntr0 += NICGet(CNTR0);
    cntr1 += NICGet(CNTR1);
    cntr2 += NICGet(CNTR2);
#endif
}


/*********************************************************************
 * Function:        WORD    MACInitEepromRegs(void)
 *
 * PreCondition:    Must be called after chip is initialized
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    None
 *
 * Note:            Initializes registers that should have been initialized with the contents
 *                  of the EEPROM
 *
 ********************************************************************/
#if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
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

BOOL    MACIsTxReady(void)
{
    // NICCurrentTxBuffer always points to free buffer, if there is any.
    // If there is none, NICCurrentTxBuffer will be a in 'Use' state.
    //return TxBuffers[NICCurrentTxBuffer].bFree;
    // Check to see if previous transmission was successful or not.
    return !(NICGet(CMDR) & 0x04);
}

// HM: Modified, some speed optimisation and put NatSemi AN 877 fixes in.
// Some macros allow additional delay after IOR and IOW assertion and
// disabeling of interrupts.
// According to my UDP tests throughput was increased from 108 UDP send/recv
// sequences (of 48 bytes) to 138 which is an improvement of ~28%.
// Code cost approx. 72 bytes. I'd say it's worth it. Further improvements
// can be achieved by coding these core routines in assembler.

#if defined STACK_DISABLES_INTS
    #define BEGIN_IO_CYCLE() { INTCON_GIEH = 0; } // Disable interrupts
    #define LEAVE_IO_CYCLE() { INTCON_GIEH = 1; } // Enable interrupts
#else
    #define BEGIN_IO_CYCLE()
    #define LEAVE_IO_CYCLE()
#endif

//#define WAIT_FOR_IOCHRDY() {while(NIC_IOCHRDY == 0);}      //Wait for IOCHRDY pin to go high
//#define WAIT_FOR_IOCHRDY() {do {NIC_IOCHRDY_TRIS=0; NIC_IOCHRDY_TRIS=1;} while(NIC_IOCHRDY == 0);}      //Wait for IOCHRDY pin to go high
#if (CLOCK_FREQ == 40000000)
#define WAIT_FOR_IOCHRDY() {Nop(); Nop(); Nop(); Nop(); Nop();}     // Delay for 500 ns
#elif (CLOCK_FREQ == 20000000)
#define WAIT_FOR_IOCHRDY() {Nop(); Nop(); Nop(); }                  // Delay for 600 ns
#endif
//#  define WAIT_FOR_IOCHRDY()

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

    #define FirstFastNICPut(a, b) \
    { \
        TRISD = 0x00; \
        FastNICPut(a, b); \
    }
#else
    #define FastNICPut(a, b) NICPut(a, b)
    #define FirstFastNICPut(a, b) NICPut(a, b)
#endif

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

void    MACPut(BYTE val)
{
    FirstFastNICPut(RBCR0, 1);
    FastNICPut(RBCR1, 0);
    FastNICPut(CMDR, 0x12);
    FastNICPut(ISR, 0x40); // Clear DMA bit
    FastNICPut(NIC_DATAPORT, val);
    WaitForDmaToFinish();
}


void    MACPutArray(BYTE *val, WORD len)
{
#if defined(__18CXX)
    overlay
#endif
    WORD_VAL length;
    WORD_VAL t;

    t.Val = len + (len & 1);

    FirstFastNICPut(ISR, 0x40);
    FastNICPut(RBCR0, t.byte.LSB);
    FastNICPut(RBCR1, t.byte.MSB);
    FastNICPut(CMDR, 0x12);

    length.Val = len;
    do {
        while (length.byte.LSB-- > 0) {
            FastNICPut(NIC_DATAPORT, *val++);
        }
        length.byte.LSB = 0xff;
    } while (length.byte.MSB-- > 0);

    WaitForDmaToFinish();
}


BYTE    MACGet(void)
{
#if defined(__18CXX)
    overlay
#endif
    BYTE b;

    FirstFastNICPut(RBCR0, 1);
    FastNICPut(RBCR1, 0);
    FastNICPut(CMDR, 0x0a);
#if defined(STACK_USE_FAST_NIC)
    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(NIC_DATAPORT);
    BEGIN_IO_CYCLE();
    NIC_IOR_IO = 0;
    WAIT_FOR_IOCHRDY();
    b = NIC_DATA_IO;
    NIC_IOR_IO = 1;
    LEAVE_IO_CYCLE();
    return (b);
#else
    return NICGet(NIC_DATAPORT);
#endif
}

/*********************************************************************
 * Function:        WORD    MACGetArray(BYTE *val, WORD len)
 *
 * PreCondition:    There must be a valid packet in the NIC's buffer. MacGetHeader() was called prior
 *                  to this function.
 *
 * Input:           - len: Length of packet to be read
 *                  - val: Buffer to read packet into
 *
 * Output:
 *
 * Return:          Number of bytes read
 *
 * Side Effects:    None
 *
 * Note:            Read the next packet from the NIC buffer. len bytes are read into val buffer
 *
 ********************************************************************/
WORD    MACGetArray(BYTE *val, WORD len)
{
#if defined(__18CXX)
    overlay
#endif
    WORD_VAL t;

    t.Val = len;

    FirstFastNICPut(RBCR0, t.byte.LSB);
    FastNICPut(RBCR1, t.byte.MSB);
    FastNICPut(CMDR, 0x0a);
    #if defined(STACK_USE_FAST_NIC)
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
    #else
    while( len-- > 0 )
    {
        *val++ = NICGet(NIC_DATAPORT);
    #endif
    }

    return t.Val;
}

void MACReserveTxBuffer(BUFFER buffer)
{
    TxBuffers[buffer].bFree = FALSE;
}


void MACDiscardTx(BUFFER buffer)
{
    TxBuffers[buffer].bFree = TRUE;
    NICCurrentTxBuffer = buffer;
}

void MACDiscardRx(void)
{
    BYTE newBoundary;

    newBoundary = NICReadPtr - 1;
    if ( newBoundary < RXSTART )
        newBoundary = RXSTOP - 1;
//    NICPut(CMDR, 0x20);     // Select PAGE 0
    NICPut(BNRY, newBoundary);
    return;
}


WORD MACGetFreeRxSize(void)
{
    BYTE NICWritePtr;
    BYTE temp;
    WORD_VAL tempVal;

    //Read current page pointer
    NICPut(CMDR, 0x60);     //Set page 1
    //NICWritePtr = NICGet(CURRP);  // TEST TEST
    NICWritePtr = NICGetCurr();
    NICPut(CMDR, 0x20);     //Reset to page 0

    if ( NICWritePtr < NICCurrentRdPtr )
        temp = (RXSTOP - NICCurrentRdPtr) + NICWritePtr;
    else
        temp = NICWritePtr - NICCurrentRdPtr;

    temp = RXPAGES - temp;
    tempVal.v[1] = temp;
    tempVal.v[0] = 0;
    return tempVal.Val;
}


#if defined(STACK_USE_DHCP) || defined(STACK_CLIENT_MODE)
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

/*********************************************************************
 * PreCondition:    
 *
 * Input:           None
 *
 * Output:          - remote: The MAC address of the node who sent this packet
 *                  - type: The type of header. Can be ETHER_IP, ETHER_ARP or MAC_UNKNOWN
 *
 * Return:          - True if header was read, False if not
 *
 * Side Effects:    None
 *
 * Note:            Read the next header from MAC buffer, if any
 *
 ********************************************************************/
BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
    NE_PREAMBLE header;
    BYTE NICWritePtr;
    WORD_VAL temp;


    *type = MAC_UNKNOWN;

    //Read current page pointer
    NICPut(CMDR, 0x60);             //Set page 1
    //NICWritePtr = NICGet(CURRP);  // TEST TEST
    NICWritePtr = NICGetCurr();
    NICPut(CMDR, 0x20);             //Reset to page 0

    // Reset NIC if overrun has occured.
    if ( NICGet(ISR) & 0x10 )
    {
        BYTE resend;

        debugPutRomString((ROM char*)"M:OR Err!");

        resend = NICGet(CMDR) & 0x04; // Save TXP bit
        NICPut(CMDR, 0x21);
        DelayMs(2); // HM: Modified, AN 877 says we need the classic 1.6 ms delay here
        NICPut(RBCR0, 0);
        NICPut(RBCR1, 0);
        // TXP bit set?
        if (resend)
           // No re-send if Packet Transmitted or Transmit Error bit is set
           resend = !(NICGet(ISR) & 0x0A);
        NICPut(TCR, 0x02);
        NICPut(CMDR, 0x22); // HM: Modified, changed to 22 to re-start NIC
        NICReadPtr = NICCurrentRdPtr = NICWritePtr; // Empty ring buffer completely
        MACDiscardRx();
        NICPut(ISR, 0xff);
        NICPut(TCR, 0x00);
        // HM: Modified, resend the packet
        if(resend)
           NICPut(CMDR, 0x26);
        return FALSE;
    }

    //Is there something in the buffer
    if ( NICWritePtr != NICReadPtr )
    {
        temp.v[1] = NICReadPtr;
        temp.v[0] = 0;
        NICSetAddr(temp.Val);

        MACGetArray((BYTE*)&header, sizeof(header));

        // Validate packet length and status as.
        if ( header.Status.PRX && (header.ReceivedBytes >= MINFRAMEC) && (header.ReceivedBytes <= MAXFRAMEC) &&
             (header.NextPacketPointer < RXSTOP) && (header.NextPacketPointer >= RXSTART) )
        {
            header.Type.Val = swaps(header.Type.Val);

            memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

            if ( (header.Type.v[1] == 0x08) && ((header.Type.v[0] == ETHER_IP) || (header.Type.v[0] == ETHER_ARP)) )
                *type = header.Type.v[0];

            NICCurrentRdPtr = NICReadPtr;
            NICReadPtr = header.NextPacketPointer;
            return TRUE;
        }
        else {
            debugPutRomString((ROM char*)"M:Inval Pct Err!");
        }

        //Ring now contains garbage
        NICReadPtr = NICCurrentRdPtr = NICWritePtr; // Empty ring buffer completely
        MACDiscardRx();
    }
    return FALSE;

}



void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen)
{
    WORD_VAL mytemp;
    BYTE etherType;

    NICPut(ISR, 0x0a);

    mytemp.v[1] = TxBuffers[NICCurrentTxBuffer].Index;
    mytemp.v[0] = 0;

    NICSetAddr(mytemp.Val);

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
// HM: Modified
//    if ( dataLen < MINFRAME ) // 64 )      // NKR 4/23/02
//        dataLen = 64; // MINFRAME;
    if ( dataLen < MINFRAMEC )
       dataLen = MINFRAMEC;
// End HM
        mytemp.Val = dataLen;

    NICPut(TBCR0, mytemp.v[0]);
    NICPut(TBCR1, mytemp.v[1]);

}

void MACFlush(void)
{
    BYTE i;

    NICPut(TPSR, TxBuffers[NICCurrentTxBuffer].Index);

    NICPut(CMDR, 0x24);

    // After every transmission, adjust transmit pointer to
    // next free transmit buffer.
    for ( i = 0; i < MAX_DATA_BUFFERS; i++ )
    {
        if ( TxBuffers[i].bFree )
        {
            NICCurrentTxBuffer = i;
            return;
        }
    }
}

void NICReset(void)
{
    debugPutRomString((ROM char*)"M:Reset!");

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

void NICPutCurr(BYTE val)
{
    WRITE_NIC_ADDR(CURRP);
    NIC_DATA_IO = val;
    TRISD = 0x00;   //Output
    BEGIN_IO_CYCLE();
    NIC_IOW_IO = 0;
    
    //NOP(); NOP(); NOP();
    //Nop(); Nop(); Nop(); Nop(); Nop();Nop(); Nop(); Nop(); Nop(); Nop();
    WAIT_FOR_IOCHRDY();

    NIC_IOW_IO = 1;
    LEAVE_IO_CYCLE();
}

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

BYTE NICGetCurr(void)
{
    BYTE val;

    TRISD = 0xff;   //Input
    WRITE_NIC_ADDR(CURRP);
    BEGIN_IO_CYCLE();
    NIC_IOR_IO = 0;
    
    //NOP(); NOP(); NOP();
    //Nop(); Nop(); Nop(); Nop(); Nop();Nop(); Nop(); Nop(); Nop(); Nop();
    WAIT_FOR_IOCHRDY();

    val = NIC_DATA_IO;
    NIC_IOR_IO = 1;
    LEAVE_IO_CYCLE();
    return val;
}

void NICSetAddr(WORD addr)
{
    WORD_VAL t;

    t.Val = addr;
    NICPut(ISR, 0x40);
    NICPut(RSAR0, t.v[0]);
    NICPut(RSAR1, t.v[1]);
}

void MACSetRxBuffer(WORD offset)
{
    WORD_VAL t;

    t.v[1] = NICCurrentRdPtr;
    t.v[0] = sizeof(NE_PREAMBLE);

    t.Val += offset;

    NICSetAddr(t.Val);
}

void MACSetTxBuffer(BUFFER buffer, WORD offset)
{
    WORD_VAL t;

    NICCurrentTxBuffer = buffer;
    t.v[1] = TxBuffers[NICCurrentTxBuffer].Index;
    t.v[0] = sizeof(ETHER_HEADER);

    t.Val += offset;

    NICSetAddr(t.Val);
}

/*
 * Prints out current MAC status via debug ports
 */
#if defined(DEBUG_MAC)
void MACShowDebug(void)
{
    char buf[5];
    debugPutRomString((ROM char*)"M:FA=");
    itoa(cntr0, buf);
    debugPutString((BYTE*)buf);
    debugPutRomString((ROM char*)",CRC=");
    itoa(cntr1, buf);
    debugPutString((BYTE*)buf);
    debugPutRomString((ROM char*)",MP=");
    itoa(cntr2, buf);
    debugPutString((BYTE*)buf);
}
#endif

#ifdef STACK_USE_MAC_GETOFFSET
WORD MACGetOffset(void)
{
    WORD_VAL t;

    t.v[1] = NICGet(RSAR1);
    t.v[0] = NICGet(RSAR0);

    return t.Val;
}
#endif


