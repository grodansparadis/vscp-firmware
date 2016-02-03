/*********************************************************************
 *
 *              ECAN C Library Source Code
 *
 *********************************************************************
 * FileName:        ECAN.h
 * Dependencies:    ECAN.def
 * Processor:       Any PIC18 with ECAN module
 * Complier:        Microchip C 2.10.06 or higher
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
 * Caio Gubel           5/5/03  Version 1.0 - Initial Release
 * Nilesh R.            7/22/03 Improved
 *********************************************************************/

#ifndef ECAN_H          // To avoid duplicate inclusion
#define ECAN_H

#include   "ecan.def"

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
    #include <p18cxxx.h>    // p18cxxx.h must have current processor
                            // defined.
#endif

#if defined(HITECH_C18)
    #include <pic18.h>
#endif




/*********************************************************************
 *
 * General purpose typedef's
 *
 * Remove these definations if they are already defined in one of your
 * application files.
 ********************************************************************/
typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef unsigned char BYTE;

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;


#define ECAN_LIB_MODE_FIXED     0
#define ECAN_LIB_MODE_RUN_TIME  1

#define ECAN_TX2_MODE_DISABLE   0
#define ECAN_TX2_MODE_ENABLE    1

    #define ECAN_INIT_NORMAL            0x00
    #define ECAN_INIT_LOOPBACK          0x40
    #define ECAN_INIT_CONFIGURATION     0x80
    #define ECAN_INIT_DISABLE           0x20
    #define ECAN_INIT_LISTEN_ONLY       0X60

#define ECAN_RXFn_ENABLE        1
#define ECAN_RXFn_DISABLE       0


// Enable auto RTR code when at least one buffer is configured in
// auto RTR mode.
#if (ECAN_FUNC_MODE_VAL != MODE_0 )
    #if ( (ECAN_B0_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B1_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B2_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B3_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B4_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) || \
          (ECAN_B5_AUTORTR_MODE == ECAN_AUTORTR_MODE_ENABLE) )
        #define ECAN_ENABLE_AUTO_RTR
    #endif
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
void ECANInitialize(void);


/*********************************************************************
 *
 * ECAN_TX_MSG_FLAGS
 *
 * This enumeration values define flags related to transmission of a
 * CAN message.  There could be more than one this flag
 * ORed together to form multiple flags.
 *
 *********************************************************************/
typedef enum _ECAN_TX_MSG_FLAGS
 {
    ECAN_TX_PRIORITY_BITS    = 0b00000011,
    ECAN_TX_PRIORITY_0       = 0b00000000,
    ECAN_TX_PRIORITY_1       = 0b00000001,
    ECAN_TX_PRIORITY_2       = 0b00000010,
    ECAN_TX_PRIORITY_3       = 0b00000011,

    ECAN_TX_FRAME_BIT        = 0b00100000,
    ECAN_TX_STD_FRAME        = 0b00000000,
    ECAN_TX_XTD_FRAME        = 0b00100000,

    ECAN_TX_RTR_BIT          = 0b01000000,
    ECAN_TX_NO_RTR_FRAME     = 0b00000000,
    ECAN_TX_RTR_FRAME        = 0b01000000
} ECAN_TX_MSG_FLAGS;


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
                     BYTE *data,
                     BYTE dataLen,
                     ECAN_TX_MSG_FLAGS msgFlags);

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
                           BYTE type);
#endif


/*********************************************************************
 *
 * ECAN_RX_MSG_FLAGS
 *
 * This enumeration values define flags related to reception of a ECAN
 * message.  There could be more than one this flag
 * ORed together to form multiple flags.
 * If a particular bit is set, corresponding meaning is TRUE or else
 * it will be FALSE.
 *
 * e.g.
 *      if (msgFlag & ECAN_RX_OVERFLOW)
 *      {
 *          // Receiver overflow has occured.
 *          ...
 *      }
 *
 ********************************************************************/
typedef enum _ECAN_RX_MSG_FLAGS
{
    ECAN_RX_OVERFLOW     = 0b00001000,
    ECAN_RX_INVALID_MSG  = 0b00010000,
    ECAN_RX_XTD_FRAME    = 0b00100000,
    ECAN_RX_RTR_FRAME    = 0b01000000,
    ECAN_RX_DBL_BUFFERED = 0b10000000
} ECAN_RX_MSG_FLAGS;

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
BOOL ECANReceiveMessage( unsigned long* id,
                        BYTE *Data,
                        BYTE *DataLen,
                        ECAN_RX_MSG_FLAGS *MsgFlags);


/*********************************************************************
 * Macro:           BYTE ECANGetFilterHitInfo(void)
 *
 * Overview:        Use this macro to extract filter number that
 *                  caused a message to get accepted.
 *                  You must call this macro immediately after calling
 *                  ECANReceiveMessage function.
 *
 * PreCondition:    ECANReceiveMessage is called and returned TRUE
 *
 * Input:           None
 *
 * Output:          Number of filter that caused acceptance.
 *                  0 means RXF0, 1 means RXF1 and so on.
 *
 * Side Effects:    None
 *
 * Note:            This macro will return filter hit information
 *                  for previously received message only.
 *
 ********************************************************************/
#define ECANGetFilterHitInfo()          (_ECANRxFilterHitInfo.Val)
    extern BYTE_VAL _ECANRxFilterHitInfo;


/*********************************************************************
 *
 * ECAN_OP_MODE
 *
 * This enumeration values define codes related to ECAN module
 * operation mode. ECANSetOperationMode() routine requires this code.
 * These values must be used by itself
 * i.e. it cannot be ORed to form * multiple values.
 *
 ********************************************************************/
typedef enum _ECAN_OP_MODE
{
    ECAN_OP_MODE_BITS    = 0xe0,   // Use this to access opmode bits
    ECAN_OP_MODE_NORMAL  = 0x00,
    ECAN_OP_MODE_SLEEP   = 0x20,
    ECAN_OP_MODE_LOOP    = 0x40,
    ECAN_OP_MODE_LISTEN  = 0x60,
    ECAN_OP_MODE_CONFIG  = 0x80
} ECAN_OP_MODE;

/*********************************************************************
 * Function:        void ECANSetOperationMode(ECAN_OP_MODE mode)
 *
 * Overview:        Use this function to switch ECAN module into
 *                  given operational mode.
 *                  You may not need to call this function if
 *                  your application does not require run-time
 *                  changes in ECAN mode.
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
void ECANSetOperationMode(ECAN_OP_MODE mode);

/*********************************************************************
 * Macro:           void ECANSetOperationModeNoWait(ECAN_OP_MODE mode)
 *
 * Overview:        Use this macro to request operation mode but
 *                  do not want macro to wait for it get accepted.
 *                  You must use ECANGetOperationMode() to esnure
 *                  requested operation mode is accepted before
 *                  performing operation critical steps.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes
 *                  in ECAN mode.
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum ECAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a non-blocking call.
 *                  It does not verify that
 *                  ECAN module is switched to requested mode or not.
 *                  Caller must use ECANGetOperationMode() to verify
 *                  correct operation mode before performing mode
 *                  specific operation.
 *
 ********************************************************************/
