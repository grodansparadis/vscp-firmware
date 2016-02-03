/**********************************************************************
 *
 *                  Helper Functions for Modtronix TCP/IP Stack
 *
 **********************************************************************
 * FileName:        Helpers.C
 * Dependencies:    compiler.h
 *                  helpers.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -E -C
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/17/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     6/25/02 Rewritten CalcIPChecksum() to avoid
 *                              multi-byte shift operation.
 ********************************************************************/

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\helpers.h"
#include "debug.h"
#include "net\mac.h"


#if defined(__C30__) || defined(HI_TECH_C)
/**
 * The function converts an unsigned integer (16 bits) into a null terminated decimal string.
 *
 * @preCondition TCPInit() is already called  AND  MAC buffer pointer set to start of buffer
 *
 * @param Value Unsigned integer to be converted
 *
 * @param Buffer Pointer to a location to write the string
 *
 * @return *Buffer: Receives the resulting string
 */
void itoa(unsigned int Value, char *Buffer)
{
    unsigned char i;
    unsigned int Digit;
    unsigned int Divisor;
    enum {FALSE = 0, TRUE} Printed = FALSE;

    if(Value)
    {
        for(i = 0, Divisor = 10000; i < 5; i++)
        {
            Digit = Value/Divisor;
            if(Digit || Printed)
            {
                *Buffer++ = '0' + Digit;
                Value -= Digit*Divisor;
                Printed = TRUE;
            }
            Divisor /= 10;
        }
    }
    else
    {
        *Buffer++ = '0';
    }

    *Buffer = '\0';
}                
#endif


/**
 * 
 * @param b An ascii byte (always uppercase) between '0'-'9' or 'A'-'F' that
 * corresponds to the lower 4 bits of the input byte.
 * ex: b = 0xAE, btohexa_low() returns 'E'
 *
 * @return the resulting ASCII character
 */
BYTE btohexa_low(BYTE b)
{
    b &= 0x0f;  //Clear upper 4 bits
    return (b>0x9) ? b+'A'-10:b+'0';
}


/**
 * 
 * @param b An ascii byte (always uppercase) between '0'-'9' or 'A'-'F' that
 * corresponds to the upper 4 bits of the input byte.
 * ex: b = 0xAE, btohexa_high() returns 'A'
 *
 * @return the resulting ASCII character
 */
BYTE btohexa_high(BYTE b)
{
    b >>= 4;
    return (b>0x9) ? b+'A'-10:b+'0';
}


WORD swaps(WORD v)
{
    WORD_VAL t;
    BYTE b;

    t.Val   = v;
    b       = t.v[1];
    t.v[1]  = t.v[0];
    t.v[0]  = b;

    return t.Val;
}


DWORD swapl(DWORD v)
{
    BYTE b;
    DWORD myV;
    DWORD_VAL *myP;

    myV     = v;
    myP     = (DWORD_VAL*)&myV;

    b       = myP->v[3];
    myP->v[3] = myP->v[0];
    myP->v[0] = b;

    b       = myP->v[2];
    myP->v[2] = myP->v[1];
    myP->v[1] = b;

    return myV;

}

WORD CalcIPChecksum(BYTE* buffer, WORD count)
{
    WORD i;
    WORD *val;
    DWORD_VAL sum;

    sum.Val = 0;

    val = (WORD *)buffer;

    /////////////////////////////////////////////////
    //Calculate sum of all 16 bit words
    i = count >> 1;
    while( i-- )
        sum.Val += *val++;

    //If count is an uneven number, add last byte
    if ( count & 1 )
        sum.Val += *(BYTE *)val;

    
    /////////////////////////////////////////////////
    // take only 16 bits out of the 32 bit sum
    while (sum.word.MSW != 0) {
        sum.Val = (DWORD)sum.word.LSW + (DWORD)sum.word.MSW;
    }

    //Return one's complement
    return (~sum.word.LSW);
}


/**
 * This function performs checksum calculation in MAC buffer itself.
 *
 * @preCondition TCPInit() is already called  AND  MAC buffer pointer set to start of buffer
 * @param len Total number of bytes to calculate checksum for
 * @return 16-bit checksum as defined by rfc 793
 */
