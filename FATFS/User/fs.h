

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FS_H
#define __FS_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"

#include "bsp_driver_sd.h"
 
void file_init(void);
void file_log(uint8_t* buff , uint16_t len);
extern uint8_t flush_force;
void file_flush(void);
 

 
#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
