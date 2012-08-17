/*  I2C BUS PERIPHERAL DEVICE  */


#define I2CBASE 	(*(volatile unsigned long *) 	0XFFFF0800)
#define I2CMSTA 	(*(volatile unsigned long *) 	0XFFFF0800)
#define I2CSSTA 	(*(volatile unsigned long *) 	0XFFFF0804)
#define I2CSRX 		(*(volatile unsigned long *) 	0XFFFF0808)
#define I2CSTX 		(*(volatile unsigned long *) 	0XFFFF080C)
#define I2CMRX 		(*(volatile unsigned long *) 	0XFFFF0810)
#define I2CMTX 		(*(volatile unsigned long *) 	0XFFFF0814)
#define I2CCNT 		(*(volatile unsigned long *) 	0XFFFF0818)
#define I2CADR 		(*(volatile unsigned long *) 	0XFFFF081C)

#define I2CBYTE 	(*(volatile unsigned long *) 	0XFFFF0824)
#define I2CALT 		(*(volatile unsigned long *) 	0XFFFF0828)
#define I2CCFG 		(*(volatile unsigned long *) 	0XFFFF082C)
#define I2CDIV	 	(*(volatile unsigned long *) 	0XFFFF0830)
#define I2CID0 		(*(volatile unsigned long *) 	0XFFFF0838)
#define I2CID1 		(*(volatile unsigned long *) 	0XFFFF083C)
#define I2CID2 		(*(volatile unsigned long *) 	0XFFFF0840)
#define I2CID3 		(*(volatile unsigned long *) 	0XFFFF0844)



/*  I2C BUS PERIPHERAL DEVICE 1 */

#define I2C0BASE 	(*(volatile unsigned long *) 	0XFFFF0800)
#define I2C0MSTA 	(*(volatile unsigned long *) 	0XFFFF0800)
#define I2C0SSTA 	(*(volatile unsigned long *) 	0XFFFF0804)
#define I2C0SRX 	(*(volatile unsigned long *) 	0XFFFF0808)
#define I2C0STX 	(*(volatile unsigned long *) 	0XFFFF080C)
#define I2C0MRX 	(*(volatile unsigned long *) 	0XFFFF0810)
#define I2C0MTX 	(*(volatile unsigned long *) 	0XFFFF0814)
#define I2C0CNT 	(*(volatile unsigned long *) 	0XFFFF0818)
#define I2C0ADR 	(*(volatile unsigned long *) 	0XFFFF081C)

#define I2C0BYTE 	(*(volatile unsigned long *) 	0XFFFF0824)
#define I2C0ALT 	(*(volatile unsigned long *)	0XFFFF0828)
#define I2C0CFG 	(*(volatile unsigned long *) 	0XFFFF082C)
#define I2C0DIV 	(*(volatile unsigned long *) 	0XFFFF0830)
#define I2C0ID0 	(*(volatile unsigned long *) 	0XFFFF0838)
#define I2C0ID1 	(*(volatile unsigned long *) 	0XFFFF083C)
#define I2C0ID2 	(*(volatile unsigned long *) 	0XFFFF0840)
#define I2C0ID3 	(*(volatile unsigned long *) 	0XFFFF0844)

/*  I2C BUS PERIPHERAL DEVICE 2 */

#define I2C1BASE 	(*(volatile unsigned long *) 	0XFFFF0900)
#define I2C1MSTA 	(*(volatile unsigned long *) 	0XFFFF0900)
#define I2C1SSTA 	(*(volatile unsigned long *) 	0XFFFF0904)
#define I2C1SRX 	(*(volatile unsigned long *)	0XFFFF0908)
#define I2C1STX 	(*(volatile unsigned long *) 	0XFFFF090C)
#define I2C1MRX 	(*(volatile unsigned long *) 	0XFFFF0910)
#define I2C1MTX 	(*(volatile unsigned long *) 	0XFFFF0914)
#define I2C1CNT 	(*(volatile unsigned long *) 	0XFFFF0918)
#define I2C1ADR 	(*(volatile unsigned long *) 	0XFFFF091C)

#define I2C1BYTE 	(*(volatile unsigned long *) 	0XFFFF0924)
#define I2C1ALT 	(*(volatile unsigned long *) 	0XFFFF0928)
#define I2C1CFG 	(*(volatile unsigned long *) 	0XFFFF092C)
#define I2C1DIV 	(*(volatile unsigned long *) 	0XFFFF0930)
#define I2C1ID0 	(*(volatile unsigned long *) 	0XFFFF0938)
#define I2C1ID1 	(*(volatile unsigned long *) 	0XFFFF093C)
#define I2C1ID2 	(*(volatile unsigned long *) 	0XFFFF0940)
#define I2C1ID3 	(*(volatile unsigned long *) 	0XFFFF0944)

