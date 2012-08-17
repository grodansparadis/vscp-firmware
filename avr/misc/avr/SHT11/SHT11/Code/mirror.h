/**
 * @file   
 * @author Alex Raimondi 
 * @date   8. 6. 2002
 * 
 * @brief  This file contains only the C-Declaration of mirrorByte
 * 
 * Mirror byte is written in assembler.
 */


#ifndef MIRROR_H
#define MIRROR_H

/**
 * @brief	Mirrors a byte 
 *
 * The byte gets mirrored. This means exchanging bit 7 with bit 0, 
 * bit 6 with bit 1 and so on.
 *
 * @param	value	the byte to be mirrored
 *
 * @return	mirrored value
*/
extern uint8 mirrorByte( uint8 value );

#endif
