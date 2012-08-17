/**************************************************************************
 *
 * FILE NAME:			mcp3221.h
 * FILE DESCRIPTION:	MCP3221 (High-res A2D) driver interface
 *
 * FILE CREATION DATE:	23-11-2003
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,23nov03 erd
 *
 ***************************************************************************/

#ifndef __MCP3221_H_
#define __MCP3221_H_

// ==================================================================
// interfaces

// mcp3221_init()
//
// Initialize the high resolution A2D module
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

extern BOOL mcp3221_init();

// mcp3221_read()
//
// Performs A2D 
//
// Arguments:
//	value: Value to be filled out with acquired value. 12 bit wide
//
// Return value:
//	TRUE if acquisition ok, FALSE if failed

extern BOOL mcp3221_read(uint_16 *value);

#endif /* __MCP3221_H_ */
