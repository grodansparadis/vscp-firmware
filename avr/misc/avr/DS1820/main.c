//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------
//
//  main.c -   Application to find and read the 1-Wire Net
//             DS1920/DS1820/DS18S20 - temperature measurement.
//
//             This application uses the files from the 'Public Domain'
//             1-Wire Net libraries ('general' and 'userial').
//
//
//  Version: 2.00
//

#include "ownet.h"
#include "temp10.h"
#include "findtype.h"
#include "avr_lnk.h"

// defines
#define MAXDEVICES         20

// local functions
void DisplaySerialNum(uchar sn[8]);

//----------------------------------------------------------------------
//  Main Test for DS1920/DS1820 temperature measurement
//
void main(void)
{
    uchar FamilySN[MAXDEVICES][8];
    float current_temp;
    uchar i = 0;
    uchar NumDevices = 0;
    
    //use port number for 1-wire
    uchar portnum = 0;
    
    //initialize USART port (AND Global Interrupt)
    serial0_init(UBRR_9600);
    
    //----------------------------------------
    // Introduction header
    PrintString("\r\nTemperature\r\n");
    
    // attempt to acquire the 1-Wire Net
    if (!owAcquire(portnum,NULL))
    {
        PrintString("Acquire failed\r\n");
        while(owHasErrors())
            printf("  - Error %d\r\n", owGetErrorNum());
        return;
    }
    
    // Find the device(s)
    NumDevices = FindDevices(portnum, FamilySN, 0x10, MAXDEVICES);
    if (NumDevices>0)
    {
        do
        {
            // read the temperature and print serial number and temperature
            for (i = NumDevices; i; i--)
            {
                PrintChar(i-1);
                PrintString(": ");
                DisplaySerialNum(FamilySN[i-1]);
                PrintString("  ");
                
                if( ReadTemperature(portnum, FamilySN[i-1],&current_temp) )
                {
                    PrintFloat(current_temp);
                    PrintString(" C\r\n");
                }
                else
                {
                    PrintString("  Convert failed.  Device is");
                    if(!owVerify(portnum, FALSE))
                        PrintString(" not");
                    PrintString(" present.\r\n");
                    while(owHasErrors())
                    {
                        PrintString("  - Error ");
                        PrintInt( owGetErrorNum() );
                        PrintString("\r\n");
                    }
                }
            }
        }while (1);// (getchar() != 'q');
    }
    else
        PrintString("No temperature devices found!\r\n");
    
    // release the 1-Wire Net
    owRelease(portnum);
    
    while(1);
}

// -------------------------------------------------------------------------------
// Read and print the serial number.
//
void DisplaySerialNum(uchar sn[8])
{
    signed char i;
    for (i = 7; i>=0; i--)
    	PrintChar(sn[i]);
}

