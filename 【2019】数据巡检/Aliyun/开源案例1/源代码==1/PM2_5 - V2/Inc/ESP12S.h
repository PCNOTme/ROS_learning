#ifndef __ESP12S_H__
#define __ESP12S_H__

#include "stm32f0xx_hal.h"


typedef enum 
{
  ATCMD_START    		= 0x00U,
  ATCMD_SEND    		= 0x01U,
  ATCMD_WAIT_REV    = 0x02U,
  ATCMD_REVOK  			= 0x03U,
	ATCMD_TIMEOUT 		= 0x04U
} ATCMD_StatusTypeDef;
//ATָ��״̬��  0׼������ȷ�����Է��ʹ���  1����״̬  2�ȴ�����  3���ճɹ�  4���ճ�ʱ

#define ESP12S_BUF_LEN  300

#define MQTT_INTERVAL_TIME   30000  //����MQTT���ݼ��ʱ�� ms

extern uint8_t ESP12S_RevBuf[ESP12S_BUF_LEN],ESP12S_RevByte;
extern uint8_t ESP12S_RevFlag;

void ATCMD_IncTick(void);
void ESP12S_Init(UART_HandleTypeDef *huart);

uint8_t Hand(char* substr,uint16_t *index,uint16_t start_index);
void Clr_RxBuf(void);
ATCMD_StatusTypeDef AT_CMD(UART_HandleTypeDef *huart,uint8_t* cmd,uint16_t timeout,const char* res,uint8_t count);

uint8_t ReConnectWiFi(UART_HandleTypeDef *huart);
uint8_t UDP_Mode(UART_HandleTypeDef *huart);
uint8_t Send_Data(UART_HandleTypeDef *huart,uint8_t* str,uint8_t link,uint8_t num);
void Weather_JSON(uint16_t i);
void Time_JSON(uint16_t i);

void ESP12_Task(UART_HandleTypeDef *huart);

#endif

