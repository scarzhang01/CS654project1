#include "motor.h"

void motor_init(uint8_t chan)
{
    // Setup Timer 2
    CLEARBIT(T2CONbits.TON);   // Disable Timer
    CLEARBIT(T2CONbits.TCS);   // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
    TMR2 = 0x00;               // Clear timer register
    T2CONbits.TCKPS = 0b10;    // Select 1:64 Prescaler
    CLEARBIT(IFS0bits.T2IF);   // Clear Timer2 interrupt status flag
    CLEARBIT(IEC0bits.T2IE);   // Disable Timer2 interrupt enable control bit
    PR2 = 4000;                // Set timer period 20ms since 4000= 20*10^-3 * 12.8*10^6 * 1/64

    if (chan == 0)
    {
        // For x, setup OC8
        CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
        OC8R = 0;           /* Set the initial duty cycle to 1.5ms, init at 0 degree*/
        OC8RS = PR2 - 300;          /* Load OCRS: next pwm duty cycle */
        OC8CON = 0x0006;            /* Set OC8: PWM, no fault check, Timer2 */
    }
    else
    {
        // For y, setup OC7
        CLEARBIT(TRISDbits.TRISD6); /* Set OC7 as output */
        OC7R = 0;  /* Set the initial duty cycle to 1.5ms, init at 0 degree*/
        OC7RS = PR2 - 300; /* Load OCRS: next pwm duty cycle */
        OC7CON = 0x0006;   /* Set OC7: PWM, no fault check, Timer2 */
    }

    SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
}

void motor_set_duty(uint8_t chan, uint16_t duty_us)
{
    if (chan == 0)
    {
        // Update x servo
        OC8RS = duty_us; /* Load OC8RS: next pwm duty cycle */
    }
    else
    {
        // Update y servo
        OC7RS = duty_us; /* Load OC7RS: next pwm duty cycle */
    }
}