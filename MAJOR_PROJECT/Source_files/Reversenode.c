#include <lpc21xx.h>
#include "types.h"
#include "can.h"
#include "lcd.h"
#include "ultra_sonic.h"
#include "delay.h"


/* ---------- GLOBAL VARIABLES ---------- */

/* CAN transmit and receive frames */
canf txf, rxf;

/* Measured ultrasonic distance */
u32 dis;


/* ---------- CAN DATA STATUS ---------- */

/* Check whether CAN receive buffer contains data */
u8 can_data_available(void)
{
    return ((C1GSR >> 0) & 1);
}


/* ---------- BUZZER FUNCTION ---------- */

/* Blink buzzer for the specified number of times */
void blink_buzzer(u8 time)
{
    u8 i;

    /* Configure buzzer pin P0.23 as output */
    IODIR0 |= 1 << 23;

    /* Initially turn buzzer OFF */
    IOCLR0 = 1 << 23;


    /* Generate buzzer ON/OFF pulses */
    for(i = 0; i < time; i++)
    {
        /* Buzzer ON */
        IOSET0 = 1 << 23;

        delay_ms(200);

        /* Buzzer OFF */
        IOCLR0 = 1 << 23;

        delay_ms(200);
    }
}


/* =========================================================
 *                         MAIN
 * ========================================================= */

int main()
{
    int i, j;


    /* ---------- INITIALIZATION ---------- */

    /* Initialize ultrasonic sensor */
    init_sonic();

    /* Initialize CAN1 */
    init_can1();

    /* Initialize LCD */
    Init_lcd();


    /* Configure P0.0-P0.7 as indicator LED outputs */
    IODIR0 |= 255 << 0;


    /* ---------- TIMER0 INITIALIZATION ---------- */

    /* Configure Timer0 prescaler */
    T0PR = 14;

    /* Start Timer0 */
    T0TCR = 0x01;


    /* ---------- STARTUP MESSAGE ---------- */

    str_lcd("ULTRSONIC");

    delay_ms(500);


    /* =====================================================
     *                     MAIN LOOP
     * ===================================================== */

    while(1)
    {
        /* ---------- DISTANCE MEASUREMENT ---------- */

        /* Measure distance using ultrasonic sensor */
        dis = dis_cal();


        /* Display distance on LCD */
        cmd_lcd(0xc0);

        u32_lcd(dis);

        str_lcd("cm   ");


        /* ---------- CAN TRANSMISSION ---------- */

        /* CAN ID 2 is used for ultrasonic distance */
        txf.id = 2;

        /* CAN data frame */
        txf.bfv.dlc = 4;

        txf.bfv.rtr = 0;

        /* Store distance in CAN data */
        txf.data1 = dis;

        txf.data2 = 0;


        /* Transmit distance to main node */
        can1_tx1(txf);


        /* ---------- CAN RECEPTION ---------- */

        if(can_data_available())
        {
            /* Receive CAN frame */
            can1_rx1(&rxf);


            /* Turn OFF all indicator LEDs */
            IOSET0 = 255 << 0;


            /* Move LCD cursor to third line */
            cmd_lcd(0x94);


            /* ---------- LEFT INDICATOR ---------- */

            if(rxf.id == 3)
            {
                str_lcd("left ");


                /* Run left indicator pattern */
                for(j = 0; j < 3; j++)
                {
                    for(i = 0; i <= 7; i++)
                    {
                        IOCLR0 = 1 << i;

                        delay_ms(100);
                    }
                }
            }


            /* ---------- RIGHT INDICATOR ---------- */

            else if(rxf.id == 4)
            {
                str_lcd("right");


                /* Run right indicator pattern */
                for(j = 0; j < 3; j++)
                {
                    for(i = 7; i >= 0; i--)
                    {
                        IOCLR0 = 1 << i;

                        delay_ms(100);
                    }
                }
            }


            /* Turn OFF all LEDs after indicator sequence */
            IOSET0 = 255 << 0;
        }


        /* ---------- CAN COMMUNICATION ERROR ---------- */

        else
        {
            cmd_lcd(0x94);

            str_lcd("DATA NOT RECVD");
        }


        /* ---------- BUZZER ALERT ---------- */

        /*
         * Distance greater than 20 cm:
         * Generate 5 buzzer pulses.
         */
        if(dis > 20)
        {
            blink_buzzer(5);
        }


        /*
         * Distance less than 20 cm or greater than 50 cm:
         * Generate 2 buzzer pulses.
         */
        else if(dis < 20 || dis > 50)
        {
            blink_buzzer(2);
        }


        /* Wait before next measurement */
        delay_ms(300);


        /* Clear LCD before next update */
        cmd_lcd(0x01);
    }
}