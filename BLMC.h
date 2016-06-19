#ifndef BLMC_H_
#define BLMC_H_

#include "main.h"
#include "timer0.h"

#define BLDC_NUM_POLES			14

struct {
	uint8_t phase;
	uint8_t commutationActive;
	uint16_t RPM;
} bldc;

void BLDC_EnableAutoCommutation();
void BLDC_DisableAutoCommutation();
void BLDC_Manuell(void);

#ifdef  _32KHZ 
#define PWM_C_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0A;}
#define PWM_B_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0C;}
#define PWM_A_ON  {TCCR1A = 0xAD; TCCR2  = 0x69;DDRB = 0x08;}
#define PWM_OFF   {TCCR1A = 0x0D; TCCR2  = 0x49;PORTC &= ~0x0E;}
#endif 

#ifdef  _16KHZ 
#define PWM_C_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x02;} 
#define PWM_B_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x04;}
#define PWM_A_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x08;}
#define PWM_OFF   {OCR1A = 0; OCR1B = 0; OCR2  = 0; TCCR1A = 0x01; TCCR2  = 0x41; DDRB = 0x0E; PORTB &= ~0x0E;}
#endif 

#define  STEUER_A_H {PWM_A_ON}
#define  STEUER_B_H {PWM_B_ON}
#define  STEUER_C_H {PWM_C_ON}

#define  STEUER_A_L {PORTD &= ~0x30; PORTD |= 0x08;}
#define  STEUER_B_L {PORTD &= ~0x28; PORTD |= 0x10;}
#define  STEUER_C_L {PORTD &= ~0x18; PORTD |= 0x20;}
#define  STEUER_OFF {PORTD &= ~0x38; PWM_OFF; }
#define  FETS_OFF {PORTD &= ~0x38; PORTB &= ~0x0E; }

#define SENSE_A ADMUX = 0 | IntRef;
#define SENSE_B ADMUX = 1 | IntRef;
#define SENSE_C ADMUX = 2 | IntRef;

#define ClrSENSE            ACSR |= 0x10
#define SENSE               ((ACSR & 0x10))
#define SENSE_L             (!(ACSR & 0x20))
#define SENSE_H             ((ACSR & 0x20))

#define SENSE_FALLING_INT    ACSR &= ~0x01
#define SENSE_RISING_INT    ACSR |= 0x03
#define SENSE_TOGGLE_INT    ACSR &= ~0x03

#endif //BLMC_H_
