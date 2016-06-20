#include "controller.h"

uint8_t EEMEM eSettingsValid;
uint8_t EEMEM eP;
uint8_t EEMEM eI;
uint8_t EEMEM eRPMToPWM[CONTROL_FORWARD_ARRAY_LENGTH];

void control_Init(uint16_t *is, uint16_t *should) {
	control.is = is;
	control.should = should;
	control.integral = 0;
	control.lastTime = timer0.ms;
	control.out = 0;
}

void control_Update(uint8_t limited) {
	cli();
	uint16_t isBuffer = *control.is;
	sei();
	int16_t diff = *control.should - isBuffer;
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
	cli();
	uint16_t isBuffer = *control.is;
	sei();
	if (!control.sampleCharacteristic) {
		// initiate sampling process
		control.sampleCharacteristic = 1;
		control.sampleTimer = SetDelay(3000);
		control.out = MIN_PWM;
	} else {
		// currently sampling
		if (DelayElapsed(control.sampleTimer)) {
			// save current sample point
			if (isBuffer < (uint16_t) CONTROL_FORWARD_ARRAY_LENGTH * 100) {
				control.RPMToPWM[isBuffer / 100] = control.out;
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

void control_SaveSettings(void) {
	eeprom_write_byte(&eP, control.P);
	eeprom_write_byte(&eI, control.I);
	eeprom_write_block(control.RPMToPWM, eRPMToPWM,
			CONTROL_FORWARD_ARRAY_LENGTH);
	eeprom_write_byte(&eSettingsValid, 0x42);
}

void control_LoadSettings(void) {
	if (eeprom_read_byte(&eSettingsValid) == 0x42) {
		// EEPROM contains valid data
		control.P = eeprom_read_byte(&eP);
		control.I = eeprom_read_byte(&eI);
		eeprom_read_block(control.RPMToPWM, eRPMToPWM,
				CONTROL_FORWARD_ARRAY_LENGTH);
	} else {
		// no valid EEPROM data
		// -> fill with default values
		control.P = CONTROL_DEFAULT_P;
		control.I = CONTROL_DEFAULT_I;
		memset(control.RPMToPWM, 0, CONTROL_FORWARD_ARRAY_LENGTH);
	}
}
