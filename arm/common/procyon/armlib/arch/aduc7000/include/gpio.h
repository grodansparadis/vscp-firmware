/*  GPIO AND SERIAL PORT MUX */

#ifndef __gpio_h
#define __gpio_h

#ifndef __gpio_base 
#define __gpio_base 0xFFFFF400
#endif

typedef struct s_GPIO_Port {
   unsigned long Data;			/* @0  */
   unsigned long Set;	        	/* @4  */
   unsigned long Clear;	        	/* @8  */
   unsigned long _fill_0;		/* @12 */
} s_GPIO_Port;

typedef struct s_GPIO {
   unsigned long Control[5];		/* @0x00-0x10 inclusive */
   unsigned long _fill_0[3];		/* @0x14-0x1C inclusive */
   s_GPIO_Port Port[5];			/* @0x20 */
} s_GPIO;

/* gpio @ 0xFFFFF400 */
#define GPIO ((volatile struct s_GPIO *) (__gpio_base))

#endif /* defined(__gpio_h) */

#define GPIOBASE 	(*(volatile unsigned long *) 	0XFFFFF400)
#define GP0CON 		(*(volatile unsigned long *) 	0XFFFFF400)
#define GP1CON 		(*(volatile unsigned long *) 	0XFFFFF404)
#define GP2CON 		(*(volatile unsigned long *) 	0XFFFFF408)
#define GP3CON 		(*(volatile unsigned long *) 	0XFFFFF40C)
#define GP4CON 		(*(volatile unsigned long *) 	0XFFFFF410)

#define GP0DAT 		(*(volatile unsigned long *) 	0XFFFFF420)
#define GP0SET 		(*(volatile unsigned long *) 	0XFFFFF424)
#define GP0CLR 		(*(volatile unsigned long *) 	0XFFFFF428)

#define GP1DAT 		(*(volatile unsigned long *) 	0XFFFFF430)
#define GP1SET 		(*(volatile unsigned long *) 	0XFFFFF434)
#define GP1CLR 		(*(volatile unsigned long *) 	0XFFFFF438)

#define GP2DAT 		(*(volatile unsigned long *) 	0XFFFFF440)
#define GP2SET 		(*(volatile unsigned long *) 	0XFFFFF444)
#define GP2CLR 		(*(volatile unsigned long *) 	0XFFFFF448)

#define GP3DAT 		(*(volatile unsigned long *) 	0XFFFFF450)
#define GP3SET 		(*(volatile unsigned long *) 	0XFFFFF454)
#define GP3CLR 		(*(volatile unsigned long *) 	0XFFFFF458)

#define GP4DAT 		(*(volatile unsigned long *) 	0XFFFFF460)
#define GP4SET 		(*(volatile unsigned long *) 	0XFFFFF464)
#define GP4CLR 		(*(volatile unsigned long *) 	0XFFFFF468)
