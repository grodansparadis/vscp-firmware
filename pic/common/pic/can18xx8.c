/*********************************************************************
 *
 *              PIC18CXX8 CAN C Library Source Code
 *
 *********************************************************************
 * FileName:        CAN18CXX8.C
 * Dependencies:    CAN18CXX8.h
 * Date:            09/06/00
 * Processor:       PIC18CXX8
 * Complier:        MPLAB 5.11.00
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
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
 *                              (Rev 1.1)
 * Nilesh Rajbharti     6/5/02  Modified RXB0DLC_RTR def to fix compiler
 *                              missing def problem.
 *                              Fixed CANIDToRegs() where "static"
 *                              mode would not compile.  (Rev 1.2)
 *
 ********************************************************************/

#include <p18cxxx.h>
#include "can18xx8.h"


#if defined(MCHP_C18)
#include <p18cxxx.h>    // p18cxxx.h must have current processor
// defined.
#endif

#if defined(HITECH_C18)
#include <pic18.h>
#endif




#if defined(MCHP_C18)
#define RXB0CON_RX0DBEN     RXB0CONbits.RXB0DBEN    // AKHE
#define BRGCON2_SAM         BRGCON2bits.SAM
#define BRGCON2_SEG2PHTS    BRGCON2bits.SEG2PHTS
#define BRGCON3_WAKFIL      BRGCON3bits.WAKFIL
#define PIR3_RXB0IF         PIR3bits.RXB0IF
#define COMSTAT_RX0OVFL     COMSTATbits.RXB0OVFL    // AKHE
#define PIR3_RXB1IF         PIR3bits.RXB1IF
#define COMSTAT_RX1OVFL     COMSTATbits.RXB1OVFL    // AKHE
#define RXB0DLC_RTR         (RXB0DLC & 0x40) // RXB0DLCbits.RTR
#define RXB0SIDL_EXID       RXB0SIDLbits.EXID
#define PIR3_IRXIF          PIR3bits.IRXIF
#endif

#if defined(HITECH_C18)
#define RXB0CON_RX0DBEN     RXB0DBEN
#define BRGCON2_SAM         SAM
#define BRGCON2_SEG2PHTS    SEG2PHTS
#define BRGCON3_WAKFIL      WAKFIL
#define PIR3_RXB0IF         RXB0IF
#define COMSTAT_RX0OVFL     RXB0OVFL
#define PIR3_RXB1IF         RXB1IF
#define COMSTAT_RX1OVFL     RXB1OVFL
#define RXB0DLC_RTR         RXB0RXRTR
#define RXB0SIDL_EXID       RXB0EXID
#define PIR3_IRXIF          IRXIF
#endif



/*
 * Private helper functions to convert 32-bit CAN ID value into
 * corresponding PIC18CXX8 registers and vice-versa.
 */

static void CANIDToRegs(BYTE* ptr,
        unsigned long val,
        enum CAN_CONFIG_FLAGS type);
static void RegsToCANID(BYTE* ptr,
        unsigned long *val,
        enum CAN_CONFIG_FLAGS type);

/*********************************************************************
 * Function:        void CANInitialize(BYTE SJW,
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
 * Output:          CAN bit rate is set. All masks registers are set
 *                  '0' to allow all messages.
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
 *
 ********************************************************************/
