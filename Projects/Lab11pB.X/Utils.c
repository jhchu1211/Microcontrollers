#include <p18f4620.h>
//#include "Main_Screen.h"
#include "Fan_Support.h"
#include "Utils.h"
#include "stdio.h"
#include "Main.h"

char duty_cycle;

void Do_Beep()
{
    Activate_Buzzer();                                                          // Activate the buzzer
    Wait_One_Sec();                                                         // Wait for half second 
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

void Wait_One_Sec()                                                             // Wait a on seconond Function
{                                                 // Wait half second
    for (int j=0; j<17000;j++);                                                                    // Set KP to 1
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

