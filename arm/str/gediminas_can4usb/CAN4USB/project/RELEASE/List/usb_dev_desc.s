///////////////////////////////////////////////////////////////////////////////
//                                                                            /
// IAR ARM ANSI C/C++ Compiler V5.12.0.50667/W32        18/Apr/2008  14:00:42 /
// Copyright 1999-2007 IAR Systems. All rights reserved.                      /
//                                                                            /
//    Cpu mode     =  thumb                                                   /
//    Endian       =  little                                                  /
//    Source file  =  D:\development\gediminas\CAN4USB\project\app\usb_dev_de /
//                    sc.c                                                    /
//    Command line =  D:\development\gediminas\CAN4USB\project\app\usb_dev_de /
//                    sc.c -lC D:\development\gediminas\CAN4USB\project\RELEA /
//                    SE\List\ -lA D:\development\gediminas\CAN4USB\project\R /
//                    ELEASE\List\ -o D:\development\gediminas\CAN4USB\projec /
//                    t\RELEASE\Obj\ --no_cse --no_unroll --no_inline         /
//                    --no_code_motion --no_tbaa --no_clustering              /
//                    --no_scheduling --debug --endian little --cpu           /
//                    ARM7TDMI-S -e --fpu None --dlib_config "D:\Program\IAR  /
//                    Systems\Embedded Workbench                              /
//                    5.0\ARM\INC\DLib_Config_Normal.h" -I                    /
//                    D:\development\gediminas\CAN4USB\project\ -I            /
//                    D:\development\gediminas\CAN4USB\project\app\ -I        /
//                    D:\development\gediminas\CAN4USB\project\board\ -I      /
//                    D:\development\gediminas\CAN4USB\project\module\ -I     /
//                    D:\development\gediminas\CAN4USB\project\STR75xLibrary\ /
//                    library\inc\ -I j:\common\ -I "D:\Program\IAR           /
//                    Systems\Embedded Workbench 5.0\ARM\INC\" --interwork    /
//                    --cpu_mode thumb -On                                    /
//    List file    =  D:\development\gediminas\CAN4USB\project\RELEASE\List\u /
//                    sb_dev_desc.s                                           /
//                                                                            /
//                                                                            /
///////////////////////////////////////////////////////////////////////////////

        NAME usb_dev_desc

        RTMODEL "__dlib_file_descriptor", "0"

        PUBLIC ManufacturerStrLan1
        PUBLIC ManufacturerStrLan2
        PUBLIC ProductStrLan1
        PUBLIC ProductStrLan2
        PUBLIC SerialNumberStrLan1
        PUBLIC UsbLanguages1Strings
        PUBLIC UsbLanguages2Strings
        PUBLIC UsbLanguagesStr
        PUBLIC UsbStandardConfigurationDescriptor
        PUBLIC UsbStandardDeviceDescriptorStr
        PUBLIC UsbString
// D:\development\gediminas\CAN4USB\project\app\usb_dev_desc.c
//    1 /*************************************************************************
//    2  *
//    3  *    Used with ICCARM and AARM.
//    4  *
//    5  *    (c) Copyright IAR Systems 2006
//    6  *
//    7  *    File name   : usb_desc.c
//    8  *    Description : usb descriptors module
//    9  *
//   10  *    $Revision: 15135 $
//   11 **************************************************************************/
//   12 #include "usb_dev_desc.h"
//   13 
//   14 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//   15 const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr =
UsbStandardDeviceDescriptorStr:
        DATA
        DC8 18, 1
        DC16 272
        DC8 2, 0, 0, 8
        DC16 65535, 8, 0
        DC8 1, 2, 3, 1, 0, 0
