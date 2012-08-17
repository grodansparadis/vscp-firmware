;**********************************************************************
; *   CAN MCP2510 definitions file                                    *
; *********************************************************************
; *   Filename:       MCP2510.inc                                     *
; *   Date:           02 Month 1999                                   *
; *   File Version:   0.90 (Beta)                                     *
; *********************************************************************
; *   Functions:                                                      *
; *               Header file                                         *
; *********************************************************************
; *   Revision History:                                               *
; *********************************************************************
;
;
;**********************************************************************
; *                     Register Definitions                          *
; *********************************************************************

; Configuration Registers *
CANSTAT         EQU    0x0E
CANCTRL         EQU    0x0F
BFPCTRL         EQU    0x0C
TEC             EQU    0x1C
REC             EQU    0x1D
CNF3            EQU    0x28
CNF2            EQU    0x29
CNF1            EQU    0x2A
CANINTE         EQU    0x2B
CANINTF         EQU    0x2C
EFLG            EQU    0x2D
TXRTSCTRL       EQU    0x0D

; Receive Filters *
RXF0SIDH        EQU    0x00
RXF0SIDL        EQU    0x01
RXF0EID8        EQU    0x02
RXF0EID0        EQU    0x03
RXF1SIDH        EQU    0x04
RXF1SIDL        EQU    0x05
RXF1EID8        EQU    0x06
RXF1EID0        EQU    0x07
RXF2SIDH        EQU    0x08
RXF2SIDL        EQU    0x09
RXF2EID8        EQU    0x0A
RXF2EID0        EQU    0x0B
RXF3SIDH        EQU    0x10
RXF3SIDL        EQU    0x11
RXF3EID8        EQU    0x12
RXF3EID0        EQU    0x13
RXF4SIDH        EQU    0x14
RXF4SIDL        EQU    0x15
RXF4EID8        EQU    0x16
RXF4EID0        EQU    0x17
RXF5SIDH        EQU    0x18
RXF5SIDL        EQU    0x19
RXF5EID8        EQU    0x1A
RXF5EID0        EQU    0x1B

; Receive Masks *
RXM0SIDH        EQU    0x20
RXM0SIDL        EQU    0x21
RXM0EID8        EQU    0x22
RXM0EID0        EQU    0x23
RXM1SIDH        EQU    0x24
RXM1SIDL        EQU    0x25
RXM1EID8        EQU    0x26
RXM1EID0        EQU    0x27

; Tx Buffer 0 *
TXB0CTRL        EQU    0x30
TXB0SIDH        EQU    0x31
TXB0SIDL        EQU    0x32
TXB0EID8        EQU    0x33
TXB0EID0        EQU    0x34
TXB0DLC         EQU    0x35
TXB0D0          EQU    0x36
TXB0D1          EQU    0x37
TXB0D2          EQU    0x38
TXB0D3          EQU    0x39
TXB0D4          EQU    0x3A
TXB0D5          EQU    0x3B
TXB0D6          EQU    0x3C
TXB0D7          EQU    0x3D
                 
; Tx Buffer 1 *
TXB1CTRL        EQU    0x40
TXB1SIDH        EQU    0x41
TXB1SIDL        EQU    0x42
TXB1EID8        EQU    0x43
TXB1EID0        EQU    0x44
TXB1DLC         EQU    0x45
TXB1D0          EQU    0x46
TXB1D1          EQU    0x47
TXB1D2          EQU    0x48
TXB1D3          EQU    0x49
TXB1D4          EQU    0x4A
TXB1D5          EQU    0x4B
TXB1D6          EQU    0x4C
TXB1D7          EQU    0x4D

; Tx Buffer 2 *
TXB2CTRL        EQU    0x50
TXB2SIDH        EQU    0x51
TXB2SIDL        EQU    0x52
TXB2EID8        EQU    0x53
TXB2EID0        EQU    0x54
TXB2DLC         EQU    0x55
TXB2D0          EQU    0x56
TXB2D1          EQU    0x57
TXB2D2          EQU    0x58
TXB2D3          EQU    0x59
TXB2D4          EQU    0x5A
TXB2D5          EQU    0x5B
TXB2D6          EQU    0x5C
TXB2D7          EQU    0x5D
                 
; Rx Buffer 0 *
RXB0CTRL        EQU    0x60
RXB0SIDH        EQU    0x61
RXB0SIDL        EQU    0x62
RXB0EID8        EQU    0x63
RXB0EID0        EQU    0x64
RXB0DLC         EQU    0x65
RXB0D0          EQU    0x66
RXB0D1          EQU    0x67
RXB0D2          EQU    0x68
RXB0D3          EQU    0x69
RXB0D4          EQU    0x6A
RXB0D5          EQU    0x6B
RXB0D6          EQU    0x6C
RXB0D7          EQU    0x6D
                 
