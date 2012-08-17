
/*  PWM     */

#if  defined (ADUC7024)  || defined (ADUC7025) || defined (ADUC7026) || defined (ADUC7027)

#define PWMBASE 	(*(volatile unsigned long *) 	0XFFFFFC00)
#define PWMCON 		(*(volatile unsigned long *) 	0XFFFFFC00)
#define PWMSTA0 	(*(volatile unsigned long *) 	0XFFFFFC04)
#define PWMDAT0 	(*(volatile unsigned long *) 	0XFFFFFC08)
#define PWMDAT1 	(*(volatile unsigned long *) 	0XFFFFFC0C)
#define PWMCFG 		(*(volatile unsigned long *) 	0XFFFFFC10)

#define PWMA 		(*(volatile unsigned long *) 	0XFFFFFC14)
#define PWMB 		(*(volatile unsigned long *) 	0XFFFFFC18)
#define PWMC 		(*(volatile unsigned long *) 	0XFFFFFC1C)
#define PWM0 		(*(volatile unsigned long *) 	0XFFFFFC14)
#define PWM1 		(*(volatile unsigned long *) 	0XFFFFFC18)
#define PWM2 		(*(volatile unsigned long *) 	0XFFFFFC1C)

#define PWMEN 		(*(volatile unsigned long *) 	0XFFFFFC20)
#define PWMDAT2 	(*(volatile unsigned long *) 	0XFFFFFC24)
#define PWMSTA1 	(*(volatile unsigned long *) 	0XFFFFFC38)

#endif
