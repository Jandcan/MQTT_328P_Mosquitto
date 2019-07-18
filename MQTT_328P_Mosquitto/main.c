/*
 * MQTT_328P_Mosquitto.c
 *
 * Created: 14/5/2019 15:54:39
 * Author : Jandry O. Banegas 
 */ 

#define ever ;;// Visto en un meme :v
#define F_CPU 8000000UL
#define BAUD0 115200

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Mi_USART.h"
#include "avr305.h"
#include "DHT11.h"
#include "Mi_CoopScheduler.h"
#include "Mi_MQTT.h"


#define LED_WIFI_ON PORTD|=(1<<PIND2)
#define LED_WIFI_OFF PORTD&=~(1<<PIND2)
#define LED_WIFI_PARPADEO PORTD^=(1<<PIND2)

#define LED_ENVIO_ON PORTD|=(1<<PIND7)
#define LED_ENVIO_OFF PORTD&=~(1<<PIND7)
#define LED_ENVIO_PARPADEO PORTD^=(1<<PIND7)

#define LED_LEER_ON PORTD|=(1<<PIND4)
#define LED_LEER_OFF PORTD&=~(1<<PIND4)
#define LED_LEER_PARPADEO PORTD^=(1<<PIND4)

#define PWM_SERVO OCR0A

#define SSID_DEF "|Banegas-Rojas|P3-P4"
#define CLAVE_DEF "el_bato_malo93"

#define BUFFER 300 // Ancho de Buffer de Lectura
#define ESPERA_DEF 1500// [ms] tiempo por defecto de espera de respuesta para el ESP8266

typedef enum {DESACTIVADA,ACTIVADA} DEPURACION;
DEPURACION DEV=DESACTIVADA;//DEPURACION ACTIVADA , cambiar para produccion

volatile uint8_t Cada2=0;
volatile uint32_t msTick=0;
volatile uint32_t msAnt=0;

volatile uint8_t _Angulo=0;
//
typedef struct{
	char _URL[50];
	char _Puerto[10];
}TCP_IP;
//
typedef struct{
	uint8_t _SERIAL;//Asignamos el Serial USART0 al modulo ESP8266
	char _AP_SSI[30];
	char _AP_Clave[30];
	char _AP_IP[20];
	char _ATCMD[100];
	char _Buff[BUFFER];
	uint32_t _TempEsp;
	TCP_IP _TCP;
}WiFi;

WiFi Cliente={
	._SERIAL=USART0,//Canal serial dedicado al WiFi, si es atmega328p colocar USART0
	._AP_SSI=SSID_DEF,//Nombre de Red
	._AP_Clave=CLAVE_DEF,//Contraseña de Red
	._TempEsp=ESPERA_DEF,//Tiempo de espera para obtener una respuesta, por defecto 3 segundos
	._Buff=" ",//encera el buffer
	._TCP._URL="192.168.1.9",
	._TCP._Puerto="1883"//puerto MQTT
};
/*
	Estructura Sensor DHT
*/
DHT11 DHT1={
	._Prtreg = &PORTB,
	._Pin = PINB0,
	._MaskReg=&PINB,
	._DirReg= &DDRB,
	._HR_Int=0,
	._HR_Decim=0,
	._Temp_Int=0,
	._Temp_Decim=0,
	._ChkSum=0
};
MQTT mqtt={
	.ID="BetaJan",
	.QoS=0,
	.Topic="test",
	//Variables para conf defecto MQTT
	.pqcounter=0,
	.will_topic="",
	.will_payload="",
	.will_qos=1,
	.will_retain=0
};
/************************************************************************/
/* Prototipado de Funciones                                                                 */
/************************************************************************/
void ConfLeds();
void ConfTimers();//configura el timer2 de 8 bits que sera usado como clock del sistema
void MoverServo(uint8_t Angulo);
/*funciones wifi*/
void Enviar_ESP( WiFi *,char *CMD);//envia un comando y espera respuesta
void Enviar_ESP_P(WiFi *Pntr, const char *CMD);//Envia un comando almacenado en flash y espera respuesta
char* Buscar_ESP(WiFi *, char *);//Busca dentro del buffer de recepcion alguna coincidencia de una cadena especifica
void Leer_ESP(WiFi *);//Espera Mientras Lee durante un lapso de tiempo
/*Funciones Aplicacion*/
uint8_t ConectarWifi(WiFi *);//Funcion para conectar wifi a la RED
uint8_t StatusWifi(WiFi* );//Verfica que exista conexion a la RED
void ObtenerIPWifi(WiFi *);//Obtiene la IP asignada por la RED
void IniciarTCP(WiFi *);//Inicia la Conexion TCP al servidor
void CerrarTCP(WiFi *);//Cierra la comunicacion TCP con el servidor
void PostTCP(WiFi *);//Mediante el metodo POST envia datos al servidor
void GetTCP(WiFi *);//Mediante el metodo GET se obtienen datos del servidor

