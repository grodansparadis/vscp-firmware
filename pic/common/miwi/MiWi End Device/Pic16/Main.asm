;*********************************************************************
;* FileName:		Main.asm
;* Dependencies:    none
;* Processor:   	PIC18F877A
;* Hardware:		PICDEM Z
;* Assembler:	    MPASMWIN 5.01 or higher
;* Linker:		    MPLINK 4.05 or higher
;* Company:		    Microchip Technology, Inc.
;*
;* Copyright and Disclaimer Notice for MiWi Software:
;*
;* Microchip Technology Inc. (“Microchip”) licenses this software to you 
;* solely for use with Microchip products, as described in the license 
;* agreement accompanying this software (“License”). The software is owned 
;* by Microchip, and is protected under applicable copyright laws.  
;* All rights reserved.
;* 
;* Distribution of this software (in object code or source code) is not 
;* permitted, except as described in Section 2 of the License.
;* 
;* SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
;* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
;* PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP 
;* BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
;* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
;* TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
;* LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, 
;* OR OTHER SIMILAR COSTS.
;*
;********************************************************************
;* File Description:
;*      This file contains an example user application that Demos
;*      the various stack calls required in order to run the stack.
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/09/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************
#define MAIN_ASM

;************************* INCLUDE FILES ****************************
	#include "Console.inc"
	#include "MiWiDefs.inc"
	#include "MiWi.inc"
	#include "SymbolTime.inc"

;************************* DEFINITIONS ******************************
;Stack Definitions
;#define DEBUG_WDTOFF   ;enable in order to disable the WDT (required
                        ;for debug mode).

;Application Definitions
#define BUTTON0 0
#define BUTTON1 1
#define NEED_TO_SEND_BUTTON0 2
#define NEED_TO_SEND_BUTTON1 3

;************************* MACROS ***********************************
mExtCall MACRO function
    movlw high(function)
    movwf PCLATH
    call function
    movlw high($)
    movwf PCLATH
    endm

;************************* CONFIGURATION BITS ***********************
#ifdef DEBUG_WDTOFF
	__config _HS_OSC & _WDT_OFF & _LVP_OFF & _BODEN_OFF
			;Set oscillator to HS,
			;watchdog time off,
			;low-voltage prog. off
#else
	__config _HS_OSC & _WDT_ON & _LVP_OFF & _BODEN_OFF
			;Set oscillator to HS,
			;watchdog time on,
			;low-voltage prog. off
#endif

;************************* VARIABLES ********************************
SECT_INT_CONTEXT_SAVE_VARS UDATA_SHR
PCLATH_TEMP     res 0x01
STATUS_TEMP     res 0x01
W_TEMP          res 0x01

SECT_MAIN_ASM_VARS UDATA
myTempVar       res 0x01
myButtons       res 0x01
#ifdef DEBUG_WDTOFF
myTempCounter   res 0x02
#endif

;************************* CODE *************************************
SECT_RESET_VECTOR CODE 0x0000
    movlw high($)        ;set the PCLATH to the first page
    movwf PCLATH    
	goto main            ;goto the main program

SECT_INTERRUPT_VECTOR CODE 0x0004
    nop                  ;kill one word to avoid having to modify
                         ;  the linker file
SECT_INTERRUPT_ROLL_OVER CODE 0x0005
    movwf W_TEMP         ;Copy W to TEMP register
    swapf STATUS,W       ;Swap status to be saved into W
    clrf STATUS          ;bank 0, regardless of current 
                         ;  bank, Clears IRP,RP1,RP0
    movwf STATUS_TEMP    ;Save status to bank 
                         ;zero STATUS_TEMP register
    movf PCLATH, W       ;Only required if using pages 1, 2 and/or 3
    movwf PCLATH_TEMP    ;Save PCLATH into W
    clrf PCLATH          ;Page zero, regardless of current page

    movlw high(InterruptHandler)    ;switch to the interrupt page
    movwf PCLATH
    goto InterruptHandler           ;go to the interrupt handler
    	    
