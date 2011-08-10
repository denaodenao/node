/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 * 
 * OpenWSN is a free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 * 
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi 
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 * 
 ******************************************************************************/ 

/*******************************************************************************
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 * 
 * @status
 *	- Tested by Jiang Ridong in 2011.05. Released.
 *
 * @author zhangwei(TongJi University) on 20070423
 * 	- revision today.
 * @modified by yan-shixing(TongJi University) on 20090725
 *  - revision. tested ok
 *
 * @modified by zhangwei(TongJi University) on 20091030
 *  - upgrade the frame output function. both the listener mode and non-listener 
 *    mode are using _output_openframe() now.
 * @modified by shi-miaojing(TongJi University) on 20091031
 *	- tested ok.
 * @modified by zhangwei in 2010
 *	- support both the binary format and text ascii format. You can use macro
 *    CONFIG_ASCII_OUTPUT to configure it.
 * @modified by zhangwei on 2011.03.04
 * 	- revision.
 * @modified by zhangwei on 2011.05.06
 * 	- revised.
 ******************************************************************************/ 

/** 
 * If You want to enable the interrupt drive mode of the UART, you should define
 * the following macro. This will improve performance. However, you should guarantee
 * the data can be really sent out. You should check the return value of uart_write()
 * in this case.
 */
//#define CONFIG_UART_INTERRUPT_DRIVEN
//#undef CONFIG_UART_INTERRUPT_DRIVEN

#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"
#include "apl_foundation.h"
#include <stdlib.h>
#include <string.h>
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_interrupt.h"
#include "../../../common/openwsn/hal/opennode2010/hal_led.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cc2520.h"
#include "../../../common/openwsn/hal/opennode2010/hal_uart.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/rtl/rtl_ascii.h"
#include "../../../common/openwsn/rtl/rtl_assert.h"
#include "../../../common/openwsn/rtl/rtl_debugio.h"
#include "../../../common/openwsn/rtl/rtl_frame.h"
#include "../../../common/openwsn/rtl/rtl_framequeue.h"
#include "../../../common/openwsn/rtl/rtl_iobuf.h"
#include "../../../common/openwsn/rtl/rtl_slipfilter.h"
#include "../../../common/openwsn/svc/svc_sio_acceptor.h"

/**
 * This macro controls the apl_ieee802frame154_dump module to output
 * the frame in ascii mode.
 */
#define CONFIG_ASCII_OUTPUT
#undef CONFIG_ASCII_OUTPUT

//#undef CONFIG_ACTIVE_SENDING_MODE
//#define CONFIG_ACTIVE_SENDING_MODE

/**
 * CONFIG_LISTENER
 * If you define this macro, the sniffer will be driven by the listener function. 
 * attention currently, the listener of cc2420 adapter is actually called in the 
 * interrupt mode. so you cannot do complicated time consumption operations inside
 * the listener.
 * 
 * CONFIG_ASCII_OUTPUT
 * To choose which kind of format to output. Binary format or Text ASCII format.
 * to decide the output format of the sniffer. If you define this macro, the sniffer
 * will convert the binary frame values into hex ascii format. each byte will be 
 * represented as two ascii characters.
 */

#define PANID						0x0001
#define LOCAL_ADDRESS				0x05
#define REMOTE_ADDRESS				0x04
#define SIO_RXBUF_SIZE				128
#define DEFAULT_CHANNEL     		11

#define MAX_IEEE802FRAME154_SIZE    128

#define SNIFFER_FMQUE_HOPESIZE  	FRAMEQUEUE_HOPESIZE(4)

#define CMD_DATA_REQUEST            0x01
#define CMD_DATA_RESET              0x02

typedef struct{
  uint32 received;
  uint32 dropped;
}TiSnifferStatistics;

//TiCc2520Adapter m_cc;  //todo����cc2520.c�ж�����
TiUartAdapter m_uart;
static char m_nio_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static char m_nio_rxque[SNIFFER_FMQUE_HOPESIZE];

TiSioAcceptor               m_sac;
TiSlipFilter                m_slip;

char txbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY)];
char rxbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY)];
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
char tmpbuf_block[IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY)];
char rmpbuf_block[ IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY)];
#endif

#ifndef CONFIG_ACTIVE_SENDING_MODE
static char m_sio_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];// m_sio_rxbuf[128];
#endif

static void nss_execute(void);
//static void nss_send_response( TiFrameQueue * fmque, TiSnifferStatistics * stat, TiUartAdapter * uart );
static void nss_send_response( TiSioAcceptor *sac,TiFrameQueue * fmque, TiSnifferStatistics * stat, TiUartAdapter * uart );

/*******************************************************************************
 * functions 
 ******************************************************************************/ 

int main(void)
{
    nss_execute();
}

/** 
 * reliable sniffer. 
 * @return None.
 */
