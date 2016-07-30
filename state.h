#ifndef STATE_H_
#define STATE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "analog.h"

#define STATE_SAMPLE_VOLTAGE		0
#define STATE_SAMPLE_CURRENT		1
// must always be last
#define STATE_NUM_SAMPLES			2

struct {
	uint16_t voltage;
	uint16_t current;
	uint8_t nextUpdate;
} state;

void State_Update();

#endif /* STATE_H_ */
