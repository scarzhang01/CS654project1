/* 
 *   This project is a demonstration of LCD print 
 *   redirect through UART2 in high-speed mode.
 *    
 *   Author: Renato Mancuso (BU)
 *   Date: Mar 19, 2020
 *   Purpose: CS454/654 Remote Labs
 * 
 */

#define FCY 12800000UL

#include <p33Fxxxx.h>
#include <stdio.h>
#include <libpic30.h>


#include "types.h"
#include "crc16.h"
#include "lcd.h"
#include <stdlib.h>

/* Normal include of LCD functions for backward code compatibility */
#include "lcd.h"

/* Include functions for high-speed UART2 init & communication */
#include "hs_uart.h"

_FOSCSEL(FNOSC_PRIPLL);
_FOSC(OSCIOFNC_OFF & POSCMD_XT);
_FWDT(FWDTEN_OFF);
_FGS(GCP_OFF);


// Min & max pulse width value (ms) corresponding to angles of board
float positive_ninety_degree = 0.9;
float negative_ninety_degree = 2.1;

void adc_init()
{
    // Disable ADC
    CLEARBIT(AD1CON1bits.ADON);
    touch_init();
    AD1PCFGL = 0xFFFF;
    AD1PCFGH = 0xFFFF;
    CLEARBIT(AD1PCFGLbits.PCFG15); // Set AD1 AN15 input pin as analog - touch screen x-axis
    CLEARBIT(AD1PCFGLbits.PCFG9);  // Set AD1 AN9 input pin as analog - touch screen y-axis
    // Configure AD1CON1
    SETBIT(AD1CON1bits.AD12B); // Set 12b Operation Mode
    AD1CON1bits.FORM = 0;      // Set integer output
    AD1CON1bits.SSRC = 0x7;    // Set automatic conversion
    // Configure AD1CON2
    AD1CON2 = 0; // Not using scanning sampling
    // Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC); // Internal clock source
    AD1CON3bits.SAMC = 0x1F;    // Sample-to-conversion clock = 31Tad
    AD1CON3bits.ADCS = 0x9;     // Tad = 9Tcy (Time cycles)
    // Leave AD1CON4 at its default value
    SETBIT(AD1CON1bits.ADON); // Enable ADC
}

// A helper function for comparison in qsort
int cmpfunc(const void *a, const void *b)
{
    return (*(uint16_t *)a - *(uint16_t *)b);
}



int main()
{
    __C30_UART=1;
    lcd_initialize();
    lcd_clear();
    adc_init();


    __delay_ms(2000);
    backLight(100);

    /* Enable redirect to UART2 */
    ENABLE_REDIRECT();
    
    __C30_UART = 1;
    lcd_locate(0, 0);
    lcd_printf("UART is READY");
    __C30_UART = 2;
    
     // Current letter
    uint8_t dataReceived;

     // Arrays for samples
    uint16_t xSamples[5];
    uint16_t ySamples[5];
    int index = 0;
    
    // Populate buffer
    while (1)
    {
        // If we receive data
        if (uart2_recv(&dataReceived) == 0)
        {     
            // Read x axis mode
            touch_select_dim(0);
            // Delay for signal to be stable
            __delay_ms(20);
            // Find median x
            for (index = 0; index < 5; index++)
            {
                xSamples[index] = touch_read();
            }
            qsort(xSamples, 5, sizeof(uint16_t), cmpfunc);

            // Print median x sample on lcd
            __C30_UART = 1;
            lcd_locate(0, 2);
            lcd_printf("X?     = %x", xSamples[2]);
            __C30_UART = 2;
            
            // Send x position
            int8_t firstByte = (xSamples[2]) & 0xff;
            uart2_send_8(firstByte);
            int8_t secondByte = (xSamples[2] >> 8) & 0xff;
            uart2_send_8(secondByte);
            
            __C30_UART = 1;
            lcd_locate(0, 3);
            lcd_printf("Finished");
            __C30_UART = 2;
            
            while(1);
        }
    }
    
    
    return 0;
}
