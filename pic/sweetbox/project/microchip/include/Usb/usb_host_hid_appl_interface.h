/******************************************************************************

  USB Host HID Application Interface Header File

Description:
    This is the header file for a USB HID Host application interface file.

    This file should be included with usb_host.h to provide the USB hardware
    interface. It must be included after the application-specific usb_config.h
    file and after the USB Embedded Host header file usb_host.h, as definitions
    in those files are required for proper compilation.

Acronyms/abbreviations used by this class:
    * HID - Human Interface Device

This file provides interface between HID application and driver. This file
also provides interface functions of HID parser. These interfaces must be used
to access the data structures populated by HID parser. These data structures
hold crucial information required by application to understand the nature of
device connected on the bus. The file also contains interface functions to
send/recieve Reports to/from device.

Summary:
    This is the header file for a USB HID Host application interface file.
*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************

* FileName:        usb_host_hid_appl_interface.h
* Dependencies:    None
* Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

Author          Date    Comments
--------------------------------------------------------------------------------
ADG          9-Apr-2008 First release
*******************************************************************************/
//DOM-IGNORE-END

//DOM-IGNORE-BEGIN
#ifndef _USB_HOST_HID_APPL_INTERFACE_H_ /* usb_host_hid_appl_interface.h */
#define _USB_HOST_HID_APPL_INTERFACE_H_
//DOM-IGNORE-END

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "usb_config.h"
#include "usb_host.h"
#include "usb_host_hid.h"
#include "usb_host_hid_parser.h"


// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
//******************************************************************************
// Section: Data Structures
//******************************************************************************

#if((HID_MAX_DATA_FIELD_SIZE >= 1) && (HID_MAX_DATA_FIELD_SIZE <= 8))  /* Maximum size of data field within a report */
    typedef unsigned char    HID_USER_DATA_SIZE;
#elif((HID_MAX_DATA_FIELD_SIZE >= 9) && (HID_MAX_DATA_FIELD_SIZE <= 16))
    typedef unsigned int     HID_USER_DATA_SIZE;
#elif((HID_MAX_DATA_FIELD_SIZE >= 17) && (HID_MAX_DATA_FIELD_SIZE <= 32))
    typedef unsigned long    HID_USER_DATA_SIZE;
#else
    typedef unsigned char    HID_USER_DATA_SIZE;
#endif


//******************************************************************************
/* HID Data Details

This structure defines the objects used by the application to access reqquired
report. Application must use parser interface functions to fill these details.
e.g. USBHostHID_ApiFindValue
*/
typedef struct _HID_DATA_DETAILS
{
    WORD reportLength;                // reportLength - the expected length of the parent report.
    WORD reportID;                    // reportID - report ID - the first byte of the parent report.
    BYTE bitOffset;                   // BitOffset - bit offset within the report.
    BYTE bitLength;                   // bitlength - length of the data in bits.
    BYTE count;                       // count - what's left of the message after this data.
    BYTE signExtend;                  // extend - sign extend the data.
    BYTE interfaceNum;                // interfaceNum - informs HID layer about interface number.
}   HID_DATA_DETAILS;
// Note: One would ordinarily get these values from a parser
//       find. (e.g. USBHostHID_ApiFindValue)
//


// *****************************************************************************
// Prototypes for Application interface funtions for usb_host_hid.c
// *****************************************************************************


/****************************************************************************
  Function:
    BOOL USBHostHID_APIEventHandler( BYTE address, USB_HID_EVENT event, 
                                     void *data, DWORD size )

  Description:
    This function is called when various events occur in the
    USB HID Host layer.

  Precondition:
    The device has been initialized.

  Parameters:
    BYTE address    -   Address of the device
    USB_EVENT event -   Event that has occurred
    void *data      -   Pointer to data pertinent to the event
    DWORD size      -   Size of the data

  Return Values:
    TRUE    -   Event was handled
    FALSE   -   Event was not handled

  Remarks:
    None
***************************************************************************/
BOOL USBHostHID_APIEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size );


/****************************************************************************
  Function:
    BYTE* USBHostHID_GetCurrentReportInfo()

  Description:
    This function returns a pointer to the current report info structure.

  Precondition:
    None

  Parameters:
    None

  Returns:
    BYTE *  - Pointer to the report Info structure.

  Remarks:
    None
  ***************************************************************************/

#define USBHostHID_GetCurrentReportInfo() (&deviceRptInfo)

/****************************************************************************
  Function:
    BYTE* USBHostHID_GetItemListPointers()

  Description:
    This function returns a pointer to list of item pointers storedin a
    structure.

  Precondition:
    None

  Parameters:
    None

  Returns:
    BYTE *  - Pointer to list of item pointers structure.

  Remarks:
    None
  ***************************************************************************/
