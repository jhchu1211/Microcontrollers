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


#define Circle_Size     20                                                      // Size of Circle for Light
#define Circle_X        60                                                      // Location of Circle
#define Circle_Y        80                                                      // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1                                                       // Size of Normal Text
#define TS_2            2                                                       // Size of Big Text

// colors
#define RD               ST7735_RED
#define CY               ST7735_CYAN
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define kp               PORTEbits.RE1                                          // definition Key Pressed LED      

#define Circle_Size     20                                                      // Size of Circle for Light
#define Circle_X        60                                                      // Location of Circle
#define Circle_Y        80                                                      // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1                                                       // Size of Normal Text
#define TS_2            2                                                       // Size of Big Text

#define RD               ST7735_RED
#define CY               ST7735_CYAN
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define D1R 0x02                                                                // Customized defines based off of schematic
#define D1G 0x04
#define D1B 0x10
#define D1CY 0x14
#define D1W 0x16

#define D2R 0x01
#define D2G 0x02
#define D2B 0x04
#define D2CY 0x06
#define D2W 0x07

#define D3R 0x04
#define D3G 0x10
#define D3B 0x20
#define D3M 0x24
#define D3W 0x34

#define RED_D1 PORTAbits.RA1
#define GREEN_D1 PORTAbits.RA2
#define BLUE_D1 PORTAbits.RA4

char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};
                                                                                // hex array for button values

char txt1[21][4] = {"CH-\0", "CH \0", "CH+\0", "PRV\0", "NXT\0", "PLY\0", " - \0", " + \0", " EQ\0", " 0 \0", "100\0", "200\0", " 1 \0", " 2 \0", " 3 \0", " 4 \0", " 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"};
                                                                                // text array based off of remote

int color[21]={RD, RD, RD, CY, CY, GR, BU, BU, MA, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK};
                                                                                // color ID based of of remote


char Device;                                                                    //Select the device
char Cmd;                                                                       //Select the command
char Address;                                                                   //Select the address
signed char tempc;                                                                     //Stores the temperature in degree c
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

void Wait_Half_Second_With_Beep()
{
    Activate_Buzzer();                                                          // Activate the buzzer
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    //Wait_One_Sec();                                                           // Call Delay_half_sec
    Deactivate_Buzzer();                                                        // Deactivate the buzzer
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

void Wait_One_Sec()
{
    Wait_Half_Second();                                                         //Wait half second
    kp = 0;                                                                     //Turn off kp
    Wait_Half_Second();                                                         //Wait half second
    kp = 1;                                                                     //Turn on kp
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
    TRISE = 0x00;                                                               // Set PORTE as output
    Initialize_Screen();                                                        // Initialize screen in setup
    RBPU=0;
    DS1621_Init();  
    init_INTERRUPT();    
    I2C_Init(100000);                                                             
}

void main() 
{ 
    Do_Init();                                                                  // Set up everyting so call Do_init
    // Initialization  
    // PART ONE
    /*
    while (1)
    {
        char tempC = DS1621_Read_Temp();                                        // Read the temperature in C and store as char
        char tempF = (tempC * 9 / 5) + 32;                                      //
        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
        Wait_One_Sec();                                                         // wait every second
    }
    */
    // Part TWO  
    
    DS3231_Setup_Time();                                                        // Setup DS3231
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
           signed char U = (int)(tempF/10);                                     // make U a signed char 
           
           Do_LED(U);                                                           // Call Do_LED with U 
       }
    
        if (nec_ok == 1)
       {
            nec_ok = 0;                                                         // set nec_ok = 0

            Nec_code1 = (char) ((Nec_code >> 8));                               // Right shift nec_code by 8 bits and instantiate into nec_code1
                              
            INTCONbits.INT0IE = 1;                                              // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;                                            // Edge programming for INT0 falling edge
            
            char found = 0xff;                                                  // set char found to 0xff
            for (int i=0; i<21; i++)                                            // cycle through 21
            {
                if(Nec_code1 == array1[i])                                      // if nec_code1 == i index in array
                    {
                        found = i;                                              // set found = i
                        i = 21;                                                 
                    }
            }
                 
            printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);            // print statements
            printf ("%d \r\n", found);
            
            //if (found = 0x90)                                                 // if EQ button is found
                //{
                //    DS3231_Setup_Time();                                        // call DS3231_Setup_Time
                //}
            if (found != 0xff)                                                  // if found is not equal to 0xff
            {   
                 
                kp = 1;                                                         // set kp on
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]);      // create visual on LCD
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
                Wait_Half_Second_With_Beep();                                   // Wait_half_sec
                kp = 0;                                                         // Turn off kp
            }
            if (array1[found] == 0x90)                                          // if EQ buttong is called
            {
                DS3231_Setup_Time();                                            // Time should reset 
            }
            
       }
    }
    
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

void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 21-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 10 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void Do_LED(char digit)                                                         // Function to output LED based off of Temp
{
    PORTA = Display_D1[digit];                                                  // Set PORTA to appropriate D1 in array 
}

