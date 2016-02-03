/**
 * @brief           FRAM Module for reading and writing from and to the FRAM
 * @file            fram.h
 * @author          <a href="www.ziggurat29.com">Ziggurat29</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *********************************************************************/

#include "projdefs.h"        //for all the definitions
#include "fram.h"
#include "spi.h"
#include "net\delay.h"

#include "busser1.h"    //whilst debugging...



//prepare for spi master mode 0
#define FRAMSPI_SetupBus()    \
FRAM_SPI_BIT_SO = 0;    \
FRAM_SPI_BIT_SCK = 0;    \
FRAM_SPI_TRI_SO = 0;    \
FRAM_SPI_TRI_SI = 1;    \
FRAM_SPI_TRI_SCK = 0;


//return D to inputs for 8019 (is this necessary? looks like 8019 code handles this already)
#define FRAMSPI_RelinquishBus()    FRAM_SPI_TRI_SO = 1; FRAM_SPI_TRI_SCK = 1;


//CS is active low
#define FRAMSPI_ConfigureCS() FRAM_SPI_TRI_CS = 0;
#define FRAMSPI_AssertCS() FRAM_SPI_BIT_CS = 0;
#define FRAMSPI_NegateCS() FRAM_SPI_BIT_CS = 1;



/////////////////////////////////////////////////
//Software SPI
/////////////////////////////////////////////////
#if defined(SOFTWARE_SPI)

//mode 0 clock is active high
#define FRAMSPI_RaiseClock() FRAM_SPI_BIT_SCK = 1;
#define FRAMSPI_LowerClock() FRAM_SPI_BIT_SCK = 0;

//bitbang in and out the data
#define FRAMSPI_BitOut(a) FRAM_SPI_BIT_SO = (a)?1:0;
#define FRAMSPI_BitIn() FRAM_SPI_BIT_SI

void FRAMSPI_Initialize(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    FRAMSPI_ConfigureCS();
}

void FRAMSPI_Prepare(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    FRAMSPI_SetupBus();
}

void FRAMSPI_Unprepare(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    FRAMSPI_RelinquishBus();    //make all inputs again for 8139; necessary?
}

BYTE FRAMSPI_ExchangeByte ( BYTE byOut )
{
    BYTE byIn;
    BYTE byCount;
    for ( byCount = 0; byCount < 8; ++byCount )
    {
        FRAMSPI_BitOut ( byOut & 0x80 );    //setup output bit
        FRAMSPI_RaiseClock();
        byOut <<= 1;    //shift output up
        byIn <<= 1;        //shift input up
        FRAMSPI_LowerClock();
        byIn |= FRAMSPI_BitIn();
    }
    return byIn;
}

#define FRAMSPI_WriteByte(a) FRAMSPI_ExchangeByte(a)
#define FRAMSPI_ReadByte() FRAMSPI_ExchangeByte(0)



/////////////////////////////////////////////////
//Hardware SPI
/////////////////////////////////////////////////
#else

//setup SPI2 master mode 0
#define    SPI_MODE MODE_00
#define SPI_SAMPLEIN SMPEND

void FRAMSPI_Initialize(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    FRAMSPI_ConfigureCS();
}

void FRAMSPI_Prepare(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    FRAMSPI_SetupBus();
    OpenSPI2 ( SPI_ROLE, SPI_MODE, SPI_SAMPLEIN );
}

void FRAMSPI_Unprepare(void)
{
    FRAMSPI_NegateCS();    //(if not already)
    CloseSPI2();    //disable SPI2
    FRAMSPI_RelinquishBus();    //make all inputs again for 8139; necessary?
}

#define FRAMSPI_WriteByte(a) WriteSPI2(a)
#define FRAMSPI_ReadByte() ReadSPI2()

#endif    //hardware or software spi implementation



/////////////////////////////////////////////////
//FRAM Functions
/////////////////////////////////////////////////

//FRAMInit
//    One-time setup of SPI interface for subsequent operations.
void FRAMInit(void)
{
    FRAMSPI_Initialize();
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_WREN );    //enable writing
    FRAMSPI_NegateCS();    //finish this instruction
    FRAMSPI_AssertCS();    //begin new one
    FRAMSPI_WriteByte ( SPI_WRSR );    //write to status register
    //FRAMSPI_WriteByte ( 0x8c );        //protect everything
    FRAMSPI_WriteByte ( 0x00 );    //protect nothing
    FRAMSPI_Unprepare();
}


//FRAMReadStatusRegister
//    Get the current contents of the FRAM status register.
BYTE FRAMReadStatusRegister(void)
{
    BYTE byRet;
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_RDSR );    //read from status register
    byRet = FRAMSPI_ReadByte();    //get value
    FRAMSPI_Unprepare();
    return byRet;
}

