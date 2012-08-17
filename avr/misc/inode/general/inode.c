#include "/usr/local/avr/include/sig-avr.h"
#include "inode.h"

int main( void );
unsigned char test1( unsigned char r );
int test2( int i );
unsigned char test3( unsigned long l );

int main()
{
	test1( 10 );
	test2( 20000 );
	test3( 567890 );
}

unsigned char test1( unsigned char r )
{
	unsigned char ttt;
	
	ttt = r;
	return 1;
}

int test2( int i )
{
	int ttt;
	
	ttt = i;
	return ttt;
}

unsigned char test3( unsigned long l )
{
	unsigned long ttt;
	
	ttt = l;
	return 1;
}

//
// Received when we enter command mode
//

SIGNAL( SIG_INTERRUPT0 )
{
		
	asm( "sei");
}

//
// Received when clocked by master. This IRQ should normally not
// happen if not in command mode.
//

SIGNAL( SIG_INTERRUPT1 )
{
	// Not interested if not in command mode
	if ( 1  )
	{
		;
	}
	
	asm("sei");
}
