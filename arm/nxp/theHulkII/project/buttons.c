//buttons.c
#include <iolpc2214.h>
#include "buttons.h"


// init buttons port
void InitButtons(void) {
  IO0DIR_bit.P0_15 = 0;    //set port0.15 as input
  IO0DIR_bit.P0_16 = 0;    //set port0.16 as input
}