void nss_execute(void)
{
	char * msg = "welcome to sniffer ...";
    TiCc2520Adapter * cc;
    TiFrame * nio_rxbuf;
	TiFrameQueue * nio_rxque;
	TiUartAdapter * uart;

    TiSioAcceptor * sio;
    TiSlipFilter  * slip;

    TiIoBuf *sio_buf_tx;
    TiIoBuf *sio_buf_rx;
    TiIoBuf *sio_buf_tmpx;
    TiIoBuf *sio_buf_rmpx;


	int len = 0;
	uint8 count;
	TiSnifferStatistics stat;

	#ifndef CONFIG_ACTIVE_SENDING_MODE
	uint16 sio_addr;
	//char * sio_rxbuf = &(m_sio_rxbuf[0]);
    TiFrame * sio_rxbuf;
    char *pc;
	#endif

	//target_init();
	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

	#ifndef CONFIG_UART_INTERRUPT_DRIVEN
	rtl_init( dbio_open(9600), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    // dbc_mem( msg, strlen(msg) );
    #endif
	
	// Initialize the TiCc2420Adapter component for wireless network communication 

	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );
	cc = cc2520_open( cc, 0, NULL, NULL, 0x00 );
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );							// enable RX mode
	cc2520_setpanid( cc, PANID );					// network identifier, seems no use in sniffer mode
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2520_disable_addrdecode( cc );				// disable address decoding
	cc2520_disable_autoack( cc );

    sio_buf_tx = iobuf_construct(( void *)(&txbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TXBUF_CAPACITY) );
    sio_buf_rx = iobuf_construct( (void *)(&rxbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_RXBUF_CAPACITY) );
#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio_buf_tmpx = iobuf_construct( (void *)(&tmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
    sio_buf_rmpx = iobuf_construct( (void *)(&rmpbuf_block), IOBUF_HOPESIZE(CONFIG_SIOACCEPTOR_TMPBUF_CAPACITY) );
#endif


    //slip = slip_filter_construct( (void *)(&m_slip),sizeof( m_slip));//
    slip = slip_filter_open( (void *)(&m_slip),sizeof( m_slip));

    sio = sac_construct( (void *)(&m_sac),sizeof(m_sac));

    uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
    uart = uart_open( uart, 2, 9600, 8, 1, 0 );
    hal_assert( uart != NULL );

    sio = sac_open( sio,slip,uart);

    sio->txbuf = sio_buf_tx;
    sio->rxbuf = sio_buf_rx;

#ifdef SIO_ACCEPTOR_SLIP_ENABLE
    sio->rmpbuf = sio_buf_rmpx;
    sio->tmpbuf = sio_buf_tmpx;
#endif
	
	nio_rxque = fmque_construct( &m_nio_rxque[0], sizeof(m_nio_rxque) );
    nio_rxbuf = frame_open( (char*)(&m_nio_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );
    sio_rxbuf = frame_open( (char*)(&m_sio_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	
	memset( &stat, 0x00, sizeof(stat) );
    uart_write( uart,msg,strlen( msg),0x00);

	while(1) 
	{
		// Query arrived frames from the wireless network communication component
		
		len = 0;
		count = 0;
        frame_reset( nio_rxbuf, 0, 0, 0 );
        frame_reset( sio_rxbuf, 0, 0, 0 );
        len = cc2520_read( cc, frame_startptr( nio_rxbuf), frame_capacity( nio_rxbuf), 0x00 );

        if ( len > 0)
        {
            frame_setlength( nio_rxbuf, len );
			stat.received ++;
			
			// @attention: The supervisory upper computer should retrieve the frames
			// out as soon as possible. If there're too many frames inside the queue,
			// then the later frames will failed to be queued, and it will be dropped
			// naturely by the fmque_pushback() function. 
			//
			if (fmque_pushback(nio_rxque, nio_rxbuf) == 0)
				stat.dropped ++;
			
			led_toggle( LED_RED );
        }

		#ifdef CONFIG_ACTIVE_SENDING_MODE
		nss_send_response( nio_rxque, &stat, uart );
		#endif
		
		#ifndef CONFIG_ACTIVE_SENDING_MODE
		
		// Query arrived commands from the serial network communication
		// component. 
		
		count = 0;
		len = 0;
        len = sac_recv( sio,sio_rxbuf,0x00);
		
		
		if (len > 0)
		{
            pc = frame_startptr(sio_rxbuf);
			switch (pc[0])//switch (sio_rxbuf[0])
			{
			case CMD_DATA_REQUEST:
				nss_send_response( sio,nio_rxque, &stat, uart );
				break;

			case CMD_DATA_RESET:
				memset( &stat, 0x00, sizeof(stat) );
				break;
					
			default:
				break;
			}
		}
		#endif /* CONFIG_ACTIVE_SENDING_MODE */
		
		// Simulate the running the cc2420 internal task
		cc2520_evolve( cc );
	}
}

/**
 * Send the response frame back to the upper computer.
 */
void nss_send_response( TiSioAcceptor *sac,TiFrameQueue * fmque, TiSnifferStatistics * stat, TiUartAdapter * uart )
{
	TiFrame * f;
	
	// @attention If you want to output the statistics information, you can uncomment
	// the following lines.
	//dbc_uint16( stat->received );
	//dbc_uint16( stat->dropped );
	
	memset( &stat, 0x00, sizeof(stat) );
	 
	f = fmque_front( fmque );
	while (f != NULL)
	{
		//dbc_write( frame_startptr(f), frame_length(f) );
		// uart_write( uart, frame_startptr(f), frame_length(f), 0x00 );
		
		// Insert a little delay between each transmitting. This seems useless. So 
		// I finally eliminate it from the released source code.
		// hal_delay( 2 );

        sac_send(sac,f,0);
		
		fmque_popfront( fmque );
		f = fmque_front( fmque );
	}
}
