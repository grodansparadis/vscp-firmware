
// system includes
#include "global.h"

// local includes
#include "i2c.h"
#include "pca9532.h"

// functions
void pca9532Init(u08 node)
{
	u08 packet[2];

	// reset all registers to default state
	packet[1] = 0;
	packet[0] = PCA9532_REG_LS0;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
	packet[0] = PCA9532_REG_LS1;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
	packet[0] = PCA9532_REG_LS2;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
	packet[0] = PCA9532_REG_LS3;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
	
	pca9532SetFreq(node, 0, 0);
	pca9532SetFreq(node, 1, 0);
	pca9532SetPWM(node, 0, 0);
	pca9532SetPWM(node, 1, 0);
}

void pca9532SetPinState(u08 node, u08 pin, u08 state)
{
	u08 packet[2];

	// determine the register to access
	packet[0] = PCA9532_REG_LS0+(pin/4);
	// get the current state
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 1, &packet[0]);
	i2cMasterReceive(PCA9532_I2C_BASE_ADDR|node, 1, &packet[1]);
	// clear old state and set new one
	packet[1] &= ~(3<<((pin%4)*2));
	packet[1] |= state<<((pin%4)*2);
	// write back to device	
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
}

void pca9532SetFreq(u08 node, u08 ch, u08 period)
{
	u08 packet[2];
	packet[0] = PCA9532_REG_PSC0+(ch<<1);
	packet[1] = period;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
}

void pca9532SetPWM(u08 node, u08 ch, u08 duty)
{
	u08 packet[2];
	packet[0] = PCA9532_REG_PWM0+(ch<<1);
	packet[1] = duty;
	i2cMasterSend(PCA9532_I2C_BASE_ADDR|node, 2, packet);
}

u16 pca9532GetInput(u08 node)
{
	return 0;
}
