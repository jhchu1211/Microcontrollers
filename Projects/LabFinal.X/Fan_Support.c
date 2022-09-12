#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

#define BLUE_D2 PORTDbits.RD7
#define RED_D2 PORTDbits.RD6

extern char HEATER;
extern char duty_cycle;
extern char heater_set_temp;
extern signed int DS1621_tempF;
extern char FAN;

int get_duty_cycle(signed int temp, int set_temp)
{
    if (temp > set_temp)
        return 0;
    else
    {
        int dc = 2*(set_temp - temp);
        if (dc > 100)
            return 100;
        else 
            return dc;
    }
        
// add code to check if temp is greater than set_temp. If so, dc = 0. Else dc = 2 times of difference of set_temp and temp
// check if dc is greater than 100. If so, set it to 100
// return dc

}

void Monitor_Heater()
{
    duty_cycle = get_duty_cycle(DS1621_tempF, heater_set_temp);
    do_update_pwm(duty_cycle);
    if (HEATER == 1) 
    {
        FAN_EN = 1;
        FAN_LED = 1;
    }
    else 
    {
     FAN_LED = 0;   FAN_EN = 0;
    }
}

void Toggle_Heater()
{
    HEATER = (!HEATER);
    //we initialize it to zero, so we have to negate that, and going forward whenever toggled it 
    //will switch back and forth between states when pressed by the button
}

int get_RPM()
{
// add old code
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
    // then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS
}

void Turn_Off_Fan()
{
// add old code   
    FAN = 0;
    FAN_EN = 0;
    FAN_LED = 0;

}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_LED = 1;
// add old code 
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