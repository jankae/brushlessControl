#ifndef ANALOG_H_
#define ANALOG_H_

#include <avr/io.h>

struct {
	uint8_t request :1;
	uint8_t done :1;
	uint8_t channel;
	uint16_t result;
} analog;

/*
 * Reads an ADC channel if requested in analog struct.
 * May only be called from the commutation interrupt or
 * while the commutation interrupt is disabled.
 */
void ADC_Update(void);

void ADC_Init(void);
void GetAnalogWerte(void);
void AdConvert(void);
void FastADConvert(void);

#endif
