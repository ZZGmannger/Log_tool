
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FIFO_H
#define __FIFO_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

struct fifo_obj
{
	uint8_t* buff;
	uint16_t size;
	uint16_t used_size;
	uint16_t head;
	uint16_t tail;

	void (*push)(struct fifo_obj* fifo, uint8_t byte);
	uint8_t (*pop)(struct fifo_obj* fifo);
	uint8_t (*is_full)(struct fifo_obj* fifo);
	uint8_t (*is_empty)(struct fifo_obj* fifo);
	uint16_t (*get_remain)(struct fifo_obj* fifo);
};

int fifo_init(struct fifo_obj* fifo,void* buff , uint16_t size);

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
