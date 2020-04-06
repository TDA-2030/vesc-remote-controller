/*
 * @Author: zhouli
 * @Date: 2020-04-04 15:28:44
 * @LastEditTime: 2020-04-06 23:39:11
 * @Description: file content
 */


#include "type_def.h"
#include "stm8s_flash.h"


#define EP_HEADER_ADDR    0x4000



void EEPROM_unlock(void)
{
	uint16_t rty=500;
  do
  {
    FLASH_CR1=0x00;
    FLASH_CR2=0x00;
    FLASH_NCR2=0xFF;
    FLASH_DUKR = 0xAE;	// 写入第一个密钥
    FLASH_DUKR = 0x56;	// 写入第二个密钥
  } while((--rty)&&((FLASH_IAPSR & 0x08) == 0));	// 若解锁未成功，则重新再来
}


void EEPROM_lock(void)
{
  FLASH_IAPSR=(u8)(~0x08); //重新上锁
}


//EEPROM指定地址写入一个数据 addr:相对地址 dat:数据
void EEPROM_write_byte_uul(unsigned char addr, unsigned char dat)
{
  unsigned char  *p;
  p=(unsigned char  *)(EP_HEADER_ADDR + addr);
  *p=dat;
  while(!(FLASH_IAPSR&0x04)); //等待写操作成功
}
 

//EEPROM指定地址读出一个数据 addr:相对地址
unsigned char EEPROM_read_byte_uul(unsigned char addr)
{
  unsigned char *p;
  p=( unsigned char  *)(EP_HEADER_ADDR + addr); 
  return *p; 
}


void EEPROM_write(uint8_t addr,const uint8_t *indata,uint8_t inlength)
{
	EEPROM_unlock();
	while(inlength--)
	{
		EEPROM_write_byte_uul(addr++,*indata++);
	}
	EEPROM_lock();
}

void EEPROM_read(uint8_t addr,uint8_t *indata,uint8_t inlength)
{
	while(inlength--)
	{
		*indata++ = EEPROM_read_byte_uul(addr++);
	}
}
