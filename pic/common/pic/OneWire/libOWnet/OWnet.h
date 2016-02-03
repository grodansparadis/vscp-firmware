/*********************************************************************
** Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
** OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
** ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
** OTHER DEALINGS IN THE SOFTWARE.
**
** Except as contained in this notice, the name of Dallas Semiconductor
** shall not be used except as stated in the Dallas Semiconductor
** Branding Policy.
*********************************************************************/
/*********************************************************************
** This port of the OneWire libraries for wizC and fedC is created by:
** (C) 2006 HMCS / Marcel van Lieshout
**
** wizC/fedC are (C) Forest Electronic Developments.
*********************************************************************/
#ifndef OWNET_H
#define OWNET_H

#include <pic.h>
#include <stddef.h>
#include <stdlib.h>

#undef B

#define OWLEVEL_IDLE_LOW     (0)
#define OWLEVEL_IDLE_HIGH    (1)
#define OWLEVEL_ACTIVE_LOW   OWLEVEL_IDLE_HIGH
#define OWLEVEL_ACTIVE_HIGH  OWLEVEL_IDLE_LOW

#include "OWnetConfig.h"

// general defines
#define WRITE_FUNCTION       (1)
#define READ_FUNCTION        (0)

// error codes
// todo: investigate these and replace with new Error Handling library
#define READ_ERROR           (-1)
#define INVALID_DIR          (-2)
#define NO_FILE              (-3)
#define WRITE_ERROR          (-4)
#define WRONG_TYPE           (-5)
#define FILE_TOO_BIG         (-6)

// Misc
#define FALSE                (0)
#define TRUE                 (1)

// mode bit flags
#define MODE_NORMAL          (0x00)
#define MODE_OVERDRIVE       (0x01)
#define MODE_STRONG5         (0x02)
#define MODE_PROGRAM         (0x04)
#define MODE_BREAK           (0x08)

// Output flags
#define LV_ALWAYS            (2)
#define LV_OPTIONAL          (1)
#define LV_VERBOSE           (0)

//--------------------------------------------------------------//
// Typedefs
//--------------------------------------------------------------//
typedef struct tagOWNETWORKVARS {
  BYTE  SerialNum[8];
  WORD  utilcrc16;
  BYTE  utilcrc8;
  BYTE  CurrentLevel;
  BYTE  LastDevice;
  BYTE  LastDiscrepancy;
  BYTE  LastFamilyDiscrepancy;
} OWNETWORKVARS;

//--------------------------------------------------------------//
// Error handling - This just loops at the errorlocation
//--------------------------------------------------------------//
#define OWERROR(err)    while(1)

