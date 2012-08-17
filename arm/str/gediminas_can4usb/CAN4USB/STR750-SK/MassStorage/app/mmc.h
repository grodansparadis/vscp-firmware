/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2003
 *
 *    File name   : mmc.h
 *    Description : define MMC module
 *
 *    History :
 *    1. Date        : July 1, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
 **************************************************************************/

#include <includes.h>

#ifndef __MMC_H
#define __MMC_H

#ifdef MMC_GLOBAL
#define MMC_EXTERN
#else
#define MMC_EXTERN  extern
#endif

#define MMC_DISK_LUN  0

// SD Maximum Block Read Access Time
#define RD_TIME_OUT               100LL   // ms
// SD Maximum Block Write Access Time
#define WR_TIME_OUT               250LL   // ms

// Hardware depends definitions
#define IdentificationModeClock   (200KHZ)
#define PC_PCSPI1                 (1<<10)
#define PC_PCSPI0                 (1<<8)

//
#define MMC_OK              0x00
#define MMC_ILDE_STATE      0x01
#define MMC_ERASE_RST       0x02
#define MMC_ILLEGAL_CMD     0x04
#define MMC_CRC_ERROR       0x08
#define MMC_ERASE_ERROR     0x10
#define MMC_ADD_ERROR       0x20
#define MMC_PARAM_ERROR     0x40

#define MMC_DATA_TOLKEN     0xFE
#define MMC_DATA_ERR_TOLKEN 0x1F
#define MMC_STOP_TRAN       0xFD

#define MMC_DLY_1MSEC       1000

typedef enum _MmcState_t
{
  MmcOk = 0, MmcNoPresent, MmcNoResponse, MmcCardError, MmcDataError,
  MmcUknowError, MmmcParameterError, MmcMiscompare
} MmcState_t;

typedef enum _MmcSpiCmdInd_t
{
  CMD0 = 0, // Resets the MultiMediaCard

  CMD1,     // Activates the card’s initialization process

  CMD9,     // Asks the selected card to send its card-specific data (CSD)

  CMD10,    // Asks the selected card to send its card identification (CID)

  CMD12,    // Stop transmission on multiple block read

  CMD13,    // Asks the selected card to send its status register

  CMD16,    // Selects a block length (in bytes) for all following block commands (read and write)

  CMD17,    // Reads a block of the size selected by the SET_BLOCKLEN command

  CMD18,    // Continuously transfers data blocks from card to host
            // until interrupted by a Stop command or the requested number of data blocks transmitted

  CMD24,    // Writes a block of the size selected by the SET_BLOCKLEN command

  CMD25,    // Continuously writes blocks of data until a ‘Stop Tran’
            // Token or the requested number of blocks received

  CMD27,    // Programming of the programmable bits of the CSD

  CMD28,    // If the card has write protection features, this
            // command sets the write protection bit of the
            // addressed group. The properties of write protection
            // are coded in the card specific data (WP_GRP_SIZE).

  CMD29,    // If the card has write protection features, this
            // command clears the write protection bit of the addressed group

  CMD30,    // If the card has write protection features, this
            // command asks the card to send the status of the write protection bits

  CMD32,    // Sets the address of the first sector of the erase group

  CMD33,    // Sets the address of the last sector in a continuous
            // range within the selected erase group, or the address
            // of a single sector to be selected for erase

  CMD34,    // Removes one previously selected sector from the erase selection

  CMD35,    // Sets the address of the first erase group within a range to be selected for erase

  CMD36,    // Sets the address of the last erase group within a
            // continuous range to be selected for erase

  CMD37,    // Removes one previously selected erase group from the erase selection.

  CMD38,    // Erases all previously selected sectors

  CMD42,    // Used to set/reset the password or lock/unlock the
            // card. The size of the Data Block is defined by the SET_BLOCK_LEN command

  CMD55,    // Notifies the card that the next command is an
            // application specific command rather than a standard command.

  CMD56,    // Used either to transfer a Data Block to the card or
            // to get a Data Block from the card for general
            // purpose/application specific commands. The size
            // of the Data Block is defined with SET_BLOCK_LEN command

  CMD58,    // Reads the OCR register of a card

  CMD59,    // Turns the CRC option on or off. A ‘1’ in the CRC
            // option bit will turn the option on, a ‘0’ will turn it off

  ACMD41,   // Activates the card’s initialization process (Only for SD)

  CMD_END   // End of commands index
} MmcSpiCmdInd_t;

