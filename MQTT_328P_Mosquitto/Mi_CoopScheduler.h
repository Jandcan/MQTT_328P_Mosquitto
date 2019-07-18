/*
 * Mi_CoopScheduler.h
 *
 * Created: 7/5/2019 14:37:16
 *  Author: Jandry O. Banegas
 */ 


#ifndef MI_COOPSCHEDULER_H_
#define MI_COOPSCHEDULER_H_

/*
	Estructura Scheduler de Tareas
	para mas info chechar: https://www.edn.com/electronics-blogs/embedded-basics/4404895/Function-pointers---Part-2--Task-Scheduling
*/
volatile uint8_t NroTareas=0;
typedef struct{
	uint16_t Intervalo;//cada cuantos milisegundos se ejecuta la tarea
	uint32_t AntmsTick;//variable para almacenar los misilesgundos transcurridos
	void (*Funcion)(void);// funcion puntero para ejecutar las tareas
}STareas;

void EjecutarScheduler(STareas *Pntr,uint32_t Tic){
	for (uint8_t IndexTarea=0;IndexTarea<NroTareas; IndexTarea++)//Realiza un barrido por cada Tarea
	{
		if (Pntr[IndexTarea].Intervalo == 0)//Si hay alguna tarea cuyo intervalo sea 0; se ejecuta de forma inmediata
		{
			(*Pntr[IndexTarea].Funcion)();
		}
		else if((Tic-Pntr[IndexTarea].AntmsTick) >= Pntr[IndexTarea].Intervalo)//Verifica que ha pasado el intervalo de cada tarea y la Ejecuta
		{
			(*Pntr[IndexTarea].Funcion)();
			Pntr[IndexTarea].AntmsTick=Tic;
		}
	}
}

#endif /* MI_COOPSCHEDULER_H_ */