/*********************************************************************
 *              PIC18CXX8 CAN C Library Header File
 *********************************************************************
 * FileName:        CAN18CXX8.h
 * Dependencies:    None
 * Date:            09/06/00
 * Processor:       PIC18CXX8
 * Complier:        MPLAB 5.11.00
 * Company:         Microchip Technology, Inc.
 *
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 *  Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     9/6/00  Original        (Rev 1.0)
 * Nilesh Rajbharti     12/1/00 Fixed bugs.
 *                              (CANRegsToID, CANReceiveMessage,
 *                               CANSetMask + Added
 *                               CAN_CONFIG_DBL_BUFFER_ON/OFF option)
 * Nilesh Rajbharti     6/8/01  Renamed CAN_CONFIG_DBL_BUFFERED to
 *                              CAN_RX_DBL_BUFFERED
 * Nilesh Rajbharti     10/11/01 Added support for HITECH compiler
 *                              Modified CAN_MESSAGE_ID def to be
 *                              compatible with HITECH Compiler
 *                              Fixed CAN_CONFIG_SAMPLE_BIT enum.
 *                              (Rev 1.1)
 *
 *********************************************************************/
#ifndef CAN18XX8_H          // To avoid duplicate inclusion
#define CAN18XX8_H

#if defined(HI_TECH_C)
#define HITECH_C18
#else
#define MCHP_C18
#endif

#if defined(MCHP_C18) && defined(HITECH_C18)
#error "Invalid Compiler selection."
#endif

#if !defined(MCHP_C18) && !defined(HITECH_C18)
#error "Compiler not supported."
#endif

#if defined(MCHP_C18)
#define COMSTAT_TXB0        COMSTATbits.TXB0 
#define COMSTAT_TXBP        COMSTATbits.TXBP
#define COMSTAT_RXBP        COMSTATbits.RXBP
#define CANCON_ABAT         CANCONbits.ABAT
#define RXB0CON_RXFUL       RXB0CONbits.RXFUL
#define RXB1CON_RXFUL       RXB1CONbits.RXFUL
#define TXB0CON_TXREQ       TXB0CONbits.TXREQ
#define TXB1CON_TXREQ       TXB1CONbits.TXREQ
#define TXB2CON_TXREQ       TXB2CONbits.TXREQ
#endif

#if defined(HITECH_C18)
#define COMSTAT_TXB0        TXB0
#define COMSTAT_TXBP        TXBP
#define COMSTAT_RXBP        RXBP
#define CANCON_ABAT         ABAT
#define RXB0CON_RXFUL       RXB0RXFUL
#define TXB0CON_TXREQ       TXB0REQ

/*
 * Following are special defs to overcome compiler problem
 * at the time of this re-write.
 * Set following line to "#if 0" after verifiying correct
 * compiler behavior.
 */
#if 1

static struct {
    unsigned : 7;
    unsigned RXFUL : 1;
} RXB1CONbits @ 0xf50;
#define RXB1CON_RXFUL       RXB1CONbits.RXFUL

static struct {
    unsigned TXPRI0 : 1;
    unsigned TXPRI1 : 1;
    unsigned : 1;
    unsigned TXREQ : 1;
    unsigned TXERR : 1;
    unsigned TXLARB : 1;
    unsigned TXABT : 1;
} TXB1CONbits @ 0xf30;
#define TXB1CON_TXREQ       TXB1CONbits.TXREQ

static struct {
    unsigned TXPRI0 : 1;
    unsigned TXPRI1 : 1;
    unsigned : 1;
    unsigned TXREQ : 1;
    unsigned TXERR : 1;
    unsigned TXLARB : 1;
    unsigned TXABT : 1;
} TXB2CONbits @ 0xf20;
#define TXB2CON_TXREQ       TXB2CONbits.TXREQ
#else
#define RXB1CON_RXFUL       RXB1RXFUL
#define TXB1CON_TXREQ       TXB1REQ
#define TXB2CON_TXREQ       TXB2REQ
#endif