//   16 {
//   17   sizeof(UsbStandardDeviceDescriptorStr_t),   // bLength
//   18   UsbDescriptorDevice,                        // bDescriptorType
//   19   0x110,                                      // bcdUSB
//   20   UsbDeviceClassCommunications,               // bDeviceClass
//   21   0,                                          // bDeviceSubClass
//   22   0,                                          // bDeviceProtocol
//   23   Ep0MaxSize,                                 // bMaxPacketSize0
//   24   0xFFFF,                                     // idVendor
//   25   0x0008,                                     // idProduct
//   26   0x0000,                                     // bcdDevice
//   27   iManufacturerStr,                           // iManufacturer
//   28   iProductStr,                                // iProduct
//   29   iSerialNumberStr,                           // iSerialNumber
//   30   1,                                          // bNumConfigurations
//   31 };
//   32 
//   33 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//   34 const Int8U UsbStandardConfigurationDescriptor[] =
UsbStandardConfigurationDescriptor:
        DATA
        DC8 9, 2, 67, 0, 2, 1, 0, 128, 50, 9, 4, 0, 0, 1, 2, 2, 0, 0, 5, 36, 0
        DC8 16, 1, 5, 36, 6, 0, 1, 5, 36, 1, 0, 1, 4, 36, 2, 6, 7, 5, 129, 3
        DC8 16, 0, 1, 9, 4, 1, 0, 2, 10, 0, 0, 0, 7, 5, 2, 2, 64, 0, 0, 7, 5
        DC8 130, 2, 64, 0, 0, 0
