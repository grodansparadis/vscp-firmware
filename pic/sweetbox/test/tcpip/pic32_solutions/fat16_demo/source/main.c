/*********************************************************************
 *
 *            PIC24 FAT16 Example Application
 *
 *********************************************************************
 * FileName:        FAT16SimpleExample.c
 * Dependencies:    See INCLUDES section below
 * Processor:       Daytona
 * Compiler:
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Log: FAT16SimpleExample.c,v $
 * Revision 1.2  2006/08/15 23:27:44  C12923
 * *** empty log message ***
 *
 * Revision 1.1  2006/03/08 18:27:59  C13015
 * Initial import of the modified FAT16 baseline code; Modifications include path changes to generic.h (now being in ..\..\Common\Include) and clean up of warnings and error during compilation.
 *
 * Revision 1.1  2006/01/26 15:34:11  C10704
 * Used new directory name.
 *
 * Revision 1.1  2006/01/25 23:59:08  C10704
 * Initial.
 *
 ********************************************************************/
#include <string.h>
#include <plib.h>
#include "mstimer.h"
#include "fat16\fat.h"
#include "pic32mx_cfg.def"

#define FS_TEST_FILE_NAME 		"FSTestWR.txt"
#define STD_TEST_FILE_NAME		"STDTestWR.txt"

// Embed device configuration bit settings - these are specific to
// Explorer 16 board. Change it as per your hardware.
// The exact method depends on the compiler in use.

char __Data[1024];


int main(void)
{
	FSFILE *pFile;
	char message[] = "Hello FAT16! FAT16 is up on PIC32!!";
	char readMessage[51];

	// Turn on the interrupts
	INTEnableSystemMultiVectoredInt();

    SYSTEMConfigWaitStatesAndPB(72000000);	
    Ex16LCDInit(36000000);
    Ex16LCDWriteLine(1, "PIC32 FAT16 Demo");    
    Ex16LCDWriteLine(2, "Working...");    


	// First of all initialize the FAT16 library.
	if ( !FSInit() )
	{
		// If failed to initialize the FAT16, set an error LED
		// Primary reasons for failure would be no SD card detected
		// Or badly formatted SD card.
        Ex16LCDWriteLine(2, "Init Error");    
		return -1;
	}

		
	// Now try to create a new file.
	pFile = FSfopen(FS_TEST_FILE_NAME, "w");
	if ( pFile == NULL )
	{
		// If failed to create one, set an error LED
        Ex16LCDWriteLine(2, "Write Open Error");    
		return -1;
	}

	// Now that we have the file created, write something in it.
	if ( FSfwrite(message, sizeof(message), 1, pFile) != 1 )
		// We are writing '1' block of data, and upon completion we must
		// receive '1' or else it is said have failed.
	{
		// Set an error LED and loop here.
        FSfclose(pFile);
        Ex16LCDWriteLine(2, "Write Error");    
        return -1;
	}


	// Close it.
	FSfclose(pFile);

	// Reopen the same file, but now in read mode.
	pFile = FSfopen(FS_TEST_FILE_NAME, "r");
	if ( pFile == NULL )
	{
		// If failed to create one, set an error LED.
        Ex16LCDWriteLine(2, "Read Open Error");    
		return -1;
	}

	memset(readMessage, 0, sizeof(readMessage));
	
	// Now read the line and see if it is same as what we wrote.
	if (!FSfread(readMessage, sizeof(message), 1, pFile))
	{
		// Set an error LED.
		FSfclose(pFile);
        Ex16LCDWriteLine(2, "Read Error");    
        return -1;
	}

	if (strncmp(readMessage, message, sizeof(message)))
	{
        Ex16LCDWriteLine(2, "W/R Error");    
		return -1;
	}


	FSfclose(pFile);


    Ex16LCDWriteLine(2, "W/R Success");

	MSTimerWait(5000);
	
    Ex16LCDWriteLine(2, "Working...");    

	// Now try to create a new file.
	pFile = fopen(STD_TEST_FILE_NAME, "w");
	if ( pFile == NULL )
	{
		// If failed to create one, set an error LED
        Ex16LCDWriteLine(2, "Write Open Error");    
		return -1;
	}

	// Now that we have the file created, write something in it.
	if ( fwrite(message, sizeof(message), 1, pFile) != 1 )
		// We are writing '1' block of data, and upon completion we must
		// receive '1' or else it is said have failed.
	{
		// Set an error LED and loop here.
        fclose(pFile);
        Ex16LCDWriteLine(2, "Write Error");    
        return -1;
	}


	// Close it.
	fclose(pFile);

	// Reopen the same file, but now in read mode.
	pFile = fopen(STD_TEST_FILE_NAME, "r");
	if ( pFile == NULL )
	{
		// If failed to create one, set an error LED.
        Ex16LCDWriteLine(2, "Read Open Error");    
		return -1;
	}

	memset(readMessage, 0, sizeof(readMessage));
	
	// Now read the line and see if it is same as what we wrote.
	if (!fread(readMessage, sizeof(message), 1, pFile))
	{
		// Set an error LED.
		fclose(pFile);
        Ex16LCDWriteLine(2, "Read Error");    
        return -1;
	}

	if (strncmp(readMessage, message, sizeof(message)))
	{
        Ex16LCDWriteLine(2, "W/R Error");    
		return -1;
	}


	fclose(pFile);


    Ex16LCDWriteLine(2, "W/R Success");

	    
    while(1)
        ;

	return 0;

}

