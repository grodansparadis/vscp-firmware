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
/*********************************************************************
** This port of the OneWire libraries for wizC and fedC is created by:
** (C) 2006 HMCS / Marcel van Lieshout
**
** wizC/fedC are (C) Forest Electronic Developments.
*********************************************************************/
//
//  OWnet.c - Network functions for 1-Wire net devices.
//
//  Version: 3.00
//

#include "ownet.h"

// exportable functions defined in wizCnet.c
BYTE bitacc(BYTE op, BYTE state, BYTE loc, BYTE *buf);

//--------------------------------------------------------------------------
// The 'owFirst' finds the first device on the 1-Wire Net  This function
// contains one parameter 'alarm_only'.  When
// 'alarm_only' is TRUE (1) the find alarm command 0xEC is
// sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                        Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): There are no devices on the 1-Wire Net.
//
BYTE owFirst(BYTE portnum, BYTE do_reset, BYTE alarm_only)
{
   owSetCurrentPort(portnum);
   
   // reset the search state
   owNetCurrent.LastDiscrepancy       = 0;
   owNetCurrent.LastDevice            = FALSE;
   owNetCurrent.LastFamilyDiscrepancy = 0;

   return owNext(owCurrentPortnum, do_reset, alarm_only);
}

//--------------------------------------------------------------------------
// The 'owNext' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'owFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'do_reset'   - TRUE (1) perform reset before search, FALSE (0) do not
//                perform reset before search.
// 'alarm_only' - TRUE (1) the find alarm command 0xEC is
//                sent instead of the normal search command 0xF0
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global SerialNum[portnum]
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
BYTE owNext(BYTE portnum, BYTE do_reset, BYTE alarm_only)
{
   BYTE bit_test, search_direction, bit_number;
   BYTE last_zero, serial_byte_number, next_result;
   BYTE serial_byte_mask;
   BYTE lastcrc8;

   owSetCurrentPort(portnum);

   // initialize for search
   bit_number = 1;
   last_zero = 0;
   serial_byte_number = 0;
   serial_byte_mask = 1;
   next_result = 0;
   setcrc8(portnum, 0);

   // if the last call was not the last one
   if(!owNetCurrent.LastDevice) {
     // check if reset first is requested
     if(do_reset) {
       // reset the 1-wire
       // if there are no parts on 1-wire, return FALSE
       if(!owTouchReset(owCurrentPortnum)) {
         // reset the search
         owNetCurrent.LastDiscrepancy       = 0;
         owNetCurrent.LastFamilyDiscrepancy = 0;
         OWERROR(OWERROR_NO_DEVICES_ON_NET);
         return FALSE;
       }
     }

     // If finding alarming devices issue a different command
     if(alarm_only) {
       owWriteByte(owCurrentPortnum, 0xEC);  // issue the alarming search command
     } else {
       owWriteByte(owCurrentPortnum, 0xF0);  // issue the search command
     }

     //pause before beginning the search
     //usDelay(100);

     // loop to do the search
     do {
       // read a bit and its compliment
       bit_test  = owTouchBit(owCurrentPortnum, 1) << 1;
       bit_test |= owTouchBit(owCurrentPortnum, 1);

       // check for no devices on 1-wire
       if(bit_test == 3) {
         break;
       }

       // all devices coupled have 0 or 1
       if(bit_test > 0) {
         search_direction = !(bit_test & 0x01);  // bit write value for search
       } else {
         // if this discrepancy if before the Last Discrepancy
         // on a previous next then pick the same as last time
         if(bit_number < owNetCurrent.LastDiscrepancy) {
           search_direction = ((owNetCurrent.SerialNum[serial_byte_number] & serial_byte_mask) > 0);
         } else {
           // if equal to last pick 1, if not then pick 0
           search_direction = (bit_number == owNetCurrent.LastDiscrepancy);
         }
         
         // if 0 was picked then record its position in LastZero
         if(search_direction == 0) {
           last_zero = bit_number;

           // check for Last discrepancy in family
           if(last_zero < 9) {
             owNetCurrent.LastFamilyDiscrepancy = last_zero;
           }
         }
       }

       // set or clear the bit in the SerialNum byte serial_byte_number
       // with mask serial_byte_mask
       if(search_direction == 1) {
         owNetCurrent.SerialNum[serial_byte_number] |= serial_byte_mask;
       } else {
         owNetCurrent.SerialNum[serial_byte_number] &= ~serial_byte_mask;
       }
       
       // serial number search direction write bit
       owTouchBit(owCurrentPortnum, search_direction);

       // increment the byte counter bit_number
       // and shift the mask serial_byte_mask
       bit_number++;
       serial_byte_mask <<= 1;

       // if the mask is 0 then go to new SerialNum byte serial_byte_number
       // and reset mask
       if(serial_byte_mask == 0) {
         lastcrc8 = docrc8(owCurrentPortnum, owNetCurrent.SerialNum[serial_byte_number]);  // accumulate the CRC
         serial_byte_number++;
         serial_byte_mask = 1;
       }
     } while(serial_byte_number < 8);  // loop until through all SerialNum bytes 0-7

     // if the search was successful then
     if(!((bit_number < 65) || lastcrc8)) {
       // search successful so set LastDiscrepancy, LastDevice, next_result
       owNetCurrent.LastDiscrepancy = last_zero;
       if(last_zero == 0) {
         owNetCurrent.LastDevice = TRUE;
       } else {
         owNetCurrent.LastDevice = FALSE;
       }
//         owNetCurrent.LastDevice = (last_zero == 0);
          
       next_result = TRUE;
     }
   }

   // if no device found then reset counters so next 'next' will be
   // like a first
   if(!next_result || !owNetCurrent.SerialNum[0]) {
     owNetCurrent.LastDiscrepancy = 0;
     owNetCurrent.LastDevice = FALSE;
     owNetCurrent.LastFamilyDiscrepancy = 0;
     next_result = FALSE;
   }

   return next_result;
}

