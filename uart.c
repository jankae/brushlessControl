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
#define MAX_EMPFANGS_BUFF  100

unsigned volatile char SIO_Sollwert = 0;
unsigned volatile char SioTmp = 0;
unsigned volatile char SendeBuffer[MAX_SENDE_BUFF];
unsigned volatile char RxdBuffer[MAX_EMPFANGS_BUFF];
unsigned volatile char NeuerDatensatzEmpfangen = 0;
unsigned volatile char UebertragungAbgeschlossen = 1;
unsigned char MeineSlaveAdresse;
unsigned char MotorTest[4] = {0,0,0,0};
unsigned volatile char AnzahlEmpfangsBytes = 0;

struct str_DebugOut    DebugOut;


int Debug_Timer;


SIGNAL(INT_VEC_TX)
{
}

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
void Decode64(unsigned char *ptrOut, unsigned char len, unsigned char ptrIn,unsigned char max)  // Wohin mit den Daten; Wie lang; Wo im RxdBuffer
{
 unsigned char a,b,c,d;
 unsigned char ptr = 0;
 unsigned char x,y,z;
 while(len)
  {
   a = RxdBuffer[ptrIn++] - '=';
   b = RxdBuffer[ptrIn++] - '=';
   c = RxdBuffer[ptrIn++] - '=';
   d = RxdBuffer[ptrIn++] - '=';
   if(ptrIn > max - 2) break;     // nicht mehr Daten verarbeiten, als empfangen wurden

   x = (a << 2) | (b >> 4);
   y = ((b & 0x0f) << 4) | (c >> 2);
   z = ((c & 0x03) << 6) | d;

   if(len--) ptrOut[ptr++] = x; else break;
   if(len--) ptrOut[ptr++] = y; else break;
   if(len--) ptrOut[ptr++] = z;	else break;
  }

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++ Empfangs-Part der Daten�bertragung
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SIGNAL(INT_VEC_RX)
{

#if  X3D_SIO == 1
 static unsigned char serPacketCounter = 100;
    SioTmp = UDR;
	if(SioTmp == 0xF5)     // Startzeichen
	 {
  	  serPacketCounter = 0;
	 }
	else
	 {
    	  if(++serPacketCounter == MotorAdresse) // (1-4)
    	  {
    		SIO_Sollwert = SioTmp;
            SIO_Timeout = 200; // werte f�r 200ms g�ltig
    	  }
    	  else
    	  {
    	   if(serPacketCounter > 100) serPacketCounter = 100;
		  }
	 }
#else
 static unsigned int crc;
 static unsigned char crc1,crc2,buf_ptr;
 static unsigned char UartState = 0;
 unsigned char CrcOkay = 0;

 SioTmp = UDR;
 if(buf_ptr >= MAX_EMPFANGS_BUFF)    UartState = 0;
 if(SioTmp == '\r' && UartState == 2)
  {
   UartState = 0;
   crc -= RxdBuffer[buf_ptr-2];
   crc -= RxdBuffer[buf_ptr-1];
   crc %= 4096;
   crc1 = '=' + crc / 64;
   crc2 = '=' + crc % 64;
   CrcOkay = 0;
   if((crc1 == RxdBuffer[buf_ptr-2]) && (crc2 == RxdBuffer[buf_ptr-1])) CrcOkay = 1; else { CrcOkay = 0; };
   if(CrcOkay) // Datensatz schon verarbeitet
    {
     //NeuerDatensatzEmpfangen = 1;
	 AnzahlEmpfangsBytes = buf_ptr;

     RxdBuffer[buf_ptr] = '\r';
	 if(/*(RxdBuffer[1] == MeineSlaveAdresse || (RxdBuffer[1] == 'a')) && */(RxdBuffer[2] == 'R')) wdt_enable(WDTO_250MS); // Reset-Commando
     uart_putchar(RxdBuffer[2]);
	 if (RxdBuffer[2] == 't') // Motortest
            { Decode64((unsigned char *) &MotorTest[0],sizeof(MotorTest),3,AnzahlEmpfangsBytes);
			  SIO_Sollwert = MotorTest[MotorAdresse - 1];
              SIO_Timeout = 500; // werte f�r 500ms g�ltig

			}
	}
  }
  else
  switch(UartState)
  {
   case 0:
          if(SioTmp == '#' && !NeuerDatensatzEmpfangen) UartState = 1;  // Startzeichen und Daten schon verarbeitet
		  buf_ptr = 0;
		  RxdBuffer[buf_ptr++] = SioTmp;
		  crc = SioTmp;
          break;
   case 1: // Adresse auswerten
		  UartState++;
		  RxdBuffer[buf_ptr++] = SioTmp;
		  crc += SioTmp;
		  break;
   case 2: //  Eingangsdaten sammeln
		  RxdBuffer[buf_ptr] = SioTmp;
		  if(buf_ptr < MAX_EMPFANGS_BUFF) buf_ptr++;
		  else UartState = 0;
		  crc += SioTmp;
		  break;
   default:
          UartState = 0;
          break;
  }


#endif
};


// --------------------------------------------------------------------------
void AddCRC(unsigned int wieviele)
{
 unsigned int tmpCRC = 0,i;
 for(i = 0; i < wieviele;i++)
  {
   tmpCRC += SendeBuffer[i];
  }
   tmpCRC %= 4096;
   SendeBuffer[i++] = '=' + tmpCRC / 64;
   SendeBuffer[i++] = '=' + tmpCRC % 64;
   SendeBuffer[i++] = '\r';
  UebertragungAbgeschlossen = 0;
  UDR = SendeBuffer[0];
}


// --------------------------------------------------------------------------
void SendOutData(unsigned char cmd,unsigned char modul, unsigned int *snd, unsigned char num)
{
 unsigned int pt = 0;
 unsigned char a,b,c;
 unsigned char ptr = 0;


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

// --------------------------------------------------------------------------
void WriteProgramData(unsigned int pos, unsigned char wert)
{
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

	UCSRB |= (1<<RXCIE);    // serieller Empfangsinterrupt

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
    	  SendOutData('D',MeineSlaveAdresse,DebugOut.Analog,8);
       	  Debug_Timer = SetDelay(50);   // Sendeintervall
    	 }
}