#define ECANSetOperationModeNoWait(mode)     CANCON = mode

/*********************************************************************
 * Macro:           ECAN_OP_MODE ECANGetOperationMode()
 *
 * Overview:        Use this macro to obtain current operation mode
 *                  ECAN module.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes.
 * PreCondition:
 *
 * Input:
 *
 * Output:          Current operational mode of ECAN module is returned
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANGetOperationMode() (CANCON & ECAN_OP_MODE_BITS)


/*********************************************************************
 * Macro:           ECANSetFunctionalMode(mode)
 *
 * Overview:        Use this macro to set functional mode of ECAN.
 *                  You may not need to call this macro if
 *                  your application will not change mode at run-time.
 *
 * PreCondition:    ECAN must be in configuration mode
 *                  and ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME
 *
 * Input:           mode        - New mode
 *                                Allowable values are
 *                                  ECAN_MODE_0 for Mode 0
 *                                  ECAN_MODE_1 for Mode 1
 *                                  ECAN_MODE_2 for Mode 2
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#if ( ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME )
    #define ECANSetFunctionalMode(mode)      \
            ECANCON_MDSEL1 = mode >> 7; \
            ECANCON_MDSEL0 = mode >> 6;
#endif

    #define ECAN_MODE_0     0x00
    #define ECAN_MODE_1     0x40
    #define ECAN_MODE_2     0x80


/*********************************************************************
 * Macro:           BYTE ECANGetFunctionalMode()
 *
 * Overview:        Use this macro to get functional mode of ECAN.
 *                  You may not need to call this macro if
 *                  your application will not change mode at run-time.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          mode    -   ECAN_MODE_0
 *                              ECAN_MODE_1
 *                              ECAN_MODE_2
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANGetFunctionalMode()      (ECANCON & 0b11000000)


/*********************************************************************
 * Macro:           ECANSetBusSampleMode(mode)
 *
 * Overview:        Use this macro to set CAN bus sample mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change
 *                  to sampling mode.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - Bus sample mode
 *                     Allowable values are
 *                      ECAN_BUS_SAMPLE_MODE_ONCE to sample once
 *                      ECAN_BUS_SAMPLE_THRICE to sample thrice.
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetBusSampleMode(mode)   BRGCON2_SAM = mode

    #define ECAN_BUS_SAMPLE_MODE_ONCE       0
    #define ECAN_BUS_SAMPLE_MODE_THRICE     1



/*********************************************************************
 * Macro:           ECANSetWakeupMode(mode)
 *
 * Overview:        Use this macro to set CAN bus activity bus mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time changes.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - CAN Bus activity wakeup mode
 *                     Allowable values are
 *                      ECAN_WAKEUP_MODE_ENABLE to enable bus wakeup
 *                      ECAN_WAKEUP_MODE_DISABLE to disable bus wakeup
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetWakeupMode(mode)      BRGCON3_WAKDIS = mode

    #define ECAN_WAKEUP_MODE_ENABLE         0
    #define ECAN_WAKEUP_MODE_DISABLE        1


/*********************************************************************
 * Macro:           ECANSetFilterMode(mode)
 *
 * Overview:        Use this macro to CAN wakeup low pass filter mode.
 *                  You may not need to call this macro if your application
 *                  does not require run-time changes.
 *
 * PreCondition:    ECAN must be in configuration mode
 *
 * Input:           mode        - CAN wakeup filter mode
 *                     Allowable values are
 *                      ECAN_FILTER_MODE_DISABLE to not use wakeup filter
 *                      ECAN_FILTER_MODE_ENABLE to use wakeup filter
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetFilterMode(mode)      BRGCON3_WAKFIL = mode

    #define ECAN_FILTER_MODE_DISABLE        0
    #define ECAN_FILTER_MODE_ENABLE         1

/*********************************************************************
 * Macro:           ECANSetTxDriveMode(mode)
 *
 * Overview:        Use this macro to set CANTX pin drive options.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - CANTX drive mode when trnasmitting
 *                                recessive bit.
 *                     Allowable values are
 *                      ECAN_TXDRIVE_MODE_TRISTATE to drive tri-state
 *                      ECAN_TXDRIVE_MODE_VDD to drive to Vdd
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetTxDriveMode(mode)     CIOCON_ENDRHI = mode

    #define ECAN_TXDRIVE_MODE_TRISTATE  0
    #define ECAN_TXDRIVE_MODE_VDD       1


/*********************************************************************
 * Macro:           ECANSetCANTX2Source(source)
 *
 * Overview:        Use this macro to set CANTX2 pin source.
 *                  This macro automatically enables CANTX2 pin.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           source      - CANTX2 source
 *                     Allowable values are
 *                      ECAN_TX2_CAN_CLOCK to output CAN clock on TX2
 *                      ECAN_TX2_CAN_DATA to output complete CAN
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetCANTX2Mode(mode)         CIOCON_TX2SRC = mode << 7; \
                                        CIOCON_TX2EN = 1
    #define ECAN_TX2_SOURCE_COMP    0
    #define ECAN_TX2_SOURCE_CLOCK   1


/*********************************************************************
 * Macro:           ECANDisableCANTX2()
 *
 * Overview:        Use this macro to disable CANTX2 pin.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANDisableCANTX2()              CIOCON_TX2EN = 0


/*********************************************************************
 * Macro:           ECANSetCaptureMode(mode)
 *
 * Overview:        Use this macro to set CAN CAPTURE mode.  This is
 *                  mainly used to timestamp incoming CAN messages.
 *                  You would also need to setup and enable CCP module
 *                  to obtain timeestamp information.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - Capture mode
 *                                Allowable values are
 *                                  ECAN_CAPTURE_MODE_DISABLE
 *                                  ECAN_CAPTURE_MODE_ENABLE
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetCaptureMode(mode)     CIOCON_CANCAP = mode

    #define ECAN_CAPTURE_MODE_DISABLE   0
    #define ECAN_CAPTURE_MODE_ENABLE    1

/*********************************************************************
 * Macro:           ECANSetPHSEG2Mode(mode)
 *
 * Overview:        Use this macro to set PHSEG2 mode.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    None
 *
 * Input:           mode        - PHSEG2 Mode
 *                                Allowable values are
 *                    ECAN_PHSEG2_MODE_AUTOMATIC
 *                      - Max. of PHSEG1 of IPT, whichever is greater
 *                    ECAN_PHSEG2_MODE_PROGRAMMABLE
 *                      - Freely programmable
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#define ECANSetPHSEG2Mode(mode)      BRGCON2_SEG2PHTS = mode

    #define ECAN_PHSEG2_MODE_AUTOMATIC      0
    #define ECAN_PHSEG2_MODE_PROGRAMMABLE   1


/*********************************************************************
 * Macro:           ECANSetB0AutoRTRMode(mode)
 *                  ECANSetB1AutoRTRMode(mode)
 *                  ECANSetB2AutoRTRMode(mode)
 *                  ECANSetB3AutoRTRMode(mode)
 *                  ECANSetB4AutoRTRMode(mode)
 *                  ECANSetB5AutoRTRMode(mode)
 *
 * Overview:        Use these macros to set automatic RTR handling
 *                  mode for given programmable buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME
 *                  ECAN_FUNC_MODE_VAL != ECAN_MODE_0
 *
 * Input:           mode        - AutoRTR mode
 *                  Allowable values are
 *                    ECAN_AUTORTR_MODE_DISABLE
 *                      - To disable automatic RTR handling
 *                    ECAN_AUTORTR_MODE_ENABLE
 *                      - To enable automatic RTR handling
 *
 * Output:          None
 *
 * Side Effects:
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetB0AutoRTRMode(mode)      B0CON_RTREN = mode;     \
                                            BSEL0_B0TXEN = 1
    #define ECANSetB1AutoRTRMode(mode)      B1CON_RTREN = mode;     \
                                            BSEL0_B1TXEN = 1
    #define ECANSetB2AutoRTRMode(mode)      B2CON_RTREN = mode;     \
                                            BSEL0_B2TXEN = 1
    #define ECANSetB3AutoRTRMode(mode)      B3CON_RTREN = mode;     \
                                            BSEL0_B3TXEN = 1
    #define ECANSetB4AutoRTRMode(mode)      B4CON_RTREN = mode;     \
                                            BSEL0_B4TXEN = 1
    #define ECANSetB5AutoRTRMode(mode)      B5CON_RTREN = mode;     \
                                            BSEL0_B5TXEN = 1
#endif

    #define ECAN_AUTORTR_MODE_DISABLE   0
    #define ECAN_AUTORTR_MODE_ENABLE    1



/*********************************************************************
 * Macro:           ECANSetRXMnValue(val, type)
 *
 * Overview:        Use these macros to set specific mask value.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN must be in Configuration mode
 *
 * Input:           val     - Value to be set
 *                      Actual value would depend on type.
 *                      If type is Standard, value will be 11-bit long
 *                      If type is Extended, value will be 29-bit long
 *                  type    - Mask type
 *                      Allowable values are
 *                          ECAN_MSG_STD for Standard type
 *                          ECAN_MSG_XTD for Extended type
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Overview:        This function sets mask value for given mask number.
 *                  Type of mask, whether standard or extended is defined
 *                  by type parameter.
 *
 * Note:            The MASK_RXF15 is only available in enhanced and
 *                  FIFO modes
 ********************************************************************/
