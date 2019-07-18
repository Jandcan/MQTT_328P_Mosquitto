/*
 * DHT11.h
 *
 * Created: 2/2/2018 2:09:56
 *  Author: jandry
 */ 
 #include <stdio.h>
 typedef struct DHT11{
  volatile uint8_t* _Prtreg;
  volatile uint8_t _Pin;
  volatile uint8_t* _MaskReg;
  volatile uint8_t* _DirReg;
  uint8_t _HR_Int;
  uint8_t _HR_Decim;
  uint8_t _Temp_Int;
  uint8_t _Temp_Decim;
  uint8_t _ChkSum;
 }DHT11;
 
 void Set_Reg_in(volatile uint8_t *Reg,uint8_t Pin);
 void Set_Reg_out(volatile uint8_t *Reg,uint8_t Pin);
 void Set_Port_High(volatile uint8_t *Port,uint8_t Pin);
 void Set_Port_Low(volatile uint8_t *Port,uint8_t Pin);
 uint8_t LeerPin(volatile uint8_t *Mask,uint8_t Pin);
 
 void DHT11_Iniciar(DHT11 *);
 void DHT11_ChkResp(DHT11 *);
 uint8_t DHT11_Leerbyte(DHT11 *);
 void DHT11_LeerVariables(DHT11 *);