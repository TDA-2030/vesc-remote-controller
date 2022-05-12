/**
  ******************************************************************************
  * @author  ��ҫ�Ƽ� ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   NRF24L01����C�ļ�
  ******************************************************************************
  * @attention
  *
  * ����	:	http://www.ashining.com
  * �Ա�	:	https://shop105912646.taobao.com
  * ����Ͱ�:	https://cdzeyao.1688.com
  ******************************************************************************
  */


#include "stm32f10x.h"
#include "24l01.h"
#include "delay.h"
#include "spi.h"


//////////////////////////////////////////////////////////////////////////////////	 

//NRF24L01 ��������	   
/****************************************************
PB4----> IRQ    MISO <----PC7
PC6----> MOSI   SCK  <----PC5
PB5----> CSN    CE   <----PA3
3V3----> VCC    GND  <----GND


****************************************************/

////////////////////////////////////////////////////////////////////////////////// 	  



//������uint SPI_RW(uint uchar)
//���ܣ�NRF24L01��SPIдʱ��
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
  * @brief :NRF24L01���Ĵ���
  * @param :
           @Addr:�Ĵ�����ַ
  * @note  :��ַ���豸����Ч
  * @retval:��ȡ������
  */
uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//������ ��ַ
    btmp = drv_spi_read_write_byte( 0xFF );				//������
	
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
	
    return btmp;
}

/**
  * @brief :NRF24L01��ָ�����ȵ�����
  * @param :
  *			@reg:��ַ
  *			@pBuf:���ݴ�ŵ�ַ
  *			@len:���ݳ���
  * @note  :���ݳ��Ȳ�����255����ַ���豸����Ч
  * @retval:��ȡ״̬
  */
void NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//������ ��ַ
    for( btmp = 0; btmp < len; btmp ++ )
    {
        *( pBuf + btmp ) = drv_spi_read_write_byte( 0xFF );	//������
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01д�Ĵ���
  * @param :��
  * @note  :��ַ���豸����Ч
  * @retval:��д״̬
  */
void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//д���� ��ַ
    drv_spi_read_write_byte( Value );			//д����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01дָ�����ȵ�����
  * @param :
  *			@reg:��ַ
  *			@pBuf:д������ݵ�ַ
  *			@len:���ݳ���
  * @note  :���ݳ��Ȳ�����255����ַ���豸����Ч
  * @retval:д״̬
  */
void NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t i;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//д���� ��ַ
    for( i = 0; i < len; i ++ )
    {
        drv_spi_read_write_byte( *( pBuf + i ) );		//д����
    }
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :���TX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( FLUSH_TX );	//��TX FIFO����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :���RX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Rx_Fifo( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( FLUSH_RX );	//��RX FIFO����
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :����ʹ����һ������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( REUSE_TX_PL );		//����ʹ����һ������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01�ղ���
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Nop( void )
{
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( NOP );		//�ղ�������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

/**
  * @brief :NRF24L01��״̬�Ĵ���
  * @param :��
  * @note  :��
  * @retval:RF24L01״̬
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    Status = drv_spi_read_write_byte( NRF_READ_REG + STATUS );	//��״̬�Ĵ���
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
	
    return Status;
}

/**
  * @brief :NRF24L01���ж�
  * @param :
           @IRQ_Source:�ж�Դ
  * @note  :��
  * @retval:�����״̬�Ĵ�����ֵ
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//�жϱ�־����
    btmp = NRF24L01_Read_Status_Register( );			//��״̬�Ĵ���
			
    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( NRF_WRITE_REG + STATUS );	//д״̬�Ĵ�������
    drv_spi_read_write_byte( IRQ_Source | btmp );		//����Ӧ�жϱ�־
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
	
    return ( NRF24L01_Read_Status_Register( ));			//����״̬�Ĵ���״̬
}

/**
  * @brief :��RF24L01�ж�״̬
  * @param :��
  * @note  :��
  * @retval:�ж�״̬
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//�����ж�״̬
}
 
 /**
  * @brief :��FIFO�����ݿ��
  * @param :��
  * @note  :��
  * @retval:���ݿ��
  */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );		//Ƭѡ
	
    drv_spi_read_write_byte( R_RX_PL_WID );	//��FIFO�����ݿ������
    btmp = drv_spi_read_write_byte( 0xFF );	//������
	
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
	
    return btmp;
}

 /**
  * @brief :�����յ�������
  * @param :��
  * @note  :��
  * @retval:
           @pRxBuf:���ݴ�ŵ�ַ�׵�ַ
  */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
	
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//������״̬
    Width = NRF24L01_Read_Top_Fifo_Width( );		//���������ݸ���

    RF24L01_SET_CS_LOW( );		//Ƭѡ
    drv_spi_read_write_byte( RD_RX_PLOAD );			//����Ч��������
	
    for( PipeNum = 0; PipeNum < Width; PipeNum ++ )
    {
        *( pRxBuf + PipeNum ) = drv_spi_read_write_byte( 0xFF );		//������
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
    NRF24L01_Flush_Rx_Fifo( );	//���RX FIFO
	
    return Width;
}

 /**
  * @brief :�������ݣ���Ӧ��
  * @param :
  *			@pTxBuf:�������ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//���ݳ����Լ32 ��ֻ����32��

    NRF24L01_Flush_Tx_Fifo( );		//��TX FIFO
	
    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( WR_TX_PLOAD );	//��������
	
    for( btmp = 0; btmp < length; btmp ++ )
    {
        drv_spi_read_write_byte( *( pTxBuf + btmp ) );	//��������
    }
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
}

 /**
  * @brief :�������ݣ�����Ӧ��
  * @param :
  *			@pTxBuf:�������ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//���ݳ��ȴ���32 ���ߵ���0 ��ִ��
    }
	
    RF24L01_SET_CS_LOW( );	//Ƭѡ
    drv_spi_read_write_byte( WR_TX_PLOAD_NACK );	//��������
    while( len-- )
    {
        drv_spi_read_write_byte( *pTxBuf );			//��������
		pTxBuf++;
    }
    RF24L01_SET_CS_HIGH( );		//ȡ��Ƭѡ
}

 /**
  * @brief :�ڽ���ģʽ����TX FIFOд����(��ACK)
  * @param :
  *			@pData:���ݵ�ַ
  *			@len:����
  * @note  :һ�β�����32���ֽ�
  * @retval:��
  */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;
	
	len = ( len > 32 ) ? 32 : len;		//���ݳ��ȴ���32����ֻд32���ֽ�

    RF24L01_SET_CS_LOW( );			//Ƭѡ
    drv_spi_read_write_byte( W_ACK_PLOAD );		//����
    for( btmp = 0; btmp < len; btmp ++ )
    {
        drv_spi_read_write_byte( *( pData + btmp ) );	//д����
    }
    RF24L01_SET_CS_HIGH( );			//ȡ��Ƭѡ
}

 /**
  * @brief :���÷��͵�ַ
  * @param :
  *			@pAddr:��ַ��ŵ�ַ
  *			@len:����
  * @note  :��
  * @retval:��
  */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
	len = ( len > 5 ) ? 5 : len;					//��ַ���ܴ���5���ֽ�
    NRF24L01_Write_Buf( TX_ADDR, pAddr, len );	//д��ַ
}

 /**
  * @brief :���ý���ͨ����ַ
  * @param :
  *			@PipeNum:ͨ��
  *			@pAddr:��ַ����ŵ�ַ
  *			@Len:����
  * @note  :ͨ��������5 ��ַ���Ȳ�����5���ֽ�
  * @retval:��
  */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//ͨ��������5 ��ַ���Ȳ�����5���ֽ�

    NRF24L01_Write_Buf( RX_ADDR_P0 + PipeNum, pAddr, Len );	//д���ַ
}

 /**
  * @brief :����ͨ���ٶ�
  * @param :
  *			@Speed:�ٶ�
  * @note  :��
  * @retval:��
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
  * @brief :���ù���
  * @param :
  *			@Power:����
  * @note  :��
  * @retval:��
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
  * @brief :����Ƶ��
  * @param :
  *			@FreqPoint:Ƶ�����ò���
  * @note  :ֵ������127
  * @retval:��
  */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_Write_Reg(  RF_CH, FreqPoint & 0x7F );
}

//NRF24L01��麯��
//����:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;  	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//��?5??????.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //??��????  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//??24L01??	
	return 0;		 //???24L01
}


 /**
  * @brief :����ģʽ
  * @param :
  *			@Mode:ģʽ����ģʽ�����ģʽ
  * @note  :��
  * @retval:��
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
  * @brief :NRF24L01����һ������
  * @param :
  *			@txbuf:�����������׵�ַ
  *			@Length:�������ݳ���
  * @note  :��
  * @retval:
  *			MAX_TX���ﵽ����ط�����
  *			TX_OK���������
  *			0xFF:����ԭ��
  */ 
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
//	uint8_t l_Status = 0;
//	uint16_t l_MsTimes = 0;
//	
//	RF24L01_SET_CS_LOW( );		//Ƭѡ
//	drv_spi_read_write_byte( FLUSH_TX );
//	RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_LOW( );		
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//д���ݵ�TX BUF 32�ֽ�  TX_PLOAD_WIDTH
	RF24L01_SET_CE_HIGH( );			//��������
