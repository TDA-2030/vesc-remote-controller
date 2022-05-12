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
#ifndef _BYTE_DEF_
#define _BYTE_DEF_

#define TX_ADR_WIDTH    5   // 5  bytes TX(RX) address width

#define TX_PLOAD_WIDTH  32  // 32 bytes TX payload


#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define STA_MARK_RX     0X40
#define STA_MARK_TX     0X20
#define STA_MARK_MX     0X10	    

#define IRQ             P11
#define CE              P14
#define CSN             P15
#define SCK             P12
#define MOSI            P13
#define MISO            P10

//***************************************************************//
//                   FUNCTION's PROTOTYPES  //
/****************************************************************
 void SPI_Init(BYTE Mode);     // Init HW or SW SPI
 BYTE SPI_RW(BYTE byte);                                // Single SPI read/write
 BYTE SPI_Read(BYTE reg);                               // Read one byte from nRF24L01
 BYTE SPI_RW_Reg(BYTE reg, BYTE byte);                  // Write one byte to register 'reg'
 BYTE SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);  // Writes multiply bytes to one register
 BYTE SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);   // Read multiply bytes from one register
//*****************************************************************/
void NRF24L01_SPI_Init(BYTE Mode);     // Init HW or SW SPI
uchar SPI_RW(BYTE byte);                                // Single SPI read/write
uchar SPI_Read(BYTE reg);                               // Read one byte from nRF24L01
uchar SPI_RW_Reg(BYTE reg, BYTE byte);                  // Write one byte to register 'reg'
uchar SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);  // Writes multiply bytes to one register
uchar SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);   // Read multiply bytes from one register

void init_nrf24l01_io(void);
void ifnnrf_rx_mode(void);
uchar send_data(uchar *buf);
void receive_mode_init(void);
uchar receive_data(uchar *buf);
void ifnnrf_CLERN_ALL();

#endif