#endif

/*********************************************************************
 *
 * General purpose typedef's used by PICCAN libray.
 *
 * Remove these definations if they are already defined in one of your
 * application files.  Make sure that corresponding header file is
 * included before including this header file.
 ********************************************************************/
typedef enum _BOOL {
    FALSE = 0, TRUE
} BOOL;
typedef unsigned char BYTE;

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * union CAN_MESSAGE_ID
 *
 * This union provides abstract data type for CAN message id.
 * It is used for both 11-bit and 29-bit message identifiers.
 * There are multiple union members to be able to access individual
 * parts of it.
 *
 ********************************************************************/
// Parse-out 29-bit or 11-bit (saved in 32-bit number)

typedef union _CAN_MESSAGE_ID {
    unsigned long ID;

    struct {

        struct {
            unsigned SIDL : 3; // SIDL<5:7>
            unsigned SIDH : 5; // SIDH<0:4>
        } BYTE1;

        struct {
            unsigned SIDHU : 3; // SIDH<5:7>
            unsigned EIDL_LN : 5; // EIDL<0:4>
        } BYTE2;

        struct {
            unsigned EIDL_UN : 3; // EIDL<5:7>
            unsigned EIDH_LN : 5; // EIDH<0:4>
        } BYTE3;

        struct {
            unsigned EIDH_UN : 3; // EIDH<5:7>
            unsigned EIDHU : 2; // SIDL<0:1>
            unsigned : 3;
        } BYTE4;
    } ID_VALS;

    // This is to allow individual byte access within message id.

    struct {
        BYTE BYTE_1;
        BYTE BYTE_2;
        BYTE BYTE_3;
        BYTE BYTE_4;
    } BYTES;
} CAN_MESSAGE_ID;

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_TX_MSG_FLAGS
 *
 * This enumeration values define flags related to transmission of a
 * CAN message.  There could be more than one this flag
 * ANDed together to form multiple flags.
 *
 *********************************************************************/
enum CAN_TX_MSG_FLAGS {
    CAN_TX_PRIORITY_BITS = 0b00000011,
    CAN_TX_PRIORITY_0 = 0b11111100, // XXXXXX00
    CAN_TX_PRIORITY_1 = 0b11111101, // XXXXXX01
    CAN_TX_PRIORITY_2 = 0b11111110, // XXXXXX10
    CAN_TX_PRIORITY_3 = 0b11111111, // XXXXXX11

    CAN_TX_FRAME_BIT = 0b00001000,
    CAN_TX_STD_FRAME = 0b11111111, // XXXXX1XX
    CAN_TX_XTD_FRAME = 0b11110111, // XXXXX0XX

    CAN_TX_RTR_BIT = 0b01000000,
    CAN_TX_NO_RTR_FRAME = 0b11111111, // X1XXXXXX
    CAN_TX_RTR_FRAME = 0b10111111 // X0XXXXXX
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_RX_MSG_FLAGS
 *
 * This enumeration values define flags related to reception of a CAN
 * message.  There could be more than one this flag
 * ANDed together to form multiple flags.
 * If a particular bit is set, corresponding meaning is TRUE or else
 * it will be FALSE.
 *
 * e.g.
 *      if (MsgFlag & CAN_RX_OVERFLOW)
 *      {
 *          // Receiver overflow has occured.  We have lost previous
 *          // message.
 *          ...
 *      }
 *
 ********************************************************************/
enum CAN_RX_MSG_FLAGS {
    CAN_RX_FILTER_BITS = 0b00000111, // Use this to access filter
    // bits
    CAN_RX_FILTER_1 = 0b00000000,
    CAN_RX_FILTER_2 = 0b00000001,
    CAN_RX_FILTER_3 = 0b00000010,
    CAN_RX_FILTER_4 = 0b00000011,
    CAN_RX_FILTER_5 = 0b00000100,
    CAN_RX_FILTER_6 = 0b00000101,

