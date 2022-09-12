#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>

#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF

#define B0 PORTBbits.RB0                                        //Definitions to simplify coding
#define B1 PORTBbits.RB1                                        //Definitions to simplify coding
#define B2 PORTBbits.RB2                                        //Definitions to simplify coding
#define B3 PORTBbits.RB3                                        //Definitions to simplify coding
#define B4 PORTBbits.RB4                                        //Definitions to simplify coding
#define B5 PORTBbits.RB5                                        //Definitions to simplify coding


int array[10] = {0x40,0x79,0x24,0x30,0x19,                      //Declaring and initializing Array 
0x12,0x02,0x78,0x00,0x10};

unsigned int nstep;                                             //Declaring variable for

float Tc;                                                       //Declaring variable for
float F;                                                        //Declaring variable for
int tempF;                                                      //Declaring variable for

void WAIT_1_SEC()                                               //Delay Function
{
    for (int j=0; j<0xffff; j++);                               //For loop to implement Delay
}
   
void putch(char c)       
{
    while(!TRMT);
    TXREG = c;
}

unsigned int Get_Full_ADC(void)
{
    int result;                                                 // Where the result is stored
    ADCON0bits.GO = 1;                                          //Start Conversion
    while(ADCON0bits.DONE==1);                                  //Wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                         //Combine result and upper byte and lower byte
                                                                //Into the result
    return result;                                              //Return the result
}

void Init_ADC(void)                                             //Function to Initialize ADCs
{
    ADCON0 = 0x01;                                              //Setting ADC0
    ADCON1 = 0x1B;                                              //Setting ADC1
    ADCON2 = 0xA9;                                              //Setting ADC2
}

void init_UART()                                                //Function to Initialize UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
        USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
        USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void Select_ADC_Channel(char channel)                           // Function to set up the Steps
{
    ADCON0 = (channel*4) + 1;
}

void DO_DISPLAY_7SEG_Lower(char digit)                          //Function to handle the 7Seg Lower digit
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
    
    

void DO_DISPLAY_7SEG_Upper(char digit)                          //Function to handle the 7Seg Upper digit
{
    PORTD = array[digit];                                       //Handling the tens place
}

void D1_None(void)                                              //Setting output for RGB LED D1 off
{
    B5 = 0;
    B4 = 0;
    B3 = 0;
}

void D1_Red(void)                                               //Setting output for RGB LED D1 Red
{
    B5 = 0;
    B4 = 0;
    B3 = 1;
} 
void D1_Green(void)                                             //Setting output for RGB LED D1 Green
{
    B5 = 0;
    B4 = 1;
    B3 = 0;
}
void D1_Blue(void)                                               //Setting output for RGB LED D1 Blue
{
    B5 = 1;
    B4 = 0;
    B3 = 0;
}
void D1_White(void)                                              //Setting output for RGB LED D1 White
{
    B5 = 1;
    B4 = 1;
    B3 = 1;
}

void D2_None(void)                                               //Setting output for RGB LED D2 off
{
    B2 = 0;
    B1 = 0;
    B0 = 0;
}
void D2_Red(void)                                                //Setting output for RGB LED D2 Red
{
    B2 = 0;
    B1 = 0;
    B0 = 1;
}
void D2_Green(void)                                              //Setting output for RGB LED D2 Green
{
    B2 = 0;
    B1 = 1;
    B0 = 0;
}
void D2_Yellow(void)                                            //Setting output for RGB LED D2 Yellow
{
    B2 = 0;
    B1 = 1;
    B0 = 1;
}
void D2_Blue(void)                                             //Setting output for RGB LED D2 Blue
{
    B2 = 1;
    B1 = 0;
    B0 = 0;
}
void D2_Purple(void)                                           //Setting output for RGB LED D2 Purple              
{ 
    B2 = 1;
    B1 = 0;
    B0 = 1;
}
void D2_Cyan(void)                                             //Setting output for RGB LED D2 Cyan
{
    B2 = 1;
    B1 = 1;
    B0 = 0;
}
void D2_White(void)                                            //Setting output for RGB LED D2 White
{
    B2 = 1;
    B1 = 1;
    B0 = 1;
}