;********************************************************************
; Function: main
;
; PreCondition: none
;
; Input: none
;
; Output: none
;
; Side Effects: none
;
; Stack Requirements: none
;
; Overview: Main application function
;*********************************************************************  
main
	call BoardInit          ;Initialize the board
	
	;Check what caused the reset to occur
	btfss PCON,1            ;if POR occured
	goto _NoBOR             ;  continue with the application
	btfsc STATUS,4          ;if the WDT was not the cause
	goto _NoWDT             ;  then skip the WDT handler
	
    bcf PORTA,0             ;WDT cause the the reset
    bsf PORTA,1             ;Initialize the LEDs
    movlw 0x03              ;reset PCON for the next reset
    movwf PCON
_WDT                        
    movlw 0x03              ;Alternate RA0 and RA1 blinking
    xorwf PORTA,f
    clrwdt
    
    #ifndef DEBUG_WDTOFF
        sleep
    #else
        ;simulate a sleep instruction by locking up the CPU
        movlw 0xFF
        movwf myTempCounter+0
        movlw 0xFF
        movwf myTempCounter+1
_WDTResetLoop
        decfsz myTempCounter+0,f
        goto _WDTResetLoop
        decfsz myTempCounter+1,f
        goto _WDTResetLoop
    #endif
    
    goto _WDT
    
_NoWDT                      ;if it wasn't the WDT
	clrwdt                  ;clear the WDT so it doesn't reset
	btfsc PCON,0            ;if there wasn't a BOR
	goto _NoBOR             ;  then skip over the BOR handler
	
    bcf PORTA,0             ;A BOR event did occur
    bsf PORTA,1             ;Initialize the LEDs
    movlw 0x03              ;reset PCON for the next reset
    movwf PCON
_BOR                        ;BOR reset caused reset
    movlw 0x01              ;Set RA1 and toggle RA0
    xorwf PORTA,f
    
    #ifndef DEBUG_WDTOFF
        sleep
    #else
        ;simulate a sleep instruction by locking up the CPU
        movlw 0xFF
        movwf myTempCounter+0
        movlw 0xFF
        movwf myTempCounter+1
_BORResetLoop
        decfsz myTempCounter+0,f
        goto _BORResetLoop
        decfsz myTempCounter+1,f
        goto _BORResetLoop
    #endif
    
    goto _BOR
    
_NoBOR                      ;If there wasn't a BOR or WDT
    movlw 0x03              ;reset PCON for the next reset
    movwf PCON
    clrwdt
    
#ifdef ENABLE_CONSOLE
    ;If the console module is enabled then initialize it
    movlw high(ConsoleInit)
    movwf PCLATH
    call ConsoleInit        ;Initialize the UART
    movlw high($)
    movwf PCLATH
#endif

_JoinFailedInit
    clrwdt
      
    movlw high(MRF24J40Init)
    movwf PCLATH
	call MRF24J40Init       ;Initialize the MRF24J40
	call MiWiInit           ;Initialize the MiWi Stack
    movlw high($)
    movwf PCLATH
	clrwdt

    ;Enable interrupts		
    bsf INTCON,PEIE
    bsf INTCON,GIE
   
    ;Attempt to rejoin a network previously joined to
    movlw high(RejoinNetwork)
    movwf PCLATH
	call RejoinNetwork
	movlw high($)
	movwf PCLATH
	
_RejoiningNetwork
    clrwdt
    ;Call MiWiTasks to process any stack tasks that are required to 
    ;  rejoin the network
    movlw high(MiWiTasks)
    movwf PCLATH
    call MiWiTasks
    movlw high($)
    movwf PCLATH
    
    ;If the rejoin attempt has not completed or expired (timed-out)
    btfsc MiWiTasksVar,REJOIN_NETWORK
    ;then loop back
    goto _RejoiningNetwork	
    
_DoNotRejoin
    ;if the rejoin was successful
    btfsc MiWiStatus,NETWORK_MEMBER 
    ;goto the application code
    goto _ApplicationLoop   

    ;if we did not rejoin a network then try to join a new network
    movlw high(StartNetworkSearch)
    movwf PCLATH
    call StartNetworkSearch
    movlw high($)
    movwf PCLATH
    
