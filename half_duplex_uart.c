#include "half_duplex_uart.h"


int HduWrite(HalfDuplexUart_t hdu, uint8_t* packetBuffer, size_t length)
{
    GPIO_setConfig(hdu.gpio, 0);
    int bytesWritten = UART_write(hdu.uart, packetBuffer, length);
    GPIO_setConfig(hdu.gpio, 1);
    return bytesWritten;
}

uint_fast32_t HduRead(HalfDuplexUart_t hdu, uint8_t* packetBuffer, size_t length)
{
	// HduWrite is responsible for making sure
	// gpio pin is set correctly for read
	// after it finishes writing
	return UART_read(hdu.uart, packetBuffer, length);
}