#define OWERROR_NO_ERROR_SET                    0
#define OWERROR_NO_DEVICES_ON_NET               1
#define OWERROR_RESET_FAILED                    2
#define OWERROR_SEARCH_ERROR                    3
#define OWERROR_ACCESS_FAILED                   4
#define OWERROR_DS2480_NOT_DETECTED             5
#define OWERROR_DS2480_WRONG_BAUD               6
#define OWERROR_DS2480_BAD_RESPONSE             7
#define OWERROR_OPENCOM_FAILED                  8
#define OWERROR_WRITECOM_FAILED                 9
#define OWERROR_READCOM_FAILED                  10
#define OWERROR_BLOCK_TOO_BIG                   11
#define OWERROR_BLOCK_FAILED                    12
#define OWERROR_PROGRAM_PULSE_FAILED            13
#define OWERROR_PROGRAM_BYTE_FAILED             14
#define OWERROR_WRITE_BYTE_FAILED               15
#define OWERROR_READ_BYTE_FAILED                16
#define OWERROR_WRITE_VERIFY_FAILED             17
#define OWERROR_READ_VERIFY_FAILED              18
#define OWERROR_WRITE_SCRATCHPAD_FAILED         19
#define OWERROR_COPY_SCRATCHPAD_FAILED          20
#define OWERROR_INCORRECT_CRC_LENGTH            21
#define OWERROR_CRC_FAILED                      22
#define OWERROR_GET_SYSTEM_RESOURCE_FAILED      23
#define OWERROR_SYSTEM_RESOURCE_INIT_FAILED     24
#define OWERROR_DATA_TOO_LONG                   25
#define OWERROR_READ_OUT_OF_RANGE               26
#define OWERROR_WRITE_OUT_OF_RANGE              27
#define OWERROR_DEVICE_SELECT_FAIL              28
#define OWERROR_READ_SCRATCHPAD_VERIFY          29
#define OWERROR_COPY_SCRATCHPAD_NOT_FOUND       30
#define OWERROR_ERASE_SCRATCHPAD_NOT_FOUND      31
#define OWERROR_ADDRESS_READ_BACK_FAILED        32
#define OWERROR_EXTRA_INFO_NOT_SUPPORTED        33
#define OWERROR_PG_PACKET_WITHOUT_EXTRA         34
#define OWERROR_PACKET_LENGTH_EXCEEDS_PAGE      35
#define OWERROR_INVALID_PACKET_LENGTH           36
#define OWERROR_NO_PROGRAM_PULSE                37
#define OWERROR_READ_ONLY                       38
#define OWERROR_NOT_GENERAL_PURPOSE             39
#define OWERROR_READ_BACK_INCORRECT             40
#define OWERROR_INVALID_PAGE_NUMBER             41
#define OWERROR_CRC_NOT_SUPPORTED               42
#define OWERROR_CRC_EXTRA_INFO_NOT_SUPPORTED    43
#define OWERROR_READ_BACK_NOT_VALID             44
#define OWERROR_COULD_NOT_LOCK_REDIRECT         45
#define OWERROR_READ_STATUS_NOT_COMPLETE        46
#define OWERROR_PAGE_REDIRECTION_NOT_SUPPORTED  47
#define OWERROR_LOCK_REDIRECTION_NOT_SUPPORTED  48
#define OWERROR_READBACK_EPROM_FAILED           49
#define OWERROR_PAGE_LOCKED                     50
#define OWERROR_LOCKING_REDIRECTED_PAGE_AGAIN   51
#define OWERROR_REDIRECTED_PAGE                 52
#define OWERROR_PAGE_ALREADY_LOCKED             53
#define OWERROR_WRITE_PROTECTED                 54
#define OWERROR_NONMATCHING_MAC                 55
#define OWERROR_WRITE_PROTECT                   56
#define OWERROR_WRITE_PROTECT_SECRET            57
#define OWERROR_COMPUTE_NEXT_SECRET             58
#define OWERROR_LOAD_FIRST_SECRET               59
#define OWERROR_POWER_NOT_AVAILABLE             60
#define OWERROR_XBAD_FILENAME                   61
#define OWERROR_XUNABLE_TO_CREATE_DIR           62
#define OWERROR_REPEAT_FILE                     63
#define OWERROR_DIRECTORY_NOT_EMPTY             64
#define OWERROR_WRONG_TYPE                      65
#define OWERROR_BUFFER_TOO_SMALL                66
#define OWERROR_NOT_WRITE_ONCE                  67
#define OWERROR_FILE_NOT_FOUND                  68
#define OWERROR_OUT_OF_SPACE                    69
#define OWERROR_TOO_LARGE_BITNUM                70
#define OWERROR_NO_PROGRAM_JOB                  71
#define OWERROR_FUNC_NOT_SUP                    72
#define OWERROR_HANDLE_NOT_USED                 73
#define OWERROR_FILE_WRITE_ONLY                 74
#define OWERROR_HANDLE_NOT_AVAIL                75
#define OWERROR_INVALID_DIRECTORY               76
#define OWERROR_HANDLE_NOT_EXIST                77
#define OWERROR_NONMATCHING_SNUM                78
#define OWERROR_NON_PROGRAM_PARTS               79
#define OWERROR_PROGRAM_WRITE_PROTECT           80
#define OWERROR_FILE_READ_ERR                   81
#define OWERROR_ADDFILE_TERMINATED              82
#define OWERROR_READ_MEMORY_PAGE_FAILED         83
#define OWERROR_MATCH_SCRATCHPAD_FAILED         84
#define OWERROR_ERASE_SCRATCHPAD_FAILED         85
#define OWERROR_READ_SCRATCHPAD_FAILED          86
#define OWERROR_SHA_FUNCTION_FAILED             87
#define OWERROR_NO_COMPLETION_BYTE              88
#define OWERROR_WRITE_DATA_PAGE_FAILED          89
#define OWERROR_COPY_SECRET_FAILED              90
#define OWERROR_BIND_SECRET_FAILED              91
#define OWERROR_INSTALL_SECRET_FAILED           92
#define OWERROR_VERIFY_SIG_FAILED               93
#define OWERROR_SIGN_SERVICE_DATA_FAILED        94
#define OWERROR_VERIFY_AUTH_RESPONSE_FAILED     95
#define OWERROR_ANSWER_CHALLENGE_FAILED         96
#define OWERROR_CREATE_CHALLENGE_FAILED         97
#define OWERROR_BAD_SERVICE_DATA                98
#define OWERROR_SERVICE_DATA_NOT_UPDATED        99
#define OWERROR_CATASTROPHIC_SERVICE_FAILURE    100
#define OWERROR_LOAD_FIRST_SECRET_FAILED        101
#define OWERROR_MATCH_SERVICE_SIGNATURE_FAILED  102
#define OWERROR_KEY_OUT_OF_RANGE                103
#define OWERROR_BLOCK_ID_OUT_OF_RANGE           104
#define OWERROR_PASSWORDS_ENABLED               105
#define OWERROR_PASSWORD_INVALID                106
#define OWERROR_NO_READ_ONLY_PASSWORD           107
#define OWERROR_NO_READ_WRITE_PASSWORD          108
#define OWERROR_OW_SHORTED                      109
#define OWERROR_ADAPTER_ERROR                   110
#define OWERROR_EOP_COPY_SCRATCHPAD_FAILED      111
#define OWERROR_EOP_WRITE_SCRATCHPAD_FAILED     112
#define OWERROR_HYGRO_STOP_MISSION_UNNECESSARY  113
#define OWERROR_HYGRO_STOP_MISSION_ERROR        114
#define OWERROR_PORTNUM_ERROR                   115
#define OWERROR_LEVEL_FAILED                    116

