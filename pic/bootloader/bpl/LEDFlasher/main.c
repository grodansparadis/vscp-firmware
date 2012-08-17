/*****************************************************************************
 * PIC18F252 MP3 PLAYER                                                      *
 *****************************************************************************/

#include <p18f452.h>	// Hardware setup
#include <delays.h>		// Delays
#include <usart.h>		// USART

//////////////////////////////////////////////////////////////////////////////

/* Set configuration bits for use with ICD2 / PICDEM2 PLUS Demo Board:
 *  - set HS oscillator
 *  - disable watchdog timer
 *  - disable low voltage programming
 */
#pragma romdata CONFIG
_CONFIG_DECL(_CONFIG1H_DEFAULT & _OSC_HS_1H,\
             _CONFIG2L_DEFAULT,\
             _CONFIG2H_DEFAULT & _WDT_OFF_2H,\
             _CONFIG3H_DEFAULT,\
             _CONFIG4L_DEFAULT & _LVP_OFF_4L,\
             _CONFIG5L_DEFAULT,\
             _CONFIG5H_DEFAULT,\
             _CONFIG6L_DEFAULT,\
             _CONFIG6H_DEFAULT,\
             _CONFIG7L_DEFAULT,\
             _CONFIG7H_DEFAULT);
#pragma romdata

#define	led	LATAbits.LATA0

void main(void)
{
	ADCON1 = 7;
	TRISAbits.TRISA0 = 0;

	while (1)
	{
		led = 1;
		Delay10KTCYx(50);
		led = 0;
		Delay10KTCYx(50);
	}
}
