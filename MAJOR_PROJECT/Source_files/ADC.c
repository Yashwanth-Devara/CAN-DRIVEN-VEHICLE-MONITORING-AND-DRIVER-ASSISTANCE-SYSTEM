#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "can_adc_defines.h"

/*
 * Function: init_adc()
 * --------------------
 * Initializes the ADC peripheral of the LPC21xx microcontroller.
 */
void init_adc()
{
    /*
     * Configure the required ADC pin as an ADC input
     * using the PINSEL1 register.
     */
    PINSEL1 |= 0x00400000;

    /*
     * Configure ADC control register:
     * - Set ADC clock divider
     * - Enable the ADC power
     */
    ADCR = CLKDIV_VALUE | PDN_BIT;
}


/*
 * Function: read_adc()
 * --------------------
 * Reads the analog value from the selected ADC channel.
 *
 * Parameter:
 *     chno -> ADC channel number
 *
 * Returns:
 *     ADC input voltage in volts.
 */
f32 read_adc(u8 chno)
{
    f32 ear;
    u16 adcdval = 0;
    unsigned long timeout;

    /*
     * Clear the previously selected ADC channel.
     *
     * Lower 8 bits of ADCR contain the channel selection
     * and other ADC control bits.
     */
    ADCR &= ~0xFF;

    /*
     * Select the required ADC channel.
     */
    ADCR |= chno;

    /*
     * Start a new ADC conversion.
     */
    ADCR |= START_CONV;

    /*
     * Small delay to allow the ADC conversion process to start.
     */
    delay_us(3);

    /*
     * Initialize timeout counter.
     *
     * This prevents the processor from getting stuck forever
     * if the ADC conversion does not complete.
     */
    timeout = 100000;

    /*
     * Wait until the ADC conversion is completed.
     *
     * DONE_BIT becomes 1 when conversion is complete.
     */
    while (((ADDR >> DONE_BIT) & 1) == 0)
    {
        /*
         * Decrease timeout counter.
         * Return 0.0 V if timeout occurs.
         */
        if (--timeout == 0)
            return 0.0f;
    }

    /*
     * Extract the 10-bit ADC result from ADDR.
     *
     * ADC result range:
     *     0    -> minimum input
     *     1023 -> maximum input
     */
    adcdval = ((ADDR >> RESULT) & 1023);

    /*
     * Stop the ADC conversion.
     */
    ADCR &= ~(START_CONV);

    /*
     * Convert the 10-bit ADC value into voltage.
     *
     * ADC reference voltage = 3.3 V
     * ADC resolution = 10 bits = 1024 levels
     *
     * Voltage = (ADC_Value × 3.3) / 1023
     */
    ear = ((3.3 * adcdval) / 1023);

    /*
     * Return the calculated voltage.
     */
    return ear;
}