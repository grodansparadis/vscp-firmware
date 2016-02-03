/*********************************************************************
 *
 *              ECAN C Library Source Code
 *
 *********************************************************************
 * FileName:        ECAN.C
 * Dependencies:    p18cxxx.h/pic18.h, ECAN.h
 * Processor:       PIC18CXX8X
 * Compiler:        Microchip C 2.10.06 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
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
 * Caio Gubel           5/5/03  ECAN Version 1.0 - Initial Release
 * Nilesh R.            7/22/03 Improved.
 ********************************************************************/

#include "ecan.h"

void _CANIDToRegs(BYTE* ptr,
                 unsigned long val,
                 BYTE type);
void _RegsToCANID(BYTE* ptr,
                 unsigned long *val,
                 BYTE type);

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_2) )
    static BYTE* _ECANPointBuffer(BYTE b);
#endif

BYTE_VAL _ECANRxFilterHitInfo;

#define _SetStdRXFnValue(f, val)                                \
    ##f##SIDH = (long)ECAN_##f##_VAL >> 3L;                     \
    ##f##SIDL = (long)ECAN_##f##_VAL >> 5L

#define _SetXtdRXFnValue(f, val)                                \
    ##f##SIDH = (long)ECAN_##f##_VAL >> 21L;                    \
    ##f##SIDL = (((long)ECAN_##f##_VAL >> 13L) & 0xe0) |        \
                    ((long)(ECAN_##f##_VAL) & 0x03L)   |        \
                    0x08;                                       \
    ##f##EIDH = (long)ECAN_##f##_VAL >> 8L;                     \
    ##f##EIDL = ECAN_##f##_VAL;


#define _SetStdRXMnValue(m, val)                                \
    RXM##m##SIDH = (long)ECAN_RXM##m##_VAL >> 3L;               \
    RXM##m##SIDL = (long)ECAN_RXM##m##_VAL >> 5L

#define _SetXtdRXMnValue(m, val)                                \
    RXM##m##SIDH = (long)ECAN_RXM##m##_VAL >> 21L;              \
    RXM##m##SIDL = (((long)ECAN_RXM##m##_VAL >> 13L) & 0xe0) |  \
                    ((long)(ECAN_RXM##m##_VAL) & 0x03L);\
    RXM##m##EIDH = (long)ECAN_RXM##m##_VAL >> 8L;\
    RXM##m##EIDL = ECAN_RXM##m##_VAL;

#define RXF0            0
#define RXF1            1
#define RXF2            2
#define RXF3            3
#define RXF4            4
#define RXF5            5
#define RXF6            6
#define RXF7            7
#define RXF8            8
#define RXF9            9
#define RXF10           10
#define RXF11           11
#define RXF12           12
#define RXF13           13
#define RXF14           14
#define RXF15           15


/*
 * Compile-time validation ECAN options as per ECAN and ECAN rules.
 */
#if ( ECAN_SJW_VAL > 4 )
#error Invalid SJW value received.
#endif

#if ( ECAN_BRP_VAL > 64 )
#error Invalid BRP value received.
#endif

#if ( ECAN_PHSEG1_VAL > 8 )
#error Invalid PHSEG1 value received.
#endif

#if ( ECAN_PHSEG2_VAL > 8 )
#error Invalid PHSEG2 value received.
#endif

#if ( ECAN_PROPSEG_VAL > 8 )
#error Invalid PROPSEG value received.
#endif

// In Mode 1 & 2, double buffering on RXB0 is not available.


/*
 * In Mode 0, RXF0-RXF5 are always enabled.
 */
#if ( ECAN_FUNC_MODE_VAL == ECAN_MODE_0 )
    #undef ECAN_RXF0_MODE_VAL
    #define ECAN_RXF0_MODE_VAL      ECAN_RXF0_ENABLE

    #undef ECAN_RXF1_MODE_VAL
    #define ECAN_RXF1_MODE_VAL      ECAN_RXF1_ENABLE

    #undef ECAN_RXF2_MODE_VAL
    #define ECAN_RXF2_MODE_VAL      ECAN_RXF2_ENABLE

    #undef ECAN_RXF3_MODE_VAL
    #define ECAN_RXF3_MODE_VAL      ECAN_RXF3_ENABLE

    #undef ECAN_RXF4_MODE_VAL
    #define ECAN_RXF4_MODE_VAL      ECAN_RXF4_ENABLE

    #undef ECAN_RXF5_MODE_VAL
    #define ECAN_RXF5_MODE_VAL      ECAN_RXF5_ENABLE
#endif


/*********************************************************************
 * Function:        void ECANInitialize(void)
 *
 * Overview:        Use this function to initialize ECAN module with
 *                  options defined in ECAN.def file.
 *                  You may manually edit ECAN.def file as per your
 *                  requirements, or use Microchip Application
 *                  Maestro tool.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    All pending transmissions are aborted.
 ********************************************************************/
void ECANInitialize(void)
{

    // Put module into Configuration mode.
    ECANSetOperationMode(ECAN_OP_MODE_CONFIG);


    // Set Bit rate values as per defines.
    BRGCON1 = ((ECAN_SJW_VAL-1) << 6) | (ECAN_BRP_VAL-1);
    BRGCON2 = (ECAN_PHSEG2_MODE_VAL << 7)      | \
              (ECAN_BUS_SAMPLE_MODE_VAL << 6)  | \
              ((ECAN_PHSEG1_VAL-1) << 3)       | \
              (ECAN_PROPSEG_VAL-1);
    BRGCON3 = (ECAN_WAKEUP_MODE_VAL << 7)      |
              (ECAN_FILTER_MODE_VAL << 6)      |
              (ECAN_PHSEG2_VAL-1);

    // Set CANTX2, TXDRIVE and CAN Capture modes.
    CIOCON = ECAN_TX2_SOURCE_VAL << 7           | \
             ECAN_TX2_MODE_VAL << 6         | \
             ECAN_TXDRIVE_MODE_VAL << 5        | \
             ECAN_CAPTURE_MODE_VAL;


    // Set WCAN functional mode.
    ECANCON_MDSEL1 = ECAN_FUNC_MODE_VAL >> 7;
    ECANCON_MDSEL0 = ECAN_FUNC_MODE_VAL >> 6;


    // Set RXB0 and RXB1 buffer modes.
#if ( ECAN_FUNC_MODE_VAL == ECAN_MODE_0 )
    RXB0CON = (ECAN_RXB0_MODE_VAL << 5) | (ECAN_RXB0_DBL_BUFFER_MODE_VAL << 2);
    RXB1CON = ECAN_RXB1_MODE_VAL << 5;
#else
    // In Mode1 & 2, Map 2-bit RXM bits into 1 RXM1 bit.
    #if ( ECAN_RXB0_MODE_VAL == ECAN_RECEIVE_ALL )
        RXB0CON = 0x40;
    #else
        RXB0CON = 0;
    #endif

    #if ( ECAN_RXB1_MODE_VAL == ECAN_RECEIVE_ALL )
        RXB1CON = 0x40;
    #else
        RXB1CON = 0;
    #endif
#endif

    // B0-B5 are available in Mode 1 and 2 only.
#if (ECAN_FUNC_MODE_VAL != ECAN_MODE_0)
    #if (ECAN_B0_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B0_MODE_VAL == ECAN_RECEIVE_ALL )
            B0CON = 0x40;
        #else
            B0CON = 0;
        #endif
    #else
        B0CON = ECAN_B0_AUTORTR_MODE << 2;
    #endif

    #if (ECAN_B1_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B1_MODE_VAL == ECAN_RECEIVE_ALL )
            B1CON = 0x40;
        #else
            B1CON = 0;
        #endif
    #else
        B1CON = ECAN_B1_AUTORTR_MODE << 2;
    #endif

    #if (ECAN_B2_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B2_MODE_VAL == ECAN_RECEIVE_ALL )
            B2CON = 0x40;
        #else
            B2CON = 0;
        #endif
    #else
        B2CON = ECAN_B2_AUTORTR_MODE << 2;
    #endif

    #if (ECAN_B3_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B3_MODE_VAL == ECAN_RECEIVE_ALL )
            B3CON = 0x40;
        #else
            B3CON = 0;
        #endif
    #else
        B3CON = ECAN_B3_AUTORTR_MODE << 2;
    #endif

    #if (ECAN_B4_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B4_MODE_VAL == ECAN_RECEIVE_ALL )
            B4CON = 0x40;
        #else
            B4CON = 0;
        #endif
    #else
        B4CON = ECAN_B4_AUTORTR_MODE << 2;
    #endif

    #if (ECAN_B5_TXRX_MODE_VAL != ECAN_BUFFER_TX)
        #if ( ECAN_B5_MODE_VAL == ECAN_RECEIVE_ALL )
            B5CON = 0x40;
        #else
            B5CON = 0;
        #endif
    #else
        B5CON = ECAN_B5_AUTORTR_MODE << 2;
    #endif

    // Enable/disable buffers B0-B5.
    BSEL0 = ECAN_B5_TXRX_MODE_VAL << 7 | \
            ECAN_B4_TXRX_MODE_VAL << 6 | \
            ECAN_B3_TXRX_MODE_VAL << 5 | \
            ECAN_B2_TXRX_MODE_VAL << 4 | \
            ECAN_B1_TXRX_MODE_VAL << 3 | \
            ECAN_B0_TXRX_MODE_VAL << 2;
#endif




    // Assign value to RXF0 only if Mode0 is used or RXF0 itself is enabled.
#if ( (ECAN_RXF0_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    // Set Standard or Extended value.
    #if ( ECAN_RXF0_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF0, ECAN_RXF0_VAL);
    #else
        _SetXtdRXFnValue(RXF0, ECAN_RXF0_VAL);
    #endif
#endif

#if ( (ECAN_RXF1_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    #if ( ECAN_RXF1_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF1, ECAN_RXF1_VAL);
    #else
        _SetXtdRXFnValue(RXF1, ECAN_RXF1_VAL);
    #endif
#endif


#if ( (ECAN_RXF2_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    #if ( ECAN_RXF2_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF2, ECAN_RXF2_VAL);
    #else
        _SetXtdRXFnValue(RXF3, ECAN_RXF3_VAL);
    #endif
#endif


#if ( (ECAN_RXF3_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    #if ( ECAN_RXF3_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF3, ECAN_RXF3_VAL);
    #else
        _SetXtdRXFnValue(RXF3, ECAN_RXF3_VAL);
    #endif
#endif


#if ( (ECAN_RXF4_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    #if ( ECAN_RXF4_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF4, ECAN_RXF4_VAL);
    #else
        _SetXtdRXFnValue(RXF4, ECAN_RXF4_VAL);
    #endif
#endif

#if ( (ECAN_RXF5_MODE_VAL == ECAN_RXFn_ENABLE) || (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    #if ( ECAN_RXF5_MSG_TYPE_VAL == ECAN_MSG_STD )
        _SetStdRXFnValue(RXF5, ECAN_RXF4_VAL);
    #else
        _SetXtdRXFnValue(RXF5, ECAN_RXF5_VAL);
    #endif
#endif


    // Filters 6-15 are available in Modes 1 and 2 only.
#if (ECAN_FUNC_MODE_VAL != ECAN_MODE_0)
    #if ( ECAN_RXF6_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF6_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF6, ECAN_RXF6_VAL);
        #else
            _SetXtdRXFnValue(RXF6, ECAN_RXF6_VAL);
        #endif
    #endif

    #if ( ECAN_RXF7_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF7_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF7, ECAN_RXF7_VAL);
        #else
            _SetXtdRXFnValue(RXF7, ECAN_RXF7_VAL);
        #endif
    #endif

    #if ( ECAN_RXF8_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF8_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF8, ECAN_RXF9_VAL);
        #else
            _SetXtdRXFnValue(RXF8, ECAN_RXF9_VAL);
        #endif
    #endif

    #if ( ECAN_RXF9_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF9_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF9, ECAN_RXF9_VAL);
        #else
            _SetXtdRXFnValue(RXF9, ECAN_RXF9_VAL);
        #endif
    #endif

    #if ( ECAN_RXF10_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF10_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF10, ECAN_RXF10_VAL);
        #else
            _SetXtdRXFnValue(RXF10, ECAN_RXF10_VAL);
        #endif
    #endif

    #if ( ECAN_RXF11_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF11_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF11, ECAN_RXF11_VAL);
        #else
            _SetXtdRXFnValue(RXF11, ECAN_RXF11_VAL);
        #endif
    #endif

    #if ( ECAN_RXF12_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF12_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF12, ECAN_RXF12_VAL);
        #else
            _SetXtdRXFnValue(RXF12, ECAN_RXF12_VAL);
        #endif
    #endif

    #if ( ECAN_RXF13_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF13_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF13, ECAN_RXF13_VAL);
        #else
            _SetXtdRXFnValue(RXF13, ECAN_RXF13_VAL);
        #endif
    #endif

    #if ( ECAN_RXF14_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF14_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF14, ECAN_RXF14_VAL);
        #else
            _SetXtdRXFnValue(RXF14, ECAN_RXF14_VAL);
        #endif
    #endif

    #if ( ECAN_RXF15_MODE_VAL == ECAN_RXFn_ENABLE )
        #if ( ECAN_RXF15_MSG_TYPE_VAL == ECAN_MSG_STD )
            _SetStdRXFnValue(RXF15, ECAN_RXF15_VAL);
        #else
            _SetXtdRXFnValue(RXF15, ECAN_RXF15_VAL);
        #endif
    #endif
#endif

    // Enable/Disable filters in Modes 1 and 2 only.
#if ( ECAN_FUNC_MODE_VAL != ECAN_MODE_0 )
    RXFCON0 = (ECAN_RXF7_MODE_VAL << 7) | \
              (ECAN_RXF6_MODE_VAL << 6) | \
              (ECAN_RXF5_MODE_VAL << 5) | \
              (ECAN_RXF4_MODE_VAL << 4) | \
              (ECAN_RXF3_MODE_VAL << 3) | \
              (ECAN_RXF2_MODE_VAL << 2) | \
              (ECAN_RXF1_MODE_VAL << 1) | \
              (ECAN_RXF0_MODE_VAL);
    RXFCON1 = (ECAN_RXF15_MODE_VAL << 7) | \
              (ECAN_RXF14_MODE_VAL << 6) | \
              (ECAN_RXF13_MODE_VAL << 5) | \
              (ECAN_RXF12_MODE_VAL << 4) | \
              (ECAN_RXF11_MODE_VAL << 3) | \
              (ECAN_RXF10_MODE_VAL << 2) | \
              (ECAN_RXF9_MODE_VAL << 1)  | \
              (ECAN_RXF8_MODE_VAL);
#endif


    // Link each filter to corresponding buffer only if we are in Mode 1 or 2.
#if ( ECAN_FUNC_MODE_VAL != ECAN_MODE_0 )
    ECANLinkRXF0F1ToBuffer(ECAN_RXF0_BUFFER_VAL,     ECAN_RXF1_BUFFER_VAL);
    ECANLinkRXF2F3ToBuffer(ECAN_RXF2_BUFFER_VAL,     ECAN_RXF3_BUFFER_VAL);
    ECANLinkRXF4F5ToBuffer(ECAN_RXF4_BUFFER_VAL,     ECAN_RXF5_BUFFER_VAL);
    ECANLinkRXF6F7ToBuffer(ECAN_RXF6_BUFFER_VAL,     ECAN_RXF7_BUFFER_VAL);
    ECANLinkRXF8F9ToBuffer(ECAN_RXF8_BUFFER_VAL,     ECAN_RXF9_BUFFER_VAL);
    ECANLinkRXF10F11ToBuffer(ECAN_RXF10_BUFFER_VAL,  ECAN_RXF11_BUFFER_VAL);
    ECANLinkRXF12F13ToBuffer(ECAN_RXF12_BUFFER_VAL,  ECAN_RXF13_BUFFER_VAL);
    ECANLinkRXF14F15ToBuffer(ECAN_RXF14_BUFFER_VAL,  ECAN_RXF15_BUFFER_VAL);

    ECANLinkRXF0Thru3ToMask(ECAN_RXF0_MASK_VAL, \
                           ECAN_RXF1_MASK_VAL, \
                           ECAN_RXF2_MASK_VAL, \
                           ECAN_RXF3_MASK_VAL);
    ECANLinkRXF4Thru7ToMask(ECAN_RXF4_MASK_VAL, \
                           ECAN_RXF5_MASK_VAL, \
                           ECAN_RXF6_MASK_VAL, \
                           ECAN_RXF7_MASK_VAL);
    ECANLinkRXF8Thru11ToMask(ECAN_RXF8_MASK_VAL, \
                           ECAN_RXF9_MASK_VAL, \
                           ECAN_RXF10_MASK_VAL, \
                           ECAN_RXF11_MASK_VAL);
    ECANLinkRXF12Thru15ToMask(ECAN_RXF12_MASK_VAL, \
                           ECAN_RXF13_MASK_VAL, \
                           ECAN_RXF14_MASK_VAL, \
                           ECAN_RXF15_MASK_VAL);
#endif


#if ( ECAN_RXM0_MSG_TYPE == ECAN_MSG_STD )
    _SetStdRXMnValue(0, ECAN_RXM0_VAL);
    RXM0SIDL_EXIDEN = 0;
#else
    _SetXtdRXMnValue(0, ECAN_RXM0_VAL);
    RXM0SIDL_EXIDEN = 1;
#endif

#if ECAN_RXM1_MSG_TYPE == ECAN_MSG_STD
    _SetStdRXMnValue(1, ECAN_RXM1_VAL);
    RXM1SIDL_EXIDEN = 0;
#else
    _SetXtdRXMnValue(1, ECAN_RXM1_VAL);
    RXM1SIDL_EXIDEN = 1;
#endif

    // SDFLC value

    // Exit with speicfied mode.  If selected mode is Configuration,
    // we do not need to do anything.
#if ( ECAN_INIT_MODE != ECAN_INIT_CONFIGURATION )
    ECANSetOperationMode(ECAN_INIT_MODE);
#endif
}



/*********************************************************************
 * Function:        BOOL ECANLoadRTRBuffer(BYTE buffer,
 *                                         unsigned long id,
 *                                         BYTE *data,
 *                                         BYTE dataLen,
 *                                         BYTE type)
 *
 * Overview:        Use this function to update automatic RTR buffer.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer number that is to be loaded
 *                  id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type as specified in
 *                                type parameter.
 *                  data        - Data bytes of upto 8 bytes in length
 *                  dataLen     - Data length from 0 thru 8.
 *                                If 0, data may be NULL.
 *                  type        - Buffer type
 *                                Must be ECAN_MSG_STD for Standard
 *                                        ECAN_MSG_XTD for Extended
 *
 * Output:          TRUE, if given data was successfully loaded
 *                  FALSE, if RTR buffer was in process of transmitting
 *                          automated response.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#if ( defined(ECAN_ENABLE_AUTO_RTR) )
BOOL ECANLoadRTRBuffer(BYTE buffer,
                       unsigned long id,
                       BYTE *data,
                       BYTE dataLen,
                       BYTE type)
{
    BYTE *pBuffer;
    BYTE *pSavedBuffer;
    BYTE i;

    pSavedBuffer = pBuffer;

#if defined(ECAN_CHECKED_BUILD)
    // Make sure that this buffer is configured for RTR handling.
    if ( !((BYTE_VAL*)pBuffer)->bits.b2 )
        return FALSE;
#endif

    /*
     * Make sure that RTR response is not currently being transmitted.
     */
    if ( ((BYTE_VAL*)pBuffer)->bits.b4 )
        return FALSE;

    // Start with zero length.
    *(pBuffer+5) = 0;

    // Save given id into ID registers..
    _CANIDToRegs((BYTE*)(pBuffer+1), id, type);

    // Prepare for data byte access.
    pBuffer += 6;

    // Copy given number of data bytes.
    for ( i = 0; i < dataLen; i++ )
        *pBuffer++ = *data++;

    // At last, update DLC value.
    *(pSavedBuffer + 5) = dataLen;

    return TRUE;
}
#endif


/*********************************************************************
 * Function:        BOOL ECANSendMessage(unsigned long id,
 *                                      BYTE *data,
 *                                      BYTE dataLen,
 *                                      ECAN_TX_MSG_FLAGS msgFlags)
 *
 * Overview:        Use this function to transmit a CAN message.
 *                  This function searches for empty transmit buffer
 *                  and loads it with given messages. Buffer is then
 *                  marked for ready to transmit.
 *
 * PreCondition:    None
 *
 * Input:           id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type as specified in
 *                                msgFlags parameter.
 *                  data        - Data bytes of upto 8 bytes in length
 *                  dataLen     - Data length from 0 thru 8.
 *                                If 0, data may be NULL.
 *                  msgFlags    - One or ECAN_TX_MSG_FLAGS values ORed
 *                                together
 *
 * Output:          TRUE, if an empty buffer was found and loaded with
 *                  given data
 *                  FALSE, if otherwise.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOL ECANSendMessage( unsigned long id,
                     BYTE* data,
                     BYTE dataLen,
                     ECAN_TX_MSG_FLAGS msgFlags)
{
#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    BYTE mode;
    BYTE buffers;
#elif ( ECAN_FUNC_MODE == ECAN_MODE_0 )
    #define buffers 2
#else
    #define buffers 8

#endif

    BYTE i,j;
    BYTE *ptr, *tempPtr;
    BYTE* pb[9];
    BYTE temp;

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || (ECAN_FUNC_MODE != ECAN_MODE_0) )
    BYTE_VAL tempBSEL0;
#endif

    /*
     * Since there are more than one transmit buffers and they are scattered in
     * SFR map, prepare table of all transmit buffers.
     */
    pb[0]=(BYTE*)&TXB0CON;
    pb[1]=(BYTE*)&TXB1CON;
    pb[2]=(BYTE*)&TXB2CON;

    /*
     * Include programmable buffers only if mode 1 or 2 is used.
     */
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || (ECAN_FUNC_MODE != ECAN_MODE_0) )

    pb[3]=(BYTE*)&B0CON;
    pb[4]=(BYTE*)&B1CON;
    pb[5]=(BYTE*)&B2CON;
    pb[6]=(BYTE*)&B3CON;
    pb[7]=(BYTE*)&B4CON;
    pb[8]=(BYTE*)&B5CON;
#endif

#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    mode = ECANCON&0xC0;
    if ( mode == ECAN_MODE_0 )
        buffers = 2;
    else
        buffers = 8;
#endif


    /*
     * Depending on whether only mode 0 is used or not, we would only need to check
     * either 3 or all buffers.
     */

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    for ( i = 0; i < buffers; i++ )
#else



    /*
     * In Mode 1 & 2, programmable buffers must be checked for buffer mode.
     * By remembering BSEL0 (programmable buffer mode type info), we can
     * reduce code required to check invidual buffer type as we scan through
     * all buffers.
     */
    tempBSEL0.Val = BSEL0 >> 1;

    for ( i = 0; i < buffers; i++ )
#endif
    {
        /*
         * Use local poiter to reduce overall code.
         * It will be more efficient to access using pointer instead of index.
         */
        ptr = pb[i];
        tempPtr = ptr;


        /*
         * In Mode 1 & 2, if buffer number is above TXB2, we must also check to
         * see if this buffer is configured for transmit mode.
         */
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )
        if ( i > 2 )
        {
            /*
             * Use previously saved BSEL0 value.
             */
            tempBSEL0.Val >>= 1;

            /*
             * If this is not transmit buffer, continue with next buffer.
             */
            if ( !tempBSEL0.bits.b0 )
                continue;
        }
#endif

        /*
         * Check to see if this buffer is empty by looking at BnCON.TXREQ bit (bit3).
         */
        if ( !(*ptr & 0x08) )
        {
            // Set transmit priority in BnCON register.
            *ptr &= ~ECAN_TX_PRIORITY_BITS;
            *ptr |= msgFlags & ECAN_TX_PRIORITY_BITS;

            // Also save DLC value.
            if ( msgFlags & ECAN_TX_RTR_BIT )
                temp = 0x40 | dataLen;
            else
                temp = dataLen;
            // Use temp to reduce evaluation of *(ptr+5) to only once.
            *(ptr+5) = temp;

            // Set standard or extended message type.
            if ( msgFlags & ECAN_TX_FRAME_BIT )
                temp = ECAN_MSG_XTD;

            else
                temp = ECAN_MSG_STD;

            // And rearrange given id accordingly.
            _CANIDToRegs((BYTE*)(ptr+1), id, temp);

            // Prepare for data byte access.
            ptr += 6;
            // Copy given number of data bytes.
            for ( j = 0 ; j < dataLen; j++ )
                *ptr++ = *data++;

            // If this buffer is configured to automatically handle RTR messages,
            // do not set TXREQ bit.  TXREQ bit will be set whenever matching RTR is received.
            if ( !(*tempPtr & 0x04) )
                *tempPtr |= 0x08;

            return TRUE;
        }
    }

    // There were no empty buffers.
    return FALSE;
}



