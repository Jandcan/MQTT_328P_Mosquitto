
#define F_CPU 8000000UL
#include <avr/io.h>

#define UART_Port (uint8_t)_SFR_IO_ADDR(PORTB)
#define UART_Tx 6
#define UART_Rx 7

#define BAUD_RATE 115200

#define TXDELAY (((F_CPU/BAUD_RATE)-7.0 +1.5)/3.0)
#define RXDELAY (((F_CPU/BAUD_RATE)-5.0 +1.5)/3.0)

void SoftEnviarCh(char ch);
void SoftEnviarStr(char *string);
void SoftEnviarStrRaw(char *string,uint16_t L);
void SoftEnviarStr_P(const char *Str);
char SoftLeerCh();
char* SoftLeerStr(char *Str,uint16_t MaxBuff);


void SoftEnviarCh(char ch) {
	uint8_t txdelay = TXDELAY;
	uint8_t delayCount = 0;

	__asm__ __volatile__(
		"0: cli\n\t"
		"   sbi %[uart_port]-1,%[uart_tx]\n\t"
		"   cbi %[uart_port],%[uart_tx]\n\t"
		"   in __tmp_reg__,%[uart_port]\n\t"
		"   ldi r25,3\n\t"
		"1: mov %[delayCount],%[txdelay]\n\t"
		"2: dec %[delayCount]\n\t"
		"   brne 2b\n\t"
		"   bst %[ch],0\n\t"
		"   bld __tmp_reg__,%[uart_tx]\n\t"
		"   lsr r25\n\t"
		"   ror %[ch]\n\t"
		"   out %[uart_port],__tmp_reg__\n\t"
		"   brne 1b\n\t"
		:
		  [ch] "+r" (ch),
		  [delayCount] "+r" (delayCount)
		:
		  [uart_port] "M" (UART_Port),
		  [uart_tx] "M" (UART_Tx),
		  [txdelay] "r" (txdelay)
		:
		  "r25"
	);
	sei();
}

void SoftEnviarStr(char *Str) {
	while(*Str) {
		SoftEnviarCh(*Str++);
	}
}

void SoftEnviarStr_P(const char *Str){
	while(pgm_read_byte(Str)) {
		SoftEnviarCh(pgm_read_byte(Str++));
	}
}

char SoftLeerCh() {
	uint8_t rxdelay = RXDELAY;
	uint8_t rxdelay15 = (RXDELAY*1.5)-2.5;
	uint8_t delayCount = 0;

	__asm__ __volatile__(
		"0: cbi %[uart_port]-1,%[uart_rx]\n\t"
		"   sbi %[uart_port],%[uart_rx]\n\t"
		"   mov %[delayCount],%[rxdelay15]\n\t"
		"   ldi %[rxdelay15],0x80\n\t"
		"1: sbic %[uart_port]-2,%[uart_rx]\n\t"
		"   rjmp 1b\n\t"
		"   cli\n\t"
		"2: subi %[delayCount], 1\n\t"
		"   brne 2b\n\t"
		"   mov %[delayCount],%[rxdelay]\n\t"
		"   sbic %[uart_port]-2,%[uart_rx]\n\t"
		"   sec\n\t"
		"   ror %[rxdelay15]\n\t"
		"   brcc 2b\n\t"
		"3: dec %[delayCount]\n\t"
		"   brne 3b\n\t"
	:
	  [rxdelay15] "+r" (rxdelay15),
	  [delayCount] "+r" (delayCount)
	:
	  [uart_port] "M" (UART_Port),
	  [uart_rx] "M" (UART_Rx),
	  [rxdelay] "r" (rxdelay)
	);
	return rxdelay15;
}
char* SoftLeerStr(char *Str,uint16_t MaxBuff){
	char c;
	uint8_t conteo=0;
	while(conteo<MaxBuff){
		c=SoftLeerCh();
		if (c!='#')
		{
			Str[conteo++]=c;
		}
		else
		{
			Str[conteo]='\0';
			break;
		}
	}
	Str[conteo]='\0';
	sei();
	return Str;
}
void SoftEnviarStrRaw(char *string,uint16_t L){
	for (uint16_t i=0;i<L;i++)
	{
		SoftEnviarCh(string[i]);
	}
}