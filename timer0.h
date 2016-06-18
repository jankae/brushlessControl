#if defined (__AVR_ATmega8__)
#define TIMER_TEILER          CK8
//#define TIMER_RELOAD_VALUE    125
#endif

#define TIM0_START       TIMSK |= 0x01
#define TIMER2_INT_ENABLE   TIMSK |= 0x40

extern volatile unsigned int CountMilliseconds;
extern volatile unsigned char Timer0Overflow;


void Timer1_Init(void);
void Delay_ms(unsigned int);
unsigned int SetDelay (unsigned int t);
char CheckDelay (unsigned int t);

