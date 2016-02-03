/*
  MCP2515.h - Library for Microchip MCP2515 CAN Controller

  Author: David Harding
  Created: 11/08/2010
  
  Minor changes 2012-08-28
  Ake Hedman, Grodans Paradis AB 
  <akhe@grodansparadis.com>
  for the VSCP (http://www.vscp.org) project.

  For further information see:
  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1289195675
  http://ww1.microchip.com/downloads/en/DeviceDoc/21801e.pdf
  http://en.wikipedia.org/wiki/CAN_bus
*/

#ifndef MCP2515_h
#define MCP2515_h

#include "MCP2515_defs.h"


class MCP2515
{
  public:
	// Constructor defining which pins to use for CS and INT
    MCP2515(byte CS_Pin, byte INT_Pin);

	// Overloaded initialization function
	int Init(int baud, byte freq);
	int Init(int baud, byte freq, byte sjw);

	// Basic MCP2515 SPI Command Set
    void Reset();
    byte Read(byte address);
    void Read(byte address, byte data[], byte bytes);
	CANMSG ReadBuffer(byte buffer);
	void Write(byte address, byte data);
	void Write(byte address, byte data[], byte bytes);
	void LoadBuffer(byte buffer, CANMSG message);
	void SendBuffer(byte buffers);
	byte Status();
	byte RXStatus();
	void BitModify(byte address, byte mask, byte data);
	
	// Added by akhe
	
	// Send message if there is a free buffer available
	// and return true. If no buffer is available
	// return false.
	bool SendMessage( CANMSG& message );
	
	// Fetch a message if there is one available and
	// return true. Return false if no message is
	// available.
	bool GetMessage( CANMSG& message ); 
	
	// Return true of there is a free transmitt
	// buffer available
	bool isFreeTxBuffer( void );

	// Extra functions
	bool Interrupt(); // Expose state of INT pin
	bool Mode(byte mode); // Returns TRUE if mode change successful

  private:
	bool _init(int baud, byte freq, byte sjw, bool autoBaud);
    // Pin variables
	byte _CS;
	byte _INT;
};

#endif 
