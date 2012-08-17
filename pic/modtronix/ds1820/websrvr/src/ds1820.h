#ifndef ds1820__h
#define ds1820__h

void ow_init(void);
char ow_in_byte(void);
void ow_out_byte(unsigned char d);
void convert(char *temp_buff);

//DS1820 is connected to PORTC bit 4.

#define DSPORT      PORTC
#define DS_BIT	    4  

#define CONVERT	    0x44
#define READSCRATCH 0xBE
#define SKIP_ROM	0xCC

#endif

