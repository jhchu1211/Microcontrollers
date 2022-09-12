#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>


#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE


#define PORTD_bit0 PORTDbits.RD0 // bit 1.1
#define PORTD_bit1 PORTDbits.RD1 // bit 1.2
#define PORTD_bit2 PORTDbits.RD2 // bit 1.3
#define PORTD_bit3 PORTDbits.RD3 // bit 2.1
#define PORTD_bit4 PORTDbits.RD4 // bit 2.2
#define PORTD_bit5 PORTDbits.RD5 // bit 2.3

int add(unsigned int aa, unsigned int ab, unsigned int ac, unsigned int ba, unsigned int bb, unsigned int bc)
{
int result = 0;
unsigned int carry1 = aa & ba;
	unsigned int result1 = aa ^ ba;
unsigned int carry2 = ab & bb;
	unsigned int result2 = ab ^ bb;
unsigned int carry3 = ac & bc;
	unsigned int result3 = ac ^ bc;
	if(result1 == 1)
	{
		result = result + 1;
	}
	if(carry1 ==1)
	{
		result = result +2;
	}
	if(result2 == 1)
	{
		result = result + 2;
	}
	if(carry2 ==1)
	{
		result = result +4;
	}
	if(result3 == 1)
	{
		result = result + 4;
	}
	if(carry3 ==1)
	{
		result = result +8;
	}
	if(result>9)
	{
		result = 0;
	}
	return result;
}


void main()
{
	TRISD = 0x3F;
	TRISB = 0;
	unsigned char array[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
	int sum;
//Lookup_table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D 0x07, 0x7F, 0x4F}

while(1)
{
	sum = add(PORTD_bit0, PORTD_bit1, PORTD_bit2, PORTD_bit3, PORTD_bit4, PORTD_bit5);
	PORTB = array[sum];
}
}
