#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_SEND_BUF_LENGTH     100
#define BAUD_RATE 38400
#define UART_NUM_DEBUG			8

struct {
	uint8_t sendBuffer[UART_SEND_BUF_LENGTH];
	uint8_t sendBufPos;
	uint8_t length;
	uint8_t transmissionActive;
	uint16_t Analog[UART_NUM_DEBUG];
} uart;

void uart_Init (void);
void uart_SendDebug(void);






#endif //_UART_H
