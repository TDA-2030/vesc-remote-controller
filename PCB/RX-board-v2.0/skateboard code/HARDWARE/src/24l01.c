/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   NRF24L01配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */


#include "stm32f10x.h"
#include "24l01.h"
#include "delay.h"
#include "spi.h"


//////////////////////////////////////////////////////////////////////////////////	 

//NRF24L01 驱动函数	   
/****************************************************
PB4----> IRQ    MISO <----PC7
PC6----> MOSI   SCK  <----PC5
PB5----> CSN    CE   <----PA3
3V3----> VCC    GND  <----GND


****************************************************/

////////////////////////////////////////////////////////////////////////////////// 	  



//函数：uint SPI_RW(uint uchar)
//功能：NRF24L01的SPI写时序
/*
u8 SPI_ReadWriteByte(u8 byte)
{
    unsigned char i;
    
  

    for(i = 0; i < 8; i++) 
    {
        if (byte & 0x80) 
            NRF24L01_MOSI=1;
        else
            NRF24L01_MOSI=0;
        byte <<= 1; 
        NRF24L01_SCK=1;  
        if (NRF24L01_MISO==1)
            byte++; 
        NRF24L01_SCK=0; 
    }
     NRF24L01_MOSI=1;
    return byte;                    
}*/




#define drv_spi_read_write_byte SPI1_ReadWriteByte
//#define drv_delay_ms delay_ms

/**
  * @brief :NRF24L01读寄存器
  * @param :
           @Addr:寄存器地址
  * @note  :地址在设备中有效
  * @retval:读取的数据
  */
uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//片选
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//读命令 地址
    btmp = drv_spi_read_write_byte( 0xFF );				//读数据
	
    RF24L01_SET_CS_HIGH( );			//取消片选
	
    return btmp;
}

/**
  * @brief :NRF24L01读指定长度的数据
  * @param :
  *			@reg:地址
  *			@pBuf:数据存放地址
  *			@len:数据长度
  * @note  :数据长度不超过255，地址在设备中有效
  * @retval:读取状态
  */
void NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//片选
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//读命令 地址
    for( btmp = 0; btmp < len; btmp ++ )
    {
        *( pBuf + btmp ) = drv_spi_read_write_byte( 0xFF );	//读数据
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01写寄存器
  * @param :无
  * @note  :地址在设备中有效
  * @retval:读写状态
  */
void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//写命令 地址
    drv_spi_read_write_byte( Value );			//写数据
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01写指定长度的数据
  * @param :
  *			@reg:地址
  *			@pBuf:写入的数据地址
  *			@len:数据长度
  * @note  :数据长度不超过255，地址在设备中有效
  * @retval:写状态
  */
void NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t i;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//写命令 地址
    for( i = 0; i < len; i ++ )
    {
        drv_spi_read_write_byte( *( pBuf + i ) );		//写数据
    }
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :清空TX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( FLUSH_TX );	//清TX FIFO命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :清空RX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Rx_Fifo( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( FLUSH_RX );	//清RX FIFO命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :重新使用上一包数据
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( REUSE_TX_PL );		//重新使用上一包命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01空操作
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Nop( void )
{
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( NOP );		//空操作命令
	
    RF24L01_SET_CS_HIGH( );		//取消片选
}

/**
  * @brief :NRF24L01读状态寄存器
  * @param :无
  * @note  :无
  * @retval:RF24L01状态
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    Status = drv_spi_read_write_byte( NRF_READ_REG + STATUS );	//读状态寄存器
	
    RF24L01_SET_CS_HIGH( );		//取消片选
	
    return Status;
}

/**
  * @brief :NRF24L01清中断
  * @param :
           @IRQ_Source:中断源
  * @note  :无
  * @retval:清除后状态寄存器的值
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//中断标志处理
    btmp = NRF24L01_Read_Status_Register( );			//读状态寄存器
			
    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( NRF_WRITE_REG + STATUS );	//写状态寄存器命令
    drv_spi_read_write_byte( IRQ_Source | btmp );		//清相应中断标志
    RF24L01_SET_CS_HIGH( );			//取消片选
	
    return ( NRF24L01_Read_Status_Register( ));			//返回状态寄存器状态
}

/**
  * @brief :读RF24L01中断状态
  * @param :无
  * @note  :无
  * @retval:中断状态
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//返回中断状态
}
 
 /**
  * @brief :读FIFO中数据宽度
  * @param :无
  * @note  :无
  * @retval:数据宽度
  */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );		//片选
	
    drv_spi_read_write_byte( R_RX_PL_WID );	//读FIFO中数据宽度命令
    btmp = drv_spi_read_write_byte( 0xFF );	//读数据
	
    RF24L01_SET_CS_HIGH( );		//取消片选
	
    return btmp;
}

 /**
  * @brief :读接收到的数据
  * @param :无
  * @note  :无
  * @retval:
           @pRxBuf:数据存放地址首地址
  */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
	
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//读接收状态
    Width = NRF24L01_Read_Top_Fifo_Width( );		//读接收数据个数

    RF24L01_SET_CS_LOW( );		//片选
    drv_spi_read_write_byte( RD_RX_PLOAD );			//读有效数据命令
	
    for( PipeNum = 0; PipeNum < Width; PipeNum ++ )
    {
        *( pRxBuf + PipeNum ) = drv_spi_read_write_byte( 0xFF );		//读数据
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
    NRF24L01_Flush_Rx_Fifo( );	//清空RX FIFO
	
    return Width;
}

 /**
  * @brief :发送数据（带应答）
  * @param :
  *			@pTxBuf:发送数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//数据长达大约32 则只发送32个

    NRF24L01_Flush_Tx_Fifo( );		//清TX FIFO
	
    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( WR_TX_PLOAD );	//发送命令
	
    for( btmp = 0; btmp < length; btmp ++ )
    {
        drv_spi_read_write_byte( *( pTxBuf + btmp ) );	//发送数据
    }
    RF24L01_SET_CS_HIGH( );			//取消片选
}

 /**
  * @brief :发送数据（不带应答）
  * @param :
  *			@pTxBuf:发送数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//数据长度大于32 或者等于0 不执行
    }
	
    RF24L01_SET_CS_LOW( );	//片选
    drv_spi_read_write_byte( WR_TX_PLOAD_NACK );	//发送命令
    while( len-- )
    {
        drv_spi_read_write_byte( *pTxBuf );			//发送数据
		pTxBuf++;
    }
    RF24L01_SET_CS_HIGH( );		//取消片选
}

 /**
  * @brief :在接收模式下向TX FIFO写数据(带ACK)
  * @param :
  *			@pData:数据地址
  *			@len:长度
  * @note  :一次不超过32个字节
  * @retval:无
  */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;
	
	len = ( len > 32 ) ? 32 : len;		//数据长度大于32个则只写32个字节

    RF24L01_SET_CS_LOW( );			//片选
    drv_spi_read_write_byte( W_ACK_PLOAD );		//命令
    for( btmp = 0; btmp < len; btmp ++ )
    {
        drv_spi_read_write_byte( *( pData + btmp ) );	//写数据
    }
    RF24L01_SET_CS_HIGH( );			//取消片选
}

 /**
  * @brief :设置发送地址
  * @param :
  *			@pAddr:地址存放地址
  *			@len:长度
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
	len = ( len > 5 ) ? 5 : len;					//地址不能大于5个字节
    NRF24L01_Write_Buf( TX_ADDR, pAddr, len );	//写地址
}

 /**
  * @brief :设置接收通道地址
  * @param :
  *			@PipeNum:通道
  *			@pAddr:地址存肥着地址
  *			@Len:长度
  * @note  :通道不大于5 地址长度不大于5个字节
  * @retval:无
  */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//通道不大于5 地址长度不大于5个字节

    NRF24L01_Write_Buf( RX_ADDR_P0 + PipeNum, pAddr, Len );	//写入地址
}

 /**
  * @brief :设置通信速度
  * @param :
  *			@Speed:速度
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_Speed( nRf24l01SpeedType Speed )
{
	uint8_t btmp = 0;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP );
	btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	
	if( Speed == SPEED_250K )		//250K
	{
		btmp |= ( 1<<5 );
	}
	else if( Speed == SPEED_1M )   //1M
	{
   		btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	}
	else if( Speed == SPEED_2M )   //2M
	{
		btmp |= ( 1<<3 );
	}

	NRF24L01_Write_Reg( RF_SETUP, btmp );
}

 /**
  * @brief :设置功率
  * @param :
  *			@Power:功率
  * @note  :无
  * @retval:无
  */
