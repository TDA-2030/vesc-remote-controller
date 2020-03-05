#ifndef __STMFLASH_H__
#define __STMFLASH_H__

#include <stdint.h>

void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead) ;


						   
#endif

















