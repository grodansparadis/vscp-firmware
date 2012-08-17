// File: loadtask.h
// 
// 

#define LFTP_PORT				21	// Port where ftp server lives	
#define LFTP_TIMEOUT			10	// Seconds before starting all over again
#define LFTP_MAX_RETRIES		10	// Max number of connection retries


// ftp server
#define LFTP_SERVER_IP_v0		192
#define LFTP_SERVER_IP_v1		168
#define LFTP_SERVER_IP_v2		1
#define LFTP_SERVER_IP_v3		6

// States for the load task
typedef enum _NTP_STATE
{
	LFTP_STATE_NONE,
	LFTP_STATE_ARP,
	LFTP_STATE_ARP2,
	LFTP_STATE_ARP_RESOLVE,
	LFTP_STATE_CONNECT,
	LFTP_STATE_CONNECT_WAIT,
	LFTP_STATE_USER,
	LFTP_STATE_PASS,
	LFTP_STATE_PASV,
	LFTP_STATE_RETR,
	LFTP_STATE_DATA_CONNECT,
	LFTP_STATE_WAIT_DATA_CONNECT,
	LFTP_STATE_FETCH_DATA,
	LFTP_STATE_END
} NTP_STATE;


// Prototypes
void lftp_task( void );
void writeRomString2Socket( ROM char* p );
void writeString2Socket( char* p ) ; 
void writeDecimal( BYTE b );
void getPasvParams( void );
