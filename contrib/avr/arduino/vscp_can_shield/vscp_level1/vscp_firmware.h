///////////////////////////////////////////////////////////////////////////////
// File: vscp_firmware.h
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * akhe@eurosource.se
 *
 * Copyright (C) 1995-2012 Ake Hedman,
 * Grodans Paradis AB, <akhe@grodansparadis.com>
 *
 * 080702 Cleaned up even more.
 * 060508 Cleaned up.
 * 060323 Added DM struct. 
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#ifndef VSCP_FIRMWARE_H
#define VSCP_FIRMWARE_H


/*! 
        \file vscp_firmware.h
        \brief VSCP firmware stack

This file contains the firmware neded to implement VSCP in a 
low end hardware device.

 Notes about defines
 -------------------

 Normaly make the following defines in the prjcfg.h file

 ENABLE_WRITE_2PROTECTED_LOCATIONS
 =================================
 to make it possible to write GUID, 
                        manufacturer id,
                        manufacturer sub device id
 page_low (0x92) and page_high (0x93) should both contain 0xff
 NOTE the storage must be in EEPROM also for it to work.

 */


#include "vscp_compiler.h" 	// This file should be in your project folder
#include "vscp_projdefs.h"	// This file should be in your project folder
#include <inttypes.h>

#ifdef __cplusplus
extern "C"
{
#endif

// ******************************************************************************
//  				VSCP Constants
// ******************************************************************************

#define VSCP_MAJOR_VERSION                     1    ///< VSCP Major version
#define VSCP_MINOR_VERSION                     5    ///< VSCP Minor Version

#define VSCP_ADDRESS_MASTER                    0x00
#define VSCP_ADDRESS_FREE                      0xff

#define VSCP_SIZE_GUID                         16   ///< # GUID bytes
#define VSCP_SIZE_DEVURL                       32   ///< # of device URL bytes
#define VSCP_SIZE_STD_DM_ROW                   8    ///< Size for level I decision matrix row

#define VSCP_BOOT_FLAG                         0xff // Boot flag is stored in persistent storage
// and if it is there the bootloader will be
// activated.

// Bootloaders
#define VSCP_BOOTLOADER_VSCP			0x00	// VSCP bootloader algorithm
#define VSCP_BOOTLOADER_PIC1            0x01	// PIC algorithm 0
#define VSCP_BOOTLOADER_AVR1			0x10	// AVR algorithm 0
#define VSCP_BOOTLOADER_LPC1			0x20	// NXP/Philips LPC algorithm 0
#define VSCP_BOOTLOADER_ST			    0x30	// ST STR algorithm 0
#define VSCP_BOOTLOADER_NONE			0xff

#define  VSCP_LEVEL1_COMMON_REGISTER_START      0x80

// State machine states 
#define VSCP_STATE_STARTUP                      0x00	// Cold/warm reset
#define VSCP_STATE_INIT                         0x01	// Assigning nickname
#define VSCP_STATE_PREACTIVE                    0x02	// Waiting for host initialixation
#define VSCP_STATE_ACTIVE                       0x03	// The normal state
#define VSCP_STATE_ERROR                        0x04	// error state. Big problems.

// State machine sub states 
#define VSCP_SUBSTATE_NONE                      0x00	// No state
#define VSCP_SUBSTATE_INIT_PROBE_SENT           0x01	// probe sent
#define VSCP_SUBSTATE_INIT_PROBE_ACK            0x02	// probe ACK received

// Helper consts and 
#define VSCP_VALID_MSG                          0x80	// Bit 7 set in flags

#define VSCP_PRIORITY7                          0x00
#define VSCP_PRIORITY_HIGH                      0x00
#define VSCP_PRIORITY6                          0x01
#define VSCP_PRIORITY5                          0x02
#define VSCP_PRIORITY4                          0x03
#define VSCP_PRIORITY_MEDIUM                    0x03
#define VSCP_PRIORITY_NORMAL                    0x03
#define VSCP_PRIORITY3                          0x04
#define VSCP_PRIORITY2                          0x05
#define VSCP_PRIORITY1                          0x06
#define VSCP_PRIORITY0                          0x07
#define VSCP_PRIORITY_LOW                       0x07


#define VSCP_PROBE_TIMEOUT                      1000    // ms - one second
#define VSCP_PROBE_TIMEOUT_COUNT                3       // Max # probe timeouts allowed

// ******************************************************************************
//  			VSCP Register - Logical positions
// ******************************************************************************

#define VSCP_REG_ALARMSTATUS                    0x80
#define VSCP_REG_VSCP_MAJOR_VERSION             0x81
#define VSCP_REG_VSCP_MINOR_VERSION		0x82

#define VSCP_REG_NODE_CONTROL                   0x83

#define VSCP_REG_USERID0                        0x84
#define VSCP_REG_USERID1                        0x85
#define VSCP_REG_USERID2                        0x86
#define VSCP_REG_USERID3                        0x87
#define VSCP_REG_USERID4			0x88

#define VSCP_REG_MANUFACTUR_ID0			0x89
#define VSCP_REG_MANUFACTUR_ID1			0x8A
#define VSCP_REG_MANUFACTUR_ID2			0x8B
#define VSCP_REG_MANUFACTUR_ID3			0x8C

#define VSCP_REG_MANUFACTUR_SUBID0		0x8D
#define VSCP_REG_MANUFACTUR_SUBID1		0x8E
#define VSCP_REG_MANUFACTUR_SUBID2		0x8F
#define VSCP_REG_MANUFACTUR_SUBID3		0x90

#define VSCP_REG_NICKNAME_ID			0x91

#define VSCP_REG_PAGE_SELECT_MSB                0x92
#define VSCP_REG_PAGE_SELECT_LSB                0x93

#define VSCP_REG_FIRMWARE_MAJOR_VERSION		0x94
#define VSCP_REG_FIRMWARE_MINOR_VERSION		0x95
#define VSCP_REG_FIRMWARE_SUB_MINOR_VERSION	0x96

#define VSCP_REG_BOOT_LOADER_ALGORITHM		0x97
#define VSCP_REG_BUFFER_SIZE                    0x98
#define VSCP_REG_PAGES_USED			0x99

#define VSCP_REG_GUID                           0xD0
#define VSCP_REG_DEVICE_URL                     0xE0

// INIT LED function codes
#define VSCP_LED_OFF		            	0x00
#define VSCP_LED_ON                             0x01
#define VSCP_LED_BLINK1                         0x02

/*!
        \struct _imsg
        Input message
 */
struct _imsg {
    /*!
            Input message flags\n
            ==================\n
            Bit 7 - Set if message valid\n
            Bit 6 - Reserved\n
            Bit 5 - Hardcoded (will never be set)\n
            Bit 3 - Number of data bytes MSB\n
            Bit 2 - Number of data bytes \n
            Bit 1 - Number of data bytes\n
            Bit 0 - Number of data bytes LSB\n
     */
    uint8_t flags; ///< Input message flags

    uint8_t priority; 	///< Priority for the message 0-7
    uint16_t vscp_class;///< VSCP class
    uint8_t vscp_type; 	///< VSCP type
    uint8_t oaddr; 		///< Packet originating address
    uint8_t data[8]; 	///< data bytes
};

/*!
        \struct _omsg
        Output message
 */
struct _omsg {
    /*!
            Output message flags ( Message to send )\n
            ========================================
            Bit 7 - Set if message should be sent (cleard when sent)\n
            Bit 6 - Reserved\n
            Bit 5 - Reserved\n
            Bit 2 - Number of data bytes MSB\n
            Bit 2 - Number of data bytes \n
            Bit 1 - Number of data bytes\n
            Bit 0 - Number of data bytes LSB\n
     */
    uint8_t flags; ///< Output message flags

    uint8_t priority; ///< Priority for the message 0-7
    uint16_t vscp_class; ///< VSCP class
    uint8_t vscp_type; ///< VSCP type
    /// Originating address is always *this* node
    uint8_t data[8]; ///< data bytes
};

/*!
        Decision Matrix - definitions

        A matrix row consist of 8 bytes and have the following format

        | oaddr | flags | class-mask | class-filter | type-mask | type-filter | action | action-param |

        oaddr is the originating address.

        flag
        ====
        bit 7 - Enabled (==1).
        bit 6 - oaddr should be checked (==1) or not checked (==0)
        bit 5 - Reserved
        bit 4 - Reserved
        bit 3 - Reserved
        bit 2 - Reserved
        bit 1 - Classmask bit 8
        bit 0 - Classfilter bit 8

        Action = 0 is always NOOP, "no operation".
 */

#define VSCP_DM_POS_OADDR			      	0
#define VSCP_DM_POS_FLAGS			      	1
#define VSCP_DM_POS_CLASSMASK		   		2
#define VSCP_DM_POS_CLASSFILTER		  		3
#define VSCP_DM_POS_TYPEMASK		    	4
#define VSCP_DM_POS_TYPEFILTER		  		5
#define VSCP_DM_POS_ACTION			    	6
#define VSCP_DM_POS_ACTIONPARAM		  		7

#define VSCP_DM_FLAG_ENABLED		    	0x80
#define VSCP_DM_FLAG_CHECK_OADDR	  		0x40
#define VSCP_DM_FLAG_HARDCODED		  		0x20
#define VSCP_DM_FLAG_CHECK_ZONE		  		0x10
#define VSCP_DM_FLAG_CHECK_SUBZONE			0x08
#define VSCP_DM_FLAG_CLASS_MASK		  		0x02
#define VSCP_DM_FLAG_CLASS_FILTER	  		0x01

/*!
        \struct _dmrow
        Decision matrix row element (for RAM storage)
        Each DM row consist of a structure of this type.
 */
struct _dmrow {
    uint8_t oaddr; ///< Originating address
    uint8_t flags; ///< Decion matrix row flags
    uint8_t class_mask; ///< Mask for class (lower eight bits)
    uint8_t class_filter; ///< Filter for class (lower eight bits)
    uint8_t type_mask; ///< Mask for type
    uint8_t type_filter; ///< Filter for type
    uint8_t action; ///< Action code
    uint8_t action_param; ///< Action parameter
};

// -----------------------------------------------------------------------------

/// External - VSCP Data
extern uint8_t vscp_nickname; ///< Assigned node nickname
extern uint8_t vscp_errorcnt; ///< VSCP error counter
extern uint8_t vscp_alarmstatus; ///< VSCP alarm status register
extern uint8_t vscp_node_state; ///< VSCP state machine main state
extern uint8_t vscp_node_substate; ///< VSCP state machine sub state
extern uint8_t vscp_initledfunc; ///<
///           The following are defined in vscp.c
extern struct _imsg vscp_imsg; ///< Current input event
extern struct _omsg vscp_omsg; ///< Current outgoing event
extern volatile uint16_t vscp_timer; ///< 1 ms timer counter
extern uint8_t vscp_probe_address; ///< Probe address for nicknam discovery
extern volatile uint8_t vscp_initbtncnt; ///< init button counter
extern volatile uint8_t vscp_statuscnt; ///< status LED counter
extern uint16_t vscp_page_select; ///< Selected Register Page
extern uint8_t vscp_second;
extern uint8_t vscp_minute;
extern uint8_t vscp_hour;

extern const uint8_t vscp_deviceURL[]; ///<  GUID

// Prototypes


/*!
        \fn vscp_init
        Init the VSCP firmware.
	
        Call this before entering the mainloop.
 */
void vscp_init(void);


/*!
        Set VSCP error state
 */
void vscp_error(void);

/*!
        Handle nickname probing
	
        This routine should be called periodically while
        in state VSCP_STATE_INIT
 */
void vscp_handleProbeState(void);

/*!
        Handle the preactive state
	
        This state is entered if a nod with nickname=0 answers the
        probe.Zero is reserved for a segment controller and if it
        is available and acknowledge this way that it is the node should
        wait for the segment controller to set its nickname. This should
        happen before a preset timeout (1 second) and if this time is
        exceeded the init process is started again to give the segment controler
        more chances to do its job.
 */
void vscp_handlePeActiveState(void);

/*!
        Handle incoming CLASS1.PROTOCOL event
	
        The event should be in the vscp_imsg buffer on entry.
 */
void vscp_handleProtocolEvent(void);

/*!
        Go to the active state
	
        This mean the node sends new node online and informs other nodes
        about its aquired nicknam.
 */
void vscp_goActiveState(void);

/*!
        Send periodic heartbeat
        This event should be called by the application perodic
        to inform the world about its existance and tell it's
        alive.
        @param zone Zone the module belongs to or zero if no zone.
        @param subzone Subzone the module belongs to or zero i no zubzone.
 */
void vscp_sendHeartBeat(uint8_t zone, uint8_t subzone);

/*!
        Handle received VSCP segment controller heartbeat
 */
void vscp_handleHeartbeat(void);

/*!
        Handle Set Nickname event
 */
void vscp_handleSetNickname(void);

/*!
        Handle Drop Nickname
 */
void vscp_handleDropNickname(void);

/*!
        Report a new node on the bus
 */
void vscp_newNodeOnline(void);

/*!
        Read a VSCP register
        @param reg register to read.
        @return Content of register.
 */
uint8_t vscp_readRegister(uint8_t reg);

/*!
        Read standard register (upper part)
        @param reg Register to read (>=0x80)
        @return Register content or 0xff for nn valid register
 */
uint8_t vscp_readStdReg(uint8_t reg);

/*!
        Write VSCP register
        @param reg Register to write to
        @param value Value to write
        @return Content of register after write.
 */
uint8_t vscp_writeRegister(uint8_t reg, uint8_t value);

/*!
        Write standard register (upper part)
        @param reg Register to write to
        @param value Value to write
        @return Content of register after write.
 */
uint8_t vscp_writeStdReg(uint8_t reg, uint8_t value);

/*!
        Do One second work
	
        This routine should be called once a second by the
        application.
 */
void vscp_doOneSecondWork(void);

/*!

 */
int8_t vscp_check_pstorage(void);



/*!
        Send VSCP event in the out-buffer.
        @return TRUE on success.
 */
int8_t vscp_sendEvent(void);

/*!
        Get VSCP Event if there is no message in the input buffer.
        @return TRUE if a valid event is placed in the in-buffer.
 */
int8_t vscp_getEvent(void);



// --------------------------- External Functions -----------------------------
//
// All functions below should be implemented by the application
//
// --------------------------- External Functions -----------------------------


/*!
    Get a VSCP frame frame
    @param pvscpclass Pointer to variable that will get VSCP class.
    @param pvscptype Ponter to variable which will get VSCP type.
    @param pNodeId Pointer to variable which will get nodeid.
        @param pPriority Pointer to variable which will get priority (0-7).
    @param pSize Pointer to variable that will get datasize.
    @param pData pinter to array that will get event data.
    @return TRUE on success.
 */
int8_t getVSCPFrame(uint16_t *pvscpclass,
        uint8_t *pvscptype,
        uint8_t *pNodeId,
        uint8_t *pPriority,
        uint8_t *pSize,
        uint8_t *pData);

/*!
    Send a VSCP frame 
    @param vscpclass VSCP class for event.
    @param vscptype VSCP type for event.
        @param nodeid Nodeid for originating node.
        @param priority Priotity for event.
    @param size Size of data portion.
    @param pData Pointer to event data.
    @return TRUE on success.
 */
int8_t sendVSCPFrame(uint16_t vscpclass,
        uint8_t vscptype,
        uint8_t nodeid,
        uint8_t priority,
        uint8_t size,
        uint8_t *pData);


/*!
        The following methods must be defined
        in the application and should return firmware version
        information
 */
uint8_t vscp_getMajorVersion(void);
uint8_t vscp_getMinorVersion(void);
uint8_t vscp_getSubMinorVersion(void);

/*! 
        Get GUID from permament storage
 */
uint8_t vscp_getGUID(uint8_t idx);
void vscp_setGUID(uint8_t idx, uint8_t data);

/*!
        User ID 0 idx=0
        User ID 1 idx=1
        User ID 2 idx=2
        User ID 3 idx=3
 */
uint8_t vscp_getUserID(uint8_t idx);
void vscp_setUserID(uint8_t idx, uint8_t data);

/*!
        Handle manufacturer id.
	
        Not that both main and sub id are fetched here
                Manufacturer device ID byte 0 - idx=0
                Manufacturer device ID byte 1 - idx=1
                Manufacturer device ID byte 2 - idx=2
                Manufacturer device ID byte 3 - idx=3
                Manufacturer device sub ID byte 0 - idx=4
                Manufacturer device sub ID byte 1 - idx=5
                Manufacturer device sub ID byte 2 - idx=6
                Manufacturer device sub ID byte 3 - idx=7
 */
uint8_t vscp_getManufacturerId(uint8_t idx);
void vscp_setManufacturerId(uint8_t idx, uint8_t data);

/*!
        Get bootloader algorithm from permanent storage
 */
uint8_t vscp_getBootLoaderAlgorithm(void);

/*! 
        Get buffer size
 */
uint8_t vscp_getBufferSize(void);

/*! 
        Get number of register pages used by app.
 */
uint8_t vscp_getRegisterPagesUsed(void);

/*!
        Get URL from device from permanent storage
        index 0-15
 */
uint8_t vscp_getMDF_URL(uint8_t idx);

/*!
        Fetch nickname from permanent storage
        @return read nickname.
 */
uint8_t vscp_readNicknamePermanent(void);

/*! 
        Write nickname to permanent storage
        @param nickname to write
 */
void vscp_writeNicknamePermanent(uint8_t nickname);

/*! 
        Fetch segment CRC from permanent storage
 */
uint8_t vscp_getSegmentCRC(void);

/*! 
        Write segment CRC to permanent storage
 */
void vscp_setSegmentCRC(uint8_t crc);

/*! 
        Write control byte permanent storage
 */
void vscp_setControlByte(uint8_t ctrl);

/*!
        Fetch control byte from permanent storage
 */
uint8_t vscp_getControlByte(void);

/*! 
        Get page select bytes
                idx=0 - byte 0 MSB
                idx=1 - byte 1 LSB
 */
uint8_t vscp_getPageSelect(uint8_t idx);

/*!
        Set page select registers
        @param idx 0 for LSB, 1 for MSB
        @param data Byte to set of page select registers
 */
void vscp_setPageSelect(uint8_t idx, uint8_t data);

/*!
        Read application register (lower part)
        @param reg Register to read (<0x80)
        @return Register content or 0x00 for non valid register
 */
uint8_t vscp_readAppReg(uint8_t reg);

/*!
        Write application register (lower part)
        @param reg Register to read (<0x80)
        @param value Value to write to register.
        @return Register content or 0xff for non valid register
 */
uint8_t vscp_writeAppReg(uint8_t reg, uint8_t value);

/*!
        Get DM matrix info
        The output message data structure should be filled with
        the following data by this routine.
        byte 0 - Number of DM rows. 0 if none.
        byte 1 - offset in register space.
        byte 2 - start page MSB
        byte 3 - start page LSB
        byte 4 - End page MSB
        byte 5 - End page LSB
        byte 6 - Level II size of DM row (Just for Level II nodes).
 */
void vscp_getMatrixInfo(char *pData);

/*!
        Get embedded MDF info
        If available this routine sends an embedded MDF file
        in several events. See specification CLASS1.PROTOCOL
        Type=35/36
 */
void vscp_getEmbeddedMdfInfo(void);

/*!
        Go bootloader mode
        This routine force the system into bootloader mode according
        to the selected protocol.
 */
void vscp_goBootloaderMode(void);

/*!
        Get Zone for device
        Just return zero if not used.
 */
uint8_t vscp_getZone(void);

/*!
        Get Subzone for device
        Just return zero if not used.
    
 */
uint8_t vscp_getSubzone(void);

#ifdef __cplusplus
}
#endif

#endif