    CAN_RX_OVERFLOW = 0b00001000, // Set if Overflowed else
    // cleared

    CAN_RX_INVALID_MSG = 0b00010000, // Set if invalid else
    // cleared

    CAN_RX_XTD_FRAME = 0b00100000, // Set if XTD message else
    // cleared

    CAN_RX_RTR_FRAME = 0b01000000, // Set if RTR message else
    // cleared

    CAN_RX_DBL_BUFFERED = 0b10000000 // Set if this message was
    // hardware double-buffered
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_MASK
 *
 * This enumeration values define mask codes.  Routine CANSetMask()
 * requires this code as one of its arguments.  These enumerations
 * must be used by itself i.e. it cannot be ANDed to form multiple
 * values.
 *
 ********************************************************************/
enum CAN_MASK {
    CAN_MASK_B1,
    CAN_MASK_B2
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_FILTER
 *
 * This enumeration values define filter codes.  Routine CANSetFilter
 * requires this code as one of its arguments.  These enumerations
 * must be used by itself
 * i.e. it cannot be ANDed to form multiple values.
 *
 ********************************************************************/
enum CAN_FILTER {
    CAN_FILTER_B1_F1,
    CAN_FILTER_B1_F2,
    CAN_FILTER_B2_F1,
    CAN_FILTER_B2_F2,
    CAN_FILTER_B2_F3,
    CAN_FILTER_B2_F4
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_OP_MODE
 *
 * This enumeration values define codes related to CAN module
 * operation mode. CANSetOperationMode() routine requires this code.
 * These values must be used by itself
 * i.e. it cannot be ANDed to form * multiple values.
 *
 ********************************************************************/
enum CAN_OP_MODE {
    CAN_OP_MODE_BITS = 0b11100000, // Use this to access opmode
    // bits
    CAN_OP_MODE_NORMAL = 0b00000000,
    CAN_OP_MODE_SLEEP = 0b00100000,
    CAN_OP_MODE_LOOP = 0b01000000,
    CAN_OP_MODE_LISTEN = 0b01100000,
    CAN_OP_MODE_CONFIG = 0b10000000
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_CONFIG_FLAGS
 *
 * This enumeration values define flags related to configuring CAN
 * module. Routines CANInitialize() and CANSetBaudRate() use these
 * codes. One or more these values may be ANDed to form multiple
 * flags.
 *
 ********************************************************************/
enum CAN_CONFIG_FLAGS {
    CAN_CONFIG_DEFAULT = 0b11111111, // 11111111

    CAN_CONFIG_PHSEG2_PRG_BIT = 0b00000001,
    CAN_CONFIG_PHSEG2_PRG_ON = 0b11111111, // XXXXXXX1
    CAN_CONFIG_PHSEG2_PRG_OFF = 0b11111110, // XXXXXXX0

    CAN_CONFIG_LINE_FILTER_BIT = 0b00000010,
    CAN_CONFIG_LINE_FILTER_ON = 0b11111111, // XXXXXX1X
    CAN_CONFIG_LINE_FILTER_OFF = 0b11111101, // XXXXXX0X

    CAN_CONFIG_SAMPLE_BIT = 0b00000100,
    CAN_CONFIG_SAMPLE_ONCE = 0b11111111, // XXXXX1XX
    CAN_CONFIG_SAMPLE_THRICE = 0b11111011, // XXXXX0XX

    CAN_CONFIG_MSG_TYPE_BIT = 0b00001000,
    CAN_CONFIG_STD_MSG = 0b11111111, // XXXX1XXX
    CAN_CONFIG_XTD_MSG = 0b11110111, // XXXX0XXX

    CAN_CONFIG_DBL_BUFFER_BIT = 0b00010000,
    CAN_CONFIG_DBL_BUFFER_ON = 0b11111111, // XXX1XXXX
    CAN_CONFIG_DBL_BUFFER_OFF = 0b11101111, // XXX0XXXX

