/***********************************************************************/
/*                                                                     */
/*  SYSCALLS.C:  System Calls Remapping                                */
/*                                                                     */
/***********************************************************************/

#include <stdlib.h>


extern int putchar (int ch);

/*	Write a buffer using putchar function in serial.c 	*/

int write (int file, char * ptr, int len) {
  int i;

  for (i = 0; i < len; i++) putchar (*ptr++);
  return len;
}

int isatty (int fd) {
  return 1;
}


void _exit (int n) {
label:  goto label; /* endless loop */
}

