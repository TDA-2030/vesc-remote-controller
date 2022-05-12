/******************************************************************************/
/*                        -- 深圳市安信可科技有限公司 --                      */
/*
//  文件名：                                              
//  说明：                                                               
//  编写人员：wuzhibiao                                                                   
//  编写日期：2013年3月15日                                                               
//  程序维护：
//  维护记录：
//
//                                                          
*/ 
// 免责声明：该程序仅用于学习与交流 
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#include "typedef.h"
#include "gpio.h"
#include "nrf24l01p.h"
#include "delay.h"


uchar const TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address
uchar const RX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01};
uchar flag;

/**************************************************/

/**************************************************
Function: init_io();
Description:
  flash led one time,chip enable(ready to TX or RX Mode),
  Spi disable,Spi clock line init high
/**************************************************/
void init_nrf24l01_io(void)
{
	CE=0;			// chip enable
	CSN=1;			// Spi disable	
	SCK=0;			// Spi clock line init high
}
/**************************************************
Function: SPI_RW();

Description:
  Writes one byte to nRF24L01, and return the byte read
  from nRF24L01 during write, according to SPI protocol
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)   // output 8-bit
   	{
   		MOSI = (byte & 0x80);         // output 'byte', MSB to MOSI
   		byte = (byte << 1);           // shift next bit into MSB..
   		SCK = 1;                      // Set SCK high..
		MISO=1;
   		byte |= MISO;       		  // capture current MISO bit
   		SCK = 0;            		  // ..then set SCK low again
   	}
    return(byte);           		  // return read byte
}
/**************************************************/

/**************************************************
Function: SPI_RW_Reg();

Description:
  Writes value 'value' to register 'reg'
/**************************************************/
uchar SPI_RW_Reg(BYTE reg, BYTE value)
{
	uchar status;
  	CSN = 0;                   // CSN low, init SPI transaction
  	status = SPI_RW(reg);      // select register
  	SPI_RW(value);             // ..and write value to it..
  	CSN = 1;                   // CSN high again
  	return(status);            // return nRF24L01 status byte
}
/**************************************************/

/**************************************************
Function: SPI_Read();

Description:
  Read one byte from nRF24L01 register, 'reg'
/**************************************************/
BYTE SPI_Read(BYTE reg)
{
	BYTE reg_val;
  	CSN = 0;                // CSN low, initialize SPI communication...
  	SPI_RW(reg);            // Select register to read from..
  	reg_val = SPI_RW(0);    // ..then read registervalue
  	CSN = 1;                // CSN high, terminate SPI communication
  	return(reg_val);        // return register value
}
/**************************************************/

/**************************************************
Function: SPI_Read_Buf();

Description:
  Reads 'bytes' #of bytes from register 'reg'
  Typically used to read RX payload, Rx/Tx address
/**************************************************/
uchar SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
	uchar status,byte_ctr;

  	CSN = 0;                    		// Set CSN low, init SPI tranaction
  	status = SPI_RW(reg);       		// Select register to write to and read status byte

  	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);    // Perform SPI_RW to read byte from nRF24L01

  	CSN = 1;                           // Set CSN high again

  	return(status);                    // return nRF24L01 status byte
}
/**************************************************/

/**************************************************
Function: SPI_Write_Buf();

Description:
  Writes contents of buffer '*pBuf' to nRF24L01
  Typically used to write TX payload, Rx/Tx address
/**************************************************/
uchar SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
	uchar status,byte_ctr;

  	CSN = 0;                   // Set CSN low, init SPI tranaction
  	status = SPI_RW(reg);    // Select register to write to and read status byte
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
    	SPI_RW(*pBuf++);
  	CSN = 1;                 // Set CSN high again
  	return(status);          // return nRF24L01 status byte
}
/**************************************************/

/**************************************************
Function: RX_Mode();

Description:
  This function initializes one nRF24L01 device to
  RX Mode, set RX address, writes RX payload width,
  select RF channel, datarate & LNA HCURR.
  After init, CE is toggled high, which means that
  this device is now ready to receive a datapacket.
/**************************************************/
void power_off()
{
    CE=0;
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0D); 
    CE=1;
    _delay_us(20);
}
/**************************************************/

/**************************************************
Function: TX_Mode();

Description:
  This function initializes one nRF24L01 device to
  TX mode, set TX address, set RX address for auto.ack,
  fill TX payload, select RF channel, datarate & TX pwr.
  PWR_UP is set, CRC(2 bytes) is enabled, & PRIM:TX.

  ToDo: One high pulse(>10us) on CE will now send this
  packet and expext an acknowledgment from the RX device.
/**************************************************/
void receive_mode_init(void) 
{
	CE=0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x27);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
    SPI_RW_Reg(WRITE_REG+STATUS,0xff); // 清除所有中断标志
  	CE = 1; // Set CE pin high to enable RX device 
}

void clearTXFIFO(void)
{
    CSN = 0;
    SPI_RW(FLUSH_TX); 
    _delay_us(10);
    CSN = 1;
}

uchar send_data(uchar *buf)
{
    unsigned int uiNum; 
    power_off();
	CE=0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  	SPI_Write_Buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH); // Writes data to TX payload

  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x27);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
    IRQ = 1;
	CE=1;
    _delay_us(500);
    CE=0;
    
    uiNum = 0;
    while(IRQ == 1) {
        if (uiNum > 20000) {
            uiNum = 0;
            ifnnrf_CLERN_ALL();
            return 0;
        } else {
            uiNum++;
            _delay_us(10);
        }
    }
    if (SPI_Read(STATUS) & STA_MARK_TX) {
        SPI_RW_Reg(WRITE_REG+STATUS,0xFF); // 清除所有中断标志
        clearTXFIFO();
        return 1;
    } else {
        SPI_RW_Reg(WRITE_REG+STATUS,0xFF); // 清除所有中断标志
        clearTXFIFO();
    }
    return 0;
}

// 这个函数最好频繁调用  或者放在中断里
uchar receive_data(uchar *buf)
{
    uchar RXflag = 0;
    IRQ = 1;
    if (IRQ == 0) {
        if (SPI_Read(STATUS) & STA_MARK_RX) {
            SPI_Read_Buf(RD_RX_PLOAD,buf,TX_PLOAD_WIDTH);
            RXflag = 1;
            SPI_RW_Reg(WRITE_REG+STATUS,0xff); // 清除所有中断标志
        } else {
            ifnnrf_CLERN_ALL();
            RXflag = 0;
        }
    }    
    return RXflag;  
}

void SPI_CLR_Reg(BYTE R_T)
{
  	CSN = 0; 
	if(R_T==1)                  // CSN low, init SPI transaction
  	SPI_RW(FLUSH_TX);             // ..and write value to it..
	else
	SPI_RW(FLUSH_RX);             // ..and write value to it..
  	CSN = 1;                   // CSN high again
}

void ifnnrf_CLERN_ALL()
{
  SPI_CLR_Reg(0);
  SPI_CLR_Reg(1);
  SPI_RW_Reg(WRITE_REG+STATUS,0xff);
  IRQ=1;
}