#if defined(NON_MCHP_MAC)
WORD CalcIPBufferChecksum(WORD len)
{
    #define CRC_BUF_SIZE 64ul
    BOOL lbMSB;
    WORD_VAL checkSum;
    BYTE c;
    BYTE i;
    BYTE bytesRead;
    BYTE buf[CRC_BUF_SIZE];

    lbMSB = TRUE;
    checkSum.Val = 0;

    while(len)
    {
        //Determine how many byte we should read, and read them from the current MAC Receive Buffer
        bytesRead = len>=CRC_BUF_SIZE ? CRC_BUF_SIZE : len;
        MACGetArray(buf, bytesRead);
        len -= bytesRead;

        //Calculate checksum for read bytes
        for (i = 0; i < bytesRead; i++) {

            c = buf[i];

            if ( !lbMSB )
            {
                if ( (checkSum.v[0] = c + checkSum.v[0]) < c)
                {
                    if ( ++checkSum.v[1] == 0 )
                        checkSum.v[0]++;
                }
            }
            else
            {
                if ( (checkSum.v[1] = c + checkSum.v[1]) < c)
                {
                    if ( ++checkSum.v[0] == 0 )
                        checkSum.v[1]++;
                }
            }

            lbMSB = !lbMSB;
        }
    }

    checkSum.v[1] = ~checkSum.v[1];
    checkSum.v[0] = ~checkSum.v[0];
    return checkSum.Val;
}
#endif

/**
 * Get the hex value for the given alpha numrical character. The returned value will be from
 * 0 - 15. If the given byte is not a alphe numerical value, 0xff is returned!
 */
BYTE getHexNibble(BYTE c) {
    if ((c<='9') && (c>='0'))
        return (c - '0');
    else if ((c<='F')&&(c>='A'))
        return (c - 55);
    else if ((c<='f')&&(c>='a'))
        return (c - 87);
        
    return 0xff;    //Indicate given byte was NOT alpha numerical
}

/**
 * Converts the given 2 character hex string to a byte value. Only the first two character of the given
 * string is checked. No NULL terminating character is required.
 * 
 * @param s Input string. First two characters of string must contain hex string. For example "8F" or "8f"
 * @param c Will contain the byte representation of the given string on return.
 *
 * @return 1 if a valid hex value was found, or 0 if s did NOT contain a valid hex value
 */
BYTE hextob(char * s, BYTE * c) {
    BYTE lsb;
    BYTE msb;

    lsb = getHexNibble(s[1]);
    msb = getHexNibble(s[0]);
    
    if ((lsb == 0xff) || (msb == 0xff))
        return 0;   //Return error

    *c = ((msb << 4) | lsb);
    
    return 1;       //Return OK, value is in c
}


#if defined(HITECH_C18)
/**
 * Convert all lower case characters in a string to upper case.
 * This function converts all lower case characters in str to upper case characters.
 * All characters that are not lower case (a to z) are not affected.
 * @param s Pointer to NULL terminated string
 * @return This function returns the given string, containing the converted string
 */
char *strupr (char *s)
{
    char c;
    char *t;

    t = s;
    while( (c = *t) )
    {
        if ( (c >= 'a' && c <= 'z') )
            *t -= ('a' - 'A');
    t++;
    }
    return s;
}
#endif


#if defined(HITECH_C18)
void *mymemcpy(void * d1, const void * s1, register unsigned char n)
{

    register char *     d;
    register const char *   s;

    s = s1;
    d = d1;
    while(n--)
        *d++ = *s++;
    return d1;
}
#endif


/**
 * Copies the source string (in RAM) to the given destination in EEPROM. 
 * Characters in strSrc are copied up to, and including, the terminating null character
 * strSrc.
 *
 * @param strSrc Pointer to null terminated Source string contained in RAM.
 * @param strDst Destination address (offset) in the EEPROM
 * @param len The maximum string length that is allowed to be written (excluding NULL terminating char).
 *            If the source string is longer then this value, the destination will be truncated
 *            with a NULL! To disable length checking, set this value to 255.
 *            For example, if len = 8, then that string can have a maximum of 8 characters = 8 + NULL.
 *            The destination must always have len + 1 positions available!
 *
 * @return Returns the number of bytes written, excluding the terminating null. For example, if
 * 8 is returned, then 8 characters + NULL terminating char were written.
 */