/////////////////////////////////////////////////
//block IO methods

//FRAMWriteArr
//    Write a sequence of ram bytes to the FRAM.
void FRAMWriteArr ( FRAM_ADDR addr, BYTE* abyData, int nLen )
{
    int nIdx;
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_WREN );    //enable writing
    FRAMSPI_NegateCS();    //finish this instruction
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_WRITE );    //write data
    FRAMSPI_WriteByte ( addr>>8 );        //addr high
    FRAMSPI_WriteByte ( addr );            //addr low
    for ( nIdx = 0; nIdx < nLen; ++nIdx ) {
        FRAMSPI_WriteByte ( abyData[nIdx] );
    }
    FRAMSPI_Unprepare();
}

//FRAMReadArr
//    Read a sequence of FRAM bytes to ram.
void FRAMReadArr ( FRAM_ADDR addr, BYTE* abyData, int nLen )
{
    int nIdx;
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_READ );        //read data
    FRAMSPI_WriteByte ( addr>>8 );        //addr high
    FRAMSPI_WriteByte ( addr );            //addr low
    for ( nIdx = 0; nIdx < nLen; ++nIdx )
        { abyData[nIdx] = FRAMSPI_ReadByte(); }
    FRAMSPI_Unprepare();
}

/////////////////////////////////////////////////
//sequential IO methods

void FRAMBeginWrite ( FRAM_ADDR addr )
{
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_WREN );    //enable writing
    FRAMSPI_NegateCS();    //finish this instruction
    FRAMSPI_AssertCS();    //begin new instruction
    FRAMSPI_WriteByte ( SPI_WRITE );    //write data
    FRAMSPI_WriteByte ( addr>>8 );        //addr high
    FRAMSPI_WriteByte ( addr );            //addr low
}

void FRAMWrite ( BYTE byVal )
{
    FRAMSPI_WriteByte ( byVal );
}

void FRAMEndWrite(void)
{
    FRAMSPI_Unprepare();
}

void FRAMBeginRead ( FRAM_ADDR addr )
{
    FRAMSPI_Prepare();
    FRAMSPI_AssertCS();             //begin new instruction
    FRAMSPI_WriteByte ( SPI_READ ); //read data
    FRAMSPI_WriteByte ( addr>>8 );  //addr high
    FRAMSPI_WriteByte ( addr );     //addr low
}

BYTE FRAMRead(void)
{
    return FRAMSPI_ReadByte();
}

void FRAMEndRead(void)
{
    FRAMSPI_Unprepare();
}


/////////////////////////////////////////////////
//testing stuff
#if 0

//testing stuff
static ROM char g_szHorror[] = "Horror\r\n";
static ROM char g_szHmm[] = "Hmm...\r\n";
static ROM char g_szOh[] = "Oh!\r\n";

static ROM char sl_szStartTest[] = "Starting FRAM test\r\n";
static ROM char sl_szCRLF[] = "\r\n";
static ROM char sl_szEndTest[] = "Ending FRAM test\r\n";

static ROM char sl_szWriteOut[] = "Writing out:\r\n";
static ROM char sl_szReadIn[] = "Read in:\r\n";

BYTE g_aby[128];

void FRAMTest(void)
{
    int nIdx;

    serPutRomString ( sl_szStartTest );

    for ( nIdx = 0; nIdx < sizeof(g_aby); ++nIdx )
        { g_aby[nIdx] = nIdx; }

    serPutRomString ( sl_szWriteOut );
    for ( nIdx = 0; nIdx < sizeof(g_aby); ++nIdx )
        { serPutByteHex ( g_aby[nIdx] ); }
    serPutRomString ( sl_szCRLF );
    FRAMWriteArr ( 0, g_aby, sizeof(g_aby) );

    FRAMReadArr ( 0, g_aby, sizeof(g_aby) );
    serPutRomString ( sl_szReadIn );
    for ( nIdx = 0; nIdx < sizeof(g_aby); ++nIdx )
        { serPutByteHex ( g_aby[nIdx] ); }
    serPutRomString ( sl_szCRLF );

    serPutRomString ( sl_szEndTest );
}

void FRAMTest2(void)
{
    int nIdx;

    serPutRomString ( sl_szStartTest );

    FRAMReadArr ( 64, g_aby, sizeof(g_aby) );
    serPutRomString ( sl_szReadIn );
    for ( nIdx = 0; nIdx < sizeof(g_aby); ++nIdx )
        { serPutByteHex ( g_aby[nIdx] ); }
    serPutRomString ( sl_szCRLF );

    serPutRomString ( sl_szEndTest );
}

#endif
