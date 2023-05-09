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
uint16_t cmpfunc(const void *a, const void *b)
{
    return (*(uint16_t *)a - *(uint16_t *)b);
}

uint16_t median(uint16_t arr[]) {
    qsort(arr, 5, sizeof(uint16_t), cmpfunc);
    return arr[2];
}

void printBits(uint16_t num) {
  int i = 0;
  for (; i < (sizeof(uint16_t) * 8); i++) {
    __C30_UART = 1;
    lcd_printf("%i", num & 0x01);
//    __C30_UART = 2;
    num = num >> 1;
  }
}

// ISR for Timer 1 that updates the position of the servos
// In order to move the ball into a corner
// Arrays for samples
uint16_t xSamples[5];
uint16_t ySamples[5];
int index = 0;
int8_t firstByte_x;
int8_t secondByte_x;
int8_t firstByte_y;
int8_t secondByte_y;
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    // Read x axis mode
    touch_select_dim(0);
    // Delay for signal to be stable
    __delay_ms(10);

    for (index = 0; index < 5; index++)
    {
        xSamples[index] = touch_read();
    }
    touch_select_dim(1);
    // Delay for signal to be stable
    __delay_ms(10);

    for (index = 0; index < 5; index++)
    {
        ySamples[index] = touch_read();
    }
    
    // Print x and y
    uint16_t mX = median(xSamples);
    uint16_t mY = median(ySamples);
    __C30_UART = 1;
    lcd_locate(0, 2);
    lcd_printf("x: %5d", mX);
    lcd_locate(0, 3);
    lcd_printf("y: %5d", mY);
//    __C30_UART = 2;
    
    firstByte_x = (mX) & 0xff;
    secondByte_x = (mX >> 8) & 0xff;
    firstByte_y = (mY) & 0xff;
    secondByte_y = (mY >> 8) & 0xff;
    
    // Send data
    uart2_send_8(firstByte_x);
    uart2_send_8(secondByte_x);
    uart2_send_8(firstByte_y);
    uart2_send_8(secondByte_y);
    
    CLEARBIT(IFS0bits.T1IF);
}

// Setup Timer 1 - interrupt every 0.05 sec using external clock
void timer_init()
{
    // Enable LPOSCEN
    __builtin_write_OSCCONL(OSCCONL | 2);
    // Disable Timer
    T1CONbits.TON = 0;
    // Select external clock 32,767
    T1CONbits.TCS = 1;
    // Disable Synchronization
    T1CONbits.TSYNC = 0;
    // Select 1:1 Prescaler
    T1CONbits.TCKPS = 0b00;
    // Clear timer register
    TMR1 = 0x00;
    // Load the period value
    PR1 = 1639;
    // Set Timer1 Interrupt Priority Level
    IPC0bits.T1IP = 0x01;
    // Clear Timer1 Interrupt Flag
    IFS0bits.T1IF = 0;
    // Enable Timer1 interrupt
    IEC0bits.T1IE = 1;
    // Start Timer
    T1CONbits.TON = 1;
}

int main()
{
    __C30_UART=1;
    lcd_initialize();
    lcd_clear();
    adc_init();
    motor_init(0);
    motor_init(1);

    __delay_ms(2000);
    backLight(100);

    /* Enable redirect to UART2 */
    ENABLE_REDIRECT();
    
    __C30_UART = 1;
    lcd_locate(0, 0);
    lcd_printf("UART is READY");
//    __C30_UART = 2;
    
     // Current letter
    uint8_t dataReceived;
    // Current str
    unsigned char new_pr[4];
    int byte_read = 0;
    uint16_t dutyX;
    uint16_t dutyY;
            
    timer_init();
    
    // Populate buffer
    while (1)
    {
        // If we receive data
        if (uart2_recv(&dataReceived) == 0)
        {     
            new_pr[byte_read] = dataReceived;
            byte_read++;
            
            // If we received an entire packet
            if (byte_read == 4) {
                dutyX = (new_pr[1] << 8) | new_pr[0];
                dutyY = (new_pr[3] << 8) | new_pr[2];
                
                __C30_UART = 1;
                lcd_locate(0, 5);
                lcd_printf("dutyX:%d,dutyY:%d\n", dutyX, dutyY);
                __C30_UART = 2;
                
                motor_set_duty(0, dutyX);
                motor_set_duty(1, dutyY);
                
                byte_read = 0;
            }
            
        }
    }
    
    
    return 0;
}
