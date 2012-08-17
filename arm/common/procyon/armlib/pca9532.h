
#ifndef PCA9532_H
#define PCA9532_H

#include "global.h"

// device base address
#define PCA9532_I2C_BASE_ADDR	0x60

// device registers
#define PCA9532_REG_INPUT0		0
#define PCA9532_REG_INPUT1		1
#define PCA9532_REG_PSC0		2
#define PCA9532_REG_PWM0		3
#define PCA9532_REG_PSC1		4
#define PCA9532_REG_PWM1		5
#define PCA9532_REG_LS0			6
#define PCA9532_REG_LS1			7
#define PCA9532_REG_LS2			8
#define PCA9532_REG_LS3			9

// possible pin/led states
#define PCA9532_LED_OFF			0
#define PCA9532_LED_ON			1
#define PCA9532_LED_DIM0		2
#define PCA9532_LED_DIM1		3

void pca9532Init(u08 node);

void pca9532SetPinState(u08 node, u08 pin, u08 state);

void pca9532SetFreq(u08 node, u08 ch, u08 period);

void pca9532SetPWM(u08 node, u08 ch, u08 duty);

u16 pca9532GetInput(u08 node);


#endif
