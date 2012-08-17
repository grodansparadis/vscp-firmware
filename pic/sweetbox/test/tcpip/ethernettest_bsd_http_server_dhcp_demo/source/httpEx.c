/********************************************************************
 * FileName:		httpEx.c
 * Dependencies:
 * Processor:		PIC24FJ128
 * Hardware:		N/A
 * Assembler:		N/A
 * Linker:		N/A
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
 * Name         Date            Changes
 * S Justice    01/09/06        Initial Version
 ********************************************************************/

#include <string.h>
#include "bsd_http_server\http.h"
#include "httpEx.h"
#include "ex16lcd.h"
#include <plib.h>

#define HTTPExampleLed1Toggle()     (mPORTASetPinsDigitalOut(BIT_7), mPORTAToggleBits(BIT_7))
#define HTTPLed1On()                (mPORTAReadLatchBits(BIT_7))
#define HTTPConfigLed1()            (mPORTASetPinsDigitalOut(BIT_7))

#define HTTPExampleLed2Toggle()     (mPORTASetPinsDigitalOut(BIT_6), mPORTAToggleBits(BIT_6))
#define HTTPLed2On()                (mPORTAReadLatchBits(BIT_6))
#define HTTPConfigLed2()            (mPORTASetPinsDigitalOut(BIT_6))

const HTTP_EX_FILENAME athenticateFiles[] =
{
    {   "authent.htm"   }
};
#define HTTP_EX_AUTHENT_SIZE    (sizeof(athenticateFiles) / sizeof(HTTP_EX_FILENAME))

const HTTP_EX_FILENAME actionFile[] =
{
    {   "io.htm"  }
};
#define HTTP_EX_POST_SIZE        (sizeof(actionFile) / sizeof(HTTP_EX_FILENAME))

const HTTP_EX_FILENAME  ioVar[] =
{
    {   "line1"     },
    {   "line2"     },
    {   "LED1"      },
    {   "LED2"      }
};
#define HTTP_EX_POST_IO_SIZE (sizeof(ioVar) / sizeof(HTTP_EX_FILENAME))


HTTP_EX_FILENAME    lcdLine[2] =
{
    {   "                "   },
    {   "                "   }
};


/*********************************************************************
 * Function:        void HTTPExecCmd(BYTE **arg, BYTE num_arg)
 *
 * PreCondition:
 *
 * Input:           **arg - a pointer to an array of ASCII params
 *                  num_arg - number of params in the array
 *
 * Output:          none
 *
 * Side Effects:
 *
 * Overview:        This function passes params that it has received via
 *                  http GET method
 *
 * Note:
 ********************************************************************/
void HTTPExecCmd(BYTE **arg, BYTE num_arg)
{
    BYTE    param;

    num_arg--;
    param = 1;

    while((num_arg) && (num_arg != 1))
    {
        num_arg -= 2;

        if(num_arg)
            HTTPSendVar(arg[0], arg[param], arg[param + 1], TRUE);
        else
            HTTPSendVar(arg[0], arg[param], arg[param + 1], FALSE);

        param += 2;
    }

    if(num_arg)
        HTTPSendVar(arg[0], arg[param], NULL, FALSE);

}
/*********************************************************************
 * Function:        WORD HTTPGetVar(BYTE var, WORD ref, BYTE *data)
 *
 * PreCondition:
 *
 * Input:           var - escape char
 *                  ref - index of data
 *                  data - byte of file
 *
 * Output:          new ref
 *
 * Side Effects:
 *
 * Overview:        This function passes the escape character and the param
 *                  that is read from the file
 *
 * Note:
 ********************************************************************/
WORD HTTPGetVar(BYTE var, WORD ref, BYTE *data)
{
    switch(var)
    {

        case 1:
            HTTPConfigLed1();
            if(HTTPLed1On())
            {
                *data = 'N';
                ref = HTTP_GET_FILE_EOF_VAR;
            }else
            {
                *data = 'F';

                if(ref)
                    ref = HTTP_GET_FILE_EOF_VAR;
                else
                    ref++;
            }
            break;

        case 2:
            if(HTTPLed2On())
            {
                *data = 'N';
                ref = HTTP_GET_FILE_EOF_VAR;
            }else
            {
                *data = 'F';

                if(ref)
                    ref = HTTP_GET_FILE_EOF_VAR;
                else
                    ref++;
            }
            break;

        case 3:
               *data = lcdLine[0].filename[ref];
               
                ref++;
                
                if((ref >= EX16LCD_LINE_SIZE) || (!lcdLine[0].filename[ref]))
                    ref = HTTP_GET_FILE_EOF_VAR;
            break;        
                
        case 4:
               *data = lcdLine[1].filename[ref];
               
                ref++;
                
                if((ref >= EX16LCD_LINE_SIZE) || (!lcdLine[1].filename[ref]))
                    ref = HTTP_GET_FILE_EOF_VAR;
            break;

        default:
            return HTTP_GET_FILE_EOF_VAR;
    }

    return ref;
}
/*********************************************************************
 * Function:        void HTTPPutVar(BYTE *filename, BYTE *var, BYTE *value, BOOL end)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:
 *
 * Note:
 ********************************************************************/
