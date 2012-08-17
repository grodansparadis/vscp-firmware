#ifndef INC_LPC_I2C_H
#define INC_LPC_I2C_H

// I2C Interface Registers
typedef struct
{
  REG_8 conset;                         // Control Set Register
  REG_8 _pad0[3];
  REG_8 stat;                           // Status Register
  REG_8 _pad1[3];
  REG_8 dat;                            // Data Register
  REG_8 _pad2[3];
  REG_8 adr;                            // Slave Address Register
  REG_8 _pad3[3];
  REG16 sclh;                           // SCL Duty Cycle Register (high half word)
  REG16 _pad4;
  REG16 scll;                           // SCL Duty Cycle Register (low half word)
  REG16 _pad5;
  REG_8 conclr;                         // Control Clear Register
  REG_8 _pad6[3];
} i2cRegs_t;

//---  LPC21XX I2C flags --------------------

#define  I2C_FLAG_AA    (1<<2)
#define  I2C_FLAG_SI    (1<<3)
#define  I2C_FLAG_STO   (1<<4)
#define  I2C_FLAG_STA   (1<<5)
#define  I2C_FLAG_I2EN  (1<<6)

//---- I2C Speed
#define  I2C_SPEED_100   0
#define  I2C_SPEED_400   1

//--- write timeout ( > 5 mS)
#define  I2C_WR_TIMEOUT     10000

//--- Errors
#define  I2C_NO_ERR                    0
#define  I2C_ERR_NO_RESPONSE           1
#define  I2C_ERR_WRONG_PARAM           2
#define  I2C_ERR_WR_TIMEOUT            3
#define  I2C_ERR_BAD_ADDR              4

//----- I/O Extender PCA9555

#define  I2C_PCA9555_IN     0
#define  I2C_PCA9555_OUT    2
#define  I2C_PCA9555_PI     4
#define  I2C_PCA9555_CONF   6

#define I2C_MAX1239_CH_0    (0<<4 | 0<<3 | 0<<2 | 0<<1)
#define I2C_MAX1239_CH_1    (0<<4 | 0<<3 | 0<<2 | 1<<1)
#define I2C_MAX1239_CH_2    (0<<4 | 0<<3 | 1<<2 | 0<<1)
#define I2C_MAX1239_CH_3    (0<<4 | 0<<3 | 1<<2 | 1<<1)
#define I2C_MAX1239_CH_4    (0<<4 | 1<<3 | 0<<2 | 0<<1)
#define I2C_MAX1239_CH_5    (0<<4 | 1<<3 | 0<<2 | 1<<1)
#define I2C_MAX1239_CH_6    (0<<4 | 1<<3 | 1<<2 | 0<<1)
#define I2C_MAX1239_CH_7    (0<<4 | 1<<3 | 1<<2 | 1<<1)
#define I2C_MAX1239_CH_8    (1<<4 | 0<<3 | 0<<2 | 0<<1)
#define I2C_MAX1239_CH_9    (1<<4 | 0<<3 | 0<<2 | 1<<1)
#define I2C_MAX1239_CH_10   (1<<4 | 0<<3 | 1<<2 | 0<<1)
#define I2C_MAX1239_CH_11   (1<<4 | 0<<3 | 1<<2 | 1<<1)
#define I2C_MAX1239_SETUP    0xa2 //setup, intclk, refin, 
#define I2C_MAX1239_CONF     0x61
#define I2C_MAX1239_ADDR     0x35 //0110101 typ EEE

void i2c_lpc_init(U8 Mode);
U8 pca9555_write(U8 Mode, U8 addr, U16 val);
U8 pca9555_read(U8 Mode, U8 addr, U16 * ret_val);
U8 max1239_write(U8 addr, U8 val);
U8 max1239_read(U8 addr, U16 * ret_val);
#endif
