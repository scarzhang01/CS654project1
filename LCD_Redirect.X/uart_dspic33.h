#if defined(__dsPIC30F2010__) || defined(__dsPIC30F2011__) || defined(__dsPIC30F2012__) ||defined(__dsPIC30F3010__) || \
	defined(__dsPIC30F3011__) || defined(__dsPIC30F3012__) || defined(__dsPIC30F3013__) || defined(__dsPIC30F3014__) || \
	defined(__dsPIC30F4011__) || defined(__dsPIC30F4012__) || defined(__dsPIC30F4013__) || defined(__dsPIC30F5011__) || \
	defined(__dsPIC30F5013__) || defined(__dsPIC30F5015__) || defined(__dsPIC30F5016__) || defined(__dsPIC30F6010__) || \
	defined(__dsPIC30F6010A__) || defined(__dsPIC30F6011__) || defined(__dsPIC30F6011A__) || defined(__dsPIC30F6012__) ||\
	defined(__dsPIC30F6012A__) || defined(__dsPIC30F6013__) || defined(__dsPIC30F6013A__) || defined(__dsPIC30F6014__) || \
	defined(__dsPIC30F6014A__) || defined(__dsPIC30F6015__)
 
#include <p30fxxxx.h>

#elif defined(__dsPIC33FJ64GP206__) || defined(__dsPIC33FJ64GP306__) || defined(__dsPIC33FJ64GP310__) || defined(__dsPIC33FJ64GP706__) || \
	defined(__dsPIC33FJ64GP708__) || defined(__dsPIC33FJ64GP710__) || defined(__dsPIC33FJ128GP206__) || defined(__dsPIC33FJ128GP306__) || \
	defined(__dsPIC33FJ128GP310__) || defined(__dsPIC33FJ128GP706__) || defined(__dsPIC33FJ128GP708__) || defined(__dsPIC33FJ128GP710__) || \
	defined(__dsPIC33FJ256GP506__) || defined(__dsPIC33FJ256GP510__) || defined(__dsPIC33FJ256GP710__) || defined(__dsPIC33FJ64MC506__) || \
	defined(__dsPIC33FJ64MC508__) || defined(__dsPIC33FJ64MC510__) || defined(__dsPIC33FJ64MC706__) || defined(__dsPIC33FJ64MC710__) || \
	defined(__dsPIC33FJ128MC506__) || defined(__dsPIC33FJ128MC510__) || defined(__dsPIC33FJ128MC706__) || defined(__dsPIC33FJ128MC708__) ||\
	defined(__dsPIC33FJ128MC710__) || defined(__dsPIC33FJ256MC510__) || defined(__dsPIC33FJ256MC710__) 

#include <p33Fxxxx.h>

#elif defined(__PIC24HJ64GP206__) || defined(__PIC24HJ64GP210__) || defined(__PIC24HJ64GP506__) || defined(__PIC24HJ64GP510__) || \
	defined(__PIC24HJ128GP206__) || defined(__PIC24HJ128GP210__) || defined(__PIC24HJ128GP506__) || defined(__PIC24HJ128GP510__) || \
	defined(__PIC24HJ128GP306__) || defined(__PIC24HJ128GP310__) || defined(__PIC24HJ256GP206__) || defined(__PIC24HJ256GP210__) || \
	defined(__PIC24HJ256GP610__)
#include <p24Hxxxx.h>

#endif

#include "uart.h"


/*********************************************************************
* Function Name     : OpenUART1                                      *
* Description       : This function configures the UART mode,        * 
*                     UART Interrupt modes and the Baud Rate         *
* Parameters        : unsigned int config1 operation setting         *
*                     unsigned int config2 TX & RX interrupt modes   *
*                     unsigned int ubrg baud rate setting            *
* Return Value      : None                                           *
*********************************************************************/

#if defined(__dsPIC30F2010__) || defined(__dsPIC30F2011__) || defined(__dsPIC30F2012__) ||defined(__dsPIC30F3010__) || \
	defined(__dsPIC30F3011__) || defined(__dsPIC30F3012__) || defined(__dsPIC30F3013__) || defined(__dsPIC30F3014__) || \
	defined(__dsPIC30F4011__) || defined(__dsPIC30F4012__) || defined(__dsPIC30F4013__) || defined(__dsPIC30F5011__) || \
	defined(__dsPIC30F5013__) || defined(__dsPIC30F5015__) || defined(__dsPIC30F5016__) || defined(__dsPIC30F6010__) || \
	defined(__dsPIC30F6010A__) || defined(__dsPIC30F6011__) || defined(__dsPIC30F6011A__) || defined(__dsPIC30F6012__) ||\
	defined(__dsPIC30F6012A__) || defined(__dsPIC30F6013__) || defined(__dsPIC30F6013A__) || defined(__dsPIC30F6014__) || \
	defined(__dsPIC30F6014A__) || defined(__dsPIC30F6015__)

void OpenUART1(unsigned int config1,unsigned int config2, unsigned int ubrg)
{
    U1BRG  = ubrg;     /* baud rate */
    U1MODE = config1;  /* operation settings */
    U1STA = config2;   /* TX & RX interrupt modes */
}


#elif defined(__dsPIC33FJ64GP206__) || defined(__dsPIC33FJ64GP306__) || defined(__dsPIC33FJ64GP310__) ||\
    defined(__dsPIC33FJ64GP706__) || defined(__dsPIC33FJ64GP708__) || defined(__dsPIC33FJ64GP710__) || \
    defined(__dsPIC33FJ128GP206__) || defined(__dsPIC33FJ128GP306__) || defined(__dsPIC33FJ128GP310__) ||\
    defined(__dsPIC33FJ128GP706__) || defined(__dsPIC33FJ128GP708__) || defined(__dsPIC33FJ128GP710__) || \
    defined(__dsPIC33FJ256GP506__) || defined(__dsPIC33FJ256GP510__) || defined(__dsPIC33FJ256GP710__) || \
    defined(__dsPIC33FJ64MC506__) || defined(__dsPIC33FJ64MC508__) || defined(__dsPIC33FJ64MC510__) || \
    defined(__dsPIC33FJ64MC706__) || defined(__dsPIC33FJ64MC710__) || defined(__dsPIC33FJ128MC506__) || \
    defined(__dsPIC33FJ128MC510__) || defined(__dsPIC33FJ128MC706__) || defined(__dsPIC33FJ128MC708__) ||\
    defined(__dsPIC33FJ128MC710__) || defined(__dsPIC33FJ256MC510__) || defined(__dsPIC33FJ256MC710__) ||\
    defined(__PIC24HJ64GP206__) || defined(__PIC24HJ64GP210__) || defined(__PIC24HJ64GP506__) || \
    defined(__PIC24HJ64GP510__) || defined(__PIC24HJ128GP206__) || defined(__PIC24HJ128GP210__) ||\
    defined(__PIC24HJ128GP506__) || defined(__PIC24HJ128GP510__) || defined(__PIC24HJ128GP306__) ||\
    defined(__PIC24HJ128GP310__) || defined(__PIC24HJ256GP206__) || defined(__PIC24HJ256GP210__) || \
    defined(__PIC24HJ256GP610__)


void OpenUART1(unsigned int config1,unsigned int config2, unsigned int ubrg)
{
    U1BRG  = ubrg;     /* baud rate */
    U1MODE = config1;  /* operation settings */
    U1STA = config2;   /* TX & RX interrupt modes */
}
#endif