/**
 * @brief           Security related code for Modtronix TCP/IP Stack
 * @file            security.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v3.00 or higher <br>
 *                  HITECH PICC-18 8.35PL3 or higher
 * @ingroup         mod_tcpip_security
 *
 * @section description Description
 **********************************
 * Contains TCP/IP security code.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-12-26, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
// TCP/IP Security - documentation module
/**
@defgroup mod_tcpip_security Security
The security.h file contains security related code.
*/


#ifndef SECURITY_H
#define SECURITY_H


/////////////////////////////////////////////////
// User types that can be logged in.
// - Guest = user that has not logged in
// - Admin = user that has logged in
// - Super = super user, has all rights
#define USER_GUEST         (0ul)
#define USER_ADMIN         (1ul)
#define USER_SUPER         (2ul)



#endif
