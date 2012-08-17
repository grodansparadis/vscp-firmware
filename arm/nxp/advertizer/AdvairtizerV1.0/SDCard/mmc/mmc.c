/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : mmc.c
 *    Description : MMC
 *
 *    History :
 *    1. Data        : July 1, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
**************************************************************************/
#define MMC_GLOBAL
#include <global.h>

//#include <sd/mmc/includes.h>
//#include "mmc.h"
//#include "mmc_ll_SPI1.c"
//#include <sd/integer.h>
//#include <sd/diskio.h>
//#include <lpcUART.h>

#define MMC_RET_ERROR(Res)          MmcLastError = Res;return(MmcCardError)
#define MMC_RET_DATA_ERR(Res)       MmcLastError = Res;return(MmcDataError)

#define CSD_GET_TRAN_SPEED_EXP()      (MmcSdCsd[ 3]&0x07)
#define CSD_GET_TRAN_SPEED_MANT()    ((MmcSdCsd[ 3]&0xF8)>>3)
#define CSD_GET_NSAC()                (MmcSdCsd[ 2]        )
#define CSD_GET_TAAC_EXP()            (MmcSdCsd[ 1]&0x7)
#define CSD_GET_TAAC_MANT()          ((MmcSdCsd[ 1]&0xF8)>>3)
#define CSD_GET_R2W_FACTOR()         ((MmcSdCsd[12]&0x1C)>>2)
#define CSD_GET_READ_BL_LEN()         (MmcSdCsd[ 5]&0x0F)
#define CSD_GET_C_SIZE()            (((MmcSdCsd[ 6]&0x03)<<10) + (MmcSdCsd[7]<<2) + ((MmcSdCsd[8]&0xc0)>>6))
#define CSD_GET_C_SIZE_MULT()       (((MmcSdCsd[ 9]&0x03)<<1 ) +((MmcSdCsd[10]&0x80)>>7))
#define CSD_GET_PERM_WRITE_PROTECT() ((MmcSdCsd[14]&0x20)>>5)
#define CSD_GET_TMP_WRITE_PROTECT()  ((MmcSdCsd[14]&0x10)>>4)

const U32 MmcTransfExp[] =
{
     10000UL,
    100000UL,
   1000000UL,
  10000000UL,
         0UL,
         0UL,
         0UL,
         0UL,
};

const U32 MmmcAccessTime [] =
{
 10000000UL,
  1000000UL,
   100000UL,
    10000UL,
     1000UL,
      100UL,
       10UL,
        1UL,
};

const U32 MmcCsdMant[] =
{
  0UL,10UL,12UL,13UL,15UL,
  20UL,25UL,
  30UL,35UL,
  40UL,45UL,
  50UL,55UL,
  60UL,
  70UL,
  80UL,
};

const U32 MmcAccessTimeMant[] =
{
  0UL,100UL,83UL,77UL,67UL,
  50UL,40UL,
  33UL,29UL,
  25UL,22UL,
  20UL,18UL,
  17UL,
  14UL,
  13UL,
};

const MmcCommads_t MmcCmd[CMD_END] =
{
  // CMD0
  {0x40,MmcNoArg    ,MmcR1 },
  // CMD1
  {0x41,MmcNoArg    ,MmcR1 },
  // CMD9
  {0x49,MmcNoArg    ,MmcR1 },
  // CMD10
  {0x4A,MmcNoArg    ,MmcR1 },
  // CMD12
  {0x4C,MmcNoArg    ,MmcR1 },
  // CMD13
  {0x4D,MmcNoArg    ,MmcR2 },
  // CMD16
  {0x50,MmcBlockLen ,MmcR1 },
  // CMD17
  {0x51,MmcDataAdd  ,MmcR1 },
  // CMD18
  {0x52,MmcDataAdd  ,MmcR1 },
  // CMD24
  {0x58,MmcDataAdd  ,MmcR1 },
  // CMD25
  {0x59,MmcDataAdd  ,MmcR1 },
  // CMD27
  {0x5B,MmcNoArg    ,MmcR1 },
  // CMD28
  {0x5C,MmcDataAdd  ,MmcR1b},
  // CMD29
  {0x5D,MmcDataAdd  ,MmcR1b},
  // CMD30
  {0x5E,MmcDataAdd  ,MmcR1 },
  // CMD32
  {0x60,MmcDataAdd  ,MmcR1 },
  // CMD33
  {0x61,MmcDataAdd  ,MmcR1 },
  // CMD34
  {0x62,MmcDataAdd  ,MmcR1 },
  // CMD35
  {0x63,MmcDataAdd  ,MmcR1 },
  // CMD36
  {0x64,MmcDataAdd  ,MmcR1 },
  // CMD37
  {0x65,MmcDataAdd  ,MmcR1 },
  // CMD38
  {0x66,MmcDummyWord,MmcR1b},
  // CMD42
  {0x6A,MmcDummyWord,MmcR1b},
  // CMD55
  {0x77,MmcNoArg    ,MmcR1 },
  // CMD56
  {0x78,MmcNoArg    ,MmcR1 },
  // CMD58
  {0x7A,MmcNoArg    ,MmcR3 },
  // CMD59
  {0x7B,MmcDummyWord,MmcR1 },
  // ACMD41
  {0x69,MmcNoArg    ,MmcR1 }
};

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