void LeerSensor();
void EnviarMQTT();
void LeerMQTT();

static STareas Tareas[]={
	{500  ,0 , LeerSensor},
	{3000 ,0 , EnviarMQTT},
	{100 ,0 , LeerMQTT}
};

/************************************************************************/
/*      Incio de Programa                                                                     */
/************************************************************************/
int main(void)
{
	ConfLeds();
	ConfTimers();
	Inicia_Usart(USART0,round(MiUBRR0));//Inicializa USART del ESP8266
	sei();
	//Parpadea el LED WIFI mientras trata de enlazarse al Router
	do
	{
		LED_WIFI_PARPADEO;
	} while (!ConectarWifi(&Cliente));
	LED_WIFI_ON;
	SoftEnviarStr_P(PSTR("Listo\r\n"));
	IniciarTCP(&Cliente);//Conectado al Wifi, se enlaza a ubidots
	_delay_ms(100);
	NroTareas=sizeof(Tareas)/sizeof(*Tareas);
	for(ever)
    {
		EjecutarScheduler(Tareas,msTick);
    }
}
/************************************************************************/
/*				INTERRUPCIONES                                                  */
/************************************************************************/
ISR(TIMER2_OVF_vect){
	Cada2++;// Configurado el micro a 8Mhz cada dos overflows equivalen a 1ms
	if(Cada2==2){
		msTick++;
		Cada2=0;
	}
}
/************************************************************************/
/*				FUNCIONES                                                           */
/************************************************************************/
/*
	Conf Leds
*/
void ConfLeds(){
	/*
		PIND2=LED_WIFI
		PIND3=LED_ENVIO
		PIND4=LED_LEER
	*/
	DDRD|=(1<<PIND2)|(1<<PIND7)|(1<<PIND4);//se declaran como salida los pines de los LEDs de estado
}
/*
	Conftimer 2
*/
void ConfTimers(){
	/**********************Timer 2 SysClock*************************************************************/
	TCCR2A|=(1<<COM2A1)|(1<<COM2B1)|(1<<WGM20);//modo PWM, Phase correct
	TCCR2B=(1<<CS21);//preescala de 8
	TIMSK2=(1<<TOIE2);//interrupcion de overflow activada
	TCNT2=0;
	/**********************Timer 0 PWM*************************************************************/
	TCCR0A|=(1<<COM0A1)|(1<<COM0B1)|(1<<WGM00);	//modo PWM,Phase correct
	TCCR0B|=(1<<CS00)|(1<<CS01);//preescala de 64
	DDRD|=(1<<PIND6);// PIND6 PWM (OCR0A) 
}
/*
	FUNCIONES WIFI
*/
void Enviar_ESP(WiFi *Pntr, char *CMD){
	uint16_t conteo=0;
	Env_Usart(Pntr->_SERIAL,CMD);
	uint32_t Act_mili=msTick;
	memset(Pntr->_Buff,0,BUFFER);
	while((Act_mili+Pntr->_TempEsp)>msTick){
		while (CaracterSinLeer_Usart(Pntr->_SERIAL))
		{
			Pntr->_Buff[conteo++]=leerCaracter_Usart(Pntr->_SERIAL);
		}
		Pntr->_Buff[conteo]='\0';
	}
	if(DEV) SoftEnviarStr(Pntr->_Buff); 
}
/*
*/
void Enviar_ESP_P(WiFi *Pntr, const char *CMD){
	uint16_t conteo=0;
	uint32_t Act_mili=msTick;
	Env_Usart_P(Pntr->_SERIAL,CMD);
	memset(Pntr->_Buff,0,BUFFER);
	while((Act_mili+Pntr->_TempEsp)>msTick){
		while (CaracterSinLeer_Usart(Pntr->_SERIAL))
		{
			Pntr->_Buff[conteo++]=leerCaracter_Usart(Pntr->_SERIAL);
		}
		Pntr->_Buff[conteo]='\0';
	}
	
	if(DEV) SoftEnviarStr(Pntr->_Buff);
}
/*
*/
char* Buscar_ESP(WiFi *Pntr, char *Str){
	char *_status;
	_status=strstr(Pntr->_Buff,Str);
	return _status;
}
/*
*/
void Leer_ESP(WiFi *Pntr){
	uint16_t conteo=0;
	uint32_t Act_mili=msTick;
	memset(Pntr->_Buff,0,BUFFER);
	while((Act_mili+Pntr->_TempEsp)>msTick){
		while (CaracterSinLeer_Usart(Pntr->_SERIAL))
		{
			Pntr->_Buff[conteo++]=leerCaracter_Usart(Pntr->_SERIAL);
		}
		Pntr->_Buff[conteo]='\0';
	}
	
	if(DEV) SoftEnviarStr(Pntr->_Buff);
}
/*
*/
uint8_t ConectarWifi(WiFi *W){
	char *Index;
	W->_TempEsp=1000;
	if(StatusWifi(W)){
		if (DEV){
			SoftEnviarStr_P(PSTR("Ya estas conectado a la RED!, "));
			SoftEnviarStr(W->_AP_SSI);
			SoftEnviarStr_P(PSTR("\r\n"));
		}
		ObtenerIPWifi(W);
		return 1;
	}else{
		if(DEV) SoftEnviarStr_P(PSTR("No hay Red!\r\n Conectando...\r\n"));	
		W->_TempEsp=3000;
		sprintf_P(W->_ATCMD,PSTR("AT+CWJAP=\"%s\",\"%s\"\r\n"),W->_AP_SSI,W->_AP_Clave);
		Enviar_ESP(W,W->_ATCMD);
		Index=Buscar_ESP(W,"WIFI CONNECTED");
		W->_TempEsp=ESPERA_DEF;
		return (Index!=NULL)?1:0;
	}
}
/*
*/
uint8_t StatusWifi(WiFi *W){
	char *Index_;
	uint8_t Red;
	char *Index_st;
	Red=0;
	//
	do{
		Env_Usart_P(W->_SERIAL,PSTR("+++"));
		_delay_ms(1000);
		Enviar_ESP_P(W,PSTR("\r\n"));
		Enviar_ESP_P(W,PSTR("AT\r\n"));
		Index_st=Buscar_ESP(W,"ERROR");
	} while (Index_st!=NULL);
	//
	W->_TempEsp=400;
	//W->_TempEsp=ESPERA_DEF;
	Enviar_ESP_P(W,PSTR("AT+CWJAP?\r\n"));//verifica si el modulo wifi esta conectado a una red
	Index_=Buscar_ESP(W,"+CWJAP:");//busca respuesta esperada si hay conexion;
	Red=(Index_!=NULL)?1:0;
	if(DEV) SoftEnviarStr(Index_),SoftEnviarStr_P(PSTR("\r\n"));
	W->_TempEsp=200;
	return Red;//devuelve 1 si hay coincidencia y 0 si no hay 
}
/*
*/
void ObtenerIPWifi(WiFi *W){
	char *Index_;
	char *Index_st;
	char *Cliente_IP;
	do{
		Env_Usart_P(W->_SERIAL,PSTR("+++"));
		_delay_ms(1000);
		Enviar_ESP_P(W,PSTR("\r\n"));
		Enviar_ESP_P(W,PSTR("AT\r\n"));
		Index_st=Buscar_ESP(W,"ERROR");
	} while (Index_st!=NULL);
	
	Enviar_ESP_P(W,PSTR("AT+CIFSR\r\n"));
	Index_=Buscar_ESP(W,"STAIP,");
	Cliente_IP=strtok_r(Index_+6,"\"\"",&Index_);
	strcpy(W->_AP_IP,Cliente_IP);
	if(DEV) SoftEnviarStr_P(PSTR("IP: ")),SoftEnviarStr(W->_AP_IP),SoftEnviarStr_P(PSTR("\r\n"));
}
/*
*/
void IniciarTCP(WiFi *W){
	W->_TempEsp=500;
	Enviar_ESP_P(W,PSTR("AT+CIPMUX=0\r\n"));
	sprintf_P(W->_ATCMD,PSTR("AT+CIPSTART=\"TCP\",\"%s\",%s\r\n"),W->_TCP._URL,W->_TCP._Puerto);
	Enviar_ESP(W,W->_ATCMD);	Enviar_ESP_P(W,PSTR("AT+CIPMODE=1\r\n"));
	sprintf_P(W->_ATCMD,PSTR("AT+CIPSEND\r\n"));
	Env_Usart(W->_SERIAL,W->_ATCMD);
}
/*
*/
void CerrarTCP(WiFi *W){
	W->_TempEsp=200;
	Env_Usart_P(W->_SERIAL,PSTR("+++"));
	_delay_ms(1000);
	Enviar_ESP_P(W,PSTR("AT+CIPMODE=0\r\n"));
	Enviar_ESP_P(W,PSTR("AT+CIPCLOSE\r\n"));
}

