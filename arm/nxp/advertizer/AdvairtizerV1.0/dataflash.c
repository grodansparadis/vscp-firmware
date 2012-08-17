#include <global.h>

void cs_low(void){ 
  F_IO0CLR = CS_FLASH;
}
void cs_high(void){ 
  F_IO0SET = CS_FLASH;
}

U8 dataflash_read_status(void){
    cs_low();
    /* send opcode and read status afterwards */
    spiSendByte(DATAFLASH_READ_STATUS);
    U8 data = spiSendByte(0);
    cs_high();
    return data;
}
//time @ 10mhz sspi 6us
void dataflash_wait_busy(void){
    cs_low();
    /* send opcode and read status until BUSY bit is unset */
    spiSendByte(DATAFLASH_READ_STATUS);//D7
    while(!(spiSendByte(0) & (1<<DATAFLASH_STATUS_BUSY)));//bit7
    cs_high();
}

void dataflash_read_flash(U16 page_address, U16 offset, U8 *data, U16 len){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    spiSendByte(DATAFLASH_MAIN_MEMORY_PAGE_READ);//D2

    /* send 3 address bytes: 2 don't care bits, 12 bits page address,
     * 10 bits byte offset */
    page_address <<= 2;
    spiSendByte((U8)(page_address>>8));
    spiSendByte((U8)(page_address) | (U8)(offset>>8));
    spiSendByte((U8)(offset));
    /* send 4 don't care bytes */
    for (U8 i = 0; i < 4; i++)
        spiSendByte(0);
    /* read memory */
    while (len-- != 0)
        *data++ = spiSendByte(0);
    cs_high();
}

void dataflash_load_buffer(U8 buffer, U16 page_address){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    if (buffer == 1)
        spiSendByte(DATAFLASH_LOAD_BUFFER1);
    else
        spiSendByte(DATAFLASH_LOAD_BUFFER2);

    /* send 3 address bytes: 2 don't care bits, 12 bits page address,
     * 10 don't care bits */
    page_address <<= 2;
    spiSendByte((U8)(page_address>>8));
    spiSendByte((U8)(page_address));
    spiSendByte(0);
    cs_high();
}

void dataflash_read_buffer(U8 buffer, U16 offset, U8 *data, U16 len){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    if (buffer == 1)
        spiSendByte(DATAFLASH_READ_BUFFER1);
    else
        spiSendByte(DATAFLASH_READ_BUFFER2);

    /* send 3 address bytes: 14 don't care bits, 10 bits offset within the buffer */
    spiSendByte(0);
    spiSendByte((U8)(offset>>8));
    spiSendByte((U8)(offset));
    /* send one don't care byte */
    spiSendByte(0);

    /* read memory */
    while (len-- != 0)
        *data++ = spiSendByte(0);
    cs_high();
}

void dataflash_write_buffer(U8 buffer, U16 offset, U8 *data, U16 len){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    if (buffer == 1)
        spiSendByte(DATAFLASH_WRITE_BUFFER1);
    else
        spiSendByte(DATAFLASH_WRITE_BUFFER2);

    /* send 3 address bytes: 14 don't care bits, 10 bits offset within the buffer */
    spiSendByte(0);
    spiSendByte((U8)(offset>>8));
    spiSendByte((U8)(offset));

    /* send memory */
    while (len-- != 0)
        spiSendByte(*data++);
    cs_high();

}

void dataflash_save_buffer(U8 buffer, U16 page_address){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    if (buffer == 1)
        spiSendByte(DATAFLASH_SAVE_BUFFER1);
    else
        spiSendByte(DATAFLASH_SAVE_BUFFER2);

    /* send 3 address bytes: 2 don't care bits, 12 bits page address,
     * 10 don't care bits */
    page_address <<= 2;
    spiSendByte((U8)(page_address>>8));
    spiSendByte((U8)(page_address));
    spiSendByte(0);
    cs_high();
}

void dataflash_erase_page(U16 page_address){
    dataflash_wait_busy();
    cs_low();
    /* send opcode */
    spiSendByte(DATAFLASH_PAGE_ERASE);

    /* send 3 address bytes: 2 don't care bits, 12 bits page address, 10
     * don't care bits */
    page_address <<= 2;
    spiSendByte((U8)(page_address>>8));
    spiSendByte((U8)(page_address));
    spiSendByte(0);
    cs_high();
}
