/**
 * @file Mi_USART.h
 * @brief Driver comunicacion USART.
 * 
 * @author  Ing. Jandry O. Banegas
 * 
 * @date 6/4/2018
 */ 

#ifndef BAUD0
	#define BAUD0 9600 
#endif
#ifndef BAUD1
	#define BAUD1 9600
#endif

#define USART0 0
#define USART1 1

#define MiUBRR0 (((F_CPU/8.0)/BAUD0)-1.0)
#define MiUBRR1 (((F_CPU/8.0)/BAUD1)-1.0)

#define LLEGO_UN_CARACTER_USART0 (UCSR0A & (1<<RXC0))
#define LLEGO_UN_CARACTER_USART1 (UCSR1A & (1<<RXC1))

/**
 *Esta funcion inicializa y configura los registros de la comunicacion USART
 *
 *@param nserial El canal USART a configurar
 *@param baudios Tasa de bits del canal USART
 *@return Nada
 */
void Inicia_Usart(unsigned char nserial,unsigned int baudios);//funcion para cargar la configuracion inicial del USART
/**
 *Esta funcion lee los caracteres que llegan por el puerto USART
 *
 *@param nserial Selecciona el canal USART el cual escuchara
 *@return caracter USART
 */
unsigned char leerCaracter_Usart(unsigned char nserial);//lee los datos que llegan por el RX
/**
 *Esta funcion envia caracteres por USART
 *
 *@param nserial Selecciona el canal USART
 *@param STR Puntero a la cadena de caracteres a ser enviados
 *@return Nada
 */
void Env_Usart(unsigned char nserial,char *STR);//envia datos por el puerto TX
/**
 *Esta funcion envia caracteres por USART de las cadenas almacenadas en memoria Flash
 *
 *@param nserial Selecciona el canal USART
 *@param STR Puntero a la cadena de caracteres a ser enviados almacenados en Flash
 *@return Nada
 */
void Env_Usart_P(unsigned char nserial,const char *STR);//envia datos por el puerto TX, usando espacio de programa 
/**
 *Esta funcion comprueba si hay algun carcater sin leer en la comunicacion USART
 *
 *@param nserial El canal USART en el que se comprueba si hay algun carcater USART
 *@return TRUE o FALSE dependiendo de la exsitencia de carcater sin leer
 */
char CaracterSinLeer_Usart(unsigned char nserial);// Funcion para verififcar si no hay algun caracter sin leer en el puerto serial
/*
*/
void EnvCh_Usart(char c);
/*
*/
void Env_Usart_RAW(char *STR,uint16_t L);