#include "state.h"

uint8_t stateADCChannels[2] = {7, 6};

void State_Update() {
	if (analog.done) {
		analog.done = 0;
		switch (state.nextUpdate) {
		case STATE_SAMPLE_VOLTAGE:
			cli();
			state.voltage -= state.voltage >> 3;
			state.voltage += (55 * analog.result) >> 4;
			sei();
			break;
		case STATE_SAMPLE_CURRENT:
			cli();
			state.current -= state.current >> 3;
			state.current += (50 * analog.result);
			sei();
			break;
		}
		state.nextUpdate++;
		state.nextUpdate %= STATE_NUM_SAMPLES;
		analog.channel = stateADCChannels[state.nextUpdate];
		analog.request = 1;
	} else {
		analog.channel = stateADCChannels[state.nextUpdate];
		analog.request = 1;
	}
}
