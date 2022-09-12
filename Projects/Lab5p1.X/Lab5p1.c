#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>


#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF

#define DP PORTDbits.RD7

void SET_ADC_CH(char); 
void INIT_ADC(void); 
void DO_INIT(void); 
void init_UART(void);
int U, L;
int array[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10}; // array for lighting up the display

void WAIT_1_SEC()                                               //Delay Function
{
    for (int j=0; j<0xffff; j++);                               //For loop to implement Delay
}
void putch (char c)
{
while (!TRMT);
TXREG = c;
}
unsigned int GET_FULL_ADC(void);

void DO_DISPLAY_7SEG_Upper(char digit)                          //Function to handle the 7Seg Lower digit
{                                              
    PORTC = array[digit] & 0x3F;                                //Handling the tens place
    char PC = array[digit] & 0x01;                              //Code in this section to handle busted c0 pin
    char PE = array[digit] & 0x40;                              //Code in this section to handle busted c0 pin
    if (PE == 0 && PC ==0) 
        PORTE = 0x00;                                           // sets port e to zero
    else if (PC == 0 && PE != 0)
        PORTE = 0x01;                                           //makes it so pin e0 is one
    else if (PC == 1 && PE == 0)
        PORTE = 0x02;                                           //makes pin e1 or c0 one
    else if (PC == 1 && PE != 0)
        PORTE = 0x03;                                           // sets both to one
}
    
    

void DO_DISPLAY_7SEG_Lower(char digit)                          //Function to handle the 7Seg Upper digit
{
    PORTD = array[digit];                                       //Handling the tens place
}

void main()
{
DO_INIT();
while(1)
{
SET_ADC_CH(0);
int ivolt = GET_FULL_ADC() * 4; // get input voltage
float volt = ivolt / 1000.0;
printf ("volt = %f\r\n",volt); // display voltage results
U = ((int)(volt));
L = ((int)(volt*10)%10);
DO_DISPLAY_7SEG_Upper(U);                                            // Display upper value on upper 7seg
DO_DISPLAY_7SEG_Lower(L);                                            // Display decimal value on lower 7seg 
DP = 0;
WAIT_1_SEC();
}
}

void SET_ADC_CH(char ADC_CH) // Set ADC channels
{
ADCON0 = (ADC_CH * 4) + 1;
}
void INIT_ADC(void)
{
ADCON0=0x01; // select channel AN0, and turn on the ADC subsystem
ADCON1=0x1B ; // select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage
ADCON2=0xA9; // right justify the result. Set the bit conversion time (TAD) and acquisition time
}
void init_UART(void)
{
OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
OSCCON = 0x60;
}
void DO_INIT()
{
init_UART();
TRISA = 0xFF; 
TRISB = 0x00; 
TRISC = 0x00; 
TRISD = 0x00; 
TRISE = 0x00; 
INIT_ADC();
}
unsigned int GET_FULL_ADC(void)
{
int result;
ADCON0bits.GO = 1; // Start Conversion
while(ADCON0bits.DONE == 1); // Wait for conversion to be completed (DONE=0)
result = (ADRESH * 0x100 + ADRESL); // Combine result of upper byte and
//lower byte into
return result; // return the most significant 8- bits of the result.
}