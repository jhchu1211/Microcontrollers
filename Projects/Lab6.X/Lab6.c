#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>


#pragma config OSC = INTIO67
#pragma config BOREN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = PORTBE


#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2
#define NSLT_RED PORTAbits.RA3
#define NSLT_GREEN PORTAbits.RA4
#define NSPED_SW PORTAbits.RA5 // Location of Switch for North/South PED
#define EWPED_SW PORTBbits.RB0 // Location of Switch for East/West PED
#define NSLT_SW PORTBbits.RB1 // Location of Switch for North/South Left Turn
#define EWLT_SW PORTBbits.RB2 // Location of Switch for East/West Left Turn
#define SPKR PORTBbits.RB3 // Location of Speaker
#define EW_RED PORTBbits.RB4
#define EW_GREEN PORTBbits.RB5
#define MODE_LED PORTBbits.RB6
#define SEC_LED PORTDbits.RD7 // Location of Second LED
#define SEG_G PORTEbits.RE0 // upper Digi g ; was EWLT_RED
#define SEG_A PORTEbits.RE1 // upper Digi a; was SEG_G
#define EWLT_RED PORTEbits.RE2 // D2 R; was EWLT_GREEN
#define EWLT_GREEN PORTBbits.RB7 // D2 G; was EWLT_GREEN
#define OFF 0 // Number definition of Off Color
#define RED 1 // Number definition of Red Color
#define GREEN 2 // Number definition of Green Color
#define YELLOW 3 // Number definition of Yellow Color


void putch (char c);
void Wait_One_Second(); // one second delay function protype
void Wait_One_Second_With_Beep(); // on second beebing function prototype
void Wait_N_Seconds(char Seconds); // n sec delay function prototype
void Wait_Half_Second();
void PED_Control(char Direction, char Num_Sec);
void Activate_Buzzer();
void Deactivate_Buzzer();
unsigned int get_full_ADC(void); // get input analogy voltage function prototype
void init_ADC(void); // set ADCON
void init_UART();
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EW_LT(char color);
void Set_NS_LT(char color);
void Day_Mode();
void Night_Mode();
void Do_Display_7Seg_Upper(char Digit);
void Do_Display_7Seg_Lower(char Digit);


char dir;
char Count; // RAM variable for Second Count
char PED_Count; // RAM variable for Second Pedestrian Count
char SW_NSPED; // RAM variable for NS Pedestrian Switch
char SW_NSLT; // RAM variable for NS Left Turn Switch
char SW_EWPED; // RAM variable for EW Pedestrian Switch
char SW_EWLT; // RAM variable for EW Left Turn Switch
char SW_MODE; // RAM variable for Mode Light Sensor
int array[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

void init_UART()
{
OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE
& USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
OSCCON = 0x60;
}
unsigned int GET_FULL_ADC(void)                                                 //Function for conversion
{
int result;                                                                     //Declaring variables
ADCON0bits.GO = 1;                                                              //Start Conversion
while(ADCON0bits.DONE == 1);                                                    //Wait for conversion to be completed (DONE=0)
result = (ADRESH * 0x100 + ADRESL);                                             //Combine result of upper byte and lower byte into

return result;                                                                  //return the most significant 8- bits of the result.
}

void Wait_Half_Second()
{
 T0CON = 0x02; // Timer 0, 16-bit mode,  1:8
 TMR0L = 0xDB; // set the lower byte of TMR
 TMR0H = 0x0B; // set the upper byte of TMR
 INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
 T0CONbits.TMR0ON = 1; // Turn on the Timer 0
 while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
 T0CONbits.TMR0ON = 0; // turn off the Timer 0
}

void Activate_Buzzer()                                                          //Function to Activate Buzzer
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;
} 

