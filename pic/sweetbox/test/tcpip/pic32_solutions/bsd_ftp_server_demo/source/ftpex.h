/********************************************************************
 * FileName:		FTPex.h
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
 * FTP Server
 *
 * Change History:
 * Name				Date            Changes
 * Sean Justice		03/10/06		Initial Version
 ********************************************************************/

/*********************************************************************
 * Function:		PUBLIC BOOL FTPVerify(	BYTE *user, 
 *											BYTE *password)
 *
 * PreCondition:    None.
 *
 * Input:           user		- FTP user name
 *					password	- FTP password
 *
 * Output:          true if the user and password are valid, else false
 *
 * Side Effects:    None
 *
 * Overview:        This funciton looks at the user and password to determine
 *					if they are valid
 ********************************************************************/
PUBLIC BOOL FTPVerify(BYTE *user, BYTE *password);
