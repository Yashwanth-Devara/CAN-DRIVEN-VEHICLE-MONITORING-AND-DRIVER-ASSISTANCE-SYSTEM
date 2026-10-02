#include "types.h"


/* ---------- CAN FRAME STRUCTURE ---------- */

/*
 * CAN frame structure used for
 * transmission and reception.
 */
typedef struct can_frame
{
    /* CAN message identifier */
    u32 id;


    /* CAN frame control information */
    struct bitfield
    {
        /* Remote Transmission Request bit
         * 0 = Data frame
         * 1 = Remote frame
         */
        u32 rtr : 1;

        /* Data Length Code
         * Specifies the number of data bytes.
         */
        u32 dlc : 4;

    } bfv;


    /* CAN data registers
     * data1 = first 4 bytes
     * data2 = next 4 bytes
     */
    u32 data1, data2;

} canf;


/* ---------- CAN FUNCTION PROTOTYPES ---------- */

/* Initialize CAN1 peripheral */
void init_can1(void);


/* Transmit a CAN frame */
void can1_tx1(canf txf);


/* Receive a CAN frame */
void can1_rx1(canf *rxf);