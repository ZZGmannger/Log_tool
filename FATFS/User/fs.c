
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
  空闲状态下：
	周期性检查 fifo中是否有需要写入的数据 
	当fifo不为空说明有数据需要写入，进入ready状态

  ready状态下：
	进行挂载操作，挂载成功则进入写状态，失败则再次尝试挂载，3次进入err状态

  在写入状态下：
    1.判断fifo 数据大小，若不够大，继续等待
	2.打开文件 ，失败则再次尝试，3次失败 进入err状态
	3.成功打开后，写入数据
    4.关闭文件, 写入正确后切换状态到IDLE
	5.判断当前文件大小，到达较大的情况下（1天的记录），创建新的文件

  在err状态下：
	处理错误，回到IDLE状态
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
				/*判断fifo使用情况*/
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
							/*检查文件大小，如果满足时间要求，如存储了一天*/
						    /*产生下一个文件名，存储当前文件名。 */
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
