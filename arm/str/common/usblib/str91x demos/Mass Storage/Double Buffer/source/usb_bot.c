/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_bot.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : BOT State Machine management 
********************************************************************************
* History:
* 10/01/2006 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "memory.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "usb_scsi.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 Bot_State;
Bulk_Only_CBW CBW;
Bulk_Only_CSW CSW;
/* Extern variables ----------------------------------------------------------*/
extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern u32 Data_Len;
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
extern void Send_Data_Request(u8* Data_Address,u16 Data_Length);
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Mass_Storage_In
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_In (void)
{
 u8 CMD;
  CMD = CBW.CB[0];
  switch (Bot_State) {
    case BOT_DATA_IN:
      switch (CMD) {
        case SCSI_READ10:
       SCSI_Read10_Cmd();
        break;
      }
      break;
    case BOT_DATA_IN_LAST:
      Set_CSW (CSW_CMD_PASSED);
      break;
    case BOT_ERROR:
      Bot_State = BOT_IDLE;
      SetEPRxStatus(ENDP2, EP_RX_VALID);
      break;
    case BOT_CSW_Send:
      Bot_State = BOT_IDLE;
      break;
    default:
      break;
  }
}
/*******************************************************************************
* Function Name  : Mass_Storage_Out
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_Out (void)
{
 u8 CMD;
  CMD = CBW.CB[0];
  switch (Bot_State) {
    case BOT_IDLE:
      CBW_Decode();
      break;
    case BOT_DATA_OUT:
      if (CMD==SCSI_WRITE10) {
         SCSI_Write10_Cmd();
          break;
      }
     Bot_Abort(DIR_OUT);
     Set_Scsi_Sense_Data(ILLEGAL_REQUEST,INVALID_FIELED_IN_COMMAND);
     Set_CSW (CSW_PHASE_ERROR);
      break;
    default:
     Bot_Abort(DIR_OUT);
     Set_Scsi_Sense_Data(ILLEGAL_REQUEST,INVALID_FIELED_IN_COMMAND);
     Set_CSW (CSW_CMD_FAILED);
       break;
  }
}

/*******************************************************************************
* Function Name  : CBW_Decode
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void CBW_Decode(void)
{
u32 Counter;

  for (Counter = 0; Counter < Data_Len; Counter++) {
    *((u8 *)&CBW + Counter) = Bulk_Data_Buff[Counter];
  }
  CSW.dTag = CBW.dTag;
  CSW.dDataResidue = CBW.dDataLength;
  if (Data_Len != BOT_CBW_PACKET_LENGTH)
  {
    Bot_Abort(BOTH_DIR);
    CBW.dSignature = 0; /* reset the CBW.dSignature to desible the clear feature until receiving a Mass storage reset*/
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST,PARAMETER_LIST_LENGTH_ERROR);
    Set_CSW (CSW_CMD_FAILED);
    return;
  }
  if (CBW.dSignature == BOT_CBW_SIGNATURE) {
    /* Valid CBW */
   if ((CBW.bLUN != 0) || (CBW.bCBLength < 1) || CBW.bCBLength > 16) {
     Bot_Abort(BOTH_DIR);
     Set_Scsi_Sense_Data(ILLEGAL_REQUEST,INVALID_FIELED_IN_COMMAND);
     Set_CSW (CSW_CMD_FAILED);
    } else {
      switch (CBW.CB[0]) {
        case SCSI_TEST_UNIT_READY:
          SCSI_TestUnitReady_Cmd();
          break;
        case SCSI_REQUEST_SENSE:
          SCSI_RequestSense_Cmd ();
          break;
        case SCSI_INQUIRY:
          SCSI_Inquiry_Cmd();
          break;
        case SCSI_START_STOP_UNIT:
          SCSI_Start_Stop_Unit_Cmd();
          break;
        case SCSI_ALLOW_MEDIUM_REMOVAL:
          SCSI_Prevent_Removal_Cmd();
          break;
        case SCSI_MODE_SENSE6:
          SCSI_ModeSense6_Cmd ();
          break;
        case SCSI_MODE_SENSE10:
	      SCSI_ModeSense10_Cmd ();
          break;
        case SCSI_READ_FORMAT_CAPACITIES:
          SCSI_ReadFormatCapacity_Cmd();
          break;
        case SCSI_READ_CAPACITY10:
          SCSI_ReadCapacity10_Cmd();
          break;
        case SCSI_READ10:
          SCSI_Read10_Cmd();
          break;
        case SCSI_WRITE10:
          SCSI_Write10_Cmd();
          break;
        case SCSI_VERIFY10:
          SCSI_Verify10_Cmd();
          break;
          /*Unsupported command*/

        case SCSI_MODE_SELECT10:
             SCSI_Mode_Select10_Cmd();
             break;
        case SCSI_MODE_SELECT6:
             SCSI_Mode_Select6_Cmd();
             break;
        case SCSI_FORMAT_UNIT:
             SCSI_FormatUnit_Cmd();
             break;
        case SCSI_SEND_DIAGNOSTIC:
             SCSI_Send_Diagnostic_Cmd();
             break;
        case SCSI_READ6:
             SCSI_Read6_Cmd();
             break;
        case SCSI_READ12:
             SCSI_Read12_Cmd();
             break;
        case SCSI_READ16:
             SCSI_Read16_Cmd();
             break;
        case SCSI_READ_CAPACITY16:
             SCSI_READ_CAPACITY16_Cmd();
             break;
        case SCSI_WRITE6:
             SCSI_Write6_Cmd();
             break;
         case SCSI_WRITE12:
             SCSI_Write12_Cmd();
             break;     
        case SCSI_WRITE16:
             SCSI_Write16_Cmd();
             break;
        case SCSI_VERIFY12:
             SCSI_Verify12_Cmd();
             break;
        case SCSI_VERIFY16:
             SCSI_Verify16_Cmd();
             break;
        default:{
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(ILLEGAL_REQUEST,INVALID_COMMAND);
          Set_CSW (CSW_CMD_FAILED);
          break;
        }
      }
    }
  } else {
    /* Invalid CBW */
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST,INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED);
   }
}
/*******************************************************************************
* Function Name  : Transfer_Data_Request
* Description    :
* Input          :
*      u8* Data_Address : point to the data to transfer.
*      u16 Data_Length : the nember of Bytes to transfer.
* Output         : None.
* Return         : None.
*******************************************************************************/

