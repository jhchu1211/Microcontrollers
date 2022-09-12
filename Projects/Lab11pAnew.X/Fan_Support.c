#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

#define RED_D1 PORTAbits.RA1                                                    // Defining port A1
#define GREEN_D1 PORTAbits.RA2                                                  // Defining port A2
#define BLUE_D1 PORTAbits.RA4
//#define FAN_EN PORTAbits.RA5
#define RED_D2 PORTDbits.RD6
#define BLUE_D2 PORTDbits.RD7
//#define FAN_LED PORTEbits.RE2

extern char FAN;
extern char duty_cycle;


int get_RPM()
{
int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
// then RPS = count /2
TMR3L = 0; // clear out the count
return (RPS * 60); // return RPM = 60 * RPS

}

void Toggle_Fan()
{
if (FAN==0) Turn_On_Fan();
else Turn_Off_Fan();
 
}

void Turn_Off_Fan()
{

FAN=0;
FAN_EN=0;
FAN_LED=0;

}

void Turn_On_Fan()
{

FAN=1;
do_update_pwm(duty_cycle);
FAN_EN=1;
FAN_LED=1;
 
}

void Increase_Speed()
{
if (duty_cycle==100)
{
Do_Beep();
Do_Beep();
do_update_pwm(duty_cycle);
}
else
{
duty_cycle = duty_cycle + 5;
do_update_pwm(duty_cycle);
}
}

void Decrease_Speed()
{
 if (duty_cycle==0)
{
Do_Beep();
Do_Beep();
do_update_pwm(duty_cycle);
}
else
{
duty_cycle = duty_cycle - 5;
do_update_pwm(duty_cycle);

}
}

void do_update_pwm(char duty_cycle) 
{ 
float dc_f;
int dc_I;
PR2 = 0b00000100 ; // set the frequency for 25 Khz
T2CON = 0b00000111 ; //
dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz
// signal
dc_I = (int) dc_f; // get the integer part
if (dc_I > duty_cycle) dc_I++; // round up function
CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
CCPR1L = (dc_I) >> 2;

}

void Set_DC_RGB(int duty_cycle)
{
if (duty_cycle>=0 & duty_cycle<9)
{
 RED_D1=0;
 GREEN_D1=0;
 BLUE_D1=0;
}
else if (duty_cycle>=10 & duty_cycle<19)
{
 RED_D1=1;
 GREEN_D1=0;
 BLUE_D1=0;
}
else if (duty_cycle>=20 & duty_cycle<29)
{
 RED_D1=0;
 GREEN_D1=1;
 BLUE_D1=0;
}
 else if (duty_cycle>=30 & duty_cycle<39)
{
 RED_D1=1;
 GREEN_D1=1;
 BLUE_D1=0;
} 
else if (duty_cycle>=40 & duty_cycle<49)
{
 RED_D1=0;
 GREEN_D1=0;
 BLUE_D1=1;
}
else if (duty_cycle>=50 & duty_cycle<59)
{
 RED_D1=1;
 GREEN_D1=0;
 BLUE_D1=1;
}
else if (duty_cycle>=60 & duty_cycle<69)
{
 RED_D1=0;
 GREEN_D1=1;
 BLUE_D1=1;
}
else if (duty_cycle>=70)
{
 RED_D1=1;
 GREEN_D1=1;
 BLUE_D1=1;
}
    
}

void Set_RPM_RGB(int rpm)
{
if (rpm==0)
{
 RED_D2=0;
 BLUE_D2=0;
}
else if (rpm>0 & rpm<1800)
{
 RED_D2=1;
 BLUE_D2=0;
}
else if (rpm>=1800 & rpm<2700)
{
 RED_D2=1;
 BLUE_D2=1;
}
 else if (rpm>=2700)
{
 RED_D2=0;
 BLUE_D2=1;
}  
}
