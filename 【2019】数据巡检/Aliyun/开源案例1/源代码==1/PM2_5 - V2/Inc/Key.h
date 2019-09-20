#ifndef __KEY_H__
#define __KEY_H__

#define key_state_0 0  // ������ʼ̬
#define key_state_1 1  // ����������ȷ��̬ 
#define key_state_2 2  
#define key_state_3 3  		

#define N_key     0             //�޼�  

#define S_key1    1             //key1����  
#define D_key1    2             //key1˫��  
#define L_key1    3             //key1����  

#define S_key2    4             //key2����  
#define D_key2    5             //key2˫��  
#define L_key2    6             //key2����  

#define S_key3    7             //key3����  
#define D_key3    8             //key3˫��  
#define L_key3    9             //key3����  



#define KEY1       (GPIO_PIN_1)
//#define KEY2       (GPIO_PIN_4)
//#define KEY3       (GPIO_PIN_5)


//��������
#define GPIO_KEY 				GPIOB
#define GPIO_Pin_Key 		GPIO_PIN_1
#define key_input    GPIO_KEY->IDR & GPIO_Pin_Key   // ���������  


extern void Key_IncTick(void);                 
extern unsigned char key_read(void);			// 10ms����1�Σ����ذ���ֵ

#endif

