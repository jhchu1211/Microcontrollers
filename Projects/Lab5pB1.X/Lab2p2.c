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


void main()

{

    char in; //Declaring the Variable
    
    TRISA  = 0xFF;
    TRISC  = 0x00;
    ADCON1 = 0x0F;
    while(1)   // This is an endless loop
    {
        in = PORTA; // Assigning Port A to in
        in = in  & 0x07; 
        PORTC = in; // Sending results to Port C
        
    }
}