//	while( 0 != RF24L01_GET_IRQ_STATUS( ))
//	{
//		drv_delay_ms( 1 );
//		if( 500 == l_MsTimes++ )						//500ms��û�з��ͳɹ������³�ʼ���豸
//		{
////			NRF24L01_Gpio_Init( );
////			RF24L01_Init( );
//			RF24L01_Set_Mode( MODE_TX );
//			break;
//		}
//	}
//	l_Status = NRF24L01_Read_Reg(STATUS);						//��״̬�Ĵ���
//	NRF24L01_Write_Reg( STATUS, l_Status );						//���TX_DS��MAX_RT�жϱ�־
//	
//	if( l_Status & MAX_TX )	//�ﵽ����ط�����
//	{
//		NRF24L01_Write_Reg( FLUSH_TX,0xff );	//���TX FIFO�Ĵ���
//		return MAX_TX; 
//	}
//	if( l_Status & TX_OK )	//�������
//	{
//		return TX_OK;
//	}
	
	return 0xFF;	//����ԭ����ʧ��
}

///**
//  * @brief :NRF24L01��������
//  * @param :
//  *			@rxbuf:�������ݴ�ŵ�ַ
//  * @note  :��
//  * @retval:���յ������ݸ���
//  */ 
//uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
//{
//	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
//	
//	RF24L01_SET_CS_LOW( );		//Ƭѡ
//	drv_spi_read_write_byte( FLUSH_RX );
//	RF24L01_SET_CS_HIGH( );
//	
////	while( 0 != RF24L01_GET_IRQ_STATUS( ))
////	{
////		drv_delay_ms( 100 );
////		
////		if( 30 == l_100MsTimes++ )		//3sû���չ����ݣ����³�ʼ��ģ��
////		{
//////			NRF24L01_Gpio_Init( );
//////			RF24L01_Init( );
////			RF24L01_Set_Mode( MODE_RX );
////			break;
////		}
////	}
//	
//	l_Status = NRF24L01_Read_Reg( STATUS );		//��״̬�Ĵ���
//	NRF24L01_Write_Reg( STATUS,l_Status );		//���жϱ�־
//	if( l_Status & RX_OK)	//���յ�����
//	{
//		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
//		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//���յ����� 
//		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
//		return l_RxLength; 
//	}	
//	
//	return 0;				//û���յ�����	
//}

const uint8_t TX_ADDRESS[5]=  {'F','U','C','K','!'}; //���͵�ַ
const uint8_t RX_ADDRESS_0[5]={'F','U','C','K','!'}; //P0���յ�ַ
 /**
  * @brief :RF24L01ģ���ʼ��
  * @param :��
  * @note  :��
  * @retval:��
  */
void RF24L01_Init( void )
{
	
    RF24L01_SET_CE_LOW( );
    //NRF24L01_Clear_IRQ_Flag( IRQ_ALL );
	
	
#if DYNAMIC_PACKET == 1
	
    NRF24L01_Write_Reg( DYNPD, ( 1 << 0 ) ); 	//ʹ��ͨ��1��̬���ݳ���
    NRF24L01_Write_Reg( FEATRUE, 0x07 );
    NRF24L01_Read_Reg( DYNPD );
    NRF24L01_Read_Reg( FEATRUE );
	
#elif DYNAMIC_PACKET == 0
    
    NRF24L01_Write_Reg(RX_PW_P0,FIXED_PACKET_LEN); 	//�̶����ݳ���
	
#endif	//DYNAMIC_PACKET
    
    NRF24L01_Write_Reg( EN_AA, ( 1 << ENAA_P0 ) );   		//ͨ��0�Զ�Ӧ��
    NRF24L01_Write_Reg( EN_RXADDR, ( 1 << ERX_P0 ) );		//ͨ��0����
    NRF24L01_Write_Reg( SETUP_AW, AW_5BYTES );     			//��ַ��� 5���ֽ�
    NRF24L01_Write_Reg( SETUP_RETR, ARD_4000US |
                        ( REPEAT_CNT & 0x0F ) );         	//�ظ��ȴ�ʱ�� 250us
	
    RF24LL01_Write_Hopping_Point(80);            			//��ʼ��ͨ��
	NRF24L01_Set_Power(POWER_0DBM);           //���ù���
	NRF24L01_Set_Speed(SPEED_2M);             //����ͨ���ٶ�
	
	NRF24L01_Set_TxAddr( (uint8_t*)TX_ADDRESS, 5 );                      //����TX��ַ
    NRF24L01_Set_RxAddr( 0, (uint8_t*)RX_ADDRESS_0, 5 );                   //����RX��ַ
	
	
	NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//�����ж�
                                      ( 1 << EN_CRC ) |     //ʹ��CRC 
									  ( 1 << CRCO )        //2���ֽ�
                                  /*  ( 1 << PWR_UP )*/ );    //�����豸
}