//--------------------------------------------------------------//
//  Determine the number of buses to be used
//--------------------------------------------------------------//
#if   defined(OWNET7_SENSE_PORT)
  #define MAX_PORTNUM   8
#elif defined(OWNET6_SENSE_PORT)
  #define MAX_PORTNUM   7
#elif defined(OWNET5_SENSE_PORT)
  #define MAX_PORTNUM   6
#elif defined(OWNET4_SENSE_PORT)
  #define MAX_PORTNUM   5
#elif defined(OWNET3_SENSE_PORT)
  #define MAX_PORTNUM   4
#elif defined(OWNET2_SENSE_PORT)
  #define MAX_PORTNUM   3
#elif defined(OWNET1_SENSE_PORT)
  #define MAX_PORTNUM   2
#elif defined(OWNET0_SENSE_PORT)
  #define MAX_PORTNUM   1
#else
  #error No OneWire Networks defined
#endif  
    
//--------------------------------------------------------------//
// Determine which library to use
//--------------------------------------------------------------//
#if MAX_PORTNUM == 1
  #define OWSINGLEBUS
#else
  #define OWMULTIBUS
#endif

#ifdef OWSINGLEBUS
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/SingleBus/OWnet.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/SingleBus/OWlnk.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/SingleBus/OWses.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/SingleBus/OWtran.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/SingleBus/OWcrc.c"
//--------------------------------------------------------------//
// Determine the needed functionality
//--------------------------------------------------------------//
  #define OWNET_SENSE_PORTI          PORT##OWNET0_SENSE_PORT
  #if _SERIES == 18
    #define OWNET_SENSE_PORTO        LAT##OWNET0_SENSE_PORT
  #else
    #define OWNET_SENSE_PORTO        PORT##OWNET0_SENSE_PORT
  #endif
  #define OWNET_SENSE_TRIS           TRIS##OWNET0_SENSE_PORT
  #define OWNET_SENSE_PIN            OWNET0_SENSE_PIN

  #ifdef OWNET0_DRIVE_PORT
    #if _SERIES == 18
      #define OWNET_DRIVE_PORT       LAT##OWNET0_DRIVE_PORT
    #else
      #define OWNET_DRIVE_PORT       PORT##OWNET0_DRIVE_PORT
    #endif
    #define OWNET_DRIVE_TRIS         TRIS##OWNET0_DRIVE_PORT
    #define OWNET_DRIVE_PIN          OWNET0_DRIVE_PIN
	#ifdef OWNET0_DRIVE_POLARITY
	  #define OWNET_DRIVE_POLARITY   OWNET0_DRIVE_POLARITY
	#else
	  #define OWNET_DRIVE_POLARITY   OWLEVEL_IDLE_LOW
	#endif
  #endif
	
  #ifdef OWNET0_DPU_PORT
    #if _SERIES == 18
      #define OWNET_DPU_PORT         LAT##OWNET0_DPU_PORT
    #else
      #define OWNET_DPU_PORT         PORT##OWNET0_DPU_PORT
    #endif
    #define OWNET_DPU_TRIS           TRIS##OWNET0_DPU_PORT
    #define OWNET_DPU_PIN            OWNET0_DPU_PIN
	#ifdef OWNET0_DPU_POLARITY
	  #define OWNET_DPU_POLARITY     OWNET0_DPU_POLARITY
	#else
	  #define OWNET_DPU_POLARITY     OWLEVEL_IDLE_LOW
	#endif
  #endif

  #ifdef OWNET0_SPU_PORT
    #if _SERIES == 18
      #define OWNET_SPU_PORT         LAT##OWNET0_SPU_PORT
    #else
      #define OWNET_SPU_PORT         PORT##OWNET0_SPU_PORT
    #endif
    #define OWNET_SPU_TRIS           TRIS##OWNET0_SPU_PORT
    #define OWNET_SPU_PIN            OWNET0_SPU_PIN
	#ifdef OWNET0_SPU_POLARITY
	  #define OWNET_SPU_POLARITY     OWNET0_SPU_POLARITY
	#else
	  #define OWNET_SPU_POLARITY     OWLEVEL_IDLE_LOW
	#endif
  #endif

  #ifdef OWNET0_PPU_PORT
    #if _SERIES == 18
      #define OWNET_PPU_PORT         LAT##OWNET0_PPU_PORT
    #else
      #define OWNET_PPU_PORT         PORT##OWNET0_PPU_PORT
    #endif
    #define OWNET_PPU_TRIS           TRIS##OWNET0_PPU_PORT
    #define OWNET_PPU_PIN            OWNET0_PPU_PIN
	#ifdef OWNET0_PPU_POLARITY
	  #define OWNET_PPU_POLARITY     OWNET0_PPU_POLARITY
	#else
	  #define OWNET_PPU_POLARITY     OWLEVEL_IDLE_LOW
	#endif
  #endif

  // One Wire functions defined in wizCnet.c (SingleBus)
  #define owFirst(p1,p2,p3)                      SowFirst(p2,p3)
  #define owNext(p1,p2,p3)                       SowNext(p2,p3)
  #define owSerialNum(p1,p2,p3)                  SowSerialNum(p2,p3)
  #define owFamilySearchSetup(p1,p2)             SowFamilySearchSetup(p2)
  #define owSkipFamily(p1)                       SowSkipFamily()
  #define owAccess(p1)                           SowAccess()
  #define owVerify(p1,p2)                        SowVerify(p2)
  #define owOverdriveAccess(p1)                  SowOverdriveAccess()
  BYTE  SowFirst(BYTE do_reset, BYTE alarm_only);
  BYTE  SowNext(BYTE do_reset, BYTE alarm_only);
  void  SowSerialNum(BYTE *serialnum_buf, BYTE do_read);
  void  SowFamilySearchSetup(BYTE search_family);
  void  SowSkipFamily(void);
  BYTE  SowAccess(void);
  BYTE  SowVerify(BYTE alarm_only);
  BYTE  SowOverdriveAccess(void);

  // external One Wire functions defined in wizCses.c (SingleBus)
  #define owAcquire(p1,p2)                       SowAcquire(p2)
  #define owAcquireEx(p1)                        SowAcquireEx(p1)
  #define owRelease(p1)                          SowRelease()
  BYTE SowAcquire(BYTE *port_zstr);
  BYTE SowAcquireEx(BYTE *port_zstr);
  void SowRelease(void);
  // external One Wire globals from wizCses.c (MultiBus)
  extern OWNETWORKVARS  owNetCurrent;

  // external One Wire functions from link layer wizClnk.c (SingleBus)
  #define owTouchReset(p1)                       SowTouchReset()
  #define owTouchBit(p1,p2)                      SowTouchBit(p2)
  #define owTouchByte(p1,p2)                     SowTouchByte(p2)
  #define owWriteByte(p1,p2)                     SowWriteByte(p2)
  #define owReadByte(p1)                         SowReadByte()
  #define owSpeed(p1,p2)                         SowSpeed(p2)
  #define owLevel(p1,p2)                         SowLevel(p2)
  #define owProgramPulse(p1)                     SowProgramPulse()
  #define owWriteBytePower(p1,p2)                SowWriteBytePower(p2)
  #define owReadBytePower(p1)                    SowReadBytePower()
  #define owHasPowerDelivery(p1)                 SowHasPowerDelivery()
  #define owHasProgramPulse(p1)                  SowHasProgramPulse()
  #define owHasOverDrive(p1)                     SowHasOverDrive()
  #define owReadBitPower(p1,p2)                  SowReadBitPower(p2)
  BYTE SowTouchReset(void);
  BYTE SowTouchBit(BYTE sendbit);
  BYTE SowTouchByte(BYTE sendbyte);
  BYTE SowWriteByte(BYTE sendbyte);
  BYTE SowReadByte(void);
  BYTE SowSpeed(BYTE new_speed);
  BYTE SowLevel(BYTE new_level);
  BYTE SowProgramPulse(void);
  BYTE SowWriteBytePower(BYTE sendbyte);
  BYTE SowReadBytePower(void);
  BYTE SowHasPowerDelivery(void);
  BYTE SowHasProgramPulse(void);
  BYTE SowHasOverDrive(void);
  BYTE SowReadBitPower(BYTE applyPowerResponse);
  // external One Wire global from wizClnk.c (SingleBus)
  extern BYTE FAMILY_CODE_04_ALARM_TOUCHRESET_COMPLIANCE;

  // external One Wire functions from transaction layer in wizCtran.c (SingleBus)
  #define owBlock(p1,p2,p3,p4)                   SowBlock(p2,p3,p4)
  #define owReadPacketStd(p1,p2,p3,p4)           SowReadPacketStd(p2,p3,p4)
  #define owWritePacketStd(p1,p2,p3,p4,p5,p6)    SowWritePacketStd(p2,p3,p4,p5,p6)
  #define owProgramByte(p1,p2,p3,p4,p5,p6)       SowProgramByte(p2,p3,p4,p5,p6)
  BYTE SowBlock(BYTE do_reset, BYTE *tran_buf, BYTE tran_len);
  BYTE SowReadPacketStd(BYTE do_access, int start_page, BYTE *read_buf);
  BYTE SowWritePacketStd(int start_page, BYTE *write_buf,
                         BYTE write_len, BYTE is_eprom, BYTE crc_type);
  BYTE SowProgramByte(BYTE write_byte, int addr, BYTE write_cmd,
                      BYTE crc_type, BYTE do_access);

  // external functions defined in wizCcrc.c (SingleBus)
  #define setcrc16(p1,p2)                        Ssetcrc16(p2)
  #define docrc16(p1,p2)                         Sdocrc16(p2)
  #define setcrc8(p1,p2)                         Ssetcrc8(p2)
  #define docrc8(p1,p2)                          Sdocrc8(p2)
  void Ssetcrc16(WORD reset);
  WORD Sdocrc16(WORD cdata);
  void Ssetcrc8(BYTE reset);
  BYTE Sdocrc8(BYTE x);
  