_SearchingForANetwork
    clrwdt
    ;Call MiWiTasks in order to handle stack tasks during the search
    ;  process
    movlw high(MiWiTasks)
    movwf PCLATH
    call MiWiTasks
    movlw high($)
    movwf PCLATH
    
    ;if the network search process hasn't found joined a network
    ;  or hasn't timed out yet
	btfsc MiWiTasksVar,SEARCH_FOR_NETWORK
	;  then loopback and keep looking
	goto _SearchingForANetwork
	
	;if the network search is either timed-out or found a network
	btfss MiWiStatus,FOUND_NETWORK
	;  then if we failed to find a network, skip to _JoinFailedInit
	goto _JoinFailedInit
	
	;If we found a network then try to join it
	movlw high(JoinNetwork)
	movwf PCLATH
	call JoinNetwork
	movlw high($)
	movwf PCLATH
	
_JoiningNetwork
    clrwdt
    ;Call MiWiTasks in order to handle stack tasks during the joining
    ;  process
    movlw high(MiWiTasks)
    movwf PCLATH
    call MiWiTasks
    movlw high($)
    movwf PCLATH
    
    ;if the join hasn't timed out yet
    btfsc MiWiTasksVar,JOIN_NETWORK
    ;  then loop back
    goto _JoiningNetwork
       
    ;if the join is complete then see if it was successful
    btfss MiWiTasksVar,JOIN_SUCCESSFUL
    ;If the join failed then decide what to do.
    ;  In this case we are just reinitializing 
    ;  everything and starting over
    goto _JoinFailedInit    
    
    ;Otherwise the join was successful so we are up and running 
    
;==================================================================;
;=====================   Application code   =======================;
;==================================================================;
_ApplicationLoop    
    ;Don't sleep if the TX is busy, if we have a packet to process, 
    ;or if a data request is in progress
    movf MiWiStatus,w
    andlw MIWI_STATUS_DONT_SLEEP_MASK
    ;Don't sleep if...
    ;  Data request in progress, Rx received data, Tx is busy
    btfss STATUS,Z
    goto _ApplicationLoopDontSleep
        
    ;if either button needs processing then don't go to sleep
    movf myButtons,w
    andlw (1<<NEED_TO_SEND_BUTTON1 | 1<<NEED_TO_SEND_BUTTON0)
    btfss STATUS,Z
    goto _ApplicationLoopDontSleep
      
    #ifndef DEBUG_WDTOFF
        ;TODO - actually put the MRF24J40 to sleep
        movlw high(MRF24J40Sleep)
        movwf PCLATH
        call MRF24J40Sleep

        sleep     
        
        call MRF24J40Wake
        movlw high($)
        movwf PCLATH
        ;wake up the MRF24J40 and wait for it to stabilize
    #else
        movlw high(MRF24J40Sleep)
        movwf PCLATH
        call MRF24J40Sleep
        movlw high($)
        movwf PCLATH
        
        ;simulate a sleep instruction by locking up the CPU
        movlw 0xFF
        movwf myTempCounter+0
        movlw 0xFF
        movwf myTempCounter+1
_ApplicationSleepDelayLoop1
        decfsz myTempCounter+0,f
        goto _ApplicationSleepDelayLoop1
        decfsz myTempCounter+1,f
        goto _ApplicationSleepDelayLoop1
        
        movlw high(MRF24J40Wake)
        movwf PCLATH
        call MRF24J40Wake
        movlw high($)
        movwf PCLATH
    #endif
    
    clrwdt  ;clear the watchdog first thing
       
    ;only data request once per sleep period
    ;send the data request (short address mode)
    movlw DATA_REQ_SHORT_ADDR
    movwf MRF_DATA
    movlw high(LoadDataRequest)
    movwf PCLATH
    call LoadDataRequest
    call TransmitPacket
    movlw high($)
    movwf PCLATH
    
_ApplicationLoopDontSleep
    ;clear the watchdog in case we didn't sleep and missed the other clrwdt
    clrwdt
    
    ;If we are removed from the network
    btfss MiWiStatus,NETWORK_MEMBER
    ;restart the join process (or whatever the application wants to do)
    goto _JoinFailedInit
    
    ;Process any button presses
    call ProcessButton
    
    ;Handle any MiWi Stack tasks
    movlw high(MiWiTasks)
    movwf PCLATH
    call MiWiTasks
    movlw high($)
    movwf PCLATH
    
    ;Clear the WDT again
    clrwdt 
    
    ;see if there is a packet waiting to be processed
    ;  at the application level
    btfss MiWiStatus,PACKET_RECEIVED
    goto _ApplicationLoopNoPacket
    
    ;There is a packet, process it here
    movf RxPacketAddress,w
    ;in this example we are going to skip straight
    ;  to the report type and ID
    addlw d'11' 
    movwf FSR
    movf INDF,w
    xorlw 0x12
    btfss STATUS,Z
    ;not the Report Type we were looking for, discard the packet
    goto _ApplicationLoopPacketDiscard
    incf FSR,f
    movf INDF,w
    xorlw 0x34
    btfss STATUS,Z
    ;not the Report Type we were looking for, discard the packet
    goto _ApplicationLoopPacketDiscard  

