/*
 * Mi_MQTT.h
 *
 * Created: 17/7/2019 23:17:55
 *  Author: Jandry Banegas
 */ 


#ifndef MI_MQTT_H_
#define MI_MQTT_H_

#define MQTT_PROTOCOL_LEVEL		4

#define MQTT_CTRL_CONNECT		0x1
#define MQTT_CTRL_CONNECTACK	0x2
#define MQTT_CTRL_PUBLISH		0x3
#define MQTT_CTRL_PUBACK		0x4
#define MQTT_CTRL_PUBREC		0x5
#define MQTT_CTRL_PUBREL		0x6
#define MQTT_CTRL_PUBCOMP		0x7
#define MQTT_CTRL_SUBSCRIBE		0x8
#define MQTT_CTRL_SUBACK		0x9
#define MQTT_CTRL_UNSUBSCRIBE	0xA
#define MQTT_CTRL_UNSUBACK		0xB
#define MQTT_CTRL_PINGREQ		0xC
#define MQTT_CTRL_PINGRESP		0xD
#define MQTT_CTRL_DISCONNECT	0xE

#define MQTT_QOS_1				0x1
#define MQTT_QOS_0				0x0

//Ajustar acorde a la aplicacion y rendimiento
#define MQTT_CONN_KEEPALIVE		60

#define MQTT_CONN_USERNAMEFLAG	0x80
#define MQTT_CONN_PASSWORDFLAG	0x40
#define MQTT_CONN_WILLRETAIN	0x20
#define MQTT_CONN_WILLQOS_1		0x08
#define MQTT_CONN_WILLQOS_2		0x18
#define MQTT_CONN_WILLFLAG		0x04
#define MQTT_CONN_CLEANSESSION	0x02

//Si el servidor MQTT tiene autentificacion llenar lo siguiente:
#define CLOUD_USERNAME		""
#define CLOUD_KEY			""

typedef struct{
	char ID[10];
	unsigned char QoS;
	char Topic[100];
	char Payload[200];
	unsigned char Buffer[250];
	
	unsigned int pqcounter;
	char will_topic[1];
	char will_payload[1];
	unsigned char will_qos;
	unsigned char will_retain;
}MQTT;



unsigned int MQTT_Conectar(MQTT *);
unsigned int MQTT_Publicar(MQTT *);
unsigned int MQTT_Suscribir(MQTT *M);
unsigned char* AddStringToBuf(unsigned char *_buf, const char *_string);



#endif /* MI_MQTT_H_ */