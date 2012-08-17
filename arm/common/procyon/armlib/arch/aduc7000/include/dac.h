/*  DAC INTERFACE REGISTERS   */

#ifndef __dac_h
#define __dac_h

#if  defined (ADUC7020)  || defined (ADUC7026) 

#ifndef DAC_COUNT
#define DAC_COUNT 4		/*	Define the Number of DACs */
#endif

#elif defined  (ADUC7021) || defined (ADUC7024) 

#ifndef DAC_COUNT
#define DAC_COUNT 2		/*	Define the Number of DACs */
#endif

#elif defined (ADUC7022) || defined (ADUC7025) || defined (ADUC7027)

#ifndef DAC_COUNT
#define DAC_COUNT 0		/* Define the Number of DACs */
#endif

#endif



#ifndef __dac_base 		/*	Define the DAC Base location */
#define __dac_base 0xFFFF0600
#endif


/*	Setup a structure which will allow access to DACs */


typedef struct s_DAC_Channel {
   unsigned long Control;	/* @0 */
   unsigned long Data;	    	/* @4 */
} s_DAC_Channel;

typedef struct s_DAC {
   s_DAC_Channel Channel[DAC_COUNT];	/* @0 */
} s_DAC;

/* dac @ 0xFFFF0600 */

#define DAC ((volatile struct s_DAC *) (__dac_base))

#define DACBASE 	(*(volatile unsigned long *) 	0XFFFF0600)
#define DAC0CON 	(*(volatile unsigned long *)	0XFFFF0600)
#define DAC0DAT 	(*(volatile unsigned long *) 	0XFFFF0604)
#define DAC1CON 	(*(volatile unsigned long *) 	0XFFFF0608)
#define DAC1DAT 	(*(volatile unsigned long *) 	0XFFFF060C)

#if  defined (ADUC7020)  || defined (ADUC7026) 

#define DAC2CON 	(*(volatile unsigned long *) 	0XFFFF0610)
#define DAC2DAT 	(*(volatile unsigned long *) 	0XFFFF0614)
#define DAC3CON 	(*(volatile unsigned long *) 	0XFFFF0618)
#define DAC3DAT 	(*(volatile unsigned long *) 	0XFFFF061C)

#endif

#endif