; Rx Buffer 1 *
RXB1CTRL        EQU    0x70
RXB1SIDH        EQU    0x71
RXB1SIDL        EQU    0x72
RXB1EID8        EQU    0x73
RXB1EID0        EQU    0x74
RXB1DLC         EQU    0x75
RXB1D0          EQU    0x76
RXB1D1          EQU    0x77
RXB1D2          EQU    0x78
RXB1D3          EQU    0x79
RXB1D4          EQU    0x7A
RXB1D5          EQU    0x7B
RXB1D6          EQU    0x7C
RXB1D7          EQU    0x7D
                         

; *******************************************************************
; *               Bit register masks                                *
; *******************************************************************

; TXBnCTRL *
TXREQ           EQU    0x08
TXP             EQU    0x03    

; RXBnCTRL *
RXM             EQU    0x60
BUKT            EQU    0x04

; CANCTRL *
REQOP           EQU    0xE0
ABAT            EQU    0x10
CLKEN           EQU    0x04
CLKPRE          EQU    0x03

; CANSTAT *
REQOP           EQU    0xE0
ICOD            EQU    0x0E

; CANINTE *   
RX0IE           EQU    0x01
RX1IE           EQU    0x02
TX0IE           EQU    0x04
TX1IE           EQU    0x80
TX2IE           EQU    0x10
ERRIE           EQU    0x20
WAKIE           EQU    0x40
IVMIE           EQU    0x80

; CANINTF *   
RX0IF           EQU    0x01
RX1IF           EQU    0x02
TX0IF           EQU    0x04
TX1IF           EQU    0x80
TX2IF           EQU    0x10
ERRIF           EQU    0x20
WAKIF           EQU    0x40
IVMIF           EQU    0x80

; BFPCTRL *
B1BFS           EQU    0x20
B0BFS           EQU    0x10
B1BFE           EQU    0x08
B0BFE           EQU    0x04
B1BFM           EQU    0x02
B0BFM           EQU    0x01

; CNF1 Masks *
SJW             EQU    0xC0
BRP             EQU    0x3F

; CNF2 Masks *
BTLMODE         EQU    0x80
SAM             EQU    0x40
PHSEG1          EQU    0x38
PRSEG           EQU    0x07

; CNF3 Masks *
WAKFIL          EQU    0x40
PHSEG2          EQU    0x07

; TXRTSCTRL Masks *
TXB2RTS         EQU    0x04
TXB1RTS         EQU    0x02
TXB0RTS         EQU    0x01


; *******************************************************************
; *                    Bit Timing Configuration                     *
; *******************************************************************
 
; CNF1 *
SJW_1TQ         EQU    0x40
SJW_2TQ         EQU    0x80
SJW_3TQ         EQU    0x90
SJW_4TQ         EQU    0xC0

; CNF2 *
BTLMODE_CNF3    EQU    0x80
BTLMODE_PH1_IPT EQU    0x00

SMPL_3X         EQU    0x40
SMPL_1X         EQU    0x00

PHSEG1_8TQ      EQU    0x38
PHSEG1_7TQ      EQU    0x30
PHSEG1_6TQ      EQU    0x28
PHSEG1_5TQ      EQU    0x20
PHSEG1_4TQ      EQU    0x18
PHSEG1_3TQ      EQU    0x10
PHSEG1_2TQ      EQU    0x08
PHSEG1_1TQ      EQU    0x00

PRSEG_8TQ       EQU    0x07
PRSEG_7TQ       EQU    0x06
PRSEG_6TQ       EQU    0x05
PRSEG_5TQ       EQU    0x04
PRSEG_4TQ       EQU    0x03
PRSEG_3TQ       EQU    0x02
PRSEG_2TQ       EQU    0x01
PRSEG_1TQ       EQU    0x00

; CNF3 *
PHSEG2_8TQ      EQU    0x07
PHSEG2_7TQ      EQU    0x06
PHSEG2_6TQ      EQU    0x05
PHSEG2_5TQ      EQU    0x04
PHSEG2_4TQ      EQU    0x03
PHSEG2_3TQ      EQU    0x02
PHSEG2_2TQ      EQU    0x01
PHSEG2_1TQ      EQU    0x00

WAKFIL_ENABLED  EQU    0x40
WAKFIL_DISABLED EQU    0x00    


; *******************************************************************
; *                  Control/Configuration Registers                *
; *******************************************************************

; CANINTE *
RX0IE_ENABLED   EQU    0x01
RX0IE_DISABLED  EQU    0x00
RX1IE_ENABLED   EQU    0x02
RX1IE_DISABLED  EQU    0x00
G_RXIE_ENABLED  EQU    0x03
G_RXIE_DISABLED EQU    0x00

