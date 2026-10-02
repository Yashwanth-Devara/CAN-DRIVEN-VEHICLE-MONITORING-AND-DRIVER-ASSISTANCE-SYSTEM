#include <lpc21xx.h>
#include "can.h"
#include "delay.h"
#include "types.h"
#include "lcd.h"
#include "can_ds18b20.h"
#include "ultra_sonic.h"


/* ---------- CAN DATA STATUS ---------- */

/* Check whether CAN receive buffer contains data */
u8 can_data_available(void)
{
    return ((C1GSR >> 0) & 1);
}


/* ---------- GLOBAL VARIABLES ---------- */

/* CAN receive and transmit frames */
canf rxf, txf;

/* Vehicle monitoring data */
u32 fuel, temp, tpd, dist;

/* Indicator status:
 * 0 = OFF
 * 1 = LEFT
 * 2 = RIGHT
 */
u8 indicator = 0;

/* Temporary temperature variable */
int tp;

/* Vehicle direction:
 * 1 = Forward
 * 0 = Reverse
 */
volatile u8 dir = 1;


/* ---------- LCD CUSTOM CHARACTERS ---------- */

/* Left indicator symbol */
u8 left_lut[8] =
{
    0x02,0x06,0x0e,0x1e,
    0x0e,0x06,0x02,0x00
};

/* Right indicator symbol */
u8 right_lut[8] =
{
    0x08,0x0c,0x0e,0x0f,
    0x0e,0x0c,0x08,0x00
};

/* Fuel empty symbol */
u8 fuel_empty[8] =
{
    0x0E,0x11,0x11,0x11,
    0x11,0x11,0x1F
};

/* Fuel low symbol */
u8 fuel_low[8] =
{
    0x0E,0x11,0x11,0x11,
    0x1F,0x1F,0x1F
};

/* Fuel medium symbol */
u8 fuel_mid[8] =
{
    0x0E,0x11,0x1F,0x1F,
    0x1F,0x1F,0x1F
};

/* Fuel full symbol */
u8 fuel_full[8] =
{
    0x0E,0x1F,0x1F,0x1F,
    0x1F,0x1F,0x1F
};


/* ---------- BUILD LCD CUSTOM CHARACTERS ---------- */

/* Store indicator and fuel icons in LCD CGRAM */
void build_cgram(void)
{
    u8 i;

    /* Store left indicator */
    cmd_lcd(0x40);

    for(i=0; i<8; i++)
        char_lcd(left_lut[i]);


    /* Store right indicator */
    cmd_lcd(0x48);

    for(i=0; i<8; i++)
        char_lcd(right_lut[i]);


    /* Store empty fuel icon */
    cmd_lcd(0x50);

    for(i=0; i<8; i++)
        char_lcd(fuel_empty[i]);


    /* Store low fuel icon */
    cmd_lcd(0x58);

    for(i=0; i<8; i++)
        char_lcd(fuel_low[i]);


    /* Store medium fuel icon */
    cmd_lcd(0x60);

    for(i=0; i<8; i++)
        char_lcd(fuel_mid[i]);


    /* Store full fuel icon */
    cmd_lcd(0x68);

    for(i=0; i<8; i++)
        char_lcd(fuel_full[i]);
}


/* ---------- FUEL DISPLAY ---------- */

/* Display fuel percentage and fuel-level icon */
void display_fuel(u32 percent)
{
    u8 icon;

    /* Select fuel icon according to fuel percentage */
    if(percent < 25)
        icon = 2;

    else if(percent < 50)
        icon = 3;

    else if(percent < 75)
        icon = 4;

    else
        icon = 5;


    /* Display fuel percentage */
    cmd_lcd(0xc0);

    str_lcd("FUEL: ");

    u32_lcd(percent);

    str_lcd("%    ");


    /* Display fuel icon */
    char_lcd(icon);

    str_lcd("   ");
}


/* =========================================================
 *                  EINT0 - DIRECTION
 * ========================================================= */

/* EINT0 ISR: Toggle Forward/Reverse mode */
void EINT0_isr() __irq
{
    static unsigned long last_tick = 0;
    unsigned long now = T0TC;


    /* Check input pin */
    if(((IOPIN0 >> 1) & 1) == 1)
    {
        /* Switch debounce */
        if((now - last_tick) > 50000)
        {
            /* Toggle direction */
            dir ^= 1;

            last_tick = now;
        }
    }


    /* Clear external interrupt */
    EXTINT |= 1 << 0;

    /* End interrupt */
    VICVectAddr = 0;
}