void NRF24L01_Set_Power( nRf24l01PowerType Power )
{
    uint8_t btmp;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP ) & ~0x07;
    switch( Power )
    {
        case POWER_F18DBM:
            btmp |= PWR_18DB;
            break;
        case POWER_F12DBM:
            btmp |= PWR_12DB;
            break;
        case POWER_F6DBM:
            btmp |= PWR_6DB;
            break;
        case POWER_0DBM:
            btmp |= PWR_0DB;
            break;
        default:
            break;
    }
    NRF24L01_Write_Reg( RF_SETUP, btmp );
}

 /**
  * @brief :设置频率
  * @param :
  *			@FreqPoint:频率设置参数
  * @note  :值不大于127
  * @retval:无
  */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_Write_Reg(  RF_CH, FreqPoint & 0x7F );
}

//NRF24L01检查函数
//返回:0：成功;1：失败	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;  	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д?5??????.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //??д????  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//??24L01??	
	return 0;		 //???24L01
}


 /**
  * @brief :设置模式
  * @param :
  *			@Mode:模式发送模式或接收模式
  * @note  :无
  * @retval:无
  */
void RF24L01_Set_Mode( nRf24l01ModeType Mode )
{
    uint8_t controlreg = 0;
	RF24L01_SET_CE_LOW( );
	controlreg = NRF24L01_Read_Reg( CONFIG );
	
    if( Mode == MODE_TX )       
    {
        controlreg &= ~( 1<< PRIM_RX );
		controlreg |= ( 1<< PWR_UP );
        NRF24L01_Write_Reg( CONFIG, controlreg );
    }
    else if( Mode == MODE_RX )
    {	 
        controlreg |= ( 1<< PRIM_RX );
		controlreg |= ( 1<< PWR_UP );
        NRF24L01_Write_Reg( CONFIG, controlreg );
		
    }
	else if( Mode == MODE_PD )
    {
        controlreg &= ~( 1<< PWR_UP );
        NRF24L01_Write_Reg( CONFIG, controlreg );
		
    }
	RF24L01_SET_CE_HIGH();
	delay_us(130);
}

/**
  * @brief :NRF24L01发送一次数据
  * @param :
  *			@txbuf:待发送数据首地址
  *			@Length:发送数据长度
  * @note  :无
  * @retval:
  *			MAX_TX：达到最大重发次数
  *			TX_OK：发送完成
  *			0xFF:其他原因
  */ 
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
//	uint8_t l_Status = 0;
//	uint16_t l_MsTimes = 0;
//	
//	RF24L01_SET_CS_LOW( );		//片选
//	drv_spi_read_write_byte( FLUSH_TX );
//	RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_LOW( );		
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//写数据到TX BUF 32字节  TX_PLOAD_WIDTH
	RF24L01_SET_CE_HIGH( );			//启动发送
