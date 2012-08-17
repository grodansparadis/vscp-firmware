
#define UART_FRAME_LEN                23
#define UART_RX_BUFF_SIZE             64
#define UART_TX_BUFF_SIZE             16
#define UART_RX_BUFF_MASK             UART_RX_BUFF_SIZE - 1
#define UART_TX_BUFF_MASK             UART_TX_BUFF_SIZE - 1


typedef struct {
  u8  Data[UART_FRAME_LEN];
  u8  Cnt;  
}RXTX_UART_FRAME;

// dirbam su UART1
#define RXTX_UART1

#define DLE		0x10
#define STX		0x02
#define ETX		0x03

#define INCOMING_STATE_NONE		0	// Waiting for <STX>
#define INCOMING_STATE_STX		1	// Reading data
#define INCOMING_STATE_ETX		2	// <ETX> has been received

#define INCOMING_SUBSTATE_NONE	        0	// Idle
#define INCOMING_SUBSTATE_DLE	        1	// <DLE> received

#define OUTGOING_STATE_NONE		0	// Waiting for CAN packet

// VSCP over RS-232 frame format positions
#define FRAME_OPERATION                 1
#define FRAME_FLAGS                     2
#define FRAME_CHANNEL                   3
#define FRAME_SEQ_NUMBER                4
#define FRAME_COMMAND_MSB               5
#define FRAME_COMMAND_LSB               6
#define FRAME_ERROR_MSB                 5
#define FRAME_ERROR_LSB                 6
#define FRAME_BEGIN_DATA_BLOCK          7
#define FRAME_END_DATA_BLOCK           22
#define FRAME_CHECKSUM                 23 

// Frame operation commands
#define CMD_NO_OPERATION                0
#define CMD_LEVEL1_EVENT                1
#define CMD_LEVEL2_EVENT                2
#define CMD_POLL_FOR_EVENT              3
#define CMD_NO_EVENTS                   4
#define CMD_CAN_FRAME                   5
#define CMD_RESERVED                   10
#define CMD_SENT_ACK                  249
#define CMD_SENT_NACK                 250
#define CMD_ACK                       251
#define CMD_NACK                      252
#define CMD_ERROR                     253
#define CMD_COMMAND_REPLAY            254
#define CMD_COMMAND                   255

// Data commands
#define CODE_OPEN                       1
#define CODE_CLOSE                      2
#define CODE_SET_BITRATE                3
#define CODE_SET_FILTER_MASK            4
#define CODE_ONOFF_ERROR                5
#define CODE_GET_STATUS                 6
#define CODE_GET_STATISTICS             7
#define CODE_GET_SERIAL                 8
#define CODE_ONOFF_TIMESTAMPS           9
#define CODE_STORE_FRAME               10
#define CODE_SEND_FRAME                11
#define CODE_STORE_AUTORESPONSE        12 
#define CODE_ONOFF_AUTORESPONSE        13
#define CODE_CLEAR_STATISTICS          14

extern void RxUartTask(void);
extern void TxUartTask(void);
extern void RxTxTasksInit(void);

extern RXTX_UART_FRAME     RxUartFrame;
extern RXTX_UART_FRAME     TxUartFrame;

extern RXTX_UART_FRAME     RxUartFrameList[];
extern RXTX_UART_FRAME     TxUartFrameList[];
extern u8                  RxUartFrameListHead;
extern u8                  RxUartFrameListTail;
extern u8                  TxUartFrameListHead;
extern u8                  TxUartFrameListTail;
extern RXTX_UART_FRAME     TempFrame;