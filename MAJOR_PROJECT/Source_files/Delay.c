#include"types.h"

void delay_us(unsigned int delay_Us)
{
    volatile unsigned int d = delay_Us*12;
    while(d--);
}

void delay_ms(unsigned int delay_Ms)
{
    volatile unsigned int d = delay_Ms*12000;
    while(d--);
}

void delay_s(unsigned int delay_S)
{
    volatile unsigned int d = delay_S*12000000;
    while(d--);
}