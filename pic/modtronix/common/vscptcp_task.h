// File: vscptcp_task.h
// 
// 


#define VSCP_TIMEOUT			5		// Seconds before starting all over again
#define VSCP_RXTX_TIMEOUT		2		// Receive/Transmit timeout
#define VSCP_MAX_RETRIES		10		// Max number of connection retries


// States for the time services
typedef enum _VSCP_STATE
{
	VSCP_STATE_NONE,
	VSCP_STATE_ARP,
	VSCP_STATE_ARP2,
	VSCP_STATE_ARP_RESOLVE,
	VSCP_STATE_CONNECT,
	VSCP_STATE_CONNECT_WAIT,
	VSCP_STATE_REST,
	VSCP_STATE_USERNAME,
	VSCP_STATE_USERNAME_RESPONSE,
	VSCP_STATE_PASSWORD_RESPONSE,
	VSCP_STATE_FAST_RESPONSE,
	VSCP_STATE_WORK_IDLE,
	VSCP_STATE_WORK_TX_WAIT_RESPONSE,
	VSCP_STATE_WORK_RX_WAIT_RESPONSE,
} VSCP_STATE;



// Prototypes

// Do VSCP TCP task
void vscp_tcp_task( void );

// Check TCP server response
uint8_t checkResponse( TCP_SOCKET vscpsocket );

// Write an event to the server
void FAST_WriteEvent( TCP_SOCKET sock, vscpMsg *pMsg );

// Write a command to the server
void FAST_WriteCommandEvent( TCP_SOCKET sock, uint8_t command );

// Rean an event from the server
uint8_t FAST_ReadEvent( TCP_SOCKET sock, vscpMsg *pMsg, uint8_t *ptype );

// Read error frame
uint8_t FAST_CheckResponse( TCP_SOCKET sock, uint8_t *perrorcode );

