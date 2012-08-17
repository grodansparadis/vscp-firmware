/***********************************************************************************
*	Copyright 2004 Anglia Design
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise.  You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY:   ANGLIA SHALL NOT BE LIABLE FOR ANY LOSS OF PROFITS,
*	LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR INDIRECT, SPECIAL,
*	INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER THIS AGREEMENT OR
*	OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	Web     		: www.anglia-designs.com
*
***********************************************************************************/

// Martin Thomas: added additional dummy-syscalls, modified sbrk

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

//mt:
#include <reent.h>

// extern _ssize_t _write_r _PARAMS ((struct _reent *, int, const void *, size_t));
_ssize_t _write_r(struct _reent *reent, int fd, const void *ptr, size_t len)
{
	return len;
}

// extern _ssize_t _read_r _PARAMS ((struct _reent *, int, void *, size_t));
_ssize_t _read_r(struct _reent *reent, int fd, void *ptr, size_t len)
{
	return len;
}

// extern int _close_r _PARAMS ((struct _reent *, int));
int _close_r(struct _reent *reent, int fd)
{
	return 0;
}

// extern _off_t _lseek_r _PARAMS ((struct _reent *, int, _off_t, int));
_off_t _lseek_r(struct _reent *reent, int fd, _off_t off, int dir)
{
	return 0;
}

// extern int _fstat_r _PARAMS ((struct _reent *, int, struct stat *));
extern int _fstat_r(struct _reent *reent, int fd, struct stat *st)
{
	return 0;
}

void exit(int n)
{
	while(1);
}

int isatty(int fd); // avoid prototype warning
int isatty(int fd)
{
	return 1;
}

register char *stack_ptr asm ("sp");

extern char* end; // Defined by the linker

// extern void *_sbrk_r _PARAMS ((struct _reent *, ptrdiff_t));
void* _sbrk_r(struct _reent *reent, ptrdiff_t incr)
{
	//mt:  extern char end asm ("end");		// Defined by the linker
	//extern char end asm ("__heap_start__");		// Defined by the linker
	static char *heap_end;
	char *prev_heap_end;

	if( heap_end == NULL ) {
		// mt heap_end = &end;
		heap_end = end;
	}
	prev_heap_end = heap_end;

	if(( heap_end + incr ) > stack_ptr )
	{
		/* Some of the libstdc++-v3 tests rely upon detecting
		out of memory errors, so do not abort here.  */
		//write( 1, "sbrk: Heap and stack collision\n", 32 );
		//abort();
		exit(1);
		//errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;
}
