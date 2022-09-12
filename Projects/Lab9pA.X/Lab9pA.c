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

#include "ST7735.h" 
#include "ST7735_TFT.c"
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz

#define key PORTEbits.RE1               // LED port

void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();

unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define D1R              0x02 // hex for red on d1
#define D1B              0x10 // hex for blue on d1
#define D1G              0x04 // hex for green on d1
#define D1M              0x12 // hex for magenta/pink on d1
#define D1W              0x16 // hex for white on d1

//for d2, we move c0 for c4 because of bad pin c0. we keep everything else as is
#define D2R              0x10 // hex for red on d2
#define D2B              0x04 // hex for blue on d2
#define D2G              0x02 // hex for green on d2
#define D2M              0x14 // hex for magenta/pink on d2
#define D2W              0x16 // hex for white on d2

#define D3R              0x04 // hex for red on d3
#define D3B              0x20 // hex for blue on d3
#define D3G              0x10 // hex for green on d3
#define D3M              0x24 // hex for magenta/pink on d3
#define D3W              0x34 // hex for white on d3

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char array1[21]={0xa2, 0x62,0xe2, 0x22,0x2, 0xc2,0xe0,0xa8, 0x90, 
0x68, 0x98, 0xb0,0x30, 0x18,0x7a,0x10, 0x38,0x5a, 0x42,0x4a,0x52 };
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0", "|<<\0" , ">>|\0", ">||\0"," - \0" , " + \0", "EQ \0",
" 0 \0", "100\0", "200\0", " 1 \0", " 2 \0", " 3 \0", " 4 \0", " 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"};
int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA, 
BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK};
char d1[21] = {D1R,0,0,D1B,0,0,D1M,0,0,D1W,0,0,D1W,0,0,D1W,0,0,D1W,0,0};
char d2[21] = {0,D2R,0,0,D2B,0,0,D2M,0,0,D2W,0,0,D2W,0,0,D2W,0,0,D2W,0};
char d3[21] = {0,0,D3R,0,0,D3G,0,0,D3M,0,0,D3W,0,0,D3W,0,0,D3W,0,0,D3W};
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
    OSCCON = 0x60;
}

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCONbits.INT0IF == 1) INT0_isr();
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void Wait_Half_Second() //taken from previous labs
{
 T0CON = 0x02; // Timer 0, 16-bit mode,  1:8
 TMR0L = 0xDB; // set the lower byte of TMR
 TMR0H = 0x0B; // set the upper byte of TMR
 INTCONbits.TMR0IF = 0; // clear the Timer 0 flag
 T0CONbits.TMR0ON = 1; // Turn on the Timer 0
 while (INTCONbits.TMR0IF == 0); // wait for the Timer Flag to be 1 for done
 T0CONbits.TMR0ON = 0; // turn off the Timer 0
}

void Activate_Buzzer() //taken from previous labs
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ; 
}

void Deactivate_Buzzer() //taken from previous labs
{
 CCP2CON = 0x0;
 PORTBbits.RB3 = 0; 
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void INT0_isr(void)
{
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            // Clear Timer 1
            TMR1H = 0;                      // Reset Timer1
            TMR1L = 0;                      //
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High            
            return;
        }
        
        case 1 :
        {
            if((Time_Elapsed > 8500) && (Time_Elapsed < 9500) )
            {
                Nec_state = 2;                  // Set Nec_State to state 1
                INTCON2bits.INTEDG0 = 0;
            }
            return;
            
        }
        
        case 2 :                            // Add your code here
        {
                if((Time_Elapsed > 4000) && (Time_Elapsed < 5000) )
            {
                Nec_state = 3;                  // Set Nec_State to state 1
                INTCON2bits.INTEDG0 = 1;
            }
            return;
        }
        
        case 3 :                            // Add your code here
        {
                if((Time_Elapsed > 400) && (Time_Elapsed < 700) )
            {
                Nec_state = 4;                  // Set Nec_State to state 1
                INTCON2bits.INTEDG0 = 0;
            }
                 return;
        }
        
        case 4 :                            // Add your code here
        {
                if((Time_Elapsed > 400) && (Time_Elapsed < 1800) )
            {
                Nec_code = Nec_code << 1;                  // Left shift Nec code by 1
                if(Time_Elapsed > 1000)
                {
                    Nec_code = Nec_code + 1;
                }
                bit_count = bit_count +1;                  //Add bit_count by 1
                if(bit_count > 31)
                {
                    nec_ok = 1;
                    INTCONbits.INT0IE = 0;
                    Nec_state = 0;
                }
                Nec_state = 3;
            }
            else
            {
               
             force_nec_state0();
            }
                INTCON2bits.INTEDG0 = 1;
                return;
        }
                
                
    }
}


    

void main()
{
    init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0; 
    TRISA = 0x00;// Enable PORTB internal pull up resistor
    TRISB = 0x01;
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;
    ADCON1 = 0x0F;                          //
    Initialize_Screen();
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                  // Enable external interrupt
    TMR1H = 0;                              // Reset Timer1
    TMR1L = 0;                              //
    PIR1bits.TMR1IF = 0;                    // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                    // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                    // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                     // Enable global interrupts
    nec_ok = 0;                             // Clear flag
    Nec_code = 0x0;                         // Clear code
    
    while(1)
    {
        if (nec_ok == 1) // flag trigger
        {

            nec_ok = 0; // clear flag again

            Nec_code1 = (char) ((Nec_code >> 8)); 
            printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            
            char found = 0xff;
            
            for (int in = 0; in<21;in++)
            {
                if (Nec_code1==array1[in])
                {
                    found = in;
                }
            }
            printf ("NEC_Code = %08lx %x", Nec_code, Nec_code1);
            printf ("%d \r\n", found);
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
                key = 1; // LED flash on
                PORTA = d1[found]; // output for D1
                PORTB = d3[found]; // output for D3
                PORTC = d2[found] & 0x16; // mask for PORTC and output for D2
                Activate_Buzzer(); // buzzer on
                Wait_Half_Second(); // wait a half second
                key = 0; //LED flash off
                Deactivate_Buzzer(); // buzzer off
            }
            
        }
    }
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

    strcpy(txt, "LAB 9 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}
