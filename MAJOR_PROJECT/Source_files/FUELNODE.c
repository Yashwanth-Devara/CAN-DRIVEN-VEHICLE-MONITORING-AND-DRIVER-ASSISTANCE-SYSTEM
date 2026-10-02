#include "types.h"
#include "delay.h"
#include "can.h"
#include "can_adc_defines.h"
#include "can_adc.h"
#include "lcd.h"

/*
 * Global variable to store the ADC voltage value.
 *
 * read_adc() returns the converted analog voltage
 * as a floating-point value.
 */
f32 t;

/*
 * Global variable to store fuel percentage.
 *
 * Value will be calculated from ADC voltage
 * and limited between 0 and 100%.
 */
u32 per;


int main()
{
    /*
     * CAN frame structure.
     *
     * txf will contain:
     * - CAN ID
     * - RTR
     * - DLC
     * - CAN data
     */
    canf txf;

    /*
     * Initialize CAN1 peripheral.
     */
    init_can1();

    /*
     * Initialize ADC peripheral.
     */
    init_adc();

    /*
     * Initialize LCD.
     */
    Init_lcd();

    /*
     * Display project/module name on LCD.
     */
    str_lcd("can adc tx");


    /*
     * Continuously read ADC,
     * calculate fuel percentage,
     * display it on LCD,
     * and transmit it through CAN.
     */
    while(1)
    {
        /*
         * Read analog voltage from ADC Channel 0.
         *
         * read_adc(CH0) returns a voltage between
         * approximately 0V and 3.3V.
         */
        t = read_adc(CH0);


        /*
         * Convert ADC voltage into percentage.
         *
         * Formula:
         *
         *       Voltage
         * Percentage = ----------- × 100
         *          3.3
         *
         * Example:
         * 3.3V → 100%
         * 1.65V → 50%
         * 0V → 0%
         */
        per = (u32)((t / 3.3f) * 100.0f);


        /*
         * Safety limit.
         *
         * If the calculated percentage is greater
         * than 100, force it back to 100%.
         */
        if(per > 100)
            per = 100;


        /*
         * Move LCD cursor to second line.
         *
         * 0xC0 is the starting address of
         * the second line of a standard 16x2 LCD.
         */
        cmd_lcd(0xc0);


        /*
         * Display fuel percentage value.
         */
        u32_lcd(per);


        /*
         * Display percentage symbol.
         *
         * Extra spaces are used to clear
         * characters remaining from a previous value.
         */
        str_lcd("%   ");


        /*
         * Configure CAN frame.
         *
         * CAN ID = 1
         *
         * In this project, CAN ID 1 is used
         * to transmit fuel percentage information
         * from the Fuel Node to the Main Node.
         */
        txf.id = 1;


        /*
         * RTR = 0 means this is a DATA frame.
         *
         * RTR = 1 would indicate a Remote frame.
         */
        txf.bfv.rtr = 0;


        /*
         * Set Data Length Code to 4.
         *
         * The CAN frame is configured to contain
         * 4 bytes of data.
         */
        txf.bfv.dlc = 4;


        /*
         * Store the calculated fuel percentage
         * in the first CAN data register.
         */
        txf.data1 = per;


        /*
         * No additional data is used in data2.
         */
        txf.data2 = 0;


        /*
         * Transmit the CAN frame.
         *
         * The CAN driver loads the ID, DLC and data
         * into the CAN controller registers and
         * starts transmission.
         */
        can1_tx1(txf);


        /*
         * Wait for 1 second before taking the next
         * ADC reading and transmitting the next
         * fuel percentage.
         */
        delay_ms(1000);
    }
}