#define ECANSetRXM0Value(val, type)       \
                    _CANIDToRegs((BYTE*)&RXM0SIDH, val, type)
#define ECANSetRXM1Value(val, type)       \
                    _CANIDToRegs((BYTE*)&RXM1SIDH, val, type)




/*********************************************************************
 * Macro:           ECANSetRXFnValue(val, type)
 *
 * Overview:        Use these macros to set receive filter values.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 * PreCondition:    ECAN must be in Configuration mode
 *                  To set RXF6-RXF15 values,
 *                  ECAN_LIB_MODE_VAL = ECAN_LIB_MODE_RUN_TIME OR
 *                  ECAN_FUNC_MODE != ECAN_MODE_0
 *
 * Input:           val         - Value to be set
 *                  type        - Type of filter
 *                                Allowable values are
 *                                  ECAN_MSG_STD if this is Standard filter
 *                                  ECAN_MSG_XTD if this is Extended filter
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Use this macro to assign value to a receive filter.
 *
 * Note:            There are total of sixteen macros - one for each
 *                  receive filter. e.g. for RXF0 use ECANSetRXF0Value()
 *                  for RXF2 use ECANSetRXF2Value()
 ********************************************************************/
#define ECANSetRXF0Value(val, type)       \
    RXFCON0_RXF0EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF0SIDH, val, type)

#define ECANSetRXF1Value(val, type)       \
    RXFCON0_RXF1EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF1SIDH, val, type);

#define ECANSetRXF2Value(val, type)       \
    RXFCON0_RXF2EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF2SIDH, val, type);

#define ECANSetRXF3Value(val, type)       \
    RXFCON0_RXF3EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF3SIDH, val, type);

#define ECANSetRXF4Value(val, type)       \
    RXFCON0_RXF4EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF4SIDH, val, type);

#define ECANSetRXF5Value(val, type)       \
    RXFCON0_RXF5EN = 1;         \
    _CANIDToRegs((BYTE*)&RXF5SIDH, val, type);

#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetRXF6Value(val, type)       \
        RXFCON0_RXF6EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF6SIDH, val, type);

    #define ECANSetRXF7Value(val, type)       \
        RXFCON0_RXF7EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF7SIDH, val, type);

    #define ECANSetRXF8Value(val, type)       \
        RXFCON1_RXF8EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF8SIDH, val, type);

    #define ECANSetRXF9Value(val, type)       \
        RXFCON1_RXF9EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF9SIDH, val, type);

    #define ECANSetRXF10Value(val, type)       \
        RXFCON1_RXF10EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF10SIDH, val, type);

    #define ECANSetRXF11Value(val, type)       \
        RXFCON1_RXF11EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF11SIDH, val, type);

    #define ECANSetRXF12Value(val, type)       \
        RXFCON1_RXF12EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF12SIDH, val, type);

    #define ECANSetRXF13Value(val, type)       \
        RXFCON1_RXF13EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF13SIDH, val, type);

    #define ECANSetRXF14Value(val, type)       \
        RXFCON1_RXF14EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF14SIDH, val, type);

    #define ECANSetRXF15Value(val, type)       \
        RXFCON1_RXF15EN = 1;         \
        _CANIDToRegs((BYTE*)&RXF15SIDH, val, type);
#endif

    #define ECAN_MSG_STD    0
    #define ECAN_MSG_XTD    1


