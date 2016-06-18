// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Regler f�r Brushless-Motoren
// + ATMEGA8 mit 8MHz
// + (c) 01.2007 Holger Buss
// + Nur f�r den privaten Gebrauch / NON-COMMERCIAL USE ONLY
// + Keine Garantie auf Fehlerfreiheit
// + Kommerzielle Nutzung nur mit meiner Zustimmung
// + Der Code ist f�r die Hardware BL_Ctrl V1.0 entwickelt worden
// + www.mikrocontroller.com
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "main.h"
#include "uart.h"

#define MAX_SENDE_BUFF     100

unsigned volatile char SendeBuffer[MAX_SENDE_BUFF];
unsigned volatile char UebertragungAbgeschlossen = 1;

struct str_DebugOut    DebugOut;


int Debug_Timer;

void SendUart(void)
{
 static unsigned int ptr = 0;
 unsigned char tmp_tx;
 if(!(UCSRA & 0x40)) return;
 if(!UebertragungAbgeschlossen)
  {
   ptr++;                    // die [0] wurde schon gesendet
   tmp_tx = SendeBuffer[ptr];
   if((tmp_tx == '\r') || (ptr == MAX_SENDE_BUFF))
    {
     ptr = 0;
     UebertragungAbgeschlossen = 1;
    }
   USR |= (1<TXC);
   UDR = tmp_tx;
  }
  else ptr = 0;
}



// --------------------------------------------------------------------------
void SendOutData(unsigned int *snd, unsigned char num)
{
 unsigned int pt = 0;


// SendeBuffer[pt++] = '#';               // Startzeichen
// SendeBuffer[pt++] = modul;             // Adresse (a=0; b=1,...)
// SendeBuffer[pt++] = cmd;		        // Commando

 uint8_t i;
 for(i=0;i<num;i++)
  {
	 SendeBuffer[pt++] = ' ';
	 SendeBuffer[pt++] = 'A';
	 SendeBuffer[pt++] = '0' + i;
	 SendeBuffer[pt++] = ':';
	 SendeBuffer[pt++] = ' ';
	 SendeBuffer[pt++] = '0' + snd[i] / 10000;
	 SendeBuffer[pt++] = '0' + (snd[i] / 1000)%10;
	 SendeBuffer[pt++] = '0' + (snd[i] / 100)%10;
	 SendeBuffer[pt++] = '0' + (snd[i] / 10)%10;
	 SendeBuffer[pt++] = '0' + snd[i] % 10;
  }
 SendeBuffer[pt++] = '\n';
// AddCRC(pt);
}



//############################################################################
//Routine f�r die Serielle Ausgabe
int uart_putchar (char c)
//############################################################################
{
	if (c == '\n')
		uart_putchar('\r');
	//Warten solange bis Zeichen gesendet wurde
	loop_until_bit_is_set(USR, UDRE);
	//Ausgabe des Zeichens
	UDR = c;

	return (0);
}

//############################################################################
//INstallation der Seriellen Schnittstelle
void UART_Init (void)
//############################################################################
{
	//Enable TXEN im Register UCR TX-Data Enable & RX Enable

	UCR=(1 << TXEN) | (1 << RXEN);
    // UART Double Speed (U2X)
	USR   |= (1<<U2X);
	// RX-Interrupt Freigabe

//	UCSRB |= (1<<RXCIE);    // serieller Empfangsinterrupt

	// TX-Interrupt Freigabe
//	UCSRB |= (1<<TXCIE);           

	//Teiler wird gesetzt 
	UBRR= (SYSCLK / (BAUD_RATE * 8L) -1 );
	//�ffnet einen Kanal f�r printf (STDOUT)
//	fdevopen (uart_putchar, NULL);
    Debug_Timer = SetDelay(200);
//    // Version beim Start ausgeben (nicht sch�n, aber geht... )
//	uart_putchar ('\n');uart_putchar ('B');uart_putchar ('L');uart_putchar (':');
//	uart_putchar ('V');uart_putchar (0x30 + VERSION_HAUPTVERSION);uart_putchar ('.');uart_putchar (0x30 + VERSION_NEBENVERSION/10); uart_putchar (0x30 + VERSION_NEBENVERSION%10);
//    uart_putchar ('\n');uart_putchar('H');uart_putchar('W');uart_putchar (':');uart_putchar('1');uart_putchar('.');uart_putchar ((0x30-10) + HwVersion);
//	uart_putchar ('\n');uart_putchar ('A');uart_putchar ('D');uart_putchar ('R'); uart_putchar (':'); uart_putchar (0x30 + MotorAdresse);

}




//---------------------------------------------------------------------------------------------
void DatenUebertragung(void)
{
 if((CheckDelay(Debug_Timer) && UebertragungAbgeschlossen))	 // im Singlestep-Betrieb in jedem Schtitt senden
    	 {
    	  SendOutData(DebugOut.Analog,8);
       	  Debug_Timer = SetDelay(50);   // Sendeintervall
    	 }
}