/*********************************************************************
 * Function:        BOOL ECANReceiveMessage(unsigned long *id,
 *                                  BYTE *data,
 *                                  BYTE *dataLen,
 *                                  ECAN_RX_MSG_FLAGS *msgFlags)
 *
 * Overview:        Use this function to check for full receive buffer
 *                  and extract received data into local buffers.
 *
 * PreCondition:    None
 *
 * Input:           id          - Pointer to buffer that will be
 *                                populated with receive ID.
 *                  data        - Pointer to buffer that will be
 *                                populated with data if there is any
 *                  dataLen     - Pointer to buffer that will be
 *                                populated with count of bytes
 *                                copied in data buffer.
 *                  msgFlags    - Pointer to buffer that will be
 *                                copied with information about
 *                                received message. More than
 *                                one information is ORed together.
 *
 * Output:          TRUE, if a full receive buffer was found and
 *                  given parameters were populated.
 *                  FALSE, if otherwise.
 *
 * Side Effects:    None
 *
 * Note:            If you need to know the filter number that caused
 *                  this message to get accepted, call
 *                  ECANGetFilterHitInfo().
 *
 ********************************************************************/
BOOL ECANReceiveMessage(unsigned long *id,
                       BYTE *data,
                       BYTE *dataLen,
                       ECAN_RX_MSG_FLAGS *msgFlags)
{
#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    BYTE mode;
#endif

    BYTE *ptr, *savedPtr;
    char i;
    BYTE_VAL temp;

    _ECANRxFilterHitInfo.Val = 0;

#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    mode = ECANCON&0xC0;
    if ( mode == ECAN_MODE_0 )
#endif


#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_LIB_MODE_VAL == ECAN_LIB_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )
    {
        // Find which buffer is ready.
        if ( RXB0CON_RXFUL )
        {
            // Clear the received flag.
            PIR3_RXB0IF = 0;

            // Record and forget any previous overflow
            if ( COMSTAT_RXB0OVFL )
            {
                *msgFlags |= ECAN_RX_OVERFLOW;
                COMSTAT_RXB0OVFL = 0;
            }

            _ECANRxFilterHitInfo.bits.b0 = RXB0CON_FILHIT0;

            ptr = (BYTE*)&RXB0CON;

        }
        else if ( RXB1CON_RXFUL )
        {
            // Clear the received flag.
            PIR3_RXB1IF = 0;

            // Record and forget any previous overflow
            if ( COMSTAT_RXB1OVFL )
            {
                *msgFlags |= ECAN_RX_OVERFLOW;
                COMSTAT_RXB1OVFL = 0;
            }

            _ECANRxFilterHitInfo.Val = RXB1CON & 0x07;

            if ( _ECANRxFilterHitInfo.Val < 0x02 )
                *msgFlags |= ECAN_RX_DBL_BUFFERED;


            ptr = (BYTE*)&RXB1CON;
        }
        else
            return FALSE;

        goto _SaveMessage;
    }
#endif

#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    else if ( mode == ECAN_MODE_1 )
#endif

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_1) )

    {
        if ( RXB0CON_RXFUL )
            ptr = (BYTE*)&RXB0CON;

        else if (RXB1CON_RXFUL)
            ptr = (BYTE*)&RXB1CON;

        else if ( (BSEL0_B0TXEN==0) && B0CON_RXFUL )
            ptr = (BYTE*)&B0CON;

        else if ( (BSEL0_B1TXEN==0) && B1CON_RXFUL )
            ptr = (BYTE*)&B1CON;

        else if ( (BSEL0_B2TXEN==0) && B2CON_RXFUL )
            ptr = (BYTE*)&B2CON;

        else if ( (BSEL0_B3TXEN==0) && B3CON_RXFUL )
            ptr = (BYTE*)&B3CON;

        else if ( (BSEL0_B4TXEN==0) && B4CON_RXFUL )
            ptr = (BYTE*)&B4CON;

        else if ( (BSEL0_B5TXEN==0) && B5CON_RXFUL )
            ptr = (BYTE*)&B5CON;

        else
            return FALSE;

        goto _SaveMode12Message;


    }
