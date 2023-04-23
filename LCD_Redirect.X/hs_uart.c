/* 
 *   Implementation of UART2 init, send/receive functions
 *   for LCD print redirect through UART2 in high-speed mode.
 *    
 *   Author: Renato Mancuso (BU)
 *   Date: Mar 19, 2020
 *   Purpose: CS454/654 Remote Labs
 * 
 */

#include "hs_uart.h"

void uart2_init(unsigned long baudr, unsigned int byteformat, unsigned int mode)
{
	/* Stop UART port */
	U2MODEbits.UARTEN = 0;
	
	/* Disable Interrupts */
	IEC1bits.U2RXIE = 0;
	IEC1bits.U2TXIE = 0;
	
	/* Clear Interrupt flag bits */
	IFS1bits.U2RXIF = 0;
	IFS1bits.U2TXIF = 0;
		
	/* Set IO pins */
	TRISFbits.TRISF4 = 1;	// RX Pin
	TRISFbits.TRISF5 = 0;	// TX Pin
    
    PORTFbits.RF5 = 0;
    
	if (mode) {
		TRISFbits.TRISF12 = 1;	// CTS Pin
		TRISFbits.TRISF13 = 0;	// RTS Pin
	}
	
	/* baud rate */
	U2BRG = ((uint32_t)(FCY/16)/ baudr) ; // With BRGH = 0
	
	/* Operation settings and start port */
	U2MODE = 0;
	U2MODEbits.BRGH = 0;
    
	if (mode) {	// Flow Control Mode
		U2MODEbits.RTSMD = 1;
		U2MODEbits.UEN = 2;
	} else {	// Simple Mode
		U2MODEbits.RTSMD = 0;
		U2MODEbits.UEN = 0;	
	}
	U2MODE |= byteformat & 0x07; // Number of bit, Parity and Stop bits

    //Start port
	U2MODEbits.UARTEN = 1;
    
	/* TX & RX interrupt modes */
	//U2STA |= 1 << 14;
	U2STAbits.UTXEN = 1;

}

void uart2_send_8(int8_t data){
	while (U2STAbits.UTXBF);
	U2TXREG = data; 
	while(!U2STAbits.TRMT);
}

int8_t uart2_recv(uint8_t* data){
		if (U2STAbits.OERR) {
			U2STAbits.OERR = 0;
			return -2;
		}
		if (U2STAbits.URXDA) {
			*data = U2RXREG & 0x00FF;
			return 0;
		} else {
			*data = 0;
			return -1;
		}
	return 0;
}