#ifdef ENABLE_CLUSTER_SOCKETS
    ;if we don't have a socket configured
    ;   then don't toggle the light
    btfss MiWiSocketStatus,SOCKET_RESPONSE_RECEIVED 
    goto _ApplicationLoopPacketDiscard
#endif
    
    ;if it was the right report type then let's process the packet
    incf FSR,f
    movf INDF,w
    btfsc STATUS,Z  ;if the data byte was 0x00 then clear PORTA0
    bcf PORTA,0
    incf INDF,w
    btfsc STATUS,Z  ;if the data byte was 0xFF then set PORTA0
    bsf PORTA,0
    movf INDF,w
    xorlw 0x55
    btfss STATUS,Z  ;if the data byte was 0x55 then toggle PORTA0
    goto _ApplicationLoopPacketDiscard 
_ApplicationLoopToggleLight 
    movlw 0x01
    xorwf PORTA,F
    ;fall through to the discard section 
     
_ApplicationLoopPacketDiscard
    ;Indicate that the application has finished with the packet
    ;  This is required in order to receive any more packets
    bcf MiWiStatus,PACKET_RECEIVED
    
_ApplicationLoopNoPacket
#ifdef ENABLE_CLUSTER_SOCKETS
    ;if we recently received a open socket response
    btfss MiWiSocketStatus,SOCKET_RESPONSE_RECEIVED
    goto _ApplicationLoopButton0    ;otherwise handle the buttons
    ;if the socket was successfully created
    btfsc MiWiSocketStatus,SOCKET_CREATED
    bsf PORTA,1
#endif

_ApplicationLoopButton0
    #ifdef ENABLE_CLUSTER_SOCKETS
        ;if the TX buffer isn't already busy
        btfsc MiWiStatus,TX_BUSY
        goto _ApplicationLoopNotButton1
        ;and BUTTON0 was pressed
        btfss myButtons,NEED_TO_SEND_BUTTON0
        goto _ApplicationLoopNotButton0
        
        ;***** Example 1 ******
        ; use the socket created by the RB4 button to 
        ;    toggle the light on the other board
        ;**********************
        ;If there is a valid socket
        btfss MiWiSocketStatus,SOCKET_RESPONSE_RECEIVED
        goto _ApplicationLoopButton0Done
        btfss MiWiSocketStatus,SOCKET_CREATED
        goto _ApplicationLoopButton0Done
        
        ;send a light toggle message to the socketed partner
        call SendLightToggle

        ;***** Example 2 ******
        ; Search the network for device by EUI
        ;**********************   
        
        ;load the address we are looking for     
;        movlw 0x07      ;Long address LSB
;        movwf sourceLongAddress+0
;        movlw 0x06
;        movwf sourceLongAddress+1
;        movlw 0x05
;        movwf sourceLongAddress+2
;        movlw 0x04
;        movwf sourceLongAddress+3
;        movlw 0x03
;        movwf sourceLongAddress+4
;        movlw 0x02
;        movwf sourceLongAddress+5
;        movlw 0x01
;        movwf sourceLongAddress+6
;        movlw 0x55      ;Long address MSB
;        movwf sourceLongAddress+7
;        ; send the request for that devices information
;        mExtCall SendEUIAddressSearchRequest
        
        
_ApplicationLoopButton0Done 
    #endif
        ;once the BUTTON0 processing is done
        ;clear the flag so we can get another Button event
        bcf myButtons,NEED_TO_SEND_BUTTON0
