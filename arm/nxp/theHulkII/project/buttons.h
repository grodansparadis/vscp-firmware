// button.h
#include <iolpc2214.h>

#define BUT1_PRESSED   IO0PIN_bit.P0_15
#define BUT2_PRESSED   IO0PIN_bit.P0_16

// init buttons port
void InitButtons(void);

