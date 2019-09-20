#ifndef _GUI_H
#define _GUI_H

#include "stm32f0xx_hal.h"


#define PAGE1			0
#define PAGE2			1
#define PAGE3			2
#define PAGE4			3
#define PAGE5			4

#define MAX_PAGE 5


typedef struct 
{
	uint8_t 	page; 		//ҳ��
	uint8_t   page_init;
}STRGUI;


typedef struct 
{
	uint8_t Flag;
	uint8_t  Todaycode; 		//������������
	uint8_t   TodayT[8];			//�����¶�
	uint8_t  Tomorrowcode; 	//������������	
	uint8_t   TomorrowT[8];	//�����¶�
}WEATHER_t;

void GUI_IncTick(void);
void Key_Process(void);
void GUI_Init(void);
void GUI_Task(void);


#endif