void DO_DISPLAY_D1(int T)                                      //Setting output for D1 based on Temp
{
    if(T < 45) D1_None();                                      //Set D1 off if Temp is below 45
    else if (T >= 46 && T < 56) D1_Red();                      //Set D1  Red if Temp is between 46 and 56
    else if (T >= 56 && T < 66) D1_Green();                    //Set D1 Green if Temp is between 56 and 66
    else if (T >= 66 && T < 76) D1_Blue();                     //Set D1 Blue if Temp is between 66 and 76
    else if (T >= 76) D1_White();                              //Set D1 White if Temp is greater than 76
}

void DO_DISPLAY_D2(int T)                                      //Setting output for D2 based on Temp
{
    if (T > 70)                                                //Set D2 White if T is greater than 70
        D2_White();
    else
        PORTB = T/10;                                          //Divide T by 10 and send to port B                 
        
}

void DO_DISPLAY_D3(int mv)                                      //Setting output for D3 based on Voltage
{
    if (mv < 2500)
    {
        PORTA = 0x01<<4;                                       //Set Port A if Voltage lower than 2500mv
    }
    else if (mv >= 2500 && mv < 3400)
    {
        PORTA = 0x02<<4;                                       //Set Port A if Voltage is between 2500mv and 3400mv
    }
    else 
    {
        PORTA = 0x03<<4;                                       //Set Port A if Voltage is greater than 3400
    }
}

void main()
{
    init_UART();                                               //Initializing UART
    Init_ADC();                                                //Initializing ADC
    TRISA = 0x0F;                                              //Make PORTA Inputs 0xFF
    TRISB = 0x00;                                              //Make PORTB all outputs
    TRISC = 0x00;                                              //Make PORTC all outputs
    TRISD = 0x00;                                              //Make PORTD all outputs
    TRISE = 0x00;                                              //Make PORTE all outputs
    PORTC = 0xFF;                                              //Clearing PortC
    PORTD = 0xFF;                                              //Clearing PortD
    PORTB = 0x00;                                              //Clearing PortB
    
    while(1)
    { 
        Select_ADC_Channel(0);                                 //Calling the Function to Set the Channel
        int num_step = Get_Full_ADC();                         //Calling Function to measure the steps and assign it to declared variable
        float voltage_mv = num_step * 4.0;                     //Multiply steps by 4 to get the height of the step and assign it to the declared Variable
        float temperature_C = (1035.0 - voltage_mv) / 5.50;    //Converting Voltage to Celcius
        float temperature_F = (1.80 * temperature_C) + 32.0;   //Converting Celcius to Fahrenheit
        int tempF = (int) temperature_F;                       //Assigning the value of Degree Fahrenheit to a int
        char U = tempF / 10;                                   //Setting Upper Digit
        char L = tempF % 10;                                   //Setting Lower Digit 
        DO_DISPLAY_7SEG_Upper(U);                              //Send Upper digit Value to Upper 7seg
        DO_DISPLAY_7SEG_Lower(L);                              //Send Lower digit Value to Lower 7seg
        
        printf ("Steps - %d \r\n", num_step);                  //Output results for Tera Term
        printf ("Voltage = %f (mV)\r\n", voltage_mv);          //Output results for Tera Term
        printf (" Temperature = %d F \r \n", tempF);           //Output results for Tera Term
        DO_DISPLAY_D2(tempF);                                  //Calling Function for D2
        DO_DISPLAY_D1(tempF);                                  //Calling Function for D1
         
        Select_ADC_Channel(1);                                 //Calling the Function to set the channel
        int num_step = Get_Full_ADC();                         //Calling Function to measure the steps and assign it to declared variable
        int voltage_mv1 = num_step * 4;                        //Multiply steps by 4 to get the height of the step and assign it to the declared Variable
        printf ("Light Volt = %d mV \r\n\n", voltage_mv1);     //Output results for Tera Term
        DO_DISPLAY_D3(voltage_mv1);                            //Calling Function for D3
        
        WAIT_1_SEC();                                          //Calling Delay Function
    }
}