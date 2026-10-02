#include "types.h"


/* ---------- ULTRASONIC INITIALIZATION ---------- */

/* Initialize ultrasonic sensor pins */
void init_sonic(void);


/* ---------- TRIGGER PULSE ---------- */

/* Generate trigger pulse for ultrasonic sensor */
void send_pulse(void);


/* ---------- ECHO PULSE ---------- */

/* Read echo pulse duration using Timer0 */
u32 read_pulse(void);


/* ---------- DISTANCE CALCULATION ---------- */

/* Calculate distance from echo pulse duration */
u32 dis_cal(void);