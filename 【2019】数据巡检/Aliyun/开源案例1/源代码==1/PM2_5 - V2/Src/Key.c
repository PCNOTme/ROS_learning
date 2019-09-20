#include "Key.h"
#include "stm32f0xx_hal.h"

__IO uint8_t key_tick=0;

void Key_IncTick()
{
	key_tick++;
}

unsigned char key_driver(void)  
{  
    static unsigned char key_state = key_state_0,key_temp = N_key;
    static int key_time = 0;  
    unsigned char key_press, key_return = N_key;  

    key_press = key_input;//HAL_GPIO_ReadPin(GPIO_KEY,GPIO_Pin_Key);                    // ������I/O��ƽ  

    switch (key_state)  
    {  
      case key_state_0:                              // ������ʼ̬  
        if (key_press!=GPIO_Pin_Key) key_state = key_state_1;      // �м������£�״̬ת��������������ȷ��״̬  
        break;  
        
      case key_state_1:                      // ����������ȷ��̬  
        if (key_press!=GPIO_Pin_Key)  
        {  
             key_time = 0;                   //   
             if((key_press & KEY1) == 0) key_temp = S_key1;
//             else if((key_press & KEY2) == 0) key_temp = S_key2;
//             else if((key_press & KEY3) == 0) key_temp = S_key3;
             key_state = key_state_2;   // ������Ȼ���ڰ��£�������ɣ�״̬ת�������¼�ʱ��ļ�ʱ״̬�������صĻ����޼��¼�  
        }  
        else  
             key_state = key_state_0;   // ������̧��ת����������ʼ̬���˴���ɺ�ʵ�������������ʵ�����İ��º��ͷŶ��ڴ������ġ�  
        break;  
        
      case key_state_2:  
        if(key_press==GPIO_Pin_Key)  
        {  
             key_return = key_temp;        // ��ʱ�����ͷţ�˵���ǲ���һ�ζ̲���������
             key_state = key_state_0;   // ת����������ʼ̬  
        }  
        else if (++key_time >= 200)     // �������£���ʱ��10ms��10msΪ������ѭ��ִ�м����  
        {  
             key_return = key_temp+2;        // ����ʱ��>2000ms���˰���Ϊ�������������س����¼�  
             key_state = key_state_3;   // ת�����ȴ������ͷ�״̬  
        }  
        break;  

      case key_state_3:                 // �ȴ������ͷ�״̬����״ֻ̬�����ް����¼�  
        if (key_press==GPIO_Pin_Key) key_state = key_state_0; //�������ͷţ�ת����������ʼ̬  
        break;  
    }  
    return key_return;  
} 

//10msɨ��һ��
unsigned char key_read(void)  
{  
    static unsigned char key_m = key_state_0, key_time_1 = 0,key_temp=0;  
    unsigned char key_return = N_key,key_press;  
      
		if(key_tick<10) return 0;
		key_tick=0;
	
    key_press = key_driver();  
      
    switch(key_m)  
    {  
        case key_state_0:  
            if (key_press == S_key1 || key_press == S_key2 || key_press == S_key3)  
            {  
                 key_time_1 = 0;               // ��1�ε����������أ����¸�״̬�жϺ����Ƿ����˫��  
                 key_temp = key_press;
                 key_m = key_state_1;  
            }  
            else  
                 key_return = key_press;        // �����޼�������������ԭ�¼�  
            break;  

        case key_state_1:  
            if (key_press == S_key1 || key_press == S_key2 || key_press == S_key3)             // ��һ�ε���������϶�<500ms��  
            {  
                 key_return = key_temp+1;           // ����˫�����¼����س�ʼ״̬  
                 key_m = key_state_0;  
            }  
            else                                 
            {                                  // ����300ms�ڿ϶������Ķ����޼��¼�����Ϊ����>1000ms����1sǰ�Ͳ㷵�صĶ����޼�  
                 if(++key_time_1 >= 30)  
                 {  
                      key_return = key_temp;      // 300ms��û���ٴγ��ֵ����¼���������һ�εĵ����¼�  
                      key_m = key_state_0;     // ���س�ʼ״̬  
                 }  
             }  
             break;  
    } 
    return key_return;  
}      




