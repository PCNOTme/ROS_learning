#ifndef _FILE_H
#define _FILE_H

#include "stm32f0xx_hal.h"




#define  WRITE_FILE_TIME  60 //д�ļ�ʱ���� ��λ��

void File_IncTick(void);
void WriteData(void);
void ReadParam(void);


#endif

