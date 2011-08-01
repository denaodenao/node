//
//
//#include "apl_foundation.h"
//#include "../../common/openwsn/rtl/rtl_frame.h"
//#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
////#include "../../common/openwsn/hal/opennode2010/hal_led.h"
//
//USART_InitTypeDef USART_InitStructure;
//
//#define MAX_IEEE802FRAME154_SIZE                128//todo
//
//#define FAILED 1
//#define RXFIFO_START 0x180
//#define  channel 11
//
//#define GPIO_SPI GPIOB
//#define SPI_pin_MISO  GPIO_Pin_14
//#define SPI_pin_MOSI  GPIO_Pin_15
//#define SPI_pin_SCK   GPIO_Pin_13
//#define SPI_pin_SS    GPIO_Pin_12
//
//
///***********************************************************************************
// * LOCAL FUNCTIONS
// */
//
//static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
//TiIEEE802Frame154Descriptor m_desc;
//
//void RCC_Configuration(void);
//static void CC2520_Activate( void);
//static void SPI_GPIO_Configuration( void);
//static void USART_GPIO_Configuration( void);
//static uint8 USART_Send( uint8 ch);
//
//
//void RCC_Configuration(void)
//{
//	RCC_PCLK2Config(RCC_HCLK_Div8);
//	RCC_PCLK1Config(RCC_HCLK_Div8);
//
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//
//}
//
//static void SPI_GPIO_Configuration( void)
//{
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_MOSI|SPI_pin_SCK;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_MISO;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_SS;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//}
//
//static void USART_GPIO_Configuration( void)
//{
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//
//	// Configure USART2 Rx (PA.3) as input floating 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//}
//
//static void CC2520_Activate( void)
//{
//	int i;
//    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
////   /*
////	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_Init(GPIOA, &GPIO_InitStructure);
////    */
//	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_Init( GPIOB,&GPIO_InitStructure);
//	
//
//	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init( GPIOB,&GPIO_InitStructure);
//
//	//GPIO_ResetBits( GPIOA,GPIO_Pin_8);
//
//	/*********************************************************/
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
//	//GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
//	//GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_1)//reset the cc2520 nRST
//    //GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
//	//GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14);//read the input of the SO.
//    /************************************************************************/
//    GPIO_ResetBits( GPIOB,GPIO_Pin_1);//reset the cc2520 nRST
//	GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
//	for ( i=0;i<13500;i++);//wait for the regulator to be stabe.
//
//	GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
//	GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
//	for ( i=0;i<13500;i++);//wait for the output of SO to be 1//todo for testing
//	hal_assert( GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));//todo该语句报错，可能是因为SO引脚的 输出模式改变的原
//	GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
//	hal_delayus( 2 );
//}
//
//
//static uint8 USART_Send( uint8 ch)
//{
//	USART_SendData( USART2,ch);
//	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//	{
//	}
//}
//
//
//
///***********************************************************************************
// * @fn          main
// *
// * @brief       This is the main entry of the "Read Regs" application
// *
// * @param       none
// *
// * @return      none
// */
//
//void main (void)
//{
//	int i;
//    uint16 g;
//	uint8 len;
//	uint8 state;
//	uint8 k;
//	uint8 data[40];
//	TiFrame *rxbuf;//todo
//	char * ptr;//todo
//	TiIEEE802Frame154Descriptor * desc;//todo
//    g = 0x01;
//	state = 0x00;
//	len = 0x00;
//    RCC_Configuration( );
//    led_open();
//    led_off( LED_RED);//todo 下面三句的顺序不能变
//    CC2520_Activate();
//	SPI_GPIO_Configuration();
//	CC2520_SPI_OPEN( );
//	//CC2520_SPI_BEGIN();
//    USART_GPIO_Configuration();
//
//	/***********************************************/
//	USART_InitStructure.USART_BaudRate = 9600;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//
//	USART_Init( USART2,&USART_InitStructure);
//	USART_Cmd( USART2,ENABLE);
//	/***********************************************/
//    //现在还缺少相应寄存器的配置;这可能是导致发送失败的原因之一
//    halRfInit();//todo 设置相应的寄存器
//
//	halRfSetPower( TXPOWER_4_DBM);
//	halRfSetChannel( channel);
//	halRfSetShortAddr( 0x02);
//	halRfSetPanId( 0x01);
//
//	CC2520_REGWR8( CC2520_FRMFILT0,0x00);//disable the filter
//	//当禁止使用filter功能时，RX_FRM_ACCEPTED  exception将不能发生，但是RX_FRM_DONE
//	//仍然可以发生，从而可以作为判断是否有帧到达的依据。
//	//desc = ieee802frame154_open( &m_desc );
//	//rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
//
//	CC2520_SRFOFF();
//	CC2520_SRXON();
//	while (1)//todo for testing
//	{
//		
//		if (CC2520_REGRD8( CC2520_EXCFLAG1)&0x11)
//		{
//			led_toggle( LED_RED);
//
//			len = CC2520_RXBUF8();
//
//			data[0]=len;
//			
//			CC2520_RXBUF( len,data+1);
//			 
//
//			for ( i=0;i<len+1;i++)
//			{
//				USART_Send( data[i]);
//			}
//            CC2520_REGWR8(CC2520_EXCFLAG1,0x00);//todo clear the exception
//            CC2520_SFLUSHRX();//todo clear the rxfifo
//			CC2520_SFLUSHRX();//todo clear the rxfifo
////			
//		}
//		
//	}
//	
//}
//
//
//


#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"

#define CONFIG_LISTENER    
#undef  CONFIG_LISTENER    

#define TEST_CHOICE 1
//#define TEST_ACK
//#undef  TEST_ACK


#define PANID				0x0002
#define LOCAL_ADDRESS		0x03
#define REMOTE_ADDRESS		0x04
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11
#define MAX_IEEE802FRAME154_SIZE                128

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
void recvnode1(void);


int main(void)
{
   recvnode1();
   
}

void recvnode1(void)
{
    TiCc2520Adapter * cc;
	TiFrame * rxbuf;
	uint8 len;
    uint8 i;
    char *pc;
	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );
	halUartInit(9600,0);
	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );

	cc2520_open( cc, 0, NULL, NULL, 0x00 );
	
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );							//Enable RX
	cc2520_disable_addrdecode( cc );					//禁止地址译码
	#ifdef TEST_ACK
	cc2420_enable_autoack(cc);
	#endif

	cc2520_setpanid( cc, PANID );					//网络标识
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    
	while(1) 
	{
		frame_reset( rxbuf,0,0,0);
		
		len = cc2520_read( cc, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00 );
        if ( len)
        {
			frame_setlength( rxbuf,len);
            pc = frame_startptr(rxbuf);
            for ( i=0;i<len;i++)
            {
                USART_Send( pc[i]);
            }
			led_toggle( LED_RED);
        }
	}
}