//   35 {
//   36 // Configuration Descriptor
//   37   // bLength
//   38   sizeof(UsbStandardConfigurationDescriptor_t),
//   39   // bDescriptorType
//   40   UsbDescriptorConfiguration,
//   41   // wTotalLength
//   42   _2BL(sizeof(UsbStandardConfigurationDescriptor_t)+
//   43   (2 * sizeof(UsbStandardInterfaceDescriptor_t))+
//   44    sizeof(CDC_HeaderFuncDescriptor_t)+
//   45    sizeof(CDC_CallManagementFuncDescriptor_t)+
//   46    sizeof(CDC_AbstrCtrlFuncDescriptor_t)+
//   47   (sizeof(CDC_UnionFuncDescriptor_t)+1)+
//   48   (3 * sizeof(UsbStandardEpDescriptor_t))),
//   49   // bNumInterfaces
//   50   2,
//   51   // bConfigurationValue
//   52   1,
//   53   // iConfiguration
//   54   0,
//   55   // bmAttributes
//   56   UsbConfigurationCommmonAttr,
//   57   // bMaxPower
//   58   UsbConfigPower_mA(100),
//   59 
//   60 // Interface Descriptors AbstractControlModel
//   61   // bLength
//   62   sizeof(UsbStandardInterfaceDescriptor_t),
//   63   // bDescriptorType
//   64   UsbDescriptorInterface,
//   65   // bInterfaceNumber
//   66   CDC_CTRL_INTERFACE_IND,
//   67   // bAlternateSetting
//   68   0,
//   69   // bNumEndpoints
//   70   1,
//   71   // bInterfaceClass
//   72   CommInterfaceClassCode,
//   73   // bInterfaceSubClass
//   74   AbstractControlModel,
//   75   // bInterfaceProtocol
//   76   0,
//   77   // iInterface
//   78   0,
//   79 
//   80 // Header Functional Descriptor
//   81   // bFunctionLength
//   82   sizeof(CDC_HeaderFuncDescriptor_t),
//   83   // bDescriptorType
//   84   CS_INTERFACE,
//   85   // bDescriptorSubtype
//   86   Header,
//   87   // bcdCDC
//   88   _2BL(0x110),
//   89 
//   90 // Union Functional Descriptor
//   91   // bFunctionLength
//   92   sizeof(CDC_UnionFuncDescriptor_t)+1,
//   93   // bDescriptorType
//   94   CS_INTERFACE,
//   95   // bDescriptorSubtype
//   96   Union,
//   97   // bMasterInterface
//   98   CDC_CTRL_INTERFACE_IND,
//   99   // bDataInterface
//  100   CDC_DATA_INTERFACE_IND,
//  101 
//  102 //Call Management Functional Descriptor
//  103   // bFunctionLength
//  104   sizeof(CDC_CallManagementFuncDescriptor_t),
//  105   // bDescriptorType
//  106   CS_INTERFACE,
//  107   // bDescriptorSubtype
//  108   CallManagement,
//  109   // bmCapabilities
//  110   0,
//  111   // bDataInterface
//  112   1,
//  113 
//  114 //ACM Functional Descriptor
//  115   // bFunctionLength
//  116   sizeof(CDC_AbstrCtrlFuncDescriptor_t),
//  117   // bDescriptorType
//  118   CS_INTERFACE,
//  119   // bDescriptorSubtype
//  120   AbstractControlManagement,
//  121   // bmCapabilities
//  122   (2 | (CDC_DEVICE_SUPPORT_BREAK?4:0)),
//  123 
//  124 // Endpoint Descriptors ReportEp
//  125   // bLength
//  126   sizeof(UsbStandardEpDescriptor_t),
//  127   // bDescriptorType
//  128   UsbDescriptorEp,
//  129   // bEndpointAddress
//  130   UsbEpIn(ReportEp>>1),
//  131   // bmAttributes
//  132   UsbEpTransferInterrupt,
//  133   // wMaxPacketSize
//  134   _2BL(ReportEpMaxSize),
//  135   // bInterval
//  136   0x1,
//  137 
//  138 // Interface Descriptors Data Class Interface
//  139   // bLength
//  140   sizeof(UsbStandardInterfaceDescriptor_t),
//  141   // bDescriptorType
//  142   UsbDescriptorInterface,
//  143   // bInterfaceNumber
//  144   CDC_DATA_INTERFACE_IND,
//  145   // bAlternateSetting
//  146   0,
//  147   // bNumEndpoints
//  148   2,
//  149   // bInterfaceClass
//  150   DataInterfaceClassCodes,
//  151   // bInterfaceSubClass
//  152   0,
//  153   // bInterfaceProtocol
//  154   0,
//  155   // iInterface
//  156   0,
//  157 
//  158 // Endpoint Descriptors CommOutEp
//  159   // bLength
//  160   sizeof(UsbStandardEpDescriptor_t),
//  161   // bDescriptorType
//  162   UsbDescriptorEp,
//  163   // bEndpointAddress
//  164   UsbEpOut(CommOutEp>>1),
//  165   // bmAttributes
//  166   UsbEpTransferBulk,
//  167   // wMaxPacketSize
//  168   _2BL(CommOutEpMaxSize),
//  169   // bInterval
//  170   0x00,
//  171 
//  172 // Endpoint Descriptors CommInEp
//  173   // bLength
//  174   sizeof(UsbStandardEpDescriptor_t),
//  175   // bDescriptorType
//  176   UsbDescriptorEp,
//  177   // bEndpointAddress
//  178   UsbEpIn(CommInEp>>1),
//  179   // bmAttributes
//  180   UsbEpTransferBulk,
//  181   // wMaxPacketSize
//  182   _2BL(CommInEpMaxSize),
//  183   // bInterval
//  184   0x00,
//  185   0,
//  186 };
//  187 
//  188 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  189 const Int8U UsbLanguagesStr [] =
UsbLanguagesStr:
        DATA
        DC8 6, 3, 9, 4, 2, 4, 0, 0
//  190 {
//  191   // Length of LanguagesStr + 2
//  192   6,
//  193   // Descriptor Type
//  194   UsbDescriptorString,
//  195   // Languages ID
//  196   // Languages1 ID English
//  197   0x09,0x04,
//  198   // Languages2 ID Bulgarian
//  199   0x02,0x04
//  200 };
//  201 
//  202 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  203 const Int8U ManufacturerStrLan1 [] =
ManufacturerStrLan1:
        DATA
        DC8 24, 3, 73, 0, 65, 0, 82, 0, 32, 0, 83, 0, 121, 0, 115, 0, 116, 0
        DC8 101, 0, 109, 0, 115, 0