_ApplicationLoopNotButton0

    #ifdef ENABLE_CLUSTER_SOCKETS
        ;if the TX Buffer is not busy
        btfsc MiWiStatus,TX_BUSY
        goto _ApplicationLoopNotButton1
        ;and BUTTON1 was pressed
        btfss myButtons,NEED_TO_SEND_BUTTON1
        goto _ApplicationLoopNotButton1
        
        ;Attempt to open a cluster socket
        movlw high(OpenSocket)
        movwf PCLATH
        call OpenSocket
        movlw high($)
        movwf PCLATH
        
        ;clear the LED that indicates the socket
        ;  formation was successful
        bcf PORTA,1
    #endif
        ;indicate that the button event has been serviced
        bcf myButtons,NEED_TO_SEND_BUTTON1
_ApplicationLoopNotButton1
    ;all events and handlers are finished
    ;loop back to the main application loop
	goto _ApplicationLoop
	
;********************************************************************
; Function: ProcessButton
;
; PreCondition: none
;
; Input: none
;
; Output: myButtons,BUTTON0 or BUTTON1 may become set
;
; Side Effects: RBIF is cleared, RBIE is set
;
; Stack Requirements: 1 level deep
;
; Overview: Processes the button presses
;*********************************************************************  
ProcessButton
    ;test to see if PORTB,5 is pressed
    btfsc PORTB,5
    goto _ProcessButtonClearRB5
    ;it it wasn't pressed before then set the event
    btfsc myButtons,BUTTON0
    goto _ProcessButtonCheckRB4 
    ;RB5 has been press and wasn't previously pressed
    ;indicate the press event
    bsf myButtons,BUTTON0
    bsf myButtons,NEED_TO_SEND_BUTTON0
    goto _ProcessButtonCheckRB4
    ;if RB5 was not pressed then clear the event
_ProcessButtonClearRB5
    bcf myButtons,BUTTON0
_ProcessButtonCheckRB4   
    ;check to see if RB4 is pressed 
    btfsc PORTB,4
    goto _ProcessButtonClearRB4
    btfsc myButtons,BUTTON1
    goto _ProcessButtonDone 
    ;RB5 has been press and wasn't previously pressed
    ;indicate the press
    bsf myButtons,BUTTON1
    bsf myButtons,NEED_TO_SEND_BUTTON1
    goto _ProcessButtonDone
_ProcessButtonClearRB4
    ;if RB4 was not pressed then clear the event
    bcf myButtons,BUTTON1
_ProcessButtonDone
    movf PORTB,w    ;clear the mismatch
    bcf INTCON,RBIF ;clear the interrupt flag
    bsf INTCON,RBIE ;set the interrupt enable
    return

;********************************************************************
; Function: SendLightToggle
;
; PreCondition: if TX_BUSY is sent then the packet will not go out.
;
; Input: Valid socket handle in MiWiSocketHandle
;
; Output: none
;
; Side Effects: TX_BUSY becomes set
;
; Stack Requirements: 1 level deep
;
; Overview: Sends out a light toggle message to the socketed partner
;*********************************************************************  
#ifdef ENABLE_CLUSTER_SOCKETS
SendLightToggle   
    ;point to the socketed network table entry
    bcf STATUS,IRP  
    movf MiWiSocketHandle,w
    movwf FSR
    ;if the entry is valid and has a valid short address
    btfss INDF,STATUS_IS_VALID
    goto _SendLightToggleEnd
    btfss INDF,STATUS_SHORT_ADDRESS_VALID
    goto _SendLightToggleEnd
    
    ;send out the toggle packet
    ;read out the PANID of the socketed partner
    incf FSR,f
    movf INDF,w
    movwf MiWiSourcePANID+0
    incf FSR,f
    movf INDF,w
    movwf MiWiSourcePANID+1
    
    ;followed by the source short address    
    incf FSR,f
    movf INDF,w
    movwf MiWiSourceShortAddress+0    
    incf FSR,f
    movf INDF,w
    movwf MiWiSourceShortAddress+1
    
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;load the TX buffer with the toggle packet
    ;header size
    movlw d'23'  
    movwf INDF
    incf FSR,F
    ;packet size
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x61
    movwf INDF
    incf FSR,F
    
    movlw 0x88
    movf MRF_DATA,f
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
      
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination Short Address
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
      
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;Start of the MiWi Header
    
    movlw 0x04      ;HOPS
    movwf INDF
    incf FSR,F
    
    movlw 0x06      ;MiWi FrameControl - 
                    ;  no ACK (this is it), Intra, no security
    movwf INDF
    incf FSR,F
    
    ;destination PANID - located in sourcePANID
    movf MiWiSourcePANID+0,w
    movwf INDF
    incf FSR,F
    
    movf MiWiSourcePANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination address - located in sourceShortAddress
    movf MiWiSourceShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf MiWiSourceShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;source PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;source address
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MiWiSourceSeqNum,W     ;send the same sequence 
                                ;  number that we got from them
    movwf INDF
    incf FSR,F
    
    ;Report Type
    movlw 0x12
    movwf INDF
    incf FSR,F
    
    ;Report ID
    movlw 0x34
    movwf INDF
    incf FSR,F
    
    ;Report Value
    movlw 0x55
    movwf INDF
    incf FSR,F
    
    ;send the packet
    movlw high(TransmitPacket)
    movwf PCLATH
    call TransmitPacket
    movlw high($)
    movwf PCLATH
        
