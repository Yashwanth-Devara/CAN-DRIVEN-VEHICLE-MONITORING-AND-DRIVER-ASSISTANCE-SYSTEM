// Function definitions for LCD
#include "lcd_defines.h"
#include "lcd.h"
#include "types.h"
#include "delay.h"
#include <lpc21xx.h>

/*
 * Function: write_lcd()
 * ---------------------
 * Sends one byte/command/data to the LCD.
 *
 * The LCD is connected in 8-bit mode.
 *
 * byte → data/command to be sent to LCD
 */
void write_lcd(u8 byte)
{
    /*
     * Clear the previous 8-bit LCD data from P0.8-P0.15
     * and place the new byte on the LCD data pins.
     *
     * lcd_data is the starting bit position of LCD data pins.
     */
    IOPIN0 = ((IOPIN0 & ~(255 << lcd_data)) |
              (byte << lcd_data));

    /*
     * RW = 0 → Write operation.
     */
    IOCLR0 = 1 << lcd_rw;

    /*
     * Generate Enable pulse.
     *
     * EN = 1 → LCD reads the data.
     */
    IOSET0 = 1 << lcd_en;

    /*
     * Small delay to keep Enable high for
     * the required amount of time.
     */
    delay_us(1);

    /*
     * End the Enable pulse.
     *
     * EN = 0 → LCD latches the data.
     */
    IOCLR0 = 1 << lcd_en;

    /*
     * Wait for LCD command/data processing.
     */
    delay_ms(2);
}


/*
 * Function: cmd_lcd()
 * -------------------
 * Sends a command to the LCD.
 *
 * Example:
 *     cmd_lcd(0x01);   // Clear LCD
 *     cmd_lcd(0xC0);   // Move cursor to second line
 */
void cmd_lcd(u8 byte)
{
    /*
     * RS = 0 → Command/Register Select.
     *
     * Therefore, the byte will be interpreted
     * as an LCD command.
     */
    IOCLR0 = 1 << lcd_rs;

    /*
     * Send the command byte to LCD.
     */
    write_lcd(byte);
}


/*
 * Function: Init_lcd()
 * --------------------
 * Initializes the LCD in 8-bit mode.
 */
void Init_lcd()
{
    /*
     * Configure LCD pins as outputs.
     *
     * P0.8-P0.15 → LCD data pins
     * lcd_rs     → Register Select
     * lcd_rw     → Read/Write
     * lcd_en     → Enable
     */
    IODIR0 |= 0xff << lcd_data |
              1 << lcd_rs |
              1 << lcd_rw |
              1 << lcd_en;

    /*
     * Wait after power-on.
     *
     * This gives the LCD enough time to
     * complete its internal power-on reset.
     */
    delay_ms(15);

    /*
     * LCD initialization sequence.
     *
     * Send Function Set initialization command.
     */
    cmd_lcd(0x30);

    delay_ms(4);

    delay_us(100);

    /*
     * Repeat initialization command as required
     * during LCD power-on initialization.
     */
    cmd_lcd(0x30);

    delay_us(100);

    cmd_lcd(0x30);

    /*
     * Function Set:
     *
     * 8-bit interface
     * 2-line display
     * 5x8 character font
     */
    cmd_lcd(0x38);

    /*
     * Display ON, cursor OFF.
     */
    cmd_lcd(0x0c);

    /*
     * Clear the LCD display.
     */
    cmd_lcd(0x01);

    /*
     * Entry mode:
     *
     * Cursor moves to the next position
     * after writing a character.
     */
    cmd_lcd(0x06);
}


/*
 * Function: char_lcd()
 * --------------------
 * Displays one character on the LCD.
 *
 * Example:
 *     char_lcd('A');
 */
void char_lcd(u8 ascii)
{
    /*
     * RS = 1 → Data mode.
     *
     * Therefore, the byte is treated as
     * display data rather than a command.
     */
    IOSET0 = 1 << lcd_rs;

    /*
     * Send ASCII character to LCD.
     */
    write_lcd(ascii);
}


/*
 * Function: str_lcd()
 * -------------------
 * Displays a string on the LCD.
 *
 * The string is processed character by character
 * until the NULL character '\0' is encountered.
 */
void str_lcd(s8 *str)
{
    /*
     * Continue until the end of the string.
     */
    while(*str)
    {
        /*
         * Display the current character
         * and increment the string pointer.
         */
        char_lcd(*str++);
    }
}


/*
 * Function: u32_lcd()
 * -------------------
 * Displays an unsigned 32-bit integer on the LCD.
 *
 * Since the number is stored as digits in reverse
 * order using division by 10, the digits are later
 * displayed from the last digit to the first.
 */
