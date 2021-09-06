#ifndef HALF_DUPLEX_UART_H
#define HALF_DUPLEX_UART_H

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

typedef struct {
	UART_Handle uart;
	unsigned int gpio;
} HalfDuplexUart_t;


int HduWrite(HalfDuplexUart_t uart, uint8_t* packetBuffer, size_t length);
uint_fast32_t HduRead(HalfDuplexUart_t uart, uint8_t* packetBuffer, size_t length);

#endif