//--------------------------------------------------------------------------
// The 'owSerialNum' function either reads or sets the SerialNum buffer
// that is used in the search functions 'owFirst' and 'owNext'.
// This function contains two parameters, 'serialnum_buf' is a pointer
// to a buffer provided by the caller.  'serialnum_buf' should point to
// an array of 8 unsigned chars.  The second parameter is a flag called
// 'do_read' that is TRUE (1) if the operation is to read and FALSE
// (0) if the operation is to set the internal SerialNum buffer from
// the data in the provided buffer.
//
// 'portnum'       - number 0 to MAX_PORTNUM-1.  This number is provided to
//                   indicate the symbolic port number.
// 'serialnum_buf' - buffer to that contains the serial number to set
//                   when do_read = FALSE (0) and buffer to get the serial
//                   number when do_read = TRUE (1).
// 'do_read'       - flag to indicate reading (1) or setting (0) the current
//                   serial number.
//
void owSerialNum(BYTE portnum, BYTE *serialnum_buf, BYTE do_read)
{
   BYTE i;

   owSetCurrentPort(portnum);
   
   // read the internal buffer and place in 'serialnum_buf'
   if(do_read) {
     for(i = 0; i < 8; i++) {
       serialnum_buf[i] = owNetCurrent.SerialNum[i];
     }
   } else {
     // set the internal buffer from the data in 'serialnum_buf'
     for(i = 0; i < 8; i++) {
       owNetCurrent.SerialNum[i] = serialnum_buf[i];
     }
   }
}  

//--------------------------------------------------------------------------
// Setup the search algorithm to find a certain family of devices
// the next time a search function is called 'owNext'.
//
// 'portnum'       - number 0 to MAX_PORTNUM-1.  This number was provided to
//                   OpenCOM to indicate the port number.
// 'search_family' - family code type to set the search algorithm to find
//                   next.
//
void owFamilySearchSetup(BYTE portnum, BYTE search_family)
{
   BYTE i;

   owSetCurrentPort(portnum);
   
   // set the search state to find SearchFamily type devices
   owNetCurrent.SerialNum[0] = search_family;
   for(i = 1; i < 8; i++) {
     owNetCurrent.SerialNum[i] = 0;
   }
   owNetCurrent.LastDiscrepancy       = 64;
   owNetCurrent.LastDevice            = FALSE;
   owNetCurrent.LastFamilyDiscrepancy = 0;
}