#endif

#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    else
#endif

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_2) )
    {
        if ( COMSTAT_FIFOEMPTY == 1 )      //if FIFO is NOT empty --> meaning there is a message to be read waiting in Bn or RXBn Buffer
        {
            ptr = (BYTE*)_ECANPointBuffer(CANCON&0x07);          // get the pointer to the proper buffer

            goto _SaveMode12Message;
        }

        return FALSE;
    }
#endif


#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )
_SaveMode12Message:
    _ECANRxFilterHitInfo.Val = *ptr & 0x1f;

    PIR3_RXBnIF = 0;

    if ( COMSTAT_RXBnOVFL )
    {
        *msgFlags |= ECAN_RX_OVERFLOW;
        COMSTAT_RXBnOVFL = 0;
    }
#endif

_SaveMessage:
    savedPtr = ptr;
    *msgFlags = 0;

    // Retrieve message length.
    temp.Val = *(ptr+5);
    *dataLen = temp.Val & 0b00001111;

    // Determine whether this was RTR or not.
    if ( temp.bits.b6 )
        *msgFlags |= ECAN_RX_RTR_FRAME;

    // Retrieve EIDX bytes only if this is extended message
    temp.Val = *(ptr+2);
    if ( temp.bits.b3 )
    {
        *msgFlags |= ECAN_RX_XTD_FRAME;
        temp.Val = ECAN_MSG_XTD;
    }
    else
        temp.Val = ECAN_MSG_STD;

    _RegsToCANID(ptr+1, id, temp.Val);

    // Get message data itself
    ptr += 6;
    temp.Val = *dataLen;
    for ( i = 0; i < temp.Val; i++ )
        *data++ = *ptr++;

    // Record and Clear any previous invalid message bit flag.
    if ( PIR3_IRXIF )
    {
        *msgFlags |= ECAN_RX_INVALID_MSG;
        PIR3_IRXIF = 0;
    }

    // Mark that this buffer is read and empty.
    *savedPtr &= 0x7f;