DiskStatus_t MmcDskStatus;
U32 MmcLastError,Tnac,Twr;
U8 MmcSdCsd[16];
bool bMmcChanged;
/*************************************************************************
 * Function Name: MmcSendCmd
 * Parameters: MmcSpiCmdInd_t ComdInd,Int32U Arg
 *
 * Return: Int32U
 *
 * Description: MMC commands implemet
 *
 *************************************************************************/
U32 MmcSendCmd(MmcSpiCmdInd_t ComdInd,U32 Arg){
U32 ch = 0xFF;
U32 i;
  // Chip Select
  MmcChipSelect(1);//clr CS
  // Send command code
  MmcTranserByte(MmcCmd[ComdInd].TxData);
  // Send command's argulents
  if(MmcCmd[ComdInd].Arg == MmcNoArg)
  {
    MmcTranserByte(0x00);
    MmcTranserByte(0x00);
    MmcTranserByte(0x00);
    MmcTranserByte(0x00);
  }
  else
  {
    MmcTranserByte(Arg >> 24);
    MmcTranserByte(Arg >> 16);
    MmcTranserByte(Arg >> 8 );
    MmcTranserByte(Arg      );
  }
  // Send CRC
  if(ComdInd == CMD0)
  {
    MmcTranserByte(0x95);
  }
  else
  {
    MmcTranserByte(0xFF);
  }

  for(i = 9; i && (ch == 0xFF); --i){
   ch = MmcTranserByte(0xFF);

  }
  if (i == 0)
  {
    MmcChipSelect(0);
    return((U32)-1);
  }

  switch (MmcCmd[ComdInd].Resp)
  {
  case MmcR1b:
    for (U32 Busy = 0,i = Twr; i && (Busy != 0xFF); --i)
    {
      Busy = MmcTranserByte(0xFF);
    }
  case MmcR1:
    return(ch);
  case MmcR2:
    Arg  = ((U32)ch << 8) & 0x0000FF00;
    Arg |= MmcTranserByte(0xFF) & 0xFF;
//    MmcChipSelect(0);
    return(Arg);
  case MmcR3:
  default:
    Arg  = ((U32)ch << 24) & 0xFF000000;
    Arg |= ((U32)MmcTranserByte(0xFF) << 16) & 0x00FF0000;
    Arg |= ((U32)MmcTranserByte(0xFF) << 8 ) & 0x0000FF00;
    Arg |= MmcTranserByte(0xFF) & 0xFF;
//    MmcChipSelect(0);
    return(Arg);
  }
}

/*************************************************************************
 * Function Name: MmcSetBlockLen
 * Parameters: Int32U Length
 *
 * Return: Int32U
 *
 * Description: Set Block length Return command's result
 *
 *************************************************************************/
U32 MmcSetBlockLen(U32 length)
{
U32 res = MmcSendCmd(CMD16,length);
  MmcChipSelect(0);
  return(res);
}

/*************************************************************************
 * Function Name: MmcInitMedia
 * Parameters: none
 *
 * Return: MmcState_t
 *
 * Description: Mmc detect and initialize
 *
 *************************************************************************/
