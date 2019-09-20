#include "ESP12S.h"
#include "string.h"
#include "usart.h"
#include "gui.h"
#include "rtc.h"
#include "lcd.h"
#include "MQTTPacket.h"
#include "transport.h"
#include "cJSON.h"
#include "DHT12.h"
#include <stdlib.h>
#include "PMS5003.h"

extern RTC_TimeTypeDef nTime;
extern RTC_DateTypeDef nDate;

__IO uint16_t Atcmd_tick=0;
__IO uint32_t ESP12_Tick=0;


//uint8_t teststatus=0;
char *topic="";//��������

uint8_t SSID[20]="WIN4540";//WIFI;
uint8_t Password[20]="66666666";//wifi����
uint8_t Gateway[16]="192.168.0.1";
uint8_t IP[16]="192.168.0.11";
uint8_t ESP12S_RevBuf[ESP12S_BUF_LEN];
uint8_t ESP12S_RevByte;
uint8_t ESP12S_RevFlag=0;

static uint8_t sendstr[70]="";
uint8_t WIFI_Status=1;
uint16_t Revcnt=0;

void ATCMD_IncTick()
{
    Atcmd_tick++;
//    MQTT_tick++;
    ESP12_Tick++;
//    if(MQTT_tick>100)
//    {
//        MQTT_RevFlag=1;
//        Revcnt=0;
//    }
}



void ESP12S_Init(UART_HandleTypeDef *huart)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    Atcmd_tick=0;
    while(Atcmd_tick<200);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    Atcmd_tick=0;
    while(Atcmd_tick<200);
		Clr_RxBuf();
//	while(ConnectWiFi(huart)!=0);
//	while(TCP_UDP_Mode(huart)!=0);
//
//	while(Send_Data(huart,"123",0,3)!=0);
//	while(Send_Data(huart,"GET https://api.seniverse.com/v3/weather/now.json?key=nyldnfnczklzs&location=shijiazhuang&language=zh-Hans&unit=c\n\n",1,118)!=0);
//	while(Send_Data(huart,"123",2,3)!=0);
}



//�ж�str���Ƿ����substr,��������1  ����������0
uint8_t Hand(char* substr,uint16_t *index,uint16_t start_index)
{
    uint16_t i,flag=0;
    char *p;
    for(i=start_index; i<ESP12S_BUF_LEN; i++)
    {
        if(ESP12S_RevBuf[i]==*substr)
        {
            flag=1;
            p=substr;
            while(*p)
            {
                if(ESP12S_RevBuf[i]==*p)
                {
                    *p++;
                    i++;
                }
                else
                {
                    flag=0;
                    break;
                }
            }
            if(flag==1)
            {
                *index = i;
                break;
            }
        }
    }
    return flag;
}
//��ս��ջ�����
void Clr_RxBuf()
{
    for(uint16_t i=0; i<ESP12S_BUF_LEN; i++)
    {
        ESP12S_RevBuf[i]=0;
    }
    Revcnt=0;
}

//------------------����ATָ��-----------------
//����1��huart ���ں�
//����2��cmd ATָ��
//����3��timeout ���η��͵ĳ�ʱʱ��
//����4��res Ҫ�жϵķ��ؽ��
//����5��count ���Դ���
//����ֵ��ATCMD_REVOK �������յ��ظ�  ATCMD_TIMEOUT ��ʱ
ATCMD_StatusTypeDef AT_CMD(UART_HandleTypeDef *huart,uint8_t* cmd,uint16_t timeout,const char* res,uint8_t count)
{
    static ATCMD_StatusTypeDef atcmd_status=ATCMD_START;
    static uint8_t cnt=1;
    uint16_t temp;
    switch(atcmd_status)
    {
    case ATCMD_START:
        cnt = count;
        atcmd_status=ATCMD_SEND;
        break;
    case ATCMD_SEND:
        Clr_RxBuf();
        //HAL_UART_Receive_IT(&huart1, ESP12S_RevBuf, 5); //����1DMA��������
        HAL_UART_Transmit_IT(huart,cmd,strlen((const char*)cmd));
        atcmd_status=ATCMD_WAIT_REV;
        Atcmd_tick=0;
        break;
    case ATCMD_WAIT_REV:
        if(Atcmd_tick<timeout)
        {
            if(Hand((char*)res,&temp,0))//
            {
                atcmd_status=ATCMD_REVOK;
            }
        }
        else
        {
            if(cnt>0)//�ٴη���
                atcmd_status=ATCMD_SEND;
            else
                atcmd_status=ATCMD_TIMEOUT;
            cnt--;
        }
        break;
    case ATCMD_REVOK:
    case ATCMD_TIMEOUT:
        atcmd_status=ATCMD_START;
    default:
        break;
    }
    return atcmd_status;
}



