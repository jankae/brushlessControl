#ifndef _TWI_SLAVE_H_
#define _TWI_SLAVE_H_

#include <avr/io.h>
#include <util/twi.h>
#include "timer0.h"

extern unsigned char I2C_RXBuffer;
extern unsigned char Byte_Counter;

#define TWI_BUS_ERR_1   0x00
#define TWI_BUS_ERR_2   0xF8

// Status Slave RX Mode
#define SR_SLA_ACK      0x60
#define SR_LOST_ACK     0x68
#define SR_GEN_CALL_ACK 0x70
#define GEN_LOST_ACK    0x78
#define SR_PREV_ACK     0x80
#define SR_PREV_NACK    0x88
#define GEN_PREV_ACK    0x90
#define GEN_PREV_NACK   0x98
#define STOP_CONDITION  0xA0
#define REPEATED_START  0xA0

// Status Slave TX mode
#define SW_SLA_ACK      0xA8
#define SW_LOST_ACK     0xB0
#define SW_DATA_ACK     0xB8
#define SW_DATA_NACK    0xC0
#define SW_LAST_ACK     0xC8

#define TWI_TX_BUFFER_LENGTH	4
#define TWI_STATE_IDLE			0x00
#define TWI_STATE_REC_PWM		0x01
#define TWI_STATE_REC_RPM		0x02

struct {
	uint16_t RPM;
	uint8_t PWM;
	uint8_t controllerActive;
	uint8_t TXBuffer[TWI_TX_BUFFER_LENGTH];
	uint8_t bytecount;
	uint8_t state;
	uint8_t buffer;
	uint16_t timeout;
}twi;

void InitIC2_Slave (uint8_t adr);

#endif