void Transfer_Data_Request(u8* Data_Pointer, u16 Data_Len)
{
  Send_Data_Request(Data_Pointer, Data_Len);
  Bot_State = BOT_DATA_IN_LAST;
  CSW.dDataResidue -= Data_Len;
  CSW.bStatus = CSW_CMD_PASSED;
}
/*******************************************************************************
* Function Name  : Set_CSW
* Description    :
* Input          : u8 CSW_Status this filed can be CSW_CMD_PASSED,CSW_CMD_FAILED,
                  or CSW_PHASE_ERROR.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_CSW (u8 CSW_Status)
{
  CSW.dSignature = BOT_CSW_SIGNATURE;
  CSW.bStatus = CSW_Status;
  Bot_State = BOT_CSW_Send;
  if (CSW_Status!=CSW_CMD_PASSED)
  {
     ClearDTOG_RX(ENDP1);
     ClearDTOG_TX(ENDP1); /* clear the data toggle bits for the endpoint IN*/
     Bot_State = BOT_ERROR;
  }
  Send_Data_Request((u8 *)&CSW,CSW_DATA_LENGTH); 
}

void Bot_Abort(u8 Direction)
{
  switch (Direction){
case DIR_IN :
  SetDouBleBuffEPStall(ENDP1,EP_DBUF_IN);
   break;
case DIR_OUT :
   SetDouBleBuffEPStall(ENDP2,EP_DBUF_OUT);
   FreeUserBuffer(ENDP2,EP_DBUF_OUT);
  break;
case BOTH_DIR :
  SetDouBleBuffEPStall(ENDP1,EP_DBUF_IN);
  SetDouBleBuffEPStall(ENDP2,EP_DBUF_OUT);
  FreeUserBuffer(ENDP2,EP_DBUF_OUT);
  break;
default:
  return;
  }
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
