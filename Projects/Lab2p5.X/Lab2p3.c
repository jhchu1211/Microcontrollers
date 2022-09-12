#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF
#define delay 5
void Delay_One_Sec()
{
    for (int I=0; I<17000;I++);
}


void main()

{

    char i; //Declaring the Variable
    
    TRISA  = 0xFF;
    TRISB  = 0x00;
    TRISC  = 0x00;
    TRISD  = 0x00;
    ADCON1 = 0x0F;
    PORTB  = 0x00;
    PORTC  = 0x00;
    while(1)   // This is an endless loop
    {
        for (i=0; i<8; i++)
        {
            PORTD = i;
            
            Delay_One_Sec();
        } // Sending results to Port C
        
    }
}