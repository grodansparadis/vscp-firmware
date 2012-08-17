/***********************************************************************/
/*                                                                     */
/*  SERIAL.C:  Low Level Serial Routines                               */
/*                                                                     */
/***********************************************************************/

#include <ADuC7024.H>                      /* ADuC7024 definitions          */

#define CR     0x0D
int putchar(int ch)  __attribute__ ((used));
int getchar (void)  __attribute__ ((used));

int putchar(int ch){                   /* Write character to Serial Port  */

	if (ch == '\n')  {
    	while(!(0x020==(COMSTA0 & 0x020)))
    	{}
		COMTX = CR;							/* output CR */
		}
    while(!(0x020==(COMSTA0 & 0x020)))
    {}
 
 	return (COMTX = ch);
}


int getchar (void)  {                      /* Read character from Serial Port */

   	while(!(0x01==(COMSTA0 & 0x01)))
   	{}
  	return (COMTX);
}