BYTE strcpyram2ee(WORD strDst, char * strSrc, BYTE len) {
    BYTE bytesWritten;
    bytesWritten = 0;
    
    //Wait if something is currently being written
    while(EECON1_WR) FAST_USER_PROCESS();

    //Configure for EEPROM writing, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x2F;

    EECON1_WREN = 1;        //Write enable

    //Write whole string, including NULL character
    do {
        //Get data to be written
        EEDATA = (BYTE)*strSrc++;
        
        //If len exceeded, truncate string and exit loop
        if (++bytesWritten > len) {
            EEDATA = '\0';
        }
        
        //Set address of EEPROM, and increment destination address
        EEADRH = (BYTE)(strDst >> 8);
        EEADR = (BYTE)(strDst++);

        EECON2      = 0x55;
        EECON2      = 0xAA;
        EECON1_WR   = 1;    //Initiate write

        Nop();

        //Wait for write to finish
        while(EECON1_WR) FAST_USER_PROCESS();
    } while (EEDATA != '\0');
    
    //Disable Write Enable
    EECON1_WREN = 0;
    
    return bytesWritten-1;
}


/**
 * Copies the source string (in EEPROM) to the given destination in buffer in RAM.
 * Characters in strSrc are copied up to, and including, the terminating null character
 * strSrc.
 *
 * @param strDst Pointer to RAM buffer where EEPROM string will be copied to
 * @param strSrc Source address (offset) in the EEPROM
 * @param len The maximum string length that is allowed to be written (excluding NULL terminating char).
 *            If the source string is longer then this value, the destination will be truncated
 *            with a NULL!
 *            For example, if len = 8, then that string can have a maximum of 8 characters = 8 + NULL.
 *            The destination must always have len + 1 positions available!
 *
 * @return Returns the number of bytes written, excluding the terminating null. For example, if
 * 8 is returned, then 8 characters + NULL terminating char were written.
 */
BYTE strcpyee2ram(char * strDst, WORD strSrc, BYTE len) {
    BYTE bytesWritten;
    bytesWritten = 0;
    
    //Wait if something is currently being written
    while(EECON1_WR) FAST_USER_PROCESS();

    //Configure for EEPROM reading, EEPGD=0, CFGS=0
    EECON1 &= 0x3F;

    //Write whole string, including NULL character
    do {
        //If len exceeded, truncate string and exit loop
        if (++bytesWritten > len) {
            *strDst = '\0'; //Truncate with NULL, and break
            break;
        }

        //Get next byte from EEPROM
        EEADR = (BYTE)strSrc;
        EEADRH = (BYTE)(strSrc>>8);
        strSrc++;

        EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.

        //Save byte to destination
        *strDst = EEDATA;
        strDst++;
    } while (EEDATA != '\0');
    
    return bytesWritten-1;
}


/**
 * Compare two strings.
 *
 * @param strRam Pointer to null terminated string contained in RAM
 * @param strEe Address (offset) in EEPROM of null terminated string
 *
 * @return Returns 0 if both strings are the same, else non 0
 */
BYTE strcmpee2ram(char * strRam, WORD strEe) {
    char c;
    
    //Set address of EEPROM
    EEADRH = (BYTE)(strEe >> 8);
    EEADR = (BYTE)(strEe);

    //Configure for EEPROM reading, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x3F;

    /*    
    #if (DEBUG_GEN >= LOG_INFO)
    debugPutGenMsg(6);     //@mxd:6:%s%s
    //First NULL terminated string
    debugPutRomString((ROM char*)"strcmpee2ram() received strRam = ");
    //Second NULL terminated string
    debugPutString(strRam);
    #endif
    */

    while (1) {
        c = *strRam++;
        EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
      
        /*
        #if (DEBUG_GEN >= LOG_INFO)
        debugPutGenMsg(6);     //@mxd:6:%s%s
        //First NULL terminated string
        debugPutRomString((ROM char*)"Comparing ee2ram, ram=");
        //Second NULL terminated string
        debugPutByteHex(c);
        debugPutRomStringXNull((ROM char*)", ee=");
        debugPutByteHex(EEDATA);
        debugPutByte('\0');
        #endif
        */

        //If not equal, return 1
        if (c != EEDATA) return 1;
        
        //c and EEDATA are both = NULL. Both strings are the same!
        if (c == '\0') return 0;

        if (++EEADR == 0) {
            EEADRH++; //Increment EEPROM address
        }
    }

    return 1;
}