TX0IE_ENABLED   EQU    0x04
TX0IE_DISABLED  EQU    0x00
TX1IE_ENABLED   EQU    0x08
TX2IE_DISABLED  EQU    0x00
TX2IE_ENABLED   EQU    0x10
TX2IE_DISABLED  EQU    0x00
G_TXIE_ENABLED  EQU    0x1C
G_TXIE_DISABLED EQU    0x00

ERRIE_ENABLED   EQU    0x20
ERRIE_DISABLED  EQU    0x00
WAKIE_ENABLED   EQU    0x40
WAKIE_DISABLED  EQU    0x00
IVRE_ENABLED    EQU    0x80
IVRE_DISABLED   EQU    0x00

; CANINTF *
RX0IF_SET       EQU    0x01
RX0IF_RESET     EQU    0x00
RX1IF_SET       EQU    0x02
RX1IF_RESET     EQU    0x00
TX0IF_SET       EQU    0x04
TX0IF_RESET     EQU    0x00
TX1IF_SET       EQU    0x08
TX2IF_RESET     EQU    0x00
TX2IF_SET       EQU    0x10
TX2IF_RESET     EQU    0x00
ERRIF_SET       EQU    0x20
ERRIF_RESET     EQU    0x00
WAKIF_SET       EQU    0x40
WAKIF_RESET     EQU    0x00
IVRF_SET        EQU    0x80
IVRF_RESET      EQU    0x00

; CANCTRL * 
REQOP_CONFIG    EQU    0x80
REQOP_LISTEN    EQU    0x60
REQOP_LOOPBACK  EQU    0x40
REQOP_SLEEP     EQU    0x20
REQOP_NORMAL    EQU    0x00

ABORT           EQU    0x10

CLKOUT_ENABLED  EQU    0x04
CLKOUT_DISABLED EQU    0x00
CLKOUT_PRE_8    EQU    0x03
CLKOUT_PRE_4    EQU    0x02
CLKOUT_PRE_2    EQU    0x01
CLKOUT_PRE_1    EQU    0x00

; CANSTAT *
OPMODE_CONFIG   EQU    0x80
OPMODE_LISTEN   EQU    0x60
OPMODE_LOOPBACK EQU    0x40
OPMODE_SLEEP    EQU    0x20
OPMODE_NORMAL   EQU    0x00


; RXBnCTRL *
RXM_RCV_ALL     EQU    0x60
RXM_VALID_EXT   EQU    0x40
RXM_VALID_STD   EQU    0x20
RXM_VALID_ALL   EQU    0x00

RXRTR_REMOTE    EQU    0x08
RXRTR_NO_REMOTE EQU    0x00

BUKT_ROLLOVER    EQU    0x04
BUKT_NO_ROLLOVER EQU    0x00

FILHIT0_FLTR_1  EQU    0x01
FILHIT0_FLTR_0  EQU    0x00

FILHIT1_FLTR_5  EQU    0x05
FILHIT1_FLTR_4  EQU    0x04
FILHIT1_FLTR_3  EQU    0x03
FILHIT1_FLTR_2  EQU    0x02
FILHIT1_FLTR_1  EQU    0x01
FILHIT1_FLTR_0  EQU    0x00


; TXBnCTRL *
TXREQ_SEND      EQU    0x08
TXREQ_ABORT     EQU    0x00

TXP_HIGHEST     EQU    0x03
TXP_INTER_HIGH  EQU    0x02
TXP_INTER_LOW   EQU    0x01
TXP_LOWEST      EQU    0x00
    

; *******************************************************************
; *                  Register Bit Masks                             *
; *******************************************************************
 
DLC_0          EQU    0x00
DLC_1          EQU    0x01
DLC_2          EQU    0x02
DLC_3          EQU    0x03
DLC_4          EQU    0x04
DLC_5          EQU    0x05
DLC_6          EQU    0x06
DLC_7          EQU    0x07    
DLC_8          EQU    0x08
 

; *******************************************************************
; *                  CAN SPI commands                               * 
; *******************************************************************

CAN_RESET       EQU    0xC0
CAN_READ        EQU    0x03
CAN_WRITE       EQU    0x02
CAN_RTS         EQU    0x80
CAN_RTS_TXB0    EQU    0x81
CAN_RTS_TXB1    EQU    0x82
CAN_RTS_TXB2    EQU    0x83
CAN_RD_STATUS   EQU    0xA0
CAN_BIT_MODIFY  EQU    0x05    


; *******************************************************************
; *                  Miscellaneous                                  *
; *******************************************************************

DUMMY_BYTE      EQU    0x00
TXB0            EQU    0x31
TXB1            EQU    0x41
TXB2            EQU    0x51
RXB0            EQU    0x61
RXB1            EQU    0x71
EXIDE_SET       EQU    0x08
EXIDE_RESET     EQU    0x00