void Deactivate_Buzzer()                                                        //Function to deactivate buzzer
{
 CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
} 
void init_ADC()
{
ADCON0 = 0x01; // Select channel 1
ADCON1 = 0x0E; // Set AN0-3 as A/D converter
ADCON2 = 0xA9; // Right justified, 8TAD, Fosc/32
}
void main()
{
float Volt;
int MODE;
int nStep;
init_UART(); // initialize UART (for debugging purpose)
init_ADC(); // initialize ADC port
OSCCON =0x60; // set the processor to run 

TRISA = 0x21; // Set port a0 and a5 input rest output 
TRISB = 0x07; // Set port b RB0, RB1, RB2 as input rest as output
TRISC = 0x00; // Set port c RC4 as input rest as output
TRISD = 0x00; // Set port d as all outputs
TRISE = 0x00; // Set port e as all outputs
PORTD = 0x00;

while(1)
{ // doing the loop for day mode and night mode
//ADCON0 = 0x01;
    Do_Display_7Seg_Upper('o');
    Do_Display_7Seg_Lower('o');
    nStep = GET_FULL_ADC();  // Set voltage equal to returned get_full_ ADC value

    Volt = (nStep*4) / 1000; // Set volt mV to V 
    if (Volt < 3) // less sensitive for light
    {
        SW_MODE = 1;
    }
    else
    {
        SW_MODE = 0;
    }
    if(SW_MODE == 1)
    {
        MODE_LED =1;
        Day_Mode(); // If SW_MODE is equal to 0 then run day mode
    }
    else
    {
        MODE_LED =0;
        Night_Mode(); // If SW_MODE is not equal to 0 then run night mode
    }
}
}
void putch (char c)
{
while (!TRMT);
TXREG = c;
}


void Set_NSLT(char color)
{
 switch (color)
 {
 case OFF: NSLT_RED =0;NSLT_GREEN=0;break; // Turns off the NS LED
 case RED: NSLT_RED =1;NSLT_GREEN=0;break; // Sets NS LED RED
 case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NS LED GREEN
 case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break; // sets NS LED YELLOW
 }
}
void Set_EW(char color)
{
 switch (color)
 {
 case OFF: EW_RED =0;EW_GREEN=0;break; // Turns off the NS LED
 case RED: EW_RED =1;EW_GREEN=0;break; // Sets NS LED RED
 case GREEN: EW_RED =0;EW_GREEN=1;break; // sets NS LED GREEN
 case YELLOW: EW_RED =1;EW_GREEN=1;break; // sets NS LED YELLOW
 }
}
void Set_EWLT(char color)
{
 switch (color)
 {
 case OFF: EWLT_RED =0;EWLT_GREEN=0;break; // Turns off the NS LED
 case RED: EWLT_RED =1;EWLT_GREEN=0;break; // Sets NS LED RED
 case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets NS LED GREEN
 case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break; // sets NS LED YELLOW
 }
}

