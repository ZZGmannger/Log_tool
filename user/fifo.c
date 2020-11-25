
#include "fifo.h"
#include "usart.h"

static uint8_t fifo_pop(struct fifo_obj* fifo);
static void fifo_push(struct fifo_obj* fifo , uint8_t byte);
static uint8_t fifo_is_full(struct fifo_obj* fifo);
static uint8_t fifo_is_empty(struct fifo_obj* fifo);
static uint16_t fifo_get_free_size(struct fifo_obj* fifo);

int fifo_init(struct fifo_obj* fifo,void* buff , uint16_t size)
{
	if((uint8_t*)buff != NULL && size)
	{
		fifo->buff = buff;
		fifo->size = size;
		fifo->head = 0;
		fifo->tail = 0;
		fifo->used_size = 0;
		
		fifo->get_remain = fifo_get_free_size;
		fifo->push = fifo_push;
		fifo->pop = fifo_pop;
		fifo->is_empty = fifo_is_empty;
		fifo->is_full = fifo_is_full;
	
		
		return 0;
	}
	return -1;
}

static uint8_t fifo_is_empty(struct fifo_obj* fifo)
{
	if(fifo ==NULL){
		return 0;
	}
	return (fifo->used_size ? 0:1);
}
static uint8_t fifo_is_full(struct fifo_obj* fifo)
{
	if(fifo == NULL){
		return 1;
	}
	return (fifo->used_size >= fifo->size);
}
static uint16_t fifo_get_free_size(struct fifo_obj* fifo)
{
	if(fifo == NULL){
		return 1;
	}
	return (fifo->size - fifo->used_size);
}
static void fifo_push(struct fifo_obj* fifo , uint8_t byte)
{
	if(fifo->used_size < fifo->size)
	{
		fifo->buff[fifo->tail] = byte;
		fifo->tail = (fifo->tail + 1)%fifo->size;
		++fifo->used_size;
	}
}


static uint8_t fifo_pop(struct fifo_obj* fifo)
{
	uint8_t tmp_data = 0;
	if(fifo->used_size)
	{
		tmp_data = fifo->buff[fifo->head];
		fifo->head = (fifo->head + 1)%fifo->size;
		--fifo->used_size;
	}
	return tmp_data;
}
