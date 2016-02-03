// File: ntptask.h
// 
// 

#define NTP_PORT				37	// Port where ntp server lives	
#define NTP_NUMBER_OF_SERVERS	3	// Numbers of servers to try
#define NTP_TIMEOUT				2	// Seconds before starting all over again
#define NTP_MAX_RETRIES			2	// Max number of connection retries
#define NTP_N_OF_SERVERS		3	// Number of servers to try

// Time server
#define NTP_SERVER1_URL		"swisstime.ethz.ch"
#define NTP_SERVER1_IP		"129.132.2.21"

#define NTP_SERVER1_IP_v0	129
#define NTP_SERVER1_IP_v1	132
#define NTP_SERVER1_IP_v2	2
#define NTP_SERVER1_IP_v3	21
/*
#define NTP_SERVER1_IP_v0	192
#define NTP_SERVER1_IP_v1	168
#define NTP_SERVER1_IP_v2	1
#define NTP_SERVER1_IP_v3	6
*/
#define NTP_SERVER2_URL		"ntp.metas.ch"	
#define NTP_SERVER2_IP      "162.23.41.39"

#define NTP_SERVER2_IP_v0	162
#define NTP_SERVER2_IP_v1	23
#define NTP_SERVER2_IP_v2	41
#define NTP_SERVER2_IP_v3	34

#define NTP_SERVER3_URL		"ntp.brattberg.com"	
#define NTP_SERVER3_IP      "80.65.200.75"

#define NTP_SERVER3_IP_v0	80
#define NTP_SERVER3_IP_v1	65
#define NTP_SERVER3_IP_v2	200
#define NTP_SERVER3_IP_v3	75


// States for the time services
typedef enum _NTP_STATE
{
	NTP_STATE_NONE,
	NTP_STATE_ARP,
	NTP_STATE_ARP2,
	NTP_STATE_ARP_RESOLVE,
	NTP_STATE_CONNECT,
	NTP_STATE_CONNECT_WAIT,
	NTP_STATE_RECEIVE_PHASE
} NTP_STATE;


struct tm
{
  int tm_sec;			// Seconds after the minute [0, 59]
  int tm_min;			// Minutes after the hour [0, 59]
  int tm_hour;			// Hours since midnight [0, 23]
  int tm_mday;			// Day of the month [1, 31]
  int tm_mon;			// Months since January [0, 11]
  int tm_year;			// Years since 1900
  int tm_wday;			// Days since Sunday [0, 6]
  int tm_yday;			// Days since January 1 [0, 365]
  int tm_isdst;			// Daylight Saving Time flag
};

typedef long time_t;

// Prototypes
void ntp_task( void );
struct tm * gmtime( const time_t *timer, struct tm *tmbuf );