HTTP_STATUS_CODES HTTPSendVar(BYTE *filename, BYTE *var, BYTE *value, BOOL end)
{
    BYTE    i;
    BYTE    k;

    for(i = 0; i < HTTP_EX_POST_SIZE; i++)
    {
        if(!strcmp(filename, actionFile[i].filename))
            break;
    }

    if(i >= HTTP_EX_POST_SIZE)
        return HTTP_STATUS_NOT_FOUND;

    switch(i)
    {
        case 0:
            for(k = 0; k < HTTP_EX_POST_IO_SIZE; k++)
            {
                if(!strcmp(var, ioVar[k].filename))
                    break;
            }

            switch(k)
            {
            case 0:
                Ex16LCDWriteLine(1, value);
                strncpy(lcdLine[0].filename, value, EX16LCD_LINE_SIZE);
                break;

            case 1:
                Ex16LCDWriteLine(2, value);
                strncpy(lcdLine[1].filename, value, EX16LCD_LINE_SIZE);
                break;

            case 2:
                HTTPExampleLed1Toggle();
                break;

            case 3:
                HTTPExampleLed2Toggle();
                break;

            default:
                return HTTP_STATUS_NOT_IMPLEMENTED;
            }
            break;

    }
    return HTTP_STATUS_OK;
}
/*********************************************************************
 * Function:        BOOL HTTPAuthorizationRequired(BYTE *filename)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:
 *
 * Note:
 ********************************************************************/
BOOL HTTPAuthorizationRequired(BYTE *filename)
{
	BYTE    idx;

    for(idx = 0; idx < HTTP_EX_AUTHENT_SIZE; idx++)
    {
        if(!strcmp(filename, athenticateFiles[idx].filename))
            return TRUE;
    }

    return FALSE;
}
/*********************************************************************
 * Function:        BOOL HTTPPUTSendFile(BYTE *filename)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:
 *
 * Note:
 ********************************************************************/
BOOL HTTPPUTSendFile(BYTE *filename, HTTP_STATUS_CODES status)
{
    if(status == HTTP_STATUS_CREATED)
		strcpy(filename, "put_ok.cgi");
	else if(status == HTTP_STATUS_INTERNAL_SERVER_ERR)
		strcpy(filename, "put_err.htm");
	else
		return FALSE;
		
    return TRUE;
}
/*********************************************************************
 * Function:        BOOL HTTPChkAuthorization(BYTE *type, BYTE *text)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:
 *
 * Note:
 ********************************************************************/
BOOL HTTPChkAuthorization(BYTE *type, BYTE *text)
{
    type = strupr(type);

    if(strcmp(type, "BASIC"))
        return FALSE;

    if(strcmp(text, "YWRtaW46cGFzc3dvcmQ="))
        return FALSE;

    return TRUE;
}
/*********************************************************************
 * Function:        BOOL HTTPPUTUpload(BYTE *filename)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:        Check the file to upload to see if it is allowed
 *
 * Note:
 ********************************************************************/
BOOL HTTPPUTUpload(BYTE *filename)
{
    while(*filename != '.' || *filename != '\0')
		filename++;

	if(*filename == '\0')
		return FALSE;

	if(!strcmp(filename, ".txt"))
		return FALSE;

	return TRUE;
}
/*********************************************************************
 * Function:        BOOL HTTPPUTUpload(BYTE *filename)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:
 *
 * Overview:        Check the file to upload to see if it is allowed
 *
 * Note:
 ********************************************************************/
BYTE _GetNumberChar(DWORD number, BYTE position)
{
    DWORD character;

    character = number;

    while(position)
    {
        character /= 10;
        position--;
    }

    character %= 10;

    return (BYTE)(character | 0x30);
}
