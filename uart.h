#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "controller.h"

/*
 * Set to 0 to disable debug messages and enable uart protocol.
 * Set to 1 to enable debug messages and disable uart protocol.
 */
#define UART_DEBUG				0

#define UART_SEND_BUF_LENGTH    80
#define BAUD_RATE 38400
#define UART_NUM_DEBUG			8

// uart RX states
#define UART_RX_IDLE				0
#define UART_RX_START				1
#define UART_RX_SAVE_CONTROL		2
#define UART_RX_LOAD_CONTROL		3
#define UART_RX_SEND_PI				4
#define UART_RX_RECEIVE_PI			5
#define UART_RX_SEND_FORWARD		6
#define UART_RX_RECEIVE_FORWARD		7
#define UART_RX_SAMPLE_FORWARD		8
#define UART_RX_SET_PWM				9
#define UART_RX_SET_RPM				10
#define UART_RX_DISABLE_OVERRIDE	11

struct {
	uint8_t sendBuffer[UART_SEND_BUF_LENGTH];
	uint8_t *sendPointer;
	uint16_t sendBufPos;
	uint16_t length;
	uint8_t transmissionActive :1;
	uint8_t sampleFowardRequest :1;
	uint8_t RPMActive :1;
	uint8_t PWMActive :1;
	uint16_t RPM;
	uint8_t PWM;
#if UART_DEBUG
	uint16_t Analog[UART_NUM_DEBUG];
#else
	uint8_t RXState;
	uint16_t RXCount;
#endif
} uart;

void uart_Init(void);
void uart_SendDebug(void);
void uart_StartTransmission(void);

#endif //_UART_H
