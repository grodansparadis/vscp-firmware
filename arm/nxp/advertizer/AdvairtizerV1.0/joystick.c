/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : joystick.c                                             *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>
/**********************************************************************
 * init_joystick
***********************************************************************/
void init_joystick(void){
PINSEL2 &= ~(1<<3);//Pins P1.25-16 are used as GPIO pins.
}
/**********************************************************************
 * joystick
***********************************************************************/
int get_stick_up(void){
return debounce(F_IO1PIN, STICK_UP_BIT);
}
int get_stick_down(void){
return debounce(F_IO1PIN, STICK_DOWN_BIT);
}
int get_stick_left(void){
return debounce(F_IO1PIN, STICK_LEFT_BIT);
}
int get_stick_right(void){
return debounce(F_IO1PIN, STICK_RIGHT_BIT);
}
int get_stick_button(void){
return debounce(F_IO1PIN, STICK_BUTTON_BIT);
}

void get_joystick(void){
}
/***************************************************************************
 * Returns:
 *    -1 : key changed or bouncing
 *    -2 : already returned in act. state
 *     0 : key released
 *     1 : key pressed
 *    
 *      KEY_DEBOUNCE Time 20ms     configure in joystick.h
 *	if (debounce(port, pinstate))
*****************************************************************************/
int debounce(U32 port, U32 button)
{
	static uint32_t lastchangetime;
	static boolean  laststate = FALSE;
	static boolean  lastret = TRUE;
        boolean actstate;
	
        actstate = (port & button) ? FALSE : TRUE; // TRUE if pressed (active low)
if ( actstate)
return 1;

	if (laststate != actstate) {
		lastchangetime = getSysTICs();
		laststate = actstate;
	}
	else {
		if (getElapsedSysTICs(lastchangetime) > KEY_DEBOUNCE) {
			if ( actstate == lastret ) return -2;
			if ( actstate) {
				lastret = TRUE;
				return 1;
			}
			else {
				lastret = FALSE;
				return 0;
			}
		}
	}
	return -1; // changed or bouncing
}


