#include "main.h"
#include "mirror.h"

int main( void )
{
	union
	{
		uint16 w;
		uint8  ch[2];
	} value;
	bool noError;
	uint8 string[30];

	// enable LCD
    halLcdInit( cursorOff, LCD_FUNCTION_DEFAULT ); 

	// setup ports to communicate with sht11
	halSht11Init();

	strcpy( string, "Hallo Welt!" );

    halLcdPuts( string );

	while ( 1 )
	{
		halSht11StartMeasure( sht11MeaT );
		noError = halSht11GetMeaValue( &value.w );

		value.w -= 4000;

		itoa( value.w, string, 10 );
		halLcdGotoXY( 0, 0 );
	    halLcdPuts( string );

		halLcdGotoXY( 0, 1 );
		halLcdPutChar( noError == TRUE ? 'T' : 'F' );
	}
}
