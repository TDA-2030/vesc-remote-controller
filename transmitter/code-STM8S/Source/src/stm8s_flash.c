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
    FLASH_DUKR = 0xAE;	// д���һ����Կ
    FLASH_DUKR = 0x56;	// д��ڶ�����Կ
  } while((--rty)&&((FLASH_IAPSR & 0x08) == 0));	// ������δ�ɹ�������������
}


void EEPROM_lock(void)
{
  FLASH_IAPSR=(u8)(~0x08); //��������
}


//EEPROMָ����ַд��һ������ addr:��Ե�ַ dat:����
void EEPROM_write_byte_uul(unsigned char addr, unsigned char dat)
{
  unsigned char  *p;
  p=(unsigned char  *)(EP_HEADER_ADDR + addr);
  *p=dat;
  while(!(FLASH_IAPSR&0x04)); //�ȴ�д�����ɹ�
}
 

//EEPROMָ����ַ����һ������ addr:��Ե�ַ
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