void u32_lcd(u32 num)
{
    /*
     * Array used to temporarily store
     * individual decimal digits.
     */
    u32 a[10];

    /*
     * Index for storing digits.
     */
    s32 i = 0;

    /*
     * Special case:
     * If number is zero, directly display '0'.
     */
    if(num == 0)
    {
        char_lcd('0');
    }
    else
    {
        /*
         * Extract each digit from right to left.
         *
         * num % 10 → last digit
         * num / 10 → remove last digit
         *
         * ASCII value of '0' is 48,
         * so 48 is added to convert digit to ASCII.
         */
        while(num > 0)
        {
            a[i++] = num % 10 + 48;
            num /= 10;
        }

        /*
         * Digits were stored in reverse order.
         *
         * Display them from the last stored digit
         * to the first.
         */
        for(--i; i >= 0; i--)
        {
            char_lcd(a[i]);
        }
    }
}


/*
 * Function: s32_lcd()
 * -------------------
 * Displays a signed 32-bit integer.
 */
void s32_lcd(s32 num)
{
    /*
     * If number is negative, display '-'
     * and convert the number to positive.
     */
    if(num < 0)
    {
        char_lcd('-');
        num = -num;
    }

    /*
     * Display the absolute value.
     */
    u32_lcd(num);
}


/*
 * Function: f32_lcd()
 * -------------------
 * Displays a floating-point number.
 *
 * fnum → floating-point value
 * ndp  → number of decimal places to display
 */
void f32_lcd(f32 fnum, u32 ndp)
{
    u32 num;
    s32 i = 0;

    /*
     * Check whether the number is negative.
     *
     * NOTE:
     * Your original code checks 'num < 0.0'.
     * Since 'num' is an integer variable and is not
     * initialized at this point, this condition is
     * not correct for checking fnum.
     *
     * The intended condition is:
     *
     * if(fnum < 0.0)
     */
    if(num < 0.0)
    {
        /*
         * Display negative sign.
         */
        char_lcd('-');

        /*
         * Convert floating-point value to positive.
         */
        fnum = -fnum;
    }

    /*
     * Extract integer part of floating-point value.
     */
    num = fnum;

    /*
     * Display integer part.
     */
    u32_lcd(num);

    /*
     * Display decimal point.
     */
    char_lcd('.');

    /*
     * Generate the required number of
     * decimal places.
     */
    for(i = 0; i < ndp; i++)
    {
        /*
         * Remove integer part and multiply
         * fractional part by 10.
         *
         * Example:
         * 12.34
         *
         * fnum - num = 0.34
         * ×10 = 3.4
         */
        fnum = (fnum - num) * 10;

        /*
         * Extract the next integer digit.
         */
        num = fnum;

        /*
         * Convert digit to ASCII and display it.
         */
        char_lcd(num + 48);
    }
}


/*
 * Function: buildcgram()
 * ----------------------
 * Creates custom characters in LCD CGRAM.
 *
 * p      → pointer to character pattern
 * nbytes → number of bytes in pattern
 */
void buildcgram(s8 *p, u32 nbytes)
{
    u32 i;

    /*
     * Move LCD address pointer to CGRAM.
     *
     * Custom characters are stored in CGRAM.
     */
    cmd_lcd(goto_cgram_start);

    /*
     * RS = 1 → Data mode.
     *
     * Custom character pattern bytes are
     * written as data.
     */
    IOSET0 = 1 << lcd_rs;

    /*
     * RW = 0 → Write operation.
     */
    IOCLR0 = 1 << lcd_rw;

    /*
     * Write each byte of the custom character
     * pattern into CGRAM.
     */
    for(i = 0; i < nbytes; i++)
    {
        write_lcd(p[i]);
    }

    /*
     * Return LCD cursor to the beginning
     * of the first display line.
     */
    cmd_lcd(goto_line1_pos0);
}


/*
 * Function: fuel_indication()
 * ---------------------------
 * Stores a custom fuel-level character pattern
 * into LCD CGRAM.
 *
 * loc     → custom character location
 * pattern → pointer to 8-byte character pattern
 */
void fuel_indication(u8 loc, u8 *pattern)
{
    u32 i;

    /*
     * Calculate CGRAM address.
     *
     * Each custom character occupies 8 bytes.
     *
     * loc * 8 → starting address of character.
     */
    cmd_lcd(0x40 + (loc * 8));

    /*
     * RS = 1 → Data mode.
     */
    for(i = 0; i < 8; i++)
    {
        /*
         * Send each byte of the custom
         * character pattern to LCD.
         */
        char_lcd(pattern[i]);
    }

    /*
     * Return cursor to the beginning of
     * the first LCD line.
     */
    cmd_lcd(0x80);
}