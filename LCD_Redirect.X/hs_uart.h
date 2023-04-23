/* 
 * Support for initialization of UART2 in high-speed mode
 * 
 * Author: Renato Mancuso (BU)
 * Date: Mar 19, 2020
 * CS454/654
 *  
 */

#ifndef __HS_UART__
#define	__HS_UART__

#define FCY 12800000UL

#include <p33Fxxxx.h>
#include <stdio.h>
#include <libpic30.h>

#define BRGHV  (((FCY/baud)/16)-1)

#define MIN_JOYSTICK 200
#define MAX_JOYSTICK 1024

#define UART_ERR -1
#define UART_SUC 0

#define BIT9		0x06
#define BIT8_ODD	0x04
#define BIT8_EVEN	0x02
#define BIT8_NO		0x00

#define BIT_STOP_1	0x00
#define BIT_STOP_2	0x01

#define CTRL_SIMPLE	0x00
#define CTRL_FLOW	0x01

#define TX_INT_SINGLE	0x8000
#define TX_INT_EMPTY	0x2000
#define TX_INT_LAST	0x0000

#define RX_INT_FULL	0x00C0
#define RX_INT_3OF4	0x0080
#define RX_INT_SINGLE	0x0040

#define BAUD_9600 9600
#define BAUD_115200 115200

/* UART 2 Initialize */
void uart2_init(unsigned long baudr, unsigned int byteformat, unsigned int mode);

/* UART 2 Send and Receive Functions */
void uart2_send_8(int8_t data);
int8_t uart2_recv(uint8_t* data);

#define ENABLE_REDIRECT()                                           \
 do {                                                               \
    uart2_init(BAUD_115200, BIT8_NO | BIT_STOP_1, CTRL_SIMPLE);     \
    __C30_UART=2;                                                   \
} while (0)

#endif	/* __HS_UART__ */

