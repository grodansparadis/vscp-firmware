/*  TIMER 0    */

#define T0BASE 		(*(volatile unsigned long *) 	0XFFFF0300)
#define T0LD 		(*(volatile unsigned long *)	0XFFFF0300)
#define T0VAL 		(*(volatile unsigned long *) 	0XFFFF0304)
#define T0CON 		(*(volatile unsigned long *)	0XFFFF0308)
#define T0CLRI 		(*(volatile unsigned long *) 	0XFFFF030C)

/*  GENERAL PURPOSE TIMER   */

#define T1BASE 		(*(volatile unsigned long *) 	0XFFFF0320)
#define T1LD 		(*(volatile unsigned long *) 	0XFFFF0320)
#define T1VAL 		(*(volatile unsigned long *) 	0XFFFF0324)
#define T1CON 		(*(volatile unsigned long *) 	0XFFFF0328)
#define T1CLRI 		(*(volatile unsigned long *) 	0XFFFF032C)
#define T1CAP 		(*(volatile unsigned long *) 	0XFFFF0330)

/*  WAKE UP TIMER   */

#define T2BASE 		(*(volatile unsigned long *) 	0XFFFF0340)
#define T2LD 		(*(volatile unsigned long *) 	0XFFFF0340)
#define T2VAL 		(*(volatile unsigned long *) 	0XFFFF0344)
#define T2CON 		(*(volatile unsigned long *) 	0XFFFF0348)
#define T2CLRI 		(*(volatile unsigned long *) 	0XFFFF034C)

/*  WATCHDOG TIMER    */

#define T3BASE 		(*(volatile unsigned long *) 	0XFFFF0360)
#define T3LD 		(*(volatile unsigned long *) 	0XFFFF0360)
#define T3VAL 		(*(volatile unsigned long *) 	0XFFFF0364)
#define T3CON 		(*(volatile unsigned long *) 	0XFFFF0368)
#define T3CLRI 		(*(volatile unsigned long *) 	0XFFFF036C)
