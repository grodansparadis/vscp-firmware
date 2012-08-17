// vscp_projdefs
// Project specific settings 

// Use flying windows
#define VSCP_SERIAL_ENABLE_SLIDING_WINDOWS

// Number of active frames (flying winows)
#define VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES   10

// Max time used to transmit a frame in milliseconds
#define VSCP_SERIAL_MAX_SLIDING_WINDOW_TIME     500

// Size for sending buffer
#define SERIAL_OUT_BUFFE_SIZE                   256

// Max frames in CAN outbuffer
#define MAX_OUT_FRAMES                          16

// Max frames in CAN outbuffer
#define MAX_IN_FRAMES                           16
