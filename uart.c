#include "uart.h"

void uart_Init(void) {

	UCSRB = (1 << TXEN) | (1 << RXEN);
	// UART Double Speed (U2X)
	UCSRA |= (1 << U2X);

	// set baud rate
	UBRRH = 0;
	UBRRL = (F_CPU / (BAUD_RATE * 8L) - 1);
}

void uart_SendDebug(void) {
	if (!uart.transmissionActive) {
		uint8_t pt = 0;
		uint8_t i;
		for (i = 0; i < UART_NUM_DEBUG; i++) {
			uart.sendBuffer[pt++] = 'A';
			uart.sendBuffer[pt++] = '0' + i;
			uart.sendBuffer[pt++] = ':';
			uart.sendBuffer[pt++] = ' ';
			uart.sendBuffer[pt++] = '0' + uart.Analog[i] / 10000;
			uart.sendBuffer[pt++] = '0' + (uart.Analog[i] / 1000) % 10;
			uart.sendBuffer[pt++] = '0' + (uart.Analog[i] / 100) % 10;
			uart.sendBuffer[pt++] = '0' + (uart.Analog[i] / 10) % 10;
			uart.sendBuffer[pt++] = '0' + uart.Analog[i] % 10;
			uart.sendBuffer[pt++] = ' ';
		}
		uart.sendBuffer[pt - 1] = '\n';
		uart.length = pt;
		uart.sendBufPos = 0;
		uart.transmissionActive = 1;
		UCSRB |= (1 << UDRIE);
	}
}

ISR(USART_UDRE_vect) {
	if (uart.sendBufPos < uart.length) {
		// still some data left
		UDR = uart.sendBuffer[uart.sendBufPos++];
	} else {
		// finished sending buffer
		UCSRB &= ~(1 << UDRIE);
		uart.transmissionActive = 0;
	}
}