#define USBHostHID_GetItemListPointers() (&itemListPtrs)

/****************************************************************************
  Function:
    BOOL USBHostHID_APIInitialize( BYTE address, DWORD flags )

  Description:
    This function is called when a USB HID device is being 
    enumerated.

  Precondition:
    None

  Parameters:
    BYTE address    -   Address of the new device
    DWORD flags     -   Initialization flags


  Return Values:
    TRUE    -   We can support the device.
    FALSE   -   We cannot support the device.

  Remarks:
    None
***************************************************************************/
BOOL USBHostHID_APIInitialize( BYTE address, DWORD flags );

// *****************************************************************************
// Prototypes for interface funtions for Application layer
// *****************************************************************************

/*******************************************************************************
  Function:
    BOOL USBHostHID_ApiDeviceDetect( void )

  Description:
    This function determines if a HID device is attached
    and ready to use.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE   -  HID present and ready
    FALSE  -  HID not present or not ready

  Remarks:
    Since this will often be called in a loop while waiting for
    a device, we'll make sure the tasks are executed.
*******************************************************************************/
BOOL USBHostHID_ApiDeviceDetect( void );

/*******************************************************************************
  Function:
    BOOL USBHostHID_ApiImportData(BYTE *report, WORD reportLength, 
                     HID_USER_DATA_SIZE *buffer,HID_DATA_DETAILS *pDataDetails)
  Description:
    This function can be used by application to extract data from the input 
    reports. On receiving the input report from the device application can call
    the function with required inputs 'HID_DATA_DETAILS'.

  Precondition:
    None

  Parameters:
    BYTE *report                    - Input report received from device
    WORD reportLength               - Length of input report report
    HID_USER_DATA_SIZE *buffer      - Buffer into which data needs to be
                                      populated
    HID_DATA_DETAILS *pDataDetails  - data details extracted from report
                                      descriptor
  Return Values:
    TRUE    - If the required data is retrieved from the report
    FALSE   - If required data is not found.

  Remarks:
    None
*******************************************************************************/
BOOL USBHostHID_ApiImportData(BYTE *report,WORD reportLength,HID_USER_DATA_SIZE *buffer, HID_DATA_DETAILS *pDataDetails);;

/*******************************************************************************
  Function:
    BYTE USBHostHID_ApiGetReport(BYTE reportid, BYTE interfaceNum, BYTE size,
                                BYTE* data)

  Description:
    This function is called by application to receive Input report from device

  Precondition:
    None

  Parameters:
    BYTE reportid           - Report ID of the requested report
    BYTE interfaceNum       - Interface number of the device 
    BYTE size               - Byte size of the data buffer
    BYTE *data              - Pointer to the data buffer

  Return Values:
    USB_SUCCESS                 - Request started successfully
    USB_HID_DEVICE_NOT_FOUND    - No device with specified address
    USB_HID_DEVICE_BUSY         - Device not in proper state for
                                  performing a transfer

  Remarks:
    Device must be connected, host application is responsible for scheduling 
    messages at required rate.
*******************************************************************************/
BYTE USBHostHID_ApiGetReport(WORD reportid, BYTE interfaceNum, BYTE size,BYTE* data);

/*******************************************************************************
  Function:
    BYTE USBHostHID_ApiSendReport(BYTE reportid, BYTE interfaceNum, BYTE size,
                                BYTE* data)

  Description:
    This function is called by application to send Output report to device.

  Precondition:
    None

  Parameters:
    BYTE reportid           - Report ID of the requested report
    BYTE interfaceNum       - Interface number of the device
    BYTE size               - Byte size of the data buffer
    BYTE *data              - Pointer to the data buffer

  Return Values:
    USB_SUCCESS                 - Request started successfully
    USB_HID_DEVICE_NOT_FOUND    - No device with specified address
    USB_HID_DEVICE_BUSY         - Device not in proper state for
                                  performing a transfer

  Remarks:
    Device must be connected, host application is responsible for scheduling
    messages at required rate.
*******************************************************************************/
BYTE USBHostHID_ApiSendReport(WORD reportid, BYTE interfaceNum, BYTE size,BYTE* data);

/****************************************************************************
  Function:
    BOOL USBHostHID_ApiResetDevice( void  )

  Summary:
    This function resets the HID device.

  Description:
    This function resets the HID device attached on the bus.  It is called if
    an operation returns an error or the application can call it in case of
    error.

  Precondition:
    None

  Parameters:
    None - None

  Return Values:
    USB_SUCCESS                 - Reset successful
    USB_HID_DEVICE_NOT_FOUND    - No device with specified address
    USB_ILLEGAL_REQUEST         - Device is in an illegal USB state
                                  for reset

  Remarks:
    None
  ***************************************************************************/
