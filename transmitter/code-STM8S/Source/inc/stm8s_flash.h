
  
  
#ifndef __STM8S_FLASH_H__
#define __STM8S_FLASH_H__


void EEPROM_write(uint8_t addr,const uint8_t *indata,uint8_t inlength);
void EEPROM_read(uint8_t addr,uint8_t *indata,uint8_t inlength);


#endif