typedef enum _MmcAgmType_t
{
  MmcNoArg = 0,MmcBlockLen,MmcDataAdd, MmcDummyWord
} MmcAgmType_t;

typedef enum _MmcRespType_t
{
  MmcR1 = 0,MmcR1b,MmcR2,MmcR3
} MmcRespType_t;

typedef struct _MmcCommads_t
{
  Int8U         TxData;
  MmcAgmType_t  Arg;
  MmcRespType_t Resp;
} MmcCommads_t;

/*************************************************************************
 * Function Name: MmcSendCmd
 * Parameters: MmcSpiCmdInd_t ComdInd,Int32U Arg
 *
 * Return: Int32U
 *
 * Description: MMC commands implement
 *
 *************************************************************************/
Int32U MmcSendCmd(MmcSpiCmdInd_t ComdInd,Int32U Arg);

/*************************************************************************
 * Function Name: MmcSetBlockLen
 * Parameters: Int32U Length
 *
 * Return: Int32U
 *
 * Description: Set Block length Return command's result
 *
 *************************************************************************/
Int32U MmcSetBlockLen(Int32U length);

/*************************************************************************
 * Function Name: MmcInitMedia
 * Parameters: none
 *
 * Return: MmcState_t
 *
 * Description: Mmc detect and initialize
 *
 *************************************************************************/
MmcState_t MmcInitMedia (void);

/*************************************************************************
 * Function Name: MmcReadCardInfo
 * Parameters: pInt8U pData,
 *             MmcSpiCmdInd_t Command - CMD9, CMD10 are only allowed
 *
 * Return: MmcState_t
 *
 * Description: Read CSD or CID  registers depend of command
 *
 *************************************************************************/
MmcState_t MmcReadCardInfo(pInt8U pData, MmcSpiCmdInd_t Command);

/*************************************************************************
 * Function Name: MmcCsdImplemet
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implement data from CSD
 *
 *************************************************************************/
void MmcCsdImplemet (void);

/*************************************************************************
 * Function Name: MmcRead
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: MmcState_t
 *
 * Description: Read from a Mmc
 *
 *************************************************************************/
inline
MmcState_t MmcRead(pInt8U pData, Int32U Add, Int32U Length);

/*************************************************************************
 * Function Name: MmcWrite
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: MmcState_t
 *
 * Description: Write to a Mmc
 *
 *************************************************************************/
inline
MmcState_t MmcWrite(pInt8U pData, Int32U Add, Int32U Length);

/*************************************************************************
 * Function Name: MmcVerify
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: MmcState_t
 *
 * Description: Verify on a Mmc
 *
 *************************************************************************/
inline MmcState_t MmcVerify(pInt8U pData, Int32U Add, Int32U Length);

/*************************************************************************
 * Function Name: MmcGetLastError
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return Last error
 *
 *************************************************************************/
Int32U MmcGetLastError(void);

/*************************************************************************
 * Function Name: MmcStatusUpdate
 * Parameters: none
 *
 * Return: none
 *
 * Description: Update status of SD/MMC card
 *
 *************************************************************************/
void MmcStatusUpdate (void);

/*************************************************************************
 * Function Name: MmcDiskInit
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Init MMC/SD disk
 *
 *************************************************************************/
void MmcDiskInit (void);

/*************************************************************************
 * Function Name: MmcDiskInfo
 * Parameters:  pInt8U pData, DiskInfoType_t DiskInfoType
 *
 * Return: Int32U
 *
 * Description: Return pointer to Info structure of the disk
 * (Inquiry or Format capacity)
 *
 *************************************************************************/
Int32U MmcDiskInfo (pInt8U pData, DiskInfoType_t DiskInfoType);

/*************************************************************************
 * Function Name: MmcDiskStatus
 * Parameters:  none
 *
 * Return: pDiskCtrlBlk_t
 *
 * Description: Return pointer to status structure of the disk
 *
 *************************************************************************/
pDiskCtrlBlk_t MmcDiskStatus (void);

/*************************************************************************
 * Function Name: MmcDiskIO
 * Parameters: pInt8U pData,Int32U BlockStart,
 *             Int32U BlockNum, DiskIoRequest_t IoRequest
 *
 * Return: DiskStatusCode_t
 *
 * Description: MMC/SD disk I/O
 *
 *************************************************************************/
DiskStatusCode_t MmcDiskIO (pInt8U pData,Int32U BlockStart,
                              Int32U BlockNum, DiskIoRequest_t IoRequest);

#endif // __MMC_H
