#include <LPC21XX.h>
#include "types.h"
#include "delay.h"
#include "ultra_sonic.h"


/* ---------- ULTRASONIC SENSOR PINS ---------- */

/* Trigger pin connected to P0.21 */
#define trig_pin 21

/* Echo pin connected to P0.22 */
#define echo_pin 22


/* ---------- ULTRASONIC INITIALIZATION ---------- */

/* Configure trigger as output and echo as input */
void init_sonic()
{
    /* Configure P0.21 as output */
    IODIR0 |= 1 << trig_pin;

    /* Configure P0.22 as input */
    IODIR0 &= ~(1 << echo_pin);
}


/* ---------- TRIGGER PULSE ---------- */

/* Generate 10 us trigger pulse for ultrasonic sensor */
void send_pulse()
{
    /* Keep trigger LOW */
    IOCLR0 = (1 << trig_pin);

    delay_us(2);


    /* Generate HIGH trigger pulse */
    IOSET0 = (1 << trig_pin);

    delay_us(10);


    /* End trigger pulse */
    IOCLR0 = (1 << trig_pin);
}


/* ---------- ECHO PULSE MEASUREMENT ---------- */

/* Measure the duration of the ultrasonic echo pulse */
u32 read_pulse()
{
    u32 distance;
    unsigned long timeout;


    /* Wait for Echo to become HIGH */
    timeout = 100000;

    while(((IOPIN0 >> echo_pin) & 1) == 0)
    {
        /* Prevent infinite waiting if no echo is received */
        if(--timeout == 0)
            return 0;
    }


    /* Reset Timer0 */
    T0TCR = 0x02;

    /* Start Timer0 */
    T0TCR = 0x01;


    /* Wait until Echo becomes LOW */
    timeout = 1000000;

    while(((IOPIN0 >> echo_pin) & 1) == 1)
    {
        /* Exit if echo pulse takes too long */
        if(--timeout == 0)
            break;
    }


    /* Stop Timer0 */
    T0TCR = 0x00;


    /* Read measured pulse duration */
    distance = T0TC;

    return distance;
}


/* ---------- DISTANCE CALCULATION ---------- */

/* Convert echo pulse duration into distance in cm */
u32 dis_cal()
{
    u32 pulse;
    f32 dis;


    /* Generate ultrasonic trigger pulse */
    send_pulse();


    /* Measure echo pulse duration */
    pulse = read_pulse();


    /*
     * Convert pulse duration to distance.
     *
     * The factor 59 is used for the configured
     * Timer0 timing and ultrasonic propagation.
     */
    dis = pulse / 59.0;


    /* Return calculated distance in cm */
    return dis;
}