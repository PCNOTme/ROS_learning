#ifndef _SPI_SD_DRIVER_H_
#define _SPI_SD_DRIVER_H_

#include "stm32f0xx_hal.h"

/* Private define ------------------------------------------------------------*/
/* SD�����Ͷ��� */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

/* SPI�����ٶ�����*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define NO_RELEASE      0
#define RELEASE         1

/* SD��ָ��� */
#define CMD0    0       //����λ
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00




/* Private macro -------------------------------------------------------------*/
//SD��CSƬѡʹ�ܶ˲�����
#define SD_CS_ENABLE()      GPIOA->BRR = GPIO_PIN_4 //GPIO_ResetBits(GPIOA,GPIO_Pin_4)   //ѡ��SD��
#define SD_CS_DISABLE()     GPIOA->BSRR = GPIO_PIN_4 //GPIO_SetBits(GPIOA,GPIO_Pin_4)     //��ѡ��SD��


#define SD_DET()            1     //!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) //����п�
                                                                    //1-�� 0-��

/* Private function prototypes -----------------------------------------------*/
void SPI1_Configuration(void);
void SPI_SetSpeed(uint8_t SpeedSet);

uint8_t SPI_ReadWriteByte(uint8_t TxData);                //SPI���߶�дһ���ֽ�
uint8_t SD_WaitReady(void);                          //�ȴ�SD������
uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc);     //SD������һ������
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_Init(void);                               //SD����ʼ��
                                                //
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release);//SD��������
uint8_t SD_GetCID(uint8_t *cid_data);                     //��SD��CID
uint8_t SD_GetCSD(uint8_t *csd_data);                     //��SD��CSD
uint32_t SD_GetCapacity(void);                       //ȡSD������

uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer);  //��һ��sector
uint8_t SD_WriteSingleBlock(uint32_t sector, const uint8_t *buffer); //дһ��sector
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count); //�����sector
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count);  //д���sector

#endif
