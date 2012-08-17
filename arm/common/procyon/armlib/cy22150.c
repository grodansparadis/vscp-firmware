
// system includes
#include "global.h"

// local includes
#include "i2c.h"
#include "cy22150.h"

// functions
void cy22150Init(void)
{
	// set dividers to default
	cy22150WriteReg(CY22150_REG_DIV1, 8);
	cy22150WriteReg(CY22150_REG_DIV2, 8);

	// set crystal capacitor loading to 20pf
	// Capload = (Cload-(Cboard+Cchip))/0.09375pF
	// (20pF-(2pF+6pF))/0.09375 = 128
	cy22150WriteReg(CY22150_REG_CAPLOAD, (u08)((20-(2+6))/0.09375) );
}


void cy22150SetOutput(u08 clkpin, u08 enable)
{
	// enable/disable requested output pin
	if(enable)
		cy22150WriteReg(CY22150_REG_CLKOE, cy22150ReadReg(CY22150_REG_CLKOE) | (1<<clkpin));
	else
		cy22150WriteReg(CY22150_REG_CLKOE, cy22150ReadReg(CY22150_REG_CLKOE) & ~(1<<clkpin));
}

void cy22150SetDiv(u08 divreg, u08 div)
{
	switch(divreg)
	{
	case 0: cy22150WriteReg(CY22150_REG_DIV1, div); break;
	case 1: cy22150WriteReg(CY22150_REG_DIV2, div); break;
	}
}

void cy22150SetQ(int q)
{
	if(q<2)
		return; // error: q must be at least 2
	// write q value with PO bit masked
	cy22150WriteReg(CY22150_REG_QCNT, ((q-2)&0x7F) );
}

void cy22150SetP(int p)
{
	u08 pump;
	int pb;

	// calculate pump value from p multiplier
	if(p<45)		pump = 0;
	else if(p<480)	pump = 1;
	else if(p<640)	pump = 2;
	else if(p<800)	pump = 3;
	else			pump = 4;

	// calculate pb from p value
	pb = (p/2)-4;

	// write p value
	cy22150WriteReg(CY22150_REG_PBHI, 0xC0 | (pump<<2) | ((pb>>8)&0x03) );
	cy22150WriteReg(CY22150_REG_PBLO, (pb&0xFF) );
}

void cy22150WriteReg(u08 addr, u08 data)
{
	u08 packet[2];
	// prepare write packet
	packet[0] = addr;
	packet[1] = data;
	// write to device	
	i2cMasterSend(CY22150_I2C_ADDR, 2, packet);
}

u08 cy22150ReadReg(u08 addr)
{
	u08 data;
	// select register to read
	i2cMasterSend(CY22150_I2C_ADDR, 1, &addr);
	// read value
	i2cMasterReceive(CY22150_I2C_ADDR, 1, &data);
	// return value
	return data;
}