/* Configure EINT0 */
void EINT0(void)
{
    /* Configure P0.1 as EINT0 */
    PINSEL0 &= ~(3 << 2);

    PINSEL0 |= 0x0000000C;


    /* Configure edge-triggered interrupt */
    EXTMODE |= 1 << 0;

    EXTPOLAR |= 1 << 0;


    /* Clear pending interrupt */
    EXTINT |= 1 << 0;


    /* Configure VIC */
    VICVectAddr0 = (u32)EINT0_isr;

    VICVectCntl0 = (1 << 5) | 14;


    /* Configure as IRQ */
    VICIntSelect &= ~(1 << 14);


    /* Enable EINT0 */
    VICIntEnable |= (1 << 14);
}


/* =========================================================
 *                  EINT1 - LEFT INDICATOR
 * ========================================================= */

/* EINT1 ISR: Toggle left indicator */
void EINT1_isr() __irq
{
    static unsigned long last_tick = 0;
    unsigned long now = T0TC;


    /* Switch debounce */
    if((now - last_tick) > 50000)
    {
        /* Indicator works in Forward mode */
        if(dir == 1)
        {
            /* Toggle left indicator */
            indicator = (indicator == 1) ? 0 : 1;


            /* Send left indicator command through CAN */
            txf.id = 3;

            txf.bfv.dlc = 4;

            txf.bfv.rtr = 0;

            txf.data1 = 1;


            can1_tx1(txf);
        }

        last_tick = now;
    }


    /* Clear interrupt */
    EXTINT |= 1 << 1;

    /* End interrupt */
    VICVectAddr = 0;
}


/* Configure EINT1 */
void EINT1(void)
{
    /* Configure P0.3 as EINT1 */
    PINSEL0 &= ~(3 << 6);

    PINSEL0 |= 0x000000C0;


    /* Configure edge-triggered interrupt */
    EXTMODE |= 1 << 1;

    EXTPOLAR |= 1 << 1;


    /* Clear pending interrupt */
    EXTINT |= 1 << 1;


    /* Configure VIC */
    VICVectAddr1 = (u32)EINT1_isr;

    VICVectCntl1 = (1 << 5) | 15;


    /* Configure as IRQ */
    VICIntSelect &= ~(1 << 15);


    /* Enable EINT1 */
    VICIntEnable |= (1 << 15);
}


/* =========================================================
 *                  EINT2 - RIGHT INDICATOR
 * ========================================================= */

/* EINT2 ISR: Toggle right indicator */
void EINT2_isr() __irq
{
    static unsigned long last_tick = 0;
    unsigned long now = T0TC;


    /* Switch debounce */
    if((now - last_tick) > 50000)
    {
        /* Indicator works in Forward mode */
        if(dir == 1)
        {
            /* Toggle right indicator */
            indicator = (indicator == 2) ? 0 : 2;


            /* Send right indicator command through CAN */
            txf.id = 4;

            txf.bfv.dlc = 4;

            txf.bfv.rtr = 0;

            txf.data1 = 1;


            can1_tx1(txf);
        }

        last_tick = now;
    }


    /* Clear interrupt */
    EXTINT |= 1 << 2;

    /* End interrupt */
    VICVectAddr = 0;
}


/* Configure EINT2 */
void EINT2(void)
{
    /* Configure P0.7 as EINT2 */
    PINSEL0 &= ~(3 << 14);

    PINSEL0 |= 0x0000C000;


    /* Configure edge-triggered interrupt */
    EXTMODE |= 1 << 2;

    EXTPOLAR |= 1 << 2;


    /* Clear pending interrupt */
    EXTINT |= 1 << 2;


    /* Configure VIC */
    VICVectAddr2 = (u32)EINT2_isr;

    VICVectCntl2 = (1 << 5) | 16;


    /* Configure as IRQ */
    VICIntSelect &= ~(1 << 16);


    /* Enable EINT2 */
    VICIntEnable |= (1 << 16);
}


/* =========================================================
 *                         MAIN
 * ========================================================= */