/*********************************************************************
 * Macro:           ECANSetBnTxRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure a programmable buffer (B0-B5)
 *                  as either transmit or receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN_LIB_MODE = ECAN_LIB_MODE_RUN_TIME OR
 *                  ECAN_FUNC_MODE_VAL != ECAN_MODE_0
 *
 * Input:           buffer      - Buffer that needs to be setup
 *                                Allowable values are:
 *                                      B0, B1, B2, B3, B4, B5
 *                  mode        - Mode to be set.
 *                                Allowable values are:
 *                                      ECAN_BUFFER_TX, ECAN_BUFFER_RX
 *
 * Output:          None
 *
 * Side Effects:
 *
 * Example:        // Set B0 as Transmit buffer
 *                  ECANSetBnTxRxMode(B0, ECAN_TX)
 *
 *                  // Set B2 as receive buffer
 *                  ECANSetBnTxRxMode(B2, ECAN_RX)
 *
 * Note:            Parameter buffer must be a constant symbol of either
 *                  B0, B1, B2, B3, B4 or B5.
 *                  A variable parameter would result in compiler error.
 *
 *                  e.g. ECANSetBnTxRxMode(myBuffer, ECAN_BUFFER_TX)
 *                       would not compile.
 *
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANSetBnTxRxMode(buffer, mode)          \
                            BSEL0_##buffer##TXEN = mode
#endif

    #define RXB0            0
    #define RXB1            1
    #define B0              2
    #define B1              3
    #define B2              4
    #define B3              5
    #define B4              6
    #define B5              7
    #define TXB0            8
    #define TXB1            9
    #define TXB2            10

    #define ECAN_BUFFER_RX  0
    #define ECAN_BUFFER_TX  1

/*********************************************************************
 * Macro:           ECANSetRXB0DblBuffer(mode)
 *
 * Overview:        Use this macro to configure RXB0 in double buffering mode
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           mode        - Double buffer mode
 *                                Allowable values are
 *                                  ECAN_DBL_BUFFER_ENABLE
 *                                  ECAN_DBL_BUFFER_DISABLE,
 *
 * Output:          None
 *
 * Example:         // Enable Double buffering mode
 *                  ECANSetRXB0DblBuffer(ECAN_DBL_BUFFER_ENABLE)
 *
 *
 * Side Effects:
 *
 * Note:            None
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL == ECAN_MODE_0) )

    #define ECANSetRXB0DblBuffer(mode)       RXB0CON_RXB0DBEN = mode
#endif

    #define ECAN_DBL_BUFFER_MODE_DISABLE    0
    #define ECAN_DBL_BUFFER_MODE_ENABLE     1


/*********************************************************************
 * Macro:           ECANSetRxBnRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure receive mode of a fixed receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer that needs to be configured
 *                                Allowable values are
 *                                  RXB0, RXB1
 *                  mode        - Mode to be setup.
 *                                Allowable values are
 *                                  ECAN_RECEIVE_ALL,
 *                                  ECAN_RECEIVE_STANDARD,
 *                                  ECAN_RECEIVE_EXTENDED,
 *                                  ECAN_RECEIVE_ALL_VALID
 *
 * Output:          None
 *
 * Example:         // Configure RXB0 buffer to receive all valid messages.
 *                  ECANSetRxBnRxMode(RXB0, ECAN_RECEIVE_ALL_VALID)
 *
 *                  // Configure RXB1 buffer to receive only Standard messages.
 *                  ECANSetRxBnRxMode(RXB1, ECAN_RECEIVE_STANDARD)
 *
 * Side Effects:
 *
 * Note:            Parameter buffer must be a constant symbol of either
 *                  RXB0 or RXB1.
 *                  A variable paraemter would result in compiler error.
 *
 *                  e.g. ECANSetRxBnTxRxMode(myBuffer, ECAN_RECEIVE_ALL_VALID)
 *                       would not compiler
 *
 ********************************************************************/
#define ECANSetRxBnRxMode(buffer, mode)      \
        ##buffer##CON_RXM1 = mode >> 1; \
        ##buffer##CON_RXM0 = mode;

    #define ECAN_RECEIVE_ALL_VALID  0
    #define ECAN_RECEIVE_STANDARD   1
    #define ECAN_RECEIVE_EXTENDED   2
    #define ECAN_RECEIVE_ALL        3