#else
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/MultiBus/OWnet.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/MultiBus/OWlnk.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/MultiBus/OWses.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/MultiBus/OWtran.c"
  #pragma wizcpp uselib "$__PATHNAME__/libOWnet/MultiBus/OWcrc.c"
//--------------------------------------------------------------//
// Determine the needed functionality
//--------------------------------------------------------------//
  #if defined(OWNET0_DRIVE_PORT) || \
      defined(OWNET1_DRIVE_PORT) || \
      defined(OWNET2_DRIVE_PORT) || \
      defined(OWNET3_DRIVE_PORT) || \
      defined(OWNET4_DRIVE_PORT) || \
      defined(OWNET5_DRIVE_PORT) || \
      defined(OWNET6_DRIVE_PORT) || \
      defined(OWNET7_DRIVE_PORT)
    #define OWNET_DRIVE_USED
  #endif    
  #if defined(OWNET0_DPU_PORT) ||   \
      defined(OWNET1_DPU_PORT) ||   \
      defined(OWNET2_DPU_PORT) ||   \
      defined(OWNET3_DPU_PORT) ||   \
      defined(OWNET4_DPU_PORT) ||   \
      defined(OWNET5_DPU_PORT) ||   \
      defined(OWNET6_DPU_PORT) ||   \
      defined(OWNET7_DPU_PORT)
    #define OWNET_DPU_USED
  #endif    
  #if defined(OWNET0_SPU_PORT) ||   \
      defined(OWNET1_SPU_PORT) ||   \
      defined(OWNET2_SPU_PORT) ||   \
      defined(OWNET3_SPU_PORT) ||   \
      defined(OWNET4_SPU_PORT) ||   \
      defined(OWNET5_SPU_PORT) ||   \
      defined(OWNET6_SPU_PORT) ||   \
      defined(OWNET7_SPU_PORT)
    #define OWNET_SPU_USED
  #endif    
  #if defined(OWNET0_PPU_PORT) ||   \
      defined(OWNET1_PPU_PORT) ||   \
      defined(OWNET2_PPU_PORT) ||   \
      defined(OWNET3_PPU_PORT) ||   \
      defined(OWNET4_PPU_PORT) ||   \
      defined(OWNET5_PPU_PORT) ||   \
      defined(OWNET6_PPU_PORT) ||   \
      defined(OWNET7_PPU_PORT)
    #define OWNET_PPU_USED
  #endif    

  typedef struct tagOWNETWORKDEFS {
    BYTE *SensePortI;   // Sense Port (input)
    BYTE *SensePortO;   // Sense Port (output)
    BYTE *SenseTris;    // Sense Tris
    BYTE  SensePin;     // Sense Pin
    BYTE  SensePolarity;// Sense Polarity (not used)
    BYTE *DrivePort;    // Drive Port
    BYTE *DriveTris;    // Drive Tris
    BYTE  DrivePin;     // Drive Pin
    BYTE  DrivePolarity;// Drive Polarity
    BYTE *DPUPort;      // Dynamic PullUp Port
    BYTE *DPUTris;      // Dynamic PullUp Tris
    BYTE  DPUPin;       // Dynamic PullUp Pin
    BYTE  DPUPolarity;  // Dynamic PullUp Polarity
    BYTE *SPUPort;      // Strong PullUp Port
    BYTE *SPUTris;      // Strong PullUp Tris
    BYTE  SPUPin;       // Strong PullUp Pin
    BYTE  SPUPolarity;  // Strong PullUp Polarity
    BYTE *PPUPort;      // Program Pulse Port
    BYTE *PPUTris;      // Program Pulse Tris
    BYTE  PPUPin;       // Program Pulse Pin
    BYTE  PPUPolarity;  // Program Pulse Polarity
  } OWNETWORKDEFS;

  // One Wire functions defined in wizCnet.c (MultiBus)
  BYTE  owFirst(BYTE portnum, BYTE do_reset, BYTE alarm_only);
  BYTE  owNext(BYTE portnum, BYTE do_reset, BYTE alarm_only);
  void  owSerialNum(BYTE portnum, BYTE *serialnum_buf, BYTE do_read);
  void  owFamilySearchSetup(BYTE portnum, BYTE search_family);
  void  owSkipFamily(BYTE portnum);
  BYTE  owAccess(BYTE portnum);
  BYTE  owVerify(BYTE portnum, BYTE alarm_only);
  BYTE  owOverdriveAccess(BYTE portnum);

  // external One Wire functions defined in wizCses.c (MultiBus)
  BYTE owAcquire(BYTE portnum, BYTE *port_zstr);
  void owRelease(BYTE portnum);
  void owSetCurrentPort(BYTE portnum);
  // external One Wire globals from wizCses.c (MultiBus)
  extern BYTE           owCurrentPortnum;
  extern OWNETWORKDEFS  owNetCurrentDefs;
  extern OWNETWORKVARS  owNetCurrent;
  
    // external One Wire functions from link layer wizClnk.c
  BYTE owTouchReset(BYTE portnum);
  BYTE owTouchBit(BYTE portnum, BYTE sendbit);
  BYTE owTouchByte(BYTE portnum, BYTE sendbyte);
  BYTE owWriteByte(BYTE portnum, BYTE sendbyte);
  BYTE owReadByte(BYTE portnum);
  BYTE owSpeed(BYTE portnum, BYTE new_speed);
  BYTE owLevel(BYTE portnum, BYTE new_level);
  BYTE owProgramPulse(BYTE portnum);
  BYTE owWriteBytePower(BYTE portnum, BYTE sendbyte);
  BYTE owReadBytePower(BYTE portnum);
  BYTE owHasPowerDelivery(BYTE portnum);
  BYTE owHasProgramPulse(BYTE portnum);
  BYTE owHasOverDrive(BYTE portnum);
  BYTE owReadBitPower(BYTE portnum, BYTE applyPowerResponse);
  // external One Wire global from wizClnk.c (MultiBus)
  extern BYTE FAMILY_CODE_04_ALARM_TOUCHRESET_COMPLIANCE;

  // external One Wire functions from transaction layer in wizCtran.c (MultiBus)
  BYTE owBlock(BYTE portnum, BYTE do_reset, BYTE *tran_buf, BYTE tran_len);
  BYTE owReadPacketStd(BYTE portnum, BYTE do_access, int start_page, BYTE *read_buf);
  BYTE owWritePacketStd(BYTE portnum, int start_page, BYTE *write_buf,
                        BYTE write_len, BYTE is_eprom, BYTE crc_type);
  BYTE owProgramByte(BYTE portnum, BYTE write_byte, int addr, BYTE write_cmd,
                     BYTE crc_type, BYTE do_access);

  // external functions defined in wizCcrc.c (MultiBus)
  void setcrc16(BYTE portnum, WORD reset);
  WORD docrc16(BYTE portnum, WORD cdata);
  void setcrc8(BYTE portnum, BYTE reset);
  BYTE docrc8(BYTE portnum, BYTE x);
#endif

#endif	/*OWNET_H */
