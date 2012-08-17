/*  FLASH CONTROL INTERFACE   */

#define READ_HALF_WORD 			0x00000001						
#define WRITE_HALF_WORD			0x00000002
#define ERASE_WRITE 			0x00000003						
#define VERIFY_HALF_WORD		0x00000004
#define ERASE_PAGE			0x00000005						
#define MASS_ERASE 			0x00000006
#define READ_BURST 			0x00000007						
#define WRITE_BURST 			0x00000008
#define ERASE_BURST_WRITE		0x00000009						
#define BURST_END 			0x0000000A
#define USR_SIGN 			0x0000000B
#define FEE_PROT 			0x0000000C

#define FLASHBASE 	(*(volatile unsigned long *) 	0XFFFFF800)
#define FEESTA 		(*(volatile unsigned long *) 	0XFFFFF800)
#define FEEMOD 		(*(volatile unsigned long *)	0XFFFFF804)
#define FEECON 		(*(volatile unsigned long *) 	0XFFFFF808)
#define FEEDAT 		(*(volatile unsigned long *) 	0XFFFFF80C)
#define FEEADR 		(*(volatile unsigned long *) 	0XFFFFF810)
#define FEESIGN 	(*(volatile unsigned long *) 	0XFFFFF818)
#define FEEPRO 		(*(volatile unsigned long *) 	0XFFFFF81C)
#define FEEHIDE 	(*(volatile unsigned long *) 	0XFFFFF820)


