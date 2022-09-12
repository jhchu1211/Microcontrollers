#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF

#define DP PORTDbits.RD7


int num_step;

float volt;

//adjusted
int array[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};


void Init_ADC(void);
unsigned int Get_Full_ADC(void);
void init_UART();
void select_ADC_Channel(char);
void DO_DISPLAY7_SEG_UPPER(char);
void Delay_One_Sec();
void DO_DISPLAY7_SEG_LOWER(char);

void main()
{
 //init_IO();
 init_UART();
 Init_ADC();
 //adjusted
 TRISA = 0xFF; 
 TRISB = 0x00; 
 TRISC = 0x00;
 TRISD = 0x00;
 TRISE = 0X00;

 
while(1)
{

    select_ADC_Channel(0); 
    int num_step = Get_Full_ADC(); 
    float volt = num_step * 4 / 1000;
    //adjusted both u and l
    printf("volt = %f \r\n",volt);
    int U = ((int)(volt));
    int L = ((int)(volt*10)%10);
    DP = 0;

    
    
    DO_DISPLAY7_SEG_LOWER(L);
    DO_DISPLAY7_SEG_UPPER(U);
    

    Delay_One_Sec();
} 

}


void init_UART()
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x60;
} 

void select_ADC_Channel(char channel)
{
    ADCON0 = (channel * 4) + 1;
}


void Init_ADC(void)
{//.adjusted
ADCON0=0x01; // select channel AN0, and turn on the ADC subsystem
ADCON1=0x1B ; // select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage
ADCON2=0xA9; // right justify the result. Set the bit conversion time (TAD) and acquisition time
} 

unsigned int Get_Full_ADC(void)
{
int result;
ADCON0bits.GO=1; // Start Conversion
while(ADCON0bits.DONE==1); // wait for conversion to be completed
result = (ADRESH * 0x100) + ADRESL; // combine result of upper byte and
// lower byte into result
 return result; // return the result.
} 

void DO_DISPLAY7_SEG_UPPER(char digit)
{ //adjsuted
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
        PORTE = 0x03; 
}

void DO_DISPLAY7_SEG_LOWER(char digit)
{
    PORTD = array[digit];
    
}

void Delay_One_Sec()
{
 for(int I=0; I <0xffff; I++);
} 