//	while( 0 != RF24L01_GET_IRQ_STATUS( ))
//	{
//		drv_delay_ms( 1 );
//		if( 500 == l_MsTimes++ )						//500ms还没有发送成功，重新初始化设备
//		{
////			NRF24L01_Gpio_Init( );
////			RF24L01_Init( );
//			RF24L01_Set_Mode( MODE_TX );
//			break;
//		}
//	}
//	l_Status = NRF24L01_Read_Reg(STATUS);						//读状态寄存器
//	NRF24L01_Write_Reg( STATUS, l_Status );						//清除TX_DS或MAX_RT中断标志
//	
//	if( l_Status & MAX_TX )	//达到最大重发次数
//	{
//		NRF24L01_Write_Reg( FLUSH_TX,0xff );	//清除TX FIFO寄存器
//		return MAX_TX; 
//	}
//	if( l_Status & TX_OK )	//发送完成
//	{
//		return TX_OK;
//	}
	
	return 0xFF;	//其他原因发送失败
}

///**
//  * @brief :NRF24L01接收数据
//  * @param :
//  *			@rxbuf:接收数据存放地址
//  * @note  :无
//  * @retval:接收到的数据个数
//  */ 
//uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
//{
//	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
//	
//	RF24L01_SET_CS_LOW( );		//片选
//	drv_spi_read_write_byte( FLUSH_RX );
//	RF24L01_SET_CS_HIGH( );
//	
////	while( 0 != RF24L01_GET_IRQ_STATUS( ))
////	{
////		drv_delay_ms( 100 );
////		
////		if( 30 == l_100MsTimes++ )		//3s没接收过数据，重新初始化模块
////		{
//////			NRF24L01_Gpio_Init( );
//////			RF24L01_Init( );
////			RF24L01_Set_Mode( MODE_RX );
////			break;
////		}
////	}
//	
//	l_Status = NRF24L01_Read_Reg( STATUS );		//读状态寄存器
//	NRF24L01_Write_Reg( STATUS,l_Status );		//清中断标志
//	if( l_Status & RX_OK)	//接收到数据
//	{
//		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//读取接收到的数据个数
//		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//接收到数据 
//		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//清除RX FIFO
//		return l_RxLength; 
//	}	
//	
//	return 0;				//没有收到数据	
//}

const uint8_t TX_ADDRESS[5]=  {'F','U','C','K','!'}; //发送地址
const uint8_t RX_ADDRESS_0[5]={'F','U','C','K','!'}; //P0接收地址
 /**
  * @brief :RF24L01模块初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void RF24L01_Init( void )
{
	
    RF24L01_SET_CE_LOW( );
    //NRF24L01_Clear_IRQ_Flag( IRQ_ALL );
	
	
#if DYNAMIC_PACKET == 1
	
    NRF24L01_Write_Reg( DYNPD, ( 1 << 0 ) ); 	//使能通道1动态数据长度
    NRF24L01_Write_Reg( FEATRUE, 0x07 );
    NRF24L01_Read_Reg( DYNPD );
    NRF24L01_Read_Reg( FEATRUE );
	
#elif DYNAMIC_PACKET == 0
    
    NRF24L01_Write_Reg(RX_PW_P0,FIXED_PACKET_LEN); 	//固定数据长度
	
#endif	//DYNAMIC_PACKET
    
    NRF24L01_Write_Reg( EN_AA, ( 1 << ENAA_P0 ) );   		//通道0自动应答
    NRF24L01_Write_Reg( EN_RXADDR, ( 1 << ERX_P0 ) );		//通道0接收
    NRF24L01_Write_Reg( SETUP_AW, AW_5BYTES );     			//地址宽度 5个字节
    NRF24L01_Write_Reg( SETUP_RETR, ARD_4000US |
                        ( REPEAT_CNT & 0x0F ) );         	//重复等待时间 250us
	
    RF24LL01_Write_Hopping_Point(80);            			//初始化通道
	NRF24L01_Set_Power(POWER_0DBM);           //设置功率
	NRF24L01_Set_Speed(SPEED_2M);             //设置通信速度
	
	NRF24L01_Set_TxAddr( (uint8_t*)TX_ADDRESS, 5 );                      //设置TX地址
    NRF24L01_Set_RxAddr( 0, (uint8_t*)RX_ADDRESS_0, 5 );                   //设置RX地址
	
	
	NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//接收中断
                                      ( 1 << EN_CRC ) |     //使能CRC 
									  ( 1 << CRCO )        //2个字节
                                  /*  ( 1 << PWR_UP )*/ );    //开启设备
}