//2
MmcState_t MmcInitMedia (void)
{
U32 i,res;
  Tnac = 1;
  if(!MmcPresent())
  {
    return(MmcNoPresent);
  }
  // Clock Freq. Identification Mode < 400kHz
  MmcSetClockFreq(IdentificationModeClock);

  // Set maximum time out
  Tnac = IdentificationModeClock/SD_READ_TIME_OUT;

  // Power up cycles
  for(i = 0; i<2; i++)
  {
    // After power up at least 74 clock cycles are required prior to
    // starting bus communication
    MmcChipSelect(0);                                     //CS hi
    for(res = 10; res; --res)
    {
      MmcTranserByte(0xFF);
    }
    // CMD0 (Go to IDLE) to put MMC in SPI mode
//3
    res = MmcSendCmd(CMD0,0);
    MmcChipSelect(0);
    if(res == MMC_ILDE_STATE)
    {
      break;
    }
  }

  if(res != MMC_ILDE_STATE)
  {
    MmcChipSelect(0);
    return(MmcNoResponse);
  }

  // Determinate Card type SD or MMC
//  MmcDskStatus.DiskType = DiskMMC;
  MmcDskStatus.DiskType = DiskSD;
  for(i=100;i;--i)
  {
    MmcChipSelect(0);
    MmcTranserByte(0xFF);
    res = MmcSendCmd(CMD55,0);
    MmcChipSelect(0);
    MmcChipSelect(0);
    MmcTranserByte(0xFF);
    res = MmcSendCmd(ACMD41,0);
    MmcChipSelect(0);
    if(res & MMC_ILLEGAL_CMD)
    {
      // MMC card may be
      // CMD1 for MMC Init sequence
      // will be complete within 500ms
      for (i = 100; i;--i)
      {
        MmcChipSelect(0);
        MmcTranserByte(0xFF);
        res = MmcSendCmd(CMD1,0);
        MmcChipSelect(0);
        if(res == MMC_OK)
        {
          // Init complete
          break;
        }
        MmcDly_1ms(50);
      }
      break;
    }
    if(res == MMC_OK)
    {
      // SD card is find
      MmcDskStatus.DiskType = DiskSD;
      break;
    }
    MmcDly_1ms(50);
  }

  if(i == 0)
  {
    return(MmcNoResponse);
  }

  // Read CSD
  res = MmcReadCardInfo(MmcSdCsd,CMD9);
  if(res != MmcOk)
  {
    // CSD must be always valid
    return(MmcNoResponse);
  }

  // Implement CSD data
  MmcCsdImplemet();

  // Set Block size
  MmcSetBlockLen(MmcDskStatus.BlockSize);

  return(MmcOk);
}

/*************************************************************************
 * Function Name: MmcReadCardInfo
 * Parameters: pInt8U pData,
 *             MmcSpiCmdInd_t Command - CMD9, CMD10 are only allowed
 *
 * Return: MmcState_t
 *
 * Description: Read CSD or CID  registers depend of commoand
 *
 *************************************************************************/
MmcState_t MmcReadCardInfo(U8 *Data, MmcSpiCmdInd_t Command){
U32 i;
U32 res;
  switch(Command)
  {
  case CMD9:
  case CMD10:
    break;
  default:
    return(MmmcParameterError);
  }
  MmcChipSelect(0);
  MmcTranserByte(0xFF);
  res = MmcSendCmd(Command,0);
  if (res == MMC_OK)
  {
    for(i = 8; i ; --i)
    {
      res = MmcTranserByte(0xFF);
      if((res | MMC_DATA_ERR_TOLKEN) == MMC_DATA_ERR_TOLKEN)
      {
        MMC_RET_DATA_ERR(res);
      }
      else if (res == MMC_DATA_TOLKEN)
      {
        for(i = 0; i <16 ; ++i)
        {
          *Data++ = MmcTranserByte(0xFF);
        }
        // CRC receive
        MmcTranserByte(0xFF);
        MmcTranserByte(0xFF);
        MmcChipSelect(0);
        return(MmcOk);
      }
    }
  }
  MmcChipSelect(0);
  MMC_RET_ERROR(res);
}

/*************************************************************************
 * Function Name: MmcCsdImplemet
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implemet data from CSD
 *
 *************************************************************************/
