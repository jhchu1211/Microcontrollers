#include<p18f4620.h>
#include<stdio.h>
#include<math.h>
#include<usart.h>

#pragma config  OSC = INTIO67
#pragma config  WDT = OFF
#pragma config  LVP = OFF
#pragma config  BOREN = OFF
#pragma config CCP2MX = PORTBE

#define DP PORTDbits.RD7
#define E2 PORTEbits.RE2
#define E3 PORTAbits.RA3

int Vdec;
float temp, resist;
int array[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};            // Output of proper RGB values in array 
unsigned int nstep;

float Tc; 
float F;
int tempF;

void WAIT_1_SEC()                                                               //For loop w/ one second delay
{
    for (int j=0; j<0xffff; j++);
}
   
void putch(char c)                                                             
{
    while(!TRMT);
    TXREG = c;
}

unsigned int Get_Full_ADC(void)
{
    int result;                                                                 //Where the result is stored
    ADCON0bits.GO = 1;                                                          //Start Conversion
    while(ADCON0bits.DONE==1);                                                  //Wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;                                         //Combine result and upper byte and lower byte
                                                                                //Into the result
    return result;                                                              //Return the result
}

void Init_ADC(void)
{
    ADCON1 = 0x1A;                                                              // Selects pin A0-A4, set up Vref+/-
    ADCON2 = 0xA9;                                                              // Fight justify the result. Set the bit conversion time (TAD) and
                                                                                // acquisition time 
}

void init_UART()                                                                //Code to Initialize UART
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
        USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
        USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void Select_ADC_Channel(char channel)                                           // Set Channel 
{
    ADCON0 = (channel*4) + 1;
}

void Activate_Buzzer()
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;
} 

void Deactivate_Buzzer()
{
 CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
} 

void SET_D2_YELLOW() 
{
    E3 = 1;
    E2 = 1;
    //PORTA=0x80;
}

void SET_D2_OFF()
{
    E3 = 0;
    E2 = 0;
    //PORTA=0x00;
}

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
    

void DO_DISPLAY_7SEG_Lower(char digit)
{
    PORTD = array[digit];
}

void DO_DISPLAY_D1(float T)                                                       // Display D2 function
{
    if (T > 70) PORTB = 0xFF;                                                               // if temp is above 70, display white
    else                                                                        // else PortB will output T/10 and display color out of D2
        PORTB = T/10;
}

void DO_DISPLAY_D2(float T)                                                       // Display D1 function
{
    if (T < 0.07) SET_D2_YELLOW();
    else SET_D2_OFF();                                                            // if T is above 76, D1 = white
}

void main()
{
    init_UART();
    Init_ADC();
    TRISA = 0xFF;
    TRISB = 0x00;                                                               //Make PORTB all outputs
    TRISC = 0x00;                                                               //Make PORTC all outputs
    TRISD = 0x00;                                                               //Make PORTD all outputs
    TRISE = 0x00;                                                               //Make PORTE all outputs
    
    
    while(1)
    { 
        Select_ADC_Channel(4);                                                  // Select Channel 0
        nstep = Get_Full_ADC();                                                 // Initialize step 
        float voltage_mv = nstep * 4.0;                                      // Calculate voltage in mV
        float vL = ((voltage_mv)/(1000.0));                                  // Calculate voltage in V
        float rL = 10*vL/(4.096 - vL);                                        // 0.1 = rref
        int U = (int) rL; 
        int L = (int) (rL * 10) %10;
                
        if (rL < 0.07)
         {
            Activate_Buzzer();                      // turns buzzer
            SET_D2_YELLOW();
         } 
         else
         {
            Deactivate_Buzzer();
            SET_D2_OFF();
         }
        
        //DO_DISPLAY_D2(rL); 
        DO_DISPLAY_D1(rL);
        WAIT_1_SEC();
        if (rL < 10)
        {
            temp = rL*10;
            resist = (int) temp;
            char U = (int) rL;
            char L = (int) ((rL - U) * 10);
            PORTC=array[U];                                                                // Prints upper to seven segment display out of number like 6 out of 60 
            PORTD=array[L]; 
            DO_DISPLAY_7SEG_Upper (U);
            DO_DISPLAY_7SEG_Lower (L);
            DP = 0;
        }
        else
        {
            resist = (int) rL;
            char U = (int) resist/10;
            char L = (int) resist % 10;
            PORTC=array[U];                                                                // Prints upper to seven segment display out of number like 6 out of 60 
            PORTD=array[L]; 
            DO_DISPLAY_7SEG_Upper (U);
            DO_DISPLAY_7SEG_Lower (L);
            DP = 1;
        }
        
                                         // Display decimal value on lower 7seg
        
        printf("Resistance: %f kOhm\r\n", rL);

        WAIT_1_SEC();
    }
}