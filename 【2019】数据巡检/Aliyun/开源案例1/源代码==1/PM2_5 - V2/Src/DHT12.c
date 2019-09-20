//****************************************************************//
//DHT12 IIC���ݶ�ȡ����
//
//****************************************************************//

#include "DHT12.h"

int16_t DHT12_Temperature,DHT12_Humidity;//������ʪ�ȱ��� ���˱���Ϊȫ�ֱ���


__IO uint16_t DHT12_tick=0;

void DHT12_IncTick()
{
	DHT12_tick++;
}


void delay_us(uint32_t t)
{
	uint8_t i;
	while(t--)
	{
		for(i=0;i<2;i++)
		{
		__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();
		__nop();__nop();
		}

	}
}


void SDA_OUT()
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  /*Configure GPIO pins : PF0 PF1 */
  GPIO_InitStruct.Pin = DHT12_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT12_GPIO, &GPIO_InitStruct);
}

void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	  /*Configure GPIO pins : PF0 PF1 */
  GPIO_InitStruct.Pin = DHT12_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT12_GPIO, &GPIO_InitStruct);
}


/********************************************\
|* ���ܣ� �����������͵ĵ����ֽ�	        *|
\********************************************/
uint8_t DHT12_Rdata(void)
{
	uint8_t i;
	uint16_t j;
	uint8_t data=0,bit=0;
	
	for(i=0;i<8;i++)
	{
		while(!READ_SDA)//����ϴε͵�ƽ�Ƿ����
		{
			if(++j>=50000) //��ֹ������ѭ��
			{
				break;
			}
		}
		//��ʱMin=26us Max70us ��������"0" �ĸߵ�ƽ		 
		delay_us(30);
 
		//�жϴ�������������λ
		bit=0;
		if(READ_SDA)
		{
			bit=1;
		}
		j=0;
		while(READ_SDA)	//�ȴ��ߵ�ƽ����
		{
			if(++j>=50000) //��ֹ������ѭ��
			{
				break;
			}		
		}
		data<<=1;
		data|=bit;
	}
	return data;
}
 
/********************************************\
|* ���ܣ�DHT12��ȡ��ʪ�Ⱥ���       *|
\********************************************/
//������Humi_H��ʪ�ȸ�λ��Humi_L��ʪ�ȵ�λ��Temp_H���¶ȸ�λ��Temp_L���¶ȵ�λ��Temp_CAL��У��λ
//���ݸ�ʽΪ��ʪ�ȸ�λ��ʪ��������+ʪ�ȵ�λ��ʪ��С����+�¶ȸ�λ���¶�������+�¶ȵ�λ���¶�С����+ У��λ
//У�飺У��λ=ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
void DHT12_READ(void)
{
	uint32_t j;
	uint8_t Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp;
 
	//����������ʼ�ź�
	SDA_OUT(); //��Ϊ���ģʽ
	IIC_SDA_0;	//�������������ߣ�SDA������
	//HAL_Delay(20);//����һ��ʱ�䣨����18ms���� ֪ͨ������׼������
	if(DHT12_tick<1000)return;
	DHT12_tick=0;
	
	IIC_SDA_1;	 //�ͷ�����
	SDA_IN();	//��Ϊ����ģʽ���жϴ�������Ӧ�ź�
	delay_us(30);//��ʱ30us
 
//	Sensor_AnswerFlag=0;	//��������Ӧ��־
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(READ_SDA==0)
	{
//		Sensor_AnswerFlag=1;	//�յ���ʼ�ź�
 
		j=0;
		while((!READ_SDA)) //�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	
		{
			if(++j>=500) //��ֹ������ѭ��
			{
//				Sensor_ErrorFlag=1;
				break;
			}
		}
 
		j=0;
		while(READ_SDA)//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
		{
			if(++j>=800) //��ֹ������ѭ��
			{
//				Sensor_ErrorFlag=1;
				break;
			}		
		}
		//��������
		Humi_H=DHT12_Rdata();
		Humi_L=DHT12_Rdata();
		Temp_H=DHT12_Rdata();	
		Temp_L=DHT12_Rdata();
		Temp_CAL=DHT12_Rdata();
 
		temp=(uint8_t)(Humi_H+Humi_L+Temp_H+Temp_L);//ֻȡ��8λ
 
		if(Temp_CAL==temp)//���У��ɹ�����������
		{
			DHT12_Humidity=Humi_H*10+Humi_L; //ʪ��
	
			if(Temp_L&0X80)	//Ϊ���¶�
			{
				DHT12_Temperature =0-(Temp_H*10+((Temp_L&0x7F)))-65;
			}
			else   //Ϊ���¶�
			{
				DHT12_Temperature=Temp_H*10+Temp_L-65;//Ϊ���¶�
			}
			//�ж������Ƿ񳬹����̣��¶ȣ�-20��~60�棬ʪ��20��RH~95��RH��
			if(DHT12_Humidity>950) 
			{
			  DHT12_Humidity=950;
			}
			if(DHT12_Humidity<200)
			{
				DHT12_Humidity =200;
			}
			if(DHT12_Temperature>600)
			{
			  DHT12_Temperature=600;
			}
			if(DHT12_Temperature<-200)
			{
				DHT12_Temperature = -200;
			}
			//DHT12_Temperature=DHT12_Temperature/10;//����Ϊ�¶�ֵ
			//Humi=Humi/10; //����Ϊʪ��ֵ
			//printf("\r\n�¶�Ϊ:  %.1f  ��\r\n",Temprature); //��ʾ�¶�
			//printf("ʪ��Ϊ:  %.1f  %%RH\r\n",Humi);//��ʾʪ��	
		}
		
		else
		{
		 	//printf("CAL Error!!\r\n");
			//printf("%d \r%d \r%d \r%d \r%d \r%d \r\n",Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp);
		}
	}
	else
	{
//		Sensor_ErrorFlag=0;  //δ�յ���������Ӧ
		//printf("Sensor Error!!\r\n");
	}
 
}
 