#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    // Workaround for Rev A1 silicon
    if ( mode == ECAN_MODE_2 )
        COMSTAT_FIFOEMPTY = 0;

#elif ( ECAN_FUNC_MODE_VAL == ECAN_MODE_2 )
    COMSTAT_FIFOEMPTY = 0;
#endif

    return TRUE;
}

/*********************************************************************
 * Function:        void ECANSetOperationMode(ECAN_OP_MODE mode)
 *
 * Overview:        Use this function to switch ECAN module into
 *                  given operational mode.
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type ECAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a blocking call.  It will not return until
 *                  requested mode is set.
 ********************************************************************/
void ECANSetOperationMode(ECAN_OP_MODE mode)
{
    CANCON &= 0x1F;                         // clear previous mode
    CANCON |= mode;                         // set new mode

    while( ECANGetOperationMode() != mode ); // Wait till desired mode is set.
}


/*********************************************************************
 * Function:        void _CANIDToRegs(BYTE* ptr,
 *                                    unsigned long val,
 *                                    ECAN_MSG_TYPE type)
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
typedef union _CAN_MESSAGE_ID
{
    unsigned long ID;

    struct
    {
        struct
        {
            unsigned SIDL:3;        // SIDL<5:7>
            unsigned SIDH:5;        // SIDH<0:4>
        } BYTE1;
        struct
        {
            unsigned SIDHU:3;       // SIDH<5:7>
            unsigned EIDL_LN:5;     // EIDL<0:4>
        } BYTE2;
        struct
        {
            unsigned EIDL_UN:3;     // EIDL<5:7>
            unsigned EIDH_LN:5;     // EIDH<0:4>
        } BYTE3;
        struct
        {
            unsigned EIDH_UN:3;     // EIDH<5:7>
            unsigned EIDHU:2;       // SIDL<0:1>
            unsigned :3;
        } BYTE4;
    } ID_VALS;

    // This is to allow individual byte access within message id.
    struct
    {
        BYTE BYTE_1;
        BYTE BYTE_2;
        BYTE BYTE_3;
        BYTE BYTE_4;
    } BYTES;
} CAN_MESSAGE_ID;

void _CANIDToRegs(BYTE* ptr,
                  unsigned long val,
                  BYTE type)
{
    CAN_MESSAGE_ID *Value;

    Value =  (CAN_MESSAGE_ID*)&val;

    if ( type == ECAN_MSG_STD )
    {
        // Standard Identifier
        *ptr = Value->BYTES.BYTE_1 >> 3;        // Copy SID<7:3> to SIDH<4:0>
        *ptr |= (Value->BYTES.BYTE_2 << 5);     // Copy SID<10:8> to SIDH<7:5>
        ptr++;                                  // Point to SIDL
        *ptr = Value->BYTES.BYTE_1 << 5;        // Copy SID<2:0> to SIDL<7:5>
    }
    else
    {
        // Extended Identifier
        *ptr = Value->BYTES.BYTE_3 >> 5;        // Copy EID<23:21> to SIDH<2:0>
        *ptr |= Value->BYTES.BYTE_4 << 3;       // Copy EID<28:24> to SIDH<7:3>
        ptr++;                                  // Point to SIDL
        *ptr = (Value->BYTES.BYTE_3 << 3) & 0xE0; // Copy EID<20:18> to SIDL<7:5>
                                                // mask out EID<17:16> bits
        *ptr |= 0b00001000;                     // Set EXIDEN bit  to SIDL<3>
        *ptr |= Value->BYTES.BYTE_3 & 0x03;     // Copy EID<17:16> to SIDL<1:0>
        ptr++;                                  // Point to EIDH
        *ptr = Value->BYTES.BYTE_2;             // Copy EID<15:8> to EIDH<7:0>
        ptr++;                                  // Point to EIDL
        *ptr = Value->BYTES.BYTE_1;             // Copy EID<7:0> to EIDL<7:0>
    }
}

/*********************************************************************
 * Function:        void _RegsToCANID(BYTE *ptr,
 *                                   unsigned long *val,
 *                                   BYTE type)
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
void _RegsToCANID( BYTE* ptr,
                   unsigned long *val,
                   BYTE type )
{
    CAN_MESSAGE_ID *Value;

    Value = (CAN_MESSAGE_ID*)val;

    if ( type == ECAN_MSG_STD )
    {
        // Standard Identifier
        Value->BYTES.BYTE_1 = (*ptr << 3);      // Copy SIDH<4:0> to SID<7:3>
        Value->BYTES.BYTE_2 = *ptr >> 5;        // Copy SIDH<7:5> to SID<10:8>
        ptr++;                                  // Point to SIDL
        Value->BYTES.BYTE_1 |= (*ptr >> 5);     // Copy SIDL<7:6> to SID<2:0>
        Value->BYTES.BYTE_3 = 0x00;
        Value->BYTES.BYTE_4 = 0x00;
    }
    else
    {
        // Extended Identifier
        Value->BYTES.BYTE_3 = (*ptr << 5);      // Copy SIDH<2:0> to EID<23:21>
        Value->BYTES.BYTE_4 = (*ptr >> 3);      // Copy SIDH<7:3> to EID<29:25>
        ptr++;                          // Point to SIDL
        Value->BYTES.BYTE_3 |= (*ptr & 0x03);   // Copy SIDH<1:0> to EID<17:16>
        // Bug-Fix NKR 11/20/00
        Value->BYTES.BYTE_3 |= ((*ptr & 0xe0) >> 3);  // Copy SIDL<7:6> to EID<20:18>
        ptr++;                          // Point to EIDH
        Value->BYTES.BYTE_2 = *ptr;             // Copy EIDH<15:8> to EID<15:8>
        ptr++;                                  // Point to EIDL
        Value->BYTES.BYTE_1 = *ptr;             // Copy EIDH<7:0> to EID<7:0>
    }
}


/*********************************************************************
 * Function:        BYTE* _ECANPointBuffer(BYTE b)
 *
 * PreCondition:    None
 *
 * Input:           buffer number
 *
 * Output:          RXBnCON or RnCON address
 *
 * Side Effects:    None
 *
 * Overview:        This functions returns a pointer to the
 *                  beginning of a reception buffer
 *
 * Note:
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_FIXED) && (ECAN_FUNC_MODE_VAL == ECAN_MODE_2) )
static BYTE* _ECANPointBuffer(BYTE b)
{
    BYTE* pt;

    switch(b)
    {
    case 0:
        pt=(BYTE*)&RXB0CON;     // return pointer to correct buffer
        break;
    case 1:
        pt=(BYTE*)&RXB1CON;
        break;
    case 2:
        pt=(BYTE*)&B0CON;
        break;
    case 3:
        pt=(BYTE*)&B1CON;
        break;
    case 4:
        pt=(BYTE*)&B2CON;
        break;
    case 5:
        pt=(BYTE*)&B3CON;
        break;
    case 6:
        pt=(BYTE*)&B4CON;
        break;
    default:              //case 7:
        pt=(BYTE*)&B5CON;
        break;
    }
    return (pt);
}
#endif



