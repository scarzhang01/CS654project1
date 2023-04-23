#include "touch.h"

uint8_t cur_dimension = 0;

void touch_init(void)
{
    // Set up the I/O pins E1, E2, E3 to be output pins
    CLEARBIT(TRISEbits.TRISE1); // I/O pin set to output
    CLEARBIT(TRISEbits.TRISE2); // I/O pin set to output
    CLEARBIT(TRISEbits.TRISE3); // I/O pin set to output

    // Set up the I/O pins E1, E2, E3 so that the touchscreen is in STANDBY mode
    SETBIT(LATEbits.LATE1);
    SETBIT(LATEbits.LATE2);
    CLEARBIT(LATEbits.LATE3);
}

void touch_select_dim(uint8_t dimension)
{
    cur_dimension = dimension;
    // 0 is x dim, 1 is y dim
    if (dimension == 0)
    {
        // Set up the I/O pins E1, E2, E3 so that the touchscreen X-coordinate pin
        // Connects to the ADC
        CLEARBIT(LATEbits.LATE1);
        SETBIT(LATEbits.LATE2);
        SETBIT(LATEbits.LATE3);
    }
    else
    {
        // Set up the I/O pins E1, E2, E3 so that the touchscreen Y-coordinate pin
        // Connects to the ADC
        SETBIT(LATEbits.LATE1);
        CLEARBIT(LATEbits.LATE2);
        CLEARBIT(LATEbits.LATE3);
    }
}

uint16_t touch_read(void)
{
    // 0 is x dim, 1 is y dim
    if (cur_dimension == 0)
    {
        AD1CHS0bits.CH0SA = 0x00F; // Set ADC to Sample AN15 pin
        SETBIT(AD1CON1bits.SAMP);  // Start to sample
        while (!AD1CON1bits.DONE);  // Wait for conversion to finish
        CLEARBIT(AD1CON1bits.DONE); // MUST HAVE! clear conversion done bit
        return ADC1BUF0;            // Return sample
    }
    else
    {
        AD1CHS0bits.CH0SA = 0x009; // Set ADC to Sample AN9 pin
        SETBIT(AD1CON1bits.SAMP);  // Start to sample
        while (!AD1CON1bits.DONE);  // Wait for conversion to finish
        CLEARBIT(AD1CON1bits.DONE); // MUST HAVE! clear conversion done bit
        return ADC1BUF0;            // Return sample
    }
}
