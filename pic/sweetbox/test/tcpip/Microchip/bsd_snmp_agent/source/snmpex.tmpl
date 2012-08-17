/********************************************************************
 * FileName:		SNMP.c		
 * Dependencies:
 * Processor:				
 * Hardware:		
 * Assembler:		
 * Linker:		
 * Company:		Microchip Technology, Inc.
 *
 * Software License Agreement:
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
 *********************************************************************
 * File Description:
 *
 * HTTP Server Appilcation
 *
 * Change History:
 * Name				Date            Changes
 * Sean Justice		02/21/06		Initial Version
 ********************************************************************/


#include "bsd_snmp_agent\snmp.h"
#include "snmpex.h"
#include "snmpex_private.h"
#include "ex16lcd.h"

/********************************************************************
 * Private Variables
 ********************************************************************/
PRIVATE WORD_VAL	_an0Value;
PRIVATE WORD_VAL	_an1Value;
PRIVATE TRAP_INFO	_trapInfo = { (1) };
PRIVATE BYTE        _lcdMessage[17] = "SNMP Agent Demo";
/*********************************************************************
 * Function:       PUBLIC void SNMPProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This functions handles SNMP example variables
 **********************************************************************/
PUBLIC void SNMPProcessIO(void)
{
#ifndef _WINDOWS
    static BOOL lbNotify = FALSE;
    static BYTE i = 0;
    SNMP_VAL val;

    mSetupSwitch3();

    if (!mGetSwitch3() && !lbNotify )
        lbNotify = TRUE;

    if ( i == _trapInfo.Size )
    {
        i = 0;
        lbNotify = FALSE;
    }

    if ( lbNotify )
    {
        if ( _trapInfo.table[i].Flags.bEnabled )
        {
           val.byte = 0;
            if ( SNMPSendNotification(i, PUSH_BUTTON, val) )
                i++;
        }
        else
            i++;
    }

    _an0Value.v[0]++;
    _an1Value.v[0]++;


#endif
}
/*********************************************************************
 * Function:       PUBLIC BOOL SNMPValidate(	SNMP_ACTION pdu, 
 *												BYTE *community)
 *
 * PreCondition:    None
 *
 * Input:           pdu			- protocol data unit
 *					community	- string of the community
 *
 * Output:          true if pdu and community are valid, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function checks the PDU and community of the 
 *					request to make sure that it is valid
 ********************************************************************/
PUBLIC BOOL SNMPValidate(SNMP_ACTION pdu, BYTE *community)
{
	return TRUE;
}
/*********************************************************************
 * Function:       PUBLIC BOOL SNMPGetVar(	SNMP_ID id, 
 *											BYTE index, 
 *											BYTE *ref, 
 *											SNMP_VAL *value)
 *
 * PreCondition:    None
 *
 * Input:           id			- id of the variable
 *					index		- index if multipul estances
 *					ref			- pointer to the callback times
 *					value		- return value
 *
 * Output:          true if processed, else false
 *
 * Side Effects:    none
 *
 * Overview:        This gets requested information from the SNMP
 *					manager.
 ********************************************************************/