void CANInitialize(BYTE SJW,
        BYTE BRP,
        BYTE PHSEG1,
        BYTE PHSEG2,
        BYTE PROPSEG,
        enum CAN_CONFIG_FLAGS config) {
    BYTE FilterConfig1;
    BYTE FilterConfig2;

    // In order to setup necessary config parameters of CAN module,
    // it must be in CONFIG mode.
    CANSetOperationMode(CAN_OP_MODE_CONFIG);

    // Now set the baud rate.
    CANSetBaudRate(SJW,
            BRP,
            PHSEG1,
            PHSEG2,
            PROPSEG,
            config);


    RXB0CON = config & CAN_CONFIG_MSG_BITS;
    if ((config & CAN_CONFIG_DBL_BUFFER_BIT)
            == CAN_CONFIG_DBL_BUFFER_ON) {
        RXB0CON_RX0DBEN = 1;
    }


    RXB1CON = RXB0CON;

    // Set default filter and mask registers for all receive buffers.
    CANSetMask(CAN_MASK_B1, 0, CAN_CONFIG_XTD_MSG);
    CANSetMask(CAN_MASK_B2, 0, CAN_CONFIG_XTD_MSG);

    switch ((config & CAN_CONFIG_MSG_BITS) | ~CAN_CONFIG_MSG_BITS) {
        case CAN_CONFIG_VALID_XTD_MSG:
            FilterConfig1 = CAN_CONFIG_XTD_MSG;
            FilterConfig2 = CAN_CONFIG_XTD_MSG;
            break;

        case CAN_CONFIG_VALID_STD_MSG:
            FilterConfig1 = CAN_CONFIG_STD_MSG;
            FilterConfig2 = CAN_CONFIG_STD_MSG;
            break;
        default:
            FilterConfig1 = CAN_CONFIG_STD_MSG;
            FilterConfig2 = CAN_CONFIG_XTD_MSG;
            break;
    }

    // By default, there will be no mask on any receive filters,
    // hence filter value of '0' will be ignored.
    CANSetFilter(CAN_FILTER_B1_F1, 0, FilterConfig1);
    CANSetFilter(CAN_FILTER_B1_F2, 0, FilterConfig1);
    CANSetFilter(CAN_FILTER_B2_F1, 0, FilterConfig2);
    CANSetFilter(CAN_FILTER_B2_F2, 0, FilterConfig2);
    CANSetFilter(CAN_FILTER_B2_F3, 0, FilterConfig2);
    CANSetFilter(CAN_FILTER_B2_F4, 0, FilterConfig2);

    // Restore to Normal mode.
    CANSetOperationMode(CAN_OP_MODE_NORMAL);
}

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
 * Overview:        Given mode byte is copied to CANSTAT and made
 *                  sure that requested mode is set.
 *
 * Note:            This is a blocking call.  It will not return until
 *                  requested mode is set.
 ********************************************************************/
void CANSetOperationMode(enum CAN_OP_MODE mode) {
    // Request desired mode.
    CANCON = mode;

    // Wait till desired mode is set.
    while ((CANSTAT & CAN_OP_MODE_BITS) != mode);
}

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
 * Overview:        Given values are bit adjusted to fit in 18CXX8
 *                  BRGCONx registers and copied.
 *
 ********************************************************************/
void CANSetBaudRate(BYTE SJW,
        BYTE BRP,
        BYTE PHSEG1,
        BYTE PHSEG2,
        BYTE PROPSEG,
        enum CAN_CONFIG_FLAGS flags) {
    // Actual values are offset '0'.
    // Hence map given values from offset '1' to offset '0'
    SJW--;
    BRP--;
    PHSEG1--;
    PHSEG2--;
    PROPSEG--;

    // Bit adjust given values into their appropriate registers.
    BRGCON1 = SJW << 6;
    BRGCON1 |= BRP;

    BRGCON2 = PHSEG1 << 3;
    BRGCON2 |= PROPSEG;

    if (!(flags & CAN_CONFIG_SAMPLE_BIT))
        BRGCON2_SAM = 1;

    if (flags & CAN_CONFIG_PHSEG2_PRG_BIT)
        BRGCON2_SEG2PHTS = 1;


    BRGCON3 = PHSEG2;
    if (flags & CAN_CONFIG_LINE_FILTER_BIT)
        BRGCON3_WAKFIL = 1;

}

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANIDToRegs(BYTE* ptr,
 *                                      unsigned long val,
 *                                      enum CAN_CONFIG_FLAGS type)
 *
 * PreCondition:    None
 *
 * Input:           ptr     - Starting address of a buffer to be updated
 *                  val     - 32-bit value to be converted
 *                  type    - Type of message - either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          Given CAN id value 'val' is bit adjusted and copied
 *                  into corresponding PIC18CXX8 CAN registers
 *
 * Side Effects:    None
 *
 * Overview:        If given id is of type standard identifier,
 *                  only SIDH and SIDL are updated
 *                  If given id is of type extended identifier,
 *                  bits val<17:0> is copied to EIDH, EIDL and SIDH<1:0>
 *                  bits val<28:18> is copied to SIDH and SIDL
 *
 ********************************************************************/
