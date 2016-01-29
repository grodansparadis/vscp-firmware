#ifndef _I2C_H_
#define _I2C_H_

//---  LPC21XX I2C flags --------------------

#define  I2C_FLAG_AA    (1<<2)
#define  I2C_FLAG_SI    (1<<3)
#define  I2C_FLAG_STO   (1<<4)
#define  I2C_FLAG_STA   (1<<5)
#define  I2C_FLAG_I2EN  (1<<6)

//---- I2C Speed
#define  I2C_SPEED_100   0
#define  I2C_SPEED_400   1

//--- Errors

#define  I2C_NO_ERR                    0
#define  I2C_ERR_NO_RESPONSE           1
#define  I2C_ERR_WRONG_PARAM           2
#define  I2C_ERR_24XX_WR_TIMEOUT       3
#define  I2C_ERR_24XX_BAD_ADDR         4
#define  I2C_ERR_24XX_BAD_TYPE         5
#define  I2C_ERR_24XX_BAD_PAGESIZE     6
#define  I2C_ERR_24XX_WRONG_NUM        7
#define  I2C_ERR_LM75_WRONG_REGISTER   8


//--- EEPROM 24xx types

#define  EEPROM_24XX04                 1
#define  EEPROM_24XX08                 2
#define  EEPROM_24XX16                 3
#define  EEPROM_24XX32                 4
#define  EEPROM_24XX64                 5
#define  EEPROM_24XX128                6
#define  EEPROM_24XX256                7
#define  EEPROM_24XX512                8

//--- EEPROM 24xx max addr values

#define  EEPROM_MAX_ADDR_24XX04    0x01FF   //-- 512 Bytes
#define  EEPROM_MAX_ADDR_24XX08    0x03FF   //--  1 KBytes
#define  EEPROM_MAX_ADDR_24XX16    0x07FF   //--  2 KBytes
#define  EEPROM_MAX_ADDR_24XX32    0x0FFF   //--  4 KBytes
#define  EEPROM_MAX_ADDR_24XX64    0x1FFF   //--  8 KBytes
#define  EEPROM_MAX_ADDR_24XX128   0x3FFF   //-- 16 KBytes
#define  EEPROM_MAX_ADDR_24XX256   0x7FFF   //-- 32 KBytes
#define  EEPROM_MAX_ADDR_24XX512   0xFFFF   //-- 64 KBytes

//--- EEPROM 24xx write timeout ( > 5 mS)

#define  I2C_WR_24XX_TIMEOUT     10000

//--- Temperature sensor LM75 registers

#define  LM75_REG_TEMP      1
#define  LM75_REG_THYST     2
#define  LM75_REG_TSET      3
#define  LM75_REG_CONFIG    4


//----- I/O Extender PCA9555

#define  I2C_PCA9555_IN     0
#define  I2C_PCA9555_OUT    2
#define  I2C_PCA9555_PI     4
#define  I2C_PCA9555_CONF   6

//-- RTC    DS 1307

typedef struct _DATETIMEINFO
{
   unsigned char year;    //-- from 00
   unsigned char month;   //-- 1..12
   unsigned char day;     //-- 1..31
   unsigned char hour;    //-- 0..23
   unsigned char minute;  //-- 0..59
   unsigned char second;  //-- 0..59
}DATETIMEINFO;



//---- Prototypes ---

void i2c_lpc_init(int Mode);

int m24xx_write(
    int eeprom_type,    //-- EEPROM type
    int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
    int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
    char * buf,         //-- Data src buf
    int num);           //-- Bytes to write qty

int m24xx_read(
  int eeprom_type,    //-- EEPROM type
  int eeprom_addr,    //-- start eeprom addr ( not included Hardware A2,A1,A0)
  int eeprom_cs_val,  //-- Hardware A2,A1,A0 (valid from 24XX32)
  char * buf,         //-- Data dst buf
  int num);           //-- Bytes to read qty

int lm75_read16(int cs_val,
                int reg,
                int * ret_val);
int lm75_read_cfg(int cs_val,int * ret_val);
int lm75_write16(int cs_val,
                int reg,
                int wr_val);
int lm75_write_cfg(int cs_val,int wr_val);
int lm75_init();
int lm75_get_temp(int * ret_val);


int pca9555_write(int Mode, int val);
int pca9555_read(int Mode, int * ret_val);

char ds1307_toBCD(char bin_val);
char ds1307_fromBCD(char bcd_val);
int ds1307_set(DATETIMEINFO * dti);
int ds1307_get(DATETIMEINFO * dti);
int ds1307_wr_regs(char * buf);
int ds1307_rd_regs(char * buf);
int ds1307_wr_nvram(int addr,int num, char * buf,int * b_wr);
int ds1307_rd_nvram(int addr,int num, char * buf,int * b_rd);


#endif


