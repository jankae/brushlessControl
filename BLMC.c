/*#######################################################################################
 Flight Control
 #######################################################################################*/
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Regler f�r Brushless-Motoren
// + ATMEGA8 mit 8MHz
// + Nur f�r den privaten Gebrauch / NON-COMMERCIAL USE ONLY
// + Copyright (c) 12.2007 Holger Buss
// + www.MikroKopter.com
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Es gilt f�r das gesamte Projekt (Hardware, Software, Bin�rfiles, Sourcecode und Dokumentation), 
// + dass eine Nutzung (auch auszugsweise) nur f�r den privaten (nicht-kommerziellen) Gebrauch zul�ssig ist. 
// + Sollten direkte oder indirekte kommerzielle Absichten verfolgt werden, ist mit uns (info@mikrokopter.de) Kontakt 
// + bzgl. der Nutzungsbedingungen aufzunehmen. 
// + Eine kommerzielle Nutzung ist z.B.Verkauf von MikroKoptern, Best�ckung und Verkauf von Platinen oder Baus�tzen,
// + Verkauf von Luftbildaufnahmen, usw.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Werden Teile des Quellcodes (mit oder ohne Modifikation) weiterverwendet oder ver�ffentlicht, 
// + unterliegen sie auch diesen Nutzungsbedingungen und diese Nutzungsbedingungen incl. Copyright m�ssen dann beiliegen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Sollte die Software (auch auszugesweise) oder sonstige Informationen des MikroKopter-Projekts
// + auf anderen Webseiten oder sonstigen Medien ver�ffentlicht werden, muss unsere Webseite "http://www.mikrokopter.de"
// + eindeutig als Ursprung verlinkt werden
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Keine Gew�hr auf Fehlerfreiheit, Vollst�ndigkeit oder Funktion
// + Benutzung auf eigene Gefahr
// + Wir �bernehmen keinerlei Haftung f�r direkte oder indirekte Personen- oder Sachsch�den
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Die Portierung oder Nutzung der Software (oder Teile davon) auf andere Systeme (ausser der Hardware von www.mikrokopter.de) ist nur 
// + mit unserer Zustimmung zul�ssig
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Die Funktion printf_P() unterliegt ihrer eigenen Lizenz und ist hiervon nicht betroffen
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Redistributions of source code (with or without modifications) must retain the above copyright notice, 
// + this list of conditions and the following disclaimer.
// +   * Neither the name of the copyright holders nor the names of contributors may be used to endorse or promote products derived
// +     from this software without specific prior written permission.
// +   * The use of this project (hardware, software, binary files, sources and documentation) is only permittet 
// +     for non-commercial use (directly or indirectly)
// +     Commercial use (for excample: selling of MikroKopters, selling of PCBs, assembly, ...) is only permitted 
// +     with our written permission
// +   * If sources or documentations are redistributet on other webpages, out webpage (http://www.MikroKopter.de) must be 
// +     clearly linked as origin 
// +   * porting the sources to other systems or using the software on other systems (except hardware from www.mikrokopter.de) is not allowed
// +  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// +  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// +  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// +  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// +  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// +  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// +  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// +  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN// +  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// +  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// +  POSSIBILITY OF SUCH DAMAGE. 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "BLMC.h"

//############################################################################
// + Interruptroutine
// + Wird durch den Analogkomperator ausgel�st
// + Dadurch wird das Kommutieren erzeugt
ISR(ANA_COMP_vect)
//############################################################################
{
	uint8_t cnt0 = TCNT0;
	uint8_t additionalOverflows = 0;
	if (cnt0 < 128 && (TIFR & (1 << TOV0))) {
		// timer overflowed since entering interrupt
		additionalOverflows = 1;
	}
	switch (bldc.phase) {
	case 0:
		STEUER_A_H
		;
		STEUER_C_L
		;
//			ADC_Update();
		SENSE_FALLING_INT;
		SENSE_B
		;
		bldc.phase++;
		break;
	case 1:
		STEUER_C_L
		;
		STEUER_B_H
		;
		ADC_Update();
		SENSE_A
		;
		SENSE_RISING_INT;
		bldc.phase++;
		break;
	case 2:
		STEUER_B_H
		;
		STEUER_A_L
		;
		ADC_Update();
		SENSE_C
		;
		SENSE_FALLING_INT;
		bldc.phase++;
		break;
	case 3:
		STEUER_A_L
		;
		STEUER_C_H
		;
		ADC_Update();
		SENSE_B
		;
		SENSE_RISING_INT;
		bldc.phase++;
		break;
	case 4:
		STEUER_C_H
		;
		STEUER_B_L
		;
		ADC_Update();
		SENSE_A
		;
		SENSE_FALLING_INT;
		bldc.phase++;
		break;
	case 5:
		STEUER_B_L
		;
		STEUER_A_H
		;
		ADC_Update();
		SENSE_C
		;
		SENSE_RISING_INT;
		bldc.phase = 0;
		break;
	}
	// reset comparator interrupt flag if necessary
	// (could have been triggered during ADC reading)
	if (ACSR & (1 << ACI)) {
		ACSR |= (1 << ACI);
	}
	static uint8_t numberOfCommutations = 0;
	static uint8_t TIM0atLastCommutation = 0;
	if (numberOfCommutations++ >= 5) {
		numberOfCommutations = 0;
		// calculate time since last commutation
		if (additionalOverflows)
			timer0.overflows++;
		uint16_t time = (uint16_t) timer0.overflows << 8;
		timer0.overflows = 0;
		if (additionalOverflows)
			timer0.overflows = 255;
		if (cnt0 > TIM0atLastCommutation) {
			time += (cnt0 - TIM0atLastCommutation);
		} else {
			time -= (TIM0atLastCommutation - cnt0);
		}
		TIM0atLastCommutation = cnt0;
		bldc.RPM = (60000000L / (BLDC_NUM_POLES / 2)) / time;
	}
	ZeitZumAdWandeln = 0;
}

inline void BLDC_EnableAutoCommutation() {
	bldc.commutationActive = 1;
	ACSR |= (1 << ACIE);
}
inline void BLDC_DisableAutoCommutation() {
	bldc.commutationActive = 0;
	ACSR &= ~(1 << ACIE);
}

//############################################################################
//
void BLDC_Manuell(void)
//############################################################################
{
	switch (bldc.phase) {
	case 0:
		STEUER_A_H
		;
		STEUER_B_L
		;
		SENSE_C
		;
		SENSE_RISING_INT;
		break;
	case 1:
		STEUER_A_H
		;
		STEUER_C_L
		;
		SENSE_B
		;
		SENSE_FALLING_INT;
		break;
	case 2:
		STEUER_B_H
		;
		STEUER_C_L
		;
		SENSE_A
		;
		SENSE_RISING_INT;
		break;
	case 3:
		STEUER_B_H
		;
		STEUER_A_L
		;
		SENSE_C
		;
		SENSE_FALLING_INT;
		break;
	case 4:
		STEUER_C_H
		;
		STEUER_A_L
		;
		SENSE_B
		;
		SENSE_RISING_INT;
		break;
	case 5:
		STEUER_C_H
		;
		STEUER_B_L
		;
		SENSE_A
		;
		SENSE_FALLING_INT;
		break;
	}
}

