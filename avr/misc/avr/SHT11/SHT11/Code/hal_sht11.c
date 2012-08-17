
#include "hal_sht11.h"

const uint8 PROGMEM crcTable[] = {
	0, 49, 98, 83, 196, 245, 166, 151, 185, 136, 219, 234, 125, 76, 31, 46, 67, 114, 33, 16,
	135, 182, 229, 212, 250, 203, 152, 169, 62, 15, 92, 109, 134, 183, 228, 213, 66, 115, 32, 17,
	63, 14, 93, 108, 251, 202, 153, 168, 197, 244, 167, 150, 1, 48, 99, 82, 124, 77, 30, 47,
	184, 137, 218, 235, 61, 12, 95, 110, 249, 200, 155, 170, 132, 181, 230, 215, 64, 113, 34, 19,
	126, 79, 28, 45, 186, 139, 216, 233, 199, 246, 165, 148, 3, 50, 97, 80, 187, 138, 217, 232,
	127, 78, 29, 44, 2, 51, 96, 81, 198, 247, 164, 149, 248, 201, 154, 171, 60, 13, 94, 111,
	65, 112, 35, 18, 133, 180, 231, 214, 122, 75, 24, 41, 190, 143, 220, 237, 195, 242, 161, 144,
	7, 54, 101, 84, 57, 8, 91, 106, 253, 204, 159, 174, 128, 177, 226, 211, 68, 117, 38, 23,
	252, 205, 158, 175, 56, 9, 90, 107, 69, 116, 39, 22, 129, 176, 227, 210, 191, 142, 221, 236,
	123, 74, 25, 40, 6, 55, 100, 85, 194, 243, 160, 145, 71, 118, 37, 20, 131, 178, 225, 208,
	254, 207, 156, 173, 58, 11, 88, 105, 4, 53, 102, 87, 192, 241, 162, 147, 189, 140, 223, 238,
	121, 72, 27, 42, 193, 240, 163, 146, 5, 52, 103, 86, 120, 73, 26, 43, 188, 141, 222, 239,
	130, 179, 224, 209, 70, 119, 36, 21, 59, 10, 89, 104, 255, 206, 157, 172 };

static uint8 crcValue;
static uint8 sht11StatusReg = 0;

void halSht11Init( void )
{
	// SCK line as output but set to low first
	cbi( SHT11_SCK_PORT, SHT11_SCK_BIT ); 
	sbi( SHT11_SCK_DDR, SHT11_SCK_BIT ); 

	// DATA to input. External pull up.
	// Set PORT to 0 => pull data line low by setting port as output
	cbi( SHT11_DATA_PORT, SHT11_DATA_BIT ); 
	cbi( SHT11_DATA_DDR, SHT11_DATA_BIT ); 
}

void halSht11TransmitionStart( void )
{
	SHT11_SCK_HI();
	SHT11_DATA_LO();
	SHT11_SCK_LO();
	SHT11_SCK_HI();
	SHT11_DATA_HI();
	SHT11_SCK_LO();

	// Reset crc calculation. Start value is the content
	// of the status register.
	crcValue = mirrorByte( sht11StatusReg & 0x0F );  
}

uint8 halSht11ReadByte( bool sendAck )
{
	uint8	u8Mask; 
	uint8	value = 0;
	// SCK is low!

	u8Mask = 0x80;
	while ( u8Mask != 0 )
	{
		// SCK hi 
		SHT11_SCK_HI();
		// and read data
		if ( SHT11_GET_BIT() != 0 )
			value |= u8Mask;
		
		// SCK lo => sensor puts new data
		SHT11_SCK_LO();
		u8Mask >>= 1;
	}

	if ( sendAck )
	{
		// Get DATA line
		SHT11_DATA_LO();
		// give a clock pulse
		SHT11_SCK_HI();
		SHT11_SCK_LO();
		// Release DATA line
		SHT11_DATA_HI();
	}
	else
	{
		// give a clock pulse
		SHT11_SCK_HI();
		SHT11_SCK_LO();
	}

	return value;
}

bool halSht11SendByte( uint8 value )
{
	uint8	u8Mask;
	bool	ack;

	u8Mask = 0x80;
	while ( u8Mask != 0 )
	{
		SHT11_SCK_LO();

		if ( (value & u8Mask) != 0 )
			SHT11_DATA_HI();
		else
			SHT11_DATA_LO();
		
		// SCK hi => sensor reads data
		SHT11_SCK_HI();
		u8Mask >>= 1;
	}
	SHT11_SCK_LO();

	// Release DATA line
	SHT11_DATA_HI();
	SHT11_SCK_HI();
	// Read acknowlege
	ack = SHT11_GET_BIT() == 0 ? TRUE : FALSE;
	SHT11_SCK_LO();

	// crc calculation
	crcValue = PRG_RDB(&crcTable[crcValue ^ value]);

	return ack;
}

bool halSht11StartMeasure( sht11MeasureType type )
{
	// send a transmtion start and reset crc calculation
	halSht11TransmitionStart();

	// send command. Crc gets updated!
	return halSht11SendByte( (uint8)type );
}

bool halSht11GetMeaValue( uint16 * value )
{
	uint8 * chPtr = (uint8*)value;
	uint8 checksum;
	
	// Wait for measurement to complete
	while ( !halSht11ValueReady() );

	// read hi byte
	*(chPtr + 1) = halSht11ReadByte( TRUE );
	// crc calculation
	crcValue = PRG_RDB(&crcTable[crcValue ^ *(chPtr + 1)]);

	// read lo byte
	*chPtr = halSht11ReadByte( TRUE );
	// crc calculation
	crcValue = PRG_RDB(&crcTable[crcValue ^ *chPtr]);

	// get checksum
	checksum = halSht11ReadByte( FALSE );
	// compare it. 
	return mirrorByte( checksum ) == crcValue ? TRUE : FALSE;
}

bool halSht11WriteStatus( uint8 value )
{
	bool ack;

	// send a transmtion start and reset crc calculation
	halSht11TransmitionStart();

	// send command. Crc gets updated!
	ack = halSht11SendByte( SHT11_STATUS_W );

	// send new status register value
	ack &= halSht11SendByte( value );

	// if everything ok
	if ( ack == TRUE )
		// Store new value. This is necessary since
		// contents of status register is initial value
		// of crc calculation.
		sht11StatusReg = value;

	return ack;
}

bool halSht11ReadStatus( uint8 *value )
{
	uint8 checksum;

	// send a transmtion start and reset crc calculation
	halSht11TransmitionStart();

	// send command. Crc gets updated!
	halSht11SendByte( SHT11_STATUS_R );

	// read status register value. TRUE => send Ack
	*value = halSht11ReadByte( TRUE );
	crcValue = PRG_RDB(&crcTable[crcValue ^ *value]);
	
	// read checksum. FALSE => no Ack
	checksum = halSht11ReadByte( FALSE );

	// if everything ok
	if ( mirrorByte( checksum ) == crcValue )
	{
		// Store new value. This is necessary since
		// contents of status register is initial value
		// of crc calculation.
		sht11StatusReg = *value;
		
		return TRUE;
	}
	else 
		return FALSE;
}

