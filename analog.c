/*############################################################################

 ############################################################################*/

#include "analog.h"

void ADC_Update(void) {
	if (analog.request) {
		// save sense channel
		uint8_t sense = ADMUX;
		SFIOR = 0x00;  // Analog Comperator aus
		// select channel and select AVCC reference
		ADMUX = analog.channel | (1<<REFS1) | (1<<REFS0);
		/*
		 * initialize ADC:
		 * Prescaler = 8 -> 1MHz, a little bit too fast, but should work
		 * Enable + Start
		 */

		ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS1) | (1 << ADPS0);
		// wait for ADC to finish
		while (ADCSRA & (1 << ADSC))
			;
		ADCSRA = 0x00;
		// reset ADMUX
		ADMUX = sense;
		SFIOR = 0x08;  // Analog Comperator ein
		analog.request = 0;
		analog.result = ADC;
		analog.done = 1;
	}
}

////############################################################################
////Init ADC
//void ADC_Init(void)
////############################################################################
//{
//	ADCSRA = 0xA6;  // Free Run & 1MHZ
//	ADMUX = 7 | IntRef;      // Kanal 7
//	ADCSRA |= 0x40; // Start
//}
//
////############################################################################
////Strom Analogwerte lesen
//void AdConvert(void)
////############################################################################
//{
//	unsigned int i = 0;
//	unsigned char sense;
//	sense = ADMUX;   // Sense-Kanal merken
//	ADMUX = 0x06 | IntRef;  // Kanal 6
//	SFIOR = 0x00;  // Analog Comperator aus
//	ADCSRA = 0xD3;  // Converter ein, single
//	ADCSRA |= 0x10;  // Ready l�schen
//	ADMUX = 0x06 | IntRef;  // Kanal 6
//	ADCSRA |= 0x40;  // Start
//	while (((ADCSRA & 0x10) == 0))
//		;
//	ADMUX = sense;   // zur�ck auf den Sense-Kanal
//	i = ADCW * 4;
////  if(i > 300) i = 300;
//	Strom = (i + Strom * 7) / 8;
//	if (Strom_max < Strom)
//		Strom_max = Strom;
//	ADCSRA = 0x00;
//	SFIOR = 0x08;  // Analog Comperator ein
//}
//
//############################################################################
//Strom Analogwerte lesen
unsigned int MessAD(unsigned char channel)
//############################################################################
{
	unsigned char sense;
	sense = ADMUX;   // Sense-Kanal merken
	channel |= (1 << REFS1) | (1 << REFS0);
	ADMUX = channel;  // Kanal 6
	SFIOR = 0x00;  // Analog Comperator aus
	ADCSRA = 0xD3;  // Converter ein, single
	ADCSRA |= 0x10;  // Ready l�schen
	ADMUX = channel;  // Kanal 6
	ADCSRA |= 0x40;  // Start
	while (((ADCSRA & 0x10) == 0))
		;
	ADMUX = sense;   // zur�ck auf den Sense-Kanal
	ADCSRA = 0x00;
	SFIOR = 0x08;  // Analog Comperator ein
	return (ADCW );
}

////############################################################################
////Strom Analogwerte lesen
//void FastADConvert(void)
////############################################################################
//{
//	unsigned int i = 0;
//	i = MessAD(6) * 4;
////  i = ADCW * 4;
//	if (i > 200)
//		i = 200;
//	Strom = i;  //(i + Strom * 1) / 2;
//	if (Strom_max < Strom)
//		Strom_max = Strom;
//	ADCSRA = 0x00;
//	SFIOR = 0x08;  // Analog Comperator ein
//}
