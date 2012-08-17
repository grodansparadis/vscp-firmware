#ifndef DATAFLASH_H
#define DATAFLASH_H

/* commands */
#define DATAFLASH_MAIN_MEMORY_PAGE_READ 0xD2
#define DATAFLASH_READ_BUFFER1 0xD4
#define DATAFLASH_READ_BUFFER2 0xD6
#define DATAFLASH_WRITE_BUFFER1 0x84
#define DATAFLASH_WRITE_BUFFER2 0x87
#define DATAFLASH_LOAD_BUFFER1 0x53
#define DATAFLASH_LOAD_BUFFER2 0x55
#define DATAFLASH_SAVE_BUFFER1 0x83
#define DATAFLASH_SAVE_BUFFER2 0x86
#define DATAFLASH_READ_STATUS 0xD7
#define DATAFLASH_PAGE_ERASE 0x81

/* status register bits */
#define DATAFLASH_STATUS_PAGESIZE 0
#define DATAFLASH_STATUS_PROTECT 1
#define DATAFLASH_STATUS_COMP 6
#define DATAFLASH_STATUS_BUSY 7

/* status register static bits */
#define DATAFLASH_STATUS_STATIC 0x2c

/* structures */

/*
  * 8bit: magic byte, 0xFF if erased, 0x23 if used
  * 32bit: node number
  * 16bit: index of this page in the node
  * 32bit: version (always increasing, only pages with same node number and
           highest version are valid, this helps providing "wear levelling")
  * 16bit: pointer to the next page for this node
  * 3 byter reserved
  */
/*
struct filesystem_node_t {
    U8 magic;
    U32 node_number;
    U16 page_index;
    U32 version;
    U16 next_page;
    U8 reserved[3];
};
*/
/* prototypes */
U8 dataflash_read_status(void);
void dataflash_wait_busy(void);
void dataflash_read_flash(U16 page_address, U16 offset, U8 *data, U16 len);
void dataflash_load_buffer(U8 buffer, U16 page_address);
void dataflash_read_buffer(U8 buffer, U16 offset, U8 *data, U16 len);
void dataflash_write_buffer(U8 buffer, U16 offset, U8 *data, U16 len);
void dataflash_save_buffer(U8 buffer, U16 page_address);
void dataflash_erase_page(U16 page_address);
#endif

