#include <p18f4620.h>
#include "Main_Screen.h"
#include "Fan_Support.h"
#include "Utils.h"
#include "stdio.h"

char duty_cycle;

void Do_Beep()
{
    Activate_Buzzer();                                                          // Activate the buzzer
    Wait_One_Sec();                                                         // Wait for half second 
    Deactivate_Buzzer();
    do_update_pwm(duty_cycle);
}

void Wait_Half_Second()                                                         //Routine to wait half second
{
    T0CON = 0x03;                                                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                               // set the lower byte of TMR
    TMR0H = 0x0B;                                                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                       // turn off the Timer 0
}

void Wait_One_Sec()                                                             // Wait a on seconond Function
{
    Wait_Half_Second();                                                         // Wait half second
                                                                        // Set kp to 0
    Wait_Half_Second();                                                         // Wait half second
                                                                        // Set KP to 1
}

void Deactivate_Buzzer()                                                        // Function to Deactivate the Buzzer
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0; 
}

void Activate_Buzzer()                                                          // Function to Activate the Buzzer
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