static void CANIDToRegs(BYTE* ptr,
        unsigned long val,
        enum CAN_CONFIG_FLAGS type) {
    CAN_MESSAGE_ID *Value;

    Value = (CAN_MESSAGE_ID*) & val;

    if (type & CAN_CONFIG_MSG_TYPE_BIT) {
        // Standard Identifier
        *ptr = Value->BYTES.BYTE_1 >> 3; // Copy SID<7:3> to SIDH<4:0>
        *ptr |= (Value->BYTES.BYTE_2 << 5); // Copy SID<10:8> to SIDH<7:5>
        ptr++; // Point to SIDL
        *ptr = Value->BYTES.BYTE_1 << 5; // Copy SID<2:0> to SIDL<7:5>
    } else {
        // Extended Identifier
        *ptr = Value->BYTES.BYTE_3 >> 5; // Copy EID<23:21> to SIDH<2:0>
        *ptr |= Value->BYTES.BYTE_4 << 3; // Copy EID<28:24> to SIDH<7:3>
        ptr++; // Point to SIDL
        *ptr = (Value->BYTES.BYTE_3 << 3) & 0xE0; // Copy EID<20:18> to SIDL<7:5>
        // mask out EID<17:16> bits
        *ptr |= 0b00001000; // Set EXIDEN bit  to SIDL<3>
        *ptr |= Value->BYTES.BYTE_3 & 0x03; // Copy EID<17:16> to SIDL<1:0>
        ptr++; // Point to EIDH
        *ptr = Value->BYTES.BYTE_2; // Copy EID<15:8> to EIDH<7:0>
        ptr++; // Point to EIDL
        *ptr = Value->BYTES.BYTE_1; // Copy EID<7:0> to EIDL<7:0>
    }
}

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void RegsToCANID(BYTE *ptr,
 *                                      unsigned long *val,
 *                                      enum CAN_CONFIG_FLAGS type)
 *
 * PreCondition:    None
 *
 * Input:           ptr     - Starting address of a buffer to be updated
 *                  val     - 32-bit buffer to hold value
 *                  type    - Type of message - either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          CAN registers starting at given address are bit
 *                  adjusted and copied into 'val'
 *
 * Side Effects:    None
 *
 * Overview:        If given id is of type standard identifier,
 *                  only SIDH and SIDL are used
 *                  If given id is of type extended identifier,
 *                  bits EIDH, EIDL and SIDL<1:0> is copied to val<17:0>
 *                  bits SIDH and SIDL is copied to val<28:18>
 *
 ********************************************************************/
static void RegsToCANID(BYTE* ptr,
        unsigned long *val,
        enum CAN_CONFIG_FLAGS type) {
    CAN_MESSAGE_ID *Value;

    Value = (CAN_MESSAGE_ID*) val;

    if (type & CAN_CONFIG_MSG_TYPE_BIT) {
        // Standard Identifier
        Value->BYTES.BYTE_1 = (*ptr << 3); // Copy SIDH<4:0> to SID<7:3>
        Value->BYTES.BYTE_2 = *ptr >> 5; // Copy SIDH<7:5> to SID<10:8>
        ptr++; // Point to SIDL
        Value->BYTES.BYTE_1 |= (*ptr >> 5); // Copy SIDL<7:6> to SID<2:0>
        Value->BYTES.BYTE_3 = 0x00;
        Value->BYTES.BYTE_4 = 0x00;
    } else {
        // Extended Identifier
        Value->BYTES.BYTE_3 = (*ptr << 5); // Copy SIDH<2:0> to EID<23:21>
        Value->BYTES.BYTE_4 = (*ptr >> 3); // Copy SIDH<7:3> to EID<29:25>
        ptr++; // Point to SIDL
        Value->BYTES.BYTE_3 |= (*ptr & 0x03); // Copy SIDH<1:0> to EID<17:16>
        // Bug-Fix NKR 11/20/00
        Value->BYTES.BYTE_3 |= ((*ptr & 0xe0) >> 3); // Copy SIDL<7:6> to EID<20:18>
        ptr++; // Point to EIDH
        Value->BYTES.BYTE_2 = *ptr; // Copy EIDH<15:8> to EID<15:8>
        ptr++; // Point to EIDL
        Value->BYTES.BYTE_1 = *ptr; // Copy EIDH<7:0> to EID<7:0>
    }
}

