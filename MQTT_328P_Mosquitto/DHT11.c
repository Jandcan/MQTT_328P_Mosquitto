/*
 * DHT11.c
 *
 * Created: 2/2/2018 2:10:12
 *  Author: jandry
 */ 
 #ifndef F_CPU
	#define F_CPU 8000000UL
#endif
 #include <avr/io.h>
 #include <util/delay.h>
 #include "DHT11.h"
 
void Set_Reg_out(volatile uint8_t *Reg,uint8_t Pin){
	*Reg|=(1<<Pin);
}
void Set_Reg_in(volatile uint8_t *Reg,uint8_t Pin){
	*Reg&=~(1<<Pin);
}
void Set_Port_High(volatile uint8_t *Port,uint8_t Pin){
	*Port|=(1<<Pin);
}
void Set_Port_Low(volatile uint8_t *Port,uint8_t Pin){
	*Port&=~(1<<Pin);
}
uint8_t LeerPin(volatile uint8_t *Mask,uint8_t Pin){
	uint8_t status;
	return status=*Mask&(1<<Pin);
}
 void DHT11_Iniciar(DHT11 *Pntr){
	Set_Reg_out(Pntr->_DirReg,Pntr->_Pin);
	Set_Port_Low(Pntr->_Prtreg,Pntr->_Pin);
	_delay_ms(20);
	Set_Port_High(Pntr->_Prtreg,Pntr->_Pin);
	/*_delay_us(20);
	*Pntr->_Prtreg&=~(1<<Pntr->_Pin);*/
 }
 void DHT11_ChkResp(DHT11 *Pntr){
    Set_Reg_in(Pntr->_DirReg,Pntr->_Pin);
	while(LeerPin(Pntr->_MaskReg,Pntr->_Pin));
	while(LeerPin(Pntr->_MaskReg,Pntr->_Pin)==0);
	while(LeerPin(Pntr->_MaskReg,Pntr->_Pin));
 }
 uint8_t DHT11_Leerbyte(DHT11 *Pntr){
	uint8_t i,data = 0;
	for(i=0;i<8;i++)
	{
		while(LeerPin(Pntr->_MaskReg,Pntr->_Pin)==0);
		_delay_us(30);
		if(LeerPin(Pntr->_MaskReg,Pntr->_Pin)){  
			data = (data<<1) | (0x01);
		}else{
			data = (data<<1);
		}
		while(LeerPin(Pntr->_MaskReg,Pntr->_Pin));
	}
	return data;
}
 void DHT11_LeerVariables(DHT11 *Pntr){
	DHT11_Iniciar(Pntr);
	DHT11_ChkResp(Pntr);
	Pntr->_HR_Int = DHT11_Leerbyte(Pntr);
	Pntr->_HR_Decim = DHT11_Leerbyte(Pntr);
	Pntr->_Temp_Int = DHT11_Leerbyte(Pntr);
	Pntr->_Temp_Decim = DHT11_Leerbyte(Pntr);
	Pntr->_ChkSum = DHT11_Leerbyte(Pntr);
 }