//--------------------------------------------------------------------------
// Set the current search state to skip the current family code.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
//
void owSkipFamily(BYTE portnum)
{
   owSetCurrentPort(portnum);
   
   // set the Last discrepancy to last family discrepancy
   owNetCurrent.LastDiscrepancy       = owNetCurrent.LastFamilyDiscrepancy;
   owNetCurrent.LastFamilyDiscrepancy = 0;

   // check for end of list
   if(owNetCurrent.LastDiscrepancy == 0) {
     owNetCurrent.LastDevice = TRUE;
   }
}

//--------------------------------------------------------------------------
// The 'owAccess' function resets the 1-Wire and sends a MATCH Serial
// Number command followed by the current SerialNum code. After this
// function is complete the 1-Wire device is ready to accept device-specific
// commands.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
//
// Returns:   TRUE (1) : reset indicates present and device is ready
//                       for commands.
//            FALSE (0): reset does not indicate presence or echos 'writes'
//                       are not correct.
//
BYTE owAccess(BYTE portnum)
{
   BYTE sendpacket[9];
   BYTE i;

   owSetCurrentPort(portnum);

   // reset the 1-wire
   if(owTouchReset(owCurrentPortnum)) {
     // create a buffer to use with block function
     // match Serial Number command 0x55
     sendpacket[0] = 0x55;
     // Serial Number
     for(i = 1; i < 9; i++) {
       sendpacket[i] = owNetCurrent.SerialNum[i-1];
     }

     // send/recieve the transfer buffer
     if(owBlock(owCurrentPortnum, FALSE, sendpacket, 9)) {
       // verify that the echo of the writes was correct
       for(i = 1; i < 9; i++) {
         if(sendpacket[i] != owNetCurrent.SerialNum[i-1]) {
           return FALSE;
         }
         if(sendpacket[0] != 0x55) {
           OWERROR(OWERROR_WRITE_VERIFY_FAILED);
           return FALSE;
         } else {
           return TRUE;
         }
       }
     } else {
       OWERROR(OWERROR_BLOCK_FAILED);
     }
   } else {
     OWERROR(OWERROR_NO_DEVICES_ON_NET);
   }

   // reset or match echo failed
   return FALSE;
}

//----------------------------------------------------------------------
// The function 'owVerify' verifies that the current device
// is in contact with the 1-Wire Net.
// Using the find alarm command 0xEC will verify that the device
// is in contact with the 1-Wire Net and is in an 'alarm' state.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
// 'alarm_only'  - TRUE (1) the find alarm command 0xEC
//                 is sent instead of the normal search
//                 command 0xF0.
//
// Returns:   TRUE (1) : when the 1-Wire device was verified
//                       to be on the 1-Wire Net
//                       with alarm_only == FALSE
//                       or verified to be on the 1-Wire Net
//                       AND in an alarm state when
//                       alarm_only == TRUE.
//            FALSE (0): the 1-Wire device was not on the
//                       1-Wire Net or if alarm_only
//                       == TRUE, the device may be on the
//                       1-Wire Net but in a non-alarm state.
//
BYTE owVerify(BYTE portnum, BYTE alarm_only)
{
   BYTE i, sendlen = 0, goodbits = 0, cnt = 0, s, tst;
   BYTE sendpacket[50];

   owSetCurrentPort(portnum);
   
   // construct the search
   if(alarm_only) {
     sendpacket[sendlen++] = 0xEC; // issue the alarming search command
   } else {
     sendpacket[sendlen++] = 0xF0; // issue the search command
   }
   
   // set all bits at first
   for(i = 1; i <= 24; i++) {
     sendpacket[sendlen++] = 0xFF;
   }

   // now set or clear apropriate bits for search
   for(i = 0; i < 64; i++) {
     bitacc(WRITE_FUNCTION, bitacc(READ_FUNCTION, 0, i, &owNetCurrent.SerialNum[0]),(int)((i+1)*3-1),&sendpacket[1]);
   }
   
   // send/recieve the transfer buffer
   if(owBlock(owCurrentPortnum, TRUE, sendpacket, sendlen)) {
     // check results to see if it was a success
     for(i = 0; i < 192; i += 3) {
       tst = (bitacc(READ_FUNCTION, 0, i, &sendpacket[1]) << 1) |
              bitacc(READ_FUNCTION, 0, (int)(i+1), &sendpacket[1]);

       s = bitacc(READ_FUNCTION, 0, cnt++, &owNetCurrent.SerialNum[0]);

       if(tst == 0x03) {      // no device on line
         goodbits = 0;        // number of good bits set to zero
         break;               // quit
       }

       if(((s == 0x01) && (tst == 0x02)) ||
          ((s == 0x00) && (tst == 0x01))) {  // correct bit
         goodbits++;                       // count as a good bit
       }
     }

     // check too see if there were enough good bits to be successful
     if(goodbits >= 8) {
       return TRUE;
     }
   } else {
     OWERROR(OWERROR_BLOCK_FAILED);
   }

   // block fail or device not present
   return FALSE;
}

