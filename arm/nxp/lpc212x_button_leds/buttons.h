// button.h
#include <LPC21xx.h>

#define BUT1_PRESSED   IOPIN0 & PIO0_BUTTON1
#define BUT2_PRESSED   IOPIN0 & PIO0_BUTTON2

// init buttons port
void InitButtons(void);

// Simple delay
void DelayS (unsigned long a);
