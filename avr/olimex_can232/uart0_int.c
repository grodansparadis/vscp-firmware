// vim:ts=4:sw=4:expandtab

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart0_int.h"


#define ubrr0 ( (F_CPU / (BAUDRATE * 16)) -1 )


static int8_t rx_buffer0[RX_BUFFER_SIZE0];

volatile uint8_t rx_wr_index0;
volatile uint8_t rx_rd_index0;
volatile uint8_t rx_counter0;

// This flag is set on USART0 Receiver buffer overflow
volatile uint8_t rx_buffer_overflow0;

static int8_t tx_buffer0[TX_BUFFER_SIZE0];

volatile uint8_t tx_wr_index0;
volatile uint8_t tx_rd_index0;
volatile uint8_t tx_counter0;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud rate: define BAUDRATE

void
uart0_init()
{
    rx_wr_index0 = rx_counter0 = rx_buffer_overflow0 = 0;
    tx_wr_index0 = tx_rd_index0 = tx_counter0 = 0;

    UCSR0A=0x00;
    UCSR0B=0xD8;
    UCSR0C=0x06;
    UBRR0H=ubrr0>>8;
    UBRR0L=ubrr0;

}

ISR(USART0_RX_vect)
{
    uint8_t status, data;

    status = UCSR0A;
    data   = UDR0;

    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {
        rx_buffer0[rx_wr_index0++] = data;
        if (rx_wr_index0 == RX_BUFFER_SIZE0)
            rx_wr_index0 = 0;
         ++rx_counter0;
        if (rx_counter0 >= RX_BUFFER_SIZE0)
        {
            rx_counter0=RX_BUFFER_SIZE0;
            rx_buffer_overflow0=1;
        }
    }
}

ISR(USART0_TX_vect)
{
    if (tx_counter0)
    {
        --tx_counter0;
        UDR0 = tx_buffer0[tx_rd_index0];
        if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0 = 0;
    }
}

void
uart0_putchar(char c)
{
    while (tx_counter0 == TX_BUFFER_SIZE0);
    if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
    {
        tx_buffer0[tx_wr_index0]=c;
        if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
        cli();
        ++tx_counter0;
        sei();
    }
    else
    {
        UDR0 = c;
    }
}

char
uart0_getchar()
{
    char data;

    while (rx_counter0 == 0)
        asm volatile("wdr"::);

    data = rx_buffer0[rx_rd_index0++];
    if (rx_rd_index0 == RX_BUFFER_SIZE0)
        rx_rd_index0=0;
    cli();
    --rx_counter0;
    sei();

    return data;
}

