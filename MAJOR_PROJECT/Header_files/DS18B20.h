#include "types.h"


/* ---------- DS18B20 INITIALIZATION ---------- */

/* Initialize DS18B20 data pin */
void init_ds18b20(void);


/* ---------- 1-WIRE RESET ---------- */

/* Generate reset pulse and detect sensor presence */
u8 ResetDS18b20(void);


/* ---------- 1-WIRE BIT OPERATIONS ---------- */

/* Read one bit from DS18B20 */
u8 ReadBit(void);


/* Write one bit to DS18B20 */
void WriteBit(u8);


/* ---------- 1-WIRE BYTE OPERATIONS ---------- */

/* Read one byte from DS18B20 */
u8 ReadByte(void);


/* Write one byte to DS18B20 */
void WriteByte(u8);


/* ---------- TEMPERATURE READING ---------- */

/* Read temperature from DS18B20 */
int read_temp(void);