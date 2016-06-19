#include "twislave.h"

void twi_Init(uint8_t adr) {
	TWAR = adr;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
}

ISR (TWI_vect)
{
	switch (TWSR & 0xF8) {
	case SR_SLA_ACK:
		TWCR |= (1 << TWINT);
		twi.bytecount = 0;
		break;
	case SR_PREV_ACK:
		if (twi.bytecount == 0) {
			// first byte indicates whether RPM or PWM is transmitted
			uint8_t buf = TWDR;
			if (buf == TWI_STATE_REC_PWM)
				twi.state = TWI_STATE_REC_PWM;
			if (buf == TWI_STATE_REC_RPM)
				twi.state = TWI_STATE_REC_RPM;
		} else if (twi.bytecount == 1) {
			if (twi.state == TWI_STATE_REC_PWM) {
				twi.PWM = TWDR;
				twi.controllerActive = 0;
				twi.timeout = SetDelay(500);
			} else if (twi.state == TWI_STATE_REC_RPM) {
				// received first byte of RPM
				twi.buffer = TWDR;
			}
		} else if (twi.bytecount == 2) {
			if (twi.state == TWI_STATE_REC_RPM) {
				// received second byte of RPM
				twi.RPM = ((uint16_t) twi.buffer << 8) + TWDR;
				twi.controllerActive = 1;
				twi.timeout = SetDelay(500);
			}
		}
		twi.bytecount++;
		TWCR |= (1 << TWINT);
		break;

	case SW_SLA_ACK:
		// send first byte
		TWDR = twi.TXBuffer[0];
		twi.bytecount = 1;
		TWCR |= (1 << TWINT);
		break;

	case SW_DATA_ACK:
		TWDR = twi.TXBuffer[twi.bytecount++];
		TWCR |= (1 << TWINT);
		break;

	case TWI_BUS_ERR_2:
	case TWI_BUS_ERR_1:
		TWCR |= (1 << TWSTO) | (1 << TWINT);
		break;
	}
}