//  204 {
//  205   24, // length
//  206   UsbDescriptorString,  // Descriptor
//  207   'I',0,'A',0,'R',0,' ',0,'S',0,'y',0,'s',0,'t',0,'e',0,'m',0,'s',0, // Str
//  208 };
//  209 
//  210 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  211 const Int8U ManufacturerStrLan2 [] =
ManufacturerStrLan2:
        DATA
        DC8 24, 3, 200, 0, 192, 0, 208, 0, 32, 0, 209, 0, 232, 0, 241, 0, 242
        DC8 0, 229, 0, 236, 0, 232, 0
//  212 {
//  213   24, //length
//  214   UsbDescriptorString, // Descriptor
//  215   'È',0,'À',0,'Ð',0,' ',0,'Ñ',0,'è',0,'ñ',0,'ò',0,'å',0,'ì',0,'è',0, // Str
//  216 };
//  217 
//  218 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  219 const Int8U ProductStrLan1 [] =
ProductStrLan1:
        DATA
        DC8 24, 3, 73, 0, 65, 0, 82, 0, 32, 0, 83, 0, 84, 0, 82, 0, 55, 0, 53
        DC8 0, 48, 0, 70, 0
//  220 {
//  221   24, //length
//  222   UsbDescriptorString, // Descriptor
//  223   'I',0,'A',0,'R',0,' ',0,'S',0,'T',0,'R',0,'7',0,'5',0,'0',0,'F',0, // Str
//  224 };
//  225 
//  226 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  227 const Int8U ProductStrLan2 [] =
ProductStrLan2:
        DATA
        DC8 24, 3, 200, 0, 192, 0, 208, 0, 32, 0, 209, 0, 210, 0, 208, 0, 55, 0
        DC8 53, 0, 48, 0, 212, 0
//  228 {
//  229   24, //length
//  230   UsbDescriptorString, // Descriptor
//  231   'È',0,'À',0,'Ð',0,' ',0,'Ñ',0,'Ò',0,'Ð',0,'7',0,'5',0,'0',0,'Ô',0, // Str
//  232 };
//  233 
//  234 #pragma data_alignment=4

        SECTION `.rodata`:CONST:NOROOT(2)
//  235 const Int8U SerialNumberStrLan1 [] =
SerialNumberStrLan1:
        DATA
        DC8 18, 3, 48, 0, 48, 0, 48, 0, 48, 0, 48, 0, 48, 0, 48, 0, 49, 0, 0, 0
//  236 {
//  237   18, //length
//  238   UsbDescriptorString, // Descriptor
//  239   '0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'1',0, // Str
//  240 };
//  241 

        SECTION `.rodata`:CONST:NOROOT(2)
//  242 const Int8U * const UsbLanguages1Strings[] =
UsbLanguages1Strings:
        DATA
        DC32 ManufacturerStrLan1, ProductStrLan1, SerialNumberStrLan1, 0H
//  243 {
//  244   // iManufacturerStr
//  245   ManufacturerStrLan1,
//  246   // iProductStr
//  247   ProductStrLan1,
//  248   // iSerialNumberStr
//  249   SerialNumberStrLan1,
//  250   // Terminator
//  251   NULL
//  252 };
//  253 

        SECTION `.rodata`:CONST:NOROOT(2)
//  254 const Int8U * const UsbLanguages2Strings[] =
UsbLanguages2Strings:
        DATA
        DC32 ManufacturerStrLan2, ProductStrLan2, SerialNumberStrLan1, 0H
//  255 {
//  256   // iManufacturerStr
//  257   ManufacturerStrLan2,
//  258   // iProductStr
//  259   ProductStrLan2,
//  260   // iSerialNumberStr
//  261   SerialNumberStrLan1,
//  262   // Terminator
//  263   NULL
//  264 };
//  265 

        SECTION `.rodata`:CONST:NOROOT(2)
//  266 const Int8U * const * const UsbString[] =
UsbString:
        DATA
        DC32 UsbLanguages1Strings, UsbLanguages2Strings

        END
//  267 {
//  268   UsbLanguages1Strings,
//  269   UsbLanguages2Strings,
//  270 };
//  271 
// 
// 252 bytes in section .rodata
// 
// 252 bytes of CONST memory
//
//Errors: none
//Warnings: none
