#include "controller.h"

void control_Init(uint16_t *is, uint16_t *should) {
	control.is = is;
	control.should = should;
	control.integral = 0;
	control.lastTime = timer0.ms;
	control.out = 0;
}

void control_Update(uint8_t limited) {
	int16_t diff = *control.should - *control.is;
	int16_t out = control.RPMToPWM[*control.should / 100];
	out += control.P * diff;
	uint16_t timediff = timer0.ms - control.lastTime;
	control.lastTime = timer0.ms;
	if (!limited) {
		control.integral += (diff / 10) * timediff;
		if (control.integral < -30000)
			control.integral = -30000;
		if (control.integral > 30000)
			control.integral = 30000;
	}
	out += (control.integral * control.I) / 100;
	if (out > 255)
		out = 255;
	else if (out < 0)
		out = 0;
	control.out = out;
}

void control_Sample(void) {
	if (!control.sampleCharacteristic) {
		// initiate sampling process
		control.sampleCharacteristic = 1;
		control.sampleTimer = SetDelay(3000);
		control.out = MIN_PWM;
	} else {
		// currently sampling
		if (DelayElapsed(control.sampleTimer)) {
			// save current sample point
			if (*control.is < CONTROL_FORWARD_ARRAY_LENGTH * 100) {
				control.RPMToPWM[*control.is / 100] = control.out;
			}
			if (control.out < MAX_PWM) {
				control.out++;
				control.sampleTimer = SetDelay(50);
			} else {
				// finished sampling
				control.out = 0;
				control.samplingFinished = 1;
				control.sampleCharacteristic = 0;
				// fill (possible) missing values
				uint8_t i;
				uint8_t lastValidPWM = MAX_PWM;
				for (i = CONTROL_FORWARD_ARRAY_LENGTH - 1; i > 0; i--) {
					if (control.RPMToPWM[i] == 0) {
						// missing value found
						// -> replace with last valid value
						control.RPMToPWM[i] = lastValidPWM;
					} else {
						// value is valid
						lastValidPWM = control.RPMToPWM[i];
					}
				}
			}
		}
	}
}