/*
*/
void LeerSensor(){
	char Buff[100];
	
	DHT11_LeerVariables(&DHT1);
	sprintf_P(Buff,PSTR("Temp:%d HumR:%d\r\n"),DHT1._Temp_Int,DHT1._HR_Int);
	SoftEnviarStr(Buff);
}
/*
*/
void EnviarMQTT(){
	LED_ENVIO_PARPADEO;
	//Enviamos solicitud de Conexion
	uint16_t len;
	len=MQTT_Conectar(&mqtt);
	Env_Usart_RAW((char *)mqtt.Buffer,len);
	//Se crea el Payload
	memset(mqtt.Buffer,0,250);
	sprintf(mqtt.Topic,"test");
	sprintf(mqtt.Payload,"d1#dEstudio#d%d#d%d#d%d#",DHT1._Temp_Int,DHT1._HR_Int,_Angulo);
	len=MQTT_Publicar(&mqtt);
	//se envia
	Env_Usart_RAW((char *)mqtt.Buffer,len);
	LED_ENVIO_PARPADEO;
}
/*
*/
void LeerMQTT(){
	LED_LEER_PARPADEO;
	char *Index;
	char *Angulo;
	uint16_t len;
	msAnt=msTick;
	//Conectarse al Broker Mosquitto
	len=MQTT_Conectar(&mqtt);
	Env_Usart_RAW((char *)mqtt.Buffer,len);
	//Solicitamos Suscripcion
	memset(mqtt.Buffer,0,250);//enceramos el buffer
	sprintf(mqtt.Topic,"Disp/Actuador");//Especificamos el Topic al cual nos sucribiremos
	len=MQTT_Suscribir(&mqtt);
	Env_Usart_RAW((char *)mqtt.Buffer,len);
	//Esperar Respuesta
	Cliente._TempEsp=200;
	Leer_ESP(&Cliente);
	//Buscamos el valor del PWM del paquete receptado
	Index=memchr(Cliente._Buff,':',BUFFER);
	if(Index!=NULL){
		SoftEnviarStr_P(PSTR("Valor Obtenido...\r\n"));
		Angulo=strtok_r(Index+1,"}",&Index);//extrae el valor de la variable
		if (isdigit(Angulo[0]))//Verifica si lo obtenido es un numero
		{
			SoftEnviarStr_P(PSTR("Angulo Servo:")),SoftEnviarStr(Angulo),SoftEnviarStr_P(PSTR("\r\n"));
			_Angulo=atoi(Angulo);
			MoverServo(atoi(Angulo));
		}
	}
	LED_LEER_PARPADEO;
}
/*
*/
void MoverServo(uint8_t Angulo){
	float y,x;
	x=(float)Angulo;
	/*
		(x1, y1)=(0, 62.5)
		(x2, y2)=(180, 125)
	*/
	y=62.5+(0.347222*x);
	PWM_SERVO=round(y);
}