/**
  * @brief :NRF24L01初始化
  * @param :无
  * @note  :无
  * @retval:无
  */ 
void NRF24L01_Base_Init( void )
{
	//引脚配置 
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );//时钟使能 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//初始化NRF_IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//初始化NRF_CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//初始化NRF_CSN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	
    //GPIOE.2 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			    //使能所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
	
    SPI1_Init();         //初始化SPI
    RF24L01_SET_CE_LOW( ); 	//使能24L01
    RF24L01_SET_CS_HIGH( );	//SPI片选取消
	
}


////24L01发送接收数据宽度定义
//#define TX_ADR_WIDTH    5   	//5字节的地址宽度
//#define RX_ADR_WIDTH    5   	//5字节的地址宽度
//#define TX_PLOAD_WIDTH  32		//5字节的用户数据宽度
//#define RX_PLOAD_WIDTH  32		//5字节的用户数据宽度
//
////启动NRF24L01发送一次数据
////txbuf:待发送数据首地址
////返回值:发送完成状况
//u8 NRF24L01_TxPacket(u8 *txbuf)
//{
//	u8 sta;
//// 	SPI2_SetSpeed(SPI_SPEED_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
//	NRF24L01_CE=0;
//  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
// 	NRF24L01_CE=1;//启动发送	   
//	while(NRF24L01_IRQ!=0);//等待发送完成
//	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
//	if(sta&MAX_TX)//达到最大重发次数
//	{
//		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
//		return MAX_TX; 
//	}
//	if(sta&TX_OK)//发送完成
//	{
//		return TX_OK;
//	}
//	return 0xff;//其他原因发送失败
//}
////启动NRF24L01发送一次数据
////txbuf:待发送数据首地址
////返回值:0，没有接收数据；1：接收成功，其他，错误代码
//u8 NRF24L01_RxPacket(u8 *rxbuf)
//{
//	u8 sta;		    							   
////	SPI2_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
//	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
//	if(sta&RX_OK)//接收到数据
//	{
//		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
//		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
//		return 1; 
//	}	   
//	return 0;//没收到任何数据
//}					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
	RF24L01_SET_CE_LOW( );	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_0,5);//写RX节点地址
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,80);	     //设置RF通信频率		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,32);//选择通道0的有效数据宽度 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
//	NRF24L01_Write_Reg( CONFIG,       ( 1 << PRIM_RX )|		//接收
//                                      ( 1 << EN_CRC ) |     //使能CRC 
//									  ( 1 << CRCO ) |       //1个字节
//                                      ( 1 << PWR_UP ) );    //开启设备
  	RF24L01_SET_CE_HIGH( ); //CE为高,进入接收模式 
    
}
////该函数初始化NRF24L01到TX模式
////设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
////PWR_UP,CRC使能
////当CE变高后,即进入RX模式,并可以接收数据了		   
////CE为高大于10us,则启动发送.	 
//void NRF24L01_TX_Mode(void)
//{														 
//	NRF24L01_CE=0;	    
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,5);//写TX节点地址 
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_0,5); //设置TX节点地址,主要为了使能ACK	  
//
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,80);       //设置RF通道为40
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
////  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发送模式,开启所有中断
//	NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//接收中断
//                                      ( 1 << EN_CRC ) |     //使能CRC 1个字节
//                                      ( 1 << PWR_UP ) );    //开启设备
//	NRF24L01_CE=1;//CE为高,10us后启动发送
//
//}
//
//
//
