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
 *    1. Date        : July 1, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *    2. Date        : September 26, 2006
 *       Author      : Stanimir Bonev
 *       Description : modify
 *      Correct calculation of the Tnac and the Twr.
 *    $Revision: 15135 $
 **************************************************************************/
#define MMC_GLOBAL
#include "mmc.h"
#include "mmc_ll_SPI1.c"

#define MMC_RET_ERROR(Res)          MmcLastError = Res;return(MmcCardError)
#define MMC_RET_DATA_ERR(Res)       MmcLastError = Res;return(MmcDataError)

#define CSD_GET_TRAN_SPEED_EXP()      (MmcSdCsd[ 3]&0x07)
#define CSD_GET_TRAN_SPEED_MANT()    ((MmcSdCsd[ 3]&0xF8)>>3 )
#define CSD_GET_NSAC()                (MmcSdCsd[ 2]          )
#define CSD_GET_TAAC_EXP()            (MmcSdCsd[ 1]&0x7)
#define CSD_GET_TAAC_MANT()          ((MmcSdCsd[ 1]&0xF8)>>3 )
#define CSD_GET_R2W_FACTOR()         ((MmcSdCsd[12]&0x1C)>>2 )
#define CSD_GET_READ_BL_LEN()         (MmcSdCsd[ 5]&0x0F)
#define CSD_GET_C_SIZE()            (((MmcSdCsd[ 6]&0x03)<<10) + (MmcSdCsd[7]<<2) + ((MmcSdCsd[8]&0xc0)>>6))
#define CSD_GET_C_SIZE_MULT()       (((MmcSdCsd[ 9]&0x03)<<1 ) +((MmcSdCsd[10]&0x80)>>7))
#define CSD_GET_PERM_WRITE_PROTECT() ((MmcSdCsd[14]&0x20)>>5 )
#define CSD_GET_TMP_WRITE_PROTECT()  ((MmcSdCsd[14]&0x10)>>4 )

const Int32U MmcTransfExp[] =
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

const Int32U MmmcAccessTime [] =
{
        1UL,
       10UL,
      100UL,
     1000UL,
    10000UL,
   100000UL,
  1000000UL,
 10000000UL,
};

const Int32U MmcCsdMant[] =
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

DiskCtrlBlk_t MmcDskStatus;
Int32U MmcLastError,Tnac,Twr;
Int8U MmcSdCsd[16];
Boolean bMmcChanged;
static Boolean bMmcPermWriteProtect;

/*************************************************************************
 * Function Name: MmcSendCmd
 * Parameters: MmcSpiCmdInd_t ComdInd,Int32U Arg
 *
 * Return: Int32U
 *
 * Description: MMC commands implement
 *
 *************************************************************************/
