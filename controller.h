#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <avr/eeprom.h>
#include "timer0.h"
#include "BLMC.h"
#include "state.h"

#define CONTROL_FORWARD_ARRAY_LENGTH	250
/*
 * The critical P value can be calculated as:
 *
 * (1/td)*(pi/30)*8000*J*R/kt
 *
 * where:
 * td: maximum time difference between speed samples/controller executions [s]
 * J: Moment of inertia of the motor+load [kg*m^-2]
 * R: internal resistance of the motor [Ohm]
 * kt: Torque constant of the motor [Nm*I^-1]
 *
 * Short explanation:
 * The constant pi/30 stems from the conversion of the speed from RPM to rad/s.
 * The constant 8000 is needed in order to use uint8_t integers for P and I
 * (proportional control term will be divided by 8 after calculating it
 * -> result of controller is in mV).
 * (1/td)*(pi/30) converts the controller error to an acceleration which would
 * cancel this error in exactly one control step.
 * J*R/kt calculates the needed voltage in order to reach this acceleration.
 *
 * The actual P value must be kept below the critical P value.
 */
#define CONTROL_TD						0.007f
#define CONTROL_J						0.0000038f
#define CONTROL_R						0.290f
#define CONTROL_KT						0.0113
#define CONTROL_CRIT_P					((837.758f/CONTROL_TD)*CONTROL_J*CONTROL_R/CONTROL_KT)

#define CONTROL_DEFAULT_P				(CONTROL_CRIT_P*0.8)
#define CONTROL_DEFAULT_I				0

struct {
	uint16_t *is, *should;
	uint8_t out;
	uint16_t RPMToPWM[CONTROL_FORWARD_ARRAY_LENGTH];
	uint16_t lastTime;
	int16_t integral;
	uint8_t P, I;
	uint16_t sampleTimer;
	uint8_t sampleCharacteristic :1;
	uint8_t samplingFinished :1;
} control;

void control_Init(uint16_t *is, uint16_t *should);

void control_Sample(void);

void control_Update(uint8_t limited);

void control_SaveSettings(void);

void control_LoadSettings(void);

inline uint16_t control_VoltageToPWM(uint16_t Vout, uint16_t batVoltage) {
	return (((uint32_t) Vout) << 8) / batVoltage;
}

inline uint16_t control_PWMToVoltage(uint8_t PWM, uint16_t batVoltage) {
	return (((uint32_t) batVoltage) * PWM) >> 8;
}

#endif