PUBLIC BOOL SNMPGetVar(SNMP_ID id, BYTE index, BYTE *ref, SNMP_VAL *value)
{
    BYTE myRef;

    myRef = *ref;

    switch(id)
    {
    case LED_D5:
        value->byte = mGetLed5();
        return TRUE;

    case LED_D6:
        value->byte = mGetLed6();
        return TRUE;

    case PUSH_BUTTON:
        // There is only one button - meaning only index of 0 is allowed.
        mSetupSwitch3();
        
		value->byte = mGetSwitch3();
        return TRUE;

    case ANALOG_POT0:
        value->word = _an0Value.Val;
        return TRUE;

    case ANALOG_POT1:
        value->word = _an1Value.Val;
        return TRUE;

	case SYS_UP_TIME:
		value->dword = TickGet();
		return TRUE;

    case TRAP_RECEIVER_ID:
        if ( index < _trapInfo.Size )
        {
            value->byte = index;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_ENABLED:
        if ( index < _trapInfo.Size )
        {
            value->byte = _trapInfo.table[index].Flags.bEnabled;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_IP:
        if ( index < _trapInfo.Size )
        {
            value->dword = _trapInfo.table[index].IPAddress.Val;
            return TRUE;
        }
        break;

    case TRAP_COMMUNITY:
        if ( index < _trapInfo.Size )
        {
            if ( _trapInfo.table[index].communityLen == 0 )
                *ref = SNMP_END_OF_VAR;
            else
            {
                value->byte = _trapInfo.table[index].community[myRef];

                myRef++;

                if ( myRef == _trapInfo.table[index].communityLen )
                    *ref = SNMP_END_OF_VAR;
                else
                    *ref = myRef;
            }
            return TRUE;
        }
        break;

    case LCD_DISPLAY:
        if(myRef >= sizeof(_lcdMessage))
        {
	        myRef = SNMP_END_OF_VAR;
	    }
        else
        {
           	value->byte = _lcdMessage[myRef++];
		
			if((myRef >= sizeof(_lcdMessage)) || (_lcdMessage[myRef] == 0))
	        	myRef = SNMP_END_OF_VAR;
		}
				
        *ref = myRef;
        return TRUE;

    }

    return FALSE;
}
/*********************************************************************
 * Function:       PUBLIC BOOL SNMPGetNextIndex(BYTE id, BYTE *index)
 *
 * PreCondition:    None
 *
 * Input:           id		- id of varible
 *					index	- pointer of the current index
 *
 * Output:          true if processed, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function get the next id from the current one.
 *					If there is no other index, than it returns FALSE
 ********************************************************************/
PUBLIC BOOL SNMPGetNextIndex(BYTE id, BYTE *index)
{
    SNMP_INDEX tempIndex;

    tempIndex = *index;

    switch(id)
    {
    case TRAP_RECEIVER_ID:
        // There is no next possible index if table itself is empty.
        if ( _trapInfo.Size == 0 )
            return FALSE;

        // INDEX_INVALID means start with first index.
        if ( tempIndex == SNMP_INDEX_INVALID )
        {
            *index = 0;
            return TRUE;
        }
        else if ( tempIndex < (_trapInfo.Size - 1) )
        {
            *index = tempIndex + 1;
            return TRUE;
        }
        break;
    }
    return FALSE;
}
/*********************************************************************
 * Function:		PUBLIC BOOL SNMPSetVar(	BYTE id, 
 *											BYTE index, 
 *											BYTE reference, 
 *											SNMP_VAL value)       
 *
 * PreCondition:    None
 *
 * Input:           id			- id of the variable
 *					index		- index if multipul estances
 *					ref			- pointer to the callback times
 *					value		- return value
 *
 * Output:          true if processed, else false
 *
 * Side Effects:    none
 *
 * Overview:        This sets requested information from the SNMP
 *					manager.
 ********************************************************************/
PUBLIC BOOL SNMPSetVar(BYTE id, BYTE index, BYTE reference, SNMP_VAL value)
{
    switch(id)
    {
    case LED_D5:
        mSetupLed5();
        if(value.byte)
        	mSetLed5();
		else
			mClearLed5();
        return TRUE;

    case LED_D6:
        mSetupLed6();
        if(value.byte)
        	mSetLed6();
		else
			mClearLed6();
        return TRUE;

    case TRAP_RECEIVER_IP:
        // Make sure that index is within our range.
        if ( index < _trapInfo.Size )
        {
            // This is just an update to an existing entry.
            _trapInfo.table[index].IPAddress.Val = value.dword;
            return TRUE;
        }
        else if ( index < TRAP_TABLE_SIZE )
        {
            // This is an addition to table.
            _trapInfo.table[index].IPAddress.Val = value.dword;
            _trapInfo.table[index].communityLen = 0;
            _trapInfo.Size++;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_ENABLED:
        // Make sure that index is within our range.
        if ( index < _trapInfo.Size )
        {
            // Value of '1' means Enabled".
            if ( value.byte == 1 )
                _trapInfo.table[index].Flags.bEnabled = 1;
            // Value of '0' means "Disabled.
            else if ( value.byte == 0 )
                _trapInfo.table[index].Flags.bEnabled = 0;
            else
                // This is unknown value.
                return FALSE;
            return TRUE;
        }
        // Given index is more than our current table size.
        // If it is within our range, treat it as an addition to table.
        else if ( index < TRAP_TABLE_SIZE )
        {
            // Treat this as an addition to table.
            _trapInfo.Size++;
            _trapInfo.table[index].communityLen = 0;
        }

        break;

    case TRAP_COMMUNITY:
        // Since this is a ASCII_STRING data type, SNMP will call with
        // SNMP_END_OF_VAR to indicate no more bytes.
        // Use this information to determine if we just added new row
        // or updated an existing one.
        if ( reference ==  SNMP_END_OF_VAR )
        {
            // Index equal to table size means that we have new row.
            if ( index == _trapInfo.Size )
                _trapInfo.Size++;

            // Length of string is one more than index.
            _trapInfo.table[index].communityLen++;

            return TRUE;
        }

        // Make sure that index is within our range.
        if ( index < _trapInfo.Size )
        {
            // Copy given value into local buffer.
            _trapInfo.table[index].community[reference] = value.byte;
            // Keep track of length too.
            // This may not be NULL terminate string.
            _trapInfo.table[index].communityLen = (BYTE)reference;
            return TRUE;
        }
        break;

    case LCD_DISPLAY:
 		if ( reference != SNMP_END_OF_VAR )
        {
            _lcdMessage[reference] = value.byte;
            _lcdMessage[reference + 1] = 0;
        }
        else
        {
	        Ex16LCDWriteLine(1, _lcdMessage);
        }
       return TRUE;

    }

    return FALSE;
}
/*********************************************************************
 * Function:		PUBLIC BOOL SNMPIsValidSetLen(BYTE id, BYTE length) 
 *
 * PreCondition:    None
 *
 * Input:           id			- id of the variable
 *					length		- length in question
 *
 * Output:          true if the length is valid, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function checks the length based on the id
 ********************************************************************/
PUBLIC BOOL SNMPIsValidSetLen(BYTE id, BYTE length)
{
    switch(id)
    {
    case TRAP_COMMUNITY:
        if ( length < SNMP_COMMUNITY_MAX_LEN + 1 )
            return TRUE;
        break;

    case LCD_DISPLAY:
        if ( length < LCD_DISPLAY_LEN + 1 )
            return TRUE;
        break;
    }

    return FALSE;
}
/*********************************************************************
 * Function:       PRIVATE BOOL SNMPSendNotification(	BYTE receiverIndex, 
 *														SNMP_ID var, 
 *														SNMP_VAL val)
 *
 * PreCondition:    None
 *
 * Input:           receiverIndex	- index of the trap table
 *					var				- SNMP_ID
 *					val				- value of the SNMP 
 *
 * Output:          true if processed, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function sends the notification using the trap 
 *					port and address
 ********************************************************************/
PRIVATE BOOL SNMPSendNotification(BYTE receiverIndex, SNMP_ID var, SNMP_VAL val)
{
#ifndef _WINDOWS
    static enum { SM_PREPARE, SM_NOTIFY_WAIT } smState = SM_PREPARE;
    IP_ADDR IPAddress;

    // Convert local to network order.
    IPAddress.v[0] = _trapInfo.table[receiverIndex].IPAddress.v[3];
    IPAddress.v[1] = _trapInfo.table[receiverIndex].IPAddress.v[2];
    IPAddress.v[2] = _trapInfo.table[receiverIndex].IPAddress.v[1];
    IPAddress.v[3] = _trapInfo.table[receiverIndex].IPAddress.v[0];

    switch(smState)
    {
    case SM_PREPARE:
        SNMPNotifyPrepare(&IPAddress,
                          _trapInfo.table[receiverIndex].community,
                          _trapInfo.table[receiverIndex].communityLen,
                          MICROCHIP,            // Agent ID Var
                          6,                    // Notification code
                          (DWORD)TickGet());

        smState = SM_NOTIFY_WAIT;

        break;

    case SM_NOTIFY_WAIT:
        if ( SNMPIsNotifyReady(&IPAddress) )
        {
            smState = SM_PREPARE;
            SNMPNotify(var, val, 0);
            return TRUE;
        }
    }
    return FALSE;

#else
    return TRUE;
#endif    
}