/*********************************************************************
 * Macro:           ECANSetBnRxMode(buffer, mode)
 *
 * Overview:        Use this macro to configure receive mode of a programmable
 *                  receive buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    None
 *
 * Input:           buffer      - Buffer that needs to be configured
 *                                Allowable values are
 *                                  B0, B1, B2, B3, B4, B5
 *                  mode        - Mode to be setup.
 *                                Allowable values are
 *                                  ECAN_RECEIVE_ALL,
 *                                  ECAN_RECEIVE_ALL_VALID
 *
 * Output:          None
 *
 * Example:         // Configure B0 buffer to receive all valid messages.
 *                  ECANSetBnRxMode(B0, ECAN_RECEIVE_ALL_VALID)
 *
 *                  // Configure B1 buffer to receive only Standard messages.
 *                  ECANSetBnRxMode(RXB1, ECAN_RECEIVE_STANDARD)
 *
 * Side Effects:
 *
 * Note 1:          Parameter buffer must be a constant symbol of either
 *                  B0, B1, B2, B3, B4 or B5.
 *                  A variable paraemter would result in compiler error.
 *
 *                  e.g. ECANSetBnTxRxMode(myBuffer, ECAN_RECEIVE_ALL_VALID)
 *                       would not compiler
 *
 *      2:          This macro should be used in Mode 1 or 2 only.
 *
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )
    #define ECANSetBnRxMode(buffer, mode)      ##buffer##CON_RXM1 = mode

#endif

/*********************************************************************
 * Macro:           BYTE ECANGetTxErrorCount()
 *
 * Overview:        Use this macro to get current TXERRCNT value
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
#define ECANGetTxErrorCount()            (TXERRCNT)


/*********************************************************************
 * Macro:           BYTE ECANGetRxErrorCount()
 *
 * Overview:        Use this macro to get current RXERRCNT value.
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
#define ECANGetRxErrorCount()            (RXERRCNT)


/*********************************************************************
 * Macro:           BOOL ECANIsBusOff()
 *
 * Overview:        Use this macro to check if CAN module is in
 *                  Bus off condition.
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
#define ECANIsBusOff()                   (COMSTAT_TXB0)


/*********************************************************************
 * Macro:           BOOL ECANIsTxPassive()
 *
 * Overview:        Use this macro to check if CAN module is in
 *                  Tx passive mode.
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
#define ECANIsTxPassive()                (COMSTAT_TXBP)


/*********************************************************************
 * Macro:           BYTE ECANIsRxPassive()
 *
 * Overview:        Use this macro to check if CAN is in Rx passive.
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
#define ECANIsRxPassive()                (COMSTAT_RXBP)


/*********************************************************************
 * Macro:           void ECANAbortAll()
 *
 * Overview:        Use this macro to request abort for all tx buffers.
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
#define ECANAbortAll()                   (CANCON_ABAT = 1)

/*********************************************************************
 * Macro:           BOOL ECANIsAllAborted()
 *
 * Overview:        Use this macro to see if previous ECANAbortAll
 *                  is complete or not.
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: if there are no pending messages.
 *                  FALSE: if there abort is still in progress.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define ECANIsAllAborted()               (CANCON_ABAT)


/*********************************************************************
 * Macro:           ECANLinkRXFnFmToBuffer(RXFnBuffer, RXFmBuffer)
 *                      n = { 0, 2, 4, 6, 8, 10, 12, 14 }
 *                      m = ( 1, 3, 5, 7, 9, 11, 13, 15 }
 *                      m = n + 1
 *
 * Overview:        Use this macro to link RXFn and RXFm to
 *                  any of RXB0, RXB1, B0, B1, B2, B3, B4, B5 buffer.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN must be in Configuration mode
 *                  Given buffer must be configured as receive buffer.
 *                      (See ECANSetRxBnTxRxMode and ECANSetBnTxRxMode)
 *
 *
 * Input:           RXFnBuffer  - Buffer that is to be linked
 *                                with RXFn filter
 *                  RXFmBuffer  - Buffer that is to be linked
 *                                with RXFm filter
 *
 *                                Allowable values for both parameters
 *                                  RXB0, RXB1, B0, B1, B2, B3, B4, B5
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Note:            If possible, call this macro with constant
 *                  parameters to reduce generated code.
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANLinkRXF0F1ToBuffer(RXF0Buffer, RXF1Buffer)     \
                RXFBCON0 = (RXF1Buffer << 4) | RXF0Buffer
    #define ECANLinkRXF2F3ToBuffer(RXF2Buffer, RXF3Buffer)     \
                RXFBCON1 = (RXF2Buffer << 4) | RXF3Buffer
    #define ECANLinkRXF4F5ToBuffer(RXF4Buffer, RXF5Buffer)     \
                RXFBCON2 = (RXF4Buffer << 4) | RXF5Buffer
    #define ECANLinkRXF6F7ToBuffer(RXF6Buffer, RXF7Buffer)     \
                RXFBCON3 = (RXF6Buffer << 4) | RXF7Buffer
    #define ECANLinkRXF8F9ToBuffer(RXF8Buffer, RXF9Buffer)     \
                RXFBCON4 = (RXF8Buffer << 4) | RXF9Buffer
    #define ECANLinkRXF10F11ToBuffer(RXF10Buffer, RXF11Buffer) \
                RXFBCON5 = (RXF10Buffer << 4) | RXF11Buffer
    #define ECANLinkRXF12F13ToBuffer(RXF12Buffer, RXF13Buffer) \
                RXFBCON6 = (RXF12Buffer << 4) | RXF13Buffer
    #define ECANLinkRXF14F15ToBuffer(RXF14Buffer, RXF15Buffer) \
                RXFBCON7 = (RXF14Buffer << 4) | RXF15Buffer
#endif


/*********************************************************************
 * Macro:           ECANLinkRXF0Thru3ToMask(m0, m1, m2, m3)
 *                  ECANLinkRXF4Thru7ToMask(m4, m5, m6, m7)
 *                  ECANLinkRXF8Thru11ToMask(m8, m9, m10, m11, m12)
 *                  ECANLinkRXF12Thru15ToMask(m13, m14, m15, m16)
 *
 * Overview:        Use this macro to link receive filters to masks.
 *                  You may not need to call this macro if your
 *                  application does not require run-time change.
 *
 *
 * PreCondition:    ECAN must be in Configuration mode
 *
 * Input:           mn  - Buffer that is to be linked
 *                                with Mask
 *
 *                                Allowable values for both parameters
 *                                  ECAN_RXM0, ECAN_RXM1, ECAN_RXMF15
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 *
 * Note:            If possible, call this macro with constant
 *                  parameters to reduce generated code.
 ********************************************************************/
#if ( (ECAN_LIB_MODE_VAL == ECAN_LIB_MODE_RUN_TIME) || \
      (ECAN_FUNC_MODE_VAL != ECAN_MODE_0) )

    #define ECANLinkRXF0Thru3ToMask(m0, m1, m2, m3) \
        MSEL0 = m3 << 6 | m2 << 4 | m1 << 2 | m0;
    #define ECANLinkRXF4Thru7ToMask(m4, m5, m6, m7) \
        MSEL1 = m7 << 6 | m6 << 4 | m5 << 2 | m4;
    #define ECANLinkRXF8Thru11ToMask(m8, m9, m10, m11) \
        MSEL2 = m11 << 6 | m10 << 4 | m9 << 2 | m8;
    #define ECANLinkRXF12Thru15ToMask(m12, m13, m14, m15) \
        MSEL2 = m15 << 6 | m14 << 4 | m13 << 2 | m12;
#endif

    #define ECAN_RXM0       0
    #define ECAN_RXM1       1
    #define ECAN_RXMF15     2


/*********************************************************************
 * Macro:           ECANSetBaudRate(sjw,
 *                                 brp,
 *                                 phseg1,
 *                                 phseg2,
 *                                 propseg)
 *
 * Overview:        Use this macro to change ECAN baud rate. Normally
 *                  you would call this macro only, if your application
 *                  needs run-time baud rate changes.
 *
 * PreCondition:    ECAN is in Configuration mode
 *
 * Input:           sjw     - SJW value of 1-4
 *                  brp     - BRP value of 1-64
 *                  phseg1  - PHSEG1 value of 1-8
 *                  phseg2  - PHSEG2 value of 1-8
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Note:            If possible, call this macro with constant values
 *                  to reduce generated code size.
 ********************************************************************/
#define ECANSetBaudRate(sjw, brp, phseg1, phseg2, propseg)           \
        BRGCON1 = ((sjw-1) << 6) | (brp-1);                         \
        BRGCON2 |= (((phseg1-1) << 3) | (propseg-1));               \
        BRGCON3 = phseg2;

/*
 * This is a helper function used by some macros. Normally, you would
 * not need to call this function directly.
 */
void _CANIDToRegs(BYTE* ptr,
                  unsigned long val,
                  BYTE type );


/*
 * Following compile-time logic switches symbols as per compiler
 * being used.  Currently, it supports Microchip C18 and HiTech
 * PICC18 compilers only.
 */