_SendLightToggleEnd    
    return
#endif
    
;********************************************************************
; Function: BoardInit
;
; PreCondition: none
;
; Input: none
;
; Output:   none
;
; Side Effects: none
;
; Stack Requirements: 1 level deep
;
; Overview: The configuration of the mircocontroller is changed for
;             use with the PICDEM Z
;*********************************************************************  		
BoardInit
	banksel TRISA
	movlw b'00111100'
	movwf OPTION_REG    ;set the WDT prescaler that we desire
	                    ;  enable pullups
	                    
	;Host  MRF24J40  Direction 
	;-------------------------                   
    ;RC2   RESETn    Output
    ;RB0   INT       Input
    ;RC5   MOSI      Output
    ;RC3   SCK       Output
    ;RB2   N/C       N/A
    ;RC1   WAKE      Output
    ;RC0   CSn       Output
    ;RC4   MISO      Input
    ;RB1   N/C       N/A
    ;RB3   N/C       N/A
    
    ;Host  Application
    ;-------------------------
    ;RA0   LED
    ;RA1   LED
    ;RB4   BUTTON
    ;RB5   BUTTON
	bcf TRISA,0
	bcf TRISA,1
	bsf TRISB,0
	banksel PHY_CS_TRIS_REG
	bcf PHY_CS_TRIS	
	bcf PHY_WAKE_TRIS
	banksel TRISC
	movlw b'11010111'
	andwf TRISC,f		    ;set SDO and SCK to outputs
	movlw b'00010000'
	iorwf TRISC,f		    ;set SDI as an input
	movlw b'11111011'
	andwf TRISA,f
	movlw 0x07
	movwf ADCON1
	movlw 0xC0
	movwf SSPSTAT
	banksel SSPCON
	movlw 0x20
	movwf SSPCON
	clrf PORTA
	bsf PORTA,2
	banksel 0
	
    bsf INTCON,RBIE
	return

;********************************************************************
; Function: InterruptHandler
;
; PreCondition: none
;
; Input:   none
;
; Output:   none
;
; Side Effects: none
;
; Stack Requirements: 1 level deep
;
; Overview: Handles all interrupts
;*********************************************************************      
InterruptHandler
    clrwdt
    
    ;check to see if it was a timer 1 interrupt
    banksel PIR1
    btfss PIR1,TMR1IF
    goto _UserIntHandler
    banksel PIE1
    btfsc PIE1,TMR1IE
    banksel TimerExtension
    ;if it was then increment the timer extenders
    incf TimerExtension+0,F
    btfsc STATUS,Z
    incf TimerExtension+1,F
    banksel PIR1
    ;clear the interrupt flag
    bcf PIR1,TMR1IF
    
_UserIntHandler
    ;Insert customer interrupt handler here
    btfss INTCON,RBIF
    goto _InterruptHanderDone
    bcf INTCON,RBIE
_InterruptHanderDone

    ;restore the context information
    movf PCLATH_TEMP, W     ;Restore PCLATH
    movwf PCLATH            ;Move W into PCLATH
    swapf STATUS_TEMP,W     ;Swap STATUS_TEMP register into W
                            ;(sets bank to original state)
    movwf STATUS            ;Move W into STATUS register
    swapf W_TEMP,F          ;Swap W_TEMP
    swapf W_TEMP,W          ;Swap W_TEMP into W
    
    retfie                  ;return from the interrupt

	end