Int32U MmcSendCmd(MmcSpiCmdInd_t ComdInd,Int32U Arg)
{
Int32U ch = 0xFF;
Int32U i;
  // Chip Select
  MmcChipSelect(1);
  // Send command code
  MmcTranserByte(MmcCmd[ComdInd].TxData);
  // Send command's arguments
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
  // wait for command response
  for(i = 9; i && (ch == 0xFF); --i) ch = MmcTranserByte(0xFF);

  if (i == 0)
  {
    MmcChipSelect(0);
    return((Int32U)-1);
  }

  // Implement command response
  switch (MmcCmd[ComdInd].Resp)
  {
  case MmcR1b:
    for (Int32U Busy = 0,i = Twr; i && (Busy != 0xFF); --i)
    {
      Busy = MmcTranserByte(0xFF);
    }
  case MmcR1:
    return(ch);
  case MmcR2:
    Arg  = ((Int32U)ch << 8) & 0x0000FF00;
    Arg |= MmcTranserByte(0xFF) & 0xFF;
    return(Arg);
  case MmcR3:
  default:
    Arg  = ((Int32U)ch << 24) & 0xFF000000;
    Arg |= ((Int32U)MmcTranserByte(0xFF) << 16) & 0x00FF0000;
    Arg |= ((Int32U)MmcTranserByte(0xFF) << 8 ) & 0x0000FF00;
    Arg |= MmcTranserByte(0xFF) & 0xFF;
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
Int32U MmcSetBlockLen(Int32U length)
{
Int32U res = MmcSendCmd(CMD16,length);
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
MmcState_t MmcInitMedia (void)
{
Int32U i,res;
  Tnac = 1;
  if(!MmcPresent())
  {
    return(MmcNoPresent);
  }
  // Clock Freq. Identification Mode < 400kHz
  MmcSetClockFreq(IdentificationModeClock);

  // After power up at least 74 clock cycles are required prior to
  // starting bus communication
  MmcChipSelect(0);
  for(i = 10; i; --i) MmcTranserByte(0xFF);

  // CMD0 (Go to IDLE) to put MMC in SPI mode
  res = MmcSendCmd(CMD0,0);
  MmcChipSelect(0);
  if(res != MMC_ILDE_STATE)
  {
    return(MmcNoResponse);
  }

  // Determinate Card type SD or MMC
  MmcDskStatus.DiskType = DiskMMC;
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

  MmcCsdImplemet();
  return(MmcOk);
}

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
MmcState_t MmcReadCardInfo(pInt8U pData, MmcSpiCmdInd_t Command)
{
Int32U i;
Int32U res;
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
        // Read CSD or CID data
        for(i = 0; i <16 ; ++i)
        {
          *pData++ = MmcTranserByte(0xFF);
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
 * Description: Implement data from CSD
 *
 *************************************************************************/
void MmcCsdImplemet (void)
{
Int32U Freq;
Int64U Tmp;
  // Calculate SPI max clock
  Freq = MmcTransfExp[CSD_GET_TRAN_SPEED_EXP()] * MmcCsdMant[CSD_GET_TRAN_SPEED_MANT()];
  Freq = MmcSetClockFreq(Freq);
  if(MmcDskStatus.DiskType == DiskMMC)
  {
    // Calculate Time outs for MMC cards
    Tmp = MmmcAccessTime[CSD_GET_TAAC_EXP()] * MmcCsdMant[CSD_GET_TAAC_MANT()];
    Tmp /= 10000; // us
    // Freq [Hz], Tmp[1 us], *10
    Tmp = (Freq*Tmp)/100000LL;
    // NSAC*100*10
    Tmp += 1000*CSD_GET_NSAC();
    // Max time out
    Tnac = Tmp;
    Twr  = Tmp * (1<<CSD_GET_R2W_FACTOR());
  }
  else
  {
    // Calculate Time outs for SD cards
    // Freq [Hz], RD_TIME_OUT[ms]
    Tmp = Freq/1000;
    Tnac = Tmp * RD_TIME_OUT;
    Twr  = Tmp * WR_TIME_OUT;
  }
  // Calculate Block size and Block Number
  MmcDskStatus.BlockSize = 1<<CSD_GET_READ_BL_LEN();
  MmcDskStatus.BlockNumb = (CSD_GET_C_SIZE()+1)*(4<<CSD_GET_C_SIZE_MULT());
  // Set Write Protect
  bMmcPermWriteProtect = CSD_GET_PERM_WRITE_PROTECT() |\
                         CSD_GET_TMP_WRITE_PROTECT();
  MmcDskStatus.WriteProtect = MmcWriteProtect() |\
                              bMmcPermWriteProtect;
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
MmcState_t MmcRead(pInt8U pData, Int32U Add, Int32U Length)
{
Int32U res,i;
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
        MmcReceiveBlock(pData,Length);
        // CRC receive
        MmcTranserByte(0xFF);
        MmcTranserByte(0xFF);
        MmcChipSelect(0);
        return(MmcOk);
      }
    }
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
inline
MmcState_t MmcWrite(pInt8U pData, Int32U Add, Int32U Length)
{
Int32U res,i;
  // For synchronization
  MmcChipSelect(0);
  MmcTranserByte(0xFF);

  res = MmcSendCmd(CMD24,Add);
  if(res == MMC_OK)
  {
    MmcTranserByte(0xFF);
    MmcTranserByte(MMC_DATA_TOLKEN);
    // Send block
    MmcSendBlock(pData,Length);
    // CRC Send
    MmcTranserByte(0xFF);
    MmcTranserByte(0xFF);
    res = MmcTranserByte(0xFF) & 0x1F;
    if(res != 0x05)
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

    if (i == 0)
    {
      return(MmcNoResponse);
    }
    MmcChipSelect(0);
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
inline
MmcState_t MmcVerify(pInt8U pData, Int32U Add, Int32U Length)
{
Int32U res,i;
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
        for(i = 0; i<Length;++i,++pData)
        {
          *pData ^= MmcTranserByte(0xFF);
          if (*pData != 0)
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
Int32U MmcGetLastError(void)
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
  // Update WP state
  MmcDskStatus.WriteProtect = MmcWriteProtect() |\
                              bMmcPermWriteProtect;
  if((MmcDskStatus.DiskStatus != DiskCommandPass) | !MmcPresent())
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
void MmcDiskInit (void)
{
  MmcDskStatus.BlockNumb =\
  MmcDskStatus.BlockSize =\
  MmcLastError           = 0;
  // Init SPI
  MmcInit();
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
#if defined(MMC_DISK_INFO)
Int32U MmcDiskInfo (pInt8U pData, DiskInfoType_t DiskInfoType)
{
pMmc3FormatCapResponse_t pFormatCapacity;
  switch (DiskInfoType)
  {
  case DiskInquiry:
    memcpy(pData,MmcDskInquiry,SizeOfInquiryDescMmcDsk);
    return(SizeOfInquiryDescMmcDsk);
  case DiskFormatCapacity:
    pFormatCapacity = (pMmc3FormatCapResponse_t)pData;
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
#endif // defined(SCSI_DEV)

/*************************************************************************
 * Function Name: MmcDiskStatus
 * Parameters:  none
 *
 * Return: pDiskCtrlBlk_t
 *
 * Description: Return pointer to status structure of the disk
 *
 *************************************************************************/
pDiskCtrlBlk_t MmcDiskStatus (void)
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
DiskStatusCode_t MmcDiskIO (pInt8U pData,Int32U BlockStart,
                              Int32U BlockNum, DiskIoRequest_t IoRequest)
{
  if((pData == NULL) || (BlockStart+BlockNum > MmcDskStatus.BlockNumb))
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
    switch (MmcWrite(pData,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
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
    switch (MmcRead(pData,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
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
    switch (MmcVerify(pData,BlockStart*MmcDskStatus.BlockSize,BlockNum*MmcDskStatus.BlockSize))
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

