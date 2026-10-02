/* ---------- CLOCK CONFIGURATION ---------- */

/* Crystal oscillator frequency = 12 MHz */
#define FOSC 12000000

/* CPU clock = 5 × oscillator frequency = 60 MHz */
#define CCLK (5 * FOSC)

/* Peripheral clock = CCLK / 4 = 15 MHz */
#define PCLK (CCLK / 4)


/* ---------- ADC CLOCK CONFIGURATION ---------- */

/* Required ADC clock = 3.75 MHz */
#define ADCLK (3750000)

/* Calculate ADC clock divider */
#define DIVIDER ((PCLK / ADCLK) - 1)

/* Shift divider value to ADC clock-divider field */
#define CLKDIV_VALUE (DIVIDER << 8)


/* ---------- ADC CONTROL BITS ---------- */

/* ADC power-down bit */
#define PDN_BIT (1 << 21)

/* Start ADC conversion */
#define START_CONV (1 << 24)


/* ---------- ADC RESULT CONFIGURATION ---------- */

/* ADC result starts from bit 6 */
#define RESULT 6

/* ADC conversion DONE status bit */
#define DONE_BIT 31


/* ---------- ADC CHANNEL SELECTION ---------- */

/* ADC Channel 0 */
#define CH0 (1 << 0)

/* ADC Channel 1 */
#define CH1 (1 << 1)

/* ADC Channel 2 */
#define CH2 (1 << 2)

/* ADC Channel 3 */
#define CH3 (1 << 3)