BYTE USBHostHID_ApiResetDevice( void  );

/*******************************************************************************
  Function:
    BYTE USBHostHID_ApiGetCurrentInterfaceNum(void)

  Description:
    This function reurns the interface number of the cuurent report descriptor
    parsed. This function must be called to fill data interface detail data
    structure and passed as parameter when requesinf for report transfers.

  Precondition:
    None

  Parameters:
    BYTE *errorCode     - Error code from last transfer
    DWORD *byteCount    - Number of bytes transferred

  Return Values:
    TRUE    - Transfer is complete, errorCode is valid
    FALSE   - Transfer is not complete, errorCode is not valid

  Remarks:
    None
*******************************************************************************/
BYTE USBHostHID_ApiGetCurrentInterfaceNum(void);

/*******************************************************************************
  Function:
    BOOL USBHostHID_ApiTransferIsComplete(BYTE *errorCodeDriver, 
                                          DWORD *byteCount )

  Summary:
    This function indicates whether or not the last transfer is complete.

  Description:
    This function indicates whether or not the last transfer is complete.
    If the functions returns TRUE, the returned byte count and error
    code are valid.

  Precondition:
    None

  Parameters:
    BYTE *errorCode     - Error code from last transfer
    DWORD *byteCount    - Number of bytes transferred

  Return Values:
    TRUE    - Transfer is complete, errorCode is valid
    FALSE   - Transfer is not complete, errorCode is not valid

  Remarks:
    None
*******************************************************************************/
BOOL USBHostHID_ApiTransferIsComplete(BYTE* errorCodeDriver, BYTE* byteCount );

/*******************************************************************************
  Function:
    BOOL USBHostHID_ApiFindBit(WORD usagePage,WORD usage,HIDReportTypeEnum type,
                          BYTE* Report_ID, BYTE* Report_Length, BYTE* Start_Bit)

  Description:
    This function is used to locate a specific button or indicator.
    Once the report descriptor is parsed by the HID layer without any error,
    data from the report descriptor is stored in pre defined dat structures.
    This function traverses these data structure and exract data required
    by application

  Precondition:
    None

  Parameters:
    WORD usagePage         - usage page supported by application
    WORD usage             - usage supported by application
    HIDReportTypeEnum type - report type Input/Output for the particular
                             usage
    BYTE* Report_ID        - returns the report ID of the required usage
    BYTE* Report_Length    - returns the report length of the required usage
    BYTE* Start_Bit        - returns  the start bit of the usage in a
                             particular report

  Return Values:
    TRUE    - If the required usage is located in the report descriptor
    FALSE   - If the application required usage is not supported by the 
              device(i.e report descriptor).

  Remarks:
    Application event handler with event 'EVENT_HID_RPT_DESC_PARSED' is called.
    Application is suppose to fill in data details in structure 'HID_DATA_DETAILS'.
    This function can be used to the get the details of the required usages.
*******************************************************************************/
BOOL USBHostHID_ApiFindBit(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length, BYTE* Start_Bit);

/*******************************************************************************
  Function:
    BOOL USBHostHID_ApiFindValue(WORD usagePage,WORD usage,
                HIDReportTypeEnum type,BYTE* Report_ID,BYTE* Report_Length,BYTE*
                Start_Bit, BYTE* Bit_Length)

  Description:
    Find a specific Usage Value. Once the report descriptor is parsed by the HID
    layer without any error, data from the report descriptor is stored in
    pre defined dat structures. This function traverses these data structure and
    exract data required by application.

  Precondition:
    None

  Parameters:
    WORD usagePage         - usage page supported by application
    WORD usage             - usage supported by application
    HIDReportTypeEnum type - report type Input/Output for the particular
                             usage
    BYTE* Report_ID        - returns the report ID of the required usage
    BYTE* Report_Length    - returns the report length of the required usage
    BYTE* Start_Bit        - returns  the start bit of the usage in a
                             particular report
    BYTE* Bit_Length       - returns size of requested usage type data in bits

  Return Values:
    TRUE    - If the required usage is located in the report descriptor
    FALSE   - If the application required usage is not supported by the 
              device(i.e report descriptor).

  Remarks:
    Application event handler with event 'EVENT_HID_RPT_DESC_PARSED' is called.
    Application is suppose to fill in data details structure 'HID_DATA_DETAILS'
    This function can be used to the get the details of the required usages.
*******************************************************************************/
BOOL USBHostHID_ApiFindValue(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length,BYTE* Start_Bit, BYTE* Bit_Length);


#endif
