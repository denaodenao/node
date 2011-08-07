#include "../../../common/openwsn/configall.h"
#include "../../../common/openwsn/hal/opennode2010/hal_configall.h"
#include "../../../common/openwsn/hal/opennode2010/hal_foundation.h"
#include "apl_foundation.h"
#include "../../../common/openwsn/hal/opennode2010/hal_debugio.h"
#include "../../../common/openwsn/hal/opennode2010/hal_assert.h"
#include "../../../common/openwsn/hal/opennode2010/hal_cpu.h"
#include "../../../common/openwsn/hal/opennode2010/hal_uart.h"

TiUartAdapter               m_uart;


void main( void)
{
    TiUartAdapter * uart;
    uint8 ch;
    char buf[40];
    uintx count;

    count = 0;

    led_open();

    uart = uart_construct( (void *)(&m_uart),sizeof(m_uart));

    uart = uart_open( uart,2,9600,8,1,0);


    while ( 1)
    {
        //count = uart_read(uart,buf,40,0);

        //if ( count)
        //{
            //uart_write( uart,buf,count,0);
            //uart_putchar(uart,0xa0);//todo for testing
        //}
        /*
        if ( uart_getchar(uart,&ch))
        {
            uart_putchar( uart,ch);
            led_toggle(LED_RED);

        }*/

        if ( uart_getchar(uart,&ch))
        {
            uart_putchar(uart,ch);
        }

    }
}