//------------��������SSID������WIFI-------------
//���ӳɹ�����0  ����1��ʾ����  ����2��������
uint8_t ReConnectWiFi(UART_HandleTypeDef *huart)
{
    static uint8_t status=0;//
    static ATCMD_StatusTypeDef res;
    //static uint8_t tempstr[70]="";

    switch(status)
    {
    case 0:
        res = AT_CMD(huart,(uint8_t*)"AT+CWMODE_DEF=1\r\n",1000,"OK",3);//����ΪSTAģʽ
        if(res==ATCMD_REVOK)
            status=1;
        else if(res==ATCMD_TIMEOUT)
        {
            status=0;
            return 1;
        }
        break;
    case 1:
        sendstr[0]=0;
        strcat((char*)sendstr,"AT+CWJAP_DEF=\"");
        strcat((char*)sendstr,(const char*)SSID);
        strcat((char*)sendstr,"\",\"");
        strcat((char*)sendstr,(const char*)Password);
        strcat((char*)sendstr,"\"\r\n");
        res = AT_CMD(huart,sendstr,1000,"OK",3);//����SSID������
        if(res==ATCMD_REVOK)
            status=2;
        else if(res==ATCMD_TIMEOUT)
        {
            status=1;
            return 1;
        }
        break;
    case 2://���þ�̬IP
//			strcat((char*)tempstr,"AT+CIPSTA_DEF=\"");
//			strcat((char*)tempstr,(const char*)IP);
//			strcat((char*)tempstr,"\",\"");
//			strcat((char*)tempstr,(const char*)Gateway);
//			strcat((char*)tempstr,"\",255.255.255.0\"");
//			strcat((char*)tempstr,"\r\n");
//			res = AT_CMD(huart,tempstr,1000,"OK",3);
        res = AT_CMD(huart,"AT+CWDHCP_DEF=1,1\r\n",1000,"OK",3);//����DHCP
        if(res==ATCMD_REVOK)
            status=3;
        else if(res==ATCMD_TIMEOUT)
        {
            status=2;
            return 1;
        }
        break;
    case 3:
        res = AT_CMD(huart,"ATE0\r\n",1000,"OK",3);//�رջ���
        if(res==ATCMD_REVOK)
            status=4;
        else if(res==ATCMD_TIMEOUT)
        {
            status=3;
            return 1;
        }
        break;
    case 4:
        status=0;
        return 0;
    default:
        break;
    }
    return 2;
}



//����TCP����
uint8_t TCP_Mode(UART_HandleTypeDef *huart)
{
    static uint8_t status=0;//
    static ATCMD_StatusTypeDef res;

    switch(status)
    {
    case 0:
        res = AT_CMD(huart,(uint8_t*)"AT+CIPMUX=0\r\n",1000,"OK",3);//������
        if(res==ATCMD_REVOK)
            status=1;
        else if(res==ATCMD_TIMEOUT)
        {
            status=0;
            return 1;
        }
        break;
    case 1:
        res = AT_CMD(huart,(uint8_t*)"AT+CIPSTART=\"TCP\",\"ProductKey.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n",1000,"OK",3);//ProductKey�����Լ���
        //res = AT_CMD(huart,(uint8_t*)"AT+CIPSTART=\"TCP\",\"192.168.137.1\",1883\r\n",1000,"OK",3);
        if(res==ATCMD_REVOK)
            status=2;
        else if(res==ATCMD_TIMEOUT)
        {
            status=1;
            return 1;
        }
        break;
    case 2:
        res = AT_CMD(huart,(uint8_t*)"AT+CIPMODE=1\r\n",1000,"OK",3);//͸��ģʽ
        if(res==ATCMD_REVOK)
            status=3;
        else if(res==ATCMD_TIMEOUT)
        {
            status=2;
            return 1;
        }
        break;
    case 3:			//׼���������� ͸��
        res = AT_CMD(huart,"AT+CIPSEND\r\n",1000,">",3);
        if(res==ATCMD_REVOK)
            status=4;
        else if(res==ATCMD_TIMEOUT)
        {
            status=3;
            return 1;
        }
        break;
    case 4:
        status=0;
        return 0;
    default:
        break;
    }
    return 2;
}
unsigned char buf[200];

