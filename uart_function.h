#ifndef UART_FUNCTION_H_
#define UART_FUNCTION_H_

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#define UART_BAUD 9600
#define UBRR_VALUE F_CPU/UART_BAUD/8-1
#include<avr/io.h>
#include<stdlib.h>

void uart_init(uint16_t ubrr);
void uart_transmit(uint8_t data);
void uart_transmit_ul(uint32_t value, uint8_t *string);
void uart_transmit_int(int16_t value, uint8_t *string);
void uart_transmit_entry(uint32_t a, int16_t b, int16_t c, uint8_t *string);

#endif
