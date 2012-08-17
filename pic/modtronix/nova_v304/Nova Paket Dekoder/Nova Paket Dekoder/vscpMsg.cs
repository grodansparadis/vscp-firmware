using System;
using System.Collections.Generic;
using System.Text;

namespace BRM_vscp_message
{
/****
    // This structure is for VSCP Level II with data embedded which is used
    // for the TCP interface.
    typedef struct _VscpMsgEx 
    {	
	    _u16 crc;					// crc checksum
	    _u8  data[487];				// Pointer to data. Max 487 (512- 25) bytes
	    // Following two are for daemon internal use
	    _u32 obid;					// Used by driver for channel info etc.
	    _u32 timestamp;				// Relative time stamp for package in microseconds
    // CRC should be calculated from
    // here to end + datablock
	    _u8 head;		  	    // bit 765 prioriy, Priority 0-7 where 0 is highest.
							    // bit 4 = hardcoded, true for a hardcoded device.
							    // bit 3 = Dont calculate CRC, false for CRC usage.
	    _u16 vscp_class;		// VSCP class
	    _u16 vscp_type;			// VSCP type
	    _u8  GUID[ 16 ];		// Node address MSB(0) -> LSB(15)
	    _u16 sizeData;			// Number of valid data bytes		
    } vscpMsgEx;

****/

    class CVscpMsg
    {
	    short       crc;			// crc checksum
	    byte[]      data;			// Pointer to data. Max 487 (512- 25) bytes

	    // Following two are for daemon internal use
	    uint        obid;			// Used by driver for channel info etc.
	    uint        timestamp;		// Relative time stamp for package in microseconds

        // CRC should be calculated from
        // here to end + datablock
	    byte        head;		  	// bit 765 prioriy, Priority 0-7 where 0 is highest.
							        // bit 4 = hardcoded, true for a hardcoded device.
							        // bit 3 = Dont calculate CRC, false for CRC usage.
	    ushort      vscp_class;		// VSCP class
	    ushort      vscp_type;	    // VSCP type
	    byte[]      GUID;		    // Node address MSB(0) -> LSB(15)
	    ushort      sizeData;		// Number of valid data bytes	
	
        public CVscpMsg()
        {
	        crc = 0;
	        data = new byte[487];

            obid = 0;
	        timestamp = 0;
	        head = 0;
	        vscp_class = 0;
	        vscp_type = 0;
	        GUID = new byte[ 16 ];
	        sizeData = 0;
        }

        const int VSCP_LEVEL2_UDP_PORT = 9598; // Changed from 1681 1 + ( sqr(5) - 1 ) / 2
        const int VSCP_LEVEL2_TCP_PORT = 9598; // Changed from 1681

        const byte VSCP_ADDRESS_SEGMENT_CONTROLLER = 0x00;
        const byte VSCP_ADDRESS_NEW_NODE = 0xff;
    }
}