void mqtt_connect(UART_HandleTypeDef *huart)
{
    int32_t len;
//		int msgid = 1;
//		int req_qos = 0;
	
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;//���ò��ֿɱ�ͷ����ֵ
    
//    MQTTString topicString = MQTTString_initializer;
    int buflen = sizeof(buf);
		memset(buf,0,buflen);
    data.clientID.cstring = "xxx|securemode=3,signmethod=hmacsha1|";//�ͻ��˱�ʶ����������ÿ���ͻ���xxxΪ�Զ��壬����Ϊ�̶���ʽ
    data.keepAliveInterval = 120;//�����ʱ���������˷������յ��ͻ�����Ϣ�����ʱ����
    data.cleansession = 1;//�ñ�־��1���������붪��֮ǰ���ֵĿͻ��˵���Ϣ������������Ϊ�������ڡ�
    data.username.cstring = "";//�û��� DeviceName&ProductKey
    data.password.cstring = "";//���룬��������
    len = MQTTSerialize_connect(buf, buflen, &data); /*1 �������ӱ���*/

    transport_sendPacketBuffer(huart,buf, len);//������������
    {
        unsigned char sessionPresent, connack_rc;
        do
        {
            while(MQTTPacket_read(buf, buflen, transport_getdata) != CONNACK)//�Խ��յ��ı��Ľ��н���
            {
                ;
            }
        }
        while(MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0);
    }
}
int mqtt_publish(UART_HandleTypeDef *huart,char *pTopic,char *pMessage)
{
		static uint8_t err_cnt=0;
    int32_t len;
    MQTTString topicString = MQTTString_initializer;
    int msglen = strlen(pMessage);//���㷢����Ϣ�ĳ���
    int buflen = sizeof(buf);

		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBREC)//���յ�������Ϣ�Ļظ�
		{
				unsigned char packettype = 0;
				unsigned char dup_ack = 0;
				unsigned short packetid = 0;

				MQTTDeserialize_ack(&packettype,&dup_ack,&packetid,buf,buflen);
				err_cnt=0;
		}
		else//û���յ��ظ�
		{
				err_cnt++;
				if(err_cnt>3)//��������û�յ��ظ�  ���³�ʼ��WIFIģ��
				{
					err_cnt=0;
					ESP12S_Init(huart);
					WIFI_Status=0;
				}
		}
		Clr_RxBuf();
    memset(buf,0,buflen);
    topicString.cstring = pTopic;//"�豸topic�б��еĵ�һ��";
    len = MQTTSerialize_publish(buf, buflen, 0, 2, 0, 0, topicString, (unsigned char*)pMessage, msglen); /*2 ���췢����Ϣ�ı���*/

    transport_sendPacketBuffer(huart,buf,len);//������Ϣ
		free(pMessage);
    return 0;
}

void ESP12_Task(UART_HandleTypeDef *huart)
{
//    uint8_t res=0;

    switch(WIFI_Status)
    {
    case 0://�������ò�����WIFI
        if(ReConnectWiFi(huart)==0)
            WIFI_Status=1;
        break;
    case 1://����tcp������
        if(TCP_Mode(huart)==0)
        {
            WIFI_Status=2;
            ESP12_Tick=0;
            Clr_RxBuf();
        }
        break;
    case 2://����MQTT������
        mqtt_connect(huart);
				WIFI_Status=3;
				ESP12_Tick = MQTT_INTERVAL_TIME;
        break;
    case 3://����MQTT����
        if(ESP12_Tick>=MQTT_INTERVAL_TIME)
        { 
						cJSON * pJsonRoot = NULL;
						pJsonRoot = cJSON_CreateObject();//
						if ( NULL == pJsonRoot )
						{
								break ;
						}
						cJSON_AddStringToObject(pJsonRoot, "id", "rty");//
						cJSON_AddStringToObject(pJsonRoot, "method", "thing.event.property.post");//
						cJSON * pJsonChild = cJSON_CreateObject();//
						if ( NULL == pJsonChild )
						{
								cJSON_Delete(pJsonChild);
								break ;
						}
						cJSON_AddNumberToObject(pJsonChild, "pm2_5", PMS5003_Data.PM2_5);
						#ifdef PMS5003_ST
							cJSON_AddNumberToObject(pJsonChild, "humi", PMS5003_Data.Humidity/10);
							cJSON_AddNumberToObject(pJsonChild, "temp", PMS5003_Data.Temperature/10.0);
						#else
							cJSON_AddNumberToObject(pJsonChild, "humi", DHT12_Humidity/10);
							cJSON_AddNumberToObject(pJsonChild, "temp", DHT12_Temperature/10.0);
						#endif
						
						cJSON_AddItemToObject(pJsonRoot, "params", pJsonChild);//
	
						char* lpJsonStr = cJSON_Print(pJsonRoot);
						cJSON_Delete(pJsonRoot);//
						if(lpJsonStr==NULL)
						{
							break ;
						}
						mqtt_publish(huart,topic,lpJsonStr);
            free(lpJsonStr);
						ESP12_Tick=0;
        }
        break;
    default:
        break;
    }
}