//----------------------------------------------------------------------
// Perform a overdrive MATCH command to select the 1-Wire device with
// the address in the ID data register.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
//
// Returns:  TRUE: If the device is present on the 1-Wire Net and
//                 can do overdrive then the device is selected.
//           FALSE: Device is not present or not capable of overdrive.
//
//  *Note: This function could be converted to send DS2480
//         commands in one packet.
//
BYTE owOverdriveAccess(BYTE portnum)
{
   BYTE sendpacket[8];
   BYTE i, bad_echo = FALSE;

   owSetCurrentPort(portnum);

   // make sure normal level
   owLevel(owCurrentPortnum, MODE_NORMAL);

   // force to normal communication speed
   owSpeed(owCurrentPortnum, MODE_NORMAL);

   // call the 1-Wire Net reset function
   if(owTouchReset(owCurrentPortnum)) {
     // send the match command 0x69
     if(owWriteByte(owCurrentPortnum, 0x69)) {
       // switch to overdrive communication speed
       owSpeed(owCurrentPortnum, MODE_OVERDRIVE);

       // create a buffer to use with block function
       // Serial Number
       for(i = 0; i < 8; i++) {
         sendpacket[i] = owNetCurrent.SerialNum[i];
       }
       
       // send/recieve the transfer buffer
       if(owBlock(owCurrentPortnum, FALSE, sendpacket,8)) {
         // verify that the echo of the writes was correct
         for(i = 0; i < 8; i++) {
           if(sendpacket[i] != owNetCurrent.SerialNum[i]) {
             bad_echo = TRUE;
           }
         }
         // if echo ok then success
         if(!bad_echo) {
           return TRUE;
         } else {
           OWERROR(OWERROR_WRITE_VERIFY_FAILED);
         }
       } else {
         OWERROR(OWERROR_BLOCK_FAILED);
       }
     } else {
       OWERROR(OWERROR_WRITE_BYTE_FAILED);
     }
   } else {
     OWERROR(OWERROR_NO_DEVICES_ON_NET);
   }

   // failure, force back to normal communication speed
   owSpeed(owCurrentPortnum, MODE_NORMAL);

   return FALSE;
}

//--------------------------------------------------------------------------
// Bit utility to read and write a bit in the buffer 'buf'.
//
// 'op'    - operation (1) to set and (0) to read
// 'state' - set (1) or clear (0) if operation is write (1)
// 'loc'   - bit number location to read or write
// 'buf'   - pointer to array of bytes that contains the bit
//           to read or write
//
// Returns: 1   if operation is set (1)
//          0/1 state of bit number 'loc' if operation is reading
//
BYTE bitacc(BYTE op, BYTE state, BYTE loc, BYTE *buf)
{
   BYTE nbyt, nbit;

   nbyt = (loc / 8);
   nbit = loc - (nbyt * 8);

   if(op == WRITE_FUNCTION) {
     if(state) {
       buf[nbyt] |= (0x01 << nbit);
     } else {
       buf[nbyt] &= ~(0x01 << nbit);
     }
     return 1;
   }

   return ((buf[nbyt] >> nbit) & 0x01);
}
