#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Mi_USART.h"

/****************************************************************************************
*	Nombre: Inicia_Usart
*
*	Retorna: Nada
*
*	Parametros: UBRR, valor que se obtine de F_CPU/16/BAUD-1
*
*	Descripcion: Esta funcion inicialializa y configura los registros de la comunicacion USART
****************************************************************************************/
void Inicia_Usart(unsigned char nserial,unsigned int baudios){
	DDRD|=(1<<PIND1);
	DDRD&=~(1<<PIND0);
	switch(nserial){
		case 0:
		UCSR0A=(1<<U2X0);
		UCSR0B=(1<<RXEN0)|(1<<TXEN0);//activado recibir/transmitir datos, y la interrupcion de rx
		UCSR0C = (3<<UCSZ00);//configura la trasnmision a 8bits, 1bit de stop
		UBRR0H = (unsigned char)(baudios>>8);
		UBRR0L= (unsigned char)baudios;//el valor de dividir Fosc/16(9600) y restarle 1se trasnmitira a 9600 con un reloj de 8Mhz
		break;
		//case 1:
		///*UCSR1B=(1<<RXEN1)|(1<<TXEN1);//activado recibir/transmitir datos, y la interrupcion de rx
		//UCSR1C = (3<<UCSZ10);//configura la trasnmision a 8bits, 1bit de stop
		//UBRR1H = (unsigned char)(baudios>>8);
		//UBRR1L= (unsigned char)baudios;//el valor de dividir Fosc/16(9600) y restarle 1se trasnmitira a 9600 con un reloj de 8Mhz*/
		//break;
	}
}

/****************************************************************************************
*	Nombre: leerCaracter_Usart
*
*	Retorna: UDR0, registro donde se almacena los caracteres que recibe el pin Rx
*
*	Parametros: Nada
*
*	Descripcion: Esta funcion lee el carater que recibe el microntrolador por medio de USART
****************************************************************************************/
unsigned char leerCaracter_Usart(unsigned char nserial){
	switch(nserial){
		case 0:
		while ( !(UCSR0A & (1<<RXC0)) );//espera hasta que un nuevo caracter arribe
		return UDR0;//devuelve el caracter que ha llegado por el puerto RX
		break;
		//case 1:
		///*while ( !(UCSR1A & (1<<RXC1)) );//espera hasta que un nuevo caracter arribe
		//return UDR1;//devuelve el caracter que ha llegado por el puerto RX*/
		//break;
		default:
		return 0;
	}
}
/****************************************************************************************
*	Nombre: Enviar_Usart
*
*	Retorna: Nada
*
*	Parametros: STR, que es la cadena o simbolo a ser enviado por medio de USART
*
*	Descripcion: Esta funcion envia una cadena o carater por medio del pin Tx de USART
****************************************************************************************/
void Env_Usart(unsigned char nserial,char *STR){
	//unsigned int conteo=0;
	switch(nserial){
		case 0:
		while (*STR!='\0')
		{
			while(!(UCSR0A & (1<<UDRE0)));//espera a que el dato enviado sea recibido antes de enviar otro
			UDR0=*STR++;//enviando la cadena dat
		}
		break;
		case 1:
		/*while (*STR!='\0')
		{
			while(!(UCSR1A & (1<<UDRE1)));//espera a que el dato enviado sea recibido antes de enviar otro
			UDR1=*STR++;//enviando la cadena dat
		}*/
		break;
	}
}
/*

*/
void Env_Usart_P(unsigned char nserial,const char *STR){
	//unsigned int conteo=0;
	switch(nserial){
		case 0:
		while (pgm_read_byte(STR)!='\0')
		{
			while(!(UCSR0A & (1<<UDRE0)));//espera a que el dato enviado sea recibido antes de enviar otro
			UDR0=pgm_read_byte(STR++);//enviando la cadena dat
		}
		break;
		case 1:
		/*while (pgm_read_byte(STR)!='\0')
		{
			while(!(UCSR1A & (1<<UDRE1)));//espera a que el dato enviado sea recibido antes de enviar otro
			UDR1=pgm_read_byte(STR++);//enviando la cadena dat
		}*/
		break;
	}
}
/*

*/
char CaracterSinLeer_Usart(unsigned char nserial){
	char Status=0;
	switch(nserial){
		case 0:
		return Status=LLEGO_UN_CARACTER_USART0?1:0;
		break;
		/*case 1:
		//return Status=LLEGO_UN_CARACTER_USART1?1:0;
		break;*/
		default:
		return Status;
		break;
	}
}
/*
*/
void EnvCh_Usart(char c){
	while(!(UCSR0A & (1<<UDRE0)));//espera a que el dato enviado sea recibido antes de enviar otro
	UDR0=c;//enviando la cadena dat
}
/*
*/
void Env_Usart_RAW(char *STR,uint16_t L){
	for (uint16_t i=0;i<L;i++)
	{
		EnvCh_Usart(STR[i]);
	}
}