void MmcCsdImplemet (void){
U32 Freq;
  // Calculate SPI max clock
  Freq = MmcTransfExp[CSD_GET_TRAN_SPEED_EXP()] * MmcCsdMant[CSD_GET_TRAN_SPEED_MANT()];
//  if(Freq > (20MHZ))
  if(Freq > (20000000))
  {
//    Freq = (20MHZ);
    Freq = (30000000);
  }
  Freq = MmcSetClockFreq(Freq);
  if(MmcDskStatus.DiskType == DiskMMC)
  {
    // Calculate Time outs
    Tnac = MmmcAccessTime[CSD_GET_TAAC_EXP()] * MmcAccessTimeMant[CSD_GET_TAAC_MANT()];
    Tnac = Freq/Tnac;
    // (2^NSAC)*16 ~ (100*2^NSAC)/8
    Tnac += 1<<(CSD_GET_NSAC()+4);
    // Max time out
    Tnac *= 10;
    Twr   = Tnac * CSD_GET_R2W_FACTOR();
    // Calculate Block size and Block Number
  }
  else
  {
    Tnac = Freq/SD_READ_TIME_OUT;
    Twr  = Freq/SD_WRITE_TIME_OUT;
  }
  MmcDskStatus.BlockSize = 1<<CSD_GET_READ_BL_LEN();
  MmcDskStatus.BlockNumb = (CSD_GET_C_SIZE()+1)*(4<<CSD_GET_C_SIZE_MULT());
  // Set Write Protect
  MmcDskStatus.WriteProtect = MmcWriteProtect() |\
                              CSD_GET_PERM_WRITE_PROTECT() |\
                              CSD_GET_TMP_WRITE_PROTECT();
}

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
MmcState_t MmcRead(U8 *Data, U32 Add, U32 Length){
U32 res,i;
  // For synchronization
  MmcChipSelect(0);
  MmcTranserByte(0xFF);
  res = MmcSendCmd(CMD17,Add);
  if(res == MMC_OK)
  {
    for(i = Tnac; i; --i)
    {
      res = MmcTranserByte(0xFF);
      if((res | MMC_DATA_ERR_TOLKEN) == MMC_DATA_ERR_TOLKEN)
      {
        MMC_RET_DATA_ERR(res);
      }
      else if (res == MMC_DATA_TOLKEN)
      {
        // Receive block
        MmcReceiveBlock((U8 *) Data,Length);
        // CRC receive
        MmcTranserByte(0xFF);
        MmcTranserByte(0xFF);
        MmcChipSelect(0);
        return(MmcOk);
      }
    }
    MmcChipSelect(0);
    return(MmcNoResponse);
  }
  MmcChipSelect(0);
  MMC_RET_ERROR(res);
}

/*************************************************************************
 * Function Name: MmcWrite
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: MmcState_t
 *
 * Description: Write to a Mmc
 *
 *************************************************************************/
inline MmcState_t MmcWrite(U8 *Data, U32 Add, U32 Length){
U32 res,i;
  // For synchronization
  MmcChipSelect(0);
  MmcTranserByte(0xFF);

  res = MmcSendCmd(CMD24,Add);
  if(res == MMC_OK)
  {
    MmcTranserByte(0xFF);
    MmcTranserByte(MMC_DATA_TOLKEN);
    // Send block
    MmcSendBlock((U8 *) Data,Length);
    // CRC Send
    MmcTranserByte(0xFF);
    MmcTranserByte(0xFF);
    if((MmcTranserByte(0xFF) & 0x1F) != 0x05)
    {
      MMC_RET_ERROR(res);
    }

    for(i = Twr; i ;i--)
    {
      if(MmcTranserByte(0xFF) == 0xFF)
      {
        break;
      }
    }
    MmcChipSelect(0);
    if (i == 0)
    {
      return(MmcNoResponse);
    }
    return(MmcOk);
  }
  MmcChipSelect(0);
  MMC_RET_ERROR(res);
}

/*************************************************************************
 * Function Name: MmcVerify
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: MmcState_t
 *
 * Description: Verify on a Mmc
 *
 *************************************************************************/
inline MmcState_t MmcVerify(U8 *Data, U32 Add, U32 Length){
U32 res,i;
  // For synchronization
  MmcChipSelect(0);
  MmcTranserByte(0xFF);
  res = MmcSendCmd(CMD17,Add);
  if(res == MMC_OK)
  {
    for(i = Tnac;i;--i)
    {
      res = MmcTranserByte(0xFF);
      if((res | MMC_DATA_ERR_TOLKEN) == MMC_DATA_ERR_TOLKEN)
      {
        MMC_RET_DATA_ERR(res);
      }
      else if (res == MMC_DATA_TOLKEN)
      {
        res = 0;
        for(i = 0; i<Length;++i,++Data)
        {
          *Data ^= MmcTranserByte(0xFF);
          if (*Data != 0)
          {
            res = 1;
          }
        }
        // CRC receive
        MmcTranserByte(0xFF);
        MmcTranserByte(0xFF);
        MmcChipSelect(0);
        MmcTranserByte(0xFF);
        MmcTranserByte(0xFF);
        if (res)
        {
          return(MmcMiscompare);
        }
        return(MmcOk);
      }
    }
    return(MmcNoResponse);
  }
  MMC_RET_ERROR(res);
}

/*************************************************************************
 * Function Name: MmcGetLastError
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return Last error
 *
 *************************************************************************/
U32 MmcGetLastError(void)
{
  return(MmcLastError);
}

/*************************************************************************
 * Function Name: MmcStatusUpdate
 * Parameters: none
 *
 * Return: none
 *
 * Description: Update status of SD/MMC card
 *
 *************************************************************************/
