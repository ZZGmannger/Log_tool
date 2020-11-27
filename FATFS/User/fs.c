
#include "fatfs.h"
#include <string.h>

#include "fifo.h"

#define FAT_LOG(...)  

/*===============================================================================*/

#define FILE_FIFO_SIZE   10240U
static uint8_t file_buff[FILE_FIFO_SIZE];
static struct fifo_obj file_fifo;

void file_init(void)
{
	fifo_init(&file_fifo,file_buff,sizeof(file_buff));
}

void file_log(uint8_t* buff , uint16_t len)
{
	uint8_t* pdata = (uint8_t*)buff;

	if((file_fifo.get_remain(&file_fifo)) > len)
	{
		for(uint16_t i=0;i<len;i++)
		{
			file_fifo.push(&file_fifo , *pdata);
			pdata++;
		}
	}
	else
	{
		FAT_LOG("file write over size!");
	}
}

/*=========================================================================
  ����״̬�£�
	�����Լ�� fifo���Ƿ�����Ҫд������� 
	��fifo��Ϊ��˵����������Ҫд�룬����ready״̬

  ready״̬�£�
	���й��ز��������سɹ������д״̬��ʧ�����ٴγ��Թ��أ�3�ν���err״̬

  ��д��״̬�£�
    1.�ж�fifo ���ݴ�С���������󣬼����ȴ�
	2.���ļ� ��ʧ�����ٴγ��ԣ�3��ʧ�� ����err״̬
	3.�ɹ��򿪺�д������
    4.�ر��ļ�, д����ȷ���л�״̬��IDLE
	5.�жϵ�ǰ�ļ���С������ϴ������£�1��ļ�¼���������µ��ļ�

  ��err״̬�£�
	������󣬻ص�IDLE״̬
=========================================================================*/
static uint8_t file_state;
enum
{
	FILE_STATE_IDLE,
	FILE_STATE_READY,
	FILE_STATE_WRITE,
	FILE_STATE_READ,
	FILE_STATE_ERR
};


char file_name[24] = "Log20201227.txt";

uint8_t flush_force;
uint32_t byteswritten;                /* File write counts */
uint32_t bytesread;                   /* File read counts */

void file_flush(void)
{
	static uint32_t tick;
	static uint8_t err;
	
	if(!tick)
	{
		tick = HAL_GetTick();
	}
	if((HAL_GetTick()-tick)>300)
	{
		tick = 0;
		switch(file_state)
		{
			case FILE_STATE_IDLE:
			{
				if(!file_fifo.is_empty(&file_fifo))
				{
					err = 0;
					file_state = FILE_STATE_READY;
				}
			}break;
			case FILE_STATE_READY:
			{
				if(f_mount(&SDFatFS, "0", 0)== 0)
				{
					err = 0;
					file_state = FILE_STATE_WRITE;
				}
				else
				{
					err++;
					if(err>=3)
					{
						file_state = FILE_STATE_ERR;
					}
				}
			}break;
			case FILE_STATE_WRITE:
			{
				/*�ж�fifoʹ�����*/
				if(file_fifo.used_size >= 1024 || flush_force)
				{
					flush_force = 0;
					if(f_open(&SDFile, file_name, FA_OPEN_APPEND | FA_WRITE) == 0)
					{
						 uint8_t* ptemp = (uint8_t*)malloc(1024);
						 uint8_t* p_index = ptemp;
						 uint16_t len =  1024;
						 uint16_t size = len;
						
						 if(ptemp == NULL)
						 {
							FAT_LOG("file temp buff malloc fail!");
						 }
						 else
						 {
							 while(len--)
							 {
								*p_index = file_fifo.pop(&file_fifo);
								 p_index++;
							 }
							 
							 FAT_LOG("[%d]file start writing ...",HAL_GetTick());
							 f_write(&SDFile, ptemp, size, (void *)&byteswritten);
							 f_sync(&SDFile);
							 FAT_LOG("[%d]file write done...",HAL_GetTick());
							 
							 if(byteswritten != size)
							 {
								 FAT_LOG("file write not complete: %d!",(size-byteswritten));
							 }
							 file_state = FILE_STATE_IDLE;
							 free(ptemp);
							/*����ļ���С���������ʱ��Ҫ����洢��һ��*/
						    /*������һ���ļ������洢��ǰ�ļ����� */
						 }
						 f_close(&SDFile);
						f_mount(0, "", 0);
					}
					else
					{
						err++;
						if(err>=3)
						{
							file_state = FILE_STATE_ERR;
						}
					}
				}
			}break;
			case FILE_STATE_READ:
			{
			
			}break;
			case FILE_STATE_ERR:
			{
				err = 0;
				file_state = FILE_STATE_IDLE;
			}break;
		}
	}
	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