/*********************************************************************
 * Function:        void CANSetMask(enum CAN_MASK code,
 *                                  unsigned long val,
 *                                  enum CAN_CONFIG_FLAGS type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values will be ignored.
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
 *
 ********************************************************************/
void CANSetMask(enum CAN_MASK code,
        unsigned long val,
        enum CAN_CONFIG_FLAGS type) {
    BYTE *ptr;

    // Select appropriate starting address based on given CAN_MASK
    // value.
    if (code == CAN_MASK_B1)
        ptr = (BYTE*) & RXM0SIDH;
    else
        ptr = (BYTE*) & RXM1SIDH;

    // Convert given 32-bit id value into corresponding register values.
    CANIDToRegs(ptr, val, type);
}

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetFilter(enum CAN_FILTER code,
 *                                   unsigned long val,
 *                                   enum CAN_CONFIG type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values will be ignored.
 *
 *
 Input:             code    - One of CAN_FILTER value
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
        unsigned long val,
        enum CAN_CONFIG_FLAGS type) {
    BYTE *ptr;

    // Select appropriate starting address based on given CAN_FILTER
    // code.
    switch (code) {
        case CAN_FILTER_B1_F1:
            ptr = (BYTE*) & RXF0SIDH;
            break;

        case CAN_FILTER_B1_F2:
            ptr = (BYTE*) & RXF1SIDH;
            break;

        case CAN_FILTER_B2_F1:
            ptr = (BYTE*) & RXF2SIDH;
            break;

        case CAN_FILTER_B2_F2:
            ptr = (BYTE*) & RXF3SIDH;
            break;

        case CAN_FILTER_B2_F3:
            ptr = (BYTE*) & RXF4SIDH;
            break;

        default:
            ptr = (BYTE*) & RXF5SIDH;
            break;
    }

    // Convert 32-bit value into register values.
    CANIDToRegs(ptr, val, type);

}

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
        BYTE* Data,
        BYTE DataLen,
        enum CAN_TX_MSG_FLAGS MsgFlags) {
    BYTE i;
    BYTE *ptr;

    // Find the first empty transmitter.
    if (TXB0CON_TXREQ == 0) {
        // TxBuffer0 is empty.  Set WIN bits to point to TXB0
        CANCON &= 0b11110001;
        CANCON |= 0b00001000;
    } else if (TXB1CON_TXREQ == 0) {
        // TxBuffer1 is empty. Set WIN bits to point to TXB1
        CANCON &= 0b11110001;
        CANCON |= 0b00000110;
    } else if (TXB2CON_TXREQ == 0) {
        // TxBuffer2 is empty. Set WIN bits to point to TXB2
        CANCON &= 0b11110001;
        CANCON |= 0b00000100;
    } else
        // None of the transmit buffers were empty.
        return FALSE;

    /*
     * Now that WIN has remapped RXB0 to empty buffer, simply
     * populate RXB0 buffer
     */

    // Set transmit priority.
    RXB0CON = MsgFlags & CAN_TX_PRIORITY_BITS;

    // Populate Extended identifier information only if it is
    // desired.
    if (!(MsgFlags & CAN_TX_FRAME_BIT))
        CANIDToRegs((BYTE*) & RXB0SIDH, id, CAN_CONFIG_XTD_MSG);
    else
        CANIDToRegs((BYTE*) & RXB0SIDH, id, CAN_CONFIG_STD_MSG);

    RXB0DLC = DataLen;

    if (!(MsgFlags & CAN_TX_RTR_BIT))
        RXB0DLC |= 0b01000000;

    // Populate data values.
    ptr = (BYTE*) & RXB0D0;
    for (i = 0; i < DataLen; i++)
        ptr[i] = Data[i];

    /*
     * Mark this buffer as ready to start transmit.
     * We are not using C bit field structure because RXB0 registers
     * are remapped to one of the empty transmit buffers and their
     * bit3 is not same as RXB0CON bit3.  To avoid confusion, in-line
     * assembly is used to directly set bit 3 of corresponding TXBnCON
     * register.
     */
#if defined(MCHP_C18)
    _asm
    bsf RXB0CON, 3, 0
            _endasm
#endif
#if defined(HITECH_C18)
            asm("bsf _RXB0CON,3");
#endif

    /*
     * Restore CAN buffer mapping so that subsequent access to RXB0
     * buffers are to the real RXB0 buffer.
     */
    CANCON &= 0b11110001;

    return TRUE;
}

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        BOOL CANReceiveMessage(unsigned long *id,
 *                                  BYTE *Data,
 *                                  BYTE DataLen,
 *                                  enum CAN_RX_MSG_FLAGS MsgFlags)
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
 *                  it is extrated and returned.
 *                  If none found FALSE value is returned.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOL CANReceiveMessage(unsigned long *id,
        BYTE *Data,
        BYTE *DataLen,
        enum CAN_RX_MSG_FLAGS *MsgFlags) {
    BYTE i;
    BYTE *ptr;
    BOOL lbIsItBuffer0;

    // Start with no error or flags set.
    *MsgFlags = 0x0;

    // Find which buffer is ready.
    if (RXB0CON_RXFUL) {
        // RXBuffer0 is full.
        CANCON &= 0b11110001;

        lbIsItBuffer0 = TRUE;

        // Clear the received flag.
        PIR3_RXB0IF = 0;

        // Record and forget any previous overflow
        if (COMSTAT_RX0OVFL) {
            *MsgFlags |= CAN_RX_OVERFLOW;
            COMSTAT_RX0OVFL = 0;
        }

        if (RXB0CON_RX0DBEN) {
            *MsgFlags |= RXB0CON & CAN_RX_FILTER_BITS;
            *MsgFlags &= 0x01;
        }
    } else if (RXB1CON_RXFUL) {
        // RXBuffer1 is full
        CANCON &= 0b11110001;
        CANCON |= 0b00001010;

        lbIsItBuffer0 = FALSE;

        // Clear the received flag.
        PIR3_RXB1IF = 0;

        // Record and forget any previous overflow
        if (COMSTAT_RX1OVFL) {
            *MsgFlags |= CAN_RX_OVERFLOW;
            COMSTAT_RX1OVFL = 0;
        }

        *MsgFlags |= RXB1CON & CAN_RX_FILTER_BITS;
        if (*MsgFlags < 0x02)
            *MsgFlags |= CAN_RX_DBL_BUFFERED;
    } else
        return FALSE;

    // Retrieve message length.
    *DataLen = RXB0DLC & 0b00001111;

    // Determine whether this was RTR or not.
    if (RXB0DLC_RTR)
        *MsgFlags |= CAN_RX_RTR_FRAME;

    // Retrieve EIDX bytes only if this is extended message
    if (RXB0SIDL_EXID) {
        *MsgFlags |= CAN_RX_XTD_FRAME;

        RegsToCANID((BYTE*) & RXB0SIDH, id, CAN_CONFIG_XTD_MSG);
    } else
        RegsToCANID((BYTE*) & RXB0SIDH, id, CAN_CONFIG_STD_MSG);

    // Get message data itself
    ptr = (BYTE*) & RXB0D0;
    for (i = 0; i < *DataLen; i++)
        Data[i] = ptr[i];

    // Restore default RXB0 mapping.
    CANCON &= 0b11110001;

    // Record and Clear any previous invalid message bit flag.
    if (PIR3_IRXIF) {
        *MsgFlags |= CAN_RX_INVALID_MSG;
        PIR3_IRXIF = 0;
    }

    if (lbIsItBuffer0)
        RXB0CON_RXFUL = 0;
    else
        RXB1CON_RXFUL = 0;

    return TRUE;
}

