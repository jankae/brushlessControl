#include "uart.h"

void uart_Init(void) {

	UCSRB = (1 << TXEN) | (1 << RXEN);
	// UART Double Speed (U2X)
	UCSRA |= (1 << U2X);

#if !UART_DEBUG
	UCSRB |= (1 << RXCIE);
#endif
	// set baud rate
	UBRRH = 0;
	UBRRL = (F_CPU / (BAUD_RATE * 8L) - 1);
}

#if UART_DEBUG
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
		uart.sendPointer = uart.sendBuffer;
		uart_StartTransmission();
	}
}
#endif

void uart_StartTransmission(void) {
	uart.sendBufPos = 0;
	uart.transmissionActive = 1;
	UCSRB |= (1 << UDRIE);
}

ISR(USART_UDRE_vect) {
	if (uart.sendBufPos < uart.length) {
		// still some data left
		UDR = uart.sendPointer[uart.sendBufPos++];
	} else {
		// finished sending buffer
		UCSRB &= ~(1 << UDRIE);
		uart.transmissionActive = 0;
	}
}

#if !UART_DEBUG
ISR(USART_RXC_vect) {
	uint8_t udr = UDR;
	switch (uart.RXState) {
	case UART_RX_IDLE:
		if (udr == UART_RX_START)
			uart.RXState = UART_RX_START;
		break;
	case UART_RX_START:
		switch (udr) {
		case UART_RX_SAVE_CONTROL:
			control_SaveSettings();
			uart.RXState = UART_RX_IDLE;
			break;
		case UART_RX_LOAD_CONTROL:
			control_LoadSettings();
			uart.RXState = UART_RX_IDLE;
			break;
		case UART_RX_SEND_PI:
			if (!uart.transmissionActive) {
				uart.sendBuffer[0] = UART_RX_SEND_PI;
				uart.sendBuffer[1] = control.P;
				uart.sendBuffer[2] = control.I;
				uart.length = 3;
				uart.sendPointer = uart.sendBuffer;
				uart_StartTransmission();
			}
			uart.RXState = UART_RX_IDLE;
			break;
		case UART_RX_RECEIVE_PI:
		case UART_RX_RECEIVE_FORWARD:
		case UART_RX_SET_PWM:
		case UART_RX_SET_RPM:
			uart.RXState = udr;
			uart.RXCount = 0;
			break;
		case UART_RX_SEND_FORWARD:
			if (!uart.transmissionActive) {
				uart.length = CONTROL_FORWARD_ARRAY_LENGTH;
				uart.sendPointer = control.RPMToPWM;
				uart_StartTransmission();
			}
			uart.RXState = UART_RX_IDLE;
			break;
		case UART_RX_SAMPLE_FORWARD:
			uart.sampleFowardRequest = 1;
			uart.RXState = UART_RX_IDLE;
			break;
		case UART_RX_DISABLE_OVERRIDE:
			uart.PWMActive = 0;
			uart.RPMActive = 0;
			uart.RXState = UART_RX_IDLE;
			break;
		default:
			uart.RXState = UART_RX_IDLE;
			break;
		}
		break;
	case UART_RX_RECEIVE_PI:
		switch (uart.RXCount) {
		case 0:
			control.P = udr;
			break;
		case 1:
			control.I = udr;
			uart.RXState = UART_RX_IDLE;
			break;
		default:
			uart.RXState = UART_RX_IDLE;
			break;
		}
		uart.RXCount++;
		break;
	case UART_RX_RECEIVE_FORWARD:
		if (uart.RXCount < CONTROL_FORWARD_ARRAY_LENGTH) {
			control.RPMToPWM[uart.RXCount] = udr;
			uart.RXCount++;
		} else {
			uart.RXState = UART_RX_IDLE;
		}
		break;
	case UART_RX_SET_PWM:
		switch (uart.RXCount) {
		case 0:
			uart.PWM = udr;
			uart.PWMActive = 1;
			uart.RPMActive = 0;
			uart.RXState = UART_RX_IDLE;
			break;
		default:
			uart.RXState = UART_RX_IDLE;
			break;
		}
		uart.RXCount++;
		break;
	case UART_RX_SET_RPM:
		switch (uart.RXCount) {
		case 0:
			uart.RPMActive = 0;
			uart.PWMActive = 0;
			uart.RPM = (uint16_t) udr << 8;
			break;
		case 1:
			uart.RPM += udr;
			uart.RPMActive = 1;
			uart.RXState = UART_RX_IDLE;
			break;
		default:
			uart.RXState = UART_RX_IDLE;
			break;
		}
		uart.RXCount++;
		break;
	}
}
#endif