#if defined(MCHP_C18)
    #define COMSTAT_TXB0            COMSTATbits.TXB0
    #define COMSTAT_TXBP            COMSTATbits.TXBP
    #define COMSTAT_RXBP            COMSTATbits.RXBP
    #define COMSTAT_RXBnOVFL        COMSTATbits.RXB1OVFL
    #define COMSTAT_RXB0OVFL        COMSTATbits.RXB0OVFL
    #define COMSTAT_FIFOEMPTY       COMSTATbits.FIFOEMPTY
    #define COMSTAT_RXB1OVFL        COMSTATbits.RXB1OVFL

    #define BRGCON2_SEG2PHTS        BRGCON2bits.SEG2PHTS
    #define BRGCON2_SAM             BRGCON2bits.SAM
    #define BRGCON3_WAKDIS          BRGCON3bits.WAKDIS
    #define BRGCON3_WAKFIL          BRGCON3bits.WAKFIL

    #define CIOCON_ENDRHI           CIOCONbits.ENDRHI
    #define CIOCON_CANCAP           CIOCONbits.CANCAP

    #define PIR3_RXBnIF             PIR3bits.RXB1IF
    #define PIR3_RXB0IF             PIR3bits.RXB0IF
    #define PIR3_RXB1IF             PIR3bits.RXB1IF

    #define PIR3_IRXIF              PIR3bits.IRXIF

    #define CANCON_ABAT             CANCONbits.ABAT
    #define RXB0CON_RXFUL           RXB0CONbits.RXFUL
    #define RXB1CON_RXFUL           RXB1CONbits.RXFUL
    #define RXB0CON_RXB0DBEN        RXB0CONbits.RXBODBEN
    #define TXB0CON_TXREQ           TXB0CONbits.TXREQ
    #define TXB1CON_TXREQ           TXB1CONbits.TXREQ
    #define TXB2CON_TXREQ           TXB2CONbits.TXREQ
    #define RXB0CON_FILHIT0         RXB0CONbits.FILHIT0
    #define RXB0CON_RXM1            RXB0CONbits.RXM1
    #define RXB0CON_RXM0            RXB0CONbits.RXM0


    #define BSEL0_B0TXEN            BSEL0bits.B0TXEN
    #define BSEL0_B1TXEN            BSEL0bits.B1TXEN
    #define BSEL0_B2TXEN            BSEL0bits.B2TXEN
    #define BSEL0_B3TXEN            BSEL0bits.B3TXEN
    #define BSEL0_B4TXEN            BSEL0bits.B4TXEN
    #define BSEL0_B5TXEN            BSEL0bits.B5TXEN

    #define RXFCON0_RXF0EN          RXFCON0bits.RXF0EN
    #define RXFCON0_RXF1EN          RXFCON0bits.RXF1EN
    #define RXFCON0_RXF2EN          RXFCON0bits.RXF2EN
    #define RXFCON0_RXF3EN          RXFCON0bits.RXF3EN
    #define RXFCON0_RXF4EN          RXFCON0bits.RXF4EN
    #define RXFCON0_RXF5EN          RXFCON0bits.RXF5EN
    #define RXFCON0_RXF6EN          RXFCON0bits.RXF6EN
    #define RXFCON0_RXF7EN          RXFCON0bits.RXF7EN

    #define RXFCON1_RXF8EN          RXFCON1bits.RXF8EN
    #define RXFCON1_RXF9EN          RXFCON1bits.RXF9EN
    #define RXFCON1_RXF10EN         RXFCON1bits.RXF10EN
    #define RXFCON1_RXF11EN         RXFCON1bits.RXF11EN
    #define RXFCON1_RXF12EN         RXFCON1bits.RXF12EN
    #define RXFCON1_RXF13EN         RXFCON1bits.RXF13EN
    #define RXFCON1_RXF14EN         RXFCON1bits.RXF14EN
    #define RXFCON1_RXF15EN         RXFCON1bits.RXF15EN


    #define RXFBCON0_F0BP_0         RXFBCON0bits.F0BP_0
    #define RXFBCON0_F0BP_1         RXFBCON0bits.F0BP_1
    #define RXFBCON0_F0BP_2         RXFBCON0bits.F0BP_2
    #define RXFBCON0_F0BP_3         RXFBCON0bits.F0BP_3
    #define RXFBCON0_F1BP_0         RXFBCON0bits.F1BP_0
    #define RXFBCON0_F1BP_1         RXFBCON0bits.F1BP_1
    #define RXFBCON0_F1BP_2         RXFBCON0bits.F1BP_2
    #define RXFBCON0_F1BP_3         RXFBCON0bits.F1BP_3

    #define RXFBCON1_F2BP_0         RXFBCON1bits.F2BP_0
    #define RXFBCON1_F2BP_1         RXFBCON1bits.F2BP_1
    #define RXFBCON1_F2BP_2         RXFBCON1bits.F2BP_2
    #define RXFBCON1_F2BP_3         RXFBCON1bits.F2BP_3
    #define RXFBCON1_F3BP_0         RXFBCON1bits.F3BP_0
    #define RXFBCON1_F3BP_1         RXFBCON1bits.F3BP_1
    #define RXFBCON1_F3BP_2         RXFBCON1bits.F3BP_2
    #define RXFBCON1_F3BP_3         RXFBCON1bits.F3BP_3

    #define RXFBCON2_F4BP_0         RXFBCON2bits.F4BP_0
    #define RXFBCON2_F4BP_1         RXFBCON2bits.F4BP_1
    #define RXFBCON2_F4BP_2         RXFBCON2bits.F4BP_2
    #define RXFBCON2_F4BP_3         RXFBCON2bits.F4BP_3
    #define RXFBCON2_F5BP_0         RXFBCON2bits.F5BP_0
    #define RXFBCON2_F5BP_1         RXFBCON2bits.F5BP_1
    #define RXFBCON2_F5BP_2         RXFBCON2bits.F5BP_2
    #define RXFBCON2_F5BP_3         RXFBCON2bits.F5BP_3

    #define RXFBCON3_F6BP_0         RXFBCON3BITS.F6BP_0
    #define RXFBCON3_F6BP_1         RXFBCON3BITS.F6BP_1
    #define RXFBCON3_F6BP_2         RXFBCON3BITS.F6BP_2
    #define RXFBCON3_F6BP_3         RXFBCON3BITS.F6BP_3
    #define RXFBCON3_F7BP_0         RXFBCON3BITS.F7BP_0
    #define RXFBCON3_F7BP_1         RXFBCON3BITS.F7BP_1
    #define RXFBCON3_F7BP_2         RXFBCON3BITS.F7BP_2
    #define RXFBCON3_F7BP_3         RXFBCON3BITS.F7BP_3

    #define RXFBCON4_F8BP_0         RXFBCON4bits.F8BP_0
    #define RXFBCON4_F8BP_1         RXFBCON4bits.F8BP_1
    #define RXFBCON4_F8BP_2         RXFBCON4bits.F8BP_2
    #define RXFBCON4_F8BP_3         RXFBCON4bits.F8BP_3
    #define RXFBCON4_F9BP_0         RXFBCON4bits.F9BP_0
    #define RXFBCON4_F9BP_1         RXFBCON4bits.F9BP_1
    #define RXFBCON4_F9BP_2         RXFBCON4bits.F9BP_2
    #define RXFBCON4_F9BP_3         RXFBCON4bits.F9BP_3

    #define RXFBCON5_F10BP_0        RXFBCON5bits.F10BP_0
    #define RXFBCON5_F10BP_1        RXFBCON5bits.F10BP_1
    #define RXFBCON5_F10BP_2        RXFBCON5bits.F10BP_2
    #define RXFBCON5_F10BP_3        RXFBCON5bits.F10BP_3
    #define RXFBCON5_F11BP_0        RXFBCON5bits.F11BP_0
    #define RXFBCON5_F11BP_1        RXFBCON5bits.F11BP_1
    #define RXFBCON5_F11BP_2        RXFBCON5bits.F11BP_2
    #define RXFBCON5_F11BP_3        RXFBCON5bits.F11BP_3

    #define RXFBCON6_F12BP_0        RXFBCON6bits.F12BP_0
    #define RXFBCON6_F12BP_1        RXFBCON6bits.F12BP_1
    #define RXFBCON6_F12BP_2        RXFBCON6bits.F12BP_2
    #define RXFBCON6_F12BP_3        RXFBCON6bits.F12BP_3
    #define RXFBCON6_F13BP_0        RXFBCON6bits.F13BP_0
    #define RXFBCON6_F13BP_1        RXFBCON6bits.F13BP_1
    #define RXFBCON6_F13BP_2        RXFBCON6bits.F13BP_2
    #define RXFBCON6_F13BP_3        RXFBCON6bits.F13BP_3

    #define RXFBCON7_F14BP_0        RXFBCON7bits.F14BP_0
    #define RXFBCON7_F14BP_1        RXFBCON7bits.F14BP_1
    #define RXFBCON7_F14BP_2        RXFBCON7bits.F14BP_2
    #define RXFBCON7_F14BP_3        RXFBCON7bits.F14BP_3
    #define RXFBCON7_F15BP_0        RXFBCON7bits.F15BP_0
    #define RXFBCON7_F15BP_1        RXFBCON7bits.F15BP_1
    #define RXFBCON7_F15BP_2        RXFBCON7bits.F15BP_2
    #define RXFBCON7_F15BP_3        RXFBCON7bits.F15BP_3

    #define B0CON_TXREQ             B0CONbits.TXREQ
    #define B1CON_TXREQ             B1CONbits.TXREQ
    #define B2CON_TXREQ             B2CONbits.TXREQ
    #define B3CON_TXREQ             B3CONbits.TXREQ
    #define B4CON_TXREQ             B4CONbits.TXREQ
    #define B5CON_TXREQ             B5CONbits.TXREQ

    #define B0CON_RXM1              B0CONbits.RXM1
    #define B1CON_RXM1              B1CONbits.RXM1
    #define B2CON_RXM1              B2CONbits.RXM1
    #define B3CON_RXM1              B3CONbits.RXM1
    #define B4CON_RXM1              B4CONbits.RXM1
    #define B5CON_RXM1              B5CONbits.RXM1

    #define B0CON_RXFUL             B0CONbits.RXFUL
    #define B1CON_RXFUL             B1CONbits.RXFUL
    #define B2CON_RXFUL             B2CONbits.RXFUL
    #define B3CON_RXFUL             B3CONbits.RXFUL
    #define B4CON_RXFUL             B4CONbits.RXFUL
    #define B5CON_RXFUL             B5CONbits.RXFUL

    #define RXM0SIDL_EXIDEN         RXM0SIDLbits.EXIDEN
    #define RXM1SIDL_EXIDEN         RXM1SIDLbits.EXIDEN

    #define ECANCON_MDSEL1          ECANCONbits.MDSEL1
    #define ECANCON_MDSEL0          ECANCONbits.MDSEL0

