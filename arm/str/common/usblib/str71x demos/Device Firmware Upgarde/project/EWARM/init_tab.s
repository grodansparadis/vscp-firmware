;******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
;* File Name          : init_tab.s
;* Author             : MCD Application Team
;* Date First Issued  : 10/01/2006 : V1.0
;* Description        : The following table entry is needed to make the IAR
;*                      start-up-sequence to copy from a segment (often in ROM)
;*                      to a segment in RAM
;*******************************************************************************
;* History:
;* 10/01/2006 : V1.0
;*******************************************************************************
; THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
; CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
; OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
; OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
; CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************/
    COMMON INTVEC:ROOT
    COMMON INTVEC_ROM:ROOT


    RSEG INITTAB:CODE:ROOT(2) ; 2^2 4 bytes alignment
    DATA
    DCD       sfe(INTVEC_ROM) - sfb(INTVEC_ROM) ; size of the segment
    DCD       sfb(INTVEC)                       ; start address of where to copy to
    DCD       sfb(INTVEC_ROM)                   ; start address of where to copy from

 END