void MmcStatusUpdate (void)
{
  if(MmcDskStatus.DiskStatus != DiskCommandPass)
  {
    switch (MmcInitMedia())
    {
    case MmcOk:
      MmcCsdImplemet();
      MmcDskStatus.DiskStatus = DiskCommandPass;
      MmcDskStatus.MediaChanged = TRUE;
      break;
    case MmcCardError:
    case MmcDataError:
      MmcDskStatus.DiskStatus = DiskNotReady;
      break;
    default:
      MmcDskStatus.DiskStatus = DiskNotPresent;
      break;
    }
  }
  else if (MmcReadCardInfo(MmcSdCsd,CMD9) != MmcOk)
  {
    MmcDskStatus.DiskStatus = DiskNotReady;
  }
}

/*************************************************************************
 * Function Name: MmcDiskInit
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Init MMC/SD disk
 *
 *************************************************************************/
/*
void MmcDiskInit (void)
{
  MmcDskStatus.BlockNumb =\
  MmcDskStatus.BlockSize =\
  MmcLastError           = 0;
  // Init SPI
  SpiInit();
  // Media Init
  switch (MmcInitMedia())
  {
  case MmcOk:
    MmcCsdImplemet();
    MmcDskStatus.DiskStatus = DiskCommandPass;
    MmcDskStatus.MediaChanged = TRUE;
    break;
  case MmcCardError:
  case MmcDataError:
    MmcDskStatus.DiskStatus = DiskNotReady;
    break;
  default:
    MmcDskStatus.DiskStatus = DiskNotPresent;
    break;
  }
}
*/
/*************************************************************************
 * Function Name: MmcDiskInfo
 * Parameters:  pInt8U pData, DiskInfoType_t DiskInfoType
 *
 * Return: Int32U
 *
 * Description: Return pointer to Info stucture of the disk
 * (Inquiry or Format capacity)
 *
 *************************************************************************/
#if 0
U32 MmcDiskInfo (U8 *Data, DiskInfoType_t DiskInfoType){
  pMmc3FormatCapResponse_t pFormatCapacity;
  switch (DiskInfoType)
  {
  case DiskInquiry:
    memcpy(*Data,MmcDskInquiry,SizeOfInquiryDescMmcDsk);
    return(SizeOfInquiryDescMmcDsk);
  case DiskFormatCapacity:
    pFormatCapacity = (pMmc3FormatCapResponse_t)*Data;
    memset(pFormatCapacity,0,sizeof(Mmc3FormatCapResponse_t));
    pFormatCapacity->CapacityListLength = sizeof(Mmc3FormatCapDescriptor_t);
    if (MmcDskStatus.DiskStatus != DiskCommandPass)
    {
      pFormatCapacity->MaximumDescriptor.DescriptorType    = FormattedMedia;
      pFormatCapacity->MaximumDescriptor.BlockLength[0]    = (MmcDskStatus.BlockSize >> 16) & 0xFF;
      pFormatCapacity->MaximumDescriptor.BlockLength[1]    = (MmcDskStatus.BlockSize >>  8) & 0xFF;
      pFormatCapacity->MaximumDescriptor.BlockLength[2]    = (MmcDskStatus.BlockSize      ) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[0] = (MmcDskStatus.BlockNumb >> 24) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[1] = (MmcDskStatus.BlockNumb >> 16) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[2] = (MmcDskStatus.BlockNumb >>  8) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[3] = (MmcDskStatus.BlockNumb      ) & 0xFF;
    }
    else
    {
      pFormatCapacity->MaximumDescriptor.DescriptorType    = NoMediaPresent;
      pFormatCapacity->MaximumDescriptor.BlockLength[0]    = (2048       >> 16) & 0xFF;
      pFormatCapacity->MaximumDescriptor.BlockLength[1]    = (2048       >>  8) & 0xFF;
      pFormatCapacity->MaximumDescriptor.BlockLength[2]    = (2048            ) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[0] = (0xFFFFFFFF >> 24) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[1] = (0xFFFFFFFF >> 16) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[2] = (0xFFFFFFFF >>  8) & 0xFF;
      pFormatCapacity->MaximumDescriptor.NumberofBlocks[3] = (0xFFFFFFFF      ) & 0xFF;
    }
    return(sizeof(Mmc3FormatCapResponse_t));
  }
  return(0);
}
#endif
/*************************************************************************
 * Function Name: MmcDiskStatus
 * Parameters:  none
 *
 * Return: pDiskStatus_t
 *
 * Description: Return pointer to status stucture of the disk
 *
 *************************************************************************/
