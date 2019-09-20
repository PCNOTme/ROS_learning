#ifndef _DHT12_H_
#define _DHT12_H_

#include "stm32f0xx_hal.h"



//IO��������
//#define SDA_IN()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x08000000;}//����������
//#define SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x03000000;}//ͨ���������

#define DHT12_GPIO    GPIOF
#define DHT12_SDA_PIN  GPIO_PIN_0
#define DHT12_SCL_PIN  GPIO_PIN_1

//IO��������	 
#define IIC_SCL_1     DHT12_GPIO->BSRR = DHT12_SCL_PIN
#define IIC_SCL_0     DHT12_GPIO->BRR = DHT12_SCL_PIN
#define IIC_SDA_1     DHT12_GPIO->BSRR = DHT12_SDA_PIN
#define IIC_SDA_0     DHT12_GPIO->BRR = DHT12_SDA_PIN
#define READ_SDA      (DHT12_GPIO->IDR & DHT12_SDA_PIN)

void DHT12_IncTick(void);
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

extern int16_t DHT12_Temperature,DHT12_Humidity;

void sensor_iic_init(void);//��ʼ��DHT12_IIC
void sensor_read(void);//��ȡ����


uint8_t DHT12_Rdata(void);
void DHT12_READ(void);



#endif	//_DHT12_H_
