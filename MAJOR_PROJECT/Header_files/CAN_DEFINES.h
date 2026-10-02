#include "types.h"


/* ---------- CAN PIN CONFIGURATION ---------- */

/* CAN1 Receive pin configuration */
#define rd1_pin_25 fun2


/* ---------- CAN CLOCK CONFIGURATION ---------- */

/* Crystal oscillator frequency = 12 MHz */
#define fosc 12000000

/* CPU clock = 5 × oscillator frequency = 60 MHz */
#define cclk (fosc * 5)

/* Peripheral clock = CCLK / 4 = 15 MHz */
#define pclk (cclk / 4)


/* ---------- CAN BIT RATE CONFIGURATION ---------- */

/* CAN communication speed = 125 kbps */
#define bitrate 125000

/* Number of time quanta per CAN bit */
#define quanta 15

/* Baud-rate prescaler */
#define brp (pclk / (bitrate * quanta))

/* CAN sample point = 70% */
#define sample_point (0.7 * quanta)

/* Time segment 1 */
#define tseg1 ((u32)sample_point - 1)

/* Time segment 2 */
#define tseg2 (quanta - (1 + tseg1))

/* Synchronization Jump Width */
#define sjw ((tseg2 >= 5) ? 4 : (tseg2 - 1))


/* ---------- CAN BIT TIMING REGISTER ---------- */

/*
 * Configure C1BTR:
 * SAM   → Sampling mode
 * TSEG2 → Time Segment 2
 * TSEG1 → Time Segment 1
 * SJW   → Synchronization Jump Width
 * BRP   → Baud Rate Prescaler
 */
#define sam 0

#define btr_val ((sam << 23) | \
                 ((tseg2 - 1) << 20) | \
                 ((tseg1 - 1) << 16) | \
                 ((sjw - 1) << 14) | \
                 (brp - 1))


/* ---------- CAN COMMAND REGISTER (C1CMR) ---------- */

/* Transmission request */
#define tr_bit 0

/* Release Receive Buffer */
#define rrb_bit 2

/* Select Transmit Buffer 1 */
#define stb1_bit 5


/* ---------- CAN GLOBAL STATUS REGISTER (C1GSR) ---------- */

/* Receive Buffer Status */
#define rbs_bit 0

/* Transmit Buffer Status */
#define tbs_bit 2

/* Transmission Complete Status */
#define tcs_bit 3


/* ---------- CAN MODE REGISTER (C1MOD) ---------- */

/* Reset Mode */
#define rm_bit 0


/* ---------- CAN FRAME INFORMATION ---------- */

/* Remote Transmission Request bit */
#define rtr_bit 30

/* Data Length Code bits */
#define dlc_bit 16