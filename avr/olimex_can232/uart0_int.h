#ifndef UART0_INT_H
#define UART0_INT_H

#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 8

// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 8


void
uart0_putchar(char c);

char
uart0_getchar() ;

void
uart0_init();

#endif // UART0_INT_H