/**
  * @brief :NRF24L01��ʼ��
  * @param :��
  * @note  :��
  * @retval:��
  */ 
void NRF24L01_Base_Init( void )
{
	//�������� 
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE );//ʱ��ʹ�� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//��ʼ��NRF_IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//��ʼ��NRF_CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//��ʼ��NRF_CSN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
	
    //GPIOE.2 �ж����Լ��жϳ�ʼ������   �½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
	
  	EXTI_InitStructure.EXTI_Line=EXTI_Line8;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			    //ʹ�����ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	
    SPI1_Init();         //��ʼ��SPI
    RF24L01_SET_CE_LOW( ); 	//ʹ��24L01
    RF24L01_SET_CS_HIGH( );	//SPIƬѡȡ��
	
}


////24L01���ͽ������ݿ�ȶ���
//#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
//#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
//#define TX_PLOAD_WIDTH  32		//5�ֽڵ��û����ݿ��
//#define RX_PLOAD_WIDTH  32		//5�ֽڵ��û����ݿ��
//
////����NRF24L01����һ������
////txbuf:�����������׵�ַ
////����ֵ:�������״��
//u8 NRF24L01_TxPacket(u8 *txbuf)
//{
//	u8 sta;
//// 	SPI2_SetSpeed(SPI_SPEED_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
//	NRF24L01_CE=0;
//  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
// 	NRF24L01_CE=1;//��������	   
//	while(NRF24L01_IRQ!=0);//�ȴ��������
//	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
//	if(sta&MAX_TX)//�ﵽ����ط�����
//	{
//		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
//		return MAX_TX; 
//	}
//	if(sta&TX_OK)//�������
//	{
//		return TX_OK;
//	}
//	return 0xff;//����ԭ����ʧ��
//}
////����NRF24L01����һ������
////txbuf:�����������׵�ַ
////����ֵ:0��û�н������ݣ�1�����ճɹ����������������
//u8 NRF24L01_RxPacket(u8 *rxbuf)
//{
//	u8 sta;		    							   
////	SPI2_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
//	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
//	if(sta&RX_OK)//���յ�����
//	{
//		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
//		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
//		return 1; 
//	}	   
//	return 0;//û�յ��κ�����
//}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	RF24L01_SET_CE_LOW( );	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_0,5);//дRX�ڵ��ַ
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,80);	     //����RFͨ��Ƶ��		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,32);//ѡ��ͨ��0����Ч���ݿ�� 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
//	NRF24L01_Write_Reg( CONFIG,       ( 1 << PRIM_RX )|		//����
//                                      ( 1 << EN_CRC ) |     //ʹ��CRC 
//									  ( 1 << CRCO ) |       //1���ֽ�
//                                      ( 1 << PWR_UP ) );    //�����豸
  	RF24L01_SET_CE_HIGH( ); //CEΪ��,�������ģʽ 
    
}
////�ú�����ʼ��NRF24L01��TXģʽ
////����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
////PWR_UP,CRCʹ��
////��CE��ߺ�,������RXģʽ,�����Խ���������		   
////CEΪ�ߴ���10us,����������.	 
//void NRF24L01_TX_Mode(void)
//{														 
//	NRF24L01_CE=0;	    
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,5);//дTX�ڵ��ַ 
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS_0,5); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
//
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,80);       //����RFͨ��Ϊ40
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
////  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
//	NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//�����ж�
//                                      ( 1 << EN_CRC ) |     //ʹ��CRC 1���ֽ�
//                                      ( 1 << PWR_UP ) );    //�����豸
//	NRF24L01_CE=1;//CEΪ��,10us����������
//
//}
//
//
//
