/*
 * Mi_MQTT.c
 *
 * Created: 17/7/2019 23:20:29
 *  Author: Jandry Banegas
 */ 
#include <string.h>
#include "Mi_MQTT.h"
/*
*/
unsigned char* AddStringToBuf(unsigned char *_buf, const char *_string)
{
	unsigned int _length = strlen(_string);
	_buf[0] = _length >> 8;
	_buf[1] = _length & 0xFF;
	_buf+=2;
	strncpy((char *)_buf, _string, _length);
	return _buf + _length;
}
/*
*/
unsigned int MQTT_Conectar(MQTT *M)
{
	unsigned char*_packet;
	
	_packet=&M->Buffer[0];
	unsigned int _length;

	_packet[0] = (MQTT_CTRL_CONNECT << 4);
	_packet+=2;
	_packet = AddStringToBuf(_packet, "MQTT");
	_packet[0] = MQTT_PROTOCOL_LEVEL;
	_packet++;
	
	_packet[0] = MQTT_CONN_CLEANSESSION;
	if (strlen(M->will_topic) != 0) {
		_packet[0] |= MQTT_CONN_WILLFLAG;
		if(M->will_qos == 1)
		_packet[0] |= MQTT_CONN_WILLQOS_1;
		else if(M->will_qos == 2)
		_packet[0] |= MQTT_CONN_WILLQOS_2;
		if(M->will_retain == 1)
		_packet[0] |= MQTT_CONN_WILLRETAIN;
	}
	if (strlen(CLOUD_USERNAME) != 0)
	_packet[0] |= MQTT_CONN_USERNAMEFLAG;
	if (strlen(CLOUD_KEY) != 0)
	_packet[0] |= MQTT_CONN_PASSWORDFLAG;
	_packet++;

	_packet[0] = MQTT_CONN_KEEPALIVE >> 8;
	_packet++;
	_packet[0] = MQTT_CONN_KEEPALIVE & 0xFF;
	_packet++;
	if (strlen(M->ID) != 0) {
		_packet = AddStringToBuf(_packet, M->ID);
		} else {
		_packet[0] = 0x0;
		_packet++;
		_packet[0] = 0x0;
		_packet++;
	}
	if (strlen(M->will_topic) != 0) {
		_packet = AddStringToBuf(_packet, M->will_topic);
		_packet = AddStringToBuf(_packet, M->will_payload);
	}

	if (strlen(CLOUD_USERNAME) != 0) {
		_packet = AddStringToBuf(_packet, CLOUD_USERNAME);
	}
	if (strlen(CLOUD_KEY) != 0) {
		_packet = AddStringToBuf(_packet, CLOUD_KEY);
	}
	_length = _packet - M->Buffer;
	M->Buffer[1] = _length-2;

	return _length;
}

unsigned int MQTT_Publicar(MQTT *M)
{
	unsigned char *_packet = &M->Buffer[0];
	unsigned int _length = 0;
	unsigned int Datalen=strlen(M->Payload);

	_length += 2;
	_length += strlen(M->Topic);
	if(M->QoS > 0) {
		_length += 2;
	}
	_length += Datalen;
	_packet[0] = MQTT_CTRL_PUBLISH << 4 | M->QoS << 1;
	_packet++;
	do {
		unsigned char encodedByte = _length % 128;
		_length /= 128;
		if ( _length > 0 ) {
			encodedByte |= 0x80;
		}
		_packet[0] = encodedByte;
		_packet++;
	} while ( _length > 0 );
	_packet = AddStringToBuf(_packet, M->Topic);
	if(M->QoS > 0) {
		_packet[0] = (M->pqcounter >> 8) & 0xFF;
		_packet[1] = M->pqcounter & 0xFF;
		_packet+=2;
		M->pqcounter++;
	}
	memmove(_packet, M->Payload, Datalen);
	_packet+= Datalen;
	_length = _packet - M->Buffer;

	return _length;
}
/*
*/
unsigned int MQTT_Suscribir(MQTT *M)
{
	unsigned char *_packet = &M->Buffer[0];
	unsigned int _length;

	_packet[0] = MQTT_CTRL_SUBSCRIBE << 4 | MQTT_QOS_1 << 1;
	_packet+=2;

	_packet[0] = (M->pqcounter >> 8) & 0xFF;
	_packet[1] = M->pqcounter & 0xFF;
	_packet+=2;
	M->pqcounter++;

	_packet = AddStringToBuf(_packet, M->Topic);

	_packet[0] = M->QoS;
	_packet++;

	_length = _packet - M->Buffer;
	M->Buffer[1] = _length-2;

	return _length;
}