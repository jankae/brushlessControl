#ifndef _TIMER0_H_
#define _TIMER0_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "BLMC.h"

struct {
	volatile uint16_t ms;
	uint8_t overflows;
} timer0;

extern volatile unsigned char Timer0Overflow;

void Timer0_Init(void);
void Delay_ms(unsigned int);
unsigned int SetDelay(unsigned int t);
char DelayElapsed(unsigned int t);

#endif

