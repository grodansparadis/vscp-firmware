//buttons.c
#include <lpc21xx.h>
#include "config.h"
#include "buttons.h"


// init buttons port
void InitButtons(void) {
	IODIR0 = PIO0_BUTTON1 | PIO0_BUTTON2 ;
}