#endif

#if defined(HITECH_C18)
    #define COMSTAT_TXB0            TXB0
    #define COMSTAT_TXBP            TXBP
    #define COMSTAT_RXBP            RXBP
    #define CANCON_ABAT             ABAT
    #define RXB0CON_RXFUL           RXB0FUL
    #define TXB0CON_TXREQ           TXB0REQ
    #define RXB0CON_RXM1            RXB0M1
    #define RXB0CON_RXM0            RXB0M0

    #define BSEL0_B0TXEN            B0TXEN      // configure buffer in Transmit=1 or receive mode=0
    #define BSEL0_B1TXEN            B1TXEN
    #define BSEL0_B2TXEN            B2TXEN
    #define BSEL0_B3TXEN            B3TXEN
    #define BSEL0_B4TXEN            B4TXEN
    #define BSEL0_B5TXEN            B5TXEN



    #define RXFCON0_RXF0EN          RXF0EN
    #define RXFCON0_RXF1EN          RXF1EN
    #define RXFCON0_RXF2EN          RXF2EN
    #define RXFCON0_RXF3EN          RXF3EN
    #define RXFCON0_RXF4EN          RXF4EN
    #define RXFCON0_RXF5EN          RXF5EN
    #define RXFCON0_RXF6EN          RXF6EN
    #define RXFCON0_RXF7EN          RXF7EN

    #define RXFCON1_RXF8EN          XF8EN
    #define RXFCON1_RXF9EN          XF9EN
    #define RXFCON1_RXF10EN         RXF10EN
    #define RXFCON1_RXF11EN         RXF11EN
    #define RXFCON1_RXF12EN         RXF12EN
    #define RXFCON1_RXF13EN         RXF13EN
    #define RXFCON1_RXF14EN         RXF14EN
    #define RXFCON1_RXF15EN         RXF15EN


    #define RXFBCON0_F0BP_0         F0BP_01
    #define RXFBCON0_F0BP_1         F0BP_1
    #define RXFBCON0_F0BP_2         F0BP_2
    #define RXFBCON0_F0BP_3         F0BP_3
    #define RXFBCON0_F1BP_0         F1BP_0
    #define RXFBCON0_F1BP_1         F1BP_1
    #define RXFBCON0_F1BP_2         F1BP_2
    #define RXFBCON0_F1BP_3         F1BP_3

    #define RXFBCON1_F2BP_0         F2BP_01
    #define RXFBCON1_F2BP_1         F2BP_1
    #define RXFBCON1_F2BP_2         F2BP_2
    #define RXFBCON1_F2BP_3         F2BP_3
    #define RXFBCON1_F3BP_0         F3BP_0
    #define RXFBCON1_F3BP_1         F3BP_1
    #define RXFBCON1_F3BP_2         F3BP_2
    #define RXFBCON1_F3BP_3         F3BP_3

    #define RXFBCON2_F4BP_0         F4BP_01
    #define RXFBCON2_F4BP_1         F4BP_1
    #define RXFBCON2_F4BP_2         F4BP_2
    #define RXFBCON2_F4BP_3         F4BP_3
    #define RXFBCON2_F5BP_0         F5BP_0
    #define RXFBCON2_F5BP_1         F5BP_1
    #define RXFBCON2_F5BP_2         F5BP_2
    #define RXFBCON2_F5BP_3         F5BP_3

    #define RXFBCON3_F6BP_0         F6BP_01
    #define RXFBCON3_F6BP_1         F6BP_1
    #define RXFBCON3_F6BP_2         F6BP_2
    #define RXFBCON3_F6BP_3         F6BP_3
    #define RXFBCON3_F7BP_0         F7BP_0
    #define RXFBCON3_F7BP_1         F7BP_1
    #define RXFBCON3_F7BP_2         F7BP_2
    #define RXFBCON3_F7BP_3         F7BP_3

    #define RXFBCON4_F8BP_0         F8BP_01
    #define RXFBCON4_F8BP_1         F8BP_1
    #define RXFBCON4_F8BP_2         F8BP_2
    #define RXFBCON4_F8BP_3         F8BP_3
    #define RXFBCON4_F9BP_0         F9BP_0
    #define RXFBCON4_F9BP_1         F9BP_1
    #define RXFBCON4_F9BP_2         F9BP_2
    #define RXFBCON4_F9BP_3         F9BP_3

    #define RXFBCON5_F10BP_0        F10BP_01
    #define RXFBCON5_F10BP_1        F10BP_1
    #define RXFBCON5_F10BP_2        F10BP_2
    #define RXFBCON5_F10BP_3        F10BP_3
    #define RXFBCON5_F11BP_0        F11BP_0
    #define RXFBCON5_F11BP_1        F11BP_1
    #define RXFBCON5_F11BP_2        F11BP_2
    #define RXFBCON5_F11BP_3        F11BP_3

    #define RXFBCON6_F12BP_0        F12BP_01
    #define RXFBCON6_F12BP_1        F12BP_1
    #define RXFBCON6_F12BP_2        F12BP_2
    #define RXFBCON6_F12BP_3        F12BP_3
    #define RXFBCON6_F13BP_0        F13BP_0
    #define RXFBCON6_F13BP_1        F13BP_1
    #define RXFBCON6_F13BP_2        F13BP_2
    #define RXFBCON6_F13BP_3        F13BP_3

    #define RXFBCON7_F14BP_0        F14BP_01
    #define RXFBCON7_F14BP_1        F14BP_1
    #define RXFBCON7_F14BP_2        F14BP_2
    #define RXFBCON7_F14BP_3        F14BP_3
    #define RXFBCON7_F15BP_0        F15BP_0
    #define RXFBCON7_F15BP_1        F15BP_1
    #define RXFBCON7_F15BP_2        F15BP_2
    #define RXFBCON7_F15BP_3        F15BP_3

    #define B0CON_TXREQ             B0TXREQ
    #define B1CON_TXREQ             B1TXREQ
    #define B2CON_TXREQ             B2TXREQ
    #define B3CON_TXREQ             B3TXREQ
    #define B4CON_TXREQ             B4TXREQ
    #define B5CON_TXREQ             B5TXREQ

    #define B0CON_RXFUL             B0RXFUL
    #define B1CON_RXFUL             B1RXFUL
    #define B2CON_RXFUL             B2RXFUL
    #define B3CON_RXFUL             B3RXFUL
    #define B4CON_RXFUL             B4RXFUL
    #define B5CON_RXFUL             B5RXFUL

    #define B0CON_RXM1              B0RXM1
    #define B1CON_RXM1              B1RXM1
    #define B2CON_RXM1              B2RXM1
    #define B3CON_RXM1              B3RXM1
    #define B4CON_RXM1              B4RXM1
    #define B5CON_RXM1              B5RXM1

    /*
     * Following are special defs to overcome compiler problem
     * at the time of this re-write.
     * Set following line to "#if 0" after verifiying correct
     * compiler behavior.
     */
    #if 1
        static struct
        {
            unsigned : 7;
            unsigned RXFUL : 1;
        } RXB1CONbits @ 0xf50;
        #define RXB1CON_RXFUL       RXB1CONbits.RXFUL

        static struct
        {
            unsigned TXPRI0:1;
            unsigned TXPRI1:1;
            unsigned :1;
            unsigned TXREQ:1;
            unsigned TXERR:1;
            unsigned TXLARB:1;
            unsigned TXABT:1;
        } TXB1CONbits @ 0xf30;
        #define TXB1CON_TXREQ       TXB1CONbits.TXREQ

        static struct
        {
            unsigned TXPRI0:1;
            unsigned TXPRI1:1;
            unsigned :1;
            unsigned TXREQ:1;
            unsigned TXERR:1;
            unsigned TXLARB:1;
            unsigned TXABT:1;
        } TXB2CONbits @ 0xf20;
        #define TXB2CON_TXREQ       TXB2CONbits.TXREQ
    #else
        #define RXB1CON_RXFUL       RXB1RXFUL
        #define TXB1CON_TXREQ       TXB1REQ
        #define TXB2CON_TXREQ       TXB2REQ
    #endif

    #define RXM0SIDL_EXIDEN         RXM0EXIDM
    #define RXM1SIDL_EXIDEN         RXM1EXIDEN

    #define ECANCON_MDSEL1          MDSEL1
    #define ECANCON_MDSEL0          MDSEL0

    #define COMSTAT_FIFOEMPTY       FIFOEMPTY
    #define COMSTAT_RXBnOVFL        RXB1OVFL
    #define COMSTAT_RXB0OVFL        RXB0OVFL
    #define COMSTAT_RXB1OVFL        RXB1OVFL

    #define BRGCON2_SEG2PHTS        SEG2PHT
    #define BRGCON2_SAM             SAM
    #define BRGCON3_WAKDIS          WAKDIS
    #define BRGCON3_WAKFIL          WAKFIL

    #define CIOCON_ENDRHI           ENDRHI
    #define CIOCON_CANCAP           CANCAP

    #define PIR3_RXBnIF             RXB1IF
    #define PIR3_IRXIF              IRXIF
    #define PIR3_RXB0IF             RXB0IF
    #define PIR3_RXB1IF             RXB1IF

    #define RXB0CON_FILHIT0         RXB0FILHIT0



    #define RXB0CON_RXB0DBEN        RXBODBEN
    static volatile near bit        RXBODBEN   @ ((unsigned)&RXB0CON*8)+2;


#endif





#endif

