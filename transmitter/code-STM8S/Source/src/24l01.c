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


#include "type_def.h"
#include "24l01.h"
#include "delay.h"
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

/**************************************************************************
 * ��������SPI_Init
 * ����  ��SPIģ�����ú���
 * ����  ����
 *
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ���� 
 *************************************************************************/
void  SPI_Init()
{
  CLK_PCKENR1|=1<<1;
  SPI_CR1 = (0<<7)|(0<<6)|(1<<3)|(1<<2)|(0<<1)|(0<<0);  /* �ȷ���MSB �Ƚ�ֹSPI ��������Ϊ fbus/4 ����Ϊ��ģʽ ����״̬ʱSCKΪ�͵�ƽ ���ݴӵ�һ��ʱ�ӱ��ؿ�ʼ���� */
  SPI_CR2 = (0<<7)|(0<<5)|(0<<4)|(0<<2)|(1<<1)|(1<<0);  /* ��Ϊȫ˫��ģʽ ʹ��������豸���� �ڲ����豸ѡ��Ϊ��ģʽ */
  SPI_ICR = (0<<7)|(0<<6)|(0<<5)|(0<<4);                /* ��ֹ�����ж� */
  SPI_CR1 |= (1<<6);                                    /* ����SPIģ�� */
}

/**************************************************************************
 * ��������SPI_SendByte
 * ����  ��SPIģ�鷢�ͺ���
 * ����  ����
 *
 * ���  ����
 * ����  ����
 * ����  ���ⲿ����
 *************************************************************************/
#define RXNE (1<<0) // ���ջ������ǿ�
#define TXE  (1<<1)  //���ͻ�������
unsigned char SPI_ReadWriteByte(unsigned char byte)
{
  uint16_t retry=0;
  while(!(SPI_SR & TXE))              //�ȴ����ͼĴ���Ϊ�� 
  {
      retry++;
      if(retry>=0XFE)return 0; 	//��ʱ�˳�
  }
  SPI_DR = byte;                        //�����͵�����д�����ݼĴ��� 
  retry=0;
  
  while(!(SPI_SR & RXNE))              // �ȴ����ܼĴ����� 
  {
      retry++;
      if(retry>=0XFE)return 0; 	//��ʱ�˳�
  }
 return SPI_DR;                        // �����ݼĴ��� 
}
#undef RXNE
#undef TXE


#define drv_spi_read_write_byte SPI_ReadWriteByte
#define drv_delay_ms delay_ms

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
  *			@Speed:�ٶ�
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

/**
  * @brief :NRF24L01��������
  * @param :
  *			@rxbuf:�������ݴ�ŵ�ַ
  * @note  :��
  * @retval:���յ������ݸ���
  */ 
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//Ƭѡ
	drv_spi_read_write_byte( FLUSH_RX );
	RF24L01_SET_CS_HIGH( );
	
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		drv_delay_ms( 100 );
		
		if( 30 == l_100MsTimes++ )		//3sû���չ����ݣ����³�ʼ��ģ��
		{
//			NRF24L01_Gpio_Init( );
//			RF24L01_Init( );
			RF24L01_Set_Mode( MODE_RX );
			break;
		}
	}
	
	l_Status = NRF24L01_Read_Reg( STATUS );		//��״̬�Ĵ���
	NRF24L01_Write_Reg( STATUS,l_Status );		//���жϱ�־
	if( l_Status & RX_OK)	//���յ�����
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//���յ����� 
		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//���RX FIFO
		return l_RxLength; 
	}	
	
	return 0;				//û���յ�����	
}


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
    NRF24L01_Set_RxAddr( 0, (uint8_t*)RX_ADDRESS_0, 5 );                 //����RX��ַ
	
	NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_MAX_RT ) |*/		//���ν����ж�
                                      ( 1 << EN_CRC ) |     //ʹ��CRC
									  ( 1 << CRCO ) |       //2���ֽ�
                                      ( 1 << PWR_UP ) );    //�����豸
}

/**
  * @brief :NRF24L01������ʼ��
  * @param :��
  * @note  :��
  * @retval:��
  */ 
void NRF24L01_Base_Init(void)
{	
	//�������� 
	//CE ����Ϊ��� IRQ����Ϊ����
    
    PC_DDR |= ((1<<5)|(1<<6)); //PC4,5,6���ģʽ
    PC_CR1 |= ((1<<5)|(1<<6)); //PC4,5,6�������
    PC_CR2 |= ((1<<5)|(1<<6)); //10mhz���
	
    PD_DDR |= 1<<2; //PE5���ģʽ
    PD_CR1 |= 1<<2; //PE5�������
    PD_CR2 |= 1<<2; //10mhz���
    
    PE_DDR |= 1<<5; //PE5���ģʽ
    PE_CR1 |= 1<<5; //PE5�������
    PE_CR2 |= 1<<5; //10mhz���
 	
    PC_DDR &= ~(1<<7);
    PC_CR1 |= 1<<7;        //PC7��������
    
    PD_DDR &= ~(1<<0);
    PD_CR1 |= 1<<0;        //PD0��������
    PD_CR2 |= 1<<0;        //PD0�ж�����
    EXTI_CR1|=(2<<6);//PD�½��ش���
    ITC_SPR2_VECT6SPR = 2;//�ж����ȼ�2  
	
    SPI_Init();         //��ʼ��SPI
    NRF24L01_CE=0; 	//ʹ��24L01
    NRF24L01_CSN=1;	//SPIƬѡȡ��
}