pDiskStatus_t MmcDiskStatus (void)
{
  return(&MmcDskStatus);
}

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
DiskStatusCode_t MmcDiskIO (U8 *Data,U32 BlockStart, U32 BlockNum, DiskIoRequest_t IoRequest){
  if((*Data == NULL) || (BlockStart+BlockNum > MmcDskStatus.BlockNumb))
  {
    return(DiskParametersError);
  }
  if (MmcDskStatus.DiskStatus)
  {
    return(MmcDskStatus.DiskStatus);
  }
  switch (IoRequest)
  {
  case DiskWrite:
    if(MmcDskStatus.WriteProtect)
    {
      return(DiskParametersError);
    }
    switch (MmcWrite((U8 *) Data,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
    {
    case MmcOk:
      break;
    case MmcCardError:
    case MmcDataError:
      MmcDskStatus.DiskStatus = DiskNotReady;
      break;
    default:
      MmcDskStatus.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  case DiskRead:
    switch (MmcRead((U8 *)Data,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
    {
    case MmcOk:
      break;
    case MmcCardError:
    case MmcDataError:
      MmcDskStatus.DiskStatus = DiskNotReady;
      break;
    default:
      MmcDskStatus.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  case DiskVerify:
    switch (MmcVerify((U8 *)Data,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
    {
    case MmcOk:
      break;
    case MmcMiscompare:
      return(DiskMiscompareError);
    case MmcCardError:
    case MmcDataError:
      MmcDskStatus.DiskStatus = DiskNotReady;
      break;
    default:
      MmcDskStatus.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  default:
    return(DiskParametersError);
  }
  return(MmcDskStatus.DiskStatus);
}


/*-----------------------------------------------------------------------*/
/* Public Functions                                                      */


/*-----------------------*/
/* Initialize Disk Drive */
/* (Platform dependent)  */

DSTATUS disk_initialize(){
  MmcDskStatus.BlockNumb =\
  MmcDskStatus.BlockSize =\
  MmcLastError           = 0;
  // Init SPI
  SSpiInit();
  // Media Init
  switch (MmcInitMedia())
  {
  case MmcOk:
    MmcCsdImplemet();
    MmcDskStatus.DiskStatus = DiskCommandPass;
    MmcDskStatus.MediaChanged = TRUE;
    Stat = 0;
    if(MmcDskStatus.WriteProtect) Stat |= STA_PROTECT;
//    debug_printf("\nMmcOk");
    break;
  case MmcCardError:
    debug_printf("\nMmcCardError");
  case MmcDataError:
    debug_printf("\nMmcDataError");
    MmcDskStatus.DiskStatus = DiskNotReady;
    Stat = STA_NOINIT;
    if(MmcDskStatus.WriteProtect) Stat |= STA_PROTECT;
    break;
  default:
//    debug_printf("\ndefault");
    MmcDskStatus.DiskStatus = DiskNotPresent;
    Stat = STA_NODISK;
    break;
  }
//  debug_printf("\nStat %d", Stat);
  return Stat;
}



/*-----------------------*/
/* Shutdown              */
/* (Platform dependent)  */


DSTATUS disk_shutdown ()
{
	Stat |= STA_NOINIT;

	return Stat;
}



/*--------------------*/
/* Return Disk Status */

DSTATUS disk_status (){
	return Stat;
}



/*----------------*/
/* Read Sector(s) */

DRESULT disk_read (
	BYTE *buff,			/* Data buffer to store read data */
	DWORD sector,		        /* Sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
U32 res;

        if (Stat & STA_NOINIT) return RES_NOTRDY;
        if (!count) return RES_PARERR;
        
        res = MmcRead(buff,sector*MmcDskStatus.BlockSize,count*MmcDskStatus.BlockSize);

        if(res == MMC_OK)
	  return RES_OK;
        else
          return RES_ERROR; 
}


/*-----------------*/
/* Write Sector(s) */

#ifndef _READONLY
DRESULT disk_write (
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
U32 res;

        if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;
	if (!count) return RES_PARERR;

        res = MmcWrite((U8 *) buff,sector*MmcDskStatus.BlockSize,count*MmcDskStatus.BlockSize);

        if(res == MMC_OK)
	  return RES_OK;
        else
          return RES_ERROR; 
}
#endif


void dump (BYTE *buff, DWORD ofs, BYTE cnt)
{
	BYTE n;


	uart0Write("\n\r%08lX ", ofs);
	for(n = 0; n < cnt; n++)
		uart0Write(" %02X", buff[n]);
	uart0Putch(' ');
	for(n = 0; n < cnt; n++) {
		if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
			uart0Putch('.');
		else
			uart0Putch(buff[n]);
	}
}

/*--------------------------*/
/* Miscellaneous Functions  */

DRESULT disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
//	BYTE csd[16], i, *p;
//	WORD csize_mult, csize;

	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
#if 0
              case GET_SECTORS :	/* Get number of sectors on the disk (unsigned long) */

                        p = &csd[0];
                        if(MmcReadCardInfo(p,CMD9) == MmcOk)
                        {
                          /* Calculate block count */
                          csize_mult = ((WORD)(csd[9] & 3)    <<  1) | ((WORD)(csd[10] & 0x80) >> 7);
                          csize      = ((WORD)(csd[6] & 0x03) << 10) | ((WORD)(csd[7] << 2)) | ((WORD)(csd[8] & 0xC0) >> 6);

                          *(DWORD*)buff = (csize+1) * csize_mult;

                          /*
                          C_SIZE = 3843
                          C_SIZE_MULT = 4
                          READ_BL_LEN = 512
                          
                          memory capacity = BLOCKNR * BLOCK_LEN
                          Where:
                          BLOCKNR = (C_SIZE+1) * (2^(C_SIZE_MULT+2)) (3843+1)*64 // 246,016
                          MULT = 2^(C_SIZE_MULT+2) // (C_SIZE_MULT < 8)
                          BLOCK_LEN = 2^READ_BL_LEN // (READ_BL_LEN < 12)
                          */

                          //  csize = ((WORD)(csd[8] & 3) >> 6) + (WORD)(csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;

                          // *(DWORD*)buff = (DWORD)csize * csm;
                        }
                        res = RES_OK;
                        break;
#define PUT_IDLE_STATE	        2
#endif
		case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */

                        if(MmcReadCardInfo(buff,CMD9) == MmcOk)
    			  res = RES_OK;
			break;

		case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */

                        if(MmcReadCardInfo(buff,CMD10) == MmcOk)
    			  res = RES_OK;
			break;
# if 0
		case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
                        MmcChipSelect(0);
                        MmcTranserByte(0xFF);
                        // Chip Select
                        MmcChipSelect(1);
                        // Send command code
                        MmcTranserByte(MmcCmd[CMD58].TxData);
//                        res = MmcSendCmd(CMD58,0);
//                        printf("res=%d\n\r",res);
//                        if (res == MMC_OK) {
                          for(i = 0, p = buff; i <4 ; ++i)
                            *p++ = MmcTranserByte(0xFF);
                          MmcChipSelect(0);
                          res = RES_OK;
                       // }
                        break;
#define ATA_GET_REV		20
#define ATA_GET_MODEL	        21
#define ATA_GET_SN		22
#endif
                default:
			res = RES_PARERR;
	}


	return res;
}

/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2003
 *
 *    File name   : mmc_ll.h
 *    Description : define MMC module
 *
 *    History :
 *    1. Data        : July 1, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Data        : July 1, 2005
 *       Author      : Stanimir Bonev
 *       Description : Modify
 *        Fix a lock problem in MmcReceiveBlock
 *
 *    $Revision: 1.4 $
**************************************************************************/
#define SSP_FIFO_SIZE 8

/*************************************************************************
 * Function Name: MmcChipSelect
 * Parameters: Boolean Select
 * Return: none
 *
 * Description: Mmc Chip select control
 * Select = true  - Chip is enable
 * Select = false - Chip is disable
 *
 *************************************************************************/
void MmcChipSelect (bool Select){
  if (Select){
      F_IO0CLR = CS_SDCARD;
  }
  else
  {
    F_IO0SET = CS_SDCARD;
    // Synchronization
    while(!SSP_SR & TNF);
    SSP_DR = 0xFF;
    // Wait until tx fifo and tx shift bufer are empty
//   U32  timeout = 0;
    while(SSP_SR & BSY){
//            timeout++;
//            if (timeout > 100000)
//                break;
            };
    
    while(!SSP_SR & RNE);
    do
    {
      Select = SSP_DR;
    }
    while(SSP_SR & RNE);
  }
}
/*************************************************************************
 * Function Name: MmcPresent
 * Parameters: none
 * Return: Boolean - true cart present
 *                 - false cart no present
 *
 * Description: Mmc precent check
 *
 *************************************************************************/
inline bool MmcPresent (void){
  return(true);
}

/*************************************************************************
 * Function Name: MmcWriteProtect
 * Parameters: none
 * Return: Boolean - true cart is protected
 *                 - false cart no protected
 *
 * Description: Mmc Write protect check
 *
 *************************************************************************/
inline bool MmcWriteProtect (void)
{
  return(FALSE);
}

/*************************************************************************
 * Function Name: MmcSetClockFreq
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Set SPI ckl frequency
 *
 *************************************************************************/
U32 MmcSetClockFreq (U32 Frequency){
U32 Div = PCLK/Frequency;
  // Check min clk divider (for master mode >= 2)
  if (Div < 2)
  {
    Div = 2;
  }
  // Check max clk divider <= 254
  else if (Div > 254)
  {
    Div = 254;
  }
  // Because Bit 0 is not implement
  ++Div; Div &= ~1;
//  SSPCPSR_bit.CPSDVSR = Div;
//  SSP_CR1 = 0x00;     
  SSP_CPSR = Div;
//  SSP_CR1 |= SSE;             // Enable SSP module

  // Return real frequency
  return(PCLK/Div);
}

/*************************************************************************
 * Function Name: SpiInit
 * Parameters: none
 * Return: int
 * 0 - no error
 * 1 - speed is to high
 *
 * Description: Init SPI, Cart Present, Write Protect and Chip select pins
 * wird schon in lcd gemacht (lcd.lcd_ssp_init)
 *************************************************************************/

void SSpiInit (void){
//  U32 i;
//  volatile U32 Dummy;
//  SSP_CR1 = 0x00;                                       /* SSP master (off) in normal mode */
//  PINSEL1 |=  (2<<2) | (2<<4) | (2<<6) &  ~(3<<8) ;     // Map the SPI functionality to pins
//  F_IO0SET = CS_SDCARD;                                 /* set SSEL to high */
//  VPBDIV = 0x02;                                        /* Set PCLK 1/2 of CCLK */ 
//  SSP_CR0 = 0x07;//0x0707;                                     /* Set data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */ 
//  SSP_CPSR = 0x2;                                       /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02*/ 
//  SSP_CR1 |= SSE;             // Enable SSP module

// Chip select
//  IO0SET_bit.P0_20  = 1;

//  IO0DIR_bit.P0_20  = 1;
  // Cart present
  // Spi init
//  PM_OpenPeripheral(PC_PCSPI1);
//  PCONP |= PC_PCSPI1;
//  SSP_CR0  = 0x7;
//  SSP_CR1  = 0;
//  SSP_IMSC = 0;
// Clock Freq. Identification Mode < 400kHz
//  MmcSetClockFreq(400000);//400KHZ
//  PINSEL1_bit.P0_17 = PINSEL1_bit.P0_18 = PINSEL1_bit.P0_19 = 2;
//  PINSEL1_bit.P0_20 = 0;
//  SSPCR1_bit.SSE = 1; // Enable SPI
//SSP_CR1  = 0x00;            // SSP Master
//SSP_CR1 |= SSE;             // Enable SSP module
/*****/
//  for ( i = 0; i < 8; i++ )
//  {
//    while(!SSP_SR & RNE);
//    Dummy = SSP_DR; // clear the RxFIFO
//  }

}

/*************************************************************************
 * Function Name: MmcTranserByte
 * Parameters: Int8U ch
 * Return: Int8U
 *
 * Description: Send byte to SPI
 *
 *************************************************************************/
U8 MmcTranserByte (U8 ch)
{
  while(!SSP_SR & TNF);
  SSP_DR = ch;
  while(SSP_SR & BSY);//RX FIFO not empty
//  while(!SSP_SR & RNE);//RX FIFO not empty
//  U8 Dummy = (U8)SSP_DR;
//     BYTE data = SSP_DR;
//     debug_printf("ch %d\n ",Dummy);
  return((U8)SSP_DR);
}

/*************************************************************************
 * Function Name: MmcSendBlock
 * Parameters: pInt8U pData, Int32U Size
 *
 * Return: void
 *
 * Description: Send Block to SPI
 *
 *************************************************************************/
void MmcSendBlock (U8 *Data, U32 Size)
{
U32 OutCount = Size;
  while (OutCount)
  {
    while(SSP_SR & TNF && OutCount)
    {
      SSP_DR = *Data++;
      --OutCount;
    }
  }
  while (SSP_SR & RNE || !SSP_SR & TFE)
  {
    volatile U32 Dummy = SSP_DR;
  }
}

/*************************************************************************
 * Function Name: MmcReceiveBlock
 * Parameters: pInt8U pData, Int32U Size
 *
 * Return: void
 *
 * Description: Read Block from SPI
 *
 *************************************************************************/
void MmcReceiveBlock (U8 *Data, U32 Size)
{
 U32 Delta = 0;
  while (Size || Delta)
  {
    while(SSP_SR & TNF && (Delta < SSP_FIFO_SIZE) && Size)
    {
      SSP_DR = 0xFF;
      --Size; ++Delta;
    }
    while (SSP_SR & RNE)
    {
      *Data++ = SSP_DR;
      --Delta;
    }
  }
}

/*************************************************************************
 * Function Name: MmcDly_1ms
 * Parameters: Int32U Delay
 * Return: none
 *
 * Description: Delay [msec]
 *
 *************************************************************************/
void MmcDly_1ms (U32 Delay)
{
pause(ONE_MS * Delay);
}