/**
 * Test if given string in RAM begins with given ROM string, and Ignores Case.
 * The ROM string has to be in upper case. 
 *
 * @param strRam Pointer to string contained in RAM. This tring does not have to be NULL
 *               terminated.
 * @param strRom Pointer to null terminated string contained in ROM
 *
 * @return Returns TRUE if RAM string begins with ROM string
 */
BOOL strBeginsWithIC(char * strRam, ROM char * strRom) {
    char c;

    while ( (c = *strRam++) != '\0') {
        //If lower case, change to upper case
        if ((c >= 'a') && (c <= 'z')) {
            c -= ('a' - 'A');
        }

        //If not equal, return false
        if (c != *strRom++) {
            return FALSE;
        }
        
        //If end of ROM string reached, then RAM start with ROM string. Return TRUE!
        if (*strRom == '\0') {
            return TRUE;
        }
    }
    
    //Reached end of RAM string before end of ROM string, RAM string does NOT start with ROM string!
    return FALSE;
}


/**
 * Fills the given number of bytes in the given array with 0
 *
 * @param dest Pointer to memory area in RAM that has to be set to 0
 * @param size number of consecutive bytes to set to 0
 */
void memclr(void * dest, WORD size) {
    WORD i;
    BYTE * p;
    p = (BYTE *)dest;
    
    for (i = 0; i < size; i++) {
        *p++ = 0;
    }   
}

/* String used for encoding to base64 */
static ROM char base64EncStr[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Translation table for decoding to asscii*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/**
 * Decode Block of of 4 6-bit base64 characters to 3 ascii characters (8 bit)
 *
 */
/*
void base64DecodeBlock(BYTE in[4], BYTE out[3] )
{   
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}
*/

/**
 * Decode given 6-bit base64 characters to ASCII
 *
 * @param in    Buffer containing 6-bit base64 bytes
 * @param inLen Length of input bytes
 * @param out   Buffer where resulting decoded ASCII string will be written and NULL terminated
 */
void base64Decode(char * in, BYTE inLen, char * out)
{
    /*
    unsigned char in[4], out[3], v;
    int i, len;

    while( !feof( infile ) ) {
        for( len = 0, i = 0; i < 4 && !feof( infile ); i++ ) {
            v = 0;
            while( !feof( infile ) && v == 0 ) {
                v = (unsigned char) getc( infile );
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if( !feof( infile ) ) {
                len++;
                if( v ) {
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = 0;
            }
        }
        if( len ) {
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) {
                putc( out[i], outfile );
            }
        }
    }
    */
}

  
/**
 * Encode block of 3 ascii (8 bit) characters to 4 base64 6-bit characters
 */
/*
void base64Encodeblock( BYTE in[3], BYTE out[4], BYTE len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}
*/

/**
 * Encode given ascii characters to base64 6-bit characters, adding padding characters where required
 *
 * @param out   Buffer where output is written to
 * @param in    Input ascii string
 * @param inlen Length of input ascii string, excluding NULL terminating character
 */
void base64Encode(char *out, char *in, BYTE inlen)
{
    for (; inlen >= 3; inlen -= 3) {
        *out++ = base64EncStr[in[0] >> 2];
        *out++ = base64EncStr[((in[0] << 4) & 0x30) | (in[1] >> 4)];
        *out++ = base64EncStr[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
        *out++ = base64EncStr[in[2] & 0x3f];
        in += 3;
    }

    if (inlen > 0)
    {
        unsigned char fragment;

        *out++ = base64EncStr[in[0] >> 2];
        fragment = (in[0] << 4) & 0x30;

        if (inlen > 1)
            fragment |= in[1] >> 4;

        *out++ = base64EncStr[fragment];
        *out++ = (inlen < 2) ? '=' : base64EncStr[(in[1] << 2) & 0x3c];
        *out++ = '=';
    }

    *out = '\0';

    /*
    unsigned char in[3], out[4];
    int i, len, blocksout = 0;

    while( !feof( infile ) ) {
        len = 0;
        for( i = 0; i < 3; i++ ) {
            in[i] = (unsigned char) getc( infile );
            if( !feof( infile ) ) {
                len++;
            }
            else {
                in[i] = 0;
            }
        }
        if( len ) {
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ ) {
                putc( out[i], outfile );
            }
            blocksout++;
        }
        if( blocksout >= (linesize/4) || feof( infile ) ) {
            if( blocksout ) {
                fprintf( outfile, "\r\n" );
            }
            blocksout = 0;
        }
    }
    */
}

