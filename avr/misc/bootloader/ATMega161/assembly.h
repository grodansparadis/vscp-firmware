void write_page (unsigned int adr, unsigned char function);
void write_lock_bits (unsigned char val);
unsigned int read_program_memory (unsigned int,unsigned char);
void fill_temp_buffer (unsigned int data,unsigned int adr);
void re_enable_RWW(void);
void CSEG_Write(void);

