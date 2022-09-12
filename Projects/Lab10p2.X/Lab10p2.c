#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE
#define _XTAL_FREQ  8000000 

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "ST7735.h"
#include "I2C.c"
#include "I2C_Support.c"
#include "Interrupt.c"
#include "ST7735_TFT.c"


char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
                                                                                // hex array for button values

char txt1[21][4] = {"CH-\0", "CH \0", "CH+\0", "PRV\0", "NXT\0", "PLY\0", " - \0", " + \0", " EQ\0", " 0 \0", "100\0", "200\0", " 1 \0", " 2 \0", " 3 \0", " 4 \0", " 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"};
                                                                                // text array based off of remote
                                                                                // color ID based of of remote


char Device;                                                                    //Select the device
char Cmd;                                                                       //Select the command
char Address;                                                                   //Select the address
signed char tempC;                                                                     //Stores the temperature in degree c
signed char tempF;
unsigned char second;                                                           //Stores the second
unsigned char minute;                                                           //Stores the minute
unsigned char hour;                                                             //Stores the hour
unsigned char dow;                                                              //Stores the Day of the Week
unsigned char day;                                                              //Stores the day
unsigned char month;                                                            //Stores the month
unsigned char year;                                                             //Stores the year
unsigned char prev_second;                                                      //Stores the previous second

char tempSecond = 0xff;                                                         // Set tempSecond = 11111111
char second = 0x00;                                                             // Set variables to 0
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

char TempSecond;

char buffer[31];                                                                // general buffer for display purpose
char *nbr;                                                                      // general pointer used for buffer
char *txt;
char Display_D1[10] = {0, 0x02, 0x04, 0x06, 0x10, 0x12, 0x14, 0x16, 0x16, 0x16};// RGB array 
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
char found;
unsigned char Nec_state = 0;
unsigned int Time_Elapsed;
unsigned char i,bit_count;


void putch(char c);                                                             // signatures
void init_UART();
void Wait_Half_Second_With_Beep();
void Delay_Half_Sec();
void Wait_Half_Second();
void Wait_One_Sec();
void Do_Init();
void DS3231_Setup_Time();
void interrupt high_priority chkisr();
void TIMER1_isr(void);
void force_nec_state0();
void INT0_isr(void);
void Activate_Buzzer();
void Deactivate_Buzzer();
void Initialize_Screen();
void Do_LED(char digit);

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Do_Init()                                                                  // Initialize the ports 
{ 
                                                                                // initialize the DS1621 chip
    init_UART();                                                                // Initialize the uart
    OSCCON=0x70;                                                                // Set oscillator to 8 MHz 
    
    ADCON1=0x0F;                                                                // Make all input as digital
    TRISA = 0x00;                                                               // Set PORTA as output
    TRISB = 0x01;                                                               // Set PORTB as all output except bit 0
    TRISC = 0x00;                                                               // Set PORTC as output
    TRISD = 0x00;                                                               // Set PORTD as output
    TRISE = 0x00;                                                               // Set PORTE as output                                                   // Initialize screen in setup
    RBPU=0;
    DS1621_Init();  
    init_INTERRUPT();    
    I2C_Init(100000);                                                             
}

void main() 
{ 
    Do_Init();                                                                  // Set up everyting so call Do_init
    DS3231_Read_Time();
    while (1)
    {
       DS3231_Read_Time();
       if(tempSecond != second)
       {
           tempSecond = second;                                                 // Set tempSecond = second;                                
           signed char tempC = DS1621_Read_Temp();                              // Read the temperature in C and store as char  
           signed char tempF = (tempC * 9 / 5) + 32;                            // Change Celsius to Faren.
           printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
           printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
       }
    }
    
}
