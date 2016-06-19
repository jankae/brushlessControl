#include "timer0.h"

ISR(TIMER0_OVF_vect) {
	static unsigned char cnt;
//	Timer0Overflow++;
	if (timer0.overflows < 255) {
		timer0.overflows++;
	} else {
//		RPM = 0;
	}
	if (!cnt--) {
		cnt = 3;
		timer0.ms += 1;
//		if (I2C_Timeout)
//			I2C_Timeout--;
	}
}

void Timer0_Init(void) {
	// prescaler of 8
	TCCR0 = (1 << CS01);
	// enable overflow interrupt
	TIMSK |= (1 << TOIE0);
}

unsigned int SetDelay(unsigned int t) {
	return (timer0.ms + t - 1);
}

char DelayElapsed(unsigned int t) {
	return (((t - timer0.ms) & 0x8000) >> 8);
}

void Delay_ms(unsigned int w) {
	unsigned int akt;
	akt = SetDelay(w);
	while (!DelayElapsed(akt))
		;
}