    CAN_CONFIG_MSG_BITS = 0b01100000,
    CAN_CONFIG_ALL_MSG = 0b11111111, // X11XXXXX
    CAN_CONFIG_VALID_XTD_MSG = 0b11011111, // X10XXXXX
    CAN_CONFIG_VALID_STD_MSG = 0b10111111, // X01XXXXX
    CAN_CONFIG_ALL_VALID_MSG = 0b10011111 // X00XXXXX
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANInitialize( BYTE SJW,
 *                                      BYTE BRP,
 *                                      BYTE PHSEG1,
 *                                      BYTE PHSEG2,
 *                                      BYTE PROPSEG,
 *                                      enum CAN_CONFIG_FLAGS flags)
 *
 * PreCondition:    MCU must be in Configuration mode or else these
 *                  values will be ignored.
 *
 * Input:           SJW     - SJW value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 4)
 *                  BRP     - BRP value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 64)
 *                  PHSEG1  - PHSEG1 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PHSEG2  - PHSEG2 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PROPSEG - PROPSEG value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  flags   - Value of type enum CAN_CONFIG_FLAGS
 *
 * Output:          CAN bit rate is set.
 *                  All masks registers are set '0'
 *                  to allow all messages.
 *                  Filter registers are set according to flag value.
 *                  If (config & CAN_CONFIG_VALID_XTD_MSG)
 *                      Set all filters to XTD_MSG
 *                  Else if (config & CONFIG_VALID_STD_MSG)
 *                      Set all filters to STD_MSG
 *                  Else
 *                      Set half of the filters to STD while rests to
 *                      XTD_MSG.
 *
 * Side Effects:    All pending transmissions are aborted.
 ********************************************************************/
void CANInitialize(BYTE SJW,
        BYTE BRP,
        BYTE PHSEG1,
        BYTE PHSEG2,
        BYTE PROPSEG,
        enum CAN_CONFIG_FLAGS config);

//////////////////////////////////////////////////////////////////////


/*********************************************************************
 * Function:        void CANSetOperationMode(CAN_OP_MODE mode)
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum CAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a blocking call.  It will not return until
 *                  requested mode is set.
 ********************************************************************/
void CANSetOperationMode(enum CAN_OP_MODE mode);


//////////////////////////////////////////////////////////////////////


/*********************************************************************
 * Macro:           void CANSetOperationModeNoWait(CAN_OP_MODE mode)
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum CAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a non-blocking call.
 *                  It does not verify that
 *                  CAN module is switched to requested mode or not.
 *                  Caller must use CANGetOperationMode() to verify
 *                  correct operation mode before performing mode
 *                  specific operation.
 *
 ********************************************************************/
#define CANSetOperationModeNoWait(mode) (CANCON = mode)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           CAN_OP_MODE CANSetOperationMode()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current operational mode of CAN module is returned
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetOperationMode() (CANCON & CAN_OP_MODE_BITS)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetBaudRate(BYTE SJW,
 *                                      BYTE BRP,
 *                                      BYTE PHSEG1,
 *                                      BYTE PHSEG2,
 *                                      BYTE PROPSEG,
 *                                      enum CAN_CONFIG_FLAGS flags)
 *
 * PreCondition:    MCU must be in Configuration mode or else these
 *                  values will be ignored.
 *
 * Input:           SJW     - SJW value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 4)
 *                  BRP     - BRP value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 64)
 *                  PHSEG1  - PHSEG1 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PHSEG2  - PHSEG2 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PROPSEG - PROPSEG value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  flags   - Value of type enum CAN_CONFIG_FLAGS
 *
 * Output:          CAN bit rate is set as per given values.
 *
 * Side Effects:    None
 *
 ********************************************************************/
void CANSetBaudRate(BYTE SJW,
        BYTE BRP,
        BYTE PHSEG1,
        BYTE PHSEG2,
        BYTE PROPSEG,
        enum CAN_CONFIG_FLAGS);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetMask(enum CAN_MASK code,
 *                                  unsigned long val,
 *                                  enum CAN_CONFIG_FLAGS type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values
 *                  will be ignored.
 *
 * Input:           code    - One of CAN_MASK value
 *                  val     - Actual mask register value.
 *                  type    - Type of message to filter either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          Given value is bit adjusted to appropriate buffer
 *                  mask registers.
 *
 * Side Effects:    None
 ********************************************************************/
void CANSetMask(enum CAN_MASK code,
        unsigned long Value,
        enum CAN_CONFIG_FLAGS type);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetFilter(enum CAN_FILTER code,
 *                                    unsigned long val,
 *                                    enum CAN_CONFIG type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values will be ignored.
 *
 * Input:           code    - One of CAN_FILTER value
 *                  val     - Actual filter register value.
 *                  type    - Type of message to filter either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          Given value is bit adjusted to appropriate buffer
 *                  filter registers.
 *
 * Side Effects:    None
 ********************************************************************/
void CANSetFilter(enum CAN_FILTER code,
        unsigned long Value,
        enum CAN_CONFIG_FLAGS);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        BOOL CANSendMessage(unsigned long id,
 *                                  BYTE *Data,
 *                                  BYTE DataLen,
 *                                  enum CAN_TX_MSG_FLAGS MsgFlags)
 *
 * PreCondition:    None
 *
 * Input:           id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type.
 *                  Data        - Data bytes of upto 8 bytes in length
 *                  DataLen     - Data length from 1 thru 8.
 *                  MsgFlags    - One or CAN_TX_MSG_FLAGS values ANDed
 *                                together
 *
 * Output:          If at least one empty transmit buffer is found,
 *                  given message is queued to be transmitted. If none
 *                  found FALSE value is returned.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOL CANSendMessage(unsigned long id,
        BYTE *Data,
        BYTE DataLen,
        enum CAN_TX_MSG_FLAGS MsgFlags);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        BOOL CANReceiveMessage(unsigned long *id,
 *                                  BYTE *Data,
 *                                  BYTE *DataLen,
 *                                  enum CAN_RX_MSG_FLAGS *MsgFlags)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          id          - CAN message identifier.
 *                  Data        - Data bytes of upto 8 bytes in length
 *                  DataLen     - Data length from 1 thru 8.
 *                  MsgFlags    - One or CAN_RX_MSG_FLAGS values ANDed
 *                                together
 *
 * Output:          If at least one full receive buffer is found,
 *                  it is extrated and returned. If none found FALSE
 *                  value is returned.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOL CANReceiveMessage(unsigned long* id,
        BYTE *Data,
        BYTE *DataLen,
        enum CAN_RX_MSG_FLAGS *MsgFlags);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANGetTxErrorCount()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current transmit error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetTxErrorCount()    (TXERRCNT)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANGetRxErrorCount()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current receive error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetRxErrorCount()    (RXERRCNT)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsBusOff()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN Module is off due to excessive error
 *                  FALSE is it is not off.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsBusOff()           (COMSTAT_TXB0)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsTxPassive()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN transmit module is error passive as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsTxPassive()        (COMSTAT_TXBP)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANIsRxPassive()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN receive module is error active as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsRxPassive()        (COMSTAT_RXBP)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           void CANAbortAll()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANAbortAll()           (CANCON_ABAT = 1)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL  CANIsRxReady()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if at least one of the CAN receive buffer is
 *                  empty FALSE if none receive buffers are empty.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsRxReady()          (RXB0CON_RXFUL || RXB1CON_RXFUL)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsTxReady()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if at least one CAN transmit buffer is empty
 *                  FALSE if all CAN transmit buffers are full
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsTxReady()          (!TXB0CON_TXREQ || \
                                 !TXB1CON_TXREQ || \
                                 !TXB2CON_TXREQ )

#endif      // CAN18XX8_H

