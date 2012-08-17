
#ifndef CY22150_H
#define CY22150_H

#include "global.h"

// device base address
#define CY22150_I2C_ADDR		0xD2

// device registers
#define CY22150_REG_CLKOE		0x09
#define CY22150_REG_DIV1		0x0C
#define CY22150_REG_XDRV		0x12
#define CY22150_REG_CAPLOAD		0x13
#define CY22150_REG_PBHI		0x40
#define CY22150_REG_PBLO		0x41
#define CY22150_REG_QCNT		0x42
#define CY22150_REG_MATRIX1		0x44
#define CY22150_REG_MATRIX2		0x45
#define CY22150_REG_MATRIX3		0x46
#define CY22150_REG_DIV2		0x47

// possible pin/led states
#define PCA9532_LED_OFF			0
#define PCA9532_LED_ON			1
#define PCA9532_LED_DIM0		2
#define PCA9532_LED_DIM1		3

void cy22150Init(void);
void cy22150WriteReg(u08 addr, u08 data);
u08 cy22150ReadReg(u08 addr);

void cy22150SetDiv(u08 divreg, u08 div);
void cy22150SetOutput(u08 clkpin, u08 enable);
void cy22150SetQ(int q);
void cy22150SetP(int p);


#endif
