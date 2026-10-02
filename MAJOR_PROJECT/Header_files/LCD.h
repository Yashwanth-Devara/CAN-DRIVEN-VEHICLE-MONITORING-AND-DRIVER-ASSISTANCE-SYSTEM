#include "lcd_defines.h"
#include "types.h"


/* ---------- LCD LOW-LEVEL FUNCTIONS ---------- */

/* Write one byte to LCD */
void write_lcd(u8);


/* Send command to LCD */
void cmd_lcd(u8);


/* Initialize LCD */
void Init_lcd(void);


/* ---------- LCD DATA FUNCTIONS ---------- */

/* Display one character */
void char_lcd(u8);


/* Display a string */
void str_lcd(s8 *);


/* Display unsigned 32-bit integer */
void u32_lcd(u32);


/* Display signed 32-bit integer */
void s32_lcd(s32);


/* Display floating-point number */
void f32_lcd(f32, u32);


/* ---------- LCD CUSTOM CHARACTER FUNCTIONS ---------- */

/* Build custom characters in LCD CGRAM */
void buildcgram(s8 *, u32);


/* Create/store fuel-level custom character */
void fuel_indication(u8 loc, u8 *pattern);