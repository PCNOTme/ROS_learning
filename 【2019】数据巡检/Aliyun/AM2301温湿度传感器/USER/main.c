#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "dht11.h" 	 
//ALIENTEKս��STM32������ʵ��31
//DHT11������ʪ�ȴ����� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  
 int main(void)
 {	 
	u8 t=0;			    
	u16 temperature;  	    
	u16 humidity,i;    	   
	u8 DisNum[10];
	 u8 iDisNum[10];
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();	
	KEY_Init();	 	

 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"DS18B20 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/12");		  
 	while(DHT11_Init())	//DHT11��ʼ��	
	{
		LCD_ShowString(60,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(60,150,200,16,16,"Temp:00.00C");	 
 	LCD_ShowString(60,170,200,16,16,"Humi:00.00%");
 // LCD_ShowString(60,170,200,16,16,"Humi:  %");		
	while(1)
	{	    	    
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ
//printf("%d\t",temperature);			
			sprintf(DisNum,"%02u.%01u",temperature/10,temperature%10); 
      LCD_ShowString(100,150,200,16,16,DisNum);
			printf("%d\t",humidity);		
			sprintf(iDisNum,"%02u.%01u",humidity/10,humidity%10); 
      LCD_ShowString(100,170,200,16,16,iDisNum);
			
			//LCD_ShowNum(60+40,150,temperature,3,16);		//��ʾ�¶�	
			//temperature=temperature;
     //LCD_ShowxNum(105,150,temperature,3,16,0);			
			//LCD_ShowNum(60+40,170,humidity,3,16);			//��ʾʪ��
			//LED1=0;
//printf("%d",temperature);			
		}
		
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
	}
}