void Set_NS(char color)
{
 switch (color)
 {
 case OFF: NS_RED =0;NS_GREEN=0;break; // Turns off the NS LED
 case RED: NS_RED =1;NS_GREEN=0;break; // Sets NS LED RED
 case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN
 case YELLOW: NS_RED =1;NS_GREEN=1;break; // sets NS LED YELLOW
 }
}
void Day_Mode()
{
    while(1)
    {
        Set_NS(GREEN);
        Set_EW(RED);
        Set_EWLT(RED);
        Set_NSLT(RED); // step1
        if(NSPED_SW==1)
        {
            PED_Control(0,7); // step1a
        }
        Wait_N_Seconds(9); // step2
        Set_NS(YELLOW); 
        Wait_N_Seconds(3); // step3
        Set_NS(RED); // step4
        if(EWLT_SW==1) // step5b
        {
            Set_EWLT(GREEN); 
            Wait_N_Seconds(7);// step6
            Set_EWLT(YELLOW);
            Wait_N_Seconds(3); // step7
            Set_EWLT(RED); // step8
        }
        Set_EW(GREEN); // step9
        if (EWPED_SW==1) 
        {
            PED_Control(1,9); // step9a
        }
        Wait_N_Seconds(8); // step10
        Set_EW(YELLOW);
        Wait_N_Seconds(3); // step11
        Set_EW(RED); // step12
        if(NSLT_SW==1) // step13b
        {
            Set_NSLT(GREEN);
            Wait_N_Seconds(7); // step14
            Set_NSLT(YELLOW);
            Wait_N_Seconds(3); // step15
            Set_NSLT(RED); // step16
        }
        break; // step17
    }
}
void Night_Mode()
{
    
    while(1)
    {
        Set_NS(GREEN);
        Set_EW(RED);
        Set_EWLT(RED);
        Set_NSLT(RED); // step1
        Wait_N_Seconds(9); // step2
        Set_NS(YELLOW); 
        Wait_N_Seconds(3); // step3
        Set_NS(RED); // step4
        if (EWLT_SW==1) // step5b
        {
            Set_EWLT(GREEN);
            Wait_N_Seconds(7); // step6
            Set_EWLT(YELLOW);
            Wait_N_Seconds(3); // step7
            Set_EWLT(RED); // step8
        }
        Set_EW(GREEN); 
        Wait_N_Seconds(9); // step9
        Set_EW(YELLOW);
        Wait_N_Seconds(3); // step10
        Set_EW(RED); // step11
        if(NSLT_SW==1) // step12b
        {
            Set_NSLT(GREEN);
            Wait_N_Seconds(7); // step13
            Set_NSLT(YELLOW);
            Wait_N_Seconds(3); // step14
            Set_NSLT(RED); // step15
        }
        break; // step16
    }
}
void Do_Display_7Seg_Upper(char digit)
{
    if (digit=='o')
            PORTC = PORTC | 0x7F; 
    else
    {
    PORTC = array[digit] & 0x3F;                                //Handling the tens place
    char PC = array[digit] & 0x01;                              //Code in this section to handle busted c0 pin
    char PE = array[digit] & 0x40;                              //Code in this section to handle busted e0 pin
    if (PE == 0 && PC ==0) 
    {
        SEG_G =0;
        SEG_A =0;
    }
    else if (PC == 0 && PE != 0)
    {
        SEG_G =1;
        SEG_A =0;       
    }//makes it so pin e0 is one
    else if (PC == 1 && PE == 0)
    {
        SEG_G =0;
        SEG_A =1;   
    }//makes pin e1 or c0 one
    else if (PC == 1 && PE != 0)
    {
        SEG_G =1;
        SEG_A =1;  
    }
    }
}

void Do_Display_7Seg_Lower(char Digit)
{
    if (Digit=='o')
        PORTD = (PORTD & 0x80) |0x7F; 
    else
        PORTD = (PORTD & 0x80) | array[Digit];
}


void Wait_N_Seconds(char seconds) // counter
{
char I;
for (I=0;I<seconds;I++) // loop for the counter
{
Wait_One_Second(); // call wait one sec function

}
}
void Wait_One_Second()
{
 SEC_LED = 0; // First, turn off the SEC LED
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 1; // then turn on the SEC LED
 Wait_Half_Second(); // Wait for half second (or 500 msec)
}
void PED_Control( char Direction, char Num_Sec) 
{
    Do_Display_7Seg_Lower('o');
    Do_Display_7Seg_Upper('o');
    for( char i = Num_Sec;i>0;i--)
    {
        if (Direction == 0)
        {
            Do_Display_7Seg_Upper(i);
            
        }
        else if (Direction == 1)
        {
            Do_Display_7Seg_Lower(i);
            
        }
        Wait_One_Second_With_Beep();
    }
    Do_Display_7Seg_Upper('o');
    Do_Display_7Seg_Lower('o');
    Wait_One_Second_With_Beep();
}

 
void Wait_One_Second_With_Beep()
{
 SEC_LED = 1; // First, turn on the SEC LED
 Activate_Buzzer(); // Activate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
 SEC_LED = 0; // then turn off the SEC LED
 Deactivate_Buzzer (); // Deactivate the buzzer
 Wait_Half_Second(); // Wait for half second (or 500 msec)
}