int main()
{
    /* ---------- INITIALIZATION ---------- */

    /* Initialize CAN1 */
    init_can1();

    /* Initialize LCD */
    Init_lcd();

    /* Display ON, Cursor OFF */
    cmd_lcd(0x0C);

    /* Build LCD custom characters */
    build_cgram();


    /* ---------- STARTUP SCREEN ---------- */

    cmd_lcd(0x80);

    str_lcd("     CAN-DRIVEN     ");


    cmd_lcd(0xc0);

    str_lcd(" VECHILE MONITORING ");


    cmd_lcd(0x94);

    str_lcd("  DRIVER ASSISTANCE ");


    cmd_lcd(0xd4);

    str_lcd("       SYSTEM       ");


    /* Display startup screen for 1 second */
    delay_ms(1000);

    /* Clear LCD */
    cmd_lcd(0x01);


    /* ---------- TIMER0 ---------- */

    /* Configure Timer0 prescaler */
    T0PR = 14;

    /* Start Timer0 */
    T0TCR = 0x01;


    /* ---------- EXTERNAL INTERRUPTS ---------- */

    /* Configure direction switch */
    EINT0();

    /* Configure left indicator switch */
    EINT1();

    /* Configure right indicator switch */
    EINT2();


    /* ---------- TEMPERATURE SENSOR ---------- */

    /* Initialize DS18B20 */
    init_ds18b20();


    /* =====================================================
     *                     MAIN LOOP
     * ===================================================== */

    while(1)
    {
        /* ---------- CAN DATA RECEPTION ---------- */

        if(can_data_available())
        {
            /* Receive CAN frame */
            can1_rx1(&rxf);


            /* CAN ID 2 = Ultrasonic distance */
            if(rxf.id == 2)
            {
                dist = rxf.data1;
            }


            /* CAN ID 1 = Fuel percentage */
            if(rxf.id == 1)
            {
                fuel = rxf.data1;


                /* Check fuel node communication */
                if(fuel == 0)
                {
                    cmd_lcd(0xc0);

                    str_lcd("FUELNODE PROBLEM");
                }
            }
        }


        /* =================================================
         *                   FORWARD MODE
         * ================================================= */

        if(dir == 1)
        {
            /* Display Forward Mode */
            cmd_lcd(0x80);

            str_lcd("FORWARD MODE:");


            /* ---------- INDICATOR DISPLAY ---------- */

            cmd_lcd(0x8c);


            /* Left indicator */
            if(indicator == 1)
            {
                char_lcd(0);

                str_lcd("    ");
            }


            /* Right indicator */
            else if(indicator == 2)
            {
                char_lcd(1);

                str_lcd("    ");
            }


            /* No indicator */
            else
            {
                str_lcd("     ");
            }


            /* ---------- FUEL DISPLAY ---------- */

            if(fuel)
            {
                display_fuel(fuel);
            }


            /* ---------- TEMPERATURE ---------- */

            /* Check DS18B20 sensor */
            if(ResetDS18b20() == 0)
            {
                /* Read temperature */
                temp = read_temp();


                /* Extract integer temperature */
                tp = (temp >> 4);


                /* Extract decimal digit */
                tpd = temp & 0x08 ? 0x35 : 0x30;


                /* Display temperature */
                cmd_lcd(0x94);

                str_lcd("TEMP: ");


                /* Handle negative temperature */
                if(tp < 0)
                {
                    char_lcd('-');

                    tp = -tp;
                }


                /* Display integer part */
                u32_lcd(tp);

                char_lcd('.');


                /* Display decimal part */
                char_lcd(tpd);


                /* Degree symbol */
                char_lcd(0xdf);


                /* Celsius */
                str_lcd("C        ");
            }


            /* Sensor disconnected */
            else
            {
                cmd_lcd(0x94);

                str_lcd("NO SENSOR CONNECTED");
            }
        }


        /* =================================================
         *                   REVERSE MODE
         * ================================================= */

        else
        {
            /* Display Reverse Mode */
            cmd_lcd(0x80);

            str_lcd("REVERSE MODE");

            str_lcd("        ");


            /* ---------- DISTANCE DISPLAY ---------- */

            cmd_lcd(0xc0);


            /* Ultrasonic node communication problem */
            if(dist == 0)
            {
                cmd_lcd(0xc0);

                str_lcd("ULTRSONICNODE PROBLEM");
            }


            /* Display distance */
            else
            {
                cmd_lcd(0xc0);

                str_lcd("DIST: ");

                u32_lcd(dist);

                str_lcd("CM             ");
            }


            /* ---------- DRIVER ASSISTANCE ---------- */

            /* Distance > 50 cm → SAFE */
            if(dist > 50)
            {
                cmd_lcd(0x94);

                str_lcd("SAFE         ");
            }


            /* Distance 21–50 cm → WARNING */
            else if(dist > 20)
            {
                cmd_lcd(0x94);

                str_lcd("WARNING          ");
            }


            /* Distance <= 20 cm → STOP */
            else
            {
                cmd_lcd(0x94);

                str_lcd("STOP             ");
            }


            /* Clear fourth LCD line */
            cmd_lcd(0xd4);

            str_lcd("                    ");
        